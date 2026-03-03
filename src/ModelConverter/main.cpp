#include <fstream>
#include <string>
#include <iostream>

#include "Geometry.h"
#include "Convert.h"

/// <summary>
/// Assimp를 이용한 매쉬 데이터
/// 기본 기하 도형을 나만의 파일 데이터로 추출하는 함수
/// </summary>
/// <returns></returns>
int main()
{
	// 기본 정보
	std::string extension = ".iver";
	std::string outputPath = "E:\\IAT\\src\\Resource\\Mesh\\";

	Converter con;

	con.SetThreadCount(4);
	con.SetOutputPath(outputPath);
	con.SetExtension(extension);

	while (true)
	{
		// 파일 경로를 저장할 버퍼 ( 8KB )
		wchar_t fileBuffer[8192] = { 0 };

		// 파일 입력 받기
		// OPENFILENAME 구조체 설정
		OPENFILENAMEW ofn;
		ZeroMemory(&ofn, sizeof(ofn));

		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		ofn.lpstrFile = fileBuffer;
		ofn.nMaxFile = sizeof(fileBuffer) / sizeof(fileBuffer[0]);
		ofn.lpstrFilter = L"FBX 파일 (*.fbx)\0*.fbx\0";
		ofn.nFilterIndex = 1;
		ofn.Flags =
			OFN_EXPLORER
			| OFN_FILEMUSTEXIST
			| OFN_PATHMUSTEXIST
			| OFN_ALLOWMULTISELECT;

		if (true == GetOpenFileNameW(&ofn)) 
		{
			std::vector<std::wstring> files;

			// fileBuffer는 다음 형태로 채워짐:
			// [폴더경로]\0[파일1]\0[파일2]\0...\0\0
			std::wstring directory = fileBuffer;
			


			// 결과 출력
			std::cout << "선택된 파일 목록\n";
			for (const auto& path : files)
				std::wcout << L"  " << path << std::endl;
		}
		else 
		{
			std::cout << "파일 선택이 끝" << std::endl;
			break;
		}
	}

	// 기본 기하
	Geometry::Plane p;
	Geometry::Cube c;
	Geometry::Axes a;

	{
		std::string fileName = outputPath + p.m_name + extension;
		std::ofstream ofs(fileName, std::ios::binary);
		if (false == ofs.is_open())
		{
			return 0;
		}

		int vSize = p.vNum * sizeof(DVertex);
		int iSize = p.iNum * sizeof(int);
		int dSize = sizeof(DVertex);

		ofs.write(reinterpret_cast<const char*>(&dSize), sizeof(int));
		ofs.write(reinterpret_cast<const char*>(&vSize), sizeof(int));
		ofs.write(reinterpret_cast<const char*>(&iSize), sizeof(int));

		ofs.write(reinterpret_cast<const char*>(p.m_vectex), vSize);
		ofs.write(reinterpret_cast<const char*>(p.m_index), iSize);

		ofs.close();
	}

	{
		std::string fileName = outputPath + c.m_name + extension;
		std::ofstream ofs(fileName, std::ios::binary);
		if (false == ofs.is_open())
		{
			return 0;
		}

		int vSize = c.vNum * sizeof(DVertex);
		int iSize = c.iNum * sizeof(int);
		int dSize = sizeof(DVertex);

		ofs.write(reinterpret_cast<const char*>(&dSize), sizeof(int));
		ofs.write(reinterpret_cast<const char*>(&vSize), sizeof(int));
		ofs.write(reinterpret_cast<const char*>(&iSize), sizeof(int));

		ofs.write(reinterpret_cast<const char*>(c.m_vectex), vSize);
		ofs.write(reinterpret_cast<const char*>(c.m_index), iSize);
		ofs.close();
	}

	{
		std::string fileName = outputPath + a.m_name + extension;
		std::ofstream ofs(fileName, std::ios::binary);
		if (false == ofs.is_open())
		{
			return 0;
		}

		int vSize = a.vNum * sizeof(VertexPC);
		int iSize = a.iNum * sizeof(int);
		int dSize = sizeof(DVertex);

		ofs.write(reinterpret_cast<const char*>(&dSize), sizeof(int));
		ofs.write(reinterpret_cast<const char*>(&vSize), sizeof(int));
		ofs.write(reinterpret_cast<const char*>(&iSize), sizeof(int));

		ofs.write(reinterpret_cast<const char*>(a.m_vectex), vSize);
		ofs.write(reinterpret_cast<const char*>(a.m_index), iSize);
		ofs.close();
	}

	return 0;
}