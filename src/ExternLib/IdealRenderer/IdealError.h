#pragma once

enum class IdealError
{
	I_OK						= 0,	// ����
	NULL_POINTER_ACCESS			= 1,	// �߸��� ������ ����
	HRESULT_ERROR				= 2,	// hresult error
	CREATE_D3D_COMPONENT_FAIL	= 3,	// D3D ������� ���� ����

	UNKNOWN_ERROR,				// �� �� ���� ����
	END,
};

typedef IdealError IE;