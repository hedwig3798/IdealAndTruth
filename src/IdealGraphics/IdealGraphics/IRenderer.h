#pragma once

#ifdef IDEALGRAPHICS_EXPORTS
#define IDEALGRAPHICS_DLL __declspec(dllexport)
#else
#define IDEALGRAPHICS_DLL __declspec(dllimport)
#endif

#include "RenameType.h"
#include "IdealError.h"
#include "DXTK/SimpleMath.h"
#include <d3d11.h>

using namespace DirectX::SimpleMath;

class IRenderer
{
public:

	/// <summary>
	/// �ʱ�ȭ ������Ʈ ����ü
	/// </summary>
	struct InitializeState
	{
		/// <summary>
		/// ����̽� �÷���
		/// </summary>
		struct Device
		{
			UINT m_deviceFlags;
		} m_device;

		/// <summary>
		/// ���� ü�� ������Ʈ
		/// </summary>
		struct SwapCahin
		{
			UINT m_width;
			UINT m_height;
			UINT m_refreshRateNumerator;
			UINT m_refreshRateDenominator;
			UINT m_format;
			UINT m_scanlineOrdering;
			UINT m_scaling;

			UINT m_sampleCount;
			UINT m_sampleQuality;

			UINT m_bufferCount;

			UINT m_swapEffect;
			UINT m_flag;
			bool m_isWindowed;
		} m_swapChain;

		/// <summary>
		/// ���� ���ٽ� ������Ʈ
		/// </summary>
		struct DepthStancil
		{
			UINT m_width;
			UINT m_height;
			UINT m_mipLevel;
			UINT m_arraySize;
			UINT m_format;
			UINT m_sampleCount;
			UINT m_sampleQuality;
			UINT m_usage;
		} m_depthStancil;

		/// <summary>
		/// �����Ͷ����� ������Ʈ
		/// </summary>
		struct RaseterizerState
		{
			UINT m_fillMode;
			UINT m_cullMode;
			bool m_isFrontCCW;
			bool m_isDepthClip;
		} m_rasterizer;

		/// <summary>
		/// ���� Ÿ�� ������Ʈ
		/// </summary>
		struct RenderTargetViewState
		{
			UINT m_width;
			UINT m_height;
			UINT m_mipLevel;
			UINT m_arraySize;
			UINT m_format;
			UINT m_sampleCount;
			UINT m_usage;
			UINT m_bindFlags;
			UINT m_viewDimension;
		} m_renderTargetView;
	};

	IRenderer() {};
	virtual ~IRenderer() {};

	/// <summary>
	/// ������ �ʱ�ȭ
	/// </summary>
	/// <returns>���� ����</returns>
	virtual IE Initialize(const InitializeState& _initalizeState, HWND _hwnd) = 0;

	/// <summary>
	/// ���� �Ž� ������ ����Ʈ
	/// </summary>
	/// <returns>������Ʈ ID</returns>
	virtual IE ImportStaticMesh() = 0;

	/// <summary>
	/// �� �Ž� ������ ����Ʈ
	/// </summary>
	/// <returns>������Ʈ GUID</returns>
	virtual IE ImportBoneMesh() = 0;

	/// <summary>
	/// �ִϸ��̼� ������ ����Ʈ
	/// </summary>
	/// <returns>������Ʈ GUID</returns>
	virtual IE ImportAnimation() = 0;

	/// <summary>
	/// �ؽ��� ������ ����Ʈ
	/// </summary>
	/// <returns>������Ʈ GUID</returns>
	virtual IE ImportTexture() = 0;

	/// <summary>
	/// ���׸��� ������ ����Ʈ
	/// </summary>
	/// <returns>������Ʈ GUID</returns>
	virtual IE ImportMaterial() = 0;

	/// <summary>
	/// ī�޶� ������Ʈ ����
	/// </summary>
	/// <returns>ī�޶� ID</returns>
	virtual IE CreateCamera() = 0;

	/// <summary>
	/// ���� ī�޶� ����
	/// </summary>
	/// <param name="_cameraID">ī�޶� ���̵�</param>
	/// <returns>���� ���� </returns>
	virtual IE SetCamera(int _cameraID) = 0;

	/// <summary>
	/// ���ϸ��̼� ����
	/// </summary>
	/// <param name="_meshID">�Ž� ID</param>
	/// <returns>���� ����</returns>
	virtual IE SetAniamtion(int _meshID) = 0;

	/// <summary>
	/// �Ž��� ���� ����
	/// </summary>
	/// <param name="_mehsID">�Ž� ID</param>
	/// <returns>���� ����</returns>
	virtual IE SetMaterial(int _mehsID) = 0;

	/// <summary>
	/// ���׸��� �ؽ��� ����
	/// </summary>
	/// <param name="_materialID">���׸��� ID</param>
	/// <param name="_slot">���׸����� ����</param>
	/// <returns>���� ����</returns>
	virtual IE SetTexture(int _materialID, int _slot = 0) = 0;

	/// <summary>
	/// Draw Call
	/// </summary>
	/// <returns>���� ����</returns>
	virtual IE Draw() = 0;

	/// <summary>
	/// ���� Ÿ�� Ŭ����� ����� ��
	/// </summary>
	/// <param name="_r">R</param>
	/// <param name="_g">G</param>
	/// <param name="_b">B</param>
	/// <param name="_a">A</param>
	/// <returns>���� ����</returns>
	virtual IE SetBackgroundColor(float _r, float _g , float _b, float _a) = 0;

	/// <summary>
	/// ���� ���̴� ����
	/// </summary>
	/// <param name="_name">���̴� �̸�</param>
	/// <param name="_stream">���̴� ������ ��Ʈ��</param>
	/// <returns>���� ����</returns>
	virtual IE CreateVertexShader(const std::string& _name, std::stringstream* _stream) = 0;

	/// <summary>
	/// �ȼ� ���̴� ����
	/// </summary>
	/// <param name="_name">���̴� �̸�</param>
	/// <param name="_stream">���̴� ������ ��Ʈ��</param>
	/// <returns>���� ����</returns>
	virtual IE CreatePixelShader(const std::string&, std::stringstream* _stream) = 0;
};

/// <summary>
/// ���� �Ҵ��� ����
/// </summary>
/// <returns> ���� �Ҵ���</returns>
extern "C" IDEALGRAPHICS_DLL IRenderer* CreateD3D11Renderer(); 
