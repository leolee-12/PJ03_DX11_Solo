#include "stdafx.h"
#include "..\Public\Level_Chara_Select.h"

#include "GameInstance.h"
#include "Level_Loading.h"
#include "UI_Define.h"
#include "UIObject.h"
#include "GameObject.h"
#include "UI_Manager.h"

#include "RenderInstance.h" 
#include "BattleInterface.h"

CLevel_Chara_Select::CLevel_Chara_Select(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Chara_Select::Initialize()
{
	m_iLevelIndex = LEVEL_CHARACTER;

	if(FAILED(Ready_Camera(TEXT("Layer_Camera"))))	
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	//if (FAILED(Ready_Character(TEXT("Layer_Character"))))
	//	return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;
	
	if (FAILED(Ready_Sound()))
		return E_FAIL;



	CBattleInterface_Manager::Get_Instance()->Set_b1VS1(false);

	return S_OK;
}

void CLevel_Chara_Select::Update(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_SPACE))
	{
		if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_VS))))
			return;
	}


}

HRESULT CLevel_Chara_Select::Render(_float fTimeDelta)
{
	SetWindowText(g_hWnd, TEXT("로고레벨"));

	return S_OK;
}

HRESULT CLevel_Chara_Select::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHARACTER, TEXT("Prototype_GameObject_CharaSelectBG"), strLayerTag)))
		return E_FAIL;
	
	CUIObject::UI_DESC FudeDesc = {};
	for (int i = 0; i < 2; ++i)
	{
		FudeDesc.iNumUI = i;
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHARACTER, TEXT("Prototype_GameObject_CharacterSelectFude"), strLayerTag, &FudeDesc)))
			return E_FAIL;
	}
	
	CUIObject::UI_DESC ArrowDesc = {};
	ArrowDesc.fSpeedPerSec = 300.f;
	
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHARACTER, TEXT("Prototype_GameObject_CharaSelectArrow"), TEXT("Layer_MarkArrow"), &ArrowDesc)))
		return E_FAIL;

	//ArrowDesc.fSpeedPerSec = 10.f;
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHARACTER, TEXT("Prototype_GameObject_UI_Loading_EnergyEff"), TEXT("Layer_MarkArrowEff"),&ArrowDesc)))
	//	return E_FAIL;
	
	CUIObject::UI_DESC SelectIconDesc = {};
	for (size_t i = 0; i < CUI_Define::PAWN_END; i++)
	{
		SelectIconDesc.iNumUI = i;
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHARACTER, TEXT("Prototype_GameObject_CharaSelectIcon"), strLayerTag, &SelectIconDesc)))
			return E_FAIL;
	}
	
	CUIObject::UI_DESC SelectLightDesc = {};
	for (size_t i = 0; i < 2; i++)
	{
		SelectLightDesc.iNumUI = i;
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHARACTER, TEXT("Prototype_GameObject_CharacterSelectLight"), strLayerTag, &SelectLightDesc)))
			return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHARACTER, TEXT("Prototype_GameObject_CharacterSelectMark"), strLayerTag)))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHARACTER, TEXT("Prototype_GameObject_CharacterSelectFont"), strLayerTag)))
		return E_FAIL;
	

	return S_OK;
}

HRESULT CLevel_Chara_Select::Ready_Camera(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHARACTER, TEXT("Prototype_GameObject_UI_CharaSelect_Camera"), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Chara_Select::Ready_Character(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHARACTER, TEXT("Prototype_GameObject_CharaSelect_Model"), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Chara_Select::Ready_Sound()
{
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Audio/Narration/JPN/NA_0002_CharacterSelect_Start2.ogg", CSound_Manager::SOUND_KEY_NAME::NARRATION_CHARASELECT, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::NARRATION_CHARASELECT, false, 1.f);

	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Audio/Narration/JPN/NA_0122_Name_GKB.ogg", CSound_Manager::SOUND_KEY_NAME::NARRATION_CHARASELECT_GOKU, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Audio/Narration/JPN/NA_0126_Name_TON.ogg", CSound_Manager::SOUND_KEY_NAME::NARRATION_CHARASELECT_21, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Audio/Narration/JPN/NA_0113_Name_BUN.ogg", CSound_Manager::SOUND_KEY_NAME::NARRATION_CHARASELECT_BUU, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Audio/Narration/JPN/NA_0121_Name_HTN.ogg", CSound_Manager::SOUND_KEY_NAME::NARRATION_CHARASELECT_HIT, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Audio/Narration/JPN/NA_0105_Name_FRN.ogg", CSound_Manager::SOUND_KEY_NAME::NARRATION_CHARASELECT_FRN, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Audio/BGM/012_sto_opening.ogg", CSound_Manager::SOUND_KEY_NAME::CHARASELECT_BGM, CSound_Manager::SOUND_CATEGORY::BGM, true);
	m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::CHARASELECT_BGM, true, 0.1f);

	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Audio/UI/ARC_MENU_SYS_Cursor.ogg", CSound_Manager::SOUND_KEY_NAME::UI_MENU_CURSOR, CSound_Manager::SOUND_CATEGORY::SFX, false);

	return S_OK;
}

HRESULT CLevel_Chara_Select::Ready_Lights()
{
	LIGHT_DESC			LightDesc{};
	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, 1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.0f, 1.0f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 1.f);

	if (FAILED(m_pRenderInstance->Add_Light(LightDesc)))
		return E_FAIL;

	return S_OK;
}

CLevel_Chara_Select* CLevel_Chara_Select::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Chara_Select* pInstance = new CLevel_Chara_Select(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Chara_Select"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Chara_Select::Free()
{
	m_pGameInstance->Stop_Sound(CSound_Manager::SOUND_KEY_NAME::LOGO_BGM);

	
	__super::Free();
}
