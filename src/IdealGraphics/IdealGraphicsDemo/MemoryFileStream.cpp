#include "MemoryFileStream.h"

MemoryFileStreamBuf::MemoryFileStreamBuf(const std::vector<char>& _data)
{
	// ������ ������
	char* base = const_cast<char*>(_data.data());

	// ���� ���� ����
	setg(base, base, base + _data.size());
}

MemoryFileStreamBuf::MemoryFileStreamBuf(const std::vector<unsigned char>& _data)
{
	// ������ ������
	unsigned char* unsignedBase = const_cast<unsigned char*>(_data.data());
	char* base = reinterpret_cast<char*>(unsignedBase);

	// ���� ���� ����
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
