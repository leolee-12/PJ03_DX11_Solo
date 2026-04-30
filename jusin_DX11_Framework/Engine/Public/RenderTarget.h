#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CRenderTarget : public CBase
{
protected:
	CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderTarget() = default;

public:
	ID3D11RenderTargetView* Get_RTV()  const { return m_pRTV; }
	ID3D11ShaderResourceView* Get_SRV()  const { return m_pSRV; }

	HRESULT Initialize(_uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstName);
	void Clear();

#ifdef _DEBUG
public:
	HRESULT Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_Debug(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif

protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	ID3D11Texture2D* m_pRTTexture = { nullptr };
	ID3D11RenderTargetView* m_pRTV = { nullptr };
	ID3D11ShaderResourceView* m_pSRV = { nullptr };

	_float4 m_vClearColor = {};

#ifdef _DEBUG
	_float4x4 m_WorldMatrix = {};
#endif

public:
	static CRenderTarget* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);

protected:
	virtual void Free() override;
};

NS_END