#include "stdafx.h"
#include "Level_Stage1.h"

#include "GameInstance.h"

#include "Client_Manager.h"

#include "Camera_Manager.h"
#include "UI_Manager.h"

#include "Data_Manager.h"
#include "AnimMapObject.h"
#include "PhysX_Trigger.h"

#include "Level_Loading.h"
#include "ObjPool_Manager.h"
#include "UI_Rect.h"
#include "UI_DamageFont.h"
#include "Utility_File.h"

#include "State_Cloud_Event_PassOver.h"
#include "Effect_Manager.h"	

#include "Player.h"

#include "Camera_ThirdPerson.h"
#include "Event_Manager.h"
#include "State_List_Cloud.h"


CLevel_Stage1::CLevel_Stage1(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Stage1::Initialize()
{
	__super::Initialize();

#ifndef _DEBUG
	::ShowCursor(FALSE);
#endif

#if LEVEL_SOUND == 0
	m_pGameInstance->Reset_SoundPoints();
	m_pGameInstance->Stop_SoundAll();
	m_pGameInstance->Add_SoundPoint(m_pGameInstance->MainDataPath() + L"SoundPoint_Data/Stage1_1.json");
#endif

#if LEVEL_LIGHT != 0

	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.strLightName = "Directional_Stage1";
	LightDesc.vDirection = _float4(0.f, -1.f, 1.f, 0.f);
	LightDesc.vPosition = _float4(0.f, 0.f, 0.f, 1.f);
	LightDesc.fRange = 4.f;
	LightDesc.vDiffuse = _float4(0.35f, 0.35f, 0.35f, 0.5f);
	LightDesc.vAmbient = _float4(0.8f, 0.8f, 0.8f, 0.5f);
	LightDesc.vSpecular = _float4(0.1f, 0.1f, 0.1f, 0.5f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc, &m_pLight)))
		RETURN_EFAIL;

#endif



#if NEXT_STAGE == 0

	GET_SINGLE(CClient_Manager)->Add_Player_In_Layer(LEVEL_STAGE1, _float3(-40.f, 5.f, 14.f), _float3(-40, 5.f, 10.f));

	GET_SINGLE(CCamera_Manager)->Add_Camera_in_Layer(LEVEL_STAGE1);

#elif NEXT_STAGE == 1

	GET_SINGLE(CClient_Manager)->Add_Player_In_Layer(LEVEL_STAGE1, _float3(-40.f, 5.f, 14.f), _float3(-40, 5.f, 10.f));

	if (!m_pGameInstance->IsVisitedLevel(LEVEL_STAGE1))
	{
		if (FAILED(CCamera_Manager::GetInstance()->Initialize(m_pDevice, m_pContext)))
			RETURN_EFAIL;
	}
	else
	{
		GET_SINGLE(CCamera_Manager)->Add_GameInstance(m_pDevice, m_pContext);

		GET_SINGLE(CCamera_Manager)->Set_Target_ThirdPersonCamera(CLOUD, GET_SINGLE(CClient_Manager)->GET_SINGLE(CClient_Manager)->Get_PlayerPtr(static_cast<PLAYER_TYPE>(CLOUD)).lock()->Get_TransformCom().lock());
	}

#else
	GET_SINGLE(CClient_Manager)->Add_Player_In_Layer(LEVEL_STAGE1, _float3(-40.f, 5.f, 14.f), _float3(-40, 5.f, 10.f));

#endif

	if (FAILED(CUI_Manager::GetInstance()->Initialize(m_pDevice, m_pContext)))
		RETURN_EFAIL;

	m_pGameInstance->Play_BGM(TEXT("FF7_BGM"), TEXT("bgm_mako5_05#5 (Music-Section3).mp3"), 1.f);

	m_pGameInstance->Set_VisitedLevel(LEVEL_STAGE1);



#if LEVEL_STAGE1_TEST_MAP == 0 // 원래 스테이지

	GET_SINGLE(CEffect_Manager)->Create_Map_Effect<CEffect_Group>(L"Stage1_1");

#elif LEVEL_STAGE1_TEST_MAP == 97

	GET_SINGLE(CCamera_Manager)->Get_ThirdPersonCamera(CLOUD).lock()->Set_OffsetSetType(CCamera_ThirdPerson::OFFSET_BAHAMUT);
	GET_SINGLE(CCamera_Manager)->Get_ThirdPersonCamera(AERITH).lock()->Set_OffsetSetType(CCamera_ThirdPerson::OFFSET_BAHAMUT);

	GET_SINGLE(CEffect_Manager)->Create_Map_Effect<CParticle>(L"Boss2_Stage");

#elif LEVEL_STAGE1_TEST_MAP == 99

	GET_SINGLE(CCamera_Manager)->Get_ThirdPersonCamera(CLOUD).lock()->Set_OffsetSetType(CCamera_ThirdPerson::OFFSET_AIRBURSTER);
	GET_SINGLE(CCamera_Manager)->Get_ThirdPersonCamera(AERITH).lock()->Set_OffsetSetType(CCamera_ThirdPerson::OFFSET_AIRBURSTER);

	GET_SINGLE(CClient_Manager)->Set_PlayerPos(_float3( 84.f, 3.f, 79.2f),_float3(84.f, 3.f, 84.2f));

#endif

	GET_SINGLE(CClient_Manager)->Set_BattleMode_Natural(false);

	GET_SINGLE(CClient_Manager)->Get_PlayerPtr(AERITH).lock()->TurnOff_State(OBJSTATE::Tick);
	GET_SINGLE(CClient_Manager)->Get_PlayerPtr(AERITH).lock()->TurnOff_State(OBJSTATE::Render);

	GET_SINGLE(CClient_Manager)->Set_BattleMode(true);

	return S_OK;
}

void CLevel_Stage1::Tick(_cref_time fTimeDelta)
{
	CClient_Manager::GetInstance()->Tick_Client();
	CCamera_Manager::GetInstance()->Tick(fTimeDelta);

	auto pCollider = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_COLLIDER, 216);
	if (pCollider)
		pCollider->TurnOff_State(OBJSTATE::Active);

#if LEVEL_STAGE1_TEST_MAP == 97
	if (m_pGameInstance->Key_DownEx(DIK_F8, DIK_MD_LSHIFT))
	{
		if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_MONSTER, TEXT("Prototype_GameObject_GuardHound"))))
			return;
	}

	if (m_pGameInstance->Key_DownEx(DIK_F9, DIK_MD_LSHIFT))
	{
		if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_MONSTER, TEXT("Prototype_GameObject_Bahamut"))))
			return;
	}


#elif LEVEL_STAGE1_TEST_MAP == 99

	if (m_pGameInstance->Key_DownEx(DIK_F9, DIK_MD_LSHIFT))
	{
		if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_MONSTER, TEXT("Prototype_GameObject_AirBurster"))))
			return;
	}

	if (m_pGameInstance->Key_DownEx(DIK_F12, DIK_MD_LSHIFT))
	{
		GET_SINGLE(CCamera_Manager)->Add_Camera_in_Layer(LEVEL_STAGE1);
	}

#endif

#ifdef _DEBUG
	m_pGameInstance->Render_FPS(fTimeDelta, g_hWnd, TEXT("[GAMEPLAY_LEVEL] "));
#endif

	if (m_pGameInstance->Key_Down(DIK_RBRACKET))
	{
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_STAGE2))))
			return;
		return;
	}
	if (m_pGameInstance->Key_Down(DIK_LBRACKET))
	{
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_VILLAGE1))))
			return;
		//::ShowCursor(TRUE);
		return;
	}

	if (m_bColliderDead)
	{
		auto pCollider = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_COLLIDER, 251);
		pCollider->TurnOff_State(OBJSTATE::Active);
	}

	if (m_bNextLevel)
	{
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_STAGE2))))
			return;
		return;
	}

}

void CLevel_Stage1::Late_Tick(_cref_time fTimeDelta)
{
	CUI_Manager::GetInstance()->Tick(fTimeDelta);
}

HRESULT CLevel_Stage1::Render()
{
	if (FAILED(CCamera_Manager::GetInstance()->Render()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Layer_Environment(const LAYERTYPE& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), strLayerTag, TEXT("Prototype_GameObject_SkyBox_Test"))))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Layer_Object(const LAYERTYPE& strLayerTag)
{
	/* =================Stage1=============== 삭제금지*/
#if LEVEL_STAGE1_TEST_MAP == 0 //|| LEVEL_STAGE1_TEST_MAP == 99
	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/Map_Data/Stage1/New/Stage1Fin_V2.json", LEVEL_STAGE1, strLayerTag)))
		RETURN_EFAIL;

	if (FAILED(CData_Manager::GetInstance()->Load_InstanceData("../Bin/Data/Instance_Data/Stage1/Stage1Fin_V2.InstBin", LEVEL_STAGE1)))
		RETURN_EFAIL;

	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/AnimObject_Data/Object/Stage1/Stage1Fin_V2.json", LEVEL_STAGE1, L_ANIMOBJECT)))
		RETURN_EFAIL;

	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/AnimObject_Data/Mob/Stage1Fin.json", LEVEL_STAGE1, L_MOB)))
		RETURN_EFAIL;



#elif LEVEL_STAGE1_TEST_MAP == 1
	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/Map_Data/Stage2/28.json", LEVEL_STAGE2, L_OBJECT)))
		RETURN_EFAIL;
	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/AnimObject_Data/Object/Stage2/Stage2_1.json", LEVEL_STAGE2, L_ANIMOBJECT)))
		RETURN_EFAIL;

#elif LEVEL_STAGE1_TEST_MAP == 2
	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/Map_Data/LadderTestMap.json", LEVEL_STAGE1, strLayerTag)))
		RETURN_EFAIL;


#elif LEVEL_STAGE1_TEST_MAP == 97
	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/Map_Data/Boss2/Boss2_4.json", LEVEL_STAGE1, strLayerTag)))
		RETURN_EFAIL;

#elif LEVEL_STAGE1_TEST_MAP == 98
	/*=======================TestStage=========================== 삭제금지*/
	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/Map_Data/EffectTestMap.json", LEVEL_STAGE1, strLayerTag)))
		RETURN_EFAIL;
	//if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/AnimObject_Data/Test/TestMob.json", LEVEL_STAGE1, L_MOB)))
	//	RETURN_EFAIL;
	//if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/AnimObject_Data/Test/TestNPC.json", LEVEL_STAGE1, L_NPC)))
	//	RETURN_EFAIL;

#elif LEVEL_STAGE1_TEST_MAP == 99
	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/Map_Data/Boss1TestMap.json", LEVEL_STAGE1, strLayerTag)))
		RETURN_EFAIL;
#endif

	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Light()
{
#if LEVEL_LIGHT == 0
	m_pGameInstance->Reset_Light();
	m_pGameInstance->Add_Light("../Bin/Data/Light_Data/Stage1/Light_Stage1_ALL_ver7.json");
#endif

	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Layer_Player(const LAYERTYPE& strLayerTag)
{

#if NEXT_STAGE == 1

	if (!m_pGameInstance->IsVisitedLevel(LEVEL_STAGE1))
	{
#if TEST_PLAYER == 0
		if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), strLayerTag, TEXT("Prototype_GameObject_Cloud"))))
			RETURN_EFAIL;
		if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), strLayerTag, TEXT("Prototype_GameObject_Aerith"))))
			RETURN_EFAIL;
#endif
	}
	else
	{
		if (GET_SINGLE(CClient_Manager)->Get_PlayerPtr().lock() != nullptr)
			GET_SINGLE(CClient_Manager)->Add_Player_In_Layer(LEVEL_STAGE1, _float3(-30.f, 5.f, 14.f), _float3(-34.f, 5.f, 14.f));
		else
		{
			if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), strLayerTag, TEXT("Prototype_GameObject_Cloud"))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), strLayerTag, TEXT("Prototype_GameObject_Aerith"))))
				RETURN_EFAIL;
		}

	}

#endif // 일시적으로 시작 스테이지를 스테이지 1로 맞췄습니다. 원래 순서로 하고 싶으면 NEXT_STAGE 이걸 0으로 만들어주세요.
	
	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Layer_Monster(const LAYERTYPE& strLayerTag)
{
#if MONSTER_ON == 0

#if LEVEL_STAGE1_TEST_MAP == 0
	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/AnimObject_Data/Monster/Stage1/Stage1Fin.json", LEVEL_STAGE1, strLayerTag)))
		RETURN_EFAIL;

#elif LEVEL_STAGE1_TEST_MAP == 97
	if (m_pGameInstance->Key_DownEx(DIK_F8, DIK_MD_LSHIFT))
	{
		if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), strLayerTag, TEXT("Prototype_GameObject_Bahamut"))))
			RETURN_EFAIL;
	}

#elif LEVEL_STAGE1_TEST_MAP == 98
	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/AnimObject_Data/Monster/test.json", LEVEL_STAGE1, strLayerTag)))
		RETURN_EFAIL;

#elif LEVEL_STAGE1_TEST_MAP == 99
	/*if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), strLayerTag, TEXT("Prototype_GameObject_AirBurster"))))
		RETURN_EFAIL;*/
#endif

#endif
	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Layer_NPC(const LAYERTYPE& strLayerTag)
{
	return S_OK;
}


HRESULT CLevel_Stage1::Ready_Layer_Effect(const LAYERTYPE& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Layer_Camera(const LAYERTYPE& strLayerTag)
{
	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "S1_CutScene1", LEVELEVENT_MAKE {
		if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
		{
			GET_SINGLE(CCamera_Manager)->Call_CutSceneCamera("Stage1_Arrive_Station", (_int)CCamera::tagCameraMotionData::INTERPOLATION_TYPE::INTER_LERP);

			m_pGameInstance->Erase_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "S1_CutScene1");
		}
		return true;
		});

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "S1_CutScene2", LEVELEVENT_MAKE {
		if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
		{
			GET_SINGLE(CCamera_Manager)->Call_CutSceneCamera("Stage1_Into_Mako", (_int)CCamera::tagCameraMotionData::INTERPOLATION_TYPE::INTER_LERP, nullptr, 3.f);

			m_pGameInstance->Erase_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "S1_CutScene2");
		}
		return true;
		});

		m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "Next_Stage", LEVELEVENT_MAKE{
	
	if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
	{
		m_bNextLevel = true;
	}
	return true;
		});

	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Layer_UI(const LAYERTYPE& strLayerTag)
{
	CUI_Manager* pUI_Manager = GET_SINGLE(CUI_Manager);
	shared_ptr<CGameObject> m_pCreateUIObject = nullptr;

	_float3 PosTemp = {};
	_int	PosDir = {};

	pUI_Manager->ALL_Reset_UI();

	pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_Command"));
	pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_HPBar"));
	pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_MainPopUp"));
	pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_KeyMenual"));

	pUI_Manager->Make_Dialog();
	pUI_Manager->Make_MouseCursor();
	pUI_Manager->Set_CursorUI_OnOff(false);

	//pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_FloorDir"), &(PosTemp = { 9.5f, 7.7f, 23.5f }), nullptr, &(PosDir = 1), "S1FloorDir1");
	pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_FloorDir"), &(PosTemp = { 30.5f, 7.7f, 32.5f }), nullptr, &(PosDir = 1), "S1FloorDir2");
	pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_FloorDir"), &(PosTemp = { 147.f, 3.8f, 91.f }), nullptr, &(PosDir = 0), "S1FloorDir3");
	pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_FloorDir"), &(PosTemp = { 79.92f, 4.54f, -3.86f }), nullptr, &(PosDir = 1), "S1FloorDir4");

	pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_EventActionMarker"), &(PosTemp = { -1.f, 8.2f, 47.9f }), nullptr, &(PosDir = 3), "1st_AC");
	pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_EventActionMarker"), &(PosTemp = { 32.2f, 9.f, 29.8f }), nullptr, &(PosDir = 1), "2st_AC");
	pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_EventActionMarker"), &(PosTemp = { 38.f, 6.05f, -5.f }), nullptr, &(PosDir = 3), "3st_AC");
	pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_EventActionMarker"), &(PosTemp = { 86.75f, 3.05f, 41.2f }), nullptr, &(PosDir = 0), "4st_AC");
	pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_EventActionMarker"), &(PosTemp = { 86.f, 3.9f, 64.5f }), nullptr, &(PosDir = 1), "5st_AC");
	pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_EventActionMarker"), &(PosTemp = { 144.5f, 5.1f, 92.2f }), nullptr, &(PosDir = 0), "6st_AC");
	pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_EventActionMarker"), &(PosTemp = { 161.4f, 6.2f, 103.f }), nullptr, &(PosDir = 1), "7st_AC");
	pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_EventActionMarker"), &(PosTemp = { 176.2f, 6.2f, 96.0f }), nullptr, &(PosDir = 1), "8st_AC");
	pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_EventActionMarker"), &(PosTemp = { 69.6f, 5.f, 20.2f }), nullptr, &(PosDir = 1), "9st_AC");

	pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_Tutorial"), nullptr, nullptr, nullptr, "Tutorial_01");
	pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_Tutorial"), nullptr, nullptr, nullptr, "Tutorial_02");
	pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_Tutorial"), nullptr, nullptr, nullptr, "Tutorial_03");
	pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_Tutorial"), nullptr, nullptr, nullptr, "Tutorial_04");

	pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_MapTitle"), nullptr, nullptr, nullptr, "MapTitle_S1");
	pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_MapTitle"), nullptr, nullptr, nullptr, "MapTitle_S1_2");
	pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_MapTitle"), nullptr, nullptr, nullptr, "MapTitle_S1_3");

	pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_LockOnMarker"), &(PosTemp = { 0.f, -10.f, 0.f }), &m_pCreateUIObject);

	if (m_pCreateUIObject != nullptr)
		pUI_Manager->Set_LockOnMarker(m_pCreateUIObject);

	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Layer_Collider(const LAYERTYPE& strLayerTag)
{
#if LEVEL_STAGE1_TEST_MAP == 0
	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/Collider_Data/Stage1/Stage1Fin_V2.json", LEVEL_STAGE1, strLayerTag)))
		RETURN_EFAIL;


#elif LEVEL_STAGE1_TEST_MAP == 1
	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/Collider_Data/Stage2/Stage2Fin2.json", LEVEL_STATIC, L_COLLIDER)))
		RETURN_EFAIL;

#elif LEVEL_STAGE1_TEST_MAP == 2
	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/Collider_Data/LadderTest.json", LEVEL_STATIC, L_COLLIDER)))
		RETURN_EFAIL;

#elif LEVEL_STAGE1_TEST_MAP == 97
	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/Collider_Data/BossStage/BossStageFin_V1.json", LEVEL_STAGE1, L_COLLIDER)))
		RETURN_EFAIL;


#elif LEVEL_STAGE1_TEST_MAP == 98
	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/Collider_Data/TestColl.json", LEVEL_STAGE1, L_COLLIDER)))
		RETURN_EFAIL;

#elif LEVEL_STAGE1_TEST_MAP == 99
	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/Collider_Data/Stage1/Stage1Fin_V2.json", LEVEL_STAGE1, L_COLLIDER)))
		RETURN_EFAIL;

#endif
	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Layer_Trigger(const LAYERTYPE& strLayerTag)
{
#if LEVEL_STAGE1_TEST_MAP == 0
	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/Trigger_Data/Stage1/Object/Stage1Fin.json", LEVEL_STAGE1, strLayerTag)))
		RETURN_EFAIL;

	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/Trigger_Data/Stage1/UI/Stage1UIFin_2.json", LEVEL_STAGE1, strLayerTag)))
		RETURN_EFAIL;

	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/Trigger_Data/Stage1/Camera/Stage1CutScene.json", LEVEL_STAGE1, strLayerTag)))
		RETURN_EFAIL;

	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/Trigger_Data/Stage1/Monster/Stage1Fin.json", LEVEL_STAGE1, strLayerTag)))
		RETURN_EFAIL; // 보스 테스트때문에 잠깐 주석

#elif LEVEL_STAGE1_TEST_MAP == 2
	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/Trigger_Data/Stage2/Monster/Stage2Fin.json", LEVEL_STAGE1, strLayerTag)))
		RETURN_EFAIL;

#elif LEVEL_STAGE1_TEST_MAP == 98
	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/Trigger_Data/Stage1/UI/test.json", LEVEL_STAGE1, strLayerTag)))
		RETURN_EFAIL;

#endif

	return S_OK;
}

CLevel_Stage1* CLevel_Stage1::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)

{
	CLevel_Stage1* pInstance = new CLevel_Stage1(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Stage1");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Stage1::Ready_Event()
{
	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "First_Door", LEVELEVENT_MAKE {
		auto pObject = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_ANIMOBJECT, 0);
		if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
		{
			pObject->Get_ModelCom().lock()->Set_Animation("Main|N_Open_1", 1.f, false);
			dynamic_pointer_cast<CAnimMapObject>(pObject)->Set_Collision(true);
		}
		else if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::EXIT)
		{
			dynamic_pointer_cast<CAnimMapObject>(pObject)->Set_Collision(false);
		}
		return true;
		});

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "Lift_Up", LEVELEVENT_MAKE {
		auto pObject = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_ANIMOBJECT, 1);
		if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
		{
			pObject->Get_ModelCom().lock()->Set_Animation("Lift|N_Up_1", 0.8f, false);
			dynamic_pointer_cast<CAnimMapObject>(pObject)->Set_Collision(true);
		}
		else if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::EXIT)
		{
			dynamic_pointer_cast<CAnimMapObject>(pObject)->Set_Collision(false);
		}
		return true;
		});

	//m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "Lift_Down", LEVELEVENT_MAKE {
	//	auto pObject = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_ANIMOBJECT, 1);
	//	if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
	//	{
	//		pObject->Get_ModelCom().lock()->Set_Animation("Main|N_Down_1", 0.6f, false);
	//		dynamic_pointer_cast<CAnimMapObject>(pObject)->Set_Collision(true);
	//	}
	//	else if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::EXIT)
	//	{
	//		dynamic_pointer_cast<CAnimMapObject>(pObject)->Set_Collision(false);
	//	}
	//	return true;
	//	});

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "Switch_Down", LEVELEVENT_MAKE {
		auto pObject = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_ANIMOBJECT, 2);
		if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
		{
			dynamic_pointer_cast<CAnimMapObject>(pObject)->Set_Collision(true);
		}
		else if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::EXIT)
		{
			dynamic_pointer_cast<CAnimMapObject>(pObject)->Set_Collision(false);
		}
		return true;
		});

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "Sec_Door", LEVELEVENT_MAKE {
		auto pObject = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_ANIMOBJECT, 5);
		if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
		{
			pObject->Get_ModelCom().lock()->Set_Animation("Main|N_Open_1", 1.f, false);
			dynamic_pointer_cast<CAnimMapObject>(pObject)->Set_Collision(true);
		}
		else if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::EXIT)
		{
			dynamic_pointer_cast<CAnimMapObject>(pObject)->Set_Collision(false);
		}
		return true;
		});

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "Thr_Door", LEVELEVENT_MAKE {
		auto pObject = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_ANIMOBJECT, 6);
		if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
		{
			pObject->Get_ModelCom().lock()->Set_Animation("Main|N_Open_1", 1.f, false);
			dynamic_pointer_cast<CAnimMapObject>(pObject)->Set_Collision(true);
		}
		else if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::EXIT)
		{
			dynamic_pointer_cast<CAnimMapObject>(pObject)->Set_Collision(false);
		}
		return true;
		});

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "Elevator", LEVELEVENT_MAKE {
		auto pObject = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_ANIMOBJECT, 7);
		if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
		{
			pObject->Get_ModelCom().lock()->Set_Animation("Main|N_Open_1",1.f, false);
			dynamic_pointer_cast<CAnimMapObject>(pObject)->Set_Collision(true);
		}
		else if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::EXIT)
		{
			dynamic_pointer_cast<CAnimMapObject>(pObject)->Set_Collision(false);
		}
		return true;
		});

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "First_ItemBox", LEVELEVENT_MAKE {
		auto pObject = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_ANIMOBJECT, 8);
		if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
		{
			pObject->Get_ModelCom().lock()->Set_Animation("TreasureBox|N_Treasuer01", 0.7f, false);
			dynamic_pointer_cast<CAnimMapObject>(pObject)->Set_Collision(true);

		}
		else if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::EXIT)
		{
			dynamic_pointer_cast<CAnimMapObject>(pObject)->Set_Collision(false);
		}
		return true;
		});

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "Sec_ItemBox", LEVELEVENT_MAKE {
		auto pObject = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_ANIMOBJECT, 9);
		if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
		{
			pObject->Get_ModelCom().lock()->Set_Animation("TreasureBox|N_Treasuer01", 0.7f, false);
			dynamic_pointer_cast<CAnimMapObject>(pObject)->Set_Collision(true);
		}
		else if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::EXIT)
		{
			dynamic_pointer_cast<CAnimMapObject>(pObject)->Set_Collision(false);
		}
		return true;
		});

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "Thr_ItemBox", LEVELEVENT_MAKE {
		auto pObject = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_ANIMOBJECT, 10);
		if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
		{
			pObject->Get_ModelCom().lock()->Set_Animation("TreasureBox|N_Treasuer01", 0.7f, false);
			dynamic_pointer_cast<CAnimMapObject>(pObject)->Set_Collision(true);
		}
		else if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::EXIT)
		{
			dynamic_pointer_cast<CAnimMapObject>(pObject)->Set_Collision(false);
		}
		return true;
		});

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "Door2_Open", LEVELEVENT_MAKE {
		auto pObject = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_ANIMOBJECT, 3);

		pObject->Get_ModelCom().lock()->Set_Animation("Door2|N_Open_1", 0.3f, false);
		dynamic_pointer_cast<CAnimMapObject>(pObject)->Set_IsAnimPlaying(true);

		return true;
		});

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "Ticket_In", LEVELEVENT_MAKE {
		auto pObject = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_PLAYER, 0);
		_int iSize = m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CreateLevelIndex(), L_MONSTER)->size();

		if (iSize <= 16)
		{
			pObject->Get_StateMachineCom().lock()->Set_State<CState_Cloud_Event_PassOver>();
			pObject->Get_TransformCom().lock()->Set_Look(_float3(0.f, 0.f, 1.f), _float3(0.f, 1.f, 0.f));
		}

		return true;
		});


	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "Sneakln", LEVELEVENT_MAKE{
	auto pObject = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_PLAYER, 0);
	auto pMonsterList = m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CreateLevelIndex(), L_MONSTER);

	if (9 >= pMonsterList->size() && dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
	{
		m_bColliderDead = true;
		pObject->Get_StateMachineCom().lock()->Set_State<CState_Cloud_SneakIn>();
		pObject->Get_TransformCom().lock()->Set_Look(_float3(1.f, 0.f, 0.f), _float3(0.f, 1.f, 0.f));
		pObject->Get_TransformCom().lock()->Set_Position(1.f, _float4(81.765f, 4.8f,-3.7f, 1.f));

		m_pGameInstance->Erase_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "Sneakln");
	}
	return true;
	});

	//	/*========================== 몬 스 터 트 리 거 ================================*/

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "1st_Monster_On", LEVELEVENT_MAKE {

		if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
		{
			auto pMonsterList = m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CreateLevelIndex(), L_MONSTER);
			for (auto& pMonster : *pMonsterList)
			{
				if (pMonster->Get_ObjectTag() == L"0" || pMonster->Get_ObjectTag() == L"1")
					pMonster->TurnOn_State(OBJSTATE::Active);
			}
			m_pGameInstance->Erase_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "1st_Monster_On");
		}
		return true;
		});

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "2st_Monster_On", LEVELEVENT_MAKE {
		if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
		{
			auto pMonsterList = m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CreateLevelIndex(), L_MONSTER);
			for (auto& pMonster : *pMonsterList)
			{
				if (pMonster->Get_ObjectTag() == L"2" || pMonster->Get_ObjectTag() == L"3")
					pMonster->TurnOn_State(OBJSTATE::Active);
			}
		}
		return true;
		});

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "3st_Monster_On", LEVELEVENT_MAKE {
		if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
		{
			auto pMonsterList = m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CreateLevelIndex(), L_MONSTER);
			for (auto& pMonster : *pMonsterList)
			{
				if (pMonster->Get_ObjectTag() == L"4" || pMonster->Get_ObjectTag() == L"5" || pMonster->Get_ObjectTag() == L"6")
					pMonster->TurnOn_State(OBJSTATE::Active);
			}
		}
		return true;
		});

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "4st_Monster_On", LEVELEVENT_MAKE {
		if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
		{
			auto pMonsterList = m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CreateLevelIndex(), L_MONSTER);
			for (auto& pMonster : *pMonsterList)
			{
				if (pMonster->Get_ObjectTag() == L"7" || pMonster->Get_ObjectTag() == L"8")
					pMonster->TurnOn_State(OBJSTATE::Active);

				if (pMonster != nullptr && pMonster->Get_ObjectTag() == L"8")
				{
					GET_SINGLE(CEffect_Manager)->Create_Effect<CTrail_Buffer>(L"ET_GuardHound_Tail_Trail", pMonster);
				}
			}
		}
		return true;
		});

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "5st_Monster_On", LEVELEVENT_MAKE {
		if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
		{
			auto pMonsterList = m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CreateLevelIndex(), L_MONSTER);
			for (auto& pMonster : *pMonsterList)
			{
				if (pMonster->Get_ObjectTag() == L"9" || pMonster->Get_ObjectTag() == L"10" || pMonster->Get_ObjectTag() == L"11")
					pMonster->TurnOn_State(OBJSTATE::Active);

				auto pPlayer = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_PLAYER, 0);
				pPlayer->Get_PhysXControllerCom().lock()->Enable_Gravity(true);
			}
		}
		return true;
		});

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "6st_Monster_On", LEVELEVENT_MAKE {
		if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
		{
			auto pMonsterList = m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CreateLevelIndex(), L_MONSTER);
			for (auto& pMonster : *pMonsterList)
			{
				if (pMonster->Get_ObjectTag() == L"12" || pMonster->Get_ObjectTag() == L"13" || pMonster->Get_ObjectTag() == L"14")
					pMonster->TurnOn_State(OBJSTATE::Active);

				if (pMonster != nullptr && pMonster->Get_ObjectTag() == L"14")
				{
					GET_SINGLE(CEffect_Manager)->Create_Effect<CTrail_Buffer>(L"ET_GuardHound_Tail_Trail", pMonster);
				}
			}
		}
		return true;
		});

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "7st_Monster_On", LEVELEVENT_MAKE {
		if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
		{
			auto pMonsterList = m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CreateLevelIndex(), L_MONSTER);
			for (auto& pMonster : *pMonsterList)
			{
				if (pMonster->Get_ObjectTag() == L"15" || pMonster->Get_ObjectTag() == L"16" || pMonster->Get_ObjectTag() == L"17" || pMonster->Get_ObjectTag() == L"18")
					pMonster->TurnOn_State(OBJSTATE::Active);
			}
		}
		return true;
		});

}


void CLevel_Stage1::Free()
{
	__super::Free();
}