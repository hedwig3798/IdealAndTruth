#pragma once
#include "IRenderer.h"

/// <summary>
/// 카메라 정보를 담을 버퍼
/// </summary>
struct CameraBufferData
{
	Matrix m_view;
	Matrix m_proj;
	Vector3 m_camearaPos;

	float pad;
};

/// <summary>
/// 매쉬의 월드 값을 담을 버퍼
/// </summary>
struct WorldBufferData
{
	Matrix m_world;
	Matrix m_worldInvers;
};

/// <summary>
/// 직사광선 버퍼
/// </summary>
struct DirectionLightBufferData
{
	Vector3 m_direction;
	Vector3 m_color;
	float m_intensity;

	float pad;
};

/// <summary>
/// 점광원 버퍼
/// </summary>
struct PointLightBufferData
{
	Vector3 m_position;
	Vector3 m_color;
	float m_attenuation;
	float m_intensity;
	float m_range;

	Vector3 pad;
};

/// <summary>
/// 스포트라이트 버퍼
/// </summary>
struct SpotLightBufferData
{
	Vector3 m_direction;
	Vector3 m_position;
	Vector3 m_color;
	float m_attenuation;
	float m_inAngle;
	float m_outAngle;
	float m_intensity;

	Vector3 pad;
};

/// <summary>
/// 빛의 갯수 버퍼
/// </summary>
struct LightCountBuffer
{
	int m_numDirection;
	int m_numPoint;
	int m_numSpot;

	int pad;
};
