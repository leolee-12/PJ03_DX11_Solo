#pragma once

#include "Base.h"

BEGIN(Engine)

class CRenderTarget final : public CBase
{
private:
	CRenderTarget(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual ~CRenderTarget() = default;

public:
	ComPtr<ID3D11RenderTargetView> Get_RTV() const {
		return m_pRTV;
	}
	ComPtr<ID3D11ShaderResourceView> Get_SRV() const {
		return m_pSRV;
	}

public:
	HRESULT Initialize(_uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, _float4 vClearColor, _uint iArraySize = 1);
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName);
	HRESULT Clear();

#ifdef _DEBUG
public:
	HRESULT Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_Debug(class CShader* pShader, class CVIBuffer* pVIBuffer);
#endif

private:
	ComPtr<ID3D11Device> m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };

private:
	ComPtr<ID3D11Texture2D> m_pTexture2D = { nullptr };
	ComPtr<ID3D11RenderTargetView> m_pRTV = { nullptr };
	ComPtr<ID3D11ShaderResourceView> m_pSRV = { nullptr };

	_float4						m_vClearColor;

#ifdef _DEBUG
private:
	_float4x4				m_WorldMatrix;

#endif




public:
	static CRenderTarget* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, _float4 vClearColor, _uint iArraySize = 1);
	virtual void Free() override;
};

END