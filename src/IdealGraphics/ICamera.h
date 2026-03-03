#pragma once

class ICamera
{
public:
	/// <summary>
	/// z 설정
	/// </summary>
	/// <param name="_val"></param>
	virtual void SetNearZ(float _val) = 0;
	virtual void SetFarZ(float _val) = 0;

	/// <summary>
	/// fov 설정
	/// </summary>
	/// <param name="_val"></param>
	virtual void SetFOV(float _val) = 0;
	virtual void AddFOV(float _val) = 0;

	/// <summary>
	/// 종횡비 설정
	/// </summary>
	/// <param name="_val">값</param>
	virtual void SetAspectRatio(float _val) = 0;

	/// <summary>
	/// 카메라 상하 회전
	/// </summary>
	/// <param name="_val">회전 각도 (라디안)</param>
	virtual void RotateRight(float _val) = 0;
	/// <summary>
	/// 카메라 좌우 회전
	/// </summary>
	/// <param name="_val">회전 각도 (라디안)</param>
	virtual void RotateUp(float _val) = 0;

	/// <summary>
	/// 정면 이동
	/// </summary>
	/// <param name="_val">거리</param>
	virtual void MoveForward(float _val) = 0;
	/// <summary>
	/// 상하 이동
	/// </summary>
	/// <param name="_val">거리</param>
	virtual void MoveUp(float _val) = 0;
	/// <summary>
	/// 좌우 이동
	/// </summary>
	/// <param name="_val">거리</param>
	virtual void MoveRight(float _val) = 0;
	/// <summary>
	/// 값 기반 이동
	/// </summary>
	/// <param name="_x">x</param>
	/// <param name="_y">y</param>
	/// <param name="_z">z</param>
	virtual void Move(float _x, float _y, float _z) = 0;
	/// <summary>
	/// 위치 설정
	/// </summary>
	/// <param name="_x">x</param>
	/// <param name="_y">y</param>
	/// <param name="_z">z</param>
	virtual void SetPosition(float _x, float _y, float _z) = 0;
};
