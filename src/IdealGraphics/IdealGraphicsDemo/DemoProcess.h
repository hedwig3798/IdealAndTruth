#pragma once
#pragma comment(lib, "E:\\LuaBinding\\x64\\Debug\\lua5.1.lib")

#include <windows.h>
#include <string>
#include <iostream>

#include "IdealRenderer/IRenderer.h"
#include "FileStorage.h"

#include "lua/lua.hpp"
#include "lua_tinker.h"
#include "luaMacro.h"

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

	lua_State* m_luaState;

	IRenderer::IRenderObject m_tempObject;

	std::weak_ptr<ICamera> m_camera;

	UINT m_renderWidth;
	UINT m_renderHight;

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

	/// <summary>
	/// ���� �Ŵ�¡ �ý��� �ʱ�ȭ
	/// </summary>
	void FMSSetting();

	/// <summary>
	/// Lua Setting
	/// </summary>
	void LuaSetting();

	/// <summary>
	/// DirectX Setting
	/// </summary>
	void D3DSetting();

	/// <summary>
	/// ������ ũ�� ����
	/// ������ ���� �ÿ��� ����
	/// ���߿� ũ�� ���� �ÿ� �������� ���� �ִ°� ���� ����
	/// </summary>
	/// <param name="_width">�ʺ�</param>
	/// <param name="_hight">����</param>
	void ResizeWindow(int _width, int _hight);
	
	void CreateTexuerTemplate(const std::wstring& _path);
};
