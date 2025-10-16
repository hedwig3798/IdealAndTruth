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

		// 모든 일이 끝나거나, 큐가 비어있지 않다면 아래로 진행
		m_jobCv.wait(lock, [this]()
			{
				return true == m_alljobFinish || false == m_jobQ.empty();
			}
		);

		// 모든 일이 끝났거나 큐가 비어있으면 무시한다.
		if (true == m_alljobFinish
			&& true == m_jobQ.empty())
		{
			break;
		}

		std::string job = m_jobQ.front();
		m_jobQ.pop();
		lock.unlock();

		// 실제 작업 시작
		Assimp::Importer* importer = new Assimp::Importer();
		importer->SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, 0);

		const aiScene* scene = importer->ReadFile(
			job,
			aiProcess_ConvertToLeftHanded   // 왼손 좌표계 로드, uv 시작점 좌상단, cw 로드 (시계 방향)
			| aiProcess_MakeLeftHanded
			//| aiProcess_FlipUVs 
			//| aiProcess_FlipWindingOrder 
			| aiProcess_JoinIdenticalVertices // 동일한 위치의 정점을 하나의 정점으로 병합 (정점 데이터의 중복을 제거하고 모델 최적화)
			//| aiProcess_PreTransformVertices // 계층구조와 애니메이션 정보를 제거하고 정점 정보를 구성
			| aiProcess_Triangulate  // 삼각형 메쉬 형태로 구성
			| aiProcess_GenUVCoords  // UV 정보 생성
			| aiProcess_GenNormals  // Normal 정보 생성
			| aiProcess_CalcTangentSpace  // TangentSpace 정보 생성 
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
	// 일단 노드를 전부 본 이라고 가정한다.
	asBone bone;
	bone.m_index = _index;
	bone.m_parentIndex = _parent;
	bone.m_name = _node->mName.C_Str();

	// 부모 기준 행렬
	aiMatrix4x4t<float> transform = _node->mTransformation;
	// DX는 행 우선, FBX, ASE는 열 우선
	_node->mMetaData;

	_boneData.push_back(bone);

	// Mesh 정보 읽기
	ReadMeshData(_node, _index, _meshData, _scene);

	// 재귀
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
	// 매쉬가 없는 노드의 경우
	if (_node->mNumMeshes < 1)
	{
		return;
	}

	// 매쉬가 여러개 일 수도 있다.
	for (int i = 0; i < _node->mNumMeshes; i++)
	{
		asMesh mesh;

		mesh.m_name = _node->mName.C_Str();

		// scene 에서 매핑되는 매쉬의 인덱스
		int index = _node->mMeshes[i];
		const aiMesh* srcMesh = _scene->mMeshes[index];

		// 한 물체에 여려 매쉬가 있을 수 있다.
		// 그 매쉬마다 다른 매테리얼을 사용 할 수 있다.
		// 그 때 사용할 매핑 정보
		const aiMaterial* material = _scene->mMaterials[srcMesh->mMaterialIndex];
		// mesh->materialName = material->GetName().C_Str();

		// 정점 순회

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
