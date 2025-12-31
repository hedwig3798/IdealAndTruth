#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl\client.h>
#include <map>
#include <string>
#include <unordered_map>
#include <algorithm>

#include "IRenderer.h"
#include "windows.h"
#include "CustomAllocator/IAllocator.h"
#include "Camera.h"
#include "Vertex.h"
#include "CustomBuffer.h"
#include "DXTK/DDSTextureLoader.h"
#include "DXTK/WICTextureLoader.h"
#include "DXTK/DirectXTex.h"

using Microsoft::WRL::ComPtr;

class D3D11Renderer :
	public IRenderer
{
private:
	/// <summary>
	/// 빛 데이터를 그래픽 카드에 넣기 위한 구조체
	/// </summary>
	struct LightBuffer
	{
		ComPtr<ID3D11Buffer> m_buffer;
		ComPtr<ID3D11ShaderResourceView> m_srv;
		bool isDirty;
	};

	/// <summary>
	/// 머테리얼을 이루는 텍스쳐가 있는 구조체
	/// </summary>
	struct Material
	{
		ComPtr<ID3D11ShaderResourceView> m_albedo;
		ComPtr<ID3D11ShaderResourceView> m_normal;
		ComPtr<ID3D11ShaderResourceView> m_roughness;
		ComPtr<ID3D11ShaderResourceView> m_metalic;
	};

	/// <summary>
	/// 가장 최근 렌더한 오브젝트의 정보
	/// 달라진거만 새로 바인딩 하기 위함
	/// </summary>
	struct RenderStruct
	{
		ComPtr<ID3D11VertexShader> m_vs = nullptr;
		ComPtr<ID3D11InputLayout> m_ia = nullptr;
		ComPtr<ID3D11PixelShader> m_ps = nullptr;
		ComPtr<ID3D11Buffer> m_vb = nullptr;
		ComPtr<ID3D11Buffer> m_ib = nullptr;
	};

private:
	// 윈도우 핸들러
	HWND m_hwnd;

	void* m_fms;

	// D3D Components
	ComPtr<ID3D11Device> m_device;
	ComPtr<ID3D11DeviceContext> m_deviceContext;
	ComPtr<IDXGISwapChain> m_swapChain;
	ComPtr<ID3D11RenderTargetView> m_finalRenderTargetView;
	ComPtr<ID3D11Texture2D> m_depthStancilBuffer;
	ComPtr<ID3D11DepthStencilView> m_depthStancilView;
	ComPtr<ID3D11RasterizerState> m_rasterizerState;
	ComPtr<ID3D11Texture2D> m_finalTexture2D;
	ComPtr<ID3D11ShaderResourceView> m_finalSRV;

	// Shader map
	std::unordered_map<std::wstring, std::pair<ComPtr<ID3D11VertexShader>, ComPtr<ID3D11InputLayout>>> m_vsMap;
	std::unordered_map<std::wstring, ComPtr<ID3D11PixelShader>> m_psMap;

	// Buffer map
	std::unordered_map<std::wstring, std::pair<ComPtr<ID3D11Buffer>, UINT>> m_vBuffer;
	std::unordered_map<std::wstring, std::pair<ComPtr<ID3D11Buffer>, UINT>> m_iBuffer;

	// texture map
	std::unordered_map<std::wstring, ComPtr<ID3D11ShaderResourceView>> m_textuerMap;
	// material map
	std::unordered_map<std::wstring, Material> m_materialMap;

	// light map
	std::unordered_map<std::wstring, std::pair<LIGHT_TYPE, size_t>> m_lightMap;
	std::vector<DirectionLightBufferData> m_dirctionLightVector;
	std::vector<PointLightBufferData> m_pointLightVector;
	std::vector<SpotLightBufferData> m_spotLightVector;

	LightBuffer m_dirctionLightBuffer;
	LightBuffer m_pointLightBuffer;
	LightBuffer m_spotLightBuffer;

	uint64_t m_maxLightCount;

	// input layout buffer
	std::vector<ComPtr<ID3D11InputLayout>> m_iaBuffer;

	D3D_FEATURE_LEVEL m_featureLevel;

	// wvp matrix
	Matrix m_world;

	// custom allocator
	std::map<size_t, IAllocator*>m_allocators;

	// default background color
	float m_defaultBG[4];

	// cameras
	std::vector<std::shared_ptr<Camera>> m_cameras;
	std::weak_ptr<Camera> m_mainCamera;
	ComPtr<ID3D11Buffer> m_cameraCBuffer;

	// render object vector
	std::vector<std::shared_ptr<IRenderObject>> m_renderVector;

	RenderStruct m_currentRenderSet;

	ComPtr<ID3D11Buffer> m_worldBuffer;

	// light count buffer
	ComPtr<ID3D11Buffer> m_lightCountBuffer;
	LightCountBuffer m_lightCountData;

	UINT m_renderWidth;
	UINT m_renderHight;

	// skybox component
	RenderStruct m_skyRenderSet;
	UINT m_skyIndexSize;
	ComPtr<ID3D11ShaderResourceView> m_skyTextuer;

	// default textuer
	ComPtr<ID3D11ShaderResourceView> m_defaultDiffuse;
	ComPtr<ID3D11ShaderResourceView> m_defaultNormal;
	ComPtr<ID3D11ShaderResourceView> m_defaultRoughess;
	ComPtr<ID3D11ShaderResourceView> m_defaultMetalic;

public:
	D3D11Renderer();
	virtual ~D3D11Renderer();

private:

#pragma region Create
	/// <summary>
	/// D3D11 Device 와 Device Context를 생성
	/// </summary>
	/// <param name="_device">Device 생성을 위한 값</param>
	/// <returns>성공 여부</returns>
	IE CreateD3D11DeviceContext(const InitializeState::Device& _device);

	/// <summary>
	/// swap chain 생성
	/// </summary>
	/// <param name="_swapChain">SwapCahin 생성을 위한 값</param>
	/// <returns>성공 여부</returns>
	IE CreateSwapChain(const InitializeState::SwapCahin& _swapChain);

	/// <summary>
	/// 최종 화면을 그릴 렌더 타겟 뷰 생성
	/// </summary>
	/// <returns>성공 여부</returns>
	IE CreateFinalRenterTargetView();

	/// <summary>
	/// depth stancil 버퍼와 뷰 생성
	/// </summary>
	/// <param name="_depthStancil">생성을 위한 값</param>
	/// <returns>성공 여부</returns>
	IE CreateDepthStencilBufferAndView(const InitializeState::DepthStancil& _depthStancil);

	/// <summary>
	/// 뷰포트를 만들어 레스터라이져에 바인드
	/// </summary>
	IE CreateViewPort();

	/// <summary>
	/// 레스터라이저 설정
	/// </summary>
	/// <param name="_rasterizer">설정 값</param>
	/// <returns>성공 여부</returns>
	IE CreateRasterizerState(const InitializeState::RaseterizerState& _rasterizer);

	/// <summary>
	/// 디버깅 함수
	/// 화면을 특정 색으로 채운다
	/// </summary>
	IE ClearScreen();

	/// <summary>
	/// 카메라 버퍼 생성
	/// </summary>
	/// <returns>성공 여부</returns>
	IE CreateCameraBuffer();

	/// <summary>
	/// 월드 버퍼 생성
	/// </summary>
	/// <returns>성공 여부</returns>
	IE CreateWorldBuffer();

	/// <summary>
	/// 범용적으로 쓸 수 있는 버퍼 생성 함수
	/// </summary>
	/// <param name="_bufferSize">필요한 사이즈</param>
	/// <param name="_buffer">생성될 버퍼</param>
	/// <returns>성공 여부</returns>
	IE CreateBuffer(uint64_t _bufferSize, OUT ComPtr<ID3D11Buffer>& _buffer, void* _initData = nullptr);

	/// <summary>
	/// StructedBuffer 생성
	/// </summary>
	/// <param name="_elementSize">버퍼 내용물의 크기</param>
	/// <param name="_maxCount">요소의 최대 갯수</param>
	/// <param name="_buffer">버퍼</param>
	/// <param name="_srv">버퍼 리소스</param>
	/// <returns>성공 여부</returns>
	IE CreateStructedBuffer(
		uint64_t _elementSize
		, uint64_t _maxCount
		, OUT ComPtr<ID3D11Buffer>& _buffer
		, OUT ComPtr<ID3D11ShaderResourceView>& _srv
	);

	/// <summary>
	/// InputLayout 생성
	/// </summary>
	/// <param name="_type">정점 타입</param>
	/// <param name="_stream">vs 데이터</param>
	/// <returns>성공 여부</returns>
	IE CreateInputLayout(VERTEX_TYPE _type, const std::vector<unsigned char>& _stream);

	IE CreateSkySphereObject();
#pragma endregion

#pragma region Bind
	/// <summary>
	/// 셰이더에 메인 카메라 바인딩
	/// </summary>
	/// <returns>성공 여부</returns>
	IE BindMainCameraBuffer();

	/// <summary>
	/// 셰이더에 월드 버퍼 바인딩
	/// </summary>
	/// <returns>성공 여부</returns>
	IE BindWorldBuffer(const Matrix& _matrix);

	/// <summary>
	/// 데이터를 버퍼에 바인딩
	/// </summary>
	/// <param name="_buffer">버퍼</param>
	/// <param name="_data">데이터</param>
	/// <param name="_size">크기</param>
	/// <returns>성공 여부</returns>
	IE BindDataBuffer(
		ComPtr<ID3D11Buffer> _buffer
		, void* _data
		, uint64_t _size
	);

	/// <summary>
	/// 정점 셰이더와 그에 맞는 Input layout을 바인딩
	/// </summary>
	/// <param name="_vs">정점 셰이더</param>
	/// <param name="_ia">Input Layout</param>
	/// <returns>성공 여부</returns>
	IE BindVertexShaderAndInputLayout(ComPtr<ID3D11VertexShader> _vs, ComPtr<ID3D11InputLayout> _ia);

	/// <summary>
	/// 픽셀 셰이더 바인딩
	/// </summary>
	/// <param name="_ps">픽셀 셰이더</param>
	/// <returns>성공 여부</returns>
	IE BindPixelShader(ComPtr<ID3D11PixelShader> _ps);

	/// <summary>
	/// 정점 버퍼 바인딩
	/// </summary>
	/// <param name="_vb">정점 버퍼</param>
	/// <param name="_size">요소의 크기</param>
	/// <returns>성공 여부</returns>
	IE BindVertexBuffer(ComPtr<ID3D11Buffer> _vb, UINT _size);

	/// <summary>
	/// 인덱스 버퍼 바인딩
	/// </summary>
	/// <param name="_id">인덱스 버퍼</param>
	/// <returns>성공 여부</returns>
	IE BindIndexBuffer(ComPtr<ID3D11Buffer> _id);
#pragma endregion

	bool(*FileOpenCallbackFunc)(
		void* userData
		, const std::wstring& _filename
		, OUT FILE_STREAM& fileData
		);

public:
	/// <summary>
	/// 초기화
	/// </summary>
	/// <param name="_initalizeState">초기화 상태</param>
	/// <returns>초기화 성공 여부</returns>
	IE Initialize(const InitializeState& _initalizeState, HWND _hwnd) override;

	/// <summary>
	/// 정적 매쉬 임포트
	/// </summary>
	/// <returns>정적 매쉬 UID</returns>
	IE ImportStaticMesh() override;

	/// <summary>
	/// 본 매쉬 임포트
	/// </summary>
	/// <returns>본 매쉬 UID</returns>
	IE ImportBoneMesh() override;

	/// <summary>
	/// 애니메이션 임포트
	/// </summary>
	/// <returns>애니메이션 UID</returns>
	IE ImportAnimation() override;

	/// <summary>
	/// 텍스쳐 생성
	/// </summary>
	/// <returns>텍스쳐 UID</returns>
	IE CreateTexture(const TextuerData* _textuerData) override;

	/// <summary>
	/// 머테리얼 생성
	/// </summary>
	/// <returns>머테리얼 UID</returns>
	IE CreateMaterial(const std::wstring _name, const MaterialData& _material) override;

	/// <summary>
	/// 카메라 생성
	/// </summary>
	/// <returns>카메라 ID</returns>
	std::weak_ptr<ICamera> CreateCamera() override;

	/// <summary>
	/// 메인 카메라 생성
	/// </summary>
	/// <param name="_cameraID">카메라 UID</param>
	/// <returns>성공 여부</returns>
	IE SetCamera(std::weak_ptr<ICamera> _cameraID) override;

	/// <summary>
	/// 매쉬에 애니메이션 설정
	/// </summary>
	/// <param name="_meshID">매쉬 UID</param>
	/// <returns>성공 여부</returns>
	IE SetAniamtion(int _meshID) override;

	/// <summary>
	/// 최종 렌더링
	/// </summary>
	/// <returns>성공 여부</returns>
	IE Draw(std::function<void()> ImguiRender = nullptr) override;

	/// <summary>
	/// 렌더 타겟 클리어시 사용할 색
	/// </summary>
	/// <param name="_r">R</param>
	/// <param name="_g">G</param>
	/// <param name="_b">B</param>
	/// <param name="_a">A</param>
	/// <returns>성공 여부</returns>
	IE SetBackgroundColor(float _r, float _g, float _b, float _a) override
	{
		m_defaultBG[0] = _r;
		m_defaultBG[1] = _g;
		m_defaultBG[2] = _b;
		m_defaultBG[3] = _a;

		return IE::I_OK;
	};

	/// <summary>
	/// 정점 셰이더 생성
	/// </summary>
	/// <param name="_name">셰이더 이름</param>
	/// <param name="_stream">셰이더 데이터 스트림</param>
	/// <returns>성공 여부</returns>
	IE CreateVertexShader(VERTEX_TYPE _type, const std::wstring& _name) override;

	/// <summary>
	/// 픽셀 셰이더 생성
	/// </summary>
	/// <param name="_name">셰이더 이름</param>
	/// <param name="_stream">셰이더 데이터 스트림</param>
	/// <returns>성공 여부</returns>
	IE CreatePixelShader(const std::wstring& _name) override;

	/// <summary>
	/// 정점 및 인덱스 버퍼 생성
	/// </summary>
	/// <param name="_name">매쉬 이름</param>
	/// <param name="_stream">데이터</param>
	/// <returns>성공 여부</returns>
	IE CreateVertexIndexBuffer(const std::wstring& _name) override;

	IE AddRenderObject(std::shared_ptr<IRenderObject> _renderObject) override;

	/// <summary>
	/// 모델 오브젝트 추가
	/// </summary>
	/// <param name="_renderObject">모델 오브젝트</param>
	/// <returns>성공 여부</returns>
	IE AddModelObject(std::shared_ptr<IModelObject> _modelObject) override;

	/// <summary>
	/// 렌더 영역 설정
	/// </summary>
	/// <param name="_w">높이</param>
	/// <param name="_h">너비</param>
	/// <returns>성공 여부</returns>
	IE SetRenderSize(UINT _w, UINT _h) override;

	/// <summary>
	/// 한 씬에 존재할 수 있는 빛의 총 갯수
	/// </summary>
	/// <param name="_val">갯수</param>
	void SetMaxLightCount(uint64_t _val) override { m_maxLightCount = _val; };

	/// <summary>
	/// 씬에 빛 추가
	/// </summary>
	/// <param name="_name">빛 이름</param>
	/// <param name="_lightData">빛 데이터</param>
	/// <returns>성공 여부</returns>
	IE AddLight(const std::wstring& _name, const LightData& _lightData) override;

	/// <summary>
	/// Imgui 시작을 위한 함수
	/// </summary>
	/// <param name="ImguiStartFunc">imgui start function</param>
	/// <returns>성공 여부</returns>
	IE ImguiInitialize(
		bool(*ImguiStartFunc)(
			ID3D11Device* _device
			, ID3D11DeviceContext* _deviceContext
			)
	) override;

	/// <summary>
	/// 스카이 박스에 사용될 정점 셰이더
	/// </summary>
	/// <returns></returns>
	IE SetSkyVS(VERTEX_TYPE _type, const std::wstring& _vs) override;

	/// <summary>
	/// 스카이 박스에 사용될 픽셀 셰이더
	/// </summary>
	/// <returns></returns>
	IE SetSkyPS(const std::wstring& _ps) override;

	/// <summary>
	/// 스카이 박스에 사용될 텍스쳐
	/// </summary>
	/// <returns></returns>
	IE SetSkyTextuer(const TextuerData* _textuer) override;

	/// <summary>
	/// 텍스쳐가 없을 때 사용할 색 값
	/// </summary>
	/// <param name="_diffuse">색</param>
	/// <param name="_normal">노말</param>
	/// <param name="_roughness">거칠기</param>
	/// <param name="_metalic">금속성</param>
	/// <returns>성공여부</returns>
	IE CreateDefaultTextuer(
		const Vector3& _diffuse
		, const Vector3& _normal
		, const Vector3& _roughness
		, const Vector3& _metalic
	) override;
};

