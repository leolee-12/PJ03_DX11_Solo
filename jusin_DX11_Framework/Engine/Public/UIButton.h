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
		WNameID strShaderTag{ INVALID_TAG };
		WNameID strVIBufferTag{ INVALID_TAG };

		_uint iTextureLevel{ INVALID_INDEX };
		_uint iShaderLevel{ INVALID_INDEX };
		_uint iVIBufferLevel{ INVALID_INDEX };

		_bool bInteractable{ true };
		_float4 vColor{ g_kWhite };
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

	UI_BUTTON_STATE Get_State() const { return m_eState; }
	_bool Is_Interactable() const { return m_bInteractable; }
	WNameID Get_TextureTag() const { return m_strTextureTag; }

	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

	virtual _bool Can_Apply_Tween_Target(UI_TWEEN_TARGET eTarget) const;
	virtual HRESULT Apply_Tween_Target(UI_TWEEN_TARGET eTarget, _float fValue);

protected:
	class CShader* m_pShaderCom{ nullptr };
	class CTexture* m_pTextureCom{ nullptr };
	class CVIBuffer_Rect* m_pVIBufferCom{ nullptr };

	WNameID m_strTextureTag = { INVALID_TAG };
	WNameID m_strShaderTag = { INVALID_TAG };
	WNameID m_strVIBufferTag = { INVALID_TAG };

	_uint m_iTextureLevel = { INVALID_INDEX };
	_uint m_iShaderLevel = { INVALID_INDEX };
	_uint m_iVIBufferLevel = { INVALID_INDEX };

	UI_BUTTON_STATE m_eState = { UI_BUTTON_STATE::NORMAL };
	_bool m_bInteractable = { true };
	_float4 m_vColor = { g_kWhite };

protected:
	virtual HRESULT Ready_Components();
	virtual void On_State_Changed(UI_BUTTON_STATE eOld, UI_BUTTON_STATE eNew) {}
	HRESULT Bind_BaseMatrices(class CShader* pShader);

public:
	static CUIButton* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

protected:
	virtual void Free();
};

NS_END