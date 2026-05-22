#pragma once
#include "Base.h"

/* -------------------------------------------------- */
// 렌더러
// - 화면에 그려져야 할 객체들을 그리는 순서대로 모아놓는다
// - 보관된 순서대로 객체들의 드로우콜을 해준다
/* -------------------------------------------------- */


NS_BEGIN(Engine)
class CGameInstance;
class CGameObject;
class CShader;
class CVIBuffer_Rect;

class CRenderer final : public CBase
{
private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

public:
	HRESULT	Initialize();
	void	Add_RenderGroup(RENDERID eGroupID, class CGameObject* pGameObject);
	HRESULT	Draw();
	HRESULT	Resize();

	void	Set_UseShadow(_bool b) { m_bUseShadow = b; }
	void	Set_OutlineParam(const OUTLINE_PARAM& Param) { m_OutlineParam = Param; }

#ifdef _DEBUG
	void Add_DebugComponent(class CComponent* pComponent);
#endif

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	ID3D11DepthStencilView*		m_pMaxDSV = { nullptr };

	CGameInstance*		m_pGameInstance = { nullptr };
	list<CGameObject*>	m_RenderObjects[ETOUI(RENDERID::END)];

	CShader*			m_pShader = { nullptr };
	CShader*			m_pShader_PostProcess = { nullptr };
	CVIBuffer_Rect*		m_pVIBuffer = { nullptr };

	_float4x4			m_WorldMatrix{}, m_ViewMatrix{}, m_ProjMatrix{};
	_bool				m_bUseShadow = { true };
	OUTLINE_PARAM		m_OutlineParam{};

#ifdef _DEBUG
	list<class CComponent*>		m_DebugComponents;
#endif

private:
	HRESULT Render_Priority();
	HRESULT Render_Shadow();
	HRESULT Render_NonBlend();
	HRESULT Render_OutlineMask();
	HRESULT Render_Lights();
	HRESULT Render_Combined(_bool m_bUseShadow);
	HRESULT Render_PostProcess();
	HRESULT Render_NonLight();
	HRESULT Render_Blend();
	HRESULT Render_UI();

private:
	HRESULT Ready_DepthStencil_Buffer();
	HRESULT Change_ViewportDesc(_uint iWidth, _uint iHeight);

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