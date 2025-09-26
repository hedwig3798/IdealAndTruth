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

	void SetNearZ(float _val);
	void SetFarZ(float _val);

	void SetFOV(float _val);
	void AddFOV(float _val);

	void RotateRight(float _val);
	void RotateUp(float _val);

	void MoveForward(float _val);
	void MoveUp(float _val);
	void Move(float _x, float _y, float _z);
	void SetPosition(float _x, float _y, float _z);
};

