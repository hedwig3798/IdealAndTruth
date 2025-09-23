#pragma once
#include <streambuf>
#include <istream>
#include <vector>

/// <summary>
/// 파일 데이터를 스트림으로 받기 위한 버퍼
/// </summary>
class MemoryFileStreamBuf
	: public std::streambuf
{
public:
	MemoryFileStreamBuf(const std::vector<char>& _data);
	MemoryFileStreamBuf(const std::vector<unsigned char>& _data);
};

/// <summary>
/// 파일 데이터 스트림
/// 파일을 메모리에만 유지하기 위해 따로 만듬
/// istream 인터페이스 덕분에 여러곳에 연결 가능
/// </summary>
class MemoryFileStream
	: public std::istream
{
private:
	std::vector<char> m_data;
	MemoryFileStreamBuf m_buf;

public:
	MemoryFileStream(std::vector<char>&& _data);
	MemoryFileStream(std::vector<unsigned char>&& _data);
};
