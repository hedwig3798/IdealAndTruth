#include "DemoProcess.h"
#include "stringUtil.h"

bool DemoProcess::m_mouseDown[2] = { false, false };
int DemoProcess::m_currMouseMove[2] = { 0, 0 };
int DemoProcess::m_oldMouseMove[2] = { 0, 0 };
int DemoProcess::m_mouseMovement[2] = { 0, 0 };

DemoProcess::DemoProcess()
	: m_renderer(nullptr)
	, m_hwnd(nullptr)
	, m_managers(nullptr)
	, m_fms()
	, m_luaState(nullptr)
{
	m_mouseDown[0] = false;
	m_mouseDown[1] = false;

	m_tempRender = std::make_shared<IRenderer::IRenderObject>();
	m_tempModel = std::make_shared<IRenderer::IModelObject>();
}

DemoProcess::~DemoProcess()
{
	delete m_renderer;
	// delete m_managers;
}

void DemoProcess::Initialize(HWND _hwnd)
{
	if (NULL == _hwnd)
	{
		std::cout << "window handler error\n";
		return;
	}

	FMSSetting();
	LuaSetting();

	m_hwnd = _hwnd;

	CreateRendererState();

	HMODULE m_rendererDll = ::LoadLibraryA("./IdealGraphics.dll");
	if (nullptr == m_rendererDll)
	{
		return;
	}
	m_renderer = ((IRenderer * (*)())GetProcAddress(m_rendererDll, "CreateD3D11Renderer"))();

	m_renderer->SetRenderSize(m_renderWidth, m_renderHight);

	IE_ASSERT(
		m_renderer->Initialize(m_rendererState, m_hwnd)
		, "Renderer Initialize Fail"
	);

	IE_ASSERT(
		m_renderer->SetBackgroundColor(0, 0, 0, 1)
		, "Set Background Color Fail"
	);

	FILE_STREAM tempvs;
	m_fms.OpenFile(L"DefaultVS.cso", tempvs);

	IE_ASSERT(
		m_renderer->CreateVertexShader(IRenderer::VERTEX_TYPE::VertexPUN, "DefaultVS", tempvs)
		, "Cannot Create VertexShader"
	);


	FILE_STREAM tempps;
	m_fms.OpenFile(L"DefaultPS.cso", tempps);
	IE_ASSERT(
		m_renderer->CreatePixelShader("DefaultPS", tempps)
		, "Cannot Create PixelShader"
	);

	m_camera = m_renderer->CreateCamera();
	IE_ASSERT(
		m_renderer->SetCamera(m_camera)
		, "Cannot Set Camera"
	);

	m_camera.lock()->SetAspectRatio(static_cast<float>(m_renderWidth) / static_cast<float>(m_renderHight));

	std::vector<unsigned char> tempVertex;
	m_fms.OpenFile(L"gun.iver", tempVertex);
	std::string meshName;
	IE_ASSERT(
		m_renderer->CreateVertexIndexBuffer(tempVertex, meshName)
		, "Cannot Create VertexBuffer"
	);

	CreateMaterial(L"GunMaterial.lua");

	m_tempRender->m_isDraw = true;
	m_tempRender->m_mesh = meshName;
	m_tempRender->m_vertexShader = "DefaultVS";
	m_tempRender->m_pixelShader = "DefaultPS";
	m_tempRender->m_world = Matrix::CreateScale(0.001f);
	m_tempRender->m_material = "GunMaterial.lua";
	
	m_tempModel->m_isDraw = true;
	m_tempModel->m_renderObjects.push_back(m_tempRender);

	m_renderer->AddModelObject(m_tempModel);
}

void DemoProcess::Process()
{
	Update();
	Render();
}

void DemoProcess::Update()
{
	// 임시 카메라 움직임 제어
	// 나중에 매니져 클래스로 뺄 예정
	if (true == m_camera.expired())
	{
		return;
	}

	ICamera* camera = m_camera.lock().get();

	// 카메라 이동
	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		camera->MoveForward(0.001f);
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		camera->MoveForward(-0.001f);
	}
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		camera->MoveRight(-0.001f);
	}
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		camera->MoveRight(0.001f);
	}

	// 카메라 회전
	if (GetAsyncKeyState('W') & 0x8000)
	{
		camera->MoveForward(0.001f);
	}
	if (GetAsyncKeyState('S') & 0x8000)
	{
		camera->MoveForward(-0.001f);
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		camera->MoveRight(0.001f);
	}
	if (GetAsyncKeyState('A') & 0x8000)
	{
		camera->MoveRight(-0.001f);
	}

	if (GetAsyncKeyState('E') & 0x8000)
	{
		camera->MoveUp(0.001f);
	}
	if (GetAsyncKeyState('Q') & 0x8000)
	{
		camera->MoveUp(-0.001f);
	}

	if (true == m_mouseDown[0])
	{
		camera->RotateRight(-m_mouseMovement[1] * 0.0009f);
		camera->RotateUp(-m_mouseMovement[0] * 0.0009f);

		m_mouseMovement[0] = 0;
		m_mouseMovement[1] = 0;
	}
}

void DemoProcess::Render()
{
	IE_ASSERT(
		m_renderer->Draw()
		, "Draw Error"
	);

	// this->graphicsEngine->begineDraw();
	// this->object->Render(graphicsEngine);
	// 
	// std::wstring dt = L"DeltaTime : ";
	// dt += std::to_wstring(this->m_managers->timeManager->GetfDT());
	// dt += L"\n";
	// dt += L"FPS : ";
	// dt += std::to_wstring(1 / this->m_managers->timeManager->GetfDT());
	// 
	// float w[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	// 
	// this->graphicsEngine->endDraw();
	// 
	// int test = 0;
}

void DemoProcess::CreateRendererState()
{
	std::vector<unsigned char> s;
	m_fms.OpenFile(L"D3DSetting.lua", s);
	if (true == s.empty())
	{
		return;
	}

	// 초기 설정 불러오기
	DO_STREAM(m_luaState, s);

	// 윈도우 설정
	GET_LUA_TABLE_NEW(m_luaState, windowTable, "WindowSetting");
	GET_VALUE_NEW(windowTable, width, "Width", int);
	GET_VALUE_NEW(windowTable, height, "Height", int);
	ResizeWindow(width, height);

	m_renderWidth = width;
	m_renderHight = height;

	GET_LUA_TABLE_NEW(m_luaState, stateTable, "RendererInitializeState");
	GET_VALUE(stateTable, m_rendererState.m_renderVectorSize, "RenderVectorSize", int);

	// 디바이스 생성
	IRenderer::InitializeState::Device deviceState = {};
	deviceState.m_deviceFlags = 0;
	GET_VALUE_NEW(stateTable, device, "Device", lua_tinker::table);
	for (size_t i = 1; i <= device.size(); i++)
	{
		GET_VALUE_NEW(device, flag, static_cast<int>(i), UINT);
		deviceState.m_deviceFlags |= flag;
	}
	m_rendererState.m_device = deviceState;

	// 뎁스 스텐실 생성
	IRenderer::InitializeState::DepthStancil depthState = {};
	GET_VALUE_NEW(stateTable, depthStancil, "DepthStancil", lua_tinker::table);

	depthState.m_width = width;
	depthState.m_height = height;
	GET_VALUE(depthStancil, depthState.m_mipLevel, "mipLevel", UINT);
	GET_VALUE(depthStancil, depthState.m_arraySize, "arraySize", UINT);
	GET_VALUE(depthStancil, depthState.m_format, "format", UINT);
	GET_VALUE(depthStancil, depthState.m_sampleCount, "sampleCount", UINT);
	GET_VALUE(depthStancil, depthState.m_sampleQuality, "sampleQuality", UINT);
	GET_VALUE(depthStancil, depthState.m_usage, "usage", UINT);
	m_rendererState.m_depthStancil = depthState;

	// 스왑 체인 생성
	IRenderer::InitializeState::SwapCahin swapState = {};
	GET_VALUE_NEW(stateTable, swapCahin, "SwapCahin", lua_tinker::table);

	swapState.m_width = width;
	swapState.m_height = height;
	GET_VALUE(swapCahin, swapState.m_refreshRateNumerator, "refreshRateNumerator", UINT);
	GET_VALUE(swapCahin, swapState.m_refreshRateDenominator, "refreshRateDenominator", UINT);
	GET_VALUE(swapCahin, swapState.m_format, "format", UINT);
	GET_VALUE(swapCahin, swapState.m_scanlineOrdering, "scanlineOrdering", UINT);
	GET_VALUE(swapCahin, swapState.m_scaling, "scaling", UINT);
	GET_VALUE(swapCahin, swapState.m_sampleCount, "sampleCount", UINT);
	GET_VALUE(swapCahin, swapState.m_sampleQuality, "sampleQuality", UINT);
	GET_VALUE(swapCahin, swapState.m_bufferCount, "bufferCount", UINT);
	GET_VALUE(swapCahin, swapState.m_swapEffect, "swapEffect", UINT);
	GET_VALUE(swapCahin, swapState.m_isWindowed, "isWindowed", bool);
	m_rendererState.m_swapChain = swapState;

	// 레스터라이저
	IRenderer::InitializeState::RaseterizerState rasterState = {};
	GET_VALUE_NEW(stateTable, rasterizser, "RaseterizerState", lua_tinker::table);

	GET_VALUE(rasterizser, rasterState.m_fillMode, "fillMode", UINT);
	GET_VALUE(rasterizser, rasterState.m_cullMode, "cullMode", UINT);
	GET_VALUE(rasterizser, rasterState.m_isFrontCCW, "isFrontCCW", bool);
	GET_VALUE(rasterizser, rasterState.m_isDepthClip, "isDepthClip", bool);
	m_rendererState.m_rasterizer = rasterState;

	IRenderer::InitializeState::RenderTargetViewState renderState = {};
	GET_VALUE_NEW(stateTable, renderTarget, "RenderTargetViewState", lua_tinker::table);

	renderState.m_width = width;
	renderState.m_height = height;
	GET_VALUE(renderTarget, renderState.m_mipLevel, "mipLevel", UINT);
	GET_VALUE(renderTarget, renderState.m_arraySize, "arraySize", UINT);
	GET_VALUE(renderTarget, renderState.m_format, "format", UINT);
	GET_VALUE(renderTarget, renderState.m_sampleCount, "sampleCount", UINT);
	GET_VALUE(renderTarget, renderState.m_usage, "usage", UINT);
	GET_VALUE(renderTarget, renderState.m_viewDimension, "viewDimension", UINT);

	GET_VALUE_NEW(renderTarget, binflags, "bindFlags", lua_tinker::table);
	renderState.m_bindFlags = 0;
	for (size_t i = 1; i <= device.size(); i++)
	{
		GET_VALUE_NEW(binflags, flag, static_cast<int>(i), UINT);
		renderState.m_bindFlags |= flag;
	}
	m_rendererState.m_renderTargetView = renderState;
}

void DemoProcess::FMSSetting()
{
	std::string exePath = ::GetExeDirectoryPath();
	std::wstring resPath = ::StrToWstr(exePath) + L"\\..\\..\\Resource";
	std::wstring resOutputPath = ::StrToWstr(exePath) + L"\\..\\Resource";

	m_fms.SetThreadCount(4);
	m_fms.SetChunkSize(1024);
	m_fms.SetOutputFileName(L"IRenderer_");
	m_fms.SetCompressExtension(L".rcom");
	m_fms.SetCompressFilePath(resOutputPath);

	if (false == m_fms.CompressAll(resPath))
	{
		return;
	}
}

void DemoProcess::LuaSetting()
{
	m_luaState = ::luaL_newstate();
	::luaL_openlibs(m_luaState);

	D3DSetting();
}

void DemoProcess::D3DSetting()
{
	if (nullptr == m_luaState)
	{
		return;
	}

	std::vector<unsigned char> enumSetting;
	m_fms.OpenFile(L"D3DEnum.lua", enumSetting);
	if (true == enumSetting.empty())
	{
		std::cout << "cannot open D3DEnum.lua\n";
		return;
	}

	DO_STREAM(m_luaState, enumSetting);
}

void DemoProcess::ResizeWindow(int _width, int _hight)
{
	if (NULL == m_hwnd)
	{
		return;
	}

	RECT nowRect;
	GetWindowRect(m_hwnd, &nowRect);

	DWORD style = (DWORD)GetWindowLong(m_hwnd, GWL_STYLE);
	DWORD exstyle = (DWORD)GetWindowLong(m_hwnd, GWL_EXSTYLE);

	RECT newRect = {};
	newRect.left = 0;
	newRect.top = 0;
	newRect.right = _width;
	newRect.bottom = _hight;

	AdjustWindowRect(&newRect, style, NULL);

	// 클라이언트 영역보다 윈도 크기는 더 커야 한다. (외곽선, 타이틀 등)
	int _newWidth = (newRect.right - newRect.left);
	int _newHeight = (newRect.bottom - newRect.top);

	SetWindowPos(
		m_hwnd
		, HWND_NOTOPMOST
		, nowRect.left
		, nowRect.top
		, _newWidth
		, _newHeight
		, SWP_SHOWWINDOW
	);
}

void DemoProcess::CreateMaterial(const std::wstring& _path)
{
	if (nullptr == m_renderer)
	{
		return;
	}

	FILE_STREAM s;
	m_fms.OpenFile(_path, s);
	if (true == s.empty())
	{
		return;
	}

	// 텍스쳐 정보 가져오기
	DO_STREAM(m_luaState, s);

	FILE_STREAM albedoStream = FILE_STREAM();
	GET_LUA_TABLE_NEW(m_luaState, textuers, "Textuers");
	GET_VALUE_NEW(textuers, albedo, "albedo", std::string);
	if (0 != albedo.size())
	{
		m_fms.OpenFile(::StrToWstr(albedo), albedoStream);
	}
	IRenderer::TextuerData albetotextuer{ albedo, albedoStream };

	IRenderer::MaterialData material{ {0,0,0,0},  albetotextuer };

	m_renderer->CreateMaterial(::WstrToStr(_path), material);

	return;
}

LRESULT CALLBACK DemoProcess::WndProc(
	HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam
)
{
	// HDC			hdc;
	// PAINTSTRUCT ps;

	switch (message)
	{
		//case WM_PAINT:
		//	hdc = BeginPaint(hWnd, &ps);
		//	EndPaint(hWnd, &ps);
		//	break;
		//
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

		//
	case WM_LBUTTONDOWN:
	{
		m_mouseDown[0] = true;
		break;
	}
	case WM_RBUTTONDOWN:
	{
		m_mouseDown[1] = true;
		break;
	}
	case WM_LBUTTONUP:
	{
		m_mouseDown[0] = false;
		break;
	}
	case WM_RBUTTONUP:
	{
		m_mouseDown[1] = false;
		break;
	}
	case WM_MOUSEMOVE:
	{
		m_currMouseMove[0] = HIWORD(lParam);
		m_currMouseMove[1] = LOWORD(lParam);

		m_mouseMovement[0] = m_currMouseMove[0] - m_oldMouseMove[0];
		m_mouseMovement[1] = m_currMouseMove[1] - m_oldMouseMove[1];

		m_oldMouseMove[0] = m_currMouseMove[0];
		m_oldMouseMove[1] = m_currMouseMove[1];
		break;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}