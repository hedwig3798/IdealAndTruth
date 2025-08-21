#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl\client.h>
#include "IRenderer.h"
#include "windows.h"
#include "CustomAllocator/IAllocator.h"
#include <map>
#include <string>

using Microsoft::WRL::ComPtr;

class D3D11Renderer :
    public IRenderer
{
private:
	HWND m_hwnd;

	ComPtr<ID3D11Device> m_device;
	ComPtr<ID3D11DeviceContext> m_deviceContext;
	ComPtr<IDXGISwapChain> m_swapChain;
	ComPtr<ID3D11RenderTargetView> m_finalRenderTargetView;
	ComPtr<ID3D11Texture2D> m_depthStancilBuffer;
	ComPtr<ID3D11DepthStencilView> m_depthStancilView;
	ComPtr<ID3D11RasterizerState> m_rasterizerState;

	D3D_FEATURE_LEVEL m_featureLevel;

	Matrix m_world;
	Matrix m_view;
	Matrix m_projection;

	std::map<size_t, IAllocator*>m_allocators;

public:
	D3D11Renderer();
	virtual ~D3D11Renderer();

private:
	/// 초기화 함수 모음

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
	/// 텍스쳐 임포트
	/// </summary>
	/// <returns>텍스쳐 UID</returns>
	IE ImportTexture() override;

	/// <summary>
	/// 머테리얼 임포트
	/// </summary>
	/// <returns>머테리얼 UID</returns>
	IE ImportMaterial() override;

	/// <summary>
	/// 카메라 생성
	/// </summary>
	/// <returns>카메라 UID</returns>
	IE CreateCamera() override;

	/// <summary>
	/// 메인 카메라 생성
	/// </summary>
	/// <param name="_cameraID">카메라 UID</param>
	/// <returns>성공 여부</returns>
	IE SetCamera(int _cameraID) override;

	/// <summary>
	/// 매쉬에 애니메이션 설정
	/// </summary>
	/// <param name="_meshID">매쉬 UID</param>
	/// <returns>성공 여부</returns>
	IE SetAniamtion(int _meshID) override;

	/// <summary>
	/// 머테리얼 설정
	/// </summary>
	/// <param name="_mehsID">설정할 매쉬 UID</param>
	/// <returns>성공 여부</returns>
	IE SetMaterial(int _mehsID) override;

	/// <summary>
	/// 텍스쳐 설정
	/// </summary>
	/// <param name="_materialID">설정할 머테리얼 UID</param>
	/// <param name="_slot">설정할 슬릇</param>
	/// <returns>성공 여부</returns>
	IE SetTexture(int _materialID, int _slot = 0) override;

	/// <summary>
	/// 최종 렌더링
	/// </summary>
	/// <returns>성공 여부</returns>
	IE Draw() override;
};

