#include "pch.h"
#include "D3D11Renderer.h"
#include <sstream>
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
	, m_defaultBG{ 1, 1, 1, 1 }
	, m_mainCamera(-1)
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
		return IE::CREATE_D3D_DEVICE_FAIL;
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

	chainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
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
		return IE::CREATE_D3D_SWAP_CHAIN_FAIL;
	}

	// 디바이스 어뎁터 가져오기
	IDXGIAdapter* dxgiAdapter = nullptr;
	hr |= (dxgiDevice->GetParent(
		__uuidof(IDXGIAdapter),
		reinterpret_cast<void**>(&dxgiAdapter)));

	if (nullptr == dxgiAdapter)
	{
		return IE::CREATE_D3D_SWAP_CHAIN_FAIL;
	}

	// 디바이스 펙토리 가져오기
	IDXGIFactory* dxgiFactory = nullptr;
	hr |= (dxgiAdapter->GetParent(
		__uuidof(IDXGIFactory),
		reinterpret_cast<void**>(&dxgiFactory)));

	if (nullptr == dxgiFactory)
	{
		return IE::CREATE_D3D_SWAP_CHAIN_FAIL;
	}

	// 스왑 체인 생성
	hr |= (dxgiFactory->CreateSwapChain(m_device.Get(), &chainDesc, m_swapChain.GetAddressOf()));

	if (nullptr == m_swapChain)
	{
		return IE::CREATE_D3D_SWAP_CHAIN_FAIL;
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
		|| nullptr == m_deviceContext
		)
	{
		return IE::NULL_POINTER_ACCESS;
	}

	// 스왑 체인에서 버퍼를 가져옴
	hr |= m_swapChain->GetBuffer(
		0,
		__uuidof(ID3D11Texture2D),
		reinterpret_cast<void**>(&backBuffer)
	);

	if (nullptr == backBuffer)
	{
		return IE::CREATE_D3D_BUFFER_FAIL;
	}

	// 렌더 타겟 뷰 생성
	hr |= m_device->CreateRenderTargetView(
		backBuffer,
		0,
		m_finalRenderTargetView.GetAddressOf()
	);

	// 예외처리
	if (nullptr == m_finalRenderTargetView)
	{
		return IE::CREATE_D3D_RENDER_TARGET_FAIL;
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
		|| nullptr == m_depthStancilView)
	{
		return IE::CREATE_D3D_DEPTH_STANCIL_FIAL;
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
		return IE::CREATE_D3D_RASTERIZERSTATE_FIAL;
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

	// 렌더 타겟을 지정한 색으로 초기화
	m_deviceContext->ClearRenderTargetView(
		m_finalRenderTargetView.Get(),
		m_defaultBG
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

IE D3D11Renderer::CreateCameraBuffer()
{
	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC mbd = {};
	mbd.Usage = D3D11_USAGE_DYNAMIC;
	mbd.ByteWidth = sizeof(CameraBuffer);
	mbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	mbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	mbd.MiscFlags = 0;
	mbd.StructureByteStride = 0;

	hr = m_device->CreateBuffer(&mbd, nullptr, m_cameraCBuffer.GetAddressOf());

	if (S_OK != hr)
	{
		return IE::CREATE_D3D_BUFFER_FAIL;
	}

	return IE::I_OK;
}

IE D3D11Renderer::BindMainCameraBuffer()
{
	HRESULT hr = S_OK;

	D3D11_MAPPED_SUBRESOURCE mappedResource;

	hr = m_deviceContext->Map(
		m_cameraCBuffer.Get()
		, 0
		, D3D11_MAP_WRITE_DISCARD
		, 0
		, &mappedResource);

	if (S_OK != hr)
	{
		return IE::MAPPING_SHADER_BUFFER_FAIL;
	}

	CameraBuffer* dataptr = (CameraBuffer*)mappedResource.pData;
	dataptr->m_view = m_cameras[m_mainCamera].GetViewMatrix().Transpose();
	dataptr->m_proj = m_cameras[m_mainCamera].GetProjMatrix().Transpose();

	m_deviceContext->VSSetConstantBuffers(1, 1, m_cameraCBuffer.GetAddressOf());
	m_deviceContext->Unmap(m_cameraCBuffer.Get(), 0);

	return IE::I_OK;
}

IE D3D11Renderer::CreateInputLayout(VERTEX_TYPE _type, const std::vector<unsigned char>& _stream)
{
	HRESULT hr = S_OK;
	IE result = IE::I_OK;

	D3D11_INPUT_ELEMENT_DESC* decs = nullptr;
	ComPtr<ID3DBlob> blob;
	UINT eleNum = 0;

	if (0 == m_iaBuffer.size())
	{
		m_iaBuffer.resize(static_cast<int>(VERTEX_TYPE::END));
		for (auto& ia : m_iaBuffer)
		{
			ia = nullptr;
		}
	}

	// 셰이더 정보 가져오기
	hr = ::D3DCreateBlob(_stream.size(), blob.GetAddressOf());
	if (S_OK != hr)
	{
		return IE::CREATE_D3D_BLOB_FAIL;
	}
	std::memcpy(blob->GetBufferPointer(), _stream.data(), _stream.size());

	switch (_type)
	{
	case IRenderer::VERTEX_TYPE::VertexSuper:
	{
		decs = VertexSuper::m_IADesc;
		eleNum = 7;
		break;
	}
	case IRenderer::VERTEX_TYPE::VertexP:
	{
		decs = VertexP::m_IADesc;
		eleNum = 1;
		break;
	}
	case IRenderer::VERTEX_TYPE::VertexPU:
	{
		decs = VertexPU::m_IADesc;
		eleNum = 2;
		break;
	}
	case IRenderer::VERTEX_TYPE::VertexPN:
	{
		decs = VertexPN::m_IADesc;
		eleNum = 2;
		break;
	}
	case IRenderer::VERTEX_TYPE::VertexPUN:
	{
		decs = VertexPUN::m_IADesc;
		eleNum = 3;
		break;
	}
	case IRenderer::VERTEX_TYPE::VertexPC:
	{
		decs = VertexPC::m_IADesc;
		eleNum = 2;
		break;
	}
	default:
		break;
	}

	if (decs == nullptr
		|| m_device == nullptr)
	{
		return IE::NULL_POINTER_ACCESS;
	}

	hr = m_device->CreateInputLayout(
		decs
		, eleNum
		, blob->GetBufferPointer()
		, blob->GetBufferSize()
		, m_iaBuffer[static_cast<int>(_type)].GetAddressOf()
	);

	if (S_OK != hr)
	{
		return IE::CREATE_D3D_INPUT_LAYOUT_FAIL;
	}

	return IE::I_OK;
}

IE D3D11Renderer::CreateVertexShader(VERTEX_TYPE _type, const std::string& _name, const std::vector<unsigned char>& _stream)
{
	// 같은 이름의 셰이더가 있다면 무시힌다.
	auto mit = m_vsMap.find(_name);
	if (mit != m_vsMap.end())
	{
		return IE::ALREADY_EXIST;
	}

	// 널 체크
	if (true == _stream.empty()
		|| nullptr == m_device)
	{
		return IE::NULL_POINTER_ACCESS;
	}

	// 정점 셰이더 객체 생성
	ComPtr<ID3D11VertexShader> vs;
	m_device->CreateVertexShader(_stream.data(), _stream.size(), nullptr, vs.GetAddressOf());
	
	// 맵에 저장
	m_vsMap[_name] = vs;

	if (0 == m_iaBuffer.size())
	{
		m_iaBuffer.resize(static_cast<int>(VERTEX_TYPE::END));
		for (auto& ia : m_iaBuffer)
		{
			ia = nullptr;
		}
	}

	if (nullptr == m_iaBuffer[static_cast<int>(_type)])
	{
		return CreateInputLayout(_type, _stream);
	}

	return IE::I_OK;
}

IE D3D11Renderer::CreatePixelShader(const std::string& _name, const std::vector<unsigned char>& _stream)
{
	// 같은 이름의 셰이더가 있다면 무시힌다.
	auto mit = m_psMap.find(_name);
	if (mit != m_psMap.end())
	{
		return IE::ALREADY_EXIST;
	}

	// 널 체크
	if (true == _stream.empty()
		|| nullptr == m_device)
	{
		return IE::NULL_POINTER_ACCESS;
	}

	// 정점 셰이더 객체 생성
	ComPtr<ID3D11PixelShader> ps;
	m_device->CreatePixelShader(_stream.data(), _stream.size(), nullptr, ps.GetAddressOf());

	// 맵에 저장
	m_psMap[_name] = ps;
	return IE::I_OK;
}

IE D3D11Renderer::Initialize(const InitializeState& _initalizeState, HWND _hwnd)
{
	m_hwnd = _hwnd;

	IE result;
	result = CreateD3D11DeviceContext(_initalizeState.m_device);
	if (result != IE::I_OK)
	{
		return result;
	}
	result = CreateSwapChain(_initalizeState.m_swapChain);
	if (result != IE::I_OK)
	{
		return result;
	}
	result = CreateDepthStencilBufferAndView(_initalizeState.m_depthStancil);
	if (result != IE::I_OK)
	{
		return result;
	}
	result = CreateRasterizerState(_initalizeState.m_rasterizer);
	if (result != IE::I_OK)
	{
		return result;
	}

	result = CreateViewPort();
	if (result != IE::I_OK)
	{
		return result;
	}

	result = CreateFinalRenterTargetView();
	if (result != IE::I_OK)
	{
		return result;
	}

	// 최종 생성 뷰를 바인드 한다
	m_deviceContext->OMSetRenderTargets(
		1,
		m_finalRenderTargetView.GetAddressOf(),
		m_depthStancilView.Get()
	);

	// 특정 색으로 화면을 초기화한다.
	ClearScreen();

	// 카메라 버퍼 생성
	CreateCameraBuffer();

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

int D3D11Renderer::CreateCamera()
{
	m_cameras.push_back(Camera());

	return static_cast<int>(m_cameras.size() - 1);
}

IE D3D11Renderer::SetCamera(int _cameraID)
{
	m_mainCamera = _cameraID;

	if (m_mainCamera >= m_cameras.size()
		|| m_mainCamera < 0)
	{
		return IE::WORNG_CAMERA;
	}

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
	IE result = IE::I_OK;

	// null check
	if (nullptr == m_swapChain)
	{
		return IE::NULL_POINTER_ACCESS;
	}

	// main camera check
	if (m_mainCamera >= m_cameras.size()
		|| m_mainCamera < 0)
	{
		return IE::WORNG_CAMERA;
	}

	// 메인 카메라 행렬 계산
	if (m_cameras[m_mainCamera].Caculate())
	{
		// 계산을 했다면 바인딩
		BindMainCameraBuffer();
	}

	m_swapChain->Present(0, 0);

	result = ClearScreen();
	if (IE::I_OK != result)
	{
		return result;
	}

	return result;
}
