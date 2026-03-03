#pragma once
#include "DXTK/SimpleMath.h"

using namespace DirectX::SimpleMath;
/// <summary>
/// 사용할 모든 요소가 있는 슈퍼 정점
/// </summary>
struct VertexSuperData
{
	Vector3 m_position;
	Vector3 m_normal;
	Vector3 m_tangent;
	Vector2 m_uv;
	Vector4 m_color;
	UINT m_boneIndices[4];
	float m_boneWeights[4];
};


/// <summary>
/// 위치만 있는 정점
/// </summary>
struct VertexPData
{
	Vector3 m_position;
};

/// <summary>
/// 위치와 텍스쳐가 있는 정점
/// </summary>
struct VertexPUData
{
	Vector3 m_position;
	Vector2 m_uv;
};

/// <summary>
/// 위치와 노말이 있는 정점
/// </summary>
struct VertexPNData
{
	Vector3 m_position;
	Vector3 m_normal;
};

/// <summary>
/// 위치 텍스쳐 노말이 있는 정점
/// </summary>
struct VertexPUNData
{
	Vector3 m_position;
	Vector3 m_normal;
	Vector2 m_uv;
};

struct VertexPCData
{
	Vector3 position;
	Vector4 color;
};

struct VertexPNTUData
{
	Vector3 m_position;
	Vector3 m_normal;
	Vector3 m_tangent;
	Vector3 m_binormal;
	Vector2 m_uv;
};
