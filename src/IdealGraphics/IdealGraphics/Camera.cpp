#include "pch.h"
#include "Camera.h"

Camera::Camera()
	: m_view(Matrix::Identity)
	, m_proj(Matrix::Identity)
	, m_rotation()
	, m_position(0, 0, 0)
	, m_right(1, 0, 0)
	, m_up(0, 1, 0)
	, m_look(1, 0, 0)
	, m_fov(0.8)
	, m_aspectRatio(1)
	, m_nearZ(1)
	, m_farZ(1000)
	, m_isDirty(true)
{

}

Camera::~Camera()
{
}

bool Camera::Caculate()
{
	// 변경이 있을 때만
	if (false == m_isDirty)
	{
		return false;
	}

	// 각 벡터 계산
	m_right = Vector3::Transform(Vector3::Right, m_rotation);
	m_up = Vector3::Transform(Vector3::Up, m_rotation);
	m_look = Vector3::Transform(Vector3::Forward, m_rotation);

	m_right.Normalize();
	m_up.Normalize();
	m_look.Normalize();

	// 뷰 행렬
	m_view = 
		Matrix::CreateFromQuaternion(m_rotation) 
		* Matrix::CreateTranslation(m_position);
	m_view = m_view.Invert();

	// 프로젝션 행렬
	m_proj = Matrix::CreatePerspectiveFieldOfView(m_fov, m_aspectRatio, m_nearZ, m_farZ);

	m_isDirty = false;
	return true;
}

void Camera::SetNearZ(float _val)
{
	if (m_nearZ == _val)
	{
		return;
	}
	m_isDirty = true;
	m_nearZ = _val;
}

void Camera::SetFarZ(float _val)
{
	if (m_farZ == _val)
	{
		return;
	}
	m_isDirty = true;
	m_farZ = _val;
}

void Camera::SetFOV(float _val)
{
	if (m_fov == _val)
	{
		return;
	}

	m_isDirty = true;
	m_fov = _val;
}

void Camera::AddFOV(float _val)
{
	m_isDirty = true;
	m_fov += _val;
}

void Camera::RotateRight(float _val)
{
	m_isDirty = true;
	m_rotation *= Quaternion::CreateFromAxisAngle(Vector3::Up, _val);
	m_rotation.Normalize();
}

void Camera::RotateUp(float _val)
{
	m_isDirty = true;
	m_rotation *= Quaternion::CreateFromAxisAngle(m_right, _val);
	m_rotation.Normalize();
}

void Camera::MoveForward(float _val)
{
	m_isDirty = true;
	m_position += m_look *_val;
}

void Camera::MoveUp(float _val)
{
	m_isDirty = true;
	m_position.y += _val;
}

void Camera::Move(float _x, float _y, float _z)
{
	m_isDirty = true;
	m_position += { _x, _y, _z };
}

void Camera::SetPosition(float _x, float _y, float _z)
{
	if (m_position.x == _x
		&& m_position.y == _y
		&& m_position.z == _z)
	{
		return;
	}

	m_isDirty = true;
	m_position = { _x, _y, _z };
}

