#pragma once
#include "Game_PKM_Defines.h"
#include "UIButton.h"

NS_BEGIN(Engine)
class CTexture;
NS_END

NS_BEGIN(Game_PKM)

class CUIButton_Glow final : public CUIButton
{
public:
	struct GLOWBUTTON_DESC : public CUIButton::UIBUTTON_DESC
	{
		WNameID strGlowTextureTag{ INVALID_TAG };
		_uint iGlowTextureLevel{ INVALID_INDEX };
		_uint iGlowTextureIndex{ INVALID_INDEX };

		_float fGlowPulseSpeed{ 6.f };
		_float fGlowFadeSpeed{ 8.f };
	};

protected:
	CUIButton_Glow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIButton_Glow(const CUIButton_Glow& Prototype);
	virtual ~CUIButton_Glow() = default;

public:
	virtual _string Get_TypeName() const override { return "UIButton_Glow"; }

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	Engine::CTexture* m_pGlowTextureCom{ nullptr };

	WNameID m_strTexGlowTag{ INVALID_TAG };
	_uint m_iGlowTextureLevel{ INVALID_INDEX };
	_uint m_iGlowTextureIndex{ INVALID_INDEX };

	_float m_fGlowPulseSpeed{ 6.f };
	_float m_fGlowFadeSpeed{ 8.f };
	_float m_fGlowPhase{ 0.f };
	_float m_fGlowAmount{ 0.f };

private:
	HRESULT Ready_Components_Glow();
	HRESULT Bind_GlowResources(_uint iBaseTextureIndex);

public:
	static CUIButton_Glow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual Engine::CGameObject* Clone(void* pArg) override;

protected:
	virtual void Free() override;
};

NS_END