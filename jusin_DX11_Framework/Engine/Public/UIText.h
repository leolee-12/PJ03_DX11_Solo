#pragma once
#include "UIObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CUIText : public CUIObject
{
public:
	struct UITEXT_DESC : public CUIObject::UIOBJECT_DESC
	{
		_wstring strText = {};
		WNameID strFontTag = { INVALID_TAG };
		_float4 vColor = { g_kWhite };
		UI_TEXT_ALIGN eAlign = UI_TEXT_ALIGN::LEFT;
	};

protected:
	CUIText(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIText(const CUIText& Prototype);
	virtual ~CUIText() = default;

public:
	virtual _string Get_TypeName() const override { return "UIText"; }
	virtual UI_TYPE Get_UIType() const override { return UI_TYPE::TEXT; }

	void Set_Text(const _wstring& strText) { m_strText = strText; }
	void Set_Font(WNameID strFontTag) { m_strFontTag = strFontTag; }
	void Set_Color(const _float4& vColor) { m_vColor = vColor; }
	void Set_Align(UI_TEXT_ALIGN eAlign) { m_eAlign = eAlign; }

	const _wstring& Get_Text() const { return m_strText; }
	WNameID Get_FontTag() const { return m_strFontTag; }
	const _float4& Get_Color() const { return m_vColor; }
	UI_TEXT_ALIGN Get_Align() const { return m_eAlign; }

	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();
	virtual _bool Can_Apply_Tween_Target(UI_TWEEN_TARGET eTarget) const override;
	virtual HRESULT Apply_Tween_Target(UI_TWEEN_TARGET eTarget, _float fValue) override;

private:
	_wstring m_strText = {};
	WNameID m_strFontTag = { INVALID_TAG };
	_float4 m_vColor = { g_kWhite };
	UI_TEXT_ALIGN m_eAlign = { UI_TEXT_ALIGN::LEFT };

public:
	static CUIText* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

protected:
	virtual void Free();
};

NS_END
