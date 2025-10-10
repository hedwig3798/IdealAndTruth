#include <fstream>
#include <string>
#include "Geometry.h"

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