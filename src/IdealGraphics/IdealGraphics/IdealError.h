#pragma once

enum class IdealError
{
	I_OK							= 0,	// ����
	NULL_POINTER_ACCESS				= 1,	// �߸��� ������ ����
	HRESULT_ERROR					= 2,	// hresult error
	CREATE_D3D_COMPONENT_FAIL		= 3,	// D3D ������� ���� ����
	CREATE_D3D_TEXTURE_FAIL			= 4,	// D3D �ؽ��� ���� ����
	CREATE_D3D_RENDER_TARGET_FAIL = 5,	// D3D ���� Ÿ�� ���� ����
	CREATE_D3D_SRV_FAIL = 6,	// D3D SRV ���� ����
	CREATE_D3D_BUFFER = 7,	// D3D SRV ���� ����
	UNKNOWN_ERROR,				// �� �� ���� ����
	END,
};

typedef IdealError IE;