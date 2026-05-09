#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CTarget_Manager final : public CBase
{
private:
	CTarget_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTarget_Manager() = default;

public:
	HRESULT Add_RenderTarget(WNameID strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Add_MRT(WNameID strMRTTag, WNameID strTargetTag);
	HRESULT Begin_MRT(WNameID strMRTTag, ID3D11DepthStencilView* pDSV = nullptr);
	HRESULT End_MRT();
	HRESULT Bind_ShaderResource(WNameID strTargetTag, class CShader* pShader, const _char* pConstName);
	HRESULT Copy_Resource(WNameID strTargetTag, ID3D11Texture2D* pOut);
	HRESULT Copy_SubResource(WNameID strTargetTag, ID3D11Texture2D* pOut, const D3D11_BOX* pSrcBox);

	void Reset();

#ifdef _DEBUG
public:
	HRESULT Ready_Debug(WNameID strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_Debug(WNameID strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	ID3D11RenderTargetView* m_pBackBufferRTV = { nullptr };
	ID3D11DepthStencilView* m_pOriginalDSV = { nullptr };

	using MRT = vector<class CRenderTarget*>;
	WNameMap<class CRenderTarget*> m_RenderTargets;
	WNameMap<MRT> m_MRTs;

private:
	class CRenderTarget* Find_RenderTarget(WNameID strTargetTag);
	vector<class CRenderTarget*>* Find_MRT(WNameID strMRTTag);

public:
	static CTarget_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

private:
	virtual void Free();
};

NS_END