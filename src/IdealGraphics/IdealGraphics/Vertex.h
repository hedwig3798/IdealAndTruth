#pragma once
#include "D3D11Renderer.h"

/// <summary>
/// 위치만 있는 정점
/// </summary>
struct VertexPos
{
	Vector3 m_position;
};

/// <summary>
/// 위치와 텍스쳐가 있는 정점
/// </summary>
struct VertexPosUV
{
	Vector3 m_position;
	Vector2 m_uv;
};

/// <summary>
/// 위치와 노말이 있는 정점
/// </summary>
struct VertexPosNormal
{
	Vector3 m_position;
	Vector3 m_normal;
};

/// <summary>
/// 위치 텍스쳐 노말이 있는 정점
/// </summary>
struct VertexPosUVNormal
{
	Vector3 m_position;
	Vector2 m_uv;
	Vector3 m_normal;
};
typedef VertexPosUVNormal DVertex;