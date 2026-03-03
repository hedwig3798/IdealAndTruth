#pragma once
#include "D3D11Renderer.h"
#include "VertexType.h"
// 이 내용 스크립트로 뺄 수 없으려나?
// 정점 구조체를 어떻게 정의 할 지 고민된다.

/// <summary>
/// 사용할 모든 요소가 있는 슈퍼 정점
/// </summary>
struct VertexSuper
{
	VertexSuperData m_data;
	UINT m_boneIndices[4];
	float m_boneWeights[4];
	static D3D11_INPUT_ELEMENT_DESC m_IADesc[7];
};


/// <summary>
/// 위치만 있는 정점
/// </summary>
struct VertexP
{
	VertexPData m_data;
	static D3D11_INPUT_ELEMENT_DESC m_IADesc[1];
};

/// <summary>
/// 위치와 텍스쳐가 있는 정점
/// </summary>
struct VertexPU
{
	VertexPUData m_data;
	static D3D11_INPUT_ELEMENT_DESC m_IADesc[2];
};

/// <summary>
/// 위치와 노말이 있는 정점
/// </summary>
struct VertexPN
{
	VertexPNData m_data;
	static D3D11_INPUT_ELEMENT_DESC m_IADesc[2];
};

/// <summary>
/// 위치 텍스쳐 노말이 있는 정점
/// </summary>
struct VertexPUN
{
	VertexPUNData m_data;
	static D3D11_INPUT_ELEMENT_DESC m_IADesc[3];
};
typedef VertexPUN DVertex;

struct VertexPC
{
	VertexPCData m_data;
	static D3D11_INPUT_ELEMENT_DESC m_IADesc[2];
};

struct VertexPNTU
{
	VertexPNTUData m_data;
	static D3D11_INPUT_ELEMENT_DESC m_IADesc[5];
};
