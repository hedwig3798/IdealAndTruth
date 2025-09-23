#pragma once
#pragma comment(lib, "E:\\LuaBinding\\x64\\Debug\\lua5.1.lib")

#include <windows.h>
#include <string>

#include "IdealRenderer/IRenderer.h"
#include "FileStorage.h"

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

	IRenderer::InitializeState m_rendererState;

	FileStorage m_fms;

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

