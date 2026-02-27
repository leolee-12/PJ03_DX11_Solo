#include "stdafx.h"
#include "..\Public\Level_Lobby.h"

#include "GameInstance.h"
#include "Level_Loading.h"
#include "RenderInstance.h"
#include "UI_Manager.h"

#include "GameObject.h"

#include "Lobby_DisplayBoard.h"
#include "Lobby_Cloud.h"
#include "Lobby_Flag.h"
#include "Lobby_Staff.h"

#include "UI_Lobby_Key_Enter.h"

CLevel_Lobby::CLevel_Lobby(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Lobby::Initialize()
{
	m_iLevelIndex = LEVEL_LOBBY;

#pragma region 사본 객체 만들기

	//텍스트 박스
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOBBY, TEXT("Prototype_GameObject_Lobby_TextBox"), TEXT("Layer_Lobby_TextBox"))))
		return E_FAIL;

	//텍스트 캐릭터 아이콘
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOBBY, TEXT("Prototype_GameObject_Lobby_TextCharaIcon"), TEXT("Layer_Lobby_TextCharaIcon"))))
		return E_FAIL;

	//텍스트 커서 
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOBBY, TEXT("Prototype_GameObject_Lobby_TextCursor"), TEXT("Layer_Lobby_TextCursor"))))
		return E_FAIL;





	//로비맵
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOBBY, TEXT("Prototype_GameObject_Lobby_Center_Map"), TEXT("Layer_Lobby_Center_Map"))))
		return E_FAIL;

	//로비_배틀_빌딩
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOBBY, TEXT("Prototype_GameObject_Lobby_Battle_Building"), TEXT("Layer_Lobby_Battle_Building"))))
		return E_FAIL;

	//로컬_배틀_빌딩
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOBBY, TEXT("Prototype_GameObject_Local_Battle_Building"), TEXT("Layer_Local_Battle_Building"))))
		return E_FAIL;

	//로비_스토리_빌딩
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOBBY, TEXT("Prototype_GameObject_Lobby_Story_Mode_Building"), TEXT("Layer_Lobby_Story_Mode_Building"))))
		return E_FAIL;
	
	//아케이드_스토리_빌딩
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOBBY, TEXT("Prototype_GameObject_Lobby_Arcade_Building"), TEXT("Layer_Lobby_Arcade_Building"))))
		return E_FAIL;

	_float3 Desc_Position{};
	Desc_Position = _float3(-34.151f, 0.f, -14.184f);

	//파라솔
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOBBY, TEXT("Prototype_GameObject_Lobby_Parasol"), TEXT("Layer_Lobby_Parasol"), &Desc_Position)))
		return E_FAIL;

	Desc_Position = _float3(-14.803, 0.f, -32.624f);

	//파라솔
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOBBY, TEXT("Prototype_GameObject_Lobby_Parasol"), TEXT("Layer_Lobby_Parasol"), &Desc_Position)))
		return E_FAIL;

	Desc_Position = _float3(13.638f, 0.f, -32.924f);

	//파라솔
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOBBY, TEXT("Prototype_GameObject_Lobby_Parasol"), TEXT("Layer_Lobby_Parasol"), &Desc_Position)))
		return E_FAIL;

	Desc_Position = _float3(33.006f, 0.f, -14.184f);

	//파라솔
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOBBY, TEXT("Prototype_GameObject_Lobby_Parasol"), TEXT("Layer_Lobby_Parasol"), &Desc_Position)))
		return E_FAIL;

	//로비 고쿠
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOBBY, TEXT("Prototype_GameObject_Lobby_Goku"), TEXT("Layer_Lobby_Goku"))))
		return E_FAIL;

	//로비 NPC 프리저
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOBBY, TEXT("Prototype_GameObject_Lobby_Frieza"), TEXT("Layer_Lobby_Frieza"))))
		return E_FAIL;

	//로비 NPC 크리링
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOBBY, TEXT("Prototype_GameObject_Lobby_Krillin"), TEXT("Layer_Lobby_Krillin"))))
		return E_FAIL;

	//로비 하늘
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOBBY, TEXT("Prototype_GameObject_Lobby_Sky"), TEXT("Layer_Lobby_Sky"))))
		return E_FAIL;

	//로비 하늘바다
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOBBY, TEXT("Prototype_GameObject_Lobby_Sky_Of_Sea"), TEXT("Layer_Lobby_Sky_Of_Sea"))))
		return E_FAIL;

	//로비 경기장
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOBBY, TEXT("Prototype_GameObject_Lobby_Lobby_Wring"), TEXT("Layer_Lobby_Wring"))))
		return E_FAIL;

	//로비 자동차
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOBBY, TEXT("Prototype_GameObject_Lobby_Lobby_Car"), TEXT("Layer_Lobby_Car"))))
		return E_FAIL;

	//로비 깃발
	CLobby_Flag::FLAG_DESC tFlagDesc = {};
	for (size_t i = 0; i < 4; ++i)
	{
		tFlagDesc.iNumObject = i;
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOBBY, TEXT("Prototype_GameObject_Lobby_Lobby_Flag"), TEXT("Layer_Lobby_Flag"),&tFlagDesc)))
			return E_FAIL;
	}
	//로비 랭킹
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOBBY, TEXT("Prototype_GameObject_Lobby_Lobby_Ranking"), TEXT("Layer_Lobby_Ranking"))))
		return E_FAIL;

	//로비 스태프

	CLobby_Staff::STAFF_DESC StaffDesc = {};
	for (int i = 0; i < 2; i++)
	{
		StaffDesc.iNumObject = i;
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOBBY, TEXT("Prototype_GameObject_Lobby_Lobby_Staff"), TEXT("Layer_Lobby_Staff"),&StaffDesc)))
			return E_FAIL;
	}


	//Enter UI
	CUI_Lobby_Key_Enter::ENTER_DESC EnterDesc = {};
	EnterDesc.pTransform =dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_LOBBY, TEXT("Layer_Lobby_Frieza"),TEXT("Com_Transform") ,0));
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOBBY, TEXT("Prototype_GameObject_Lobby_Key_Enter"), TEXT("Layer_Lobby_Key_Enter"),&EnterDesc)))
		return E_FAIL;

	EnterDesc.pTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_LOBBY, TEXT("Layer_Lobby_Staff"), TEXT("Com_Transform"), 0));
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOBBY, TEXT("Prototype_GameObject_Lobby_Key_Enter"), TEXT("Layer_Lobby_Key_Enter"),&EnterDesc)))
		return E_FAIL;

	EnterDesc.pTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_LOBBY, TEXT("Layer_Lobby_Staff"), TEXT("Com_Transform"), 1));
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOBBY, TEXT("Prototype_GameObject_Lobby_Key_Enter"), TEXT("Layer_Lobby_Key_Enter"),&EnterDesc)))
		return E_FAIL;

	//로비 파티
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOBBY, TEXT("Prototype_GameObject_Lobby_Lobby_Party"), TEXT("Layer_Lobby_Party"))))
	//	return E_FAIL;

	//로비 구름
	//CLobby_Cloud::CLOUD_DESC tCloudDesc = {};
	//for (size_t i = 0; i < 5; ++i)
	//{
	//	tCloudDesc.iNumObject = i;
	//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOBBY, TEXT("Prototype_GameObject_Lobby_Cloud"), TEXT("Layer_Lobby_Cloud"))))
	//		return E_FAIL;
	//}

	//로비 카메라
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOBBY, TEXT("Prototype_GameObject_Main_Camera_Lobby"), TEXT("Layer_Main_Camera_Lobby"))))
		return E_FAIL;

#pragma endregion

#pragma region 사본 UI 객체 만들기
	//Win
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOBBY, TEXT("Prototype_GameObject_UI_WIn_Font"), TEXT("Layer_Lobby_Win"))))
	//	return E_FAIL;

	//원형 전광판
	CLobby_DisplayBoard::DISPLAY_DESC tDisplayDesc = {};

	_uint iNumFontID = CLobby_DisplayBoard::FONT_END;
	_uint iNumInOutID = CLobby_DisplayBoard::INOUT_END;

	for (size_t i = 0; i < iNumFontID; i++)
	{
		tDisplayDesc.eFontID = static_cast<CLobby_DisplayBoard::LOBBY_FONTID>(i);

		for (size_t k  = 0; k < iNumInOutID; k++)
		{
			tDisplayDesc.eInOutID = static_cast<CLobby_DisplayBoard::LOBBY_INOUT>(k);
			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOBBY, TEXT("Prototype_GameObject_Lobby_DisplayBoard"), TEXT("Layer_Lobby_ZDisplayBoard"), &tDisplayDesc)))
				return E_FAIL;
		}
	}
#pragma endregion

#pragma region Light
	LIGHT_DESC			LightDesc{};
	
	ZeroMemory(&LightDesc, sizeof(LIGHT_DESC));
	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, 1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.0f, 1.0f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 1.f);
	
	if (FAILED(m_pRenderInstance->Add_Light(LightDesc)))
		return E_FAIL;



	//ZeroMemory(&LightDesc, sizeof(LIGHT_DESC));
	//LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	//LightDesc.vPosition = _float4(20.f, 50.f, 10.f, 1.f);
	//LightDesc.fRange = 100000.f;
	//LightDesc.vDiffuse = _float4(1.0f, 1.f, 0.f, 1.f);
	//LightDesc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);
	//LightDesc.vSpecular = _float4(0.f, 1.f, 0.f, 1.f);
	//
	//if (FAILED(m_pRenderInstance->Add_Light(LightDesc)))
	//	return E_FAIL;
#pragma endregion



	//사운드 준비
	if (FAILED(Ready_Sound()))
		return E_FAIL;

	return S_OK;
}

void CLevel_Lobby::Update(_float fTimeDelta)
{
	if (m_bChangeLevel)
	{
		if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, CUI_Manager::Get_Instance()->m_eLevelID))))
			return;
	}

}

HRESULT CLevel_Lobby::Render(_float fTimeDelta)
{
	SetWindowText(g_hWnd, TEXT("로비레벨"));

	return S_OK;
}

void CLevel_Lobby::Change_Level()
{
	m_bChangeLevel = true;
}

HRESULT CLevel_Lobby::Ready_Sound()
{
	//배경음
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Audio/BGSE/Amb/ARC_BG_Amb_LobbyWind_LP.ogg", CSound_Manager::SOUND_KEY_NAME::LOBBY_BGM, CSound_Manager::SOUND_CATEGORY::BGM, true);
	m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::LOBBY_BGM, true, 1.f);

	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Audio/UI/ARC_LOB_AVT_Foot_Default.ogg", CSound_Manager::SOUND_KEY_NAME::LOBBY_FOOT_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	return S_OK;
}

CLevel_Lobby* CLevel_Lobby::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Lobby* pInstance = new CLevel_Lobby(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Lobby"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Lobby::Free()
{
	m_pGameInstance->Stop_Sound(CSound_Manager::SOUND_KEY_NAME::LOBBY_BGM);
	
	__super::Free();
}
