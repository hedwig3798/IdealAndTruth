#pragma once
#include <windows.h>

class IRenderer;

class IManagers
{
public:
	virtual void Initialize(HWND _hwnd, IRenderer* _renderer) = 0;

	virtual bool MsgProc(
		HWND hWnd
		, UINT message
		, WPARAM wParam
		, LPARAM lParam
	) = 0;
};