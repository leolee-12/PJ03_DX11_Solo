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

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	WNameMap<class CRenderTarget*> m_RenderTargets;

public:
	class CRenderTarget* Find_RenderTarget(WNameID strTargetTag);

public:
	static CTarget_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

private:
	virtual void Free();
};

NS_END