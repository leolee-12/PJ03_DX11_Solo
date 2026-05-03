#pragma once
#include "Game_PKM_Defines.h"
#include "UIButton.h"

NS_BEGIN(Game_PKM)

class CUIButton_Layered final : public CUIButton
{
public:
	struct LAYEREDBUTTON_DESC : public CUIButton::UIBUTTON_DESC
	{
		_float4 vColBase_Normal{ g_kWhite };
		_float4 vColLine_Normal{ g_kWhite };
		_float4 vColBase_Hover{ g_kWhite };
		_float4 vColLine_Hover{ g_kWhite };

		_bool bUseGlow{ false };
		_bool bUseMirrorUV{ false };

		_float fGlowPulseSpeed{ 6.f };
		_float fGlowFadeSpeed{ 8.f };
	};

protected:
	CUIButton_Layered(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LAYEREDBUTTON_DESC& tDesc);
	CUIButton_Layered(const CUIButton_Layered& Prototype);
	virtual ~CUIButton_Layered() = default;

public:
	virtual _string Get_TypeName() const override { return "UIButton_Layered"; }

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	enum class IMAGE_SLOT : _uint
	{
		BASE = 0,
		LINE = 1,
		GLOW = 2,
	};

	_float4 m_vColBase_Normal{ g_kWhite };
	_float4 m_vColLine_Normal{ g_kWhite };
	_float4 m_vColBase_Hover{ g_kWhite };
	_float4 m_vColLine_Hover{ g_kWhite };
	_float4 m_vColorLine{ g_kWhite };

	_bool m_bUseGlow{ false };
	_bool m_bUseMirrorUV{ false };

	_float m_fGlowPulseSpeed{ 6.f };
	_float m_fGlowFadeSpeed{ 8.f };
	_float m_fGlowPhase{ 0.f };
	_float m_fGlowAmount{ 0.f };

private:
	virtual HRESULT Ready_Components() override;
	virtual void On_State_Changed(UI_BUTTON_STATE eOld, UI_BUTTON_STATE eNew) override;
	HRESULT Bind_LayeredResources();
	void Apply_StateColors(UI_BUTTON_STATE eState);

public:
	static CUIButton_Layered* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LAYEREDBUTTON_DESC& tDesc);
	virtual CGameObject* Clone(void* pArg) override;

protected:
	virtual void Free() override;
};

NS_END