#include "stdafx.h"
#include "..\Public\Level_Logo.h"

#include "GameInstance.h"
#include "Level_Loading.h"


CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Logo::Initialize()
{
	m_iLevelIndex = LEVEL_LOGO;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Sound()))
		return E_FAIL;

	return S_OK;
}

void CLevel_Logo::Update(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_SPACE))
	{
		m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::LOGO_START, false, 0.2f);

		if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_LOBBY))))
			return;
	}
}

HRESULT CLevel_Logo::Render(_float fTimeDelta)
{
	SetWindowText(g_hWnd, TEXT("로고레벨"));

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	/* For.BackGround */
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOGO, TEXT("Prototype_GameObject_BackGround"), strLayerTag)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOGO, TEXT("Prototype_GameObject_Logo"), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Sound()
{
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Audio/BGM/030_sto_sento1.ogg", CSound_Manager::SOUND_KEY_NAME::LOGO_BGM, CSound_Manager::SOUND_CATEGORY::BGM, true);
	m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::LOGO_BGM, true, 0.2f);

	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Audio/UI/ARC_MENU_SYS_MapStart.ogg", CSound_Manager::SOUND_KEY_NAME::LOGO_START, CSound_Manager::SOUND_CATEGORY::SFX, false);

	return S_OK;
}

CLevel_Logo* CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Logo* pInstance = new CLevel_Logo(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Logo"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Logo::Free()
{
	m_pGameInstance->Stop_Sound(CSound_Manager::SOUND_KEY_NAME::LOGO_BGM);

	__super::Free();
}
