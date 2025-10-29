#include "pch.h"
#include "Convert.h"
#include "fstream"
Converter::Converter()
	: m_threadCount(4)
{
}

Converter::~Converter()
{

}

void Converter::ReadAssetFile(const std::vector<ConvertJob>& _data)
{

	for (int i = 0; i < m_threadCount; i++)
	{
		m_threads.emplace_back([this]()
			{
				ImportDataWithThread();
			}
		);
	}

	for (const auto& itr : _data)
	{
		std::lock_guard<std::mutex> lock(m_jobMutex);
		m_jobQ.push(itr);
		m_jobCv.notify_one();
	}

	m_alljobFinish = true;
	m_jobCv.notify_all();
	for (int i = 0; i < m_threadCount; i++)
	{
		m_threads[i].join();
	}


	return;
}

void Converter::ImportDataWithThread()
{
	while (true)
	{
		std::unique_lock<std::mutex> lock(m_jobMutex);

		// ��� ���� �����ų�, ť�� ������� �ʴٸ� �Ʒ��� ����
		m_jobCv.wait(lock, [this]()
			{
				return true == m_alljobFinish || false == m_jobQ.empty();
			}
		);

		// ��� ���� �����ų� ť�� ��������� �����Ѵ�.
		if (true == m_alljobFinish
			&& true == m_jobQ.empty())
		{
			break;
		}

		ConvertJob job = m_jobQ.front();
		m_jobQ.pop();
		lock.unlock();

		// ���� �۾� ����
		Assimp::Importer* importer = new Assimp::Importer();
		importer->SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, 0);

		unsigned int flag = aiProcess_ConvertToLeftHanded   // �޼� ��ǥ�� �ε�, uv ������ �»��, cw �ε� (�ð� ����)
			| aiProcess_MakeLeftHanded
			//| aiProcess_FlipUVs 
			//| aiProcess_FlipWindingOrder 
			| aiProcess_JoinIdenticalVertices // ������ ��ġ�� ������ �ϳ��� �������� ���� (���� �������� �ߺ��� �����ϰ� �� ����ȭ)
			| aiProcess_Triangulate  // �ﰢ�� �޽� ���·� ����
			| aiProcess_GenUVCoords  // UV ���� ����
			| aiProcess_GenNormals;  // Normal ���� ����

		// �ϴ� �־�� ���´�
		// ���߿� �����ؾߵ�
		if (true == job.m_hasBone)
		{
			// ���������� �ִϸ��̼� ������ �����ϰ� ���� ������ ����
			flag |= aiProcess_PreTransformVertices;
		}

		// ����� ������ ���� �� ����
		const aiScene* scene = importer->ReadFile(
			job.m_path,
			flag
		);

		// ���� �� ����ü
		asModel model;

		// ���� �̸� ��������
		size_t pos1 = job.m_path.find_last_of("/\\");
		std::string filename;
		if (std::string::npos == pos1)
		{
			filename = job.m_path;
		}
		filename = job.m_path.substr(pos1 + 1);

		// Ȯ���� ����
		size_t pos2 = filename.find_last_of('.');
		if (std::string::npos == pos2)
		{
			model.m_name = filename;
		}
		model.m_name = filename.substr(0, pos2);

		// �� ������ �б�
		ReadModelData(
			scene->mRootNode
			, 0
			, -1
			, model
			, scene
		);

		// �� ������ ����
		WriteModel(model);
	}
}

void Converter::ReadModelData(
	aiNode* _node
	, int _index
	, int _parent
	, OUT asModel& _model
	, const aiScene* _scene
)
{
	// �ϴ� ��带 ���� �� �̶�� �����Ѵ�.
	asBone bone;
	bone.m_index = _index;
	bone.m_parentIndex = _parent;
	bone.m_name = _node->mName.C_Str();

	// �θ� ���� ���
	aiMatrix4x4t<float> transform = _node->mTransformation;
	// DX�� �� �켱, FBX, ASE�� �� �켱
	_node->mMetaData;

	// _model.m_bones.push_back(bone);

	// Mesh ���� �б�
	ReadMeshData(_node, _index, _model, _scene);

	// ���
	for (unsigned int i = 0; i < _node->mNumChildren; i++)
	{
		ReadModelData(
			_node->mChildren[i]
			, 0 // ���߿� �� �Ľ��� �� �� �κ� �����ؾߵ�
			, _index
			, _model
			, _scene
		);
	}
}

void Converter::ReadMeshData(
	aiNode* _node
	, int _bone
	, OUT asModel& _model
	, const aiScene* _scene
)
{
	// �Ž��� ���� ����� ���
	if (_node->mNumMeshes < 1)
	{
		return;
	}

	// �Ž��� ������ �� ���� �ִ�.
	for (unsigned int i = 0; i < _node->mNumMeshes; i++)
	{
		asMesh mesh;

		mesh.m_name = _node->mName.C_Str();

		// scene ���� ���εǴ� �Ž��� �ε���
		int index = _node->mMeshes[i];
		const aiMesh* srcMesh = _scene->mMeshes[index];

		// ���� ��ȸ
		mesh.m_vertex.resize(srcMesh->mNumVertices);
		for (unsigned int v = 0; v < srcMesh->mNumVertices; v++)
		{
			// position
			mesh.m_vertex[v].m_position.x = srcMesh->mVertices[v].x;
			mesh.m_vertex[v].m_position.y = srcMesh->mVertices[v].y;
			mesh.m_vertex[v].m_position.z = srcMesh->mVertices[v].z;

			// normal
			if (true == srcMesh->HasNormals())
			{
				mesh.m_vertex[v].m_normal.x = srcMesh->mNormals[v].x;
				mesh.m_vertex[v].m_normal.y = srcMesh->mNormals[v].y;
				mesh.m_vertex[v].m_normal.z = srcMesh->mNormals[v].z;
			}

			// texture
			if (true == srcMesh->HasTextureCoords(0))
			{
				mesh.m_vertex[v].m_uv.x = srcMesh->mTextureCoords[0][v].x;
				mesh.m_vertex[v].m_uv.y = srcMesh->mTextureCoords[0][v].y;
			}
		}

		/// index
		for (unsigned int f = 0; f < srcMesh->mNumFaces; f++)
		{
			aiFace& face = srcMesh->mFaces[f];

			for (unsigned int k = 0; k < face.mNumIndices; k++)
			{
				mesh.m_index.push_back(face.mIndices[k]);
			}
		}

		_model.m_meshes.push_back(mesh);
	}
}

void Converter::WriteModel(const asModel& _model)
{
	std::string outputFilePath = m_outputPath + _model.m_name + m_extension;

	std::ofstream file;
	file.open(outputFilePath, std::ios::binary);

	if (false == file.is_open())
	{
		return;
	}

	uint64_t meshCount = _model.m_meshes.size();
	file.write(reinterpret_cast<char*>(&meshCount), sizeof(uint64_t));

	for (auto& itr : _model.m_meshes)
	{
		std::string meshName = _model.m_name + "_" + itr.m_name;
		uint64_t nameSize = meshName.size();
		file.write(reinterpret_cast<char*>(&nameSize), sizeof(uint64_t));
		file.write(meshName.c_str(), meshName.size());

		// vertex
		uint64_t vertexCount = itr.m_vertex.size();
		file.write(reinterpret_cast<char*>(&vertexCount), sizeof(uint64_t));
		file.write(reinterpret_cast<const char*>(itr.m_vertex.data()), vertexCount * sizeof(VertexPUN));

		// index
		uint64_t indexCount = itr.m_index.size();
		file.write(reinterpret_cast<char*>(&indexCount), sizeof(uint64_t));
		file.write(reinterpret_cast<const char*>(itr.m_index.data()), indexCount * sizeof(int));
	}
}
