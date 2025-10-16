#include "pch.h"
#include "Convert.h"

Converter::Converter()
	: m_threadCount(0)
{
}

Converter::~Converter()
{

}

void Converter::ReadAssetFile(const std::vector<std::string>& _files)
{
	if (1 >= m_threadCount)
	{
		for (const auto& itr : _files)
		{
			ImportData(itr);
		}
		return;
	}

	for (uint32_t i = 0; i < m_threadCount; i++)
	{
		m_threads.emplace_back([this]()
			{
				ImportDataWithThread();
			}
		);
	}

	for (const auto& itr : _files)
	{
		std::lock_guard<std::mutex> lock(m_jobMutex);
		m_jobQ.push(itr);
		m_jobCv.notify_one();
	}

	m_alljobFinish = true;
	m_jobCv.notify_all();
	for (uint32_t i = 0; i < m_threadCount; i++)
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

		std::string job = m_jobQ.front();
		m_jobQ.pop();
		lock.unlock();

		// ���� �۾� ����
		Assimp::Importer* importer = new Assimp::Importer();
		importer->SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, 0);

		const aiScene* scene = importer->ReadFile(
			job,
			aiProcess_ConvertToLeftHanded   // �޼� ��ǥ�� �ε�, uv ������ �»��, cw �ε� (�ð� ����)
			| aiProcess_MakeLeftHanded
			//| aiProcess_FlipUVs 
			//| aiProcess_FlipWindingOrder 
			| aiProcess_JoinIdenticalVertices // ������ ��ġ�� ������ �ϳ��� �������� ���� (���� �������� �ߺ��� �����ϰ� �� ����ȭ)
			//| aiProcess_PreTransformVertices // ���������� �ִϸ��̼� ������ �����ϰ� ���� ������ ����
			| aiProcess_Triangulate  // �ﰢ�� �޽� ���·� ����
			| aiProcess_GenUVCoords  // UV ���� ����
			| aiProcess_GenNormals  // Normal ���� ����
			| aiProcess_CalcTangentSpace  // TangentSpace ���� ���� 
		);

		std::vector<asMesh> meshData;
		std::vector<asBone> boneData;
		ReadModelData(
			scene->mRootNode
			, 0
			, -1
			, meshData
			, boneData
			, scene
		);
		// ReadSkinData();

		// WriteCSVFile(_savePath);
		// WriteModelFile(finalPath);
	}
}

void Converter::ImportData(std::string _path)
{

}

void Converter::ReadModelData(
	aiNode* _node
	, int _index
	, int _parent
	, std::vector<asMesh>& _meshData
	, std::vector<asBone>& _boneData
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

	_boneData.push_back(bone);

	// Mesh ���� �б�
	ReadMeshData(_node, _index, _meshData, _scene);

	// ���
	for (int i = 0; i < _node->mNumChildren; i++)
	{
		ReadModelData(
			_node->mChildren[i]
			, _boneData.size()
			, _index
			, _meshData
			, _boneData
			, _scene
		);
	}
}

void Converter::ReadMeshData(
	aiNode* _node
	, int _bone
	, std::vector<asMesh>& _meshData
	, const aiScene* _scene
)
{
	// �Ž��� ���� ����� ���
	if (_node->mNumMeshes < 1)
	{
		return;
	}

	// �Ž��� ������ �� ���� �ִ�.
	for (int i = 0; i < _node->mNumMeshes; i++)
	{
		asMesh mesh;

		mesh.m_name = _node->mName.C_Str();

		// scene ���� ���εǴ� �Ž��� �ε���
		int index = _node->mMeshes[i];
		const aiMesh* srcMesh = _scene->mMeshes[index];

		// �� ��ü�� ���� �Ž��� ���� �� �ִ�.
		// �� �Ž����� �ٸ� ���׸����� ��� �� �� �ִ�.
		// �� �� ����� ���� ����
		const aiMaterial* material = _scene->mMaterials[srcMesh->mMaterialIndex];
		// mesh->materialName = material->GetName().C_Str();

		// ���� ��ȸ

		int vertexSize = srcMesh->mNumVertices * (sizeof(aiVector3D));

		/// Position
		mesh.m_position = std::vector<aiVector3D>(
			srcMesh->mVertices
			, srcMesh->mVertices + vertexSize
		);

		/// UV
		if (srcMesh->HasTextureCoords(0))
		{
			mesh.m_textuerIndex = std::vector<aiVector3D>(
				srcMesh->mTextureCoords[0]
				, srcMesh->mTextureCoords[0] + vertexSize
			);
		}

		/// Normal
		if (srcMesh->HasNormals())
		{
			mesh.m_normal = std::vector<aiVector3D>(
				srcMesh->mNormals
				, srcMesh->mVertices + vertexSize
			);
		}

		/// index
		for (int f = 0; f < srcMesh->mNumFaces; f++)
		{
			aiFace& face = srcMesh->mFaces[f];

			for (int k = 0; k < face.mNumIndices; k++)
			{
				mesh.m_index.push_back(face.mIndices[k]);
			}
		}

		_meshData.push_back(mesh);
	}
}
