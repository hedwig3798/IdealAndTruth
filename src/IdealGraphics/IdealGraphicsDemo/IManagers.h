#pragma once
class IManagers
{
public:
	virtual bool MsgProc(
		HWND hWnd
		, UINT message
		, WPARAM wParam
		, LPARAM lParam
	) = 0;
};