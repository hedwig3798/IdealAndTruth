#pragma once
#include <string>
#include <map>
#include <queue>

#include <thread>
#include <condition_variable>
#include <mutex>

#include <Assimp/Importer.hpp>
#include <Assimp/scene.h>
#include <Assimp/postprocess.h>
#include <Assimp/config.h>
#include <Assimp/cimport.h>
#include <assimp/types.h>
#include <assimp/vector2.h>

#include "VertexType.h"

/// <summary>
/// Assimp 를 통해서 얻어낸 정보들을 메모리에 저장해둔다.
/// 이 정보들로 customData 파일을 만든다.
/// </summary>
struct asBone
{
	std::string m_name;
	int m_numChildren = 0;
	int m_index = -1;
	int m_parentIndex = -1;
	aiMatrix4x4t<float> m_relativeTransform;
	aiMatrix4x4t<float> m_offsetMatrix;
};

struct asMesh
{
	std::string m_name;

	std::vector<VertexPNTUData> m_vertex;
	std::vector<float> m_bones;
	std::vector<float> m_weight;
	std::vector<int> m_index;
};

struct asModel
{
	std::string m_name;
	std::vector<asMesh> m_meshes;
	aiMatrix4x4t<float> m_worldTM;
};

class Converter
{
public:
	struct ConvertJob
	{
		std::string m_path;
		bool m_isStatic;
		bool m_hasBone;
	};

private:


	// 멀티 스레드를 위한 변수들
	bool m_alljobFinish = false;
	std::condition_variable m_jobCv;
	std::mutex m_jobMutex;
	std::queue<ConvertJob> m_jobQ;
	std::vector<std::thread> m_threads;

	std::string m_outputPath;
	std::string m_extension;

	int m_threadCount;

public:
	Converter();
	~Converter();

	void SetThreadCount(int _val) { m_threadCount = _val; };
	void SetOutputPath(std::string _val) { m_outputPath = _val; };
	void SetExtension(std::string _val) { m_extension = _val; };

	void ReadAssetFile(const std::vector<ConvertJob>& _data);

private:
	void ImportDataWithThread();


	void ReadModelData(
		aiNode* _node
		, int _index
		, int _parent
		, OUT asModel& _model
		, const aiScene* _scene
	);

	void ReadMeshData(
		aiNode* _node
		, int _bone
		, OUT asModel& _model
		, const aiScene* _scene
	);

	void WriteModel(const asModel& _model);
};

