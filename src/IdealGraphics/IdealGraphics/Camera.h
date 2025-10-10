#pragma once
#include "DXTK/SimpleMath.h"
#include "ICamera.h"

using namespace DirectX::SimpleMath;

/// <summary>
/// ī�޶� ������ ���� Ŭ����
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

	void SetIndex(int _val) { m_containerIndex = _val; };

	/// <summary>
	/// ��� �� ���� ���
	/// </summary>
	/// <returns>����� �ߴ���</returns>
	bool Caculate();

	/// <summary>
	/// z ����
	/// </summary>
	/// <param name="_val"></param>
	void SetNearZ(float _val) override;
	void SetFarZ(float _val) override;

	/// <summary>
	/// fov ����
	/// </summary>
	/// <param name="_val"></param>
	void SetFOV(float _val) override;
	void AddFOV(float _val) override;

	/// <summary>
	/// ī�޶� ���� ȸ��
	/// </summary>
	/// <param name="_val">ȸ�� ���� (����)</param>
	void RotateRight(float _val) override;
	/// <summary>
	/// ī�޶� �¿� ȸ��
	/// </summary>
	/// <param name="_val">ȸ�� ���� (����)</param>
	void RotateUp(float _val) override;

	/// <summary>
	/// ���� �̵�
	/// </summary>
	/// <param name="_val">�Ÿ�</param>
	void MoveForward(float _val) override;
	/// <summary> 
	/// ���� �̵�
	/// </summary>
	/// <param name="_val">�Ÿ�</param>
	void MoveUp(float _val) override;
	/// <summary>
	/// �¿� �̵�
	/// </summary>
	/// <param name="_val">�Ÿ�</param>
	void MoveRight(float _val) override;
	/// <summary>
	/// �� ��� �̵�
	/// </summary>
	/// <param name="_x">x</param>
	/// <param name="_y">y</param>
	/// <param name="_z">z</param>
	void Move(float _x, float _y, float _z) override;
	/// <summary>
	/// ��ġ ����
	/// </summary>
	/// <param name="_x">x</param>
	/// <param name="_y">y</param>
	/// <param name="_z">z</param>
	void SetPosition(float _x, float _y, float _z) override;
};

