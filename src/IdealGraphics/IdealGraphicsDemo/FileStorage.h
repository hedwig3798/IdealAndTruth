#pragma once
#pragma once
#include <string>
#include <map>
#include <set>
#include <fstream>
#include <windows.h>
#include <memory>
#include <sstream>
#include <vector>
#include <queue>
#include <algorithm>
#include <thread>
#include <condition_variable>
#include <mutex>

#include "lz4hc.h"
#include "lz4.h"
#include "MemoryFileStream.h"

/// <summary>
/// File Management System
/// �ټ��� ������ �����ϰ� �ҷ����� ���� Ŭ����
/// </summary>
class FileStorage
{
private:
	/// <summary>
	/// ���� ��� ���� ����ü
	/// </summary>
	struct BlockInfo
	{
		uint32_t m_partIndex;
		uint64_t m_offset;
		uint64_t m_compressedSize;
		uint64_t m_originalSize;
		unsigned char m_key[32];
		unsigned char m_iv[16];
	};

	/// <summary>
	/// ���� ���� ����ü
	/// </summary>
	struct CompressInfo 
	{
		uint64_t m_totalOriginalSize = 0;
		std::vector<BlockInfo> m_blocks;
	};

	/// <summary>
	/// ���� �۾��� ���� �����忡 �Ѱ� �� ������
	/// </summary>
	struct JobInfo
	{
		CompressInfo& m_comInfo;
		std::vector<unsigned char> m_oriBuffer;
		uint64_t m_dataSize;
		static bool m_isSuccess;

		JobInfo(CompressInfo& _comInfo)
			: m_comInfo(_comInfo)
			, m_dataSize(0)
		{
		}
	};

	// ���� �̸� - ���� ���� ��
	std::map<std::wstring, CompressInfo> m_compressInfoMap;
	// ���� �̸� - ���� ��Ʈ�� ĳ�ÿ� ��
	std::map<std::wstring, std::unique_ptr<MemoryFileStream>> m_fileChace;

	// ���
	std::wstring m_compressPath;
	std::wstring m_decompressPath;
	// ���� ���� ����
	std::wstring m_comExtension;
	std::wstring m_comFilename;

	// �� ����� ũ��
	uint32_t m_blockSize;
	// �� ��Ʈ�� ũ��
	uint32_t m_maxPartSize;

	// ������ ����
	uint32_t m_threadCount;
	// ûũ�� ũ��
	uint32_t m_chunkSize;
	// ûũ ���⸦ ���� ���� ����
	std::condition_variable m_chunkCv;
	// ûũ ������ ���� ���ؽ�
	std::mutex m_chunkMutex;

	// ��� ���� - �����Ͱ� �ִ� ûũ
	std::vector<std::pair<BlockInfo*, std::vector<unsigned char>>> m_chunk;
	uint32_t m_currentChunkSize;

	// ��Ʈ ���� ��Ʈ��
	std::ofstream m_currentPartFileStream;
	uint64_t m_currentPartFileSize;
	uint32_t m_partFileIndex;
	bool m_chunkFinish = false;
	std::thread m_chunkThread;

	// ��Ƽ �����带 ���� ������
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
	/// ��� ������ �̸��� ���
	/// </summary>
	void ShowAllFilename();

	/// <summary>
	/// ��ο� �ִ� ��� ���� ����
	/// </summary>
	/// <returns>���� ����</returns>
	bool CompressAll(const std::wstring& _path);

	/// <summary>
	/// ���� ����
	/// </summary>
	/// <param name="_filename">���� �̸�.Ȯ����</param>
	/// <returns>���� ��Ʈ��</returns>
	std::istream* OpenFile(const std::wstring& _filename);

	/// <summary>
	/// �ε��� ���� �б�
	/// </summary>
	/// <returns>���� ����</returns>
	bool ResetCompressInfoMap();

private:

	/// <summary>
	/// ���丮 ����
	/// </summary>
	/// <param name="_path">�ش� ����� ���丮 ����</param>
	/// <returns>���� ����</returns>
	bool CreateDirectory(const std::wstring& _path);

	/// <summary>
	/// ���丮 ���� ��� ���� ����
	/// </summary>
	/// <param name="_path">���丮</param>
	bool CompressDirectory(const std::wstring& _path);

	void CompressWithThread();

	void WriteChunkToFile();

	/// <summary>
	/// ������ ���� ��ŷ
	/// </summary>
	/// <param name="_comInfo">���� ���� ����ü</param>
	/// <param name="_oriBuffer">���� �ۼ� �� ����</param>
	/// <param name="_dataSize">������ ��� ũ��</param>
	/// <returns>�����</returns>
	bool CompressWithNonThread(
		OUT CompressInfo& _comInfo
		, const std::vector<unsigned char>& _oriBuffer
		, size_t _dataSize
	);
};

