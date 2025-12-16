#pragma once
#include "IManagers.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class TImguiManager :
    public IManagers
{
public:
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

