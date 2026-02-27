#include "stdafx.h"
#include "..\Public\Level_Loading.h"

#include "Loader.h"
#include "Level_Logo.h"
#include "Level_Lobby.h"
#include "Level_Chara_Select.h"
#include "Level_GamePlay.h"
#include "Level_VS.h"

#include "GameInstance.h"
#include "RenderInstance.h"
#include "SpaceSun.h"
#include "UIObject.h"

#include "UI_Loading_BG.h"
#include "UI_LoadingMark.h"
#include "UI_Loading_Font.h"
#include "UI_LoadingSpaceLight.h"
#include "UI_Loading_FlyEff.h"
#include "UI_Loading_DragonBall.h"
#include "UI_Loading_CreateFlyEff.h"
#include "UI_Loading_EnergyEff.h"
#include "Loading_Camera.h"
#include "Loading_GodDragon.h"

#include "UI_Manager.h"

_bool CLevel_Loading::m_bIsLevelPrepared = false;

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Loading::Initialize(LEVELID eNextLevelID)
{
	m_iLevelIndex = LEVEL_LOADING;
	m_eNextLevelID = eNextLevelID;

	if (FAILED(Ready_Prototype_Component()))
		return E_FAIL;

	if (FAILED(Ready_Layer()))
		return E_FAIL;

	if (FAILED(Ready_Sound()))
		return E_FAIL;

	m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevelID);
	if (nullptr == m_pLoader)
		return E_FAIL;

	m_bIsLevelPrepared = TRUE;

	return S_OK;
}

HRESULT CLevel_Loading::Ready_Prototype_Component()
{
	if (m_bIsLevelPrepared)
		return S_OK;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_LoadingBackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/CmnBG/tex/E3_Title_BG01.png")))))
		return E_FAIL;

	/* For.Prototyp_Component_Texture_UI_LoadingBackGround_Mask */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_LoadingBackGround_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/CharaSelect_S3/tex/stage/stage_bg_%d.png"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_GameStartCircle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_LoadingMark"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/Middle/GameStart/GameStart%d.png"), 8))))
		return E_FAIL;


	/* For.Prototype_Component_Texture_UI_LoadingFont */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_LoadingFont"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/DB_load_00.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_LoadingSpaceLight */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_LoadingSpaceLight"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Stage_Image/sp_light.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_LoadingFlyEffect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_LoadingFlyEffect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Loading/CmnBG_Eff_Lens_5.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_LoadingBallEff */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_LoadingBallEff"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Loading/cp_dg_ball_Eff%d.png"), 9))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_LoadingBallEff */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_LoadingDragonBall"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Loading/DragonBall%d.png"), 8))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_LoadingBallEff */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_LoadingCreateFlyEff"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Loading/congra_eff01.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_LoadingEnergyEff */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_LoadingEnergyEff"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Loading/CmnBG_Eff_Lens_5.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Character_OutLine */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Character_OutLine_GodDragon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/PLN_ilm.png"), 1))))
		return E_FAIL;

	_matrix			PreTransformMatrix = XMMatrixIdentity();
	PreTransformMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f);

	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Loading_GodDragon"),
	//	CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Loading_GodDragon.bin", PreTransformMatrix))))
	//	return E_FAIL;

#pragma endregion

	/* For.Prototype_GameObject_UI_Loading */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Loading"),
		CUI_Loading_BG::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_LoadingMark */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_LoadingMark"),
		CUI_LoadingMark::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_LoadingFont */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_LoadingFont"),
		CUI_Loading_Font::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_LoadingSpaceLight */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_LoadingSpaceLight"),
		CUI_LoadingSpaceLight::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Loading_FlyEff */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Loading_FlyEff"),
		CUI_Loading_FlyEff::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Loading_DragonBall */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Loading_DragonBall"),
		CUI_Loading_DragonBall::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Loading_CreateFlyEff */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Loading_CreateFlyEff"),
		CUI_Loading_CreateFlyEff::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Loading_EnergyEff */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Loading_EnergyEff"),
		CUI_Loading_EnergyEff::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Loading_Camera */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Loading_Camera"),
		CLoading_Camera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	///* Prototype_GameObject_CharacterSlectModel */
	//if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Loading_GodDragon"),
	//	CLoading_GodDragon::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	m_bIsLevelPrepared = true;

	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer()
{
	m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOADING, TEXT("Prototype_GameObject_Loading_Camera"), TEXT("Layer_LoadingCamera"));
	m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOADING, TEXT("Prototype_GameObject_UI_Loading"), TEXT("Layer_UI_LoadingBackGround"));

	CUIObject::UI_DESC Desc = {};
	Desc.fSpeedPerSec = 50.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.f);
	for (int i = 0; i < 8; i++)
	{
		Desc.iNumUI = i;
		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOADING, TEXT("Prototype_GameObject_UI_LoadingMark"), TEXT("Layer_UI_LoadingBackGround"), &Desc);

	}

	m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOADING, TEXT("Prototype_GameObject_UI_LoadingFont"), TEXT("Layer_UI_LoadingBackGround"));

	CUIObject::UI_DESC BallDesc = {};
	for (int i = 0; i < 7; i++)
	{
		BallDesc.iNumUI = i;
		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOADING, TEXT("Prototype_GameObject_UI_Loading_DragonBall"), TEXT("Layer_UI_LoadingHole"), &BallDesc);
	}


//	m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOADING, TEXT("Prototype_GameObject_UI_LoadingSpaceLight"), TEXT("Layer_UI_LoadingBackGround"));

	m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOADING, TEXT("Prototype_GameObject_Loading_GodDragon"), TEXT("Layer_ZLoading_GodDragon"));


	//CUIObject::UI_DESC EnergyEffDesc = {};
	//EnergyEffDesc.fSpeedPerSec = 10.f;
	//m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOADING, TEXT("Prototype_GameObject_UI_Loading_EnergyEff"), TEXT("Layer_UI_LoadingBackGround"),&EnergyEffDesc);


#pragma region Light
	LIGHT_DESC			LightDesc{};
	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, 1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.0f, 1.0f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 1.f);
	
	if (FAILED(m_pRenderInstance->Add_Light(LightDesc)))
		return E_FAIL;
#pragma endregion

	return S_OK;
}

void CLevel_Loading::Update(_float fTimeDelta)
{
	if (m_iNumThreadEnd >= 7)
		m_fNextLevelTimer += fTimeDelta;

	CUI_Manager::ThreadPool_For_Loading eTheadID = CUI_Manager::Get_Instance()->Get_Thread();
	if (eTheadID != CUI_Manager::THREAD_END)
	{
		m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::LOADING_CREATE_SFX, false, 0.2f);
		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOADING, TEXT("Prototype_GameObject_UI_Loading_CreateFlyEff"), TEXT("Layer_UI_LoadingBackGround"));

		CUI_Loading_FlyEff::UI_FLYEFF_DESC FlyDesc = {};
		FlyDesc.fSpeedPerSec = 200.f;
		FlyDesc.eTheadID = eTheadID;

		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOADING, TEXT("Prototype_GameObject_UI_Loading_FlyEff"), TEXT("Layer_UI_LoadingFlyEff"), &FlyDesc);

		m_iNumThreadEnd++;
	}

	if (CUI_Manager::Get_Instance()->m_bGamePlayLoadingFinish)
		m_bNextLevel = TRUE;

	if (m_pLoader->isFinished())
	{
		CLevel* pNextLevel = { nullptr };
	
		switch (m_eNextLevelID)
		{
		case LEVEL_LOGO:
			pNextLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
			break;
	
		case LEVEL_GAMEPLAY:
			//UIObject 가 false 일때 (로딩 더 줄이기 위해) 바로 게임플레이 넘어가게 끔 하는 코드
			if (m_bNextLevel || CUI_Manager::Get_Instance()->m_bActive == FALSE)
				pNextLevel = CLevel_GamePlay::Create(m_pDevice, m_pContext);
	
			break;
		case LEVEL_LOBBY:
			pNextLevel = CLevel_Lobby::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_CHARACTER:
			pNextLevel = CLevel_Chara_Select::Create(m_pDevice, m_pContext);
			break;
	
		case LEVEL_VS:
			pNextLevel = CLevel_VS::Create(m_pDevice, m_pContext);
			break;
		}
	
		if (m_bNextLevel && m_eNextLevelID == LEVEL_GAMEPLAY)
		{
			if (FAILED(m_pGameInstance->Change_Level(pNextLevel)))
				return;
		}
	
		//UIObject 가 false 일때 (로딩 더 줄이기 위해) 바로 게임플레이 넘어가게 끔 하는 코드
		else if (m_eNextLevelID != LEVEL_GAMEPLAY || CUI_Manager::Get_Instance()->m_bActive == FALSE)
		{
			if (FAILED(m_pGameInstance->Change_Level(pNextLevel)))
				return;
		}
	}
}

HRESULT CLevel_Loading::Render(_float fTimeDelta)
{
	m_pLoader->Draw_Text();

	return S_OK;
}

HRESULT CLevel_Loading::Ready_Sound()
{
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Audio/UI/ARC_MENU_SYS_Decide_CharaSel.ogg", CSound_Manager::SOUND_KEY_NAME::LOADING_CREATE_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false, false);
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Audio/UI/ARC_MENU_SYS_Icon_On.ogg", CSound_Manager::SOUND_KEY_NAME::LOADING_BALL_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false, false);
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Audio/UI/ARC_MENU_SYS_Result_Rank_2.ogg", CSound_Manager::SOUND_KEY_NAME::LOADING_BALL_FINISH_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false, false);

	return S_OK;
}

CLevel_Loading* CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevelID)
{
	CLevel_Loading* pInstance = new CLevel_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Loading"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();

	m_pRenderInstance->BGLight_Pop_Front();
	Safe_Release(m_pLoader);

}
