#pragma once

enum class IdealError
{
	I_OK = 0,								// 정상
	NULL_POINTER_ACCESS = 1,				// 잘못된 포인터 참조
	HRESULT_ERROR = 2,						// hresult error
	CREATE_D3D_COMPONENT_FAIL = 3,			// D3D 구성요소 생성 실패
	CREATE_D3D_TEXTURE_FAIL = 4,			// D3D 텍스쳐 생성 실패
	CREATE_D3D_RENDER_TARGET_FAIL = 5,		// D3D 렌더 타겟 생성 실패
	CREATE_D3D_SRV_FAIL = 6,				// D3D SRV 생성 실패
	CREATE_D3D_BUFFER_FAIL = 7,				// D3D 버퍼 생성 실패
	CREATE_D3D_DEVICE_FAIL = 8,				// D3D 디바이스 생성 실패
	CREATE_D3D_SWAP_CHAIN_FAIL = 9,			// D3D 스왑 체인 생성 실패
	CREATE_D3D_DEPTH_STANCIL_FIAL = 10,		// D3D 뎁스 스텐실 생성 실패
	CREATE_D3D_RASTERIZERSTATE_FIAL = 11,	// D3D 레스터라이저 생성 실패
	ALREADY_EXIST = 12,						// 생성하려는 무언가가 이미 있음 
	STREAM_ERROR = 13,						// 올바르지 않은 스트림 
	UNKNOWN_ERROR,							// 알 수 없는 에러
	END,
};

typedef IdealError IE;