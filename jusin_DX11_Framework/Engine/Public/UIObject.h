#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CUIObject abstract : public CGameObject
{
public:
	struct UIOBJECT_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		_float fCenterX{}, fCenterY{};
		_float fSizeX{}, fSizeY{};
		_int iZOrder{};
		_bool bVisible = { true };
		UIANCHOR_DESC tAnchorDesc{};
		UILAYOUT_SLOT_DESC tLayoutSlot{};
		CUIObject* pParentUI = { nullptr };
	};

protected:
	CUIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIObject(const CUIObject& Prototype);
	virtual ~CUIObject() = default;

public:
	virtual _bool Is_UI() { return true; }
	virtual _string Get_TypeName() const { return "UIObject"; }
	virtual UI_TYPE Get_UIType() const { return UI_TYPE::WIDGET; }
	
	void Set_Center(_float fCenterX, _float fCenterY);
	void Set_Size(_float fSizeX, _float fSizeY);
	void Set_ZOrder(_int iZOrder) { m_iZOrder = iZOrder; }
	void Set_LayoutSlot(const UILAYOUT_SLOT_DESC& tLayoutDesc) { m_tLayoutSlot = tLayoutDesc; };
	void Set_Anchor(UI_ANCHOR eAnchor, _float fOffsetX, _float fOffsetY, _bool bUseAnchoredPos = true);
	void Set_AnchorOffset(_float fOffsetX, _float fOffsetY);
	void Set_ParentUI(CUIObject* pParent) { m_pParentUI = pParent; }
	void Set_Visible(_bool b) { m_bVisible = b; }

	_float2 Get_Center() const { return _float2(m_fResolvedCenterX, m_fResolvedCenterY); }
	_float2 Get_Size() const { return _float2(m_fSizeX, m_fSizeY); }
	_int Get_ZOrder() const { return m_iZOrder; }
	_bool Get_Visible() { return m_bVisible; }
	_float4 Get_ScreenRect() const;
	const UILAYOUT_SLOT_DESC& Get_LayoutSlot() const { return m_tLayoutSlot; }
	UI_ANCHOR Get_Anchor() { return (m_tAnchorDesc.bUseAnchoredPos ? m_tAnchorDesc.eAnchor : UI_ANCHOR::END); }
	CUIObject* Get_ParentUI() const { return m_pParentUI; }
	class CUIAnimator* Get_Animator() const { return m_pAnimatorCom; }

	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Refresh_Layout();
	void Apply_LayoutCenter(_float fCenterX, _float fCenterY);

	virtual _bool Can_Apply_Tween_Target(UI_TWEEN_TARGET eTarget) const;
	virtual HRESULT Apply_Tween_Target(UI_TWEEN_TARGET eTarget, _float fValue);
	
protected:
	_float2 m_vRefSize{};
	_float4x4 m_TransformMatrices[ETOUI(D3DTS::END)] = {};

	_float m_fCenterX{}, m_fCenterY{};	// 로컬
	_float m_fResolvedCenterX{}, m_fResolvedCenterY{};	// Combined (anchor, layout 계산 후)
	_float m_fSizeX{}, m_fSizeY{};
	_float m_fRotation{ 0.f };
	_int m_iZOrder{};
	_bool m_bVisible = true;
	UIANCHOR_DESC m_tAnchorDesc{};
	UILAYOUT_SLOT_DESC m_tLayoutSlot{};
	CUIObject* m_pParentUI = nullptr;

	class CUIAnimator* m_pAnimatorCom = { nullptr };

protected:
	HRESULT Ready_Animator();
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName, D3DTS eType);
	_float4 Get_ReferenceRect() const;
	_float2 Resolve_AnchorCenter() const;
	void Update_UI_Transform();

public:
	virtual CGameObject* Clone(void* pArg) = 0;

protected:
	virtual void Free();
};

NS_END
