#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl\client.h>
#include <map>
#include <string>
#include <unordered_map>

#include "IRenderer.h"
#include "windows.h"
#include "CustomAllocator/IAllocator.h"

using Microsoft::WRL::ComPtr;

class D3D11Renderer :
	public IRenderer
{
private:
	HWND m_hwnd;

	ComPtr<ID3D11Device> m_device;
	ComPtr<ID3D11DeviceContext> m_deviceContext;
	ComPtr<IDXGISwapChain> m_swapChain;
	ComPtr<ID3D11RenderTargetView> m_finalRenderTargetView;
	ComPtr<ID3D11Texture2D> m_depthStancilBuffer;
	ComPtr<ID3D11DepthStencilView> m_depthStancilView;
	ComPtr<ID3D11RasterizerState> m_rasterizerState;
	ComPtr<ID3D11Texture2D> m_finalTexture2D;
	ComPtr<ID3D11ShaderResourceView> m_finalSRV;

	std::unordered_map<std::string, ComPtr<ID3D11VertexShader>> m_vsMap;
	std::unordered_map<std::string, ComPtr<ID3D11PixelShader>> m_psMap;

	D3D_FEATURE_LEVEL m_featureLevel;

	Matrix m_world;
	Matrix m_view;
	Matrix m_projection;

	std::map<size_t, IAllocator*>m_allocators;

	float m_defaultBG[4];

public:
	D3D11Renderer();
	virtual ~D3D11Renderer();

private:
	/// �ʱ�ȭ �Լ� ����

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
	/// �ؽ��� ����Ʈ
	/// </summary>
	/// <returns>�ؽ��� UID</returns>
	IE ImportTexture() override;

	/// <summary>
	/// ���׸��� ����Ʈ
	/// </summary>
	/// <returns>���׸��� UID</returns>
	IE ImportMaterial() override;

	/// <summary>
	/// ī�޶� ����
	/// </summary>
	/// <returns>ī�޶� UID</returns>
	IE CreateCamera() override;

	/// <summary>
	/// ���� ī�޶� ����
	/// </summary>
	/// <param name="_cameraID">ī�޶� UID</param>
	/// <returns>���� ����</returns>
	IE SetCamera(int _cameraID) override;

	/// <summary>
	/// �Ž��� �ִϸ��̼� ����
	/// </summary>
	/// <param name="_meshID">�Ž� UID</param>
	/// <returns>���� ����</returns>
	IE SetAniamtion(int _meshID) override;

	/// <summary>
	/// ���׸��� ����
	/// </summary>
	/// <param name="_mehsID">������ �Ž� UID</param>
	/// <returns>���� ����</returns>
	IE SetMaterial(int _mehsID) override;

	/// <summary>
	/// �ؽ��� ����
	/// </summary>
	/// <param name="_materialID">������ ���׸��� UID</param>
	/// <param name="_slot">������ ����</param>
	/// <returns>���� ����</returns>
	IE SetTexture(int _materialID, int _slot = 0) override;

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
	IE CreateVertexShader(const std::string&, std::stringstream* _stream) override;

	/// <summary>
	/// �ȼ� ���̴� ����
	/// </summary>
	/// <param name="_name">���̴� �̸�</param>
	/// <param name="_stream">���̴� ������ ��Ʈ��</param>
	/// <returns>���� ����</returns>
	IE CreatePixelShader(const std::string&, std::stringstream* _stream) override;
};

