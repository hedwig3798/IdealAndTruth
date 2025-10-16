#pragma once
#include <string.h>
#include <Assimp/Importer.hpp>
#include <Assimp/scene.h>
#include <Assimp/postprocess.h>
#include <Assimp/config.h>
#include <Assimp/cimport.h>
#include <assimp/types.h>

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
	aiMatrix4x4t<ai_real> m_relativeTransform;
	aiMatrix4x4t<ai_real> m_offsetMatrix;
	std::vector<std::string> m_meshName;
};

struct asMesh
{
	std::string m_name;
	aiMesh* m_mesh = nullptr;
	std::vector<aiVector3D> m_position;
	std::vector<aiVector3D> m_normal;
	std::vector<aiVector3D> m_textuerIndex;
	std::vector<int> m_index;

	aiMatrix4x4t<ai_real> m_worldTM;
};