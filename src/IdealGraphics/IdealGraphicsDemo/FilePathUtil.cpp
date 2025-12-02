#include "FilePathUtil.h"

std::string GetExeDir()
{
	// 중복 연산 방지
	static std::string result = "";
	if (false == result.empty())
	{
		return result;
	}

	char buffer[MAX_PATH];
	DWORD len = ::GetModuleFileNameA(NULL, buffer, MAX_PATH);
	std::string fullPath(buffer, len);

	size_t pos = fullPath.find_last_of("\\/");
	result = fullPath.substr(0, pos);
	return result;
}

