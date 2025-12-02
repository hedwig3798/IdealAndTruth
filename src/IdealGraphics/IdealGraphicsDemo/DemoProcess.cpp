#include "DemoProcess.h"
#include "stringUtil.h"
#include "FilePathUtil.h"

bool DemoProcess::m_mouseDown[2] = { false, false };
int DemoProcess::m_currMouseMove[2] = { 0, 0 };
int DemoProcess::m_oldMouseMove[2] = { 0, 0 };
int DemoProcess::m_mouseMovement[2] = { 0, 0 };

DemoProcess::DemoProcess()
	: m_renderer(nullptr)
	, m_hwnd(nullptr)
	, m_managers(nullptr)
	, m_fms()
	, m_luaState(nullptr)
{
	m_mouseDown[0] = false;
	m_mouseDown[1] = false;

	m_tempRender = std::make_shared<IRenderer::IRenderObject>();
	m_tempModel = std::make_shared<IRenderer::IModelObject>();
}

DemoProcess::~DemoProcess()
{
	delete m_renderer;
	// delete m_managers;
}

void DemoProcess::Initialize(HWND _hwnd)
{
	if (NULL == _hwnd)
	{
		std::cout << "window handler error\n";
		return;
	}

	// 각종 세팅들
	Settings();

	m_hwnd = _hwnd;

	// 렌더러 만들기
	CreateRendererState();

	HMODULE m_rendererDll = ::LoadLibraryA("./IdealGraphics.dll");
	if (nullptr == m_rendererDll)
	{
		return;
	}
	m_renderer = ((IRenderer * (*)())GetProcAddress(m_rendererDll, "CreateD3D11Renderer"))();

	m_renderer->SetRenderSize(m_renderWidth, m_renderHight);

	// 초기화
	IE_ASSERT(m_renderer->Initialize(m_rendererState, m_hwnd));

	// 뒷배경 색
	IE_ASSERT(m_renderer->SetBackgroundColor(0, 0, 0, 1));

	// 정점 셰이더 생성
	IE_ASSERT(m_renderer->CreateVertexShader(IRenderer::VERTEX_TYPE::VertexPUN, L"DefaultVS.cso"));

	// 픽셀 셰이더 생성
	IE_ASSERT(m_renderer->CreatePixelShader(L"DefaultPS.cso"));

	// 카메라 생성
	m_camera = m_renderer->CreateCamera();
	IE_ASSERT(m_renderer->SetCamera(m_camera));

	m_camera.lock()->SetAspectRatio(static_cast<float>(m_renderWidth) / static_cast<float>(m_renderHight));

	// 정점 버퍼 생성
	IE_ASSERT(m_renderer->CreateVertexIndexBuffer(L"gun.iver"));

	// 머테리얼 데이터로 머테리얼 만들기
	CreateMaterial(L"GunMaterial.lua");

	// 매쉬 데이터 생성
	ReadScene(L"DemoScene.lua");
	// 빛 테스트
	m_renderer->SetMaxLightCount(10);

	IRenderer::LightData ld;
	ld.m_direction = {0, -1, 0};
	ld.m_intensity = 1.0f;
	ld.m_color = { 1, 1, 1 };
	ld.m_type = IRenderer::LIGHT_TYPE::DIRECTION;

	m_renderer->AddLight(L"temp", ld);
}

void DemoProcess::Process()
{
	Update();
	Render();
}

void DemoProcess::Update()
{
	// 임시 카메라 움직임 제어
	// 나중에 매니져 클래스로 뺄 예정
	if (true == m_camera.expired())
	{
		return;
	}

	ICamera* camera = m_camera.lock().get();

	// 카메라 이동
	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		camera->MoveForward(0.001f);
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		camera->MoveForward(-0.001f);
	}
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		camera->MoveRight(-0.001f);
	}
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		camera->MoveRight(0.001f);
	}

	// 카메라 회전
	if (GetAsyncKeyState('W') & 0x8000)
	{
		camera->MoveForward(0.001f);
	}
	if (GetAsyncKeyState('S') & 0x8000)
	{
		camera->MoveForward(-0.001f);
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		camera->MoveRight(0.001f);
	}
	if (GetAsyncKeyState('A') & 0x8000)
	{
		camera->MoveRight(-0.001f);
	}

	if (GetAsyncKeyState('E') & 0x8000)
	{
		camera->MoveUp(0.001f);
	}
	if (GetAsyncKeyState('Q') & 0x8000)
	{
		camera->MoveUp(-0.001f);
	}

	if (true == m_mouseDown[0])
	{
		camera->RotateRight(-m_mouseMovement[1] * 0.0009f);
		camera->RotateUp(-m_mouseMovement[0] * 0.0009f);

		m_mouseMovement[0] = 0;
		m_mouseMovement[1] = 0;
	}
}

void DemoProcess::Render()
{
	IE_ASSERT(m_renderer->Draw());

	// this->graphicsEngine->begineDraw();
	// this->object->Render(graphicsEngine);
	// 
	// std::wstring dt = L"DeltaTime : ";
	// dt += std::to_wstring(this->m_managers->timeManager->GetfDT());
	// dt += L"\n";
	// dt += L"FPS : ";
	// dt += std::to_wstring(1 / this->m_managers->timeManager->GetfDT());
	// 
	// float w[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	// 
	// this->graphicsEngine->endDraw();
	// 
	// int test = 0;
}

void DemoProcess::CreateRendererState()
{
	std::vector<unsigned char> s;
	m_fms.OpenFile(L"D3DSetting.lua", s);
	if (true == s.empty())
	{
		return;
	}

	// 초기 설정 불러오기
	DO_STREAM(m_luaState, s);

	// 윈도우 설정
	table windowTable = LuaValueGetter<table>::Get(m_luaState, "WindowSetting");
	int width = LuaValueGetter<int>::Get(windowTable, "Width");
	int height = LuaValueGetter<int>::Get(windowTable, "Height");

	ResizeWindow(width, height);

	m_renderWidth = width;
	m_renderHight = height;

	table stateTable = LuaValueGetter<table>::Get(m_luaState, "RendererInitializeState");
	m_rendererState.m_renderVectorSize = LuaValueGetter<int>::Get(stateTable, "RenderVectorSize");

	// 디바이스 생성
	IRenderer::InitializeState::Device deviceState = {};
	deviceState.m_deviceFlags = 0;
	table device = LuaValueGetter<table>::Get(stateTable, "Device");
	for (size_t i = 1; i <= device.size(); i++)
	{
		UINT flag = LuaValueGetter<UINT>::Get(device, i);
		deviceState.m_deviceFlags |= flag;
	}
	m_rendererState.m_device = deviceState;

	// 뎁스 스텐실 생성
	IRenderer::InitializeState::DepthStancil depthState = {};
	table depthStancil = LuaValueGetter<table>::Get(stateTable, "DepthStancil");

	depthState.m_width = width;
	depthState.m_height = height;
	depthState.m_mipLevel = LuaValueGetter<UINT>::Get(depthStancil, "mipLevel");
	depthState.m_arraySize = LuaValueGetter<UINT>::Get(depthStancil, "arraySize");
	depthState.m_format = LuaValueGetter<UINT>::Get(depthStancil, "format");
	depthState.m_sampleCount = LuaValueGetter<UINT>::Get(depthStancil, "sampleCount");
	depthState.m_sampleQuality = LuaValueGetter<UINT>::Get(depthStancil, "sampleQuality");
	depthState.m_usage = LuaValueGetter<UINT>::Get(depthStancil, "usage");
	m_rendererState.m_depthStancil = depthState;

	// 스왑 체인 생성
	IRenderer::InitializeState::SwapCahin swapState = {};
	table swapCahin = LuaValueGetter<table>::Get(stateTable, "SwapCahin");

	swapState.m_width = width;
	swapState.m_height = height;
	swapState.m_refreshRateNumerator = LuaValueGetter<UINT>::Get(swapCahin, "refreshRateNumerator");
	swapState.m_refreshRateDenominator = LuaValueGetter<UINT>::Get(swapCahin, "refreshRateDenominator");
	swapState.m_format = LuaValueGetter<UINT>::Get(swapCahin, "format");
	swapState.m_scanlineOrdering = LuaValueGetter<UINT>::Get(swapCahin, "scanlineOrdering");
	swapState.m_scaling = LuaValueGetter<UINT>::Get(swapCahin, "scaling");
	swapState.m_sampleCount = LuaValueGetter<UINT>::Get(swapCahin, "sampleCount");
	swapState.m_sampleQuality = LuaValueGetter<UINT>::Get(swapCahin, "sampleQuality");
	swapState.m_bufferCount = LuaValueGetter<UINT>::Get(swapCahin, "bufferCount");
	swapState.m_swapEffect = LuaValueGetter<UINT>::Get(swapCahin, "swapEffect");
	swapState.m_isWindowed = LuaValueGetter<bool>::Get(swapCahin, "isWindowed");
	m_rendererState.m_swapChain = swapState;

	// 레스터라이저
	IRenderer::InitializeState::RaseterizerState rasterState = {};
	table rasterizser = LuaValueGetter<table>::Get(stateTable, "RaseterizerState");

	rasterState.m_fillMode = LuaValueGetter<UINT>::Get(rasterizser, "fillMode");
	rasterState.m_cullMode = LuaValueGetter<UINT>::Get(rasterizser, "cullMode");
	rasterState.m_isFrontCCW = LuaValueGetter<bool>::Get(rasterizser, "isFrontCCW");
	rasterState.m_isDepthClip = LuaValueGetter<bool>::Get(rasterizser, "isDepthClip");
	m_rendererState.m_rasterizer = rasterState;

	IRenderer::InitializeState::RenderTargetViewState renderState = {};
	table renderTarget = LuaValueGetter<table>::Get(stateTable, "RenderTargetViewState");

	renderState.m_width = width;
	renderState.m_height = height;
	renderState.m_mipLevel = LuaValueGetter<UINT>::Get(renderTarget, "mipLevel");
	renderState.m_arraySize = LuaValueGetter<UINT>::Get(renderTarget, "arraySize");
	renderState.m_format = LuaValueGetter<UINT>::Get(renderTarget, "format");
	renderState.m_sampleCount = LuaValueGetter<UINT>::Get(renderTarget, "sampleCount");
	renderState.m_usage = LuaValueGetter<UINT>::Get(renderTarget, "usage");
	renderState.m_viewDimension = LuaValueGetter<UINT>::Get(renderTarget, "viewDimension");

	table binflags = LuaValueGetter<table>::Get(renderTarget, "bindFlags");
	renderState.m_bindFlags = 0;
	for (size_t i = 1; i <= device.size(); i++)
	{
		UINT flag = LuaValueGetter<UINT>::Get(binflags, i);
		renderState.m_bindFlags |= flag;
	}
	m_rendererState.m_renderTargetView = renderState;

	m_rendererState.m_maxLightCount = 10;

	m_rendererState.FileOpenCallbackFunc = DemoProcess::OpenFile;
	m_rendererState.m_fms = static_cast<void*>(&m_fms);
}

void DemoProcess::Settings()
{
	// 루아
	LuaStart();

	// 세팅 파일 읽어오기
	if (nullptr == m_luaState)
	{
		std::cout << "No Lua State At Setting Process\n";
		return;
	}

	std::string settingFilePath = ::GetExeDir();
	settingFilePath += "\\Settings.lua";

	std::ifstream settingFile(settingFilePath, std::ios::binary);
	if (false == settingFile.is_open())
	{
		return;
	}
	// 포인터를 파일의 끝으로 옮겨서 파일 크기 구하기
	settingFile.seekg(0, std::ios::end);
	uint64_t fileSize = settingFile.tellg();
	settingFile.seekg(0, std::ios::beg);

	// 파일 데이터 읽기
	std::vector<char> fileData;
	fileData.resize(fileSize);
	settingFile.read(reinterpret_cast<char*>(fileData.data()), fileSize);

	lua_tinker::dobuffer(m_luaState, fileData.data(), fileSize);

	// 세팅 파일에서 세팅 값 가져오기
	FileManagerSetting fs;
	fs.m_resourcePath = LuaValueGetter<std::wstring>::Get(m_luaState, "ResourcePath");
	fs.m_resourceOutputPath = LuaValueGetter<std::wstring>::Get(m_luaState, "ResourceOutputPath");
	fs.m_outputFileName = LuaValueGetter<std::wstring>::Get(m_luaState, "OutputFileName");
	fs.m_Extension = LuaValueGetter<std::wstring>::Get(m_luaState, "Extension");
	fs.m_threadCount = LuaValueGetter<int>::Get(m_luaState, "ThreadCount");
	fs.m_chunkSize = LuaValueGetter<int>::Get(m_luaState, "ChunkSize");
	fs.m_isDevMode = LuaValueGetter<bool>::Get(m_luaState, "DevMode");

	// 파일 세팅
	FMSSetting(fs);

	LuaSetting();

	// D3D 세팅
	// TODO : 다른 그래픽 API 사용 시 바꿀 것
	D3DSetting();
}

void DemoProcess::FMSSetting(const FileManagerSetting& _settingValue)
{
	if (true == _settingValue.m_isDevMode)
	{
		m_fms.ResetDevFileMap(_settingValue.m_resourcePath);
		m_fms.SetDevMode(_settingValue.m_isDevMode);
		return;
	}

	m_fms.SetThreadCount(_settingValue.m_threadCount);
	m_fms.SetChunkSize(_settingValue.m_chunkSize);
	m_fms.SetOutputFileName(_settingValue.m_outputFileName);
	m_fms.SetCompressExtension(_settingValue.m_Extension);
	m_fms.SetCompressFilePath(_settingValue.m_resourcePath);

	if (false == m_fms.CompressAll(_settingValue.m_resourceOutputPath))
	{
		std::cout << "fail to CompressFile\n";
		return;
	}
}

void DemoProcess::LuaStart()
{
	m_luaState = ::luaL_newstate();
	::luaL_openlibs(m_luaState);
}

void DemoProcess::LuaSetting()
{
	if (nullptr == m_luaState)
	{
		return;
	}

	std::vector<unsigned char> enumLua;
	m_fms.OpenFile(L"Enum.lua", enumLua);
	if (true == enumLua.empty())
	{
		std::cout << "cannot open Enum.lua\n";
		return;
	}

	DO_STREAM(m_luaState, enumLua);
}

void DemoProcess::D3DSetting()
{
	if (nullptr == m_luaState)
	{
		return;
	}

	std::vector<unsigned char> enumSetting;
	m_fms.OpenFile(L"D3DEnum.lua", enumSetting);
	if (true == enumSetting.empty())
	{
		std::cout << "cannot open D3DEnum.lua\n";
		return;
	}

	DO_STREAM(m_luaState, enumSetting);
}

void DemoProcess::ReadScene(std::wstring _name)
{
	// 루아 체크
	if (nullptr == m_luaState)
	{
		return;
	}

	// 씬 스크립트 읽기
	FILE_STREAM fs;
	m_fms.OpenFile(_name, fs);

	// 읽기 실패
	if (0 == fs.size())
	{
		return;
	}

	// lua state 를 통해 읽기
	DO_STREAM(m_luaState, fs);

	std::shared_ptr<IRenderer::IModelObject> modelObject = std::make_shared<IRenderer::IModelObject>();
	modelObject->m_name = _name;
	// 모델 데이터
	table models = LuaValueGetter<table>::Get(m_luaState, "Model");
	for (size_t i = 1; i <= models.size(); i++)
	{
		// 각 모델에 들어갈 매쉬의 이름
		table model = LuaValueGetter<table>::Get(models, i);

		// 매쉬 파일 이름
		std::string name = LuaValueGetter<std::string>::Get(model, "name");

		// 매쉬의 월드 변환 행렬
		Vector3 position = LuaValueGetter<Vector3>::Get(model, "Position");
		Vector3 rotation = LuaValueGetter<Vector3>::Get(model, "Rotation");
		Vector3 scale = LuaValueGetter<Vector3>::Get(model, "Scale");

		modelObject->m_world = Matrix::Identity;
		modelObject->m_world *= Matrix::CreateScale(scale);
		modelObject->m_world *= Matrix::CreateFromYawPitchRoll(rotation);
		modelObject->m_world *= Matrix::CreateTranslation(position);

		// 매쉬 파일 읽기
		FILE_STREAM meshFS;
		m_fms.OpenFile(::StrToWstr(name), meshFS);
		if (0 == meshFS.size())
		{
			return;
		}

		DO_STREAM(m_luaState, meshFS);

		std::shared_ptr<IRenderer::IRenderObject> renderObject = std::make_shared<IRenderer::IRenderObject>();
		renderObject->m_isDraw = true;
		renderObject->m_mesh = LuaValueGetter<std::wstring>::Get(m_luaState, "Mesh");
		renderObject->m_vertexShader = LuaValueGetter<std::wstring>::Get(m_luaState, "VertexShader");
		renderObject->m_pixelShader = LuaValueGetter<std::wstring>::Get(m_luaState, "PixelSahder");
		renderObject->m_material = LuaValueGetter<std::wstring>::Get(m_luaState, "Material");

		m_renderer->CreateVertexIndexBuffer(renderObject->m_mesh);
		m_renderer->CreateVertexShader(IRenderer::VERTEX_TYPE::VertexPUN, renderObject->m_vertexShader);
		m_renderer->CreatePixelShader(renderObject->m_pixelShader);

		position = LuaValueGetter<Vector3>::Get(m_luaState, "Position");
		rotation = LuaValueGetter<Vector3>::Get(m_luaState, "Rotation");
		scale = LuaValueGetter<Vector3>::Get(m_luaState, "Scale");

		renderObject->m_meshtransform = Matrix::Identity;
		renderObject->m_meshtransform *= Matrix::CreateScale(scale);
		renderObject->m_meshtransform *= Matrix::CreateFromYawPitchRoll(rotation);
		renderObject->m_meshtransform *= Matrix::CreateTranslation(position);

		// 머테리얼 데이터
		FILE_STREAM materialFS;
		m_fms.OpenFile(renderObject->m_material, materialFS);
		if (0 == materialFS.size())
		{
			return;
		}
		DO_STREAM(m_luaState, materialFS);

		table albedoTable = LuaValueGetter<table>::Get(m_luaState, "Textures");
		std::wstring albedoName = ::StrToWstr(LuaValueGetter<std::string>::Get(albedoTable, "albedo"));

		FILE_STREAM textuerStream;
		m_fms.OpenFile(albedoName, textuerStream);

		IRenderer::TextuerData albedoData{ albedoName, textuerStream };
		IRenderer::MaterialData materialData{ {0, 0, 0, 1}, albedoData };

		m_renderer->CreateMaterial(renderObject->m_material, materialData);

		modelObject->m_isDraw = true;
		modelObject->m_renderObjects.push_back(renderObject);
	}

	m_renderer->AddModelObject(modelObject);

	table lights = LuaValueGetter<table>::Get(m_luaState, "Light");
	for (size_t i = 1; i <= lights.size(); i++)
	{
		table light = LuaValueGetter<table>::Get(lights, i);
		UINT type = LuaValueGetter<UINT>::Get(lights, "type");
		float intensity = LuaValueGetter<float>::Get(lights, "intensity");
		Vector3 color = LuaValueGetter<Vector3>::Get(lights, "color");
		Vector3 direction = LuaValueGetter<Vector3>::Get(lights, "direction");

		// 이후 빛 데이터 렌더러에 전달하기
	}
}

void DemoProcess::ResizeWindow(int _width, int _hight)
{
	if (NULL == m_hwnd)
	{
		return;
	}

	RECT nowRect;
	GetWindowRect(m_hwnd, &nowRect);

	DWORD style = (DWORD)GetWindowLong(m_hwnd, GWL_STYLE);
	DWORD exstyle = (DWORD)GetWindowLong(m_hwnd, GWL_EXSTYLE);

	RECT newRect = {};
	newRect.left = 0;
	newRect.top = 0;
	newRect.right = _width;
	newRect.bottom = _hight;

	AdjustWindowRect(&newRect, style, NULL);

	// 클라이언트 영역보다 윈도 크기는 더 커야 한다. (외곽선, 타이틀 등)
	int _newWidth = (newRect.right - newRect.left);
	int _newHeight = (newRect.bottom - newRect.top);

	SetWindowPos(
		m_hwnd
		, HWND_NOTOPMOST
		, nowRect.left
		, nowRect.top
		, _newWidth
		, _newHeight
		, SWP_SHOWWINDOW
	);
}

void DemoProcess::CreateMaterial(const std::wstring& _path)
{
	if (nullptr == m_renderer)
	{
		return;
	}

	FILE_STREAM s;
	m_fms.OpenFile(_path, s);
	if (true == s.empty())
	{
		return;
	}

	// 텍스쳐 정보 가져오기
	DO_STREAM(m_luaState, s);

	FILE_STREAM albedoStream = FILE_STREAM();
	table textuers = LuaValueGetter<table>::Get(m_luaState, "Textuers");
	std::wstring albedo = LuaValueGetter<std::wstring>::Get(textuers, "albedo");

	if (0 != albedo.size())
	{
		m_fms.OpenFile(albedo, albedoStream);
	}
	IRenderer::TextuerData albetotextuer{ albedo, albedoStream };

	IRenderer::MaterialData material{ {0,0,0,0},  albetotextuer };

	m_renderer->CreateMaterial(_path, material);


	return;
}

LRESULT CALLBACK DemoProcess::WndProc(
	HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam
)
{
	// HDC			hdc;
	// PAINTSTRUCT ps;

	switch (message)
	{
		//case WM_PAINT:
		//	hdc = BeginPaint(hWnd, &ps);
		//	EndPaint(hWnd, &ps);
		//	break;
		//
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

		//
	case WM_LBUTTONDOWN:
	{
		m_mouseDown[0] = true;
		break;
	}
	case WM_RBUTTONDOWN:
	{
		m_mouseDown[1] = true;
		break;
	}
	case WM_LBUTTONUP:
	{
		m_mouseDown[0] = false;
		break;
	}
	case WM_RBUTTONUP:
	{
		m_mouseDown[1] = false;
		break;
	}
	case WM_MOUSEMOVE:
	{
		m_currMouseMove[0] = HIWORD(lParam);
		m_currMouseMove[1] = LOWORD(lParam);

		m_mouseMovement[0] = m_currMouseMove[0] - m_oldMouseMove[0];
		m_mouseMovement[1] = m_currMouseMove[1] - m_oldMouseMove[1];

		m_oldMouseMove[0] = m_currMouseMove[0];
		m_oldMouseMove[1] = m_currMouseMove[1];
		break;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

bool DemoProcess::OpenFile(void* _fms, const std::wstring& _filename, OUT FILE_STREAM& _fileData)
{
	if (nullptr == _fms)
	{
		return false;
	}

	FileStorage* fms = static_cast<FileStorage*>(_fms);

	if (nullptr == fms)
	{
		return false;
	}

	return fms->OpenFile(_filename, _fileData);
}
