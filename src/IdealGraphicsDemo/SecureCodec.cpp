#include "SecureCodec.h"

bool AES::CryptCTR(
	const std::vector<unsigned char>& _data
	, std::vector<unsigned char>& _output
	, const unsigned char* _key
	, const unsigned char* _initVector
)
{
	// 출력 벡터 초기화
	_output.clear();

	// EVP_CTX 객체 생성
	EVP_CIPHER_CTX* ctx = ::EVP_CIPHER_CTX_new();
	if (nullptr == ctx)
	{
		return false;
	}

	// AES-256-CTR 초기화
	if (1 != ::EVP_CipherInit_ex(ctx, EVP_aes_256_ctr(), nullptr, _key, _initVector, 1))
	{
		::EVP_CIPHER_CTX_free(ctx);
		return false;
	}

	// 출력 벡터 크기 초기화
	_output.resize(_data.size());

	// 암호화
	int outputLen = 0;
	if (1 != ::EVP_CipherUpdate(ctx, _output.data(), &outputLen, _data.data(), (int)_data.size()))
	{
		::EVP_CIPHER_CTX_free(ctx);
		return false;
	}

	::EVP_CIPHER_CTX_free(ctx);
	return true;
}
