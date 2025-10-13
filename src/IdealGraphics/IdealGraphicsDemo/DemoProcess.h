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
	// 윈도우 핸들러
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

	/// <summary>
	/// 파일 매니징 시스템 초기화
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
	/// 윈도우 크기 조정
	/// 지금은 시작 시에만 적용
	/// 나중에 크기 조정 시에 렌더러에 정보 주는거 만들 예정
	/// </summary>
	/// <param name="_width">너비</param>
	/// <param name="_hight">높이</param>
	void ResizeWindow(int _width, int _hight);
	
	void CreateTexuerTemplate(const std::wstring& _path);
};
