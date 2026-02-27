#include "stdafx.h"
#include "..\Public\Level_GamePlay.h"

#include "Main_Camera.h"
#include "GameInstance.h"
#include "RenderInstance.h"
#include "Effect_Manager.h"
#include "Imgui_Manager.h"
#include "QTE_Manager.h"
#include "UI_ComboNumber.h"

#include "UIObject.h"
#include "UI_GameState.h"

#include "Map_Manager.h"
#include "Character.h"
#include "Sound_Manager.h"
#include "BattleInterface.h"
#include "Opening_Kririn.h"
#include "Particle_Manager.h"
#include "SubTitle_Manager.h"

#include "Level_Loading.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
	, m_pUI_Manager{ CUI_Manager::Get_Instance() }
	, m_pIMGUI_Manager{ CImgui_Manager::Get_Instance() }
	, m_pQTE_Manager{ CQTE_Manager::Get_Instance() }
	, m_pMap_Manager{ CMap_Manager::Get_Instance() }
	, m_pParticle_Manager{ CParticle_Manager::Get_Instance() }
	, m_pSubTitle_Manager{ CSubTitle_Manager::Get_Instance() }
{
	for(int i=0; i<1000000; i++)
		Safe_AddRef(m_pDevice);


	Safe_AddRef(m_pParticle_Manager);
	Safe_AddRef(m_pSubTitle_Manager);
	Safe_AddRef(m_pMap_Manager);
	Safe_AddRef(m_pQTE_Manager);
	Safe_AddRef(m_pIMGUI_Manager);
	Safe_AddRef(m_pUI_Manager);
	Safe_AddRef(m_pEffect_Manager);
}

HRESULT CLevel_GamePlay::Initialize()
{
	m_pRenderInstance->Clear_Light();

	m_iLevelIndex = LEVEL_GAMEPLAY;
	m_pMap_Manager->Initialize(m_pDevice, m_pContext);
	Create_Effect_Manager();
	Create_QTE_Manager();
	Create_Particle_Manager();
	Create_SubTitle_Manager();

#pragma region 이펙트 세팅
	Loading_For_Effect();
#pragma endregion

#pragma region 사운드 로드
	if (FAILED(Ready_Sound()))
		return E_FAIL;
#pragma endregion

#pragma region 맵 사본 객체

	if (FAILED(Ready_Volcano()))
		return E_FAIL;

	if (FAILED(Ready_Space()))
		return E_FAIL;

	m_pMap_Manager->Map_Change(CMap_Manager::MAP_VOLCANO);
#pragma endregion

#pragma region 캐릭터 사본 객체

	//반드시 1P,2P 순서로 생성해야하는가?

	//Prototype_GameObject_Play_Goku
	//1P
	CCharacter::Character_DESC CharacterDesc{};


	if (CBattleInterface_Manager::Get_Instance()->Get_CharaDesc(0).eSlot == CUI_Define::SLOT_END)
	{
		CharacterDesc.iTeam = 1;
		CharacterDesc.ePlayerSlot = CUI_Define::LPLAYER1;


		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Play_Hit"), TEXT("Layer_Character"), &CharacterDesc)))
			return E_FAIL;

		CharacterDesc.iTeam = 2;
		CharacterDesc.ePlayerSlot = CUI_Define::RPLAYER1;

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Play_21"), TEXT("Layer_Character"), &CharacterDesc)))
			return E_FAIL;

		CharacterDesc.iTeam = 1;
		CharacterDesc.ePlayerSlot = CUI_Define::LPLAYER2;


		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Play_Hit"), TEXT("Layer_Character"), &CharacterDesc)))
			return E_FAIL;

		CharacterDesc.iTeam = 2;
		CharacterDesc.ePlayerSlot = CUI_Define::RPLAYER2;

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Play_Frieza"), TEXT("Layer_Character"), &CharacterDesc)))
			return E_FAIL;
	}
	else
	{
		for (int i = 0; i < 4; ++i)
		{
			if (CBattleInterface_Manager::Get_Instance()->Get_CharaDesc(i).eSlot == CUI_Define::SLOT_END)
				continue;

			CharacterDesc.iTeam = CBattleInterface_Manager::Get_Instance()->Get_CharaDesc(i).iTeam;
			CharacterDesc.ePlayerSlot = CBattleInterface_Manager::Get_Instance()->Get_CharaDesc(i).eSlot;
			_wstring strProtypeTag = CBattleInterface_Manager::Get_Instance()->Get_CharaDesc(i).PrototypeTag;

			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, strProtypeTag, TEXT("Layer_Character"), &CharacterDesc)))
				return E_FAIL;
		}
	}

	//if(FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Loading_GodDragon"), TEXT("Layer_UI_Loading_GodDragon"))))
	//	return E_FAIL;
#pragma endregion

#pragma region UI 사본 객체
	//캐릭터 생성 이후 IMGUI_UIMANAGER 초기화 이후 
	if (FAILED(Ready_UIObjects()))
		return E_FAIL;

#pragma endregion

#pragma region 카메라 포인트 로드

	//카메라 생성
	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Main_Camera"))))
		return E_FAIL;

	// 카메라 포인트 로드
	_wstring cameraFilePath = L"../Bin/CameraPoints.txt";
	CameraSaveData cameraSaveData;

	HRESULT hr = m_pGameInstance->Load_All_CameraPoints(cameraFilePath, &cameraSaveData);
	if (FAILED(hr)) {
		// 로드 실패 처리
		// 예: 로그 남기기 또는 기본 설정 적용
		return S_OK;
	}

	if (cameraSaveData.models.empty()) {
		// 로드된 데이터가 없으면 기본 설정 적용
		return S_OK;
	}

	// 로드된 데이터를 mainCamera에 적용
	CMain_Camera* mainCamera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, L"Layer_Main_Camera"));
	if (mainCamera) {
		mainCamera->ApplyCameraData(cameraSaveData);
	}

#pragma endregion



	//빛 준비
	if (FAILED(Ready_Lights()))
		return E_FAIL;


	CCharacter::Character_DESC Opening_CharacterDesc{};
	Opening_CharacterDesc.iTeam = 1;
	Opening_CharacterDesc.ePlayerSlot = CUI_Define::SLOT_END;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Opening_Kririn"), TEXT("Layer_Model_Opening"), &Opening_CharacterDesc)))
		return E_FAIL;


	CBattleInterface_Manager::Get_Instance()->Character_Opening_AIO();

	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Model_Preview"), TEXT("Layer_Model_Preview"))))
	//	return E_FAIL;

	return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{

	
	if (m_pGameInstance->Key_Down(DIK_LSHIFT))
	{
		CRenderInstance::Get_Instance()->Switch_AllBlackOut();
		m_fAccLevelChangeTime = 0.f;
		m_bLevelChanging = true;
	}

	if (m_bLevelChanging)
		m_fAccLevelChangeTime += fTimeDelta;

	if (m_bLevelChanging && m_fAccLevelChangeTime > 0.5f)
	{
		m_bLevelChanging = true;
		CRenderInstance::Get_Instance()->Switch_AllBlackOut();
		m_bLevelChange = false;



		if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_LOBBY))))
			return;

	}

	//if (m_bLevelChange && (m_pGameInstance->Key_Down(DIK_SPACE))
	//{
	//	m_bLevelChanging = true;
	//	CRenderInstance::Get_Instance()->Switch_AllBlackOut();
	//	m_bLevelChange = false;
	//	
	//	
	//	
	//	if(FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_LOBBY))))
	//		return;
	//
	//	return;
	//}


	if (m_bLevelChanging)
		return;

	if (m_pGameInstance->Key_Down(DIK_NUMPAD1))
	{
		list<class CUIObject*>  debug = m_pUI_Manager->m_ListBotUI;
		int a = 10;
	}
	//if (m_pGameInstance->Key_Down(DIK_E))
	//{
	//	m_pUI_Manager->CutSceneUI(TRUE);
	//}
	//
	//if (m_pGameInstance->Key_Down(DIK_R))
	//{
	//	m_pUI_Manager->CutSceneUI(FALSE);
	//}

	if (m_pGameInstance->Key_Down(DIK_F9))
	{
		//m_pRenderInstance->Switch_AllBlackOut(false);
	}
	if (m_pGameInstance->Key_Down(DIK_F10))
	{
		m_pRenderInstance->Switch_AllBlackOut();
	}

	if (m_pGameInstance->Key_Down(DIK_Z))
	{
		LIGHT_DESC			LightDesc{};

		ZeroMemory(&LightDesc, sizeof(LIGHT_DESC));
		LightDesc.eType = LIGHT_DESC::TYPE_POINT;
		LightDesc.vPosition = _float4(0.f, 0.f, 0.f, 1.f);
		LightDesc.fRange = 30.f;
		LightDesc.vDiffuse = _float4(1.2f, 1.15f, 0.7f, 1.0f);
		//LightDesc.vDiffuse = _float4(1.0f, 0.f, 0.f, 1.f);
		LightDesc.vAmbient = _float4(0.1f, 0.1f, 0.1f, 1.f);
		LightDesc.vSpecular = _float4(1.0f, 0.95f, 0.45f, 1.f);

		LightDesc.fAccTime = 0.f;
		LightDesc.fLifeTime = 3.f;
		LightDesc.strName = "Explosion";
		if (FAILED(m_pRenderInstance->Add_Effect_Light(LightDesc.strName, LightDesc)))
			return;
	}
	if (m_pGameInstance->Key_Down(DIK_X))
	{
		LIGHT_DESC			LightDesc{};

		ZeroMemory(&LightDesc, sizeof(LIGHT_DESC));
		LightDesc.eType = LIGHT_DESC::TYPE_POINT;
		LightDesc.vPosition = _float4(0.f, 0.f, 0.f, 1.f);
		LightDesc.fRange = 30.f;

		LightDesc.vDiffuse = _float4(0.9f, 1.1f, 1.7f, 1.0f); // 파란빛 계열로 변경

		LightDesc.vAmbient = _float4(0.1f, 0.1f, 0.1f, 1.f);
		LightDesc.vSpecular = _float4(1.0f, 0.95f, 0.45f, 1.f);

		LightDesc.fAccTime = 0.f;
		LightDesc.fLifeTime = 3.f;
		LightDesc.strName = "Ray";
		if (FAILED(m_pRenderInstance->Add_Effect_Light(LightDesc.strName, LightDesc)))
			return;
	}


	m_pUI_Manager->GamePlayUpdate(fTimeDelta);
	//m_pIMGUI_Manager->Update(fTimeDelta);
	m_pEffect_Manager->Update(fTimeDelta);
	m_pParticle_Manager->Update(fTimeDelta);
	m_pSubTitle_Manager->Update(fTimeDelta);
	m_pEffect_Manager->Late_Update(fTimeDelta);

	m_pQTE_Manager->Update(fTimeDelta);
	m_pQTE_Manager->Late_Update(fTimeDelta);
	m_pParticle_Manager->Late_Update(fTimeDelta);
	m_pSubTitle_Manager->Late_Update(fTimeDelta);
	m_pMap_Manager->Update(fTimeDelta);

	if (m_pGameInstance->Key_Down(DIK_V))
		CUI_Manager::Get_Instance()->WinUI(LEVEL_GAMEPLAY);
}

HRESULT CLevel_GamePlay::Render(_float fTimeDelta)
{
	SetWindowText(g_hWnd, TEXT("게임플레이레벨"));

	m_pEffect_Manager->Render(fTimeDelta);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Main_Camera"), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	

	LIGHT_DESC			LightDesc{};

	ZeroMemory(&LightDesc, sizeof(LIGHT_DESC));
	LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	LightDesc.vPosition = _float4(30.f, 50.f, 1800.f, 1.f);
	LightDesc.fRange = 10000.f;
	LightDesc.vDiffuse = _float4(0.78f, 0.95f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.2f, 0.3f, 0.4f, 1.f);
	LightDesc.vSpecular = _float4(0.1f, 0.1f, 0.1f, 0.1f);


	
	if (FAILED(m_pRenderInstance->Add_Light(LightDesc)))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_Light(LightDesc)))
		return E_FAIL;



	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_UIObjects()
{
	//UIMANAGER 초기화
	m_pUI_Manager->InitUIObject();

	CUIObject::UI_DESC tHpDesc = {};

	for (int i = 0; i < 2; ++i)
	{
		tHpDesc.eLRPos = static_cast<CUIObject::UI_LRPOS>(i);
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_HpPanel"), TEXT("Layer_UI_HpGauge"), &tHpDesc)))
			return E_FAIL;

		//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_HpEffect"), TEXT("Layer_UI_HpGauge"), &tHpDesc)))
		//	return E_FAIL;

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_HpGauge"), TEXT("Layer_UI_HpGauge"), &tHpDesc)))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_SubHpGauge"), TEXT("Layer_UI_HpGauge"), &tHpDesc)))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_SubHpPanel"), TEXT("Layer_UI_HpGauge"), &tHpDesc)))
			return E_FAIL;
	}
	//캐릭터 아이콘

	CUIObject::UI_DESC Icon_Desc = {};

	for (int i = 0; i < 2; ++i)
	{
		Icon_Desc.eLRPos = static_cast<CUIObject::UI_LRPOS>(i);

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_Chara_Icon_Panel"), TEXT("Layer_UI_Chara_Icon"), &Icon_Desc)))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_Chara_Icon"), TEXT("Layer_UI_Chara_Icon"), &Icon_Desc)))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_Chara_SubIcon"), TEXT("Layer_UI_Chara_Icon"), &Icon_Desc)))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_Sub_Chara_Icon_Panel"), TEXT("Layer_UI_Chara_Icon"), &Icon_Desc)))
			return E_FAIL;
	}


	//공격력 버프

	CUIObject::UI_DESC AttBufoDesc = {};

	for (int i = 0; i < 2; ++i)
	{
		AttBufoDesc.eLRPos = static_cast<CUIObject::UI_LRPOS>(i);

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_AttBufMark"), TEXT("Layer_UI_AttBufEffect"), &AttBufoDesc)))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_AttBuf"), TEXT("Layer_UI_AttBufEffect"), &AttBufoDesc)))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_AttBufNone"), TEXT("Layer_UI_AttBuf"), &AttBufoDesc)))
			return E_FAIL;
	}

	// 스킬
	CUIObject::UI_DESC SkilloDesc = {};
	for (int i = 0; i < 2; i++)
	{
		SkilloDesc.eLRPos = static_cast<CUIObject::UI_LRPOS>(i);

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_SkillGauge"), TEXT("Layer_UI_SkillGauge"), &SkilloDesc)))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_SkillGaugeBar"), TEXT("Layer_UI_SkillGauge"), &SkilloDesc)))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_SkillNumber"), TEXT("Layer_UI_SkillNumber"), &SkilloDesc)))
			return E_FAIL;

		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_SkillEffect"), TEXT("Layer_UI_SkillGauge"), &SkilloDesc);


	}

	//타이머

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_TimerPanel"), TEXT("Layer_UI_Timer"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_Timer"), TEXT("Layer_UI_Timer"))))
		return E_FAIL;


	//콤보
	CUIObject::UI_DESC ComboDesc = {};
	for (int i = 0; i < 3; ++i)
	{
		ComboDesc.iNumUI = i;

		for (int k = 0; k < 2; k++)
		{
			ComboDesc.eLRPos = static_cast<CUIObject::UI_LRPOS>(k);
			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_ComboNumber"), TEXT("Layer_UI_Combo_Number"), &ComboDesc);
		}
	}

	for (int k = 0; k < 2; k++)
	{
		ComboDesc.eLRPos = static_cast<CUIObject::UI_LRPOS>(k);

		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_ComboFont"), TEXT("Layer_UI_Combo_Font"), &ComboDesc);
		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_ComboEffect"), TEXT("Layer_UI_Combo_Effect"), &ComboDesc);
	}

	//Key Input


	CUIObject::UI_DESC KeyInputDesc = {};
	for (int i = 0; i < 2; i++)
	{
		KeyInputDesc.eLRPos = static_cast<CUIObject::UI_LRPOS>(i);

		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_InputDirPanel"), TEXT("Layer_UI_Input"), &KeyInputDesc);
		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_InputDir"), TEXT("Layer_UI_Input"), &KeyInputDesc);
	}

	for (int i = 0; i < 2; i++)
	{
		KeyInputDesc.eLRPos = static_cast<CUIObject::UI_LRPOS>(i);
		for (int k = 0; k < 6; k++)
		{
			KeyInputDesc.iNumUI = k;
			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_InputAction"), TEXT("Layer_UI_InputAction"), &KeyInputDesc);
		}
	}

	CUIObject::UI_DESC FontNameDesc = {};
	for (int i = 0; i < 2; ++i)
	{
		KeyInputDesc.eLRPos = static_cast<CUIObject::UI_LRPOS>(i);
		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_FontName"), TEXT("Layer_UI_FontName"), &KeyInputDesc);

	}


	//옵션

	//사운드 옵션
	m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_Opt_Sound_Panel"), TEXT("Layer_UI_Option_Sound"));
	m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_Opt_Sound_Effect"), TEXT("Layer_UI_Option_Sound"));
	m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_Opt_Sound_Title"), TEXT("Layer_UI_Option_Sound"));
	m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_Opt_Sound_Font"), TEXT("Layer_UI_Option_Sound"));
	m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_Opt_Sound_Arrow"), TEXT("Layer_UI_Option_Sound"));

	for (int i = 0; i < 3; i++)
	{
		CUIObject::UI_DESC VolumePanelDesc = {};
		VolumePanelDesc.iNumUI = i;

		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_Opt_Sound_Volume_Panel"), TEXT("Layer_UI_Option_Sound"), &VolumePanelDesc);
		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_Opt_Sound_Volume_Gauge"), TEXT("Layer_UI_Option_Sound"), &VolumePanelDesc);
	}




	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Sound()
{
	//우주배경음
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Audio/BGM/013_bat_space.ogg", CSound_Manager::SOUND_KEY_NAME::SPACE_BGM, CSound_Manager::SOUND_CATEGORY::BGM, true);
	//화산배경음
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Audio/BGM/004_bat_volcano.ogg", CSound_Manager::SOUND_KEY_NAME::VOLCANO_BGM, CSound_Manager::SOUND_CATEGORY::BGM, true);

#pragma region Goku_Sound

#pragma region Voice

	//약 공격1,2
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_Goku, L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_Voice/Light_Attack_1.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::Light_Attack_Goku_1, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_Goku, L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_Voice/Light_Attack_2.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::Light_Attack_Goku_2, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	//약 공격3 (호랴), (공중추적)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_Voice/Heavy_Attack.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_Heavy_Attack, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//어퍼 공격 (소코카)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_Voice/Upper_Attack.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_Upper_Attack, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	//원거리 공격
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_Voice/Range_Attack.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_Range_Attack, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	//에네르기 공격
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_Voice/Energy_Attack.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_Energy_Attack, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//오공 히트 총 16개
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::Hit_Goku, L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_Voice/Hit_0.wav", CSound_Manager::SOUND_GROUP_KEY_NAME::Goku_Hit_0, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::Hit_Goku, L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_Voice/Hit_1.wav", CSound_Manager::SOUND_GROUP_KEY_NAME::Goku_Hit_1, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::Hit_Goku, L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_Voice/Hit_2.wav", CSound_Manager::SOUND_GROUP_KEY_NAME::Goku_Hit_2, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//잡기 (이그조)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_Voice/Grab.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_Grab, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//잡기 이후 공격 (아다다다)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_Voice/Grab_Attack.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_Grab_Attack, CSound_Manager::SOUND_CATEGORY::VOICE, false);



	//필살기_0
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_Voice/Ultimate_0.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_Ultimate_Attack_0, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	//필살기_1
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_Voice/Ultimate_1.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_Ultimate_Attack_1, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	//필살기_2
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_Voice/Ultimate_2.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_Ultimate_Attack_2, CSound_Manager::SOUND_CATEGORY::VOICE, false);


	//입장씬
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_Voice/Entry.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_Entry, CSound_Manager::SOUND_CATEGORY::VOICE, false);

#pragma endregion

#pragma region SFX

	//SFX

	//Light_Attack
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_Hit_SFX, L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_SFX/Light_Attack_SFX_0.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::Light_Attack_Goku_1_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_Hit_SFX, L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_SFX/Light_Attack_SFX_1.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::Light_Attack_Goku_2_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_Hit_SFX, L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_SFX/Light_Attack_SFX_2.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::Light_Attack_Goku_3_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//대쉬
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_SFX/Dash_SFX.wav", CSound_Manager::SOUND_KEY_NAME::Common_Dash_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);
	//다운 훅 대쉬
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_SFX/DownHook_Dash_SFX.wav", CSound_Manager::SOUND_KEY_NAME::Common_DownHook_Dash_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);
	//에네르기파 준비
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_SFX/Energy_SFX.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_Energy_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);
	//에네르기파 발사
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_SFX/Energy_Fire_SFX.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_Energy_Fire_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//강공격
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_SFX/Heavy_Attack_SFX.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_Heavy_Attack_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false, false);
	//원거리 공격
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_SFX/Range_Attack_SFX.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_Range_Attack_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false, false);


	//초필살기 1
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_SFX/Ultimate_0.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_Ultimate_0_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);
	//초필살기 2
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_SFX/Ultimate_1.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_Ultimate_1_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);
	//초필살기 3
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_SFX/Ultimate_2.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_Ultimate_2_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);
	//초필살기 4
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_SFX/Ultimate_3.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_Ultimate_3_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//스매쉬 Hit
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_SFX/Smash_Hit.ogg", CSound_Manager::SOUND_KEY_NAME::Smash_Hit_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//기탄 Hit
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_SFX/J_Attack_Hit.ogg", CSound_Manager::SOUND_KEY_NAME::J_Attack_Hit_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false, false);

	// 밑 + 앞 + 약공격
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_SFX/Down_Forward_Light_Attack.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_Down_Forward_Light_Attack_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false, false);


	//입장씬
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_SFX/ARC_BTL_DS02_EntryGKS_Start.wav", CSound_Manager::SOUND_KEY_NAME::Goku_Entry_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

#pragma endregion


#pragma endregion

#pragma region 21_Sound

#pragma region 21_Voice

	//약공
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_21, L"../Bin/SoundSDK/AudioClip/Chara/21/Real_Voice/Light_Attack_1.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::Light_Attack_21_1, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_21, L"../Bin/SoundSDK/AudioClip/Chara/21/Real_Voice/Light_Attack_2.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::Light_Attack_21_2, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_21, L"../Bin/SoundSDK/AudioClip/Chara/21/Real_Voice/Light_Attack_3.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::Light_Attack_21_3, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_21, L"../Bin/SoundSDK/AudioClip/Chara/21/Real_Voice/Light_Attack_4.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::Light_Attack_21_4, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//강공
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_Voice/Heavy_Attack.ogg", CSound_Manager::SOUND_KEY_NAME::Heavy_Attack_21, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//아래 앞 약공 (기탄)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_Voice/Down_Forward_Light.ogg", CSound_Manager::SOUND_KEY_NAME::Down_Forward_Light_21, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//히트
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::Hit_21, L"../Bin/SoundSDK/AudioClip/Chara/21/Real_Voice/Hit_21_0.wav", CSound_Manager::SOUND_GROUP_KEY_NAME::Hit_21_0, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::Hit_21, L"../Bin/SoundSDK/AudioClip/Chara/21/Real_Voice/Hit_21_1.wav", CSound_Manager::SOUND_GROUP_KEY_NAME::Hit_21_1, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::Hit_21, L"../Bin/SoundSDK/AudioClip/Chara/21/Real_Voice/Hit_21_2.wav", CSound_Manager::SOUND_GROUP_KEY_NAME::Hit_21_2, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::Hit_21, L"../Bin/SoundSDK/AudioClip/Chara/21/Real_Voice/Hit_21_3.wav", CSound_Manager::SOUND_GROUP_KEY_NAME::Hit_21_3, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::Hit_21, L"../Bin/SoundSDK/AudioClip/Chara/21/Real_Voice/Hit_21_4.wav", CSound_Manager::SOUND_GROUP_KEY_NAME::Hit_21_4, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::Hit_21, L"../Bin/SoundSDK/AudioClip/Chara/21/Real_Voice/Hit_21_5.wav", CSound_Manager::SOUND_GROUP_KEY_NAME::Hit_21_5, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//그랩 (카그고와 이이)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_Voice/Grab.ogg", CSound_Manager::SOUND_KEY_NAME::Grab_Attack_21, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	//추적 (마다요)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_Voice/ChaseDash.ogg", CSound_Manager::SOUND_KEY_NAME::Chase_Attack_21, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	//아래 앞 J (소코) // O
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_Voice/Down_Forward_J.ogg", CSound_Manager::SOUND_KEY_NAME::Down_Forward_J_21, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	//아래 뒤 약공격 (스라나이데) // O
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_Voice/Down_Backward_Light.ogg", CSound_Manager::SOUND_KEY_NAME::Down_Backward_Light_21, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	//1필 (미세테 아게르) // O
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_Voice/Ultimate_1_Start.ogg", CSound_Manager::SOUND_KEY_NAME::Ultimate_1_Start_21, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//3필 시작 (우케테 미나사이) // O
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_Voice/Ultimate_3_Start.ogg", CSound_Manager::SOUND_KEY_NAME::Ultimate_3_Start_21, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//3필 중간 (데아!!) // 이거 태욱이가 손봐줘야함, 공중 상승 할때 중복 호출문제 때문에 X
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_Voice/Ultimate_3_Middle.ogg", CSound_Manager::SOUND_KEY_NAME::Ultimate_3_Middle_21, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//3필 마지막 (오사에 키레나이요오) // O
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_Voice/Ultimate_3_Final.ogg", CSound_Manager::SOUND_KEY_NAME::Ultimate_3_Final_21, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//J 기탄 보이스 // O
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_Voice/J_Attack_Voice.ogg", CSound_Manager::SOUND_KEY_NAME::J_Attack_Voice_21, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//공중추적 (소코네)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_Voice/Air_Chase.ogg", CSound_Manager::SOUND_KEY_NAME::Air_Chase_21, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//입장
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_Voice/Entry_0.ogg", CSound_Manager::SOUND_KEY_NAME::Entry_21, CSound_Manager::SOUND_CATEGORY::VOICE, false);
#pragma endregion

#pragma region 21_SFX

	//그랩_SFX_0
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_SFX/Grab_0.ogg", CSound_Manager::SOUND_KEY_NAME::Grab_Attack_0_21_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);
	//그랩_SFX_1
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_SFX/Grab_1.ogg", CSound_Manager::SOUND_KEY_NAME::Grab_Attack_1_21_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);
	//J 기탄
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_SFX/J_Attack_21.ogg", CSound_Manager::SOUND_KEY_NAME::J_Attack_21, CSound_Manager::SOUND_CATEGORY::SFX, false, true);

	//1필 Start
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_SFX/Ultimate_1_Start.ogg", CSound_Manager::SOUND_KEY_NAME::Ultimate_1_Start_21_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//1필 Charge
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_SFX/Ultimate_1_Charge.ogg", CSound_Manager::SOUND_KEY_NAME::Ultimate_1_Charge_21_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//1필 Expl
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_SFX/Ultimate_1_Expl.ogg", CSound_Manager::SOUND_KEY_NAME::Ultimate_1_Expl_21_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//1필 Shot
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_SFX/Ultimate_1_Shot.ogg", CSound_Manager::SOUND_KEY_NAME::Ultimate_1_Shot_21_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//3필 Start
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_SFX/Ultimate_3_Start.ogg", CSound_Manager::SOUND_KEY_NAME::Ultimate_3_Start_21_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//3필 Dash
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_SFX/Ultimate_3_Dash.ogg", CSound_Manager::SOUND_KEY_NAME::Ultimate_3_Dash_21_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//3필 Attacks
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_SFX/Ultimate_3_Attacks.ogg", CSound_Manager::SOUND_KEY_NAME::Ultimate_3_Attacks_21_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//3필 Finish
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_SFX/Ultimate_3_Finish.ogg", CSound_Manager::SOUND_KEY_NAME::Ultimate_3_Finish_21_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//입장1
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_SFX/ARC_BTL_TON_Entry_1.ogg", CSound_Manager::SOUND_KEY_NAME::Entry_21_SFX_0, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//입장2
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_SFX/ARC_BTL_TON_Entry_2.ogg", CSound_Manager::SOUND_KEY_NAME::Entry_21_SFX_1, CSound_Manager::SOUND_CATEGORY::SFX, false);
#pragma endregion

#pragma endregion

#pragma region Hit Sound

#pragma region Hit_Voice

	//약공격 4개 그룹사운드
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_HIT, L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Light_Attack_0.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::Light_Attack_Hit_0, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_HIT, L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Light_Attack_1.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::Light_Attack_Hit_1, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_HIT, L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Light_Attack_2.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::Light_Attack_Hit_2, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_HIT, L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Light_Attack_3.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::Light_Attack_Hit_3, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//약공격 마지막 강타 사운드
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Light_Attack_Final_Attack.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_Light_Attack_Final_Attack, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//J 사운드 (반사)
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::J_ATTACK_HIT, L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_J_0.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::HIT_J_Attack_0, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::J_ATTACK_HIT, L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_J_1.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::HIT_J_Attack_1, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//강공
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Heavy_Attack.ogg", CSound_Manager::SOUND_KEY_NAME::Heavy_Attack_Hit, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	// 아래 뒤 약공격 (준비자세) (토모레, 우코쿠나)
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::HIT_Down_Back_Light, L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Down_Back_Light_0.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::HIT_Down_Back_Light_0, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::HIT_Down_Back_Light, L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Down_Back_Light_1.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::HIT_Down_Back_Light_1, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	// 아래 앞 약공격 (준비자세) (이크조)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Down_Forward_Light.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_Down_Forward_Light, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	// 아래 앞 약공격 + 약공격 (쓉)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Down_Forward_Light_Light_Attack.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_Down_Forward_Light_Light_Attack, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	// 아래 앞 약공격 + 중공격 (코치라다)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Down_Forward_Light_Middle_Attack.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_Down_Forward_Light_Middle_Attack, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	// 아래 앞 약공격 + 강공격 (흠!)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Down_Forward_Light_Heavy_Attack.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_Down_Forward_Light_Heavy_Attack, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	// 공중추적 ( 세이데미로, 사떼
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Air_Chase_0.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_Air_Chase, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	// 백대시
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Back_Dash.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_BackDash, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	// 잡기 시도 (코히)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Grab_Try.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_Grab_Try, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//잡기 성공해서 연타할때 (죠요요~)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Grab_Success.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_Grab_Success, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//1필 (와카토 모이노 세겐)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Ultimate_1.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_Ultimate_1, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//3필
	//첫번째 (이마노 카마에와)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Ultimate_3_0.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_Ultimate_3_0, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	//두번째 (코우다)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Ultimate_3_1.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_Ultimate_3_1, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	//세번째 (하아아~!!) 연타
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Ultimate_3_2.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_Ultimate_3_2, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	//네번째 (오레와 센츄~)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Ultimate_3_3.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_Ultimate_3_3, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//피격
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::Hit_Hit, L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Hit_0.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::Hit_Hit_0, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::Hit_Hit, L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Hit_1.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::Hit_Hit_1, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::Hit_Hit, L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Hit_2.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::Hit_Hit_2, CSound_Manager::SOUND_CATEGORY::VOICE, false);

#pragma endregion

#pragma region Hit_SFX
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_SFX/Hit_Light_Attack_0_SFX.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_Light_Attack_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);
	//J 어택
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_SFX/Hit_J_Attack_SFX.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_J_Attack_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	// 아래 앞 약공격 + 약공격 + 중공격
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_SFX/Hit_Down_Forward_Light_Light_Attack_SFX.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_Down_Forward_Light_Light_Attack_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	// 아래 앞 약공격 + 강공격
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_SFX/Hit_Down_Forward_Light_Heavy_Attack_SFX.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_Down_Forward_Light_Heavy_Attack_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	// 아래 뒤 약공격 기모으다 유리 깨질 때
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_SFX/Hit_Down_Backward_Light_Cancle_Attack_SFX.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_Down_Backward_Light_Cancle_Attack_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//1필 스타트, 3필 스타트
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_SFX/Hit_Ultimate_1_Start.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_Ultimate_1_Start, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//1필 공격
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_SFX/Hit_Ultimate_1_Attack.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_Ultimate_1_Attack, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//3필 연타 공격
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_SFX/Hit_Ultimate_3_Attack.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_Ultimate_3_Attack, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//아이스 깨질 때
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_SFX/Hit_Ice_Hit_SFX.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_Ice_Hit_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//입장
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_SFX/ARC_BTL_HTN_Entry.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_Entry_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

#pragma endregion


#pragma endregion

#pragma region Frieza Sound

#pragma region Frieza_Voice

	//피격 
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::Hit_Frieza, L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_Voice/Hit_0.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::Hit_0_Frieza, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::Hit_Frieza, L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_Voice/Hit_1.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::Hit_1_Frieza, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::Hit_Frieza, L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_Voice/Hit_2.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::Hit_2_Frieza, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::Hit_Frieza, L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_Voice/Hit_3.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::Hit_3_Frieza, CSound_Manager::SOUND_CATEGORY::VOICE, false);


	//약공 3개, 중공도 포함
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::Light_Attack_Frieza, L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_Voice/Light_Attack_0.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::Light_Attack_0_Frieza, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::Light_Attack_Frieza, L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_Voice/Light_Attack_1.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::Light_Attack_1_Frieza, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::Light_Attack_Frieza, L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_Voice/Light_Attack_2.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::Light_Attack_2_Frieza, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//약공 마지막 (쿠라이, 아래 기탄)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_Voice/Light_Attack_Final.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Light_Attack_Final, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//백대쉬
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_Voice/Back_Dash.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Back_Dash, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//J 기탄 (또다이)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_Voice/J_Attack.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_J_Attack, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//강공격 (죠또 오도시데루카이~) , 아래 강공격
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_Voice/Heavy_Attack.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Heavy_Attack, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//아래 앞 약공격 (나마 이키다요), 중공격, 강공격
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_Voice/Down_Forward_Light.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Down_Forward_Light, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//아래 앞 J(기탄) (하이잇!~)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_Voice/Down_Forward_J.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Down_Forward_J, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	// 아래 뒤 J(기탄) (오레가 나마테루가) (부메랑)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_Voice/Down_Back_J.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Down_Back_J, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	// 잡기
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_Voice/Grab.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Grab, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	// 1필 시작 (코코마데다)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_Voice/Ultimate_0_Start.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Ultimate_0_Start, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	// 1필 마무리 (시네!)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_Voice/Ultimate_0_End.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Ultimate_0_End, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	// 3필 시작 (이마 스그네~)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_Voice/Ultimate_3_Start.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Ultimate_3_Start, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	// 3필 마무리 (으아~)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_Voice/Ultimate_3_End.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Ultimate_3_End, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	// 공중 추적 (이키마스요)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_Voice/Air_Chase.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Air_Chase, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	// 골든 프리저 (아래 + 뒤 + 강특)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_Voice/Golden.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Golden, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	// 낙법 반사 (오레~)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_Voice/Reflect.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Reflect, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//입장
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_Voice/Entry.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Entry, CSound_Manager::SOUND_CATEGORY::VOICE, false);

#pragma endregion

#pragma region Frieza_SFX

	//약공격 SFX
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_SFX/Light_Attack_SFX.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Light_Attack_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);
	//약공격 마지막 SFX
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_SFX/Light_Attack_Final_SFX.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Light_Attack_Final_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//기탄 SFX
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_SFX/J_Attack_SFX.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_J_Attack_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false, false);

	//강공격 SFX
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_SFX/Heavy_SFX.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Heavy_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//어퍼 SFX
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_SFX/Knock_Away_SFX.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Knock_Away_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//아래 앞 약공격
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_SFX/Down_Forward_Light_SFX.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Down_Forward_Light_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	// 아래 뒤 J(기탄) (부메랑)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_SFX/Down_Backward_J_SFX.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Down_Backward_J_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	// 1필 기모으기
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_SFX/Ultimate_0_Start.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Ultimate_0_Start_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	// 1필 발사
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_SFX/Ultimate_0_Fire.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Ultimate_0_Fire_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	// 1필 폭발
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_SFX/Ultimate_0_Expl.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Ultimate_0_Expl_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//3필 시작
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_SFX/Ultimate_3_Start.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Ultimate_3_Start_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//3필 폭발 _0 (동시 시작)
	//3필 폭발 _1 (동시 시작)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_SFX/Ultimate_3_Expl_0.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Ultimate_3_Expl_0_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_SFX/Ultimate_3_Expl_1.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Ultimate_3_Expl_1_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//골든프리저
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_SFX/Golden_Frieza.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Golden_Frieza_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//낙법 반사
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_SFX/Reflect_SFX.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Reflect_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//입장1
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_SFX/ARC_BTL_FRN_Entry1.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Entry_0_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//입장2
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_SFX/ARC_BTL_FRN_Entry2.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Entry_1_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

#pragma endregion

#pragma endregion

#pragma region UI 나레이션 사운드
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Audio/Narration/JPN/NA_0509_BattleCall_Start01.ogg", CSound_Manager::SOUND_KEY_NAME::NARRATION_READY, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Audio/Narration/JPN/NA_0513_BattleCall_Fight01.ogg", CSound_Manager::SOUND_KEY_NAME::NARRATION_FIGHT, CSound_Manager::SOUND_CATEGORY::VOICE, false);
#pragma endregion

#pragma region UI 사운드
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Audio/UI/ARC_MENU_SYS_MenuOpen.ogg", CSound_Manager::SOUND_KEY_NAME::UI_MENU_OPEN, CSound_Manager::SOUND_CATEGORY::SFX, false);
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Audio/UI/ARC_MENU_SYS_MenuClose.ogg", CSound_Manager::SOUND_KEY_NAME::UI_MENU_CLOSE, CSound_Manager::SOUND_CATEGORY::SFX, false);
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Audio/UI/ARC_MENU_SYS_Cursor.ogg", CSound_Manager::SOUND_KEY_NAME::UI_MENU_CURSOR, CSound_Manager::SOUND_CATEGORY::SFX, false);
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Common/Real_SFX/ARC_BTL_SYS_Fight.ogg", CSound_Manager::SOUND_KEY_NAME::Common_FIGHT_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Common/Real_SFX/ARC_BTL_SYS_Finish.ogg", CSound_Manager::SOUND_KEY_NAME::Common_FINISH_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);
#pragma endregion

#pragma region QTE 사운드
	//QTE 성공
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Audio/UI/QTE_UI_SOUND/QTE_Sucess.ogg", CSound_Manager::SOUND_KEY_NAME::QTE_SUCCESS_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);
	//동시잡기 성공
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Audio/UI/QTE_UI_SOUND/QTE_SAME_GRAB_ICON_Sucess.ogg", CSound_Manager::SOUND_KEY_NAME::QTE_SAMEGRAB_SUCCESS_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);
	//동시잡기 실패
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Audio/UI/QTE_UI_SOUND/QTE_SAME_GRAB_ICON_Fail.ogg", CSound_Manager::SOUND_KEY_NAME::QTE_SAMEGRAB_FAIL_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);
#pragma endregion

#pragma region 공통 사운드


#pragma region SFX

	//약공격 맞았을 때
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Common/Real_SFX/ARC_BTL_CMN_Hit_Small-A.ogg", CSound_Manager::SOUND_KEY_NAME::Common_Small_Hit_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//중공격 맞았을 때
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Common/Real_SFX/ARC_BTL_CMN_Hit_Midle-A.ogg", CSound_Manager::SOUND_KEY_NAME::Common_Medium_Hit_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//강공격 맞았을 때
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Common/Real_SFX/ARC_BTL_CMN_Hit_Large-A.ogg", CSound_Manager::SOUND_KEY_NAME::Common_Heavy_Hit_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//약공격 가드
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Common/Real_SFX/ARC_BTL_CMN_Guard_Small-A.ogg", CSound_Manager::SOUND_KEY_NAME::Common_Small_Guard_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//중공격 가드
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Common/Real_SFX/ARC_BTL_CMN_Guard_Midle.ogg", CSound_Manager::SOUND_KEY_NAME::Common_Medium_Guard_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//강공격 가드
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Common/Real_SFX/ARC_BTL_CMN_Guard_Large.ogg", CSound_Manager::SOUND_KEY_NAME::Common_Heavy_Guard_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//점프
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Common/Real_SFX/ARC_BTL_CMN_GK_Jump.ogg", CSound_Manager::SOUND_KEY_NAME::Common_Jump_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//백대시
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Common/Real_SFX/ARC_BTL_CMN_PC_AirDash.ogg", CSound_Manager::SOUND_KEY_NAME::Common_Back_Dash_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//공중에서 내리 찍혔을 때
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Common/Real_SFX/ARC_BTL_CMN_GroundCrush.ogg", CSound_Manager::SOUND_KEY_NAME::Common_GroundCrush_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//대시 시작
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Common/Real_SFX/ARC_BTL_CMN_JetDashStart.ogg", CSound_Manager::SOUND_KEY_NAME::Common_Dash_Start_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//대시 진행중
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Common/Real_SFX/ARC_BTL_CMN_JetDashLoop_LP.ogg", CSound_Manager::SOUND_KEY_NAME::Common_Dash_Loop_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//대시 종료
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Common/Real_SFX/ARC_BTL_CMN_JetDashStop_Basa.ogg", CSound_Manager::SOUND_KEY_NAME::Common_Dash_End_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//벽에 부딪혔을 때
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Common/Real_SFX/ARC_BTL_CMN_CrashAirWall.ogg", CSound_Manager::SOUND_KEY_NAME::Common_Crash_Air_Wall_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//잡기 시작 (드래곤 러쉬)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Common/Real_SFX/ARC_BTL_CMN_DrgnRush_Start.ogg", CSound_Manager::SOUND_KEY_NAME::Common_Grab_Start_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//잡기 중 (드래곤 러쉬)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Common/Real_SFX/ARC_BTL_CMN_DrgnRush_Rush.ogg", CSound_Manager::SOUND_KEY_NAME::Common_Grab_Rush_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//잡기 끝 (드래곤 러쉬)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Common/Real_SFX/ARC_BTL_CMN_DrgnRush_Fnsh.ogg", CSound_Manager::SOUND_KEY_NAME::Common_Grab_End_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

#pragma endregion



#pragma endregion

#pragma region 오공 + 크리링 vs 프리저

	//Voice
	// 
	//프리저 그만해! (야메다 프리저!)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku_vs_Frieza/ds02-003.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_vs_Frieza_0, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	//고쿠!!		(크리링)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku_vs_Frieza/ds02-004.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_vs_Frieza_1, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	//프리저 웃음소리 (크크)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku_vs_Frieza/ds02-006.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_vs_Frieza_2, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	//용서 못해.. 잘도 (우르상..!) -> 손오공이 대사
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku_vs_Frieza/ds02-007.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_vs_Frieza_3, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	//나니..? (프리저) 
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku_vs_Frieza/ds02-009.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_vs_Frieza_4, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	//오레와 와까다제! (나 화났다 프리저) -> 손오공 대사
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku_vs_Frieza/ds02-010.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_vs_Frieza_5, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//SFX
	//프리저 폭발 SFX
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku_vs_Frieza/ARC_BTL_CMN_Hit_FRN_DthScr.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_vs_Frieza_Expl_SFX_0, CSound_Manager::SOUND_CATEGORY::SFX, false);
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku_vs_Frieza/ARC_BTL_CMN_Hit_FRN_DtRtzn.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_vs_Frieza_Expl_SFX_1, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//손오공 기모으기
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku_vs_Frieza/ARC_BTL_CMN_Charge_Loop_LP.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_vs_Frieza_Goku_ChargeLoof_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//손오공 기모으기
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku_vs_Frieza/ARC_BTL_GKS_SprKameha_Chrg.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_vs_Frieza_Goku_Charge_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//손오공 화났을 때 효과음
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku_vs_Frieza/ARC_BTL_SYS_ActSkill_Normal.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_vs_Frieza_Goku_Attack_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

#pragma endregion

#pragma region 히트 승리씬
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_Voice/vhtn404_vs_gkb.ogg", CSound_Manager::SOUND_KEY_NAME::HIT_WIN, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/SFX/ARC_BTL_HTN_Win.ogg", CSound_Manager::SOUND_KEY_NAME::HIT_WIN_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

#pragma endregion

#pragma region 씬 전환 터질때 혹은 에네르기파
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Common/Real_SFX/ARC_BG_FtbsFin_Start.ogg", CSound_Manager::SOUND_KEY_NAME::SCENE_CHANGE_START, CSound_Manager::SOUND_CATEGORY::SFX, false);
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Common/Real_SFX/ARC_BG_FtbsFin_BlowOut.ogg", CSound_Manager::SOUND_KEY_NAME::SCENE_CHANGE_Futtobi, CSound_Manager::SOUND_CATEGORY::SFX, false);
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Common/Real_SFX/ARC_BG_FtbsFin_Enter.ogg", CSound_Manager::SOUND_KEY_NAME::SCENE_CHANGE_Enter, CSound_Manager::SOUND_CATEGORY::SFX, false);
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Common/Real_SFX/ARC_BG_FinSmoke_PreFight.ogg", CSound_Manager::SOUND_KEY_NAME::SCENE_CHANGE_PreFight, CSound_Manager::SOUND_CATEGORY::SFX, false);
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Common/Real_SFX/ARC_BG_ExplFin_Tunagi.ogg", CSound_Manager::SOUND_KEY_NAME::SCENE_CHANGE_Tunagi, CSound_Manager::SOUND_CATEGORY::SFX, false);

	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Common/Real_SFX/ARC_BG_FtbsFin_HitVolcano.ogg", CSound_Manager::SOUND_KEY_NAME::SCENE_CHANGE_Far_Expl, CSound_Manager::SOUND_CATEGORY::SFX, false);
#pragma endregion



	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Space()
{

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Shadow_Camera"), TEXT("Layer_A"))))
		return E_FAIL;

	CMap_Manager::Map_Object_Key tDesc{};
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_SpaceSky");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_SpaceSky"), TEXT("Layer_SpaceSky"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_SpaceHorizon");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_SpaceHorizon"), TEXT("Layer_SpaceHorizon"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_SpaceSun");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_SpaceSun"), TEXT("Layer_SpaceSun"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_SpaceGround");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_SpaceGround"), TEXT("Layer_SpaceGround"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_SpaceCliff");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_SpaceCliff"), TEXT("Layer_SpaceSky"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_SpaceStage");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_SpaceStage"), TEXT("Layer_SpaceStage"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_SpaceEarth");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_SpaceEarth"), TEXT("Layer_Space_Earth"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_SpaceEarth_Light");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_SpaceEarth_Light"), TEXT("Layer_SpaceEarth_Light"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_FallingStar");

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_FallingStar"), TEXT("Layer__FallingStar"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_SpaceMoon");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_SpaceMoon"), TEXT("Layer_SpaceMoon"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_SpaceStone");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_SpaceStone"), TEXT("Layer_SpaceStone"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_SpaceRock");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_SpaceRock"), TEXT("Layer_SpaceRock"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_SpaceMeteoBreak");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_SpaceMeteoBreak"), TEXT("Layer_MeteoBreak"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_SpaceEF");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_SpaceEF"), TEXT("Layer_MeteoBreakA"), &tDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Volcano()
{
	CMap_Manager::Map_Object_Key tDesc{};
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_VolcanoEF");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_VolcanoEF"), TEXT("Layer_VolcanoStage"), &tDesc)))
		return E_FAIL;

	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_Volcano_Stage");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Volcano_Stage"), TEXT("Layer_VolcanoStage"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_Volcano_Cliff_Back");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Volcano_Cliff_Back"), TEXT("Layer_VolcanoStage"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_Volcano_Cliff_Far");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Volcano_Cliff_Far"), TEXT("Layer_VolcanoStage"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_Volcano_Cliff04");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Volcano_Cliff04"), TEXT("Layer_VolcanoStage"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_Volcano_Cliff03");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Volcano_Cliff03"), TEXT("Layer_VolcanoStage"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_Volcano_Cliff02");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Volcano_Cliff02"), TEXT("Layer_VolcanoStage"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_Volcano_Cliff01");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Volcano_Cliff01"), TEXT("Layer_VolcanoStage"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_Volcano_Ground");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Volcano_Ground"), TEXT("Layer_VolcanoStage"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_Volcano_GroundRock");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Volcano_GroundRock"), TEXT("Layer_VolcanoStage"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_Volcano_Island01");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Volcano_Island01"), TEXT("Layer_VolcanoStage"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_Volcano_Island02");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Volcano_Island02"), TEXT("Layer_VolcanoStage"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_Volcano_Lava_Pool");

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Volcano_Lava_Pool"), TEXT("Layer_VolcanoStage"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_Volcano_Lava_Fall");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Volcano_Lava_Fall"), TEXT("Layer_VolcanoStage"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_Volcano_Lava_Ground");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Volcano_Lava_Ground"), TEXT("Layer_VolcanoStage"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_Volcano_SkyCloud");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Volcano_SkyCloud"), TEXT("Layer_VolcanoStage"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_Volcano_Smoke");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Volcano_Smoke"), TEXT("Layer_VolcanoStage"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_Volcano_Mountain");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Volcano_Mountain"), TEXT("Layer_VolcanoStage"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_Volcano_Destructive");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Volcano_Destructive"), TEXT("Layer_VolcanoStage"), &tDesc)))
		return E_FAIL;





	return S_OK;
}

void CLevel_GamePlay::Create_Effect_Manager()
{
	m_pEffect_Manager = CEffect_Manager::Get_Instance();
	m_pEffect_Manager->Initialize(m_pDevice, m_pContext);
}

void CLevel_GamePlay::Create_QTE_Manager()
{
	m_pQTE_Manager = CQTE_Manager::Get_Instance();
	m_pQTE_Manager->Initialize(m_pDevice, m_pContext);
}

void CLevel_GamePlay::Create_Particle_Manager()
{
	m_pParticle_Manager = CParticle_Manager::Get_Instance();
	m_pParticle_Manager->Initialize(m_pDevice, m_pContext);
}

void CLevel_GamePlay::Create_SubTitle_Manager()
{
	m_pSubTitle_Manager = CSubTitle_Manager::Get_Instance();
	m_pSubTitle_Manager->Initialize(m_pDevice, m_pContext);
}

HRESULT CLevel_GamePlay::Loading_For_Effect()
{
	vector<EFFECT_LAYER_DATA>* pLoaded = static_cast<vector<EFFECT_LAYER_DATA>*>(m_pGameInstance->Load_All_Effects());

	return CEffect_Manager::Get_Instance()->Set_Saved_Effects(pLoaded);
}

void CLevel_GamePlay::Change_Level_ForCharacter()
{
	m_bLevelChange = true;
}

HRESULT CLevel_GamePlay::Ready_Character()
{
	CCharacter::Character_DESC SlotDesc = {};

	//플레이어 생성
	SlotDesc.ePlayerSlot = CUI_Define::LPLAYER1;
	SlotDesc.fSpeedPerSec = 5.f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Play_Goku"), TEXT("Layer_Character"), &SlotDesc)))
		return E_FAIL;

	//SlotDesc.ePlayerSlot = CUI_Define::LPLAYER2;
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Play_21"), TEXT("Layer_Character"), &SlotDesc)))
	//	return E_FAIL;



	//2팀

	SlotDesc.ePlayerSlot = CUI_Define::RPLAYER1;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Play_21"), TEXT("Layer_Character"), &SlotDesc)))
		return E_FAIL;




	m_pUI_Manager->InitUIObject();

	return S_OK;
}

CLevel_GamePlay* CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_GamePlay* pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_GamePlay"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();

	CFrameEvent_Manager::Destroy_Instance();
	Safe_Release(m_pEffect_Manager);

	Safe_Release(m_pQTE_Manager);
	Safe_Release(m_pUI_Manager);
	Safe_Release(m_pMap_Manager);
	Safe_Release(m_pParticle_Manager);
	Safe_Release(m_pSubTitle_Manager);
	Safe_Release(m_pIMGUI_Manager);
}
