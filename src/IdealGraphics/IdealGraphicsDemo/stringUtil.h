#pragma once

#include <string>
#include <windows.h>

uint32_t WstringByteSize(const std::wstring& str) 
{
	return static_cast<uint32_t>(str.size() * sizeof(wchar_t));
}

std::string WstrToStr(const std::wstring& wstr, UINT codePage = CP_UTF8) 
{
	// �ٲܰ� ���ٸ�
	if (wstr.empty())
	{
		return std::string();
	}

	// ��ȯ�� ���ڿ� ���� ���
	int needSize = WideCharToMultiByte(
		codePage
		, 0
		, wstr.c_str()
		, static_cast<int>(wstr.size())
		, nullptr
		, 0
		, nullptr
		, nullptr
	);

	std::string result(needSize, 0);

	// ���� ��ȯ
	WideCharToMultiByte(
		codePage
		, 0
		, wstr.c_str()
		, static_cast<int>(wstr.size())
		, &result[0]
		, needSize
		, nullptr
		, nullptr
	);

	return result;
}