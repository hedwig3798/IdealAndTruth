#pragma once
#include <streambuf>
#include <istream>
#include <vector>

/// <summary>
/// ���� �����͸� ��Ʈ������ �ޱ� ���� ����
/// </summary>
class MemoryFileStreamBuf
	: public std::streambuf
{
public:
	MemoryFileStreamBuf(const std::vector<char>& _data);
	MemoryFileStreamBuf(const std::vector<unsigned char>& _data);
};

/// <summary>
/// ���� ������ ��Ʈ��
/// ������ �޸𸮿��� �����ϱ� ���� ���� ����
/// istream �������̽� ���п� �������� ���� ����
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
