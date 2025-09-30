#include "DemoProcess.h"
#include "stringUtil.h"

ManagerSet* DemoProcess::m_staticManagers = nullptr;

DemoProcess::DemoProcess()
	: m_renderer(nullptr)
	, m_hwnd(nullptr)
	, m_managers(nullptr)
	, m_fms()
{
}

DemoProcess::~DemoProcess()
{
	delete m_renderer;
	// delete m_managers;
}

void DemoProcess::Initialize(HWND _hwnd)
{
	IE result;

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

	IE_ASSERT(
		m_renderer->Initialize(m_rendererState, m_hwnd)
		, "Renderer Initialize Fail"
	);

	// result = m_renderer->Initialize(m_rendererState, m_hwnd);
	// if (IE::I_OK != result)
	// {
	// 	return;
	// }

	IE_ASSERT(
		m_renderer->SetBackgroundColor(1, 0, 0, 1)
		, "Set Background Color Fail"
	);

	std::vector<unsigned char> tempvs;
	m_fms.OpenFile(L"DefaultVS.cso", tempvs);

	IE_ASSERT(
		m_renderer->CreateVertexShader(IRenderer::VERTEX_TYPE::VertexPU, "DefaultVS", tempvs)
		, "Cannot Create VertexShader"
	);


	std::vector<unsigned char> tempps;
	m_fms.OpenFile(L"DefaultVS.cso", tempps);
	IE_ASSERT(
		m_renderer->CreatePixelShader("DefaultPS", tempps)
		, "Cannot Create PixelShader"
	);


	int camera = m_renderer->CreateCamera();
	IE_ASSERT(
		m_renderer->SetCamera(camera)
		, "Cannot Set Camera"
	);

	RECT windowSize;
	GetWindowRect(m_hwnd, &windowSize);

	//m_renderer->CreateCamera(
	//	"default",
	//	static_cast<float>(windowSize.bottom - windowSize.top),
	//	static_cast<float>(windowSize.right - windowSize.left));

	// m_renderer->SetMainCamera("default");

	// camera = this->m_renderer->GetCamera();

	// m_renderer->CreateSkyBox("moring", "../AssimpData/SkyBox/skymap.dds");
	// m_renderer->SetSkyBox("moring");

	// m_managers = new ManagerSet();
	// m_staticManagers = m_managers;
	// m_managers->Initialize(hwnd);
}

void DemoProcess::Process()
{
	Update();
	Render();
}

void DemoProcess::Update()
{
	// this->m_managers->Update();
	// 
	// this->object->Update(this->m_managers->timeManager->GetDT());
	// CameraUpdate(this->m_managers->timeManager->GetfDT());
	// this->m_managers->keyManager->mouseDX = 0;
	// this->m_managers->keyManager->mouseDY = 0;
	// 
	// graphicsEngine->GetDT(this->m_managers->timeManager->GetDT());
}

void DemoProcess::Render()
{
	IE result;
	result = m_renderer->Draw();
	if (IE::I_OK != result)
	{
		std::cout << "DrawError : " << static_cast<int>(result) << '\n';
		return;
	}

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

void DemoProcess::CameraUpdate(float _dt)
{
	float speed = 10;

	// if (this->m_managers->keyManager->GetKeyState(KEY::W) == KEY_STATE::HOLD)
	// {
	// 	this->camera.lock()->MoveFoward(this->m_managers->timeManager->GetfDT() * speed);
	// }
	// if (this->m_managers->keyManager->GetKeyState(KEY::S) == KEY_STATE::HOLD)
	// {
	// 	this->camera.lock()->MoveFoward(-this->m_managers->timeManager->GetfDT() * speed);
	// }
	// if (this->m_managers->keyManager->GetKeyState(KEY::A) == KEY_STATE::HOLD)
	// {
	// 	this->camera.lock()->MoveRight(-this->m_managers->timeManager->GetfDT() * speed);
	// }
	// if (this->m_managers->keyManager->GetKeyState(KEY::D) == KEY_STATE::HOLD)
	// {
	// 	this->camera.lock()->MoveRight(this->m_managers->timeManager->GetfDT() * speed);
	// }
	// if (this->m_managers->keyManager->GetKeyState(KEY::Q) == KEY_STATE::HOLD)
	// {
	// 	this->camera.lock()->MoveUP(-this->m_managers->timeManager->GetfDT() * speed);
	// }
	// if (this->m_managers->keyManager->GetKeyState(KEY::E) == KEY_STATE::HOLD)
	// {
	// 	this->camera.lock()->MoveUP(this->m_managers->timeManager->GetfDT() * speed);
	// }
	// 
	// if (this->m_managers->keyManager->GetMouseState(MOUSE::LEFT) == KEY_STATE::HOLD)
	// {
	// 	this->camera.lock()->RotateRight(this->m_managers->keyManager->mouseDX * 0.003f);
	// 	this->camera.lock()->RotateUp(this->m_managers->keyManager->mouseDY * 0.003f);
	// }
}

void DemoProcess::CreateRendererState()
{
	std::vector<unsigned char> s;
	m_fms.OpenFile(L"D3DSetting.lua", s);
	if (true == s.empty())
	{
		return;
	}

	DO_STREAM(m_luaState, s);
	GET_LUA_TABLE_NEW(m_luaState, stateTable, "RendererInitializeState");
	GET_LUA_TABLE_NEW(m_luaState, windowTable, "WindowSetting");

	GET_VALUE_NEW(windowTable, width, "Width", int);
	GET_VALUE_NEW(windowTable, height, "Height", int);

	ResizeWindow(width, height);

	RECT windowSize = {};
	GetWindowRect(m_hwnd, &windowSize);

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

	depthState.m_width = windowSize.right - windowSize.left;
	depthState.m_height = windowSize.bottom - windowSize.top;
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

	swapState.m_width = windowSize.right - windowSize.left;
	swapState.m_height = windowSize.bottom - windowSize.top;
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

	renderState.m_width = windowSize.right - windowSize.left;
	renderState.m_height = windowSize.bottom - windowSize.top;
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

LRESULT CALLBACK DemoProcess::WndProc(HWND hWnd, UINT message,
	WPARAM wParam, LPARAM lParam)
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
	//case WM_LBUTTONDOWN:
	//	m_staticManagers->keyManager->OnMouseLeftDown(LOWORD(lParam), HIWORD(lParam));
	//	break;
	//case WM_RBUTTONDOWN:
	//	m_staticManagers->keyManager->OnMouseRightDown(LOWORD(lParam), HIWORD(lParam));
	//	break;
	//case WM_LBUTTONUP:
	//	m_staticManagers->keyManager->OnMouseLeftUp(LOWORD(lParam), HIWORD(lParam));
	//	break;
	//case WM_RBUTTONUP:
	//	m_staticManagers->keyManager->OnMouseRightUp(LOWORD(lParam), HIWORD(lParam));
	//	break;
	//case WM_MOUSEMOVE:
	//	m_staticManagers->keyManager->OnMouseMove(static_cast<int>(wParam), LOWORD(lParam), HIWORD(lParam));
	//	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}