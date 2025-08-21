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
	// ������ �ڵ鷯
	HWND m_hwnd;
	IRenderer* m_renderer;
	HMODULE m_rendererDll;
	ManagerSet* m_managers;
	static ManagerSet* m_staticManagers;

	std::wstring m_explain;

	IRenderer::InitializeState m_rendererState;

public:
	// ������ �Ҹ���
	DemoProcess();
	~DemoProcess();

	// ����� �ʱ�ȭ
	void Initialize(HWND _hwnd);

	// �������� ����Ǵ� �Լ�
	void Process();

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
		WPARAM wParam, LPARAM lParam);

private:
	// ���� ����Ǵ� �Լ�
	void Update();
	void Render();
	void CameraUpdate(float _dt);

	void CreateRendererState();
};

