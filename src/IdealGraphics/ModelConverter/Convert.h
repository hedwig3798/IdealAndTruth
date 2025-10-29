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

