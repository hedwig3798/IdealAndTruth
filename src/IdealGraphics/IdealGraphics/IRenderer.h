#pragma once
#include "RenameType.h"
#include "IdealError.h"

class IRenderer
{
public:

	/// <summary>
	/// 초기화 스테이트 구조체
	/// </summary>
	struct InitializeState
	{
		/// <summary>
		/// 디바이스 플래그
		/// </summary>
		struct Device
		{
			UINT m_deviceFlags;
		} m_device;

		/// <summary>
		/// 스왑 체인 스테이트
		/// </summary>
		struct SwapCahin
		{
			UINT m_width;
			UINT m_height;
			UINT m_refreshRateNumerator;
			UINT m_refreshRateDenominator;
			UINT m_format;
			UINT m_scanlineOrdering;
			UINT m_scaling;

			UINT m_sampleCount;
			UINT m_sampleQuality;

			UINT m_bufferCount;

			UINT m_swapEffect;
			UINT m_flag;
			bool m_isWindowed;
		} m_swapChain;

		/// <summary>
		/// 뎁스 스텐실 스테이트
		/// </summary>
		struct DepthStancil
		{
			UINT m_width;
			UINT m_height;
			UINT m_mipLevel;
			UINT m_arraySize;
			UINT m_format;
			UINT m_sampleCount;
			UINT m_sampleQuality;
			UINT m_usage;
		} m_depthStancil;

		/// <summary>
		/// 레스터라이저 스테이트
		/// </summary>
		struct RaseterizerState
		{
			UINT m_fillMode;
			UINT m_cullMode;
			bool m_isFrontCCW;
			bool m_isDepthClip;
		} m_rasterizer;
	};

	IRenderer() {};
	virtual ~IRenderer() {};

	/// <summary>
	/// 렌더러 초기화
	/// </summary>
	/// <returns>성공 여부</returns>
	virtual IE Initialize(const InitializeState& _initalizeState) = 0;

	/// <summary>
	/// 정적 매쉬 데이터 임포트
	/// </summary>
	/// <returns>오브젝트 ID</returns>
	virtual IE ImportStaticMesh() = 0;

	/// <summary>
	/// 본 매쉬 데이터 임포트
	/// </summary>
	/// <returns>오브젝트 GUID</returns>
	virtual IE ImportBoneMesh() = 0;

	/// <summary>
	/// 애니메이션 데이터 임포트
	/// </summary>
	/// <returns>오브젝트 GUID</returns>
	virtual IE ImportAnimation() = 0;

	/// <summary>
	/// 텍스쳐 데이터 임포트
	/// </summary>
	/// <returns>오브젝트 GUID</returns>
	virtual IE ImportTexture() = 0;

	/// <summary>
	/// 머테리얼 데이터 임포트
	/// </summary>
	/// <returns>오브젝트 GUID</returns>
	virtual IE ImportMaterial() = 0;

	/// <summary>
	/// 카메라 오브젝트 생성
	/// </summary>
	/// <returns>카메라 ID</returns>
	virtual IE CreateCamera() = 0;

	/// <summary>
	/// 메인 카메라 설정
	/// </summary>
	/// <param name="_cameraID">카메라 아이디</param>
	/// <returns>성공 여부 </returns>
	virtual IE SetCamera(int _cameraID) = 0;

	/// <summary>
	/// 정적 매쉬 생성
	/// </summary>
	/// <param name="_GUID">매쉬 데이터 GUID</param>
	/// <returns>매쉬 ID</returns>
	virtual IE CreateStaticMesh(int _GUID) = 0;

	/// <summary>
	/// 본 매쉬 생성
	/// </summary>
	/// <param name="_GUID">본 매쉬 데이터</param>
	/// <returns>매쉬 ID</returns>
	virtual IE CreateBoneMesh(int _GUID) = 0;

	/// <summary>
	/// 에니메이션 설정
	/// </summary>
	/// <param name="_meshID">매쉬 ID</param>
	/// <returns>성공 여부</returns>
	virtual IE SetAniamtion(int _meshID) = 0;

	/// <summary>
	/// 매쉬의 재질 설정
	/// </summary>
	/// <param name="_mehsID">매쉬 ID</param>
	/// <returns>성공 여부</returns>
	virtual IE SetMaterial(int _mehsID) = 0;

	/// <summary>
	/// 머테리얼에 텍스쳐 설정
	/// </summary>
	/// <param name="_materialID">머테리얼 ID</param>
	/// <param name="_slot">머테리얼의 슬릇</param>
	/// <returns>성공 여부</returns>
	virtual IE SetTexture(int _materialID, int _slot = 0) = 0;

	/// <summary>
	/// Draw Call
	/// </summary>
	/// <returns>성공 여부</returns>
	virtual IE Draw() = 0;
};

