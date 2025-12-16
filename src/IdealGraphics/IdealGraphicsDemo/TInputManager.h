#pragma once
#include <windows.h>
#include <iostream>

#include "IManagers.h"

#define MAX_KEY 256U
#define MAX_MOUSE 2

class TInputManager
	: public IManagers
{
private:
	bool m_mouseDown[MAX_MOUSE][2];
	int m_currMouseMove[2];
	int m_oldMouseMove[2];
	int m_mouseMovement[2];

	bool m_keyboradInput[MAX_KEY][2];

public:
	TInputManager();
	~TInputManager();

	virtual bool MsgProc(
		HWND hWnd
		, UINT message
		, WPARAM wParam
		, LPARAM lParam
	) override;

	void KeyFrameProcess();

	bool GetKeyDown(UINT _key);
	bool GetKeyUp(UINT _key);
	bool GetKeyHold(UINT _key);

	bool GetMouseDown(int _val);
	bool GetMouseUp(int _val);
	bool GetMouseHold(int _val);

	int GetMouseDX() { return m_mouseMovement[0]; };
	int GetMouseDY() { return m_mouseMovement[1]; };

	void ResetMouseMovement();

	void Update();
};

