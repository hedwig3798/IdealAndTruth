#pragma once

enum class IdealError
{
	I_OK = 0,								// ����
	NULL_POINTER_ACCESS = 1,				// �߸��� ������ ����
	HRESULT_ERROR = 2,						// hresult error
	CREATE_D3D_COMPONENT_FAIL = 3,			// D3D ������� ���� ����
	CREATE_D3D_TEXTURE_FAIL = 4,			// D3D �ؽ��� ���� ����
	CREATE_D3D_RENDER_TARGET_FAIL = 5,		// D3D ���� Ÿ�� ���� ����
	CREATE_D3D_SRV_FAIL = 6,				// D3D SRV ���� ����
	CREATE_D3D_BUFFER_FAIL = 7,				// D3D ���� ���� ����
	CREATE_D3D_DEVICE_FAIL = 8,				// D3D ����̽� ���� ����
	CREATE_D3D_SWAP_CHAIN_FAIL = 9,			// D3D ���� ü�� ���� ����
	CREATE_D3D_DEPTH_STANCIL_FIAL = 10,		// D3D ���� ���ٽ� ���� ����
	CREATE_D3D_RASTERIZERSTATE_FIAL = 11,	// D3D �����Ͷ����� ���� ����
	STREAM_ERROR = 12,						// �ùٸ��� ���� ��Ʈ�� 
	WRONG_CAMERA = 13,						// �ùٸ��� ���� ī�޶�
	MAPPING_SHADER_BUFFER_FAIL = 14,		// ���۸� ���̴��� ���� ���� 
	CREATE_D3D_BLOB_FAIL = 15,				// D3D blob ��ü ���� ���� 
	CREATE_D3D_INPUT_LAYOUT_FAIL = 16,		// D3D IA ���� ���� 
	WRONG_TYPE = 17,						// �߸��� Ÿ�� ����
	OUT_OF_POINTER_BOUNDARY = 18,			// �����Ͱ� ������ ������ �Ѿ
	UNKNOWN_ERROR,							// �� �� ���� ����
	END,
};

typedef IdealError IE;

#ifdef _DEBUG
#define IE_ASSERT(FUNC, MSG) \
{ \
	IE result;\
	result = FUNC; \
	if(IE::I_OK != result) \
	{ \
		std::cout << static_cast<int>(result) << " : " << MSG << std::endl; \
		return; \
	} \
}
#else
#define IE_ASSERT(FUNC, MSG) FUNC;
#endif