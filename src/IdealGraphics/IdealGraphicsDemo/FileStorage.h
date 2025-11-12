#pragma once
#pragma once
#include <string>
#include <map>
#include <set>
#include <fstream>
#include <memory>
#include <sstream>
#include <vector>
#include <queue>
#include <windows.h>
#include <thread>
#include <condition_variable>
#include <mutex>

#define OUT

/// <summary>
/// File Management System
/// 다수의 파일을 저장하고 불러오기 위한 클래스
/// </summary>
class FileStorage
{
private:
	/// <summary>
	/// 압축 블록 정보 구조체
	/// </summary>
	struct BlockInfo
	{
		uint32_t m_partIndex;
		uint64_t m_offset;
		uint64_t m_compressedSize;
		uint64_t m_originalSize;
		uint64_t m_blockIndex;
		unsigned char m_key[32];
		unsigned char m_iv[16];

		bool operator<(const BlockInfo& _other)
		{
			return m_blockIndex < _other.m_blockIndex;
		}
	};

	/// <summary>
	/// 압축 정보 구조체
	/// </summary>
	struct CompressInfo 
	{
		uint64_t m_totalOriginalSize = 0;
		std::vector<BlockInfo> m_blocks;
	};

	/// <summary>
	/// 분할 작업을 위해 스레드에 넘겨 줄 데이터
	/// </summary>
	struct JobInfo
	{
		CompressInfo& m_comInfo;
		std::vector<unsigned char> m_oriBuffer;
		uint64_t m_dataSize;
		uint64_t m_blockIndex;
		static bool m_isSuccess;

		JobInfo(CompressInfo& _comInfo)
			: m_comInfo(_comInfo)
			, m_dataSize(0)
			, m_blockIndex(0)
		{
		}


	};

	// 파일 이름 - 압축 정보 맵
	std::map<std::wstring, CompressInfo> m_compressInfoMap;

	// 경로
	std::wstring m_compressPath;
	std::wstring m_decompressPath;
	// 공통 파일 정보
	std::wstring m_comExtension;
	std::wstring m_comFilename;

	// 한 블록의 크기
	uint32_t m_blockSize;
	// 한 파트의 크기
	uint32_t m_maxPartSize;

	// 스레드 갯수
	uint32_t m_threadCount;
	// 청크의 크기
	uint32_t m_chunkSize;
	// 청크 쓰기를 위한 조건 변수
	std::condition_variable m_chunkCv;
	// 청크 접근을 위한 뮤텍스
	std::mutex m_chunkMutex;

	// 블록 정보 - 데이터가 있는 청크
	std::vector<std::pair<BlockInfo*, std::vector<unsigned char>>> m_chunk;
	uint32_t m_currentChunkSize;

	// 파트 파일 스트림
	std::ofstream m_currentPartFileStream;
	uint64_t m_currentPartFileSize;
	uint32_t m_partFileIndex;
	bool m_chunkFinish = false;
	std::thread m_chunkThread;

	// 멀티 스레드를 위한 변수들
	bool m_alljobFinish = false;
	std::condition_variable m_jobCv;
	std::mutex m_jobMutex;
	std::queue<JobInfo> m_jobQ;
	std::vector<std::thread> m_threads;

public:
	FileStorage();
	virtual ~FileStorage();

public:
#pragma region GET_SET
	void SetCompressExtension(const std::wstring& _extension) { m_comExtension = _extension; };
	void SetDecompressOutputPath(const std::wstring& _path) { m_decompressPath = _path; };
	void SetCompressFilePath(const std::wstring& _path) { m_compressPath = _path; };
	void SetOutputFileName(const std::wstring& _name) { m_comFilename = _name; };

	void SetThreadCount(uint32_t _count) { m_threadCount = _count; };
	void SetChunkSize(uint32_t _size) { m_chunkSize = _size; };
#pragma endregion GET_SET
	/// <summary>
	/// 모든 파일의 이름을 출력
	/// </summary>
	void ShowAllFilename();

	/// <summary>
	/// 경로에 있는 모든 파일 압축
	/// </summary>
	/// <returns>성공 여부</returns>
	bool CompressAll(const std::wstring& _path);

	/// <summary>
	/// 파일 데이터 얻기
	/// </summary>
	/// <param name="_filename">파일 이름.확장자</param>
	/// <param name="_fileData">데이터를 받을 벡터</param>
	/// <returns>성공 여부</returns>
	bool OpenFile(
		const std::wstring& _filename
		, OUT std::vector<unsigned char>& _fileData
	);

	/// <summary>
	/// 인덱스 파일 읽기
	/// </summary>
	/// <returns>성공 여부</returns>
	bool ResetCompressInfoMap();

private:

	/// <summary>
	/// 디렉토리 생성
	/// </summary>
	/// <param name="_path">해당 경로의 디렉토리 생성</param>
	/// <returns>성공 여부</returns>
	bool CreateDirectory(const std::wstring& _path);

	/// <summary>
	/// 디렉토리 내의 모든 파일 압축
	/// </summary>
	/// <param name="_path">디렉토리</param>
	bool CompressDirectory(const std::wstring& _path);

	/// <summary>
	/// 스레드를 활용해서 압축 해제
	/// </summary>
	void CompressWithThread();

	/// <summary>
	/// 청크를 파일에 쓰기
	/// </summary>
	void WriteChunkToFile();

	/// <summary>
	/// 스레드 없이 패킹
	/// </summary>
	/// <param name="_comInfo">압축 정보 구조체</param>
	/// <param name="_oriBuffer">최종 작성 할 파일</param>
	/// <param name="_dataSize">데이터 블록 크기</param>
	/// <returns>결과값</returns>
	bool CompressWithNonThread(
		OUT CompressInfo& _comInfo
		, const std::vector<unsigned char>& _oriBuffer
		, size_t _dataSize
	);
};

