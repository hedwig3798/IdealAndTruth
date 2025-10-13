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
	, m_currentRenderSet()
{

}

D3D11Renderer::~D3D11Renderer()
{
	m_allocators.clear();
}

IE D3D11Renderer::CreateD3D11DeviceContext(const InitializeState::Device& _device)
{
	HRESULT hr = S_OK;

	// D3D11 ����̽� ����
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

	// ����  ü�� ����ü
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

	// ����̽� �������̽� ��������
	IDXGIDevice* dxgiDevice = nullptr;
	hr |= (m_device->QueryInterface(
		__uuidof(IDXGIDevice),
		reinterpret_cast<void**>(&dxgiDevice)));

	if (nullptr == dxgiDevice)
	{
		return IE::CREATE_D3D_SWAP_CHAIN_FAIL;
	}

	// ����̽� ��� ��������
	IDXGIAdapter* dxgiAdapter = nullptr;
	hr |= (dxgiDevice->GetParent(
		__uuidof(IDXGIAdapter),
		reinterpret_cast<void**>(&dxgiAdapter)));

	if (nullptr == dxgiAdapter)
	{
		return IE::CREATE_D3D_SWAP_CHAIN_FAIL;
	}

	// ����̽� ���丮 ��������
	IDXGIFactory* dxgiFactory = nullptr;
	hr |= (dxgiAdapter->GetParent(
		__uuidof(IDXGIFactory),
		reinterpret_cast<void**>(&dxgiFactory)));

	if (nullptr == dxgiFactory)
	{
		return IE::CREATE_D3D_SWAP_CHAIN_FAIL;
	}

	// ���� ü�� ����
	hr |= (dxgiFactory->CreateSwapChain(m_device.Get(), &chainDesc, m_swapChain.GetAddressOf()));

	if (nullptr == m_swapChain)
	{
		return IE::CREATE_D3D_SWAP_CHAIN_FAIL;
	}

	// ����� �������̽� ����
	dxgiDevice->Release();
	dxgiAdapter->Release();
	dxgiFactory->Release();

	return IE::I_OK;
}

IE D3D11Renderer::CreateFinalRenterTargetView()
{
	ID3D11Texture2D* backBuffer = nullptr;
	HRESULT hr = S_OK;

	// ����ó��
	if (nullptr == m_swapChain
		|| nullptr == m_device
		|| nullptr == m_deviceContext
		)
	{
		return IE::NULL_POINTER_ACCESS;
	}

	// ���� ü�ο��� ���۸� ������
	hr |= m_swapChain->GetBuffer(
		0,
		__uuidof(ID3D11Texture2D),
		reinterpret_cast<void**>(&backBuffer)
	);

	if (nullptr == backBuffer)
	{
		return IE::CREATE_D3D_BUFFER_FAIL;
	}

	// ���� Ÿ�� �� ����
	hr |= m_device->CreateRenderTargetView(
		backBuffer,
		0,
		m_finalRenderTargetView.GetAddressOf()
	);

	// ����ó��
	if (nullptr == m_finalRenderTargetView)
	{
		return IE::CREATE_D3D_RENDER_TARGET_FAIL;
	}

	// ����� �� ���� �������̽� ��ȯ	
	hr |= backBuffer->Release();

	return IE::I_OK;
}

IE D3D11Renderer::CreateDepthStencilBufferAndView(const InitializeState::DepthStancil& _depthStancil)
{
	HRESULT hr = S_OK;

	// ����ó��
	if (nullptr == m_device)
	{
		return IE::NULL_POINTER_ACCESS;
	}

	// ����ü �� ä���
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

	// ���� ���ٽ� ���� ����
	hr |= m_device->CreateTexture2D(
		&depthStancilDesc,
		nullptr,
		m_depthStancilBuffer.GetAddressOf()
	);

	// ���� ���ٽ� �� ����
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
	// ����ó��
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

	// ����ó��
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
	// ����ó��
	if (nullptr == m_deviceContext
		|| nullptr == m_finalRenderTargetView
		|| nullptr == m_depthStancilView)
	{
		return IE::NULL_POINTER_ACCESS;
	}

	// ���� Ÿ���� ������ ������ �ʱ�ȭ
	m_deviceContext->ClearRenderTargetView(
		m_finalRenderTargetView.Get(),
		m_defaultBG
	);

	// ���� ���ٽ� �� �ʱ�ȭ
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

IE D3D11Renderer::CreateWorldBuffer()
{
	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC mbd = {};
	mbd.Usage = D3D11_USAGE_DYNAMIC;
	mbd.ByteWidth = sizeof(WorldBuffer);
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

	CameraBuffer* dataptr = (CameraBuffer*)mappedResource.pData;
	dataptr->m_view = m_mainCamera.lock()->GetViewMatrix().Transpose();
	dataptr->m_proj = m_mainCamera.lock()->GetProjMatrix().Transpose();

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

	WorldBuffer* dataptr = (WorldBuffer*)mappedResource.pData;
	dataptr->m_world = _matrix.Transpose();

	m_deviceContext->VSSetConstantBuffers(0, 1, m_worldBuffer.GetAddressOf());
	m_deviceContext->Unmap(m_worldBuffer.Get(), 0);

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

	// IA ������ ���� �ʿ��� ���
	D3D11_INPUT_ELEMENT_DESC* decs = nullptr;
	ComPtr<ID3DBlob> blob;
	UINT eleNum = 0;

	// ���� IA ���۰� ���ٸ� ����
	if (0 == m_iaBuffer.size())
	{
		m_iaBuffer.resize(static_cast<int>(VERTEX_TYPE::END));
		for (auto& ia : m_iaBuffer)
		{
			ia = nullptr;
		}
	}

	// ���̴� ���� ��������
	hr = ::D3DCreateBlob(_stream.size(), blob.GetAddressOf());
	if (S_OK != hr)
	{
		return IE::CREATE_D3D_BLOB_FAIL;
	}
	std::memcpy(blob->GetBufferPointer(), _stream.data(), _stream.size());

	// ���� Ÿ�Կ� ���� �ٸ���
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
	// Ÿ���� ������ �׳� ���� Ÿ�� ����
	default:
	{
		decs = VertexSuper::m_IADesc;
		eleNum = 7;
		break;
	}
	}

	// �� üũ
	if (decs == nullptr
		|| m_device == nullptr)
	{
		return IE::NULL_POINTER_ACCESS;
	}

	// IA �����
	hr = m_device->CreateInputLayout(
		decs
		, eleNum
		, blob->GetBufferPointer()
		, blob->GetBufferSize()
		, m_iaBuffer[static_cast<int>(_type)].GetAddressOf()
	);

	// IA üũ
	if (S_OK != hr)
	{
		return IE::CREATE_D3D_INPUT_LAYOUT_FAIL;
	}

	return IE::I_OK;
}

IE D3D11Renderer::CreateVertexShader(VERTEX_TYPE _type, const std::string& _name, const std::vector<unsigned char>& _stream)
{
	// ���� �̸��� ���̴��� �ִٸ� ��������.
	auto mit = m_vsMap.find(_name);
	if (mit != m_vsMap.end())
	{
		return IE::I_OK;
	}

	// �� üũ
	if (true == _stream.empty()
		|| nullptr == m_device)
	{
		return IE::NULL_POINTER_ACCESS;
	}

	// ���� ���̴� ��ü ����
	ComPtr<ID3D11VertexShader> vs;
	m_device->CreateVertexShader(_stream.data(), _stream.size(), nullptr, vs.GetAddressOf());

	// �ʿ� ����
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
		result = CreateInputLayout(_type, _stream);
		if (IE::I_OK != result)
		{
			return result;
		}
		m_vsMap[_name].second = m_iaBuffer[static_cast<int>(_type)];
	}

	return IE::I_OK;
}

IE D3D11Renderer::CreatePixelShader(const std::string& _name, const std::vector<unsigned char>& _stream)
{
	// ���� �̸��� ���̴��� �ִٸ� ��������.
	auto mit = m_psMap.find(_name);
	if (mit != m_psMap.end())
	{
		return IE::I_OK;
	}

	// �� üũ
	if (true == _stream.empty()
		|| nullptr == m_device)
	{
		return IE::NULL_POINTER_ACCESS;
	}

	// ���� ���̴� ��ü ����
	ComPtr<ID3D11PixelShader> ps;
	m_device->CreatePixelShader(_stream.data(), _stream.size(), nullptr, ps.GetAddressOf());

	// �ʿ� ����
	m_psMap[_name] = ps;
	return IE::I_OK;
}

IE D3D11Renderer::CreateVertexIndexBuffer(std::string _name, const std::vector<unsigned char>& _stream)
{
	if (m_vBuffer.end() != m_vBuffer.find(_name)
		&& m_iBuffer.end() != m_iBuffer.find(_name))
	{
		return IE::I_OK;
	}

	HRESULT hr = S_OK;

	UINT offset = 0;
	int dSize;
	std::memcpy(&dSize, _stream.data() + offset, sizeof(int));
	offset += sizeof(int);

	int vSize;
	std::memcpy(&vSize, _stream.data() + offset, sizeof(int));
	offset += sizeof(int);

	int iSize;
	std::memcpy(&iSize, _stream.data() + offset, sizeof(int));
	offset += sizeof(int);


	const unsigned char* vBuffer = _stream.data() + offset;
	offset += vSize;

	const unsigned char* iBuffer = _stream.data() + offset;
	offset += iSize;

	D3D11_BUFFER_DESC vb = {};
	vb.Usage = D3D11_USAGE_IMMUTABLE;
	vb.ByteWidth = vSize;
	vb.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vb.CPUAccessFlags = 0;
	vb.MiscFlags = 0;
	vb.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vInitData = {};
	vInitData.pSysMem = vBuffer;

	hr = m_device->CreateBuffer(
		&vb
		, &vInitData
		, m_vBuffer[_name].first.GetAddressOf()
	);
	if (S_OK != hr)
	{
		return IE::CREATE_D3D_BUFFER_FAIL;
	}
	m_vBuffer[_name].second = dSize;


	D3D11_BUFFER_DESC ib = {};
	ib.Usage = D3D11_USAGE_IMMUTABLE;
	ib.ByteWidth = iSize;
	ib.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ib.CPUAccessFlags = 0;
	ib.MiscFlags = 0;
	ib.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA iInitData = {};
	iInitData.pSysMem = iBuffer;

	hr = m_device->CreateBuffer(
		&ib
		, &iInitData
		, m_iBuffer[_name].first.GetAddressOf()
	);
	m_iBuffer[_name].second = iSize / sizeof(UINT);

	if (S_OK != hr)
	{
		return IE::CREATE_D3D_BUFFER_FAIL;
	}

	return IE::I_OK;
}

IE D3D11Renderer::AddRenderObject(const IRenderObject& _renderObject)
{
	// �ϴ� �����ؼ� ��������
	// ���߿� �����ͷ� �ٲ���ҵ�?
	m_renderVector.push_back(_renderObject);

	return IE::I_OK;
}

IE D3D11Renderer::SetRenderSize(UINT _w, UINT _h)
{
	m_renderWidth = _w;
	m_renderHight = _h;

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

	// ���� ���� �並 ���ε� �Ѵ�
	m_deviceContext->OMSetRenderTargets(
		1,
		m_finalRenderTargetView.GetAddressOf(),
		m_depthStancilView.Get()
	);

	// Ư�� ������ ȭ���� �ʱ�ȭ�Ѵ�.
	ClearScreen();

	// ī�޶� ���� ����
	CreateCameraBuffer();

	// ������Ʈ�� ���� ���� ���� ����
	CreateWorldBuffer();

	m_renderVector.reserve(_initalizeState.m_renderVectorSize);

	m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

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
	if (nullptr == m_swapChain
		|| true == m_mainCamera.expired())
	{
		return IE::NULL_POINTER_ACCESS;
	}

	// ���� ī�޶� ��� ���
	if (m_mainCamera.lock()->Caculate())
	{
		// ����� �ߴٸ� ���ε�
		BindMainCameraBuffer();
	}

	if (false == m_renderVector.empty())
	{
		// ������ �� ������Ʈ ����
		std::sort(m_renderVector.begin(), m_renderVector.end());
		for (auto& itr : m_renderVector)
		{
			// ���� �� �� �� ���ϴ� �׸��� �ʴ´�.
			if (false == itr.m_isDraw)
			{
				break;
			}

			// �ϴ� �ִ°Ÿ� �����ͼ� ���ε� �Ѵ�.
			// ���� �ؽ��Ĵ� ����

			// ���� ���̴� ���ε�
			auto vs = m_vsMap.find(itr.m_vertexShader);
			if (m_vsMap.end() == vs)
			{
				continue;
			}
			result = BindVertexShaderAndInputLayout(vs->second.first, vs->second.second);
			if (IE::I_OK != result)
			{
				return result;
			}

			auto ps = m_psMap.find(itr.m_pixelShader);
			if (m_psMap.end() == ps)
			{
				continue;
			}
			BindPixelShader(ps->second);
			if (IE::I_OK != result)
			{
				return result;
			}

			auto vb = m_vBuffer.find(itr.m_mesh);
			if (m_vBuffer.end() == vb)
			{
				continue;
			}
			BindVertexBuffer(vb->second.first, vb->second.second);
			if (IE::I_OK != result)
			{
				return result;
			}

			auto ib = m_iBuffer.find(itr.m_mesh);
			if (m_iBuffer.end() == ib)
			{
				continue;
			}
			BindIndexBuffer(ib->second.first);
			if (IE::I_OK != result)
			{
				return result;
			}

			// ���� ����� ��κ� �ٸ��� �� ���ε�
			BindWorldBuffer(itr.m_world);

			m_deviceContext->DrawIndexed(ib->second.second, 0, 0);
		}
	}

	m_swapChain->Present(0, 0);

	result = ClearScreen();
	if (IE::I_OK != result)
	{
		return result;
	}

	return result;
}
