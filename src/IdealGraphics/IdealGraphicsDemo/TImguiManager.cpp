#include "TImguiManager.h"
#include "IdealRenderer/IRenderer.h"

#include <iostream>

TImguiManager::TImguiManager()
	: m_fpsTimer(0.0f)
	, m_fpsViwe(0)
	, m_fpsCounter(0)
{
	m_frameStart = ::clock();
	m_frameEnd = ::clock();
}

TImguiManager::~TImguiManager()
{

}

void TImguiManager::Render()
{
	m_frameEnd = ::clock();

	m_fpsTimer += static_cast<double>(m_frameEnd - m_frameStart) / CLOCKS_PER_SEC;
	m_fpsCounter++;

	if (1 <= m_fpsTimer)
	{
		m_fpsTimer -= 1.0;
		m_fpsViwe = m_fpsCounter;
		m_fpsCounter = 0;
	}

	m_frameStart = ::clock();

	::ImGui_ImplDX11_NewFrame();
	::ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	static float f = 0.0f;
	static int counter = 0;

	ImGui::Begin("Hello, world!");

	ImGui::Text("FPS : %d", m_fpsViwe);

	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);

	if (ImGui::Button("Button"))
	{
		counter++;
	}

	ImGui::SameLine();
	ImGui::Text("counter = %d", counter);

	ImGui::End();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void TImguiManager::Initialize(HWND _hwnd, IRenderer* _renderer)
{
	if (NULL == _hwnd
		|| nullptr == _renderer)
	{
		std::cout << "cannot initialize imgui manager\n";
	}

	::ImGui_ImplWin32_EnableDpiAwareness();
	float main_scale = ::ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup scaling
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(main_scale);        
	style.FontScaleDpi = main_scale;        

	// Setup Platform/Renderer backends
	::ImGui_ImplWin32_Init(_hwnd);
	IE_ASSERT(_renderer->ImguiInitialize(::ImGui_ImplDX11_Init));
}

bool TImguiManager::MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (nullptr == ImGui::GetCurrentContext())
	{
		return false;
	}

	if (true == ::ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
	{
		return true;
	}

	// Imgui 조작 시 다른 키 입력 방지
	ImGuiIO& io = ImGui::GetIO();

	switch (message)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_MOUSEMOVE:
		if (io.WantCaptureMouse)
		{
			return true; 
		}
		break;

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
	case WM_CHAR:
		if (io.WantCaptureKeyboard)
		{
			return true;
		}
		break;
	}

	return false;
}
