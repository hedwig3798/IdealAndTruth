#include "DemoProcess.h"

ManagerSet* DemoProcess::m_staticManagers = nullptr;

DemoProcess::DemoProcess()
	: m_renderer(nullptr)
	, m_hwnd(nullptr)
	, m_managers(nullptr)
{
	m_explain = L"W, S : 카메라 전방, 후방 이동\n";
	m_explain += L"A, D : 카메라 좌우 이동\n";
	m_explain += L"Q, E : 카메라 상하 이동\n";
	m_explain += L"화살표 : 카메라 회전\n";
	m_explain += L"숫자 0, 1, 2, 3 : 조명 갯수 설정\n";
	m_explain += L"마우스 좌클릭 드래그: 카메라 회전\n";
	// m_staticManagers = m_managers;
}

DemoProcess::~DemoProcess()
{
	delete m_renderer;
	// delete m_managers;
}

void DemoProcess::Initialize(HWND _hwnd)
{
	m_hwnd = _hwnd;

	std::string path("./IdealGraphics.dll");

	CreateRendererState();

	HMODULE m_rendererDll = ::LoadLibraryA(path.c_str());
	if (nullptr == m_rendererDll)
	{
		return;
	}
	IRenderer* m_renderer = ((IRenderer * (*)())GetProcAddress(m_rendererDll, "CreateD3D11Renderer"))();
	m_renderer->Initialize(m_rendererState);

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
	RECT windowSize = {};
	GetWindowRect(m_hwnd, &windowSize);

	// 디바이스 생성
	m_rendererState.m_device.m_deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG;

	// 뎁스 스텐실 생성
	m_rendererState.m_depthStancil.m_width = windowSize.right - windowSize.left;
	m_rendererState.m_depthStancil.m_height = windowSize.bottom - windowSize.top;
	m_rendererState.m_depthStancil.m_mipLevel = 1;
	m_rendererState.m_depthStancil.m_arraySize = 1;
	m_rendererState.m_depthStancil.m_format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	m_rendererState.m_depthStancil.m_sampleCount = 1;
	m_rendererState.m_depthStancil.m_sampleQuality = 0;

	m_rendererState.m_depthStancil.m_usage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// 스왑 체인 생성
	m_rendererState.m_swapChain.m_width = windowSize.right - windowSize.left;
	m_rendererState.m_swapChain.m_height = windowSize.bottom - windowSize.top;
	m_rendererState.m_swapChain.m_refreshRateNumerator = 60;
	m_rendererState.m_swapChain.m_refreshRateDenominator = 1;
	m_rendererState.m_swapChain.m_format = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_rendererState.m_swapChain.m_scanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	m_rendererState.m_swapChain.m_scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	m_rendererState.m_swapChain.m_sampleCount = 1;
	m_rendererState.m_swapChain.m_sampleQuality = 0;

	m_rendererState.m_swapChain.m_bufferCount = 1;
	m_rendererState.m_swapChain.m_swapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// 레스터라이저
	m_rendererState.m_rasterizer.m_fillMode = D3D11_FILL_SOLID;
	m_rendererState.m_rasterizer.m_cullMode = D3D11_CULL_BACK;
	m_rendererState.m_rasterizer.m_isFrontCCW = true;
	m_rendererState.m_rasterizer.m_isDepthClip = true;
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
	//case WM_DESTROY:
	//	PostQuitMessage(0);
	//	break;
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