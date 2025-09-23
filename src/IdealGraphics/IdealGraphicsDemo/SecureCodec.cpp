#include "SecureCodec.h"

bool AES::CryptCTR(
	const std::vector<unsigned char>& _data
	, std::vector<unsigned char>& _output
	, const unsigned char* _key
	, const unsigned char* _initVector
)
{
	// ��� ���� �ʱ�ȭ
	_output.clear();

	// EVP_CTX ��ü ����
	EVP_CIPHER_CTX* ctx = ::EVP_CIPHER_CTX_new();
	if (nullptr == ctx)
	{
		return false;
	}

	// AES-256-CTR �ʱ�ȭ
	if (1 != ::EVP_CipherInit_ex(ctx, EVP_aes_256_ctr(), nullptr, _key, _initVector, 1))
	{
		::EVP_CIPHER_CTX_free(ctx);
		return false;
	}

	// ��� ���� ũ�� �ʱ�ȭ
	_output.resize(_data.size());

	// ��ȣȭ
	int outputLen = 0;
	if (1 != ::EVP_CipherUpdate(ctx, _output.data(), &outputLen, _data.data(), (int)_data.size()))
	{
		::EVP_CIPHER_CTX_free(ctx);
		return false;
	}

	::EVP_CIPHER_CTX_free(ctx);
	return true;
}
