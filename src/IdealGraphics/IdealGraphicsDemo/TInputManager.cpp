#include "TInputManager.h"

TInputManager::TInputManager()
	:m_mouseDown{ 0, 0 }
	, m_currMouseMove{ 0, 0 }
	, m_oldMouseMove{ 0, 0 }
	, m_mouseMovement{ 0, 0 }
{
	for (UINT i = 0; i < MAX_KEY; i++)
	{
		m_keyboradInput[i][0] = false;
		m_keyboradInput[i][1] = false;
	}
}

TInputManager::~TInputManager()
{

}

bool TInputManager::MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
	case WM_LBUTTONDOWN:
	{
		m_mouseDown[0][0] = true;
		break;
	}
	case WM_RBUTTONDOWN:
	{
		m_mouseDown[1][0] = true;
		break;
	}
	case WM_LBUTTONUP:
	{
		m_mouseDown[0][0] = false;
		break;
	}
	case WM_RBUTTONUP:
	{
		m_mouseDown[1][0] = false;
		break;
	}
	case WM_MOUSEMOVE:
	{
		m_currMouseMove[1] = HIWORD(lParam);
		m_currMouseMove[0] = LOWORD(lParam);

		break;
	}
	case WM_KEYDOWN:
	{
		m_keyboradInput[wParam & 0xFF][0] = true;
		break;
	}
	case WM_KEYUP:
	{
		m_keyboradInput[wParam & 0xFF][0] = false;
		break;
	}
	break;
	}



	return false;
}

void TInputManager::KeyFrameProcess()
{
	for (UINT i = 0; i < MAX_KEY; ++i)
	{
		m_keyboradInput[i][1] = m_keyboradInput[i][0];
	}

	for (UINT i = 0; i < MAX_MOUSE; ++i)
	{
		m_mouseDown[i][1] = m_mouseDown[i][0];
	}
}

bool TInputManager::GetKeyDown(UINT _key)
{
	if (_key >= MAX_KEY)
	{
		std::cout << "Wrong key type\n";
		return false;
	}

	return (true == m_keyboradInput[_key][0]) && (false == m_keyboradInput[_key][1]);
}

bool TInputManager::GetKeyUp(UINT _key)
{
	if (_key >= MAX_KEY)
	{
		std::cout << "Wrong key type\n";
		return false;
	}

	return (false == m_keyboradInput[_key][0]) && (true == m_keyboradInput[_key][1]);
}

bool TInputManager::GetKeyHold(UINT _key)
{
	if (_key >= MAX_KEY)
	{
		std::cout << "Wrong key type\n";
		return false;
	}

	return m_keyboradInput[_key][0] && m_keyboradInput[_key][1];
}

bool TInputManager::GetMouseDown(int _val)
{
	if (0 > _val
		|| 2 <= _val)
	{
		return false;
	}

	return (true == m_mouseDown[_val][0]) && (false == m_mouseDown[_val][1]);
}

bool TInputManager::GetMouseUp(int _val)
{
	if (0 > _val
		|| 2 <= _val)
	{
		return false;
	}
	return (false == m_mouseDown[_val][0]) && (true == m_mouseDown[_val][1]);
}

bool TInputManager::GetMouseHold(int _val)
{
	if (0 > _val
		|| 2 <= _val)
	{
		return false;
	}
	return m_mouseDown[_val][0] && m_mouseDown[1];
}

void TInputManager::ResetMouseMovement()
{
	m_mouseMovement[0] = 0;
	m_mouseMovement[1] = 0;
}

void TInputManager::Update()
{
	KeyFrameProcess();

	m_mouseMovement[0] = m_currMouseMove[0] - m_oldMouseMove[0];
	m_mouseMovement[1] = m_currMouseMove[1] - m_oldMouseMove[1];

	m_oldMouseMove[0] = m_currMouseMove[0];
	m_oldMouseMove[1] = m_currMouseMove[1];
}
