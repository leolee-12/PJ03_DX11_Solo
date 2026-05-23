#pragma once
#include "Game_PKM_Defines.h"
#include "UIButton.h"

NS_BEGIN(Game_PKM)

class CUIButton_Entry final : public CUIButton
{
public:
	struct ENTRYBUTTON_DESC : public CUIButton::UIBUTTON_DESC
	{
		_uint iBaseTextureIndex{ 0 };
		_uint iLineTextureIndex{ 1 };
		_uint iGlowTextureIndex{ 2 };
		_uint iDiffuseTextureIndex{ 3 };

		_float4 vColDiff{ g_kWhite };
		_float4 vColLine{ g_kWhite };

		_uint iShaderPass{ 0u };
	};

protected:
	CUIButton_Entry(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const ENTRYBUTTON_DESC&
		tDesc);
	CUIButton_Entry(const CUIButton_Entry& Prototype);
	virtual ~CUIButton_Entry() = default;

public:
	virtual _string Get_TypeName() const override { return "UIButton_Entry"; }

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Set_Selected(_bool bSelected) { m_bSelected = bSelected; }
	_bool Is_Selected() const { return m_bSelected; }
	void Reset_SelectedVisual();

private:
	_uint m_iBaseTextureIndex{ 0 };
	_uint m_iLineTextureIndex{ 1 };
	_uint m_iGlowTextureIndex{ 2 };
	_uint m_iDiffuseTextureIndex{ 3 };

	_float4 m_vColDiff{ g_kWhite };
	_float4 m_vColLine{ g_kWhite };

	_bool m_bSelected{ false };
	_float m_fGlowPhase{ 0.f };
	_float m_fGlowAmount{ 0.f };
	_uint m_iShaderPass{ 0u };

private:
	virtual HRESULT Ready_Components() override;
	HRESULT Bind_EntryResources();

public:
	static CUIButton_Entry* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const
		ENTRYBUTTON_DESC& tDesc);
	virtual CGameObject* Clone(void* pArg) override;

protected:
	virtual void Free() override;
};

NS_END