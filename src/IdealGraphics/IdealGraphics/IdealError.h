#pragma once

enum class IdealError
{
	I_OK						= 0,	// 정상
	NULL_POINTER_ACCESS			= 1,	// 잘못된 포인터 참조
	HRESULT_ERROR				= 2,	// hresult error
	CREATE_D3D_COMPONENT_FAIL	= 3,	// D3D 구성요소 생성 실패

	UNKNOWN_ERROR,				// 알 수 없는 에러
	END,
};

typedef IdealError IE;