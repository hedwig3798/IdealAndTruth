#include <fstream>
#include <string>
#include <iostream>

#include "Geometry.h"
#include "Convert.h"

/// <summary>
/// Assimp�� �̿��� �Ž� ������
/// �⺻ ���� ������ ������ ���� �����ͷ� �����ϴ� �Լ�
/// </summary>
/// <returns></returns>
int main()
{
	// �⺻ ����
	std::string extension = ".iver";
	std::string outputPath = "E:\\IAT\\src\\Resource\\Mesh\\";

	Converter con;

	con.SetThreadCount(4);
	con.SetOutputPath(outputPath);
	con.SetExtension(extension);

	while (true)
	{
		// ���� ��θ� ������ ���� ( 8KB )
		wchar_t fileBuffer[8192] = { 0 };

		// ���� �Է� �ޱ�
		// OPENFILENAME ����ü ����
		OPENFILENAMEW ofn;
		ZeroMemory(&ofn, sizeof(ofn));

		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		ofn.lpstrFile = fileBuffer;
		ofn.nMaxFile = sizeof(fileBuffer) / sizeof(fileBuffer[0]);
		ofn.lpstrFilter = L"FBX ���� (*.fbx)\0*.fbx\0";
		ofn.nFilterIndex = 1;
		ofn.Flags =
			OFN_EXPLORER
			| OFN_FILEMUSTEXIST
			| OFN_PATHMUSTEXIST
			| OFN_ALLOWMULTISELECT;

		if (true == GetOpenFileNameW(&ofn)) 
		{
			std::vector<std::wstring> files;

			// fileBuffer�� ���� ���·� ä����:
			// [�������]\0[����1]\0[����2]\0...\0\0
			std::wstring directory = fileBuffer;
			


			// ��� ���
			std::cout << "���õ� ���� ���\n";
			for (const auto& path : files)
				std::wcout << L"  " << path << std::endl;
		}
		else 
		{
			std::cout << "���� ������ ��" << std::endl;
			break;
		}
	}

	// �⺻ ����
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