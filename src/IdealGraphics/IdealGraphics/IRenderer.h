#pragma once

#ifdef IDEALGRAPHICS_EXPORTS
#define IDEALGRAPHICS_DLL __declspec(dllexport)
#else
#define IDEALGRAPHICS_DLL __declspec(dllimport)
#endif

#include "RenameType.h"
#include "IdealError.h"
#include "ICamera.h"

#include "DXTK/SimpleMath.h"

#include <memory>
#include <vector>

using namespace DirectX::SimpleMath;

class IRenderer
{
public:

	/// <summary>
	/// ������ ������Ʈ ����
	/// </summary>
	struct IRenderObject
	{
		std::string m_vertexShader;
		std::string m_pixelShader;
		std::string m_mesh;
		std::string m_metrial;
		Matrix m_world;
		bool m_isDraw;

		IRenderObject() = default;

		// ���� �� ���縦 �����ϱ� ����
		IRenderObject(const IRenderObject&) = default;
		IRenderObject& operator=(const IRenderObject&) = default;

		IRenderObject(IRenderObject&&) noexcept = default;
		IRenderObject& operator=(IRenderObject&&) noexcept = default;

		bool operator<(const IRenderObject& other) const noexcept 
		{
			if (m_isDraw != other.m_isDraw)
			{
				return m_isDraw;
			}

			int cmp = m_vertexShader.compare(other.m_vertexShader);
			if (0 != cmp)
			{
				return cmp < 0;
			}
			cmp = m_pixelShader.compare(other.m_pixelShader);
			if (0 != cmp)
			{
				return cmp < 0;
			}
			cmp = m_metrial.compare(other.m_metrial);
			if (0 != cmp)
			{
				return cmp < 0;
			}
			cmp = m_mesh.compare(other.m_mesh);
			if (0 != cmp)
			{
				return cmp < 0;
			}

			return false;
		}
	};

	/// <summary>
	/// �ʱ�ȭ ������Ʈ ����ü
	/// </summary>
	struct InitializeState
	{
		UINT m_renderVectorSize;

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

	enum class VERTEX_TYPE
	{
		VertexSuper,
		VertexP,
		VertexPU,
		VertexPN,
		VertexPUN,
		VertexPC,
		END,
	};

	enum class GEOMETRY_TYPE
	{
		PLANE,
		CUBE,
		AXES,
		END,
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
	virtual std::weak_ptr<ICamera> CreateCamera() = 0;

	/// <summary>
	/// ���� ī�޶� ����
	/// </summary>
	/// <param name="_cameraID">ī�޶� ���̵�</param>
	/// <returns>���� ���� </returns>
	virtual IE SetCamera(std::weak_ptr<ICamera> _cameraID) = 0;

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
	virtual IE CreateVertexShader(VERTEX_TYPE _type, const std::string& _name, const std::vector<unsigned char>& _stream) = 0;

	/// <summary>
	/// �ȼ� ���̴� ����
	/// </summary>
	/// <param name="_name">���̴� �̸�</param>
	/// <param name="_stream">���̴� ������ ��Ʈ��</param>
	/// <returns>���� ����</returns>
	virtual IE CreatePixelShader(const std::string&, const std::vector<unsigned char>& _stream) = 0;

	/// <summary>
	/// ���� �� �ε��� ���� ����
	/// </summary>
	/// <param name="_name">�Ž� �̸�</param>
	/// <param name="_stream">������</param>
	/// <returns>���� ����</returns>
	virtual IE CreateVertexIndexBuffer(std::string _name, const std::vector<unsigned char>& _stream) = 0;

	/// <summary>
	/// ������ ������Ʈ �߰�
	/// </summary>
	/// <param name="_renderObject">������ ������Ʈ</param>
	/// <returns>���� ����</returns>
	virtual IE AddRenderObject(const IRenderObject& _renderObject) = 0;

	/// <summary>
	/// ���� ���� ����
	/// </summary>
	/// <param name="_w">����</param>
	/// <param name="_h">�ʺ�</param>
	/// <returns>���� ����</returns>
	virtual IE SetRenderSize(UINT _w, UINT _h) = 0;
};

/// <summary>
/// ���� �Ҵ��� ����
/// </summary>
/// <returns> ���� �Ҵ���</returns>
extern "C" IDEALGRAPHICS_DLL IRenderer* CreateD3D11Renderer(); 
