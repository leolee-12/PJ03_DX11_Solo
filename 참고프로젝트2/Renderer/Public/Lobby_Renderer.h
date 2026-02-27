#pragma once

#include "Base.h"

/* 1. 화면에 그려야할 객체들을 그리는 순서대로 보관한다. */
/* 2. 보관하고 있는 객체들의 렌더함수를 호출한다.(렌더콜) */

BEGIN(Engine)
class CGameObject;
class CShader;
class CVIBuffer_Rect;
class CTransform;
class CGameInstance;
class CComponent;
END

BEGIN(Renderer)

class CLobby_Renderer final : public CBase
{
public:
	enum RENDERGROUP { RG_PRIORITY, RG_NONBLEND, RG_SHADOWOBJ, RG_NONLIGHT, RG_BLEND, RG_UI, RG_END };
private:
	CLobby_Renderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLobby_Renderer() = default;

public:
	HRESULT Initialize();
	HRESULT Add_LobbyRenderObject(RENDERGROUP eRenderGroup, class CGameObject* pRenderObject);
	HRESULT Add_LobbyDebugComponent(class CComponent* pDebugComponent);
	HRESULT Draw(_float fTimeDelta);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	list<class CGameObject*>	m_RenderObjects[RG_END];
	list<class CComponent*>		m_DebugComponent;

	class CGameInstance* m_pGameInstance = { nullptr };
	class CRenderInstance* m_pRenderInstance = { nullptr };

	class CShader* m_pShader = { nullptr };
	class CVIBuffer_Rect* m_pVIBuffer = { nullptr };

	ID3D11DepthStencilView* m_pShadowDSV = { nullptr };

	_float4x4					m_WorldMatrix = {};
	_float4x4					m_ViewMatrix = {};
	_float4x4					m_ProjMatrix = {};


private:
	HRESULT Render_Priority(_float fTimeDelta);
	HRESULT Render_ShadowObj(_float fTimeDelta);
	HRESULT Render_NonBlend(_float fTimeDelta);
	HRESULT Render_Lights(_float fTimeDelta);
	HRESULT Render_Deferred(_float fTimeDelta);
	HRESULT Render_NonLight(_float fTimeDelta);
	HRESULT Render_Blend(_float fTimeDelta);
	HRESULT Render_UI(_float fTimeDelta);

#ifdef _DEBUG
private:
	HRESULT Render_Debug(_float fTimeDelta);
#endif

public:
	static CLobby_Renderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END