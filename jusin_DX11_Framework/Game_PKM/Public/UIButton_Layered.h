#pragma once
#include "Game_PKM_Defines.h"
#include "UIButton.h"

NS_BEGIN(Engine)
class CTexture;
NS_END

NS_BEGIN(Game_PKM)

class CUIButton_Layered final : public CUIButton
{
public:
	struct LAYEREDBUTTON_DESC : public CUIButton::UIBUTTON_DESC
	{
		WNameID strLineTextureTag{ INVALID_TAG };
		WNameID strGlowTextureTag{ INVALID_TAG };

		_uint iLineTextureLevel{ INVALID_INDEX };
		_uint iGlowTextureLevel{ INVALID_INDEX };
		_uint iLineTextureIndex{ INVALID_INDEX };
		_uint iGlowTextureIndex{ INVALID_INDEX };

		_float4 vColorBG_Normal{ g_kWhite };
		_float4 vColorLine_Normal{ g_kWhite };
		_float4 vColorBG_Hover{ g_kWhite };
		_float4 vColorLine_Hover{ g_kWhite };

		_bool bUseGlow{ false };
		_bool bUseMirrorUV{ false };

		_float fGlowPulseSpeed{ 6.f };
		_float fGlowFadeSpeed{ 8.f };
	};

protected:
	CUIButton_Layered(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIButton_Layered(const CUIButton_Layered& Prototype);
	virtual ~CUIButton_Layered() = default;

public:
	virtual _string Get_TypeName() const override { return "UIButton_Layered"; }

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture* m_pLineTextureCom{ nullptr };
	CTexture* m_pGlowTextureCom{ nullptr };

	WNameID m_strTexLineTag{ INVALID_TAG };
	WNameID m_strTexGlowTag{ INVALID_TAG };

	_uint m_iLineTextureLevel{ INVALID_INDEX };
	_uint m_iGlowTextureLevel{ INVALID_INDEX };
	_uint m_iLineTextureIndex{ INVALID_INDEX };
	_uint m_iGlowTextureIndex{ INVALID_INDEX };

	_float4 m_vColDiff_Normal{ g_kWhite };
	_float4 m_vColLine_Normal{ g_kWhite };
	_float4 m_vColDiff_Hover{ g_kWhite };
	_float4 m_vColLine_Hover{ g_kWhite };
	_float4 m_vColorLine{ g_kWhite };

	_bool m_bUseGlow{ false };
	_bool m_bUseMirrorUV{ false };

	_float m_fGlowPulseSpeed{ 6.f };
	_float m_fGlowFadeSpeed{ 8.f };
	_float m_fGlowPhase{ 0.f };
	_float m_fGlowAmount{ 0.f };

	UI_BUTTON_STATE m_eLastAppliedState{ UI_BUTTON_STATE::END };

private:
	HRESULT Ready_Components_Layered();
	HRESULT Bind_LayeredResources();
	void Apply_StateColors(UI_BUTTON_STATE eState);

public:
	static CUIButton_Layered* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

protected:
	virtual void Free() override;
};

NS_END