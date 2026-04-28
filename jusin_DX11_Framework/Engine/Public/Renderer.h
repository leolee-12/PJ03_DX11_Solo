#pragma once
#include "Base.h"

/* -------------------------------------------------- */
// 렌더러
// - 화면에 그려져야 할 객체들을 그리는 순서대로 모아놓는다
// - 보관된 순서대로 객체들의 드로우콜을 해준다
/* -------------------------------------------------- */


NS_BEGIN(Engine)

class CRenderer final : public CBase
{
private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

public:
	HRESULT		Initialize();
	void		Add_RenderGroup(RENDERID eGroupID, class CGameObject* pGameObject);
	HRESULT		Draw();

#ifdef _DEBUG
	void Add_DebugComponent(class CComponent* pComponent);
#endif

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

	class CGameInstance*		m_pGameInstance = { nullptr };
	list<class CGameObject*>	m_RenderObjects[ETOUI(RENDERID::END)];

	class CShader*				m_pShader = { nullptr };
	class CVIBuffer_Rect*		m_pVIBuffer = { nullptr };
	_float4x4					m_WorldMatrix{}, m_ViewMatrix{}, m_ProjMatrix{};

#ifdef _DEBUG
	list<class CComponent*>		m_DebugComponents;
#endif

private:
	HRESULT Render_Priority();
	HRESULT Render_NonBlend();
	HRESULT Render_Lights();
	HRESULT Render_Combined();
	HRESULT Render_NonLight();
	HRESULT Render_Blend();
	HRESULT Render_UI();

#ifdef _DEBUG
private:
	HRESULT Render_Debug();
#endif

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

protected:
	virtual void Free() override;
};

NS_END