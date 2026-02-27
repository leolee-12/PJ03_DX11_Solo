#include "stdafx.h"
#include "..\Public\Level_VS.h"

#include "GameInstance.h"
#include "Level_Loading.h"
#include "UI_Define.h"
#include "UIObject.h"
#include "GameObject.h"
#include"UI_Manager.h"
#include "BattleInterface.h"

#include "RenderInstance.h" 

#include <time.h>
#include <thread>

CLevel_VS::CLevel_VS(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_VS::Initialize()
{
	m_iLevelIndex = LEVEL_VS;

	m_pRenderInstance->Clear_Light();

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Sound()))
		return E_FAIL;

	return S_OK;
}

void CLevel_VS::Update(_float fTimeDelta)
{
	if (fTimeDelta >= 0.1f)
		fTimeDelta = 0.f;

	m_fNextLevelTimer += fTimeDelta;

	if (m_bBlack && m_fNextLevelTimer > 4.5f)
	{
		m_bBlack = false;
		CRenderInstance::Get_Instance()->Switch_AllBlackOut();
	}

	if (m_fNextLevelTimer >= 5.f)
	{
		m_fNextLevelTimer = 0.f;

		if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY))))
			return;
	}
	m_fUILightCreateTimer += fTimeDelta;
	
	if (m_fUILightCreateTimer >= 1.f)
	{
		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_VS, TEXT("Prototype_GameObject_VS_DynamicLight"), TEXT("Layer_ABackGround"));
		m_fUILightCreateTimer = 0.f;
	}
}

HRESULT CLevel_VS::Render(_float fTimeDelta)
{
	SetWindowText(g_hWnd, TEXT("로고레벨"));

	return S_OK;
}

HRESULT CLevel_VS::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_VS, TEXT("Prototype_GameObject_VS_BG"), TEXT("Layer_ABackGround"))))
		return E_FAIL;

	CUIObject::UI_DESC BarDesc = {};
	for (size_t i = 0; i < 2; ++i)
	{
		BarDesc.iNumUI = i;
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_VS, TEXT("Prototype_GameObject_VS_Bar"), strLayerTag,&BarDesc)))
			return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_VS, TEXT("Prototype_GameObject_VS_StaticLight"), strLayerTag)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_VS, TEXT("Prototype_GameObject_VS_DynamicLight"), strLayerTag)))
		return E_FAIL;

	for (int i = 0; i < 4; i++)
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_VS, TEXT("Prototype_GameObject_VS_Bolt"), strLayerTag)))
			return E_FAIL;
	}

	CUIObject::UI_DESC MarkDesc = {};
	for (int i = 0; i < 2; i++)
	{
		MarkDesc.iNumUI = i;
		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_VS, TEXT("Prototype_GameObject_VS_Mark"), strLayerTag, &MarkDesc);
	}

	m_pGameInstance->Add_GameObject_ToLayer(LEVEL_VS, TEXT("Prototype_GameObject_VS_MarkEff"), strLayerTag, &MarkDesc);

	CUIObject::UI_DESC CharaDesc = {};
	for (size_t i = 0; i < 2 ; i++)
	{
		CharaDesc.iNumUI = i;
		for (size_t k = 0; k < 2; k++)
		{
			CharaDesc.eLRPos = static_cast<CUIObject::UI_LRPOS>(k);
			CharaDesc.iTotalNum = i * 2 + k;

			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_VS, TEXT("Prototype_GameObject_VS_CharaPanel"), strLayerTag, &CharaDesc)))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_VS, TEXT("Prototype_GameObject_VS_NameOutLine"), strLayerTag, &CharaDesc)))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_VS, TEXT("Prototype_GameObject_VS_Name"), strLayerTag, &CharaDesc)))
				return E_FAIL;
		}
	}

	CUIObject::UI_DESC TeamDesc = {};

	for (size_t i = 0; i < 2; i++)
	{
		TeamDesc.eLRPos = static_cast<CUIObject::UI_LRPOS>(i);
		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_VS, TEXT("Prototype_GameObject_VS_TeamPanel"), strLayerTag, &TeamDesc);

	}

	for (size_t i = 0; i < 2; i++)
	{
		TeamDesc.eLRPos = static_cast<CUIObject::UI_LRPOS>(i);
		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_VS, TEXT("Prototype_GameObject_VS_Ball"), strLayerTag, &TeamDesc);
	}


	return S_OK;
}

HRESULT CLevel_VS::Ready_Sound()
{
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Audio/Narration/JPN/NA_0401_Versus_GetReadyForBattle.ogg", CSound_Manager::SOUND_KEY_NAME::NARRATION_VS, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::NARRATION_VS, false, 1.f);

	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Audio/UI/ARC_MENU_SYS_VSLoading.ogg", CSound_Manager::SOUND_KEY_NAME::NARRATION_VS_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);
	m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::NARRATION_VS_SFX, false, 0.2f);
	
	return S_OK;
}

CLevel_VS* CLevel_VS::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_VS* pInstance = new CLevel_VS(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_VS"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_VS::Free()
{
	__super::Free();
}
