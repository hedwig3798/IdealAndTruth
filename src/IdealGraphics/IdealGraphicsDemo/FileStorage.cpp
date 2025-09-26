#include "FileStorage.h"
#include "stringUtil.h"
#include "SecureCodec.h"

#include <iostream>

bool FileStorage::JobInfo::m_isSuccess = true;

FileStorage::FileStorage()
	: m_compressInfoMap{}
	, m_comExtension(L"")
	, m_compressPath(L"")
	, m_decompressPath(L"")
	, m_blockSize(0)
	, m_maxPartSize(0)
	, m_threadCount(0)
	, m_currentPartFileSize(0)
	, m_partFileIndex(0)
	, m_chunkSize(0)
{
}

FileStorage::~FileStorage()
{
}


bool FileStorage::CreateDirectory(const std::wstring& _path)
{
	bool result = ::CreateDirectory(_path.c_str(), nullptr);
	// 디렉토리 생성 실패
	if (false == result)
	{
		// 단 이미 있어서 실패한 경우는 성공과 같다.
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool FileStorage::CompressDirectory(
	const std::wstring& _path
)
{
	// 입력받은 디렉토리 열기
	WIN32_FIND_DATAW fd;
	HANDLE hFind = FindFirstFileW((_path + L"\\*").c_str(), &fd);

	// 실패
	if (INVALID_HANDLE_VALUE == hFind)
	{
		return false;
	}

	// 디렉토리 순회
	do
	{
		std::wstring name = fd.cFileName;

		// 특수 디렉토리 제외
		if (name == L"." || name == L"..")
		{
			continue;
		}

		// 파일의 전체 경로
		std::wstring fullPath = _path + L"\\" + name;

		// 디렉토리라면 재귀
		if (FILE_ATTRIBUTE_DIRECTORY & fd.dwFileAttributes)
		{
			CompressDirectory(fullPath);
		}
		// 파일만 압축
		else
		{
			// 파일 스트림
			std::ifstream oriFile(fullPath, std::ios::binary);

			// 파일 열기 실패
			if (false == oriFile.is_open())
			{
				return false;
			}

			// 압축 데이터
			std::shared_ptr<std::mutex> comInfoMutex = std::make_shared<std::mutex>();

			// 모든 파일의 인덱스를 위한 기록
			// 근데 이거 필요한건가? 흠...
			// 스레드에서 참조하기 위해 일단 필요하다
			m_compressInfoMap[name] = CompressInfo();

			// 파일 크기 구하기
			oriFile.seekg(0, std::ios::end);
			uint64_t fileSize = oriFile.tellg();
			oriFile.seekg(0, std::ios::beg);

			m_compressInfoMap[name].m_totalOriginalSize = fileSize;

			// 파일 분할 시작
			uint64_t remaining = fileSize;
			while (remaining > 0)
			{
				// 원본 파일 데이터 버퍼
				std::vector<unsigned char> oriBuffer(m_blockSize);

				// 읽을 크기 구하기 ( 남아있는 크기와, 한 블록의 크기 중 작은것 )
				uint64_t toRead = std::min<uint64_t>(m_blockSize, remaining);

				// 파일 읽기
				oriFile.read(reinterpret_cast<char*>(oriBuffer.data()), toRead);
				if (1 >= m_threadCount)
				{
					// 병렬 처리 없는 압축
					bool isCompressSuccess = CompressWithNonThread(
						m_compressInfoMap[name]
						, oriBuffer
						, toRead
					);
					if (false == isCompressSuccess)
					{
						return false;
					}

				}
				else
				{
					// 새로운 압축 데이터를 큐에 넣음
					JobInfo job(m_compressInfoMap[name]);
					job.m_oriBuffer = std::move(oriBuffer);
					job.m_dataSize = toRead;

					std::lock_guard<std::mutex> lock(m_jobMutex);
					m_jobQ.push(std::move(job));
					m_jobCv.notify_one();
				}
				// 남은 파일 크기
				remaining -= toRead;
			}


		}
	} while (FindNextFileW(hFind, &fd));

	FindClose(hFind);
	return true;
}


void FileStorage::CompressWithThread()
{
	while (true)
	{
		std::unique_lock<std::mutex> lock(m_jobMutex);

		// 모든 일이 끝나거나, 큐가 비어있지 않다면 아래로 진행
		m_jobCv.wait(lock, [this]()
			{
				return true == m_alljobFinish || false == m_jobQ.empty();
			}
		);

		// 모든 일이 끝났거나 큐가 비어있으면 무시한다.
		if (true == m_alljobFinish
			&& true == m_jobQ.empty())
		{
			break;
		}

		JobInfo job = std::move(m_jobQ.front());
		m_jobQ.pop();
		lock.unlock();

		// 여기서 압축 해제 시작
		// 압축 데이터 버퍼
		std::vector<unsigned char> outBuffer(::LZ4_compressBound(static_cast<int>(m_blockSize)));

		// 읽은 부분 압축
		int compressedSize =
			::LZ4_compress_HC(
				reinterpret_cast<const char*>(job.m_oriBuffer.data())
				, reinterpret_cast<char*>(outBuffer.data())
				, static_cast<int>(job.m_dataSize)
				, static_cast<int>(outBuffer.size())
				, 9
			);

		// 압축된 크기가 0 이하면 문제가 있다.
		if (compressedSize <= 0)
		{
			return;
		}

		outBuffer.resize(compressedSize);

		BlockInfo bInfo;

		// 암호화를 위한 데이터 생성
		::RAND_bytes(bInfo.m_key, sizeof(bInfo.m_key));
		::RAND_bytes(bInfo.m_iv, sizeof(bInfo.m_iv));

		std::vector<unsigned char> encrypted;
		if (false == AES::CryptCTR(outBuffer, encrypted, bInfo.m_key, bInfo.m_iv))
		{
			return;
		}

		// 블록 메타데이터 작성
		bInfo.m_compressedSize = compressedSize;
		bInfo.m_originalSize = job.m_dataSize;

		// 락이 두개가 필요한가?
		std::unique_lock<std::mutex> chunkLock(m_chunkMutex);

		// 압축 정보에 블록 정보를 넣는다
		job.m_comInfo.m_blocks.push_back(bInfo);
		m_currentChunkSize += compressedSize;

		m_chunk.push_back(
			{
				&job.m_comInfo.m_blocks.back()
				, std::move(encrypted)
			}
		);

		m_chunkCv.notify_one();

		chunkLock.unlock();
	}
}

void FileStorage::WriteChunkToFile()
{
	while (true)
	{
		std::unique_lock<std::mutex> lock(m_chunkMutex);

		// 모든 일이 끝나거나, 데이터가 충분히 모였다면
		m_chunkCv.wait(lock, [this]()
			{
				return true == m_chunkFinish || m_chunkSize <= m_currentChunkSize;
			}
		);

		// 작업이 완료되고 남은 데이터가 없다면
		if (true == m_chunkFinish
			&& 0 == m_currentChunkSize)
		{
			break;
		}

		// 출력 파일이 없으면 무시한다.
		if (false == m_currentPartFileStream.is_open())
		{
			break;
		}

		for (auto& chunk : m_chunk)
		{
			// 현재 파트 파일이 꽉 찼으면 새로 생성
			if (m_currentPartFileSize + chunk.second.size() + sizeof(uint64_t) > m_maxPartSize)
			{
				// 현재 파트 파일을 닫는다
				if (m_currentPartFileStream.is_open())
				{
					m_currentPartFileStream.close();
				}

				// 새 파트 파일을 생성하고 연다
				std::wstring currentPartPath =
					m_compressPath + L"\\part_"
					+ std::to_wstring(m_partFileIndex++)
					+ m_comExtension;

				m_currentPartFileStream.open(currentPartPath, std::ios::binary);
				m_currentPartFileSize = 0;
			}

			// 압축 데이터 기록
			uint64_t compSize32 = chunk.first->m_compressedSize;
			m_currentPartFileStream.write(reinterpret_cast<char*>(&compSize32), sizeof(compSize32));

			// 압축 및 암호화 된 데이터 기록
			uint64_t offset = m_currentPartFileStream.tellp();
			m_currentPartFileStream.write(reinterpret_cast<char*>(chunk.second.data()), chunk.first->m_compressedSize);

			// 블록 메타데이터 작성
			chunk.first->m_partIndex = m_partFileIndex - 1;
			chunk.first->m_offset = offset;

			// 현재 파트 크기 증가
			m_currentPartFileSize += static_cast<uint64_t>(sizeof(uint64_t)) + chunk.first->m_compressedSize;
		}

		// 청크 초기화
		m_chunk.clear();
		m_currentChunkSize = 0;
	}


	return;
}

bool FileStorage::CompressWithNonThread(
	OUT CompressInfo& _comInfo
	, const std::vector<unsigned char>& _oriBuffer
	, size_t _dataSize
)
{
	// 압축 데이터 버퍼
	std::vector<unsigned char> outBuffer(::LZ4_compressBound(static_cast<int>(m_blockSize)));

	// 읽은 부분 압축
	int compressedSize =
		::LZ4_compress_HC(
			reinterpret_cast<const char*>(_oriBuffer.data())
			, reinterpret_cast<char*>(outBuffer.data())
			, static_cast<int>(_dataSize)
			, static_cast<int>(outBuffer.size())
			, 9
		);

	// 압축된 크기가 0 이하면 문제가 있다.
	if (compressedSize <= 0)
	{
		return false;
	}

	outBuffer.resize(compressedSize);

	BlockInfo bInfo;

	// 암호화를 위한 데이터 생성
	::RAND_bytes(bInfo.m_key, sizeof(bInfo.m_key));
	::RAND_bytes(bInfo.m_iv, sizeof(bInfo.m_iv));

	std::vector<unsigned char> encrypted;
	if (false == AES::CryptCTR(outBuffer, encrypted, bInfo.m_key, bInfo.m_iv))
	{
		return false;
	}

	// 현재 파트 파일이 꽉 찼으면 새로 생성
	if (m_currentPartFileSize + compressedSize + sizeof(uint32_t) > m_maxPartSize)
	{
		// 현재 파트 파일을 닫는다
		if (m_currentPartFileStream.is_open())
		{
			m_currentPartFileStream.close();
		}

		// 새 파트 파일을 생성하고 연다
		std::wstring currentPartPath =
			m_compressPath + L"\\part_"
			+ std::to_wstring(m_partFileIndex++)
			+ m_comExtension;

		m_currentPartFileStream.open(currentPartPath, std::ios::binary);
		m_currentPartFileSize = 0;
	}

	// 압축 데이터 기록
	uint32_t compSize32 = static_cast<uint32_t>(compressedSize);
	m_currentPartFileStream.write(reinterpret_cast<char*>(&compSize32), sizeof(compSize32));

	// 압축 및 암호화 된 데이터 기록
	uint64_t offset = m_currentPartFileStream.tellp();
	m_currentPartFileStream.write(reinterpret_cast<char*>(encrypted.data()), compressedSize);

	// 블록 메타데이터 작성
	bInfo.m_partIndex = m_partFileIndex - 1;
	bInfo.m_offset = offset;
	bInfo.m_compressedSize = compSize32;
	bInfo.m_originalSize = _dataSize;

	// 압축 정보에 블록 정보를 넣는다
	_comInfo.m_blocks.push_back(bInfo);

	// 현재 파트 크기 증가
	m_currentPartFileSize += sizeof(uint32_t) + compressedSize;

	return true;
}

void FileStorage::ShowAllFilename()
{
	for (const auto& itr : m_compressInfoMap)
	{
		std::wcout << itr.first << std::endl;
	}
}

bool FileStorage::CompressAll(const std::wstring& _path)
{
	m_chunkFinish = false;
	m_alljobFinish = false;

	// 출력 디렉토리가 없다면 현재 디렉토리에 폴더 하나 만들어서 넣기
	if (0 == m_compressPath.length())
	{
		m_compressPath = L"./compress";
	}

	// 확장자 없다면 만들기
	if (0 == m_comExtension.length())
	{
		m_comExtension = L".rcom";
	}

	// 1 MB 단위 블록
	if (0 == m_blockSize)
	{
		m_blockSize = 1024 * 1024;
	}

	// 50 MB 단위 파트 사이즈
	if (0 == m_maxPartSize)
	{
		m_maxPartSize = 50 * 1024 * 1024;
	}

	// 출력 디렉토리 생성
	if (false == CreateDirectory(m_compressPath))
	{
		return false;
	}

	// 병렬처리를 하는 경우 미리 만들어두기
	if (m_threadCount > 1)
	{
		m_threads.reserve(m_threadCount);

		for (uint32_t i = 0; i < m_threadCount; i++)
		{
			m_threads.emplace_back([this]()
				{
					CompressWithThread();
				}
			);
		}

		m_chunkThread = std::thread([this]()
			{
				WriteChunkToFile();
			}
		);
	}

	// 현재 파트의 위치
	std::wstring currentPartPath = m_compressPath + L"\\part_" + std::to_wstring(m_partFileIndex++) + m_comExtension;
	// 현재 파트 파일 생성 및 열기
	m_currentPartFileStream.open(currentPartPath, std::ios::binary);

	// 압축
	bool isCompressSuccess = CompressDirectory(_path);

	if (m_threadCount > 1)
	{
		m_alljobFinish = true;
		m_jobCv.notify_all();
		for (uint32_t i = 0; i < m_threadCount; i++)
		{
			m_threads[i].join();
		}

		m_chunkFinish = true;
		m_chunkCv.notify_all();
		m_chunkThread.join();
	}

	if (false == isCompressSuccess)
	{
		return false;
	}

	// 파트 파일 닫기
	if (m_currentPartFileStream.is_open())
	{
		m_currentPartFileStream.close();
	}

	// 인덱스 파일 생성
	std::ofstream indexFile(m_compressPath + L"\\.index", std::ios::binary);

	// 압축을 진행 할 전체 파일 갯수 기록
	uint32_t fileCount = static_cast<uint32_t>(m_compressInfoMap.size());
	indexFile.write(reinterpret_cast<char*>(&fileCount), sizeof(fileCount));

	// 모든 파일에 대한 인덱스 기록
	for (auto& fileInfo : m_compressInfoMap)
	{
		// 압축 파일 정보
		const CompressInfo& comFileInfo = fileInfo.second;
		std::wstring name = fileInfo.first;

		// 파일 이름
		uint32_t nameLen = static_cast<uint32_t>(name.size() * sizeof(wchar_t));
		indexFile.write(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
		indexFile.write(reinterpret_cast<const char*>(name.data()), nameLen);

		// 파일 원본 크기
		indexFile.write(reinterpret_cast<const char*>(&comFileInfo.m_totalOriginalSize), sizeof(comFileInfo.m_totalOriginalSize));

		// 블록 갯수
		uint32_t blockCount = static_cast<uint32_t>(comFileInfo.m_blocks.size());
		indexFile.write(reinterpret_cast<char*>(&blockCount), sizeof(blockCount));

		// 블록 정보
		for (auto& block : comFileInfo.m_blocks)
		{
			// 해당 블록이 있는 파트 번호
			indexFile.write(reinterpret_cast<const char*>(&block.m_partIndex), sizeof(block.m_partIndex));
			// 블록이 어느 위치에 있는지
			indexFile.write(reinterpret_cast<const char*>(&block.m_offset), sizeof(block.m_offset));
			// 블록의 크기
			indexFile.write(reinterpret_cast<const char*>(&block.m_compressedSize), sizeof(block.m_compressedSize));
			// 압축 해제 후 원본 크기
			indexFile.write(reinterpret_cast<const char*>(&block.m_originalSize), sizeof(block.m_originalSize));

			// 암호화 정보 기록
			indexFile.write(reinterpret_cast<const char*>(block.m_key), sizeof(block.m_key));
			indexFile.write(reinterpret_cast<const char*>(block.m_iv), sizeof(block.m_iv));
		}
	}

	m_compressInfoMap.clear();
	m_threads.clear();
	return true;
}

bool FileStorage::OpenFile(
	const std::wstring& _filename
	, OUT std::vector<unsigned char>& _fileData
)
{
	_fileData.clear();

	if (true == m_compressInfoMap.empty())
	{
		ResetCompressInfoMap();
	}

	// 있는 파일인지 확인
	auto it = m_compressInfoMap.find(_filename);
	if (it == m_compressInfoMap.end())
	{
		return false;
	}

	// 압축 정보
	const CompressInfo& fileInfo = it->second;

	// 원본 크기 만큼 확장
	_fileData.reserve(fileInfo.m_totalOriginalSize);

	// 블록 순서대로 해제
	for (const auto& block : fileInfo.m_blocks)
	{
		// 압축 파일 경로
		std::wstring currentPartPath = m_compressPath + L"\\part_" + std::to_wstring(block.m_partIndex) + m_comExtension;

		// 압축 파일 읽기 모드로 열기
		std::ifstream comFile(currentPartPath, std::ios::binary);

		// 파일 열기 실패
		if (false == comFile.is_open())
		{
			return false;
		}

		// 압축 파일의 오프셋 위치 부터 읽기 블록 데이터 읽기
		comFile.seekg(block.m_offset, std::ios::beg);
		std::vector<unsigned char> compBuf(block.m_compressedSize);
		comFile.read(reinterpret_cast<char*>(compBuf.data()), block.m_compressedSize);

		std::vector<unsigned char> decryptFile;
		// 블록 데이터 복호화
		if (false == AES::CryptCTR(compBuf, decryptFile, block.m_key, block.m_iv))
		{
			return false;
		}

		// 블록 압축 해제
		std::vector<unsigned char> decomFile(block.m_originalSize);
		int decSize =
			LZ4_decompress_safe(
				reinterpret_cast<char*>(decryptFile.data())
				, reinterpret_cast<char*>(decomFile.data())
				, static_cast<int>(decryptFile.size())
				, static_cast<int>(decomFile.size())
			);

		// 압축 해제 실패
		if (decSize <= 0)
		{
			return false;
		}

		// 파일 데이터 이어붙이기
		_fileData.insert(_fileData.end(), decomFile.begin(), decomFile.begin() + decSize);
	}

	return true;
}

bool FileStorage::ResetCompressInfoMap()
{
	// 인덱스 파일 읽기
	std::ifstream indexFile(m_compressPath + L"\\.index", std::ios::binary);
	if (false == indexFile.is_open())
	{
		return false;
	}

	// 전체 파일 갯수
	int fileCount = 0;
	indexFile.read(reinterpret_cast<char*>(&fileCount), sizeof(uint32_t));
	if (fileCount < 0)
	{
		return false;
	}

	// 모든 파일에 대한 데이터 읽기
	for (int i = 0; i < fileCount; i++)
	{
		// 압축 파일 정보
		uint32_t nameLen = 0;
		std::wstring name;

		// 파일 이름
		indexFile.read(reinterpret_cast<char*>(&nameLen), sizeof(uint32_t));
		name.resize(nameLen / sizeof(wchar_t));
		indexFile.read(reinterpret_cast<char*>(&name[0]), nameLen);

		// 파일 맵 생성
		m_compressInfoMap[name] = CompressInfo();
		CompressInfo& comFileInfo = m_compressInfoMap[name];

		// 파일 원본 크기
		indexFile.read(reinterpret_cast<char*>(&comFileInfo.m_totalOriginalSize), sizeof(comFileInfo.m_totalOriginalSize));

		// 블록 갯수
		uint32_t blockCount;
		indexFile.read(reinterpret_cast<char*>(&blockCount), sizeof(blockCount));

		// 블록 정보
		for (uint32_t j = 0; j < blockCount; j++)
		{
			BlockInfo block;

			// 해당 블록이 있는 파트 번호
			indexFile.read(reinterpret_cast<char*>(&block.m_partIndex), sizeof(block.m_partIndex));
			// 블록이 어느 위치에 있는지
			indexFile.read(reinterpret_cast<char*>(&block.m_offset), sizeof(block.m_offset));
			// 블록의 크기
			indexFile.read(reinterpret_cast<char*>(&block.m_compressedSize), sizeof(block.m_compressedSize));
			// 압축 해제 후 원본 크기
			indexFile.read(reinterpret_cast<char*>(&block.m_originalSize), sizeof(block.m_originalSize));
			// 암호화 정보
			indexFile.read(reinterpret_cast<char*>(block.m_key), sizeof(block.m_key));
			indexFile.read(reinterpret_cast<char*>(block.m_iv), sizeof(block.m_iv));

			comFileInfo.m_blocks.push_back(block);
		}
	}

	return true;
}

