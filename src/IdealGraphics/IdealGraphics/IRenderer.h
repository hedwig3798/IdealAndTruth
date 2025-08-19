#pragma once
#include "RenameType.h"
#include "IdealError.h"

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
	};

	IRenderer() {};
	virtual ~IRenderer() {};

	/// <summary>
	/// ������ �ʱ�ȭ
	/// </summary>
	/// <returns>���� ����</returns>
	virtual IE Initialize(const InitializeState& _initalizeState) = 0;

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
	/// ���� �Ž� ����
	/// </summary>
	/// <param name="_GUID">�Ž� ������ GUID</param>
	/// <returns>�Ž� ID</returns>
	virtual IE CreateStaticMesh(int _GUID) = 0;

	/// <summary>
	/// �� �Ž� ����
	/// </summary>
	/// <param name="_GUID">�� �Ž� ������</param>
	/// <returns>�Ž� ID</returns>
	virtual IE CreateBoneMesh(int _GUID) = 0;

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
};

