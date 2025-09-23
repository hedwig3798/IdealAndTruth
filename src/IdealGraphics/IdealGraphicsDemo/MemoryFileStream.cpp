#include "MemoryFileStream.h"

MemoryFileStreamBuf::MemoryFileStreamBuf(const std::vector<char>& _data)
{
	// 데이터 시작점
	char* base = const_cast<char*>(_data.data());

	// 버퍼 구역 설정
	setg(base, base, base + _data.size());
}

MemoryFileStreamBuf::MemoryFileStreamBuf(const std::vector<unsigned char>& _data)
{
	// 데이터 시작점
	unsigned char* unsignedBase = const_cast<unsigned char*>(_data.data());
	char* base = reinterpret_cast<char*>(unsignedBase);

	// 버퍼 구역 설정
	setg(base, base, base + _data.size());
}

MemoryFileStream::MemoryFileStream(std::vector<char>&& _data)
	: std::istream(&m_buf)
	, m_data(_data)
	, m_buf(m_data)
{
}

MemoryFileStream::MemoryFileStream(std::vector<unsigned char>&& _data)
	: std::istream(&m_buf)
	, m_data(_data.begin(), _data.end())
	, m_buf(m_data)
{
}
