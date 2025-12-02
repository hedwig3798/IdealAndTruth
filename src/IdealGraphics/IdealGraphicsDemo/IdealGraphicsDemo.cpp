#include "framework.h"
#include "D3D.h"
#include "DemoProcess.h"

#define MAX_LOADSTRING 100

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// 프로그램의 최초 진입점(Entry Point). 멀티바이트 프로젝트
int WINAPI wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR	lpCmdLine,
	_In_ int nCmdShow)
{
	// 디버깅을 위한 콘솔 창
	::AllocConsole();
	FILE* fp;
	::freopen_s(&fp, "CONOUT$", "w", stdout);
	::freopen_s(&fp, "CONOUT$", "w", stderr);
	::freopen_s(&fp, "CONIN$", "r", stdin);

	std::cout << "start Ideal Graphics Demo\n";

	WCHAR szAppName[] = L"D3D Renderer Test";
	HWND hWnd;
	MSG	msg;
	WNDCLASSEXW wcex;

	DemoProcess* demoProcess = new DemoProcess();

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = demoProcess->WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = szAppName;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	// 윈도 클래스 등록
	RegisterClassExW(&wcex);

	// 윈도 생성
	hWnd = CreateWindowW(szAppName,
		szAppName,
		WS_OVERLAPPEDWINDOW,
		100, 100, 1920, 1080,
		NULL, NULL, hInstance, NULL);

	if (!hWnd) return FALSE;

	// 생성된 윈도를 화면에 표시


	ShowWindow(hWnd, SW_SHOWNORMAL);
	UpdateWindow(hWnd);

	RECT nowRect;
	DWORD _style = (DWORD)GetWindowLong(hWnd, GWL_STYLE);
	DWORD _exstyle = (DWORD)GetWindowLong(hWnd, GWL_EXSTYLE);

	GetWindowRect(hWnd, &nowRect);

	RECT newRect;
	newRect.left = 0;
	newRect.top = 0;
	newRect.right = 1240;
	newRect.bottom = 720;

	AdjustWindowRect(&newRect, _style, NULL);
	//AdjustWindowRectEx(&newRect, _style, NULL, _exstyle);

	// 클라이언트 영역보다 윈도 크기는 더 커야 한다. (외곽선, 타이틀 등)
	int _newWidth = (newRect.right - newRect.left);
	int _newHeight = (newRect.bottom - newRect.top);

	SetWindowPos(hWnd, HWND_NOTOPMOST, nowRect.left, nowRect.top,
		_newWidth, _newHeight, SWP_SHOWWINDOW);

	try
	{
		demoProcess->Initialize(hWnd);
		// 메시지 루프
		while (TRUE)
		{
			// 기본 메세지 처리
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				DispatchMessage(&msg);
				if (msg.message == WM_QUIT)
					break;

			}
			else
			{
				// 엔진 동작
				demoProcess->Process();
			}
		}
	}
	catch(const std::exception& e)
	{
		std::cout << "An unexpected exception occurred\n";
		std::cout << e.what() << "\n";
		std::cin.get();
	}
	catch (...)
	{
		std::cout << "An unexpected exception occurred\n";
		std::cin.get();
	}

	// 프로그램 종료 전 엔진 반환
	delete demoProcess;

	return (int)msg.wParam;
}

