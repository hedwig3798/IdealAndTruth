#pragma once

#include <vector>

#include <openssl/evp.h>
#include <openssl/rand.h>

class AES
{
public:
	/// <summary>
	/// ��ȣȣ �� ��ȣȭ �Լ�
	/// </summary>
	/// <param name="_data">�Ϻ�ȣȭ �� ������</param>
	/// <param name="_output">�Ϻ�ȣȭ �� ������</param>
	/// <param name="_key">��ȣȭ Ű</param>
	/// <param name="_initVector">CTR ���� ����</param>
	/// <returns></returns>
	static bool CryptCTR(
		const std::vector<unsigned char>& _data
		, std::vector<unsigned char>& _output
		, const unsigned char* _key
		, const unsigned char* _initVector);
};