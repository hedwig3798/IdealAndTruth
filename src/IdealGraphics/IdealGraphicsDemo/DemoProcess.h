#pragma once
#include <windows.h>
#include <string>
#include "IdealRenderer/IRenderer.h"
#include <d3d11.h>
class IRenderer;
class ManagerSet;

class DemoProcess
{
private:
	// 윈도우 핸들러
	HWND m_hwnd;
	IRenderer* m_renderer;
	HMODULE m_rendererDll;
	ManagerSet* m_managers;
	static ManagerSet* m_staticManagers;

	std::wstring m_explain;

	IRenderer::InitializeState m_rendererState;

public:
	// 생성자 소멸자
	DemoProcess();
	~DemoProcess();

	// 명시적 초기화
	void Initialize(HWND _hwnd);

	// 루프에서 실행되는 함수
	void Process();

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
		WPARAM wParam, LPARAM lParam);

private:
	// 실제 수행되는 함수
	void Update();
	void Render();
	void CameraUpdate(float _dt);

	void CreateRendererState();
};

