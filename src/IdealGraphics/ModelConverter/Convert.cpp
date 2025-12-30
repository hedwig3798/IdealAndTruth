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

		ConvertJob job = m_jobQ.front();
		m_jobQ.pop();
		lock.unlock();

		// 실제 작업 시작
		Assimp::Importer* importer = new Assimp::Importer();
		importer->SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, 0);

		unsigned int flag = aiProcess_ConvertToLeftHanded   // 왼손 좌표계 로드, uv 시작점 좌상단, cw 로드 (시계 방향)
			| aiProcess_MakeLeftHanded
			//| aiProcess_FlipUVs 
			//| aiProcess_FlipWindingOrder 
			| aiProcess_JoinIdenticalVertices // 동일한 위치의 정점을 하나의 정점으로 병합 (정점 데이터의 중복을 제거하고 모델 최적화)
			| aiProcess_Triangulate  // 삼각형 메쉬 형태로 구성
			| aiProcess_GenUVCoords  // UV 정보 생성
			| aiProcess_GenNormals  // Normal 정보 생성
			| aiProcess_CalcTangentSpace; // tangent 정보 생성

		// 일단 넣어는 놓는다
		// 나중에 수정해야됨
		if (true == job.m_hasBone)
		{
			// 계층구조와 애니메이션 정보를 제거하고 정점 정보를 구성
			flag |= aiProcess_PreTransformVertices;
		}

		// 경로의 파일을 읽은 씬 생성
		const aiScene* scene = importer->ReadFile(
			job.m_path,
			flag
		);

		// 최종 모델 구조체
		asModel model;

		// 파일 이름 가져오기
		size_t pos1 = job.m_path.find_last_of("/\\");
		std::string filename;
		if (std::string::npos == pos1)
		{
			filename = job.m_path;
		}
		filename = job.m_path.substr(pos1 + 1);

		// 확장자 제거
		size_t pos2 = filename.find_last_of('.');
		if (std::string::npos == pos2)
		{
			model.m_name = filename;
		}
		model.m_name = filename.substr(0, pos2);

		// 모델 데이터 읽기
		ReadModelData(
			scene->mRootNode
			, 0
			, -1
			, model
			, scene
		);

		// 모델 데이터 쓰기
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
	// 일단 노드를 전부 본 이라고 가정한다.
	asBone bone;
	bone.m_index = _index;
	bone.m_parentIndex = _parent;
	bone.m_name = _node->mName.C_Str();

	// 부모 기준 행렬
	aiMatrix4x4t<float> transform = _node->mTransformation;
	// DX는 행 우선, FBX, ASE는 열 우선
	_node->mMetaData;

	// _model.m_bones.push_back(bone);

	// Mesh 정보 읽기
	ReadMeshData(_node, _index, _model, _scene);

	// 재귀
	for (unsigned int i = 0; i < _node->mNumChildren; i++)
	{
		ReadModelData(
			_node->mChildren[i]
			, 0 // 나중에 본 파싱할 때 이 부분 수정해야됨
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
	// 매쉬가 없는 노드의 경우
	if (_node->mNumMeshes < 1)
	{
		return;
	}

	// 매쉬가 여러개 일 수도 있다.
	for (unsigned int i = 0; i < _node->mNumMeshes; i++)
	{
		asMesh mesh;

		mesh.m_name = _node->mName.C_Str();

		// scene 에서 매핑되는 매쉬의 인덱스
		int index = _node->mMeshes[i];
		const aiMesh* srcMesh = _scene->mMeshes[index];

		// 정점 순회
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

			// tangent
			if (srcMesh->HasTangentsAndBitangents())
			{
				mesh.m_vertex[v].m_tangent.x = srcMesh->mTangents[v].x;
				mesh.m_vertex[v].m_tangent.y = srcMesh->mTangents[v].x;
				mesh.m_vertex[v].m_tangent.z = srcMesh->mTangents[v].z;
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
		file.write(reinterpret_cast<const char*>(itr.m_vertex.data()), vertexCount * sizeof(VertexPUTN));

		// index
		uint64_t indexCount = itr.m_index.size();
		file.write(reinterpret_cast<char*>(&indexCount), sizeof(uint64_t));
		file.write(reinterpret_cast<const char*>(itr.m_index.data()), indexCount * sizeof(int));
	}
}
