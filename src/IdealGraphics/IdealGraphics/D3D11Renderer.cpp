#include "pch.h"
#include "D3D11Renderer.h"
#include "stringUtil.h"
#include "Sphere.h"

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
	, m_currentRenderSet()
	, m_maxLightCount(1)
	, m_lightCountData{ 0, 0, 0 }
	, m_skyIndexSize(0)
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

	chainDesc.BufferDesc.Width = m_renderWidth;
	chainDesc.BufferDesc.Height = m_renderHight;
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
	depthStancilDesc.Width = m_renderWidth;
	depthStancilDesc.Height = m_renderHight;
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

	D3D11_VIEWPORT vp = {};
	vp.Width = static_cast<float>(m_renderWidth);
	vp.Height = static_cast<float>(m_renderHight);
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

	m_deviceContext->RSSetState(m_rasterizerState.Get());

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
	mbd.ByteWidth = sizeof(CameraBufferData);
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

IE D3D11Renderer::CreateWorldBuffer()
{
	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC mbd = {};
	mbd.Usage = D3D11_USAGE_DYNAMIC;
	mbd.ByteWidth = sizeof(WorldBufferData);
	mbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	mbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	mbd.MiscFlags = 0;
	mbd.StructureByteStride = 0;

	hr = m_device->CreateBuffer(&mbd, nullptr, m_worldBuffer.GetAddressOf());

	if (S_OK != hr)
	{
		return IE::CREATE_D3D_BUFFER_FAIL;
	}

	return IE::I_OK;
}

IE D3D11Renderer::CreateBuffer(uint64_t _bufferSize, OUT ComPtr<ID3D11Buffer>& _buffer, void* _initData /*= nullptr*/)
{
	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC mbd = {};
	mbd.Usage = D3D11_USAGE_DYNAMIC;
	mbd.ByteWidth = static_cast<UINT>(_bufferSize);
	mbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	mbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	mbd.MiscFlags = 0;
	mbd.StructureByteStride = 0;

	if (nullptr == _initData)
	{
		hr = m_device->CreateBuffer(&mbd, nullptr, _buffer.GetAddressOf());
	}
	else
	{
		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = _initData;

		hr = m_device->CreateBuffer(&mbd, &initData, _buffer.GetAddressOf());
	}

	if (S_OK != hr)
	{
		return IE::CREATE_D3D_BUFFER_FAIL;
	}

	return IE::I_OK;
}

IE D3D11Renderer::CreateStructedBuffer(uint64_t _elementSize, uint64_t _maxCount, OUT ComPtr<ID3D11Buffer>& _buffer, OUT ComPtr<ID3D11ShaderResourceView>& _srv)
{
	HRESULT hr;

	D3D11_BUFFER_DESC desc = {};
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.ByteWidth = static_cast<UINT>(_elementSize * _maxCount);
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = static_cast<UINT>(_elementSize);
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = m_device->CreateBuffer(&desc, nullptr, _buffer.GetAddressOf());

	if (S_OK != hr)
	{
		return IE::CREATE_D3D_BUFFER_FAIL;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = static_cast<UINT>(_maxCount);

	hr = m_device->CreateShaderResourceView(_buffer.Get(), &srvDesc, _srv.GetAddressOf());
	if (S_OK != hr)
	{
		return IE::CREATE_D3D_SHAEDER_RESOURCE_VIEW_FAIL;
	}

	return IE::I_OK;
}

IE D3D11Renderer::BindMainCameraBuffer()
{
	if (m_mainCamera.expired())
	{
		return IE::NULL_POINTER_ACCESS;
	}

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

	CameraBufferData* dataptr = (CameraBufferData*)mappedResource.pData;
	dataptr->m_view = m_mainCamera.lock()->GetViewMatrix().Transpose();
	dataptr->m_proj = m_mainCamera.lock()->GetProjMatrix().Transpose();
	dataptr->m_camearaPos = m_mainCamera.lock()->GetPosition();

	m_deviceContext->VSSetConstantBuffers(1, 1, m_cameraCBuffer.GetAddressOf());
	m_deviceContext->Unmap(m_cameraCBuffer.Get(), 0);

	return IE::I_OK;
}

IE D3D11Renderer::BindWorldBuffer(const Matrix& _matrix)
{
	HRESULT hr = S_OK;

	D3D11_MAPPED_SUBRESOURCE mappedResource;

	hr = m_deviceContext->Map(
		m_worldBuffer.Get()
		, 0
		, D3D11_MAP_WRITE_DISCARD
		, 0
		, &mappedResource);

	if (S_OK != hr)
	{
		return IE::MAPPING_SHADER_BUFFER_FAIL;
	}

	WorldBufferData* dataptr = (WorldBufferData*)mappedResource.pData;
	dataptr->m_world = _matrix.Transpose();

	m_deviceContext->VSSetConstantBuffers(0, 1, m_worldBuffer.GetAddressOf());
	m_deviceContext->Unmap(m_worldBuffer.Get(), 0);

	return IE::I_OK;
}


IE D3D11Renderer::BindDataBuffer(ComPtr<ID3D11Buffer> _buffer, void* _data, uint64_t _size)
{
	if (nullptr == _buffer
		|| nullptr == _data
		|| nullptr == m_deviceContext)
	{
		return IE::NULL_POINTER_ACCESS;
	}

	D3D11_BUFFER_DESC desc;
	_buffer->GetDesc(&desc);

	if (desc.Usage != D3D11_USAGE_DYNAMIC
		|| false == (desc.CPUAccessFlags & D3D11_CPU_ACCESS_WRITE))
	{
		return IE::D3D_WRONG_BUFFER_ACCESS_FLAG;
	}

	if (_size > desc.ByteWidth)
	{
		return IE::OUT_OF_POINTER_BOUNDARY;
	}

	D3D11_MAPPED_SUBRESOURCE mapped;
	m_deviceContext->Map(_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	memcpy(mapped.pData, _data, _size);
	m_deviceContext->Unmap(_buffer.Get(), 0);

	return IE::I_OK;
}

IE D3D11Renderer::BindVertexShaderAndInputLayout(ComPtr<ID3D11VertexShader> _vs, ComPtr<ID3D11InputLayout> _ia)
{
	if (nullptr == m_deviceContext
		|| nullptr == _ia
		|| nullptr == _vs)
	{
		return IE::NULL_POINTER_ACCESS;
	}

	if (m_currentRenderSet.m_ia != _ia)
	{
		m_deviceContext->IASetInputLayout(_ia.Get());
	}
	if (m_currentRenderSet.m_vs != _vs)
	{
		m_deviceContext->VSSetShader(_vs.Get(), nullptr, 0);
	}

	return IE::I_OK;
}

IE D3D11Renderer::BindPixelShader(ComPtr<ID3D11PixelShader> _ps)
{
	if (nullptr == m_deviceContext
		|| nullptr == _ps
		)
	{
		return IE::NULL_POINTER_ACCESS;
	}

	if (m_currentRenderSet.m_ps != _ps)
	{
		m_deviceContext->PSSetShader(_ps.Get(), nullptr, 0);
	}

	return IE::I_OK;
}

IE D3D11Renderer::BindVertexBuffer(ComPtr<ID3D11Buffer> _vb, UINT _size)
{
	if (nullptr == m_deviceContext
		|| nullptr == _vb
		)
	{
		return IE::NULL_POINTER_ACCESS;
	}

	UINT offset = 0;
	m_deviceContext->IASetVertexBuffers(0, 1, _vb.GetAddressOf(), &_size, &offset);

	return IE::I_OK;
}

IE D3D11Renderer::BindIndexBuffer(ComPtr<ID3D11Buffer> _id)
{
	if (nullptr == m_deviceContext
		|| nullptr == _id
		)
	{
		return IE::NULL_POINTER_ACCESS;
	}

	m_deviceContext->IASetIndexBuffer(_id.Get(), DXGI_FORMAT_R32_UINT, 0);

	return IE::I_OK;
}

IE D3D11Renderer::CreateInputLayout(VERTEX_TYPE _type, const std::vector<unsigned char>& _stream)
{
	HRESULT hr = S_OK;
	IE result = IE::I_OK;

	// IA 생성을 위해 필요한 요소
	D3D11_INPUT_ELEMENT_DESC* decs = nullptr;
	ComPtr<ID3DBlob> blob;
	UINT eleNum = 0;

	// 만약 IA 버퍼가 없다면 생성
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

	// 정점 타입에 따라 다르다
	switch (_type)
	{
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
	case IRenderer::VERTEX_TYPE::VertexPNTU:
	{
		decs = VertexPNTU::m_IADesc;
		eleNum = 4;
		break;
	}
	// 타입이 없으면 그냥 슈퍼 타입 지정
	default:
	{
		decs = VertexSuper::m_IADesc;
		eleNum = 7;
		break;
	}
	}

	// 널 체크
	if (decs == nullptr
		|| m_device == nullptr)
	{
		return IE::NULL_POINTER_ACCESS;
	}

	// IA 만들기
	hr = m_device->CreateInputLayout(
		decs
		, eleNum
		, blob->GetBufferPointer()
		, blob->GetBufferSize()
		, m_iaBuffer[static_cast<int>(_type)].GetAddressOf()
	);

	// IA 체크
	if (S_OK != hr)
	{
		return IE::CREATE_D3D_INPUT_LAYOUT_FAIL;
	}

	return IE::I_OK;
}

IE D3D11Renderer::CreateSkySphereObject()
{
	if (nullptr == m_device)
	{
		return IE::NULL_POINTER_ACCESS;
	}

	HRESULT hr;

	std::vector<VertexPU> vertex;
	std::vector<UINT> index;

	// 스카이박스 정점, 인덱스 버퍼 생성
	CreateSphere(vertex, index);

	if (vertex.empty()
		|| index.empty())
	{
		return IE::STREAM_ERROR;
	}

	// sky vertex buffer
	D3D11_BUFFER_DESC vb = {};
	vb.Usage = D3D11_USAGE_IMMUTABLE;
	vb.ByteWidth = static_cast<UINT>(vertex.size() * sizeof(VertexPU));
	vb.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vb.CPUAccessFlags = 0;
	vb.MiscFlags = 0;
	vb.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vInitData = {};
	vInitData.pSysMem = vertex.data();

	hr = m_device->CreateBuffer(
		&vb
		, &vInitData
		, m_skyRenderSet.m_vb.GetAddressOf()
	);

	if (S_OK != hr)
	{
		return IE::CREATE_D3D_BUFFER_FAIL;
	}

	// sky box index buffer
	D3D11_BUFFER_DESC ib = {};
	ib.Usage = D3D11_USAGE_IMMUTABLE;
	ib.ByteWidth = static_cast<UINT>(index.size() * sizeof(UINT));
	ib.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ib.CPUAccessFlags = 0;
	ib.MiscFlags = 0;
	ib.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA iInitData = {};
	iInitData.pSysMem = index.data();

	hr = m_device->CreateBuffer(
		&ib
		, &iInitData
		, m_skyRenderSet.m_ib.GetAddressOf()
	);

	if (S_OK != hr)
	{
		return IE::CREATE_D3D_BUFFER_FAIL;
	}

	// skybox index size
	m_skyIndexSize = static_cast<UINT>(index.size());

	return IE::I_OK;
}

IE D3D11Renderer::CreateVertexShader(VERTEX_TYPE _type, const std::wstring& _name)
{
	// 같은 이름의 셰이더가 있다면 무시힌다.
	auto mit = m_vsMap.find(_name);
	if (mit != m_vsMap.end())
	{
		return IE::I_OK;
	}

	// 널 체크
	if (nullptr == FileOpenCallbackFunc
		|| nullptr == m_device)
	{
		return IE::NULL_POINTER_ACCESS;
	}

	FILE_STREAM stream;
	bool isSuccess = FileOpenCallbackFunc(m_fms, _name, stream);
	if (false == isSuccess)
	{
		return IE::OPEN_FILE_FAIL;
	}

	// 정점 셰이더 객체 생성
	ComPtr<ID3D11VertexShader> vs;
	m_device->CreateVertexShader(stream.data(), stream.size(), nullptr, vs.GetAddressOf());

	// 맵에 저장
	m_vsMap[_name].first = vs;

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
		IE result;
		result = CreateInputLayout(_type, stream);
		if (IE::I_OK != result)
		{
			return result;
		}
		m_vsMap[_name].second = m_iaBuffer[static_cast<int>(_type)];
	}
	else
	{
		m_vsMap[_name].second = m_iaBuffer[static_cast<int>(_type)];
	}

	return IE::I_OK;
}

IE D3D11Renderer::CreatePixelShader(const std::wstring& _name)
{
	// 같은 이름의 셰이더가 있다면 무시힌다.
	auto mit = m_psMap.find(_name);
	if (mit != m_psMap.end())
	{
		return IE::I_OK;
	}

	// 널 체크
	if (nullptr == FileOpenCallbackFunc
		|| nullptr == m_device)
	{
		return IE::NULL_POINTER_ACCESS;
	}

	FILE_STREAM stream;
	bool isSuccess = FileOpenCallbackFunc(m_fms, _name, stream);
	if (false == isSuccess)
	{
		return IE::OPEN_FILE_FAIL;
	}

	// 정점 셰이더 객체 생성
	ComPtr<ID3D11PixelShader> ps;
	m_device->CreatePixelShader(stream.data(), stream.size(), nullptr, ps.GetAddressOf());

	// 맵에 저장
	m_psMap[_name] = ps;
	return IE::I_OK;
}

IE D3D11Renderer::CreateVertexIndexBuffer(const std::wstring& _name)
{
	HRESULT hr = S_OK;

	FILE_STREAM stream;
	bool isSuccess = FileOpenCallbackFunc(m_fms, _name, stream);
	if (false == isSuccess)
	{
		return IE::OPEN_FILE_FAIL;
	}

	uint64_t offset = 0;

	// 매쉬 갯수
	uint64_t meshCount;
	if (offset + sizeof(uint64_t) > stream.size())
	{
		return IE::OUT_OF_POINTER_BOUNDARY;
	}
	std::memcpy(&meshCount, stream.data() + offset, sizeof(uint64_t));
	offset += sizeof(uint64_t);

	for (uint64_t i = 0; i < meshCount; i++)
	{
		// 매쉬 정보 가져오기
		uint64_t nameSize;
		std::string name;

		// 이름의 크기 읽기
		if (offset + sizeof(uint64_t) > stream.size())
		{
			return IE::OUT_OF_POINTER_BOUNDARY;
		}
		std::memcpy(&nameSize, stream.data() + offset, sizeof(uint64_t));
		offset += sizeof(uint64_t);

		// 이름 읽기
		if (offset + nameSize > stream.size())
		{
			return IE::OUT_OF_POINTER_BOUNDARY;
		}
		name = std::string(stream.data() + offset, stream.data() + offset + nameSize);
		offset += name.size();


		// 이미 있는지 체크
		if (m_vBuffer.end() != m_vBuffer.find(_name))
		{
			continue;
		}

		// 정점 데이터의 크기
		uint64_t vertexCount;
		if (offset + sizeof(uint64_t) > stream.size())
		{
			return IE::OUT_OF_POINTER_BOUNDARY;
		}
		std::memcpy(&vertexCount, stream.data() + offset, sizeof(uint64_t));
		offset += sizeof(uint64_t);
		uint64_t vertexByteSize = sizeof(VertexPNTU) * vertexCount;

		// 정점 데이터
		if (offset + vertexByteSize > stream.size())
		{
			return IE::OUT_OF_POINTER_BOUNDARY;
		}
		std::vector<VertexPNTU> vertexData;
		vertexData.resize(vertexCount);
		std::memcpy(vertexData.data(), stream.data() + offset, vertexByteSize);
		offset += vertexByteSize;

		// 인덱스 데이터의 크기
		if (offset + sizeof(uint64_t) > stream.size())
		{
			return IE::OUT_OF_POINTER_BOUNDARY;
		}
		uint64_t indexCount;
		std::memcpy(&indexCount, stream.data() + offset, sizeof(uint64_t));
		offset += sizeof(uint64_t);
		uint64_t indexByteSize = sizeof(int) * indexCount;

		// 인덱스 데이터
		std::vector<int> indexData;
		indexData.resize(indexCount);
		std::memcpy(indexData.data(), stream.data() + offset, indexByteSize);
		offset += indexByteSize;

		D3D11_BUFFER_DESC vb = {};
		vb.Usage = D3D11_USAGE_IMMUTABLE;
		vb.ByteWidth = static_cast<UINT>(vertexByteSize);
		vb.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vb.CPUAccessFlags = 0;
		vb.MiscFlags = 0;
		vb.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA vInitData = {};
		vInitData.pSysMem = vertexData.data();

		hr = m_device->CreateBuffer(
			&vb
			, &vInitData
			, m_vBuffer[_name].first.GetAddressOf()
		);
		if (S_OK != hr)
		{
			return IE::CREATE_D3D_BUFFER_FAIL;
		}
		m_vBuffer[_name].second = sizeof(VertexPNTU);


		D3D11_BUFFER_DESC ib = {};
		ib.Usage = D3D11_USAGE_IMMUTABLE;
		ib.ByteWidth = static_cast<UINT>(indexByteSize);
		ib.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ib.CPUAccessFlags = 0;
		ib.MiscFlags = 0;
		ib.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA iInitData = {};
		iInitData.pSysMem = indexData.data();

		hr = m_device->CreateBuffer(
			&ib
			, &iInitData
			, m_iBuffer[_name].first.GetAddressOf()
		);
		m_iBuffer[_name].second = static_cast<UINT>(indexCount);

		if (S_OK != hr)
		{
			return IE::CREATE_D3D_BUFFER_FAIL;
		}
	}

	return IE::I_OK;
}

IE D3D11Renderer::AddRenderObject(std::shared_ptr<IRenderObject> _renderObject)
{
	m_renderVector.push_back(_renderObject);

	return IE::I_OK;
}

IE D3D11Renderer::AddModelObject(std::shared_ptr<IModelObject> _modelObject)
{
	for (auto itr : _modelObject->m_renderObjects)
	{
		AddRenderObject(itr);
	}

	return IE::I_OK;
}

IE D3D11Renderer::SetRenderSize(UINT _w, UINT _h)
{
	m_renderWidth = _w;
	m_renderHight = _h;

	return IE::I_OK;
}

IE D3D11Renderer::AddLight(const std::wstring& _name, const LightData& _lightData)
{
	HRESULT hr = S_OK;
	ComPtr<ID3D11Buffer> lightBuffer;
	IE ie = IE::I_OK;

	// 이미 있다면 만들 필요 없다
	if (m_lightMap.end() != m_lightMap.find(_name))
	{
		return IE::I_OK;
	}

	// 각 빛 타입에 맞는 버퍼 생성
	switch (_lightData.m_type)
	{
	case LIGHT_TYPE::DIRECTION:
	{
		if (m_dirctionLightVector.size() >= m_maxLightCount)
		{
			return IE::MAX_ELEMENT;
		}
		m_dirctionLightBuffer.isDirty = true;

		DirectionLightBufferData d;
		d.m_direction = _lightData.m_direction;
		d.m_color = _lightData.m_color;
		d.m_intensity = _lightData.m_intensity;

		m_dirctionLightVector.push_back(d);
		break;
	}
	case LIGHT_TYPE::POINT:
	{
		if (m_pointLightVector.size() >= m_maxLightCount)
		{
			return IE::MAX_ELEMENT;
		}
		m_pointLightBuffer.isDirty = true;

		PointLightBufferData d;
		d.m_position = _lightData.m_position;
		d.m_color = _lightData.m_color;
		d.m_attenuation = _lightData.m_attenuation;
		d.m_intensity = _lightData.m_intensity;
		d.m_range = _lightData.m_range;

		m_pointLightVector.push_back(d);
		break;
	}
	case LIGHT_TYPE::SPOTLIGHT:
	{
		if (m_spotLightVector.size() >= m_maxLightCount)
		{
			return IE::MAX_ELEMENT;
		}
		m_spotLightBuffer.isDirty = true;

		SpotLightBufferData d;
		d.m_direction = _lightData.m_direction;
		d.m_position = _lightData.m_position;
		d.m_color = _lightData.m_color;
		d.m_attenuation = _lightData.m_attenuation;
		d.m_inAngle = _lightData.m_inAngle;
		d.m_outAngle = _lightData.m_outAngle;
		d.m_intensity = _lightData.m_intensity;

		m_spotLightVector.push_back(d);
		break;
	}
	default:
	{
		return IE::WRONG_TYPE;
		break;
	}
	}

	return ie;
}

IE D3D11Renderer::ImguiInitialize(bool(*ImguiStartFunc)(ID3D11Device* _device, ID3D11DeviceContext* _deviceContext))
{
	if (nullptr == m_device
		|| nullptr == m_deviceContext)
	{
		return IE::NULL_POINTER_ACCESS;
	}

	bool isSuccess = ImguiStartFunc(m_device.Get(), m_deviceContext.Get());

	if (false == isSuccess)
	{
		return IE::IMGUI_FAIL;
	}

	return IE::I_OK;
}


IE D3D11Renderer::SetSkyVS(VERTEX_TYPE _type, const std::wstring& _vs)
{
	IE iok;
	iok = CreateVertexShader(_type, _vs);
	if (IE::I_OK !=iok)
	{
		return iok;
	}

	m_skyRenderSet.m_vs = m_vsMap[_vs].first;
	m_skyRenderSet.m_ia = m_vsMap[_vs].second;

	return IE::I_OK;
}

IE D3D11Renderer::SetSkyPS(const std::wstring& _ps)
{
	IE iok;
	iok = CreatePixelShader(_ps);
	if (IE::I_OK != iok)
	{
		return iok;
	}

	m_skyRenderSet.m_ps = m_psMap[_ps];

	return IE::I_OK;
}

IE D3D11Renderer::SetSkyTextuer(const TextuerData& _textuer)
{
	IE iok;
	iok = CreateTexture(_textuer);
	if (IE::I_OK != iok)
	{
		return iok;
	}

	m_skyTextuer = m_textuerMap[_textuer.m_name];

	return IE::I_OK;
}

IE D3D11Renderer::CreateDefaultTextuer(const Vector3& _diffuse, const Vector3& _normal, const Vector3& _roughness, const Vector3& _metalic)
{
	if (nullptr == m_device)
	{
		return IE::NULL_POINTER_ACCESS;
	}

	// 모든 텍스쳐가 공유 할 구조체
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = 1;
	texDesc.Height = 1;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	// default textier creator lamda
	auto Creator = [&](const Vector3& _color, ComPtr<ID3D11ShaderResourceView>& _targetSRV) -> IE
		{
			HRESULT hr = S_OK;

			// float 색을 0 ~ 255 사이로 스케일링
			unsigned char rawColor[4] = 
			{
				static_cast<unsigned char>(_color.x * 255.0f),
				static_cast<unsigned char>(_color.y * 255.0f),
				static_cast<unsigned char>(_color.z * 255.0f),
				255
			};

			ComPtr<ID3D11Texture2D> textuer;
			D3D11_SUBRESOURCE_DATA initData;

			initData.pSysMem = rawColor;
			initData.SysMemPitch = 4;
			hr = m_device->CreateTexture2D(&texDesc, &initData, textuer.GetAddressOf());
			if (S_OK != hr)
			{
				return IE::CREATE_D3D_TEXTURE_FAIL;
			}
			hr = m_device->CreateShaderResourceView(textuer.Get(), &srvDesc, _targetSRV.GetAddressOf());
			if (S_OK != hr)
			{
				return IE::CREATE_D3D_SRV_FAIL;
			}
			return IE::I_OK;
		};

	// create default src
	IE iok = IE::I_OK;
	iok = Creator(_diffuse, m_defaultDiffuse);
	if (IE::I_OK != iok)
	{
		return iok;
	}
	iok = Creator(_normal, m_defaultNormal);
	if (IE::I_OK != iok)
	{
		return iok;
	}
	iok = Creator(_roughness, m_defaultRoughess);
	if (IE::I_OK != iok)
	{
		return iok;
	}
	iok = Creator(_metalic, m_defaultMetalic);
	if (IE::I_OK != iok)
	{
		return iok;
	}

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

	// 오브젝트의 월드 값을 담을 버퍼
	CreateWorldBuffer();

	m_renderVector.reserve(_initalizeState.m_renderVectorSize);

	m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 빛의 최대 갯수 설정
	m_maxLightCount = _initalizeState.m_maxLightCount;

	// 각 빛의 종류에 따른 빛 버퍼 설정
	CreateStructedBuffer(
		sizeof(DirectionLightBufferData)
		, m_maxLightCount
		, m_dirctionLightBuffer.m_buffer
		, m_dirctionLightBuffer.m_srv
	);

	CreateStructedBuffer(
		sizeof(PointLightBufferData)
		, m_maxLightCount
		, m_pointLightBuffer.m_buffer
		, m_pointLightBuffer.m_srv
	);

	CreateStructedBuffer(
		sizeof(SpotLightBufferData)
		, m_maxLightCount
		, m_spotLightBuffer.m_buffer
		, m_spotLightBuffer.m_srv
	);

	// 빛 갯수를 담을 버퍼 설정
	CreateBuffer(sizeof(LightCountBuffer), m_lightCountBuffer);

	FileOpenCallbackFunc = _initalizeState.FileOpenCallbackFunc;

	m_fms = _initalizeState.m_fms;

	CreateSkySphereObject();

	CreateDefaultTextuer(
		_initalizeState.m_defaultTextuerSetting.m_diffuse
		, _initalizeState.m_defaultTextuerSetting.m_normal
		, _initalizeState.m_defaultTextuerSetting.m_roughness
		, _initalizeState.m_defaultTextuerSetting.m_metalic
	);

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

IE D3D11Renderer::CreateTexture(const TextuerData& _textuerData)
{
	HRESULT hr = S_OK;

	// 확장자 탐색. 가장 마지막 . 찾기
	size_t pos = _textuerData.m_name.find_last_of('.');
	if (pos == std::string::npos)
	{
		return IE::STREAM_ERROR;
	}

	// 확장자 분리
	std::wstring format = _textuerData.m_name.substr(pos + 1);
	std::transform(format.begin(), format.end(), format.begin(), ::tolower);

	ComPtr<ID3D11ShaderResourceView> albedoSrv;
	if (L"dds" == format)
	{
		hr = DirectX::CreateDDSTextureFromMemory(
			m_device.Get()
			, reinterpret_cast<const uint8_t*>(_textuerData.m_data.data())
			, _textuerData.m_data.size()
			, nullptr
			, albedoSrv.GetAddressOf()
		);
	}
	else if (L"png" == format)
	{
		hr = DirectX::CreateWICTextureFromMemory(
			m_device.Get()
			, reinterpret_cast<const uint8_t*>(_textuerData.m_data.data())
			, _textuerData.m_data.size()
			, nullptr
			, albedoSrv.GetAddressOf()
		);
	}
	else
	{
		return IE::UNDEFINED_FILE_EXTENSION;
	}

	if (false == SUCCEEDED(hr))
	{
		return IE::CREATE_D3D_TEXTURE_FAIL;
	}

	m_textuerMap[_textuerData.m_name] = albedoSrv;

	return IE::I_OK;
}

IE D3D11Renderer::CreateMaterial(const std::wstring _name, const MaterialData& _material)
{
	if (nullptr == m_device)
	{
		return IE::NULL_POINTER_ACCESS;
	}

	if (m_materialMap.end() != m_materialMap.find(_name))
	{
		return IE::I_OK;
	}

	HRESULT hr = S_OK;



	IE ie = CreateTexture(_material.m_albedo);

	if (IE::I_OK != ie)
	{
		return ie;
	}

	m_materialMap[_name].m_albedo = m_textuerMap[_material.m_albedo.m_name];

	return IE::I_OK;
}

std::weak_ptr<ICamera> D3D11Renderer::CreateCamera()
{
	m_cameras.push_back(std::make_shared<Camera>());
	m_cameras.back()->SetIndex(static_cast<int>(m_cameras.size()) - 1);

	return m_cameras.back();
}

IE D3D11Renderer::SetCamera(std::weak_ptr<ICamera> _cameraID)
{
	m_mainCamera = std::static_pointer_cast<Camera>(_cameraID.lock());

	if (m_mainCamera.expired())
	{
		return IE::NULL_POINTER_ACCESS;
	}

	return IE::I_OK;
}

IE D3D11Renderer::SetAniamtion(int _meshID)
{
	return IE::I_OK;
}

IE D3D11Renderer::Draw(std::function<void()> ImguiRender)
{
	IE result = IE::I_OK;

	// null check
	if (nullptr == m_swapChain
		|| true == m_mainCamera.expired())
	{
		return IE::NULL_POINTER_ACCESS;
	}

	// 메인 카메라 행렬 계산
	if (m_mainCamera.lock()->Caculate())
	{
		// 계산을 했다면 바인딩
		BindMainCameraBuffer();
	}

	// 빛 데이터 바인딩
	// 뭔가 맘에 안드는데 좀 더 좋은 방법이 있는지 생각해봐야할듯
	if (true == m_dirctionLightBuffer.isDirty)
	{
		m_dirctionLightBuffer.isDirty = false;
		BindDataBuffer(
			m_dirctionLightBuffer.m_buffer
			, m_dirctionLightVector.data()
			, sizeof(DirectionLightBufferData) * m_dirctionLightVector.size()
		);

		m_deviceContext->PSSetShaderResources(4, 1, m_dirctionLightBuffer.m_srv.GetAddressOf());
	}

	if (true == m_pointLightBuffer.isDirty)
	{
		m_pointLightBuffer.isDirty = false;
		BindDataBuffer(
			m_pointLightBuffer.m_buffer
			, m_pointLightVector.data()
			, sizeof(DirectionLightBufferData) * m_pointLightVector.size()
		);
		m_deviceContext->PSSetShaderResources(5, 1, m_pointLightBuffer.m_srv.GetAddressOf());
	}

	if (true == m_spotLightBuffer.isDirty)
	{
		m_spotLightBuffer.isDirty = false;
		BindDataBuffer(
			m_spotLightBuffer.m_buffer
			, m_spotLightVector.data()
			, sizeof(DirectionLightBufferData) * m_spotLightVector.size()
		);
		m_deviceContext->PSSetShaderResources(6, 1, m_spotLightBuffer.m_srv.GetAddressOf());
	}

	// 빛의 총 갯수가 바뀌면 새로 바인딩
	if (m_lightCountData.m_numDirection != m_dirctionLightVector.size()
		|| m_lightCountData.m_numPoint != m_pointLightVector.size()
		|| m_lightCountData.m_numSpot != m_spotLightVector.size())
	{
		m_lightCountData.m_numDirection = static_cast<int>(m_dirctionLightVector.size());
		m_lightCountData.m_numPoint = static_cast<int>(m_pointLightVector.size());
		m_lightCountData.m_numSpot = static_cast<int>(m_spotLightVector.size());

		BindDataBuffer(
			m_lightCountBuffer
			, &m_lightCountData
			, sizeof(m_lightCountData)
		);

		m_deviceContext->PSSetConstantBuffers(2, 1, m_lightCountBuffer.GetAddressOf());
	}

	// 스카이 박스 렌더링
	// 요소 중 하나라도 없으면 할 필요가 없다
	if (nullptr != m_skyRenderSet.m_vs
		&& nullptr != m_skyRenderSet.m_ia
		&& nullptr != m_skyRenderSet.m_vb
		&& nullptr != m_skyRenderSet.m_ps
		&& nullptr != m_skyRenderSet.m_ib
		&& nullptr != m_skyTextuer
		&& 0 < m_skyIndexSize)
	{
		BindVertexShaderAndInputLayout(m_skyRenderSet.m_vs, m_skyRenderSet.m_ia);
		BindPixelShader(m_skyRenderSet.m_ps);
		m_deviceContext->PSSetShaderResources(7, 1, m_skyTextuer.GetAddressOf());
		BindVertexBuffer(m_skyRenderSet.m_vb, sizeof(VertexPU));
		BindIndexBuffer(m_skyRenderSet.m_ib);
		m_deviceContext->DrawIndexed(m_skyIndexSize, 0, 0);
	}

	if (false == m_renderVector.empty())
	{
		// 렌더링 할 오브젝트 정렬
		std::sort(m_renderVector.begin(), m_renderVector.end());
		for (auto& itr : m_renderVector)
		{
			// 정렬 된 후 이 이하는 그리지 않는다.
			if (false == itr->m_isDraw)
			{
				break;
			}

			// 일단 있는거만 가져와서 바인딩 한다.
			// 아직 텍스쳐는 없음

			// 정점 셰이더 바인딩
			auto vs = m_vsMap.find(itr->m_vertexShader);
			if (m_vsMap.end() == vs)
			{
				continue;
			}
			result = BindVertexShaderAndInputLayout(vs->second.first, vs->second.second);
			if (IE::I_OK != result)
			{
				return result;
			}

			auto ps = m_psMap.find(itr->m_pixelShader);
			if (m_psMap.end() == ps)
			{
				continue;
			}
			BindPixelShader(ps->second);
			if (IE::I_OK != result)
			{
				return result;
			}

			const Material& mat = m_materialMap.find(itr->m_material)->second;
			m_deviceContext->PSSetShaderResources(0, 1, mat.m_albedo.GetAddressOf());

			auto vb = m_vBuffer.find(itr->m_mesh);
			if (m_vBuffer.end() == vb)
			{
				continue;
			}
			BindVertexBuffer(vb->second.first, vb->second.second);
			if (IE::I_OK != result)
			{
				return result;
			}

			auto ib = m_iBuffer.find(itr->m_mesh);
			if (m_iBuffer.end() == ib)
			{
				continue;
			}
			BindIndexBuffer(ib->second.first);
			if (IE::I_OK != result)
			{
				return result;
			}

			// 월드 행렬은 대부분 다르니 걍 바인딩
			BindWorldBuffer(itr->m_meshtransform);

			m_deviceContext->DrawIndexed(ib->second.second, 0, 0);
		}
	}

	if (nullptr != ImguiRender)
	{
		ImguiRender();
	}

	m_swapChain->Present(0, 0);

	result = ClearScreen();
	if (IE::I_OK != result)
	{
		return result;
	}

	return result;
}
