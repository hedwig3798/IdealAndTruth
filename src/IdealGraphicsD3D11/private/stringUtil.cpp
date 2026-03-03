#include "pch.h"
#include "stringUtil.h"

uint32_t WstringByteSize(const std::wstring& str)
{
	return static_cast<uint32_t>(str.size() * sizeof(wchar_t));
}

std::string WstrToStr(const std::wstring& _wstr, UINT _codePage)
{
	if (true == _wstr.empty()) 
	{
		return std::string();
	}

	// 필요한 크기 계산
	int needSize = WideCharToMultiByte(
		_codePage
		, 0
		, _wstr.c_str()
		, static_cast<int>(_wstr.size())
		, nullptr
		, 0
		, nullptr
		, nullptr
	);

	std::string result(needSize, 0);

	// 실제 변환
	WideCharToMultiByte(
		_codePage
		, 0
		, _wstr.c_str()
		, static_cast<int>(_wstr.size())
		, &result[0]
		, needSize
		, nullptr
		, nullptr
	);

	return result;
}

std::wstring StrToWstr(const std::string& _str, UINT _codePage)
{
	if (_str.empty()) 
	{
		return std::wstring();
	}

	// 필요한 크기 계산
	int needSize = MultiByteToWideChar(
		_codePage
		, 0
		, _str.c_str()
		, static_cast<int>(_str.size())
		, nullptr
		, 0
	);

	std::wstring result(needSize, 0);

	// 실제 변환
	MultiByteToWideChar(
		_codePage
		, 0
		, _str.c_str()
		, static_cast<int>(_str.size())
		, &result[0]
		, needSize
	);

	return result;
}

std::string GetExeDirectoryPath()
{
	char buffer[MAX_PATH];
	DWORD len = GetModuleFileNameA(NULL, buffer, MAX_PATH);
	std::string exePath(buffer);
	size_t pos = exePath.find_last_of("\\/");
	exePath = exePath.substr(0, pos);

	return exePath;
}