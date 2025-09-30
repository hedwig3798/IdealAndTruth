#pragma once
#include "DXTK/SimpleMath.h"

using namespace DirectX::SimpleMath;

/// <summary>
/// ī�޶� ������ ���� Ŭ����
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
	/// ��� �� ���� ���
	/// </summary>
	/// <returns>����� �ߴ���</returns>
	bool Caculate();

	/// <summary>
	/// z ����
	/// </summary>
	/// <param name="_val"></param>
	void SetNearZ(float _val);
	void SetFarZ(float _val);

	/// <summary>
	/// fov ����
	/// </summary>
	/// <param name="_val"></param>
	void SetFOV(float _val);
	void AddFOV(float _val);

	/// <summary>
	/// ī�޶� ���� ȸ��
	/// </summary>
	/// <param name="_val">ȸ�� ���� (����)</param>
	void RotateRight(float _val);
	/// <summary>
	/// ī�޶� �¿� ȸ��
	/// </summary>
	/// <param name="_val">ȸ�� ���� (����)</param>
	void RotateUp(float _val);

	/// <summary>
	/// ���� �̵�
	/// </summary>
	/// <param name="_val">�Ÿ�</param>
	void MoveForward(float _val);
	/// <summary>
	/// ���� �̵�
	/// </summary>
	/// <param name="_val">�Ÿ�</param>
	void MoveUp(float _val);
	/// <summary>
	/// �� ��� �̵�
	/// </summary>
	/// <param name="_x">x</param>
	/// <param name="_y">y</param>
	/// <param name="_z">z</param>
	void Move(float _x, float _y, float _z);
	/// <summary>
	/// ��ġ ����
	/// </summary>
	/// <param name="_x">x</param>
	/// <param name="_y">y</param>
	/// <param name="_z">z</param>
	void SetPosition(float _x, float _y, float _z);
};

