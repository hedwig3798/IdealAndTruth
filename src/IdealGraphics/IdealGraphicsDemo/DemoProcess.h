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

	IRenderer::InitializeState m_rendererState;

	FileStorage m_fms;

	lua_State* m_luaState;

	IRenderer::IRenderObject m_tempObject;

	std::weak_ptr<ICamera> m_camera;

	UINT m_renderWidth;
	UINT m_renderHight;

	// �ӽ� ���콺 ����
	static bool m_mouseDown[2];
	static int m_currMouseMove[2];
	static int m_oldMouseMove[2];
	static int m_mouseMovement[2];
	
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
	
	/// <summary>
	/// ���׸����� ���ø� �����͸� �����Ѵ�.
	/// </summary>
	/// <param name="_path">lua ���</param>
	void CreateMaterial(const std::wstring& _path);
};
