#pragma once
#pragma comment(lib, "E:\\LuaBinding\\x64\\Debug\\lua5.1.lib")

#include "IdealRenderer/IRenderer.h"

#include <windows.h>
#include <string>
#include <iostream>

#include "FileStorage.h"

#include "lua/lua.hpp"
#include "lua_tinker.h"
#include "luaMacro.h"

#include "IManagers.h"
#include "TInputManager.h"
#include "TImguiManager.h"

class IRenderer;
class IManagers;

class DemoProcess
{
private:

	struct FileManagerSetting
	{
		bool m_isDevMode;
		int m_threadCount;
		int m_chunkSize;
		std::wstring m_resourcePath;
		std::wstring m_resourceOutputPath;
		std::wstring m_outputFileName;
		std::wstring m_Extension;
	};

	// 윈도우 핸들러
	HWND m_hwnd;
	IRenderer* m_renderer;
	HMODULE m_rendererDll;

	// 렌더러 초기화를 위한 스테이트
	// 수정을 할 수도 있어서 변수로 저장
	IRenderer::InitializeState m_rendererState;

	// 파일 저장소 및 매니저
	FileStorage m_fms;

	// 루아 스테이트
	lua_State* m_luaState;

	// 카메라
	std::weak_ptr<ICamera> m_camera;

	// 렌더 넓이와 높이
	UINT m_renderWidth;
	UINT m_renderHight;

	// 각 매니저의 메시지 프로시져 함수 벡터
	// static 함수여야 해서 이렇게 넣는다
	static std::vector<IManagers*> m_managers;

	TInputManager* m_inputManager;
	TImguiManager* m_imguiManager;


public:
	// 생성자 소멸자
	DemoProcess();
	~DemoProcess();

	// 명시적 초기화
	void Initialize(HWND _hwnd);

	// 루프에서 실행되는 함수
	void Process();

	static LRESULT CALLBACK WndProc(
		HWND hWnd
		, UINT message
		, WPARAM wParam
		, LPARAM lParam
	);

	static bool OpenFile(
		void* _fms
		, const std::wstring& _filename
		, OUT FILE_STREAM& _fileData
	);



private:
	// 실제 수행되는 함수
	void Update();
	void Render();

	/// <summary>
	/// 렌더러 정보 입력
	/// </summary>
	void CreateRendererState();

	/// <summary>
	/// 세팅 값 가져오기
	/// </summary>
	void Settings();

	/// <summary>
	/// 파일 매니징 시스템 초기화
	/// </summary>
	void FMSSetting(const FileManagerSetting& _settingValue);

	/// <summary>
	/// Lua Start
	/// </summary>
	void Luainitialize();
	
	/// <summary>
	/// imgui ui start
	/// </summary>
	void ImguiInitialize();

	/// <summary>
	/// Lua Start	
	/// </summary>
	void LuaSetting();

	/// <summary>
	/// DirectX Setting
	/// </summary>
	void D3DSetting();

	/// <summary>
	/// 씬 스크립트 읽기
	/// </summary>
	/// <param name="_name">씬 스크립트 이름</param>
	void ReadScene(std::wstring _name);

	/// <summary>
	/// 윈도우 크기 조정
	/// 지금은 시작 시에만 적용
	/// 나중에 크기 조정 시에 렌더러에 정보 주는거 만들 예정
	/// </summary>
	/// <param name="_width">너비</param>
	/// <param name="_hight">높이</param>
	void ResizeWindow(int _width, int _hight);
	
	/// <summary>
	/// 머테리얼의 탬플릿 데이터를 생성한다.
	/// </summary>
	/// <param name="_path">lua 경로</param>
	void CreateMaterial(const std::wstring& _path);

	/// <summary>
	/// 카메라 조종 함수
	/// </summary>
	void CameraUpdate();

	/// <summary>
	/// 매니져 생성
	/// </summary>
	void CreateManagers();

	/// <summary>
	/// 매니져 초기화
	/// </summary>
	void InitManagers();

	/// <summary>
	/// 스카이 박스 설정
	/// </summary>
	void SetSkyBox();
};
