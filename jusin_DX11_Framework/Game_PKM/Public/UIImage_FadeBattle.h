#pragma once
#include "Game_PKM_Defines.h"

#include "UIImage.h"

NS_BEGIN(Engine)
class CTexture;
NS_END

NS_BEGIN(Game_PKM)

class CUIImage_FadeBattle final : public CUIImage
{
protected:
	CUIImage_FadeBattle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIImage_FadeBattle(const CUIImage_FadeBattle& Prototype);
	virtual ~CUIImage_FadeBattle() = default;

public:
	virtual _string Get_TypeName() const override { return "UIImage_FadeBattle"; }

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture* m_pColorTextureCom = { nullptr };
	_float m_fFadeTime = { 0.f };

private:
	HRESULT Ready_FadeBattleComponents();
	HRESULT Bind_FadeBattleResources();

public:
	static CUIImage_FadeBattle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

protected:
	virtual void Free() override;
};

NS_END