#pragma once

#include "Base.h"

/* 실제 레벨안에서 그려져야할 다양한 객체들을 모아놓는다 .*/
/* 그려야하는 순서대로 모아놓는다. : 깊이 테스트를 수행하고 있기때문에 그리는 순서가 상관이 없지 .*/
/* 특수한 몇몇 객체는 깊이 테스트로 인하여 그리는 순서 관리가 필요한 경우가 있다. */

/* 하늘 : 가장 먼저 그린다. */
/* 블렌드오브젝트 : 불투명한 애들 다음. */
/* 유아이 : 깊이 비교 없이 무조건 덮고 그리기위해 가장 마지막에 그린다. */



NS_BEGIN(Engine)

class CRenderer final : public CBase
{
private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

public:
	HRESULT Initialize();
	HRESULT Add_RenderGroup(RENDERGROUP eGroupID, class CGameObject* pRenderObject);
	void Draw();

#ifdef _DEBUG
	HRESULT Add_DebugComponent(class CComponent* pDebugComponent);
#endif

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };

	_float4x4				m_WorldMatrix, m_ViewMatrix{}, m_ProjMatrix{};
	class CVIBuffer_Rect*	m_pVIBuffer = { nullptr };
	class CShader*			m_pShader = { nullptr };

	ID3D11DepthStencilView* m_pShadowDSV = { nullptr };

private:
	list<class CGameObject*>			m_RenderObjects[ENUM_CLASS(RENDERGROUP::END)];	
#ifdef _DEBUG
private:
	list<class CComponent*>			m_DebugComponents;
#endif

private:
	HRESULT Ready_Shadow_DSV();
	HRESULT Change_Viewport(_uint iWidth, _uint iHeight);

private:
	void Render_Priority();
	void Render_Shadow();
	void Render_NonBlend();	
	void Render_Lights();
	void Render_Combined();
	void Render_Blur();
	void Render_NonLights();
	void Render_Blend();
	void Render_UI();

#ifdef _DEBUG
private:
	void Render_Debug();
#endif
public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END