#pragma once
#include "DXTK/SimpleMath.h"

using namespace DirectX::SimpleMath;

/// <summary>
/// 카메라 정보를 담은 클래스
/// </summary>
class Camera
{
private:
	// camera matrix
	Matrix m_view;
	Matrix m_proj;

	// camera movement
	Quaternion m_rotation;
	Vector3 m_position;

	// camera vector
	Vector3 m_right;
	Vector3 m_up;
	Vector3 m_look;

	// fov
	float m_fov;

	float m_aspectRatio;

	float m_nearZ;
	float m_farZ;

	// when true: need to calcualte matrix
	bool m_isDirty;

public:
	Camera();
	virtual ~Camera();

	const Matrix& GetViewMatrix() { return m_view; }
	const Matrix& GetProjMatrix() { return m_proj; }

	/// <summary>
	/// 행렬 및 벡터 계산
	/// </summary>
	/// <returns>계산을 했는지</returns>
	bool Caculate();

	/// <summary>
	/// z 설정
	/// </summary>
	/// <param name="_val"></param>
	void SetNearZ(float _val);
	void SetFarZ(float _val);

	/// <summary>
	/// fov 설정
	/// </summary>
	/// <param name="_val"></param>
	void SetFOV(float _val);
	void AddFOV(float _val);

	/// <summary>
	/// 카메라 상하 회전
	/// </summary>
	/// <param name="_val">회전 각도 (라디안)</param>
	void RotateRight(float _val);
	/// <summary>
	/// 카메라 좌우 회전
	/// </summary>
	/// <param name="_val">회전 각도 (라디안)</param>
	void RotateUp(float _val);

	/// <summary>
	/// 정면 이동
	/// </summary>
	/// <param name="_val">거리</param>
	void MoveForward(float _val);
	/// <summary>
	/// 상하 이동
	/// </summary>
	/// <param name="_val">거리</param>
	void MoveUp(float _val);
	/// <summary>
	/// 값 기반 이동
	/// </summary>
	/// <param name="_x">x</param>
	/// <param name="_y">y</param>
	/// <param name="_z">z</param>
	void Move(float _x, float _y, float _z);
	/// <summary>
	/// 위치 설정
	/// </summary>
	/// <param name="_x">x</param>
	/// <param name="_y">y</param>
	/// <param name="_z">z</param>
	void SetPosition(float _x, float _y, float _z);
};

