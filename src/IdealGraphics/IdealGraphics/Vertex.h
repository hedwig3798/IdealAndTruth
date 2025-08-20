#pragma once
#include "D3D11Renderer.h"

/// <summary>
/// ��ġ�� �ִ� ����
/// </summary>
struct VertexPos
{
	Vector3 m_position;
};

/// <summary>
/// ��ġ�� �ؽ��İ� �ִ� ����
/// </summary>
struct VertexPosUV
{
	Vector3 m_position;
	Vector2 m_uv;
};

/// <summary>
/// ��ġ�� �븻�� �ִ� ����
/// </summary>
struct VertexPosNormal
{
	Vector3 m_position;
	Vector3 m_normal;
};

/// <summary>
/// ��ġ �ؽ��� �븻�� �ִ� ����
/// </summary>
struct VertexPosUVNormal
{
	Vector3 m_position;
	Vector2 m_uv;
	Vector3 m_normal;
};
typedef VertexPosUVNormal DVertex;