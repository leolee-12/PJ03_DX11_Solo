#pragma once
#include "UIObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CUIButton : public CUIObject
{
public:
	enum class UI_BUTTON_STATE { NORMAL, HOVER, PRESSED, DISABLED, END };

	struct UIBUTTON_DESC : public CUIObject::UIOBJECT_DESC
	{
		WNameID strTextureTag{ INVALID_TAG };
		_uint iNormalTextureIndex{ INVALID_INDEX };
		_uint iHoverTextureIndex{ INVALID_INDEX };
		_uint iPressedTextureIndex{ INVALID_INDEX };
		_uint iDisabledTextureIndex{ INVALID_INDEX };
		_bool bInteractable{ true };
	};

protected:
	CUIButton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIButton(const CUIButton& Prototype);
	virtual ~CUIButton() = default;

public:
	virtual _string Get_TypeName() const override { return "UIButton"; }
	virtual UI_TYPE Get_UIType() const override { return UI_TYPE::BUTTON; }

	void Set_State(UI_BUTTON_STATE eState);
	void Set_Interactable(_bool bInteractable);
	void Set_Texture(WNameID strTextureTag) { m_strTextureTag = strTextureTag; }
	void Set_TextureIndex(UI_BUTTON_STATE eState, _uint iTextureIndex);

	UI_BUTTON_STATE Get_State() const { return m_eState; }
	_bool Is_Interactable() const { return m_bInteractable; }
	WNameID Get_TextureTag() const { return m_strTextureTag; }
	_uint Get_TextureIndex(UI_BUTTON_STATE eState) const;

	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	WNameID m_strTextureTag = { INVALID_TAG };
	_uint m_iTextureIndices[ETOUI(UI_BUTTON_STATE::END)] = { INVALID_INDEX, INVALID_INDEX, INVALID_INDEX, INVALID_INDEX };
	UI_BUTTON_STATE m_eState = { UI_BUTTON_STATE::NORMAL };
	_bool m_bInteractable = { true };

public:
	static CUIButton* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

protected:
	virtual void Free();
};

NS_END