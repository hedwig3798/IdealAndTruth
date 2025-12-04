#pragma once

#ifdef IDEALGRAPHICS_EXPORTS
#define IDEALGRAPHICS_DLL __declspec(dllexport)
#else
#define IDEALGRAPHICS_DLL __declspec(dllimport)
#endif

#include "RenameType.h"
#include "IdealError.h"
#include "ICamera.h"

#include "DXTK/SimpleMath.h"

#include <memory>
#include <vector>

using namespace DirectX::SimpleMath;

typedef const std::vector<unsigned char> CONST_FILE_STREAM;
typedef std::vector<unsigned char> FILE_STREAM;

struct ID3D11Device;
struct ID3D11DeviceContext;

class IRenderer
{
public:
	enum class LIGHT_TYPE
	{
		DIRECTION,
		POINT,
		SPOTLIGHT,
		END,
	};

	/// <summary>
	/// 빛을 정의하는 구조체
	/// 이 구조체 하나로 3가지 타입의 빛을 다 쓴다.
	/// 그냥 하나로 다 쓰는게 편하다
	/// 빛 종류를 바꾸는거도 편하다
	/// </summary>
	struct LightData
	{
		LIGHT_TYPE m_type;
		Vector3 m_direction;
		Vector3 m_position;
		Vector3 m_color;
		float m_attenuation;
		float m_inAngle;
		float m_outAngle;
		float m_intensity;
		float m_range;
	};

	/// <summary>
	/// 텍스쳐를 만들기 위해 필요한 데이터를 담은 구조체
	/// </summary>
	struct TextuerData
	{
		std::wstring m_name;
		CONST_FILE_STREAM& m_data;
	};
	
	/// <summary>
	/// 머테리얼을 만들기 위해 필요한 데이터를 담은 구조체
	/// 필요한 내용을 하나씩 추가 할 예정
	/// </summary>
	struct MaterialData
	{
		Vector4 m_color;
		TextuerData& m_albedo;
	};

	/// <summary>
	/// 렌더링 오브젝트 정보
	/// </summary>
	struct IRenderObject
	{
		std::wstring m_name;
		std::wstring m_vertexShader;
		std::wstring m_pixelShader;
		std::wstring m_mesh;
		std::wstring m_material;
		Matrix m_meshtransform;
		bool m_isDraw;

		IRenderObject() = default;

		// 정렬 시 복사를 방지하기 위해
		IRenderObject(const IRenderObject&) = default;
		IRenderObject& operator=(const IRenderObject&) = default;

		IRenderObject(IRenderObject&&) noexcept = default;
		IRenderObject& operator=(IRenderObject&&) noexcept = default;

		bool operator<(const IRenderObject& other) const noexcept 
		{
			if (m_isDraw != other.m_isDraw)
			{
				return m_isDraw;
			}

			int cmp = m_vertexShader.compare(other.m_vertexShader);
			if (0 != cmp)
			{
				return cmp < 0;
			}
			cmp = m_pixelShader.compare(other.m_pixelShader);
			if (0 != cmp)
			{
				return cmp < 0;
			}
			cmp = m_material.compare(other.m_material);
			if (0 != cmp)
			{
				return cmp < 0;
			}
			cmp = m_mesh.compare(other.m_mesh);
			if (0 != cmp)
			{
				return cmp < 0;
			}

			return false;
		}
	};

	/// <summary>
	/// 모델 데이터
	/// 각종 서브 매쉬의 집합체
	/// </summary>
	struct IModelObject
	{
		std::wstring m_name;
		std::vector<std::shared_ptr<IRenderObject>> m_renderObjects;
		Matrix m_world;
		bool m_isDraw;
	};

	/// <summary>
	/// 초기화 스테이트 구조체
	/// </summary>
	struct InitializeState
	{
		bool(*FileOpenCallbackFunc)(
			void* _fms
			, const std::wstring& _filename
			, OUT FILE_STREAM& fileData
		);

		void* m_fms;

		UINT m_renderVectorSize;
		UINT m_maxLightCount;

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

		/// <summary>
		/// 렌더 타겟 스테이트
		/// </summary>
		struct RenderTargetViewState
		{
			UINT m_width;
			UINT m_height;
			UINT m_mipLevel;
			UINT m_arraySize;
			UINT m_format;
			UINT m_sampleCount;
			UINT m_usage;
			UINT m_bindFlags;
			UINT m_viewDimension;
		} m_renderTargetView;
	};

	enum class VERTEX_TYPE
	{
		VertexSuper,
		VertexP,
		VertexPU,
		VertexPN,
		VertexPUN,
		VertexPC,
		END,
	};

	enum class GEOMETRY_TYPE
	{
		PLANE,
		CUBE,
		AXES,
		END,
	};



	IRenderer() {};
	virtual ~IRenderer() {};

	/// <summary>
	/// 렌더러 초기화
	/// </summary>
	/// <returns>성공 여부</returns>
	virtual IE Initialize(const InitializeState& _initalizeState, HWND _hwnd) = 0;

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
	/// 텍스쳐 데이터 생성
	/// </summary>
	/// <returns>오브젝트 GUID</returns>
	virtual IE CreateTexture(const TextuerData& _textuerData) = 0;

	/// <summary>
	/// 머테리얼 데이터 생성
	/// </summary>
	/// <returns>오브젝트 GUID</returns>
	virtual IE CreateMaterial(const std::wstring _name, const MaterialData& _material) = 0;

	/// <summary>
	/// 카메라 오브젝트 생성
	/// </summary>
	/// <returns>카메라 ID</returns>
	virtual std::weak_ptr<ICamera> CreateCamera() = 0;

	/// <summary>
	/// 메인 카메라 설정
	/// </summary>
	/// <param name="_cameraID">카메라 아이디</param>
	/// <returns>성공 여부 </returns>
	virtual IE SetCamera(std::weak_ptr<ICamera> _cameraID) = 0;

	/// <summary>
	/// 에니메이션 설정
	/// </summary>
	/// <param name="_meshID">매쉬 ID</param>
	/// <returns>성공 여부</returns>
	virtual IE SetAniamtion(int _meshID) = 0;

	/// <summary>
	/// Draw Call
	/// </summary>
	/// <returns>성공 여부</returns>
	virtual IE Draw() = 0;

	/// <summary>
	/// 렌더 타겟 클리어시 사용할 색
	/// </summary>
	/// <param name="_r">R</param>
	/// <param name="_g">G</param>
	/// <param name="_b">B</param>
	/// <param name="_a">A</param>
	/// <returns>성공 여부</returns>
	virtual IE SetBackgroundColor(float _r, float _g , float _b, float _a) = 0;

	/// <summary>
	/// 정점 셰이더 생성
	/// </summary>
	/// <param name="_name">셰이더 이름</param>
	/// <param name="_stream">셰이더 데이터 스트림</param>
	/// <returns>성공 여부</returns>
	virtual IE CreateVertexShader(VERTEX_TYPE _type, const std::wstring& _name) = 0;

	/// <summary>
	/// 픽셀 셰이더 생성
	/// </summary>
	/// <param name="_name">셰이더 이름</param>
	/// <param name="_stream">셰이더 데이터 스트림</param>
	/// <returns>성공 여부</returns>
	virtual IE CreatePixelShader(const std::wstring& _name) = 0;

	/// <summary>
	/// 정점 및 인덱스 버퍼 생성
	/// </summary>
	/// <param name="_name">매쉬 이름</param>
	/// <param name="_stream">데이터</param>
	/// <returns>성공 여부</returns>
	virtual IE CreateVertexIndexBuffer(const std::wstring& _name) = 0;

	/// <summary>
	/// 렌더링 오브젝트 추가
	/// </summary>
	/// <param name="_renderObject">렌더링 오브젝트</param>
	/// <returns>성공 여부</returns>
	[[deprecated("use AddModelObject instead")]]
	virtual IE AddRenderObject(std::shared_ptr<IRenderObject> _renderObject) = 0;

	/// <summary>
	/// 모델 오브젝트 추가
	/// </summary>
	/// <param name="_renderObject">모델 오브젝트</param>
	/// <returns>성공 여부</returns>
	virtual IE AddModelObject(std::shared_ptr<IModelObject> _modelObject) = 0;

	/// <summary>
	/// 렌더 영역 설정
	/// </summary>
	/// <param name="_w">높이</param>
	/// <param name="_h">너비</param>
	/// <returns>성공 여부</returns>
	virtual IE SetRenderSize(UINT _w, UINT _h) = 0;

	/// <summary>
	/// 한 씬에 존재할 수 있는 빛의 총 갯수
	/// </summary>
	/// <param name="_val">갯수</param>
	virtual void SetMaxLightCount(uint64_t _val) = 0;

	/// <summary>
	/// 씬에 빛 추가
	/// </summary>
	/// <param name="_name">빛 이름</param>
	/// <param name="_lightData">빛 데이터</param>
	/// <returns>성공 여부</returns>
	virtual IE AddLight(const std::wstring& _name, const LightData& _lightData) = 0;

	/// <summary>
	/// Imgui 시작을 위한 함수
	/// </summary>
	/// <param name="ImguiStartFunc">imgui start function</param>
	/// <returns>성공 여부</returns>
	virtual IE ImguiInitialize(
		bool(*ImguiStartFunc)(
			ID3D11Device* _device
			, ID3D11DeviceContext* _deviceContext
			)
	) = 0;
};

/// <summary>
/// 렌더러 생성
/// </summary>
/// <returns>렌더러</returns>
extern "C" IDEALGRAPHICS_DLL IRenderer* CreateD3D11Renderer(); 
