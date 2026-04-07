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
		UIANCHOR_DESC m_tAnchorDesc{};
		UILAYOUT_SLOT_DESC m_tLayoutSlot{};
		CUIObject* m_pParentUI = { nullptr };
	};

protected:
	CUIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIObject(const CUIObject& Prototype);
	virtual ~CUIObject() = default;

public:
	virtual _bool Is_UI() { return true; }
	virtual _string Get_TypeName() const { return "UIObject"; }
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	_float m_fViewWidth{}, m_fViewHeight{};
	_float4x4 m_TransformMatrices[ETOUI(D3DTS::END)] = {};

	_float m_fCenterX{}, m_fCenterY{};	// 로컬
	_float m_fResolvedCenterX{}, m_fResolvedCenterY{};	// Combined (anchor, layout 계산 후)
	_float m_fSizeX{}, m_fSizeY{};
	_int m_iZOrder{};
	_bool m_bVisible = true;
	UIANCHOR_DESC m_tAnchorDesc{};
	UILAYOUT_SLOT_DESC m_tLayoutSlot{};
	CUIObject* m_pParentUI = nullptr;

protected:
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName, D3DTS eType);

public:
	virtual CGameObject* Clone(void* pArg) = 0;

protected:
	virtual void Free();
};

NS_END