#pragma once

class ICamera
{
public:
	/// <summary>
	/// z ����
	/// </summary>
	/// <param name="_val"></param>
	virtual void SetNearZ(float _val) = 0;
	virtual void SetFarZ(float _val) = 0;

	/// <summary>
	/// fov ����
	/// </summary>
	/// <param name="_val"></param>
	virtual void SetFOV(float _val) = 0;
	virtual void AddFOV(float _val) = 0;

	/// <summary>
	/// ��Ⱦ�� ����
	/// </summary>
	/// <param name="_val">��</param>
	virtual void SetAspectRatio(float _val) = 0;

	/// <summary>
	/// ī�޶� ���� ȸ��
	/// </summary>
	/// <param name="_val">ȸ�� ���� (����)</param>
	virtual void RotateRight(float _val) = 0;
	/// <summary>
	/// ī�޶� �¿� ȸ��
	/// </summary>
	/// <param name="_val">ȸ�� ���� (����)</param>
	virtual void RotateUp(float _val) = 0;

	/// <summary>
	/// ���� �̵�
	/// </summary>
	/// <param name="_val">�Ÿ�</param>
	virtual void MoveForward(float _val) = 0;
	/// <summary>
	/// ���� �̵�
	/// </summary>
	/// <param name="_val">�Ÿ�</param>
	virtual void MoveUp(float _val) = 0;
	/// <summary>
	/// �¿� �̵�
	/// </summary>
	/// <param name="_val">�Ÿ�</param>
	virtual void MoveRight(float _val) = 0;
	/// <summary>
	/// �� ��� �̵�
	/// </summary>
	/// <param name="_x">x</param>
	/// <param name="_y">y</param>
	/// <param name="_z">z</param>
	virtual void Move(float _x, float _y, float _z) = 0;
	/// <summary>
	/// ��ġ ����
	/// </summary>
	/// <param name="_x">x</param>
	/// <param name="_y">y</param>
	/// <param name="_z">z</param>
	virtual void SetPosition(float _x, float _y, float _z) = 0;
};
