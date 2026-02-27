#include "stdafx.h"

#include "UI_Opt_Sound.h"
#include "RenderInstance.h"

CUI_Opt_Sound::CUI_Opt_Sound(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_Option{ pDevice , pContext }
{

}

CUI_Opt_Sound::CUI_Opt_Sound(const CUI_Opt_Sound& Prototype)
	:CUI_Option{ Prototype }
{

}

HRESULT CUI_Opt_Sound::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Opt_Sound::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CUI_Opt_Sound::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_Opt_Sound::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if(m_bKeyInput)
		m_fInputDelay += fTimeDelta;

	if (m_bKeyInput && m_fInputDelay >= 0.15f)
	{
		m_bKeyInput = FALSE;
		m_fInputDelay = 0.f;
	}
	MenuChange();

}

void CUI_Opt_Sound::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CUI_Opt_Sound::Render(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Opt_Sound::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Opt_Sound::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUI_Opt_Sound::MenuChange()
{

	if(m_pGameInstance->Key_Down(DIK_UP))
	{
		if(!m_bKeyInput)
			m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::UI_MENU_CURSOR, false, 1.f);

		m_eMenuValue = (SOUND_MENU)(m_eMenuValue - 1);

		if (m_eMenuValue < 0)
			m_eMenuValue = EXIT;


		m_bKeyInput = TRUE;
	}

	if (m_pGameInstance->Key_Down(DIK_DOWN))
	{
		if (!m_bKeyInput)
			m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::UI_MENU_CURSOR, false, 1.f);

		m_eMenuValue = (SOUND_MENU)(m_eMenuValue + 1);

		if (m_eMenuValue >= MENU_END)
			m_eMenuValue = BGM;

		m_bKeyInput = TRUE;
	}

	if (m_eMenuValue == EXIT && m_pGameInstance->Key_Down(DIK_RETURN))
	{
		m_bIsActive = FALSE;
		m_pUI_Manager->m_bOnOption = FALSE;
	}


}

void CUI_Opt_Sound::Free()
{

	__super::Free();
}
