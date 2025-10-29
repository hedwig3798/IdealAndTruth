#pragma once
#include <string.h>
#include <Assimp/Importer.hpp>
#include <Assimp/scene.h>
#include <Assimp/postprocess.h>
#include <Assimp/config.h>
#include <Assimp/cimport.h>
#include <assimp/types.h>
#include <assimp/vector2.h>
#include "DXTK/SimpleMath.h"

using namespace DirectX::SimpleMath;

/// <summary>
/// 렌더러와 통일을 위한 정점
/// </summary>
struct VertexPUN
{
	Vector3 m_position;
	Vector3 m_normal;
	Vector2 m_uv;
};

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

	std::vector<VertexPUN> m_vertex;
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


