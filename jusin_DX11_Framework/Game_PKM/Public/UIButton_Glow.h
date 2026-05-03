#pragma once
#include "Game_PKM_Defines.h"
#include "UIButton.h"

NS_BEGIN(Game_PKM)

class CUIButton_Glow final : public CUIButton
{
public:
	struct GLOWBUTTON_DESC : public CUIButton::UIBUTTON_DESC
	{
		_float fGlowPulseSpeed{ 6.f };
		_float fGlowFadeSpeed{ 8.f };
	};

protected:
	CUIButton_Glow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const GLOWBUTTON_DESC& tDesc);
	CUIButton_Glow(const CUIButton_Glow& Prototype);
	virtual ~CUIButton_Glow() = default;

public:
	virtual _string Get_TypeName() const override { return "UIButton_Glow"; }

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	enum class IMAGE_SLOT : _uint
	{
		BASE = 0,
		HOVERED = 1,
		GLOW = 2,
		DISABLED = 3,
	};

	_float m_fGlowPulseSpeed{ 6.f };
	_float m_fGlowFadeSpeed{ 8.f };
	_float m_fGlowPhase{ 0.f };
	_float m_fGlowAmount{ 0.f };

private:
	virtual HRESULT Ready_Components() override;
	virtual void On_State_Changed(UI_BUTTON_STATE eOld, UI_BUTTON_STATE eNew) override;
	HRESULT Bind_GlowResources();

public:
	static CUIButton_Glow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const GLOWBUTTON_DESC& tDesc);
	virtual CGameObject* Clone(void* pArg) override;

protected:
	virtual void Free() override;
};

NS_END