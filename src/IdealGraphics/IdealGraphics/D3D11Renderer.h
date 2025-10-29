#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl\client.h>
#include <map>
#include <string>
#include <unordered_map>
#include <algorithm>

#include "IRenderer.h"
#include "windows.h"
#include "CustomAllocator/IAllocator.h"
#include "Camera.h"
#include "Vertex.h"
#include "DXTK/DDSTextureLoader.h"
#include "DXTK/WICTextureLoader.h"

using Microsoft::WRL::ComPtr;

class D3D11Renderer :
	public IRenderer
{
private:

	struct Material
	{
		ComPtr<ID3D11ShaderResourceView> m_albedo;
	};

	struct CurrentReder
	{
		ComPtr<ID3D11VertexShader> m_vs = nullptr;
		ComPtr<ID3D11InputLayout> m_ia = nullptr;
		ComPtr<ID3D11PixelShader> m_ps = nullptr;
		ComPtr<ID3D11Buffer> m_vb = nullptr;
		ComPtr<ID3D11Buffer> m_ib = nullptr;
	};

	struct CameraBuffer
	{
		Matrix m_view;
		Matrix m_proj;
	};

	struct WorldBuffer
	{
		Matrix m_world;
	};

private:
	// ������ �ڵ鷯
	HWND m_hwnd;

	// D3D Components
	ComPtr<ID3D11Device> m_device;
	ComPtr<ID3D11DeviceContext> m_deviceContext;
	ComPtr<IDXGISwapChain> m_swapChain;
	ComPtr<ID3D11RenderTargetView> m_finalRenderTargetView;
	ComPtr<ID3D11Texture2D> m_depthStancilBuffer;
	ComPtr<ID3D11DepthStencilView> m_depthStancilView;
	ComPtr<ID3D11RasterizerState> m_rasterizerState;
	ComPtr<ID3D11Texture2D> m_finalTexture2D;
	ComPtr<ID3D11ShaderResourceView> m_finalSRV;

	// Shader map
	std::unordered_map<std::string, std::pair<ComPtr<ID3D11VertexShader>, ComPtr<ID3D11InputLayout>>> m_vsMap;
	std::unordered_map<std::string, ComPtr<ID3D11PixelShader>> m_psMap;

	// Buffer map
	std::unordered_map<std::string, std::pair<ComPtr<ID3D11Buffer>, UINT>> m_vBuffer;
	std::unordered_map<std::string, std::pair<ComPtr<ID3D11Buffer>, UINT>> m_iBuffer;

	// texture map
	std::unordered_map<std::string, ComPtr<ID3D11ShaderResourceView>> m_textuerMap;

	std::unordered_map<std::string, Material> m_materialMap;

	// input layout buffer
	std::vector<ComPtr<ID3D11InputLayout>> m_iaBuffer;

	D3D_FEATURE_LEVEL m_featureLevel;

	// wvp matrix
	Matrix m_world;

	// custom allocator
	std::map<size_t, IAllocator*>m_allocators;

	// default background color
	float m_defaultBG[4];

	// cameras
	std::vector<std::shared_ptr<Camera>> m_cameras;
	std::weak_ptr<Camera> m_mainCamera;
	ComPtr<ID3D11Buffer> m_cameraCBuffer;

	// render object vector
	std::vector<std::shared_ptr<IRenderObject>> m_renderVector;

	CurrentReder m_currentRenderSet;

	ComPtr<ID3D11Buffer> m_worldBuffer;

	UINT m_renderWidth;
	UINT m_renderHight;

public:
	D3D11Renderer();
	virtual ~D3D11Renderer();

private:
#pragma region Create
	/// <summary>
	/// D3D11 Device �� Device Context�� ����
	/// </summary>
	/// <param name="_device">Device ������ ���� ��</param>
	/// <returns>���� ����</returns>
	IE CreateD3D11DeviceContext(const InitializeState::Device& _device);

	/// <summary>
	/// swap chain ����
	/// </summary>
	/// <param name="_swapChain">SwapCahin ������ ���� ��</param>
	/// <returns>���� ����</returns>
	IE CreateSwapChain(const InitializeState::SwapCahin& _swapChain);

	/// <summary>
	/// ���� ȭ���� �׸� ���� Ÿ�� �� ����
	/// </summary>
	/// <returns>���� ����</returns>
	IE CreateFinalRenterTargetView();

	/// <summary>
	/// depth stancil ���ۿ� �� ����
	/// </summary>
	/// <param name="_depthStancil">������ ���� ��</param>
	/// <returns>���� ����</returns>
	IE CreateDepthStencilBufferAndView(const InitializeState::DepthStancil& _depthStancil);

	/// <summary>
	/// ����Ʈ�� ����� �����Ͷ������� ���ε�
	/// </summary>
	IE CreateViewPort();

	/// <summary>
	/// �����Ͷ����� ����
	/// </summary>
	/// <param name="_rasterizer">���� ��</param>
	/// <returns>���� ����</returns>
	IE CreateRasterizerState(const InitializeState::RaseterizerState& _rasterizer);

	/// <summary>
	/// ����� �Լ�
	/// ȭ���� Ư�� ������ ä���
	/// </summary>
	IE ClearScreen();

	/// <summary>
	/// ī�޶� ���� ����
	/// </summary>
	/// <returns>���� ����</returns>
	IE CreateCameraBuffer();

	/// <summary>
	/// ���� ���� ����
	/// </summary>
	/// <returns>���� ����</returns>
	IE CreateWorldBuffer();

	/// <summary>
	/// InputLayout ����
	/// </summary>
	/// <param name="_type">���� Ÿ��</param>
	/// <param name="_stream">vs ������</param>
	/// <returns>���� ����</returns>
	IE CreateInputLayout(VERTEX_TYPE _type, const std::vector<unsigned char>& _stream);
#pragma endregion

#pragma region Bind
	/// <summary>
	/// ���̴��� ���� ī�޶� ���ε�
	/// </summary>
	/// <returns>���� ����</returns>
	IE BindMainCameraBuffer();

	/// <summary>
	/// ���̴��� ���� ���� ���ε�
	/// </summary>
	/// <returns>���� ����</returns>
	IE BindWorldBuffer(const Matrix& _matrix);

	/// <summary>
	/// ���� ���̴��� �׿� �´� Input layout�� ���ε�
	/// </summary>
	/// <param name="_vs">���� ���̴�</param>
	/// <param name="_ia">Input Layout</param>
	/// <returns>���� ����</returns>
	IE BindVertexShaderAndInputLayout(ComPtr<ID3D11VertexShader> _vs, ComPtr<ID3D11InputLayout> _ia);

	/// <summary>
	/// �ȼ� ���̴� ���ε�
	/// </summary>
	/// <param name="_ps">�ȼ� ���̴�</param>
	/// <returns>���� ����</returns>
	IE BindPixelShader(ComPtr<ID3D11PixelShader> _ps);

	/// <summary>
	/// ���� ���� ���ε�
	/// </summary>
	/// <param name="_vb">���� ����</param>
	/// <param name="_size">����� ũ��</param>
	/// <returns>���� ����</returns>
	IE BindVertexBuffer(ComPtr<ID3D11Buffer> _vb, UINT _size);

	/// <summary>
	/// �ε��� ���� ���ε�
	/// </summary>
	/// <param name="_id">�ε��� ����</param>
	/// <returns>���� ����</returns>
	IE BindIndexBuffer(ComPtr<ID3D11Buffer> _id);
#pragma endregion

public:

	/// <summary>
	/// �ʱ�ȭ
	/// </summary>
	/// <param name="_initalizeState">�ʱ�ȭ ����</param>
	/// <returns>�ʱ�ȭ ���� ����</returns>
	IE Initialize(const InitializeState& _initalizeState, HWND _hwnd) override;

	/// <summary>
	/// ���� �Ž� ����Ʈ
	/// </summary>
	/// <returns>���� �Ž� UID</returns>
	IE ImportStaticMesh() override;

	/// <summary>
	/// �� �Ž� ����Ʈ
	/// </summary>
	/// <returns>�� �Ž� UID</returns>
	IE ImportBoneMesh() override;

	/// <summary>
	/// �ִϸ��̼� ����Ʈ
	/// </summary>
	/// <returns>�ִϸ��̼� UID</returns>
	IE ImportAnimation() override;

	/// <summary>
	/// �ؽ��� ����
	/// </summary>
	/// <returns>�ؽ��� UID</returns>
	IE CreateTexture(const TextuerData& _textuerData) override;

	/// <summary>
	/// ���׸��� ����
	/// </summary>
	/// <returns>���׸��� UID</returns>
	IE CreateMaterial(const std::string _name, const MaterialData& _material) override;

	/// <summary>
	/// ī�޶� ����
	/// </summary>
	/// <returns>ī�޶� ID</returns>
	std::weak_ptr<ICamera> CreateCamera() override;

	/// <summary>
	/// ���� ī�޶� ����
	/// </summary>
	/// <param name="_cameraID">ī�޶� UID</param>
	/// <returns>���� ����</returns>
	IE SetCamera(std::weak_ptr<ICamera> _cameraID) override;

	/// <summary>
	/// �Ž��� �ִϸ��̼� ����
	/// </summary>
	/// <param name="_meshID">�Ž� UID</param>
	/// <returns>���� ����</returns>
	IE SetAniamtion(int _meshID) override;

	/// <summary>
	/// ���� ������
	/// </summary>
	/// <returns>���� ����</returns>
	IE Draw() override;

	/// <summary>
	/// ���� Ÿ�� Ŭ����� ����� ��
	/// </summary>
	/// <param name="_r">R</param>
	/// <param name="_g">G</param>
	/// <param name="_b">B</param>
	/// <param name="_a">A</param>
	/// <returns>���� ����</returns>
	IE SetBackgroundColor(float _r, float _g, float _b, float _a) override
	{
		m_defaultBG[0] = _r;
		m_defaultBG[1] = _g;
		m_defaultBG[2] = _b;
		m_defaultBG[3] = _a;

		return IE::I_OK;
	};

	/// <summary>
	/// ���� ���̴� ����
	/// </summary>
	/// <param name="_name">���̴� �̸�</param>
	/// <param name="_stream">���̴� ������ ��Ʈ��</param>
	/// <returns>���� ����</returns>
	IE CreateVertexShader(VERTEX_TYPE _type, const std::string&, CONST_FILE_STREAM& _stream) override;

	/// <summary>
	/// �ȼ� ���̴� ����
	/// </summary>
	/// <param name="_name">���̴� �̸�</param>
	/// <param name="_stream">���̴� ������ ��Ʈ��</param>
	/// <returns>���� ����</returns>
	IE CreatePixelShader(const std::string&, CONST_FILE_STREAM& _stream) override;

	/// <summary>
	/// ���� �� �ε��� ���� ����
	/// </summary>
	/// <param name="_name">�Ž� �̸�</param>
	/// <param name="_stream">������</param>
	/// <returns>���� ����</returns>
	IE CreateVertexIndexBuffer(CONST_FILE_STREAM& _stream, OUT std::string& _name) override;

	IE AddRenderObject(std::shared_ptr<IRenderObject> _renderObject) override;

	/// <summary>
	/// �� ������Ʈ �߰�
	/// </summary>
	/// <param name="_renderObject">�� ������Ʈ</param>
	/// <returns>���� ����</returns>
	IE AddModelObject(std::shared_ptr<IModelObject> _modelObject) override;

	/// <summary>
	/// ���� ���� ����
	/// </summary>
	/// <param name="_w">����</param>
	/// <param name="_h">�ʺ�</param>
	/// <returns>���� ����</returns>
	IE SetRenderSize(UINT _w, UINT _h) override;
};

