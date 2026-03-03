#pragma once
#include "DXTK/SimpleMath.h"
#include "ICamera.h"

using namespace DirectX::SimpleMath;

/// <summary>
/// 카메라 정보를 담은 클래스
/// </summary>
class Camera
	: public ICamera
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

	int m_containerIndex;

public:
	Camera();
	virtual ~Camera();

	const Matrix& GetViewMatrix() { return m_view; }
	const Matrix& GetProjMatrix() { return m_proj; }
	const Vector3& GetPosition() { return m_position; }

	void SetIndex(int _val) { m_containerIndex = _val; };

	/// <summary>
	/// 행렬 및 벡터 계산
	/// </summary>
	/// <returns>계산을 했는지</returns>
	bool Caculate();

	/// <summary>
	/// z 설정
	/// </summary>
	/// <param name="_val"></param>
	void SetNearZ(float _val) override;
	void SetFarZ(float _val) override;

	/// <summary>
	/// 종횡비 설정
	/// </summary>
	/// <param name="_val">값</param>
	void SetAspectRatio(float _val) override;

	/// <summary>
	/// fov 설정
	/// </summary>
	/// <param name="_val"></param>
	void SetFOV(float _val) override;
	void AddFOV(float _val) override;

	/// <summary>
	/// 카메라 상하 회전
	/// </summary>
	/// <param name="_val">회전 각도 (라디안)</param>
	void RotateRight(float _val) override;
	/// <summary>
	/// 카메라 좌우 회전
	/// </summary>
	/// <param name="_val">회전 각도 (라디안)</param>
	void RotateUp(float _val) override;

	/// <summary>
	/// 정면 이동
	/// </summary>
	/// <param name="_val">거리</param>
	void MoveForward(float _val) override;
	/// <summary> 
	/// 상하 이동
	/// </summary>
	/// <param name="_val">거리</param>
	void MoveUp(float _val) override;
	/// <summary>
	/// 좌우 이동
	/// </summary>
	/// <param name="_val">거리</param>
	void MoveRight(float _val) override;
	/// <summary>
	/// 값 기반 이동
	/// </summary>
	/// <param name="_x">x</param>
	/// <param name="_y">y</param>
	/// <param name="_z">z</param>
	void Move(float _x, float _y, float _z) override;
	/// <summary>
	/// 위치 설정
	/// </summary>
	/// <param name="_x">x</param>
	/// <param name="_y">y</param>
	/// <param name="_z">z</param>
	void SetPosition(float _x, float _y, float _z) override;
};

