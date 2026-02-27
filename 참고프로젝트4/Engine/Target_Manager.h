#pragma once

#include "Base.h"

BEGIN(Engine)

class CTarget_Manager final : public CBase
{
private:
	CTarget_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual ~CTarget_Manager() = default;

public:
	HRESULT Initialize();
	HRESULT Add_RenderTarget(const wstring& strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor, _uint iArraySize = 1);
	HRESULT Add_MRT(const wstring& strMRTTag, const wstring& strTargetTag);
	HRESULT Begin_MRT(const wstring& strMRTTag, _bool bClear = true, ComPtr<ID3D11DepthStencilView> pDSV = nullptr);
	HRESULT End_MRT();
	HRESULT Bind_ShaderResource(const wstring& strTargetTag, class CShader* pShader, const _char* pConstantName);
	class CRenderTarget* Find_RenderTarget(const wstring& strTargetTag);
	HRESULT Bake_AntiAliasingDepthStencilView(_uint iWidth, _uint iHeight);
	ComPtr<ID3D11DepthStencilView> Get_AntiAliasingDepthStencilView() { return m_pAntiAliasingDepthStencilView;}
	D3D11_VIEWPORT Get_AntiAliasingViewPortDesc() { return m_tAntiAliasingViewPortDesc; }

#ifdef _DEBUG
public:
	HRESULT Ready_Debug(const wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_Debug(const wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif

private:
	ComPtr<ID3D11Device> m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };

private:
	map<const wstring, class CRenderTarget*>			m_RenderTargets;
	map<const wstring, list<class CRenderTarget*>>		m_MRTs;

	D3D11_VIEWPORT					m_tAntiAliasingViewPortDesc;
	ComPtr<ID3D11DepthStencilView> m_pAntiAliasingDepthStencilView = nullptr;
private:
	list<class CRenderTarget*>* Find_MRT(const wstring& strMRTTag);

public:
	static CTarget_Manager* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual void Free() override;
};

END