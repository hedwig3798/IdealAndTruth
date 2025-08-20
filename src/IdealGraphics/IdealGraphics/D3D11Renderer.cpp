#include "pch.h"
#include "D3D11Renderer.h"

#define MEM_SIZE 1'048'576'000 * 4 //4GB

D3D11Renderer::D3D11Renderer()
	: m_hwnd()
	, m_device{}
	, m_deviceContext{}
	, m_swapChain{}
	, m_finalRenderTargetView{}
	, m_depthStancilBuffer{}
	, m_rasterizerState{}
	, m_featureLevel{}
	, m_allocators{}
{

}

D3D11Renderer::~D3D11Renderer()
{
	m_allocators.clear();
}

IE D3D11Renderer::CreateD3D11DeviceContext(const InitializeState::Device& _device)
{
	HRESULT hr = S_OK;

	// D3D11 디바이스 생성
	hr |= D3D11CreateDevice(
		0,
		D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE,
		0,
		(UINT)_device.m_deviceFlags,
		0,
		0,
		D3D11_SDK_VERSION,
		m_device.GetAddressOf(),
		&m_featureLevel,
		m_deviceContext.GetAddressOf()
	);

	if (S_OK != hr)
	{
		return IE::CREATE_D3D_COMPONENT_FAIL;
	}

	return IE::I_OK;
}

IE D3D11Renderer::CreateSwapChain(const InitializeState::SwapCahin& _swapChain)
{
	HRESULT hr = S_OK;

	// 스왑  체인 구조체
	DXGI_SWAP_CHAIN_DESC chainDesc = {};
	ZeroMemory(&chainDesc, sizeof(chainDesc));

	chainDesc.BufferDesc.Width = _swapChain.m_width;
	chainDesc.BufferDesc.Height = _swapChain.m_height;
	chainDesc.BufferDesc.RefreshRate.Numerator = _swapChain.m_refreshRateNumerator;
	chainDesc.BufferDesc.RefreshRate.Denominator = _swapChain.m_refreshRateDenominator;
	chainDesc.BufferDesc.Format = (DXGI_FORMAT)_swapChain.m_format;
	chainDesc.BufferDesc.ScanlineOrdering = (DXGI_MODE_SCANLINE_ORDER)_swapChain.m_scanlineOrdering;
	chainDesc.BufferDesc.Scaling = (DXGI_MODE_SCALING)_swapChain.m_scaling;

	chainDesc.SampleDesc.Count = _swapChain.m_sampleCount;
	chainDesc.SampleDesc.Quality = _swapChain.m_sampleQuality;

	chainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	chainDesc.BufferCount = _swapChain.m_bufferCount;

	chainDesc.OutputWindow = m_hwnd;
	chainDesc.Windowed = _swapChain.m_isWindowed;
	chainDesc.SwapEffect = (DXGI_SWAP_EFFECT)_swapChain.m_swapEffect;
	chainDesc.Flags = 0;

	// 디바이스 인터페이스 가져오기
	IDXGIDevice* dxgiDevice = nullptr;
	hr |= (m_device->QueryInterface(
		__uuidof(IDXGIDevice),
		reinterpret_cast<void**>(&dxgiDevice)));

	if (nullptr == dxgiDevice)
	{
		return IE::NULL_POINTER_ACCESS;
	}

	// 디바이스 어뎁터 가져오기
	IDXGIAdapter* dxgiAdapter = nullptr;
	hr |= (dxgiDevice->GetParent(
		__uuidof(IDXGIAdapter),
		reinterpret_cast<void**>(&dxgiAdapter)));

	if (nullptr == dxgiAdapter)
	{
		return IE::NULL_POINTER_ACCESS;
	}

	// 디바이스 펙토리 가져오기
	IDXGIFactory* dxgiFactory = nullptr;
	hr |= (dxgiAdapter->GetParent(
		__uuidof(IDXGIFactory),
		reinterpret_cast<void**>(&dxgiFactory)));

	if (nullptr == dxgiFactory)
	{
		return IE::NULL_POINTER_ACCESS;
	}

	// 스왑 체인 생성
	hr |= (dxgiFactory->CreateSwapChain(m_device.Get(), &chainDesc, m_swapChain.GetAddressOf()));

	if (nullptr == m_swapChain)
	{
		return IE::NULL_POINTER_ACCESS;
	}

	// 사용한 인터페이스 제거
	dxgiDevice->Release();
	dxgiAdapter->Release();
	dxgiFactory->Release();

	return IE::I_OK;
}

IE D3D11Renderer::CreateFinalRenterTargetView()
{
	ID3D11Texture2D* backBuffer = nullptr;
	HRESULT hr = S_OK;

	// 예외처리
	if (nullptr == m_swapChain
		|| nullptr == m_device
		|| nullptr == m_finalRenderTargetView)
	{
		return IE::NULL_POINTER_ACCESS;
	}

	// 스왑 체인에서 버퍼를 가져옴
	hr |= m_swapChain->GetBuffer(
		0,
		__uuidof(ID3D11Texture2D),
		reinterpret_cast<void**>(&backBuffer)
	);

	// 렌더 타겟 뷰 생성
	hr |= m_device->CreateRenderTargetView(
		backBuffer,
		0,
		m_finalRenderTargetView.GetAddressOf()
	);

	// 예외처리
	if (nullptr == backBuffer
		|| nullptr == m_finalRenderTargetView)
	{
		return IE::CREATE_D3D_COMPONENT_FAIL;
	}

	// 사용한 백 버퍼 인터페이스 반환	
	hr |= backBuffer->Release();

	return IE::I_OK;
}

IE D3D11Renderer::CreateDepthStencilBufferAndView(const InitializeState::DepthStancil& _depthStancil)
{
	HRESULT hr = S_OK;

	// 예외처리
	if (nullptr == m_device)
	{
		return IE::NULL_POINTER_ACCESS;
	}

	// 구조체 값 채우기
	D3D11_TEXTURE2D_DESC depthStancilDesc = {};
	depthStancilDesc.Width = _depthStancil.m_width;
	depthStancilDesc.Height = _depthStancil.m_height;
	depthStancilDesc.MipLevels = _depthStancil.m_mipLevel;
	depthStancilDesc.ArraySize = _depthStancil.m_arraySize;
	depthStancilDesc.Format = (DXGI_FORMAT)_depthStancil.m_format;

	depthStancilDesc.SampleDesc.Count = _depthStancil.m_sampleCount;
	depthStancilDesc.SampleDesc.Quality = _depthStancil.m_sampleQuality;

	depthStancilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStancilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStancilDesc.CPUAccessFlags = 0;
	depthStancilDesc.MiscFlags = 0;

	// 깊이 스텐실 버퍼 생성
	hr |= m_device->CreateTexture2D(
		&depthStancilDesc,
		nullptr,
		m_depthStancilBuffer.GetAddressOf()
	);

	// 깊이 스텐실 뷰 생성
	hr |= m_device->CreateDepthStencilView(
		m_depthStancilBuffer.Get(),
		0,
		m_depthStancilView.GetAddressOf()
	);

	if (nullptr == m_depthStancilBuffer
		|| nullptr ==  m_depthStancilView)
	{
		return IE::CREATE_D3D_COMPONENT_FAIL;
	}

	return IE::I_OK;
}

IE D3D11Renderer::CreateViewPort()
{
	// 예외처리
	if (nullptr == m_deviceContext)
	{
		return IE::NULL_POINTER_ACCESS;
	}

	RECT windowSize = {};
	GetWindowRect(m_hwnd, &windowSize);

	D3D11_VIEWPORT vp = {};
	vp.Width = static_cast<FLOAT>(windowSize.right - windowSize.left);
	vp.Height = static_cast<FLOAT>(windowSize.bottom - windowSize.top);
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	m_deviceContext->RSSetViewports(1, &vp);

	return IE::I_OK;
}

IE D3D11Renderer::CreateRasterizerState(const InitializeState::RaseterizerState& _rasterizer)
{
	HRESULT hr = S_OK;

	// 예외처리
	if (nullptr == m_device)
	{
		return IE::NULL_POINTER_ACCESS;
	}

	D3D11_RASTERIZER_DESC rsDesc;
	ZeroMemory(&rsDesc, sizeof(D3D11_RASTERIZER_DESC));
	rsDesc.FillMode = (D3D11_FILL_MODE)_rasterizer.m_fillMode;
	rsDesc.CullMode = (D3D11_CULL_MODE)_rasterizer.m_cullMode;
	rsDesc.FrontCounterClockwise = _rasterizer.m_isFrontCCW;
	rsDesc.DepthClipEnable = _rasterizer.m_isDepthClip;

	hr |= m_device->CreateRasterizerState(&rsDesc, m_rasterizerState.GetAddressOf());

	if (nullptr == m_rasterizerState)
	{
		return IE::CREATE_D3D_COMPONENT_FAIL;
	}

	return IE::I_OK;
}

IE D3D11Renderer::ClearScreen()
{
	// 예외처리
	if (nullptr == m_deviceContext
		|| nullptr == m_finalRenderTargetView
		|| nullptr == m_depthStancilView)
	{
		return IE::NULL_POINTER_ACCESS;
	}

	// 임시 색 ( R G B A )
	float bgRed[4] = { 1.0f, 0.0f, 0.0f, 0.0f };

	// 렌더 타겟을 지정한 색으로 초기화
	m_deviceContext->ClearRenderTargetView(
		m_finalRenderTargetView.Get(),
		bgRed
	);

	// 깊이 스텐실 뷰 초기화
	m_deviceContext->ClearDepthStencilView(
		m_depthStancilView.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0
	);

	return IE::I_OK;
}

IE D3D11Renderer::Initialize(const InitializeState& _initalizeState)
{
	HRESULT hr = S_OK;

	CreateD3D11DeviceContext(_initalizeState.m_device);
	CreateSwapChain(_initalizeState.m_swapChain);
	CreateDepthStencilBufferAndView(_initalizeState.m_depthStancil);
	CreateRasterizerState(_initalizeState.m_rasterizer);
	
	// 최종 생성 뷰를 바인드 한다
	m_deviceContext->OMSetRenderTargets(
		1,
		m_finalRenderTargetView.GetAddressOf(),
		m_depthStancilView.Get()
	);

	// 특정 색으로 화면을 초기화한다.
	ClearScreen();

	return IE::I_OK;
}

IE D3D11Renderer::ImportStaticMesh()
{
	return IE::I_OK;
}

IE D3D11Renderer::ImportBoneMesh()
{
	return IE::I_OK;
}

IE D3D11Renderer::ImportAnimation()
{
	return IE::I_OK;
}

IE D3D11Renderer::ImportTexture()
{
	return IE::I_OK;
}

IE D3D11Renderer::ImportMaterial()
{
	return IE::I_OK;
}

IE D3D11Renderer::CreateCamera()
{
	return IE::I_OK;
}

IE D3D11Renderer::SetCamera(int _cameraID)
{
	return IE::I_OK;
}

IE D3D11Renderer::SetAniamtion(int _meshID)
{
	return IE::I_OK;
}

IE D3D11Renderer::SetMaterial(int _mehsID)
{
	return IE::I_OK;
}

IE D3D11Renderer::SetTexture(int _materialID, int _slot /*= 0*/)
{
	return IE::I_OK;
}

IE D3D11Renderer::Draw()
{
	m_swapChain->Present(0, 0);
	return IE::I_OK;
}
