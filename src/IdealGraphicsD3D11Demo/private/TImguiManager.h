#pragma once
#include "IManagers.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include <time.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class TImguiManager :
	public IManagers
{
private:
	double m_fpsTimer;
	clock_t m_frameStart;
	clock_t m_frameEnd;

	int m_fpsCounter;
	int m_fpsViwe;

public:
	TImguiManager();
	virtual ~TImguiManager();

	void Render();

public:

	virtual void Initialize(HWND _hwnd, IRenderer* _renderer);

	virtual bool MsgProc(
		HWND hWnd
		, UINT message
		, WPARAM wParam
		, LPARAM lParam
	) override;
};

