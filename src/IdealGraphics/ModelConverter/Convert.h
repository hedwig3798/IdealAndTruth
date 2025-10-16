#pragma once
#include <string>
#include <map>
#include <queue>

#include <thread>
#include <condition_variable>
#include <mutex>

#include "AssimpType.h"

class Converter
{
private:
	// 멀티 스레드를 위한 변수들
	bool m_alljobFinish = false;
	std::condition_variable m_jobCv;
	std::mutex m_jobMutex;
	std::queue<std::string> m_jobQ;
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

	void ReadAssetFile(const std::vector<std::string>& _files);

private:
	void ImportDataWithThread();
	void ImportData(std::string _path);

	void ReadModelData(
		aiNode* _node
		, int _index
		, int _parent
		, std::vector<asMesh>& _meshData
		, std::vector<asBone>& _boneData
		, const aiScene* _scene
	);

	void ReadMeshData(
		aiNode* _node
		, int _bone
		, std::vector<asMesh>& _meshData
		, const aiScene* _scene
	);
};

