#include <iostream>
#include "FileStorage.h"
#include "SecureCodec.h"

int main()
{
	unsigned char key[32];
	unsigned char iv[16];
	::RAND_bytes(key, sizeof(key));
	::RAND_bytes(iv, sizeof(iv));

	std::string plaintext = "This is a test for AES-CTR encryption in game resource files.";
	std::vector<unsigned char> input(plaintext.begin(), plaintext.end());
	std::vector<unsigned char> encrypted, decrypted;


	std::cout << "Original : " << plaintext << std::endl;

	// 암호화
	AES::CryptCTR(input, encrypted, key, iv);
	std::cout << "Encrypted : " << std::string(encrypted.begin(), encrypted.end()) << std::endl;

	// 복호화 (같은 key/iv 사용)
	AES::CryptCTR(encrypted, decrypted, key, iv);

	std::cout << "Decrypted: " << std::string(decrypted.begin(), decrypted.end()) << std::endl;

	FileStorage fms;

	fms.SetThreadCount(4);
	fms.SetChunkSize(1024);

	fms.SetOutputFileName(L"comTest_");
	fms.SetCompressExtension(L".rcom");
	fms.SetCompressFilePath(L"E:\\FMS\\CompressTest");

	if (false == fms.CompressAll(L"E:\\FMS\\CompressTestOrigin"))
	{
		std::cout << "Cannot Compress File\n";
		return 0;
	}

	std::istream* testFile;
	testFile = fms.OpenFile(L"AlphaBlend.h");
	if (!testFile)
	{
		std::cout << "Cannot Open File\n";
		return 0;
	}
	std::wstring content((std::istreambuf_iterator<char>(*testFile)), std::istreambuf_iterator<char>());

	std::wcout << content;

	return 0;
}