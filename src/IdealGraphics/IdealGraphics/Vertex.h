#pragma once
#include "D3D11Renderer.h"

// �� ���� ��ũ��Ʈ�� �� �� ��������?
// ���� ����ü�� ��� ���� �� �� ��εȴ�.

/// <summary>
/// ����� ��� ��Ұ� �ִ� ���� ����
/// </summary>
struct VertexSuper
{
	Vector3 m_position;  
	Vector3 m_normal;  
	Vector3 m_tangent;  
	Vector2 m_uv; 
	Vector4 m_color;
	UINT m_boneIndices[4];
	float m_boneWeights[4];
	static D3D11_INPUT_ELEMENT_DESC m_IADesc[7];
};


/// <summary>
/// ��ġ�� �ִ� ����
/// </summary>
struct VertexP
{
	Vector3 m_position;
	static D3D11_INPUT_ELEMENT_DESC m_IADesc[1];
};

/// <summary>
/// ��ġ�� �ؽ��İ� �ִ� ����
/// </summary>
struct VertexPU
{
	Vector3 m_position;
	Vector2 m_uv;
	static D3D11_INPUT_ELEMENT_DESC m_IADesc[2];
};

/// <summary>
/// ��ġ�� �븻�� �ִ� ����
/// </summary>
struct VertexPN
{
	Vector3 m_position;
	Vector3 m_normal;
	static D3D11_INPUT_ELEMENT_DESC m_IADesc[2];
};

/// <summary>
/// ��ġ �ؽ��� �븻�� �ִ� ����
/// </summary>
struct VertexPUN
{
	Vector3 m_position;
	Vector3 m_normal;
	Vector2 m_uv;
	static D3D11_INPUT_ELEMENT_DESC m_IADesc[3];
};
typedef VertexPUN DVertex;

struct VertexPC
{
	Vector3 position;
	Vector4 color;
	static D3D11_INPUT_ELEMENT_DESC m_IADesc[2];
};
