#pragma once

enum class IdealError
{
	I_OK = 0,									// 정상
	NULL_POINTER_ACCESS = 1,					// 잘못된 포인터 참조
	HRESULT_ERROR = 2,							// hresult error
	CREATE_D3D_COMPONENT_FAIL = 3,				// D3D 구성요소 생성 실패
	CREATE_D3D_TEXTURE_FAIL = 4,				// D3D 텍스쳐 생성 실패
	CREATE_D3D_RENDER_TARGET_FAIL = 5,			// D3D 렌더 타겟 생성 실패
	CREATE_D3D_SRV_FAIL = 6,					// D3D SRV 생성 실패
	CREATE_D3D_BUFFER_FAIL = 7,					// D3D 버퍼 생성 실패
	CREATE_D3D_DEVICE_FAIL = 8,					// D3D 디바이스 생성 실패
	CREATE_D3D_SWAP_CHAIN_FAIL = 9,				// D3D 스왑 체인 생성 실패
	CREATE_D3D_DEPTH_STANCIL_FIAL = 10,			// D3D 뎁스 스텐실 생성 실패
	CREATE_D3D_RASTERIZERSTATE_FIAL = 11,		// D3D 레스터라이저 생성 실패
	STREAM_ERROR = 12,							// 올바르지 않은 스트림 
	WRONG_CAMERA = 13,							// 올바르지 않은 카메라
	MAPPING_SHADER_BUFFER_FAIL = 14,			// 버퍼를 셰이더에 매핑 실패 
	CREATE_D3D_BLOB_FAIL = 15,					// D3D blob 객체 생성 실패 
	CREATE_D3D_INPUT_LAYOUT_FAIL = 16,			// D3D IA 생성 실패 
	WRONG_TYPE = 17,							// 잘못된 타입 지정
	OUT_OF_POINTER_BOUNDARY = 18,				// 포인터가 지정한 범위를 넘어섬
	MAX_ELEMENT = 19,							// 더이상 해당 객체를 만들 수 없음
	CREATE_D3D_SHAEDER_RESOURCE_VIEW_FAIL = 20,	// SRV 생성 실패
	D3D_WRONG_BUFFER_ACCESS_FLAG = 21,			// D3D 버퍼에 접근 플래그 오류
	UNKNOWN_ERROR,								// 알 수 없는 에러
	END,
};

typedef IdealError IE;

#ifdef _DEBUG
#define IE_ASSERT(FUNC) \
{ \
	IE result;\
	result = FUNC; \
	if(IE::I_OK != result) \
	{ \
		std::cout \
		<< static_cast<int>(result) \
		<< " : " \
		<< "[File: " << __FILE__ \
		<< ", Line: " << __LINE__ \
		<< "] " \
		<< #FUNC \
		<< std::endl; \
		return; \
	} \
}
#else
#define IE_ASSERT(FUNC) FUNC;
#endif