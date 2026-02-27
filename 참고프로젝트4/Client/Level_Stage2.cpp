#include "stdafx.h"
#include "Level_Stage2.h"

#include "GameInstance.h"

#include "Client_Manager.h"
#include "Sound_Manager.h"
#include "Camera_Manager.h"
#include "UI_Manager.h"

#include "Data_Manager.h"
#include "PhysX_Trigger.h"

#include "Level_Loading.h"
#include "ObjPool_Manager.h"
#include "UI_Rect.h"
#include "Utility_File.h"

#include "AnimMapObject.h"
#include "Sweeper.h"
#include "Event_Manager.h"
#include "State_List_Cloud.h"
#include "Map_Laser.h"
#include "Player.h"
#include "Aerith/State/State_Aerith_AI_AttackMode.h";
#include "Aerith/State/State_Aerith_AI_MoveMode.h";

CLevel_Stage2::CLevel_Stage2(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Stage2::Initialize()
{
	__super::Initialize();

#ifndef _DEBUG
	::ShowCursor(FALSE);
#endif

#if LEVEL_SOUND == 0
	m_pGameInstance->Reset_SoundPoints();
	m_pGameInstance->Stop_SoundAll();
	m_pGameInstance->Add_SoundPoint(m_pGameInstance->MainDataPath() + L"SoundPoint_Data/Stage2_1.json");
#endif

	GET_SINGLE(CClient_Manager)->Add_Player_In_Layer(LEVEL_STAGE2, _float3(-6.2f, 2.5f, 0.f), _float3(-0.67f, 2.1f, 2.2f));

	GET_SINGLE(CCamera_Manager)->Add_Camera_in_Layer(LEVEL_STAGE2);

	GET_SINGLE(CClient_Manager)->Set_attachment(false);

	GET_SINGLE(CData_Manager)->Load_LaserData("../Bin/Data/Laser_Data/T1.json", &m_vecLaserPos);

	m_TimeChecker1 = FTimeChecker(2.5f);
	m_TimeChecker2 = FTimeChecker(2.8f);
	m_TimeChecker3 = FTimeChecker(2.4);
	m_TimeChecker4 = FTimeChecker(1.3f);
	m_TimeChecker5 = FTimeChecker(1.1f);
	m_TimeChecker6 = FTimeChecker(1.f);


	GET_SINGLE(CClient_Manager)->Get_PlayerPtr(AERITH).lock()->TurnOn_State(OBJSTATE::Tick);
	GET_SINGLE(CClient_Manager)->Get_PlayerPtr(AERITH).lock()->TurnOn_State(OBJSTATE::Render);

	GET_SINGLE(CClient_Manager)->Get_PlayerPtr(AERITH).lock()->Get_StateMachineCom().lock()->Set_State< CState_Aerith_AI_AttackMode>();
	
	GET_SINGLE(CClient_Manager)->Set_BattleMode_Natural(false);
				
	GET_SINGLE(CEffect_Manager)->Create_Map_Effect<CEffect_Group>(L"Stage2_2");

	m_pGameInstance->Play_BGM(TEXT("FF7_BGM"), TEXT("bgm_mako1_04_e3#1 (Music-Section0).mp3"), 1.f);

	GET_SINGLE(CCamera_Manager)->Get_ThirdPersonCamera(CLOUD).lock()->Set_OffsetSetType(CCamera_ThirdPerson::OFFSET_DEFAULT);
	GET_SINGLE(CCamera_Manager)->Get_ThirdPersonCamera(AERITH).lock()->Set_OffsetSetType(CCamera_ThirdPerson::OFFSET_DEFAULT);

	return S_OK;
}

void CLevel_Stage2::Tick(_cref_time fTimeDelta)
{
	CClient_Manager::GetInstance()->Tick_Client();

	CCamera_Manager::GetInstance()->Tick(fTimeDelta);
	CUI_Manager::GetInstance()->Tick(fTimeDelta);


#ifdef _DEBUG
	m_pGameInstance->Render_FPS(fTimeDelta, g_hWnd, TEXT("[GAMEPLAY_LEVEL] "));
#endif

	if (m_pGameInstance->Key_Down(DIK_RBRACKET))
	{
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_STAGE3))))
			return;

#ifndef _DEBUG
		::ShowCursor(FALSE);
#endif

		return;
	}
	if (m_pGameInstance->Key_Down(DIK_LBRACKET))
	{
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_STAGE1))))
			return;
		return;
	}

	if (m_pGameInstance->Key_Down(DIK_PAUSE))
	{
		auto pMonsterList = m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CreateLevelIndex(), L_MONSTER);

		if (10 <= pMonsterList->size())
		{
			for (auto& iter : *pMonsterList)
			{
				if (pMonsterList->size() > 9)
				{
					iter->Set_Dead();
				}
			}
		}
		else if (7 <= pMonsterList->size())
		{
			for (auto& iter : *pMonsterList)
			{
				if (pMonsterList->size() > 6)
				{
					iter->Set_Dead();
				}
			}
		}
		else if (4 <= pMonsterList->size())
		{
			for (auto& iter : *pMonsterList)
			{
				if (pMonsterList->size() > 3)
				{
					iter->Set_Dead();
				}
			}
		}
	}

	//test
	//if (m_pGameInstance->Key_Down(DIK_RETURN))
	//{
	//	GET_SINGLE(CUI_Manager)->Call_Dialog("U_Dialog_S2_D2_");
	//}


	if (m_bNextLevel)
	{
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_STAGE3))))
			return;
		return;
	}

	if (m_bAddTime)
		m_fTimeAcc += fTimeDelta;

	

}

void CLevel_Stage2::Late_Tick(_cref_time fTimeDelta)
{
	if (m_bFirstLaserOn)
	{
		if (m_TimeChecker1.Update(fTimeDelta))
		{
			for (_int i = 0; i < 5; ++i)
			{
				CMap_Laser::MAPLASERDESC tDesc = {};
				tDesc.vPos = m_vecLaserPos[i];
				tDesc.eType = (CMap_Laser::LaserType)0;
				tDesc.fRotaion = m_pGameInstance->RandomInt(-20, 20);
				m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_EFFECT, TEXT("Prototype_GameObject_Map_Laser"), &tDesc);
			}
		}

		if (m_TimeChecker2.Update(fTimeDelta))
		{
			for (_int i = 5; i < 10; ++i)
			{
				CMap_Laser::MAPLASERDESC tDesc = {};
				tDesc.vPos = m_vecLaserPos[i];
				tDesc.eType = (CMap_Laser::LaserType)0;
				tDesc.fRotaion = m_pGameInstance->RandomInt(-20, 20);
				m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_EFFECT, TEXT("Prototype_GameObject_Map_Laser"), &tDesc);
			}
		}

		if (m_TimeChecker3.Update(fTimeDelta))
		{
			for (_int i = 10; i < 15; ++i)
			{
				CMap_Laser::MAPLASERDESC tDesc = {};
				tDesc.vPos = m_vecLaserPos[i];
				tDesc.eType = (CMap_Laser::LaserType)0;
				tDesc.fRotaion = m_pGameInstance->RandomInt(-20, 20);
				m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_EFFECT, TEXT("Prototype_GameObject_Map_Laser"), &tDesc);
			}
		}
	}
	else if (m_bSecLaserOn)
	{
		if (m_TimeChecker1.Update(fTimeDelta))
		{
			for (_int i = 15; i < 21; ++i)
			{
				CMap_Laser::MAPLASERDESC tDesc = {};
				tDesc.vPos = m_vecLaserPos[i];
				tDesc.eType = (CMap_Laser::LaserType)1;
				tDesc.fRotaion = m_pGameInstance->RandomInt(-20, 20);
				m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_EFFECT, TEXT("Prototype_GameObject_Map_Laser"), &tDesc);
			}
		}

		if (m_TimeChecker2.Update(fTimeDelta))
		{
			for (_int i = 21; i < 29; ++i)
			{
				CMap_Laser::MAPLASERDESC tDesc = {};
				tDesc.vPos = m_vecLaserPos[i];
				tDesc.eType = (CMap_Laser::LaserType)1;
				tDesc.fRotaion = m_pGameInstance->RandomInt(-20, 20);
				m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_EFFECT, TEXT("Prototype_GameObject_Map_Laser"), &tDesc);
			}
		}

		if (m_TimeChecker3.Update(fTimeDelta))
		{
			for (_int i = 29; i < 54; ++i)
			{
				CMap_Laser::MAPLASERDESC tDesc = {};
				tDesc.vPos = m_vecLaserPos[i];
				tDesc.eType = (CMap_Laser::LaserType)1;
				tDesc.fRotaion = m_pGameInstance->RandomInt(-20, 20);
				m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_EFFECT, TEXT("Prototype_GameObject_Map_Laser"), &tDesc);
			}
		}

	}

	if (m_fTimeAcc >= 2.f && m_bOnce)
	{
		auto pObject = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_ANIMOBJECT, 34);
		pObject->Get_ModelCom().lock()->Set_Animation("Main|N_Open_1", 1.1f, false);
		dynamic_pointer_cast<CAnimMapObject>(pObject)->Set_IsAnimPlaying(true);
		m_bOnce = false;
	}

	CUI_Manager::GetInstance()->Tick(fTimeDelta);
}


HRESULT CLevel_Stage2::Render()
{
	if (FAILED(CCamera_Manager::GetInstance()->Render()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CLevel_Stage2::Ready_Light()
{
	m_pGameInstance->Reset_Light();
	m_pGameInstance->Add_Light("../Bin/Data/Light_Data/Stage2/Stage2LightFinal3.json");

	m_pGameInstance->Make_Light_On_Owner(static_pointer_cast<CGameObject>(GET_SINGLE(CClient_Manager)->Get_PlayerPtr(Client::CLOUD).lock()), _float4(0.22f, 0.22f, 0.22f, 1.f), 2.f, _float3(0.f, 1.826f, 0.f), false, _float4(0.5f, 0.5f, 0.5f, 1.f));
	m_pGameInstance->Make_Light_On_Owner(static_pointer_cast<CGameObject>(GET_SINGLE(CClient_Manager)->Get_PlayerPtr(Client::AERITH).lock()), _float4(0.22f, 0.22f, 0.22f, 1.f), 2.f, _float3(0.f, 1.826f, 0.f), false, _float4(0.5f, 0.5f, 0.5f, 1.f));
	return S_OK;
}


HRESULT CLevel_Stage2::Ready_Layer_Environment(const LAYERTYPE& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), strLayerTag, TEXT("Prototype_GameObject_SkyBox_Test"))))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CLevel_Stage2::Ready_Layer_Object(const LAYERTYPE& strLayerTag)
{
	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/Map_Data/Stage2/New/Stage2Fin_V5.json", LEVEL_STAGE2, strLayerTag)))
		RETURN_EFAIL;

	if (FAILED(CData_Manager::GetInstance()->Load_InstanceData("../Bin/Data/Instance_Data/Stage2/Stage2Fin_V3.InstBin", LEVEL_STAGE2)))
		RETURN_EFAIL;

	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/AnimObject_Data/Object/Stage2/Stage2_1.json", LEVEL_STAGE2, L_ANIMOBJECT)))
		RETURN_EFAIL;

	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/AnimObject_Data/Mob/Stage2Fin.json", LEVEL_STAGE2, L_MOB)))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CLevel_Stage2::Ready_Layer_Player(const LAYERTYPE& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Stage2::Ready_Layer_Monster(const LAYERTYPE& strLayerTag)
{
#if MONSTER_ON == 0

	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/AnimObject_Data/Monster/Stage2/Stage2Fin.json", LEVEL_STAGE2, strLayerTag)))
		RETURN_EFAIL;

#endif
	return S_OK;
}

HRESULT CLevel_Stage2::Ready_Layer_NPC(const LAYERTYPE& strLayerTag)
{
	return S_OK;
}


HRESULT CLevel_Stage2::Ready_Layer_Effect(const LAYERTYPE& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Stage2::Ready_Layer_Camera(const LAYERTYPE& strLayerTag)
{
	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "Meet_Sweeper", LEVELEVENT_MAKE{
		if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
		{
			GET_SINGLE(CCamera_Manager)->Call_CutSceneCamera("Stage2_See_Sweeper", (_int)CCamera::tagCameraMotionData::INTERPOLATION_TYPE::INTER_NONE);

			m_pGameInstance->Erase_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "Meet_Sweeper");
		}
		return true;
		});


	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "Into_InnerMako", LEVELEVENT_MAKE{
		if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
		{
			GET_SINGLE(CCamera_Manager)->Call_CutSceneCamera("Stage2_In_InnerMako", (_int)CCamera::tagCameraMotionData::INTERPOLATION_TYPE::INTER_NONE);

			m_pGameInstance->Erase_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "Into_InnerMako");
		}
		return true;
		});

	return S_OK;
}

HRESULT CLevel_Stage2::Ready_Layer_UI(const LAYERTYPE& strLayerTag)
{
	CUI_Manager* pUI_Manager = GET_SINGLE(CUI_Manager);
	shared_ptr<CGameObject> m_pCreateUIObject = nullptr;

	_float3 PosTemp = {};
	_int	PosDir = {};

	pUI_Manager->ALL_Reset_UI();

	if (FAILED(pUI_Manager->Initialize(m_pDevice, m_pContext)))
		RETURN_EFAIL;

	pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_Command"));
	pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_HPBar"));
	pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_MainPopUp"));
	pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_KeyMenual"));

	pUI_Manager->Make_EscapeTimer();
	pUI_Manager->Make_Dialog();
	pUI_Manager->Make_MouseCursor();
	pUI_Manager->Set_CursorUI_OnOff(false);

	pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_Tutorial"), nullptr, nullptr, nullptr, "Tutorial_05");

	pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_FloorDir"), &(PosTemp = { -102.7f, -32.27f, 33.69f }), nullptr, &(PosDir = 2), "S2_FloorDir_0");
	pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_FloorDir"), &(PosTemp = { -137.53f, -47.62f, 21.53f }), nullptr, &(PosDir = 0), "S2_FloorDir_1");
	pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_FloorDir"), &(PosTemp = { -99.87f, -65.63f, 50.14f }), nullptr, &(PosDir = 3), "S2_FloorDir_2");

	pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_LockOnMarker"), &(PosTemp = { 0.f, -10.f, 0.f }), &m_pCreateUIObject);

	if (m_pCreateUIObject != nullptr)
		pUI_Manager->Set_LockOnMarker(m_pCreateUIObject);

	return S_OK;
}

HRESULT CLevel_Stage2::Ready_Layer_Collider(const LAYERTYPE& strLayerTag)
{
	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/Collider_Data/Stage2/Stage2Fin_V2.json", LEVEL_STAGE2, L_COLLIDER)))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CLevel_Stage2::Ready_Layer_Trigger(const LAYERTYPE& strLayerTag)
{
	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/Trigger_Data/Stage2/Monster/Stage2Fin.json", LEVEL_STAGE2, strLayerTag)))
		RETURN_EFAIL;

	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/Trigger_Data/Stage2/Object/Stage2_Fin1.json", LEVEL_STAGE2, strLayerTag)))
		RETURN_EFAIL;

	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/Trigger_Data/Stage2/UI/Stage2TriggerUI.json", LEVEL_STAGE2, strLayerTag)))
		RETURN_EFAIL;

	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/Trigger_Data/Stage2/Camera/CameraTriggerFin.json", LEVEL_STAGE2, strLayerTag)))
		RETURN_EFAIL;

	return S_OK;
}

void CLevel_Stage2::Ready_Event()
{
	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "1st_Monster_On", LEVELEVENT_MAKE{


		if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
		{
			auto pDoor = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_ANIMOBJECT, 34);

			auto pMonsterList = m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CreateLevelIndex(), L_MONSTER);
			for (auto& pMonster : *pMonsterList)
			{
				if (pMonster->Get_ObjectTag() == L"0" || pMonster->Get_ObjectTag() == L"1" || pMonster->Get_ObjectTag() == L"2")
					pMonster->TurnOn_State(OBJSTATE::Active);
			}

			pDoor->Get_ModelCom().lock()->Set_Animation("Main|N_Close_1", 1.1f, false);
			dynamic_pointer_cast<CAnimMapObject>(pDoor)->Set_IsAnimPlaying(true);
			m_pGameInstance->Erase_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "1st_Monster_On");
		}
		return true;

		});

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "2st_Monster_On", LEVELEVENT_MAKE{


		if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
		{
			auto pObject = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_MONSTER, 3);
			auto pMonsterList = m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CreateLevelIndex(), L_MONSTER);
			for (auto& pMonster : *pMonsterList)
			{
				if (pMonster->Get_ObjectTag() == L"3")
					pMonster->TurnOn_State(OBJSTATE::Active);
			}

			m_pGameInstance->Erase_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "2st_Monster_On");
		}
		return true;
		});

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "3st_Monster_On", LEVELEVENT_MAKE{

		
	
		if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
		{
			auto pMonsterList = m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CreateLevelIndex(), L_MONSTER);
			for (auto& pMonster : *pMonsterList)
			{
				if (pMonster->Get_ObjectTag() == L"4" || pMonster->Get_ObjectTag() == L"5" || pMonster->Get_ObjectTag() == L"6" || pMonster->Get_ObjectTag() == L"7" || pMonster->Get_ObjectTag() == L"8")
					pMonster->TurnOn_State(OBJSTATE::Active);
			}

			m_pGameInstance->Erase_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "3st_Monster_On");
		}
		return true;
		});

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "4st_Monster_On", LEVELEVENT_MAKE{
		

		if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
		{
			auto pMonsterList = m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CreateLevelIndex(), L_MONSTER);
			for (auto& pMonster : *pMonsterList)
			{
				if (pMonster->Get_ObjectTag() == L"9" || pMonster->Get_ObjectTag() == L"10" || pMonster->Get_ObjectTag() == L"11" || pMonster->Get_ObjectTag() == L"12")
					pMonster->TurnOn_State(OBJSTATE::Active);
			}

			m_pGameInstance->Erase_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "4st_Monster_On");
		}
		return true;
		});

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "5st_Monster_On", LEVELEVENT_MAKE{
		

		if (GET_SINGLE(CClient_Manager)->Get_attachment() && dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
		{
			auto pMonsterList = m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CreateLevelIndex(), L_MONSTER);
			for (auto& pMonster : *pMonsterList)
			{
				if (pMonster->Get_ObjectTag() == L"15" || pMonster->Get_ObjectTag() == L"16" || pMonster->Get_ObjectTag() == L"17")
					pMonster->TurnOn_State(OBJSTATE::Active);
			}

			m_pGameInstance->Erase_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "5st_Monster_On");
		}
		return true;
		});

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "Emergency_Protocall", LEVELEVENT_MAKE{
	
		if (GET_SINGLE(CClient_Manager)->Get_attachment() && dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
		{
			auto pMonsterList = m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CreateLevelIndex(), L_MONSTER);
			for (auto& pMonster : *pMonsterList)
			{
				if (pMonster->Get_ObjectTag() == L"13" || pMonster->Get_ObjectTag() == L"14")
					pMonster->TurnOn_State(OBJSTATE::Active);
			}

			m_pGameInstance->Play_BGM(TEXT("FF7_BGM"), TEXT("bgm_plat4_01#3 (Music-Section1).mp3"), 1.f);

			m_pGameInstance->Reset_Light();
			m_pGameInstance->Add_Light("../Bin/Data/Light_Data/Stage2/Stage2LightOff2.json");

			m_pGameInstance->Make_Light_On_Owner(static_pointer_cast<CGameObject>(GET_SINGLE(CClient_Manager)->Get_PlayerPtr(Client::CLOUD).lock()), _float4(0.22f, 0.22f, 0.22f, 1.f), 2.f, _float3(0.f, 1.826f, 0.f), false, _float4(1.f, 1.f, 1.f, 1.f));
			m_pGameInstance->Make_Light_On_Owner(static_pointer_cast<CGameObject>(GET_SINGLE(CClient_Manager)->Get_PlayerPtr(Client::AERITH).lock()), _float4(0.22f, 0.22f, 0.22f, 1.f), 2.f, _float3(0.f, 1.826f, 0.f), false, _float4(1.f, 1.f, 1.f, 1.f));

			_float3 PosTemp = {};
			_int	PosDir = {};

			GET_SINGLE(CUI_Manager)->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_FloorDir"), &(PosTemp = { -102.19f, -77.74f, 50.052f }), nullptr, &(PosDir = 1), "S2_FloorDir_3");
			GET_SINGLE(CUI_Manager)->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_FloorDir"), &(PosTemp = { -137.53f, -59.50f, 24.02f }), nullptr, &(PosDir = 2), "S2_FloorDir_4");
			GET_SINGLE(CUI_Manager)->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_FloorDir"), &(PosTemp = { -102.61f, -44.33f, 31.08f }), nullptr, &(PosDir = 0), "S2_FloorDir_5");

			GET_SINGLE(CUI_Manager)->Call_Dialog("U_Dialog_S2_D4_");

			m_pGameInstance->Play_Sound(TEXT("FF7_Resident_UI"), L"056_SE_UI (UI075_Danger).wav", ESoundGroup::UI, 1.f);

			m_pGameInstance->Erase_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "Emergency_Protocall");
		}
		return true;
		});

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "Three_Sholdierst", LEVELEVENT_MAKE{
		
		
		if (GET_SINGLE(CClient_Manager)->Get_attachment() && dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
		{
			auto pMonsterList = m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CreateLevelIndex(), L_MONSTER);
			for (auto& pMonster : *pMonsterList)
			{
				if (pMonster->Get_ObjectTag() == L"18" || pMonster->Get_ObjectTag() == L"19" || pMonster->Get_ObjectTag() == L"20")
					pMonster->TurnOn_State(OBJSTATE::Active);
			}

			m_pGameInstance->Erase_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "Three_Sholdierst");
		}
		return true;
		});

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "Three_MonoDrive", LEVELEVENT_MAKE{
		
		
		if (GET_SINGLE(CClient_Manager)->Get_attachment() && dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
		{
			auto pMonsterList = m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CreateLevelIndex(), L_MONSTER);
			for (auto& pMonster : *pMonsterList)
			{
				if (pMonster->Get_ObjectTag() == L"21" || pMonster->Get_ObjectTag() == L"22" || pMonster->Get_ObjectTag() == L"23")
					pMonster->TurnOn_State(OBJSTATE::Active);
			}

			m_pGameInstance->Erase_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "Three_MonoDrive");
		}
		return true;
		});

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "1st_AutoDoorw", LEVELEVENT_MAKE{
		auto pObject = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_ANIMOBJECT, 0);
		auto pMonsterList = m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CreateLevelIndex(), L_MONSTER);
		if (pOther != nullptr)
		{
			if (20 >= pMonsterList->size() && GET_SINGLE(CClient_Manager)->Get_PlayerPtr().lock() == pOther)
			{
				if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
				{
					pObject->Get_ModelCom().lock()->Set_Animation("Main|N_Open_1", 1.3f, false);
					dynamic_pointer_cast<CAnimMapObject>(pObject)->Set_IsAnimPlaying(true);
				}
				else if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::EXIT)
				{
					pObject->Get_ModelCom().lock()->Set_Animation("Main|N_Close_1", 1.3f, false);
					dynamic_pointer_cast<CAnimMapObject>(pObject)->Set_IsAnimPlaying(true);
				}
			}
		}
		return true;
		});

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "2st_AutoDoorw", LEVELEVENT_MAKE
		{
			auto pObject = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_ANIMOBJECT, 1);
			auto pList = m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CreateLevelIndex(), L_MONSTER);
			if (pOther != nullptr)
			{
				if ((3 < pList->size() || pList->empty()) && GET_SINGLE(CClient_Manager)->Get_PlayerPtr().lock() == pOther)
				{
					if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
					{
						pObject->Get_ModelCom().lock()->Set_Animation("Main|N_Open_1", 1.3f, false);
						dynamic_pointer_cast<CAnimMapObject>(pObject)->Set_IsAnimPlaying(true);
					}
					else if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::EXIT)
					{
						pObject->Get_ModelCom().lock()->Set_Animation("Main|N_Close_1", 1.3f, false);
						dynamic_pointer_cast<CAnimMapObject>(pObject)->Set_IsAnimPlaying(true);
					}
				}

			}
			return true;
		});

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "Elevator_Open", LEVELEVENT_MAKE
		{
			auto pObject = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_ANIMOBJECT, 34);
			if (pOther != nullptr)
			{
				if (GET_SINGLE(CClient_Manager)->Get_attachment() && GET_SINGLE(CClient_Manager)->Get_PlayerPtr().lock() == pOther)
				{
					if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
					{
						pObject->Get_ModelCom().lock()->Set_Animation("Main|N_Open_1", 1.f, false);
						dynamic_pointer_cast<CAnimMapObject>(pObject)->Set_Collision(true);
					}
					else if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::EXIT)
					{
						dynamic_pointer_cast<CAnimMapObject>(pObject)->Set_Collision(false);
					}
				}
			}
			return true;
		});

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "Next_Stage", LEVELEVENT_MAKE{

	if (GET_SINGLE(CClient_Manager)->Get_attachment() && dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
	{
		m_bNextLevel = true;
	}
	else if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
	{
		m_bAddTime = true;
	}
	return true;
		});

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "Bwomb_attachment", LEVELEVENT_MAKE{

		if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
		{
			GET_SINGLE(CClient_Manager)->Set_attachment(true);

			m_pGameInstance->Erase_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "Bwomb_attachment");
		}
		return true;
		});

	/*플레이어 이벤트 연동*/
	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "1st_LadderStart", LEVELEVENT_MAKE{

	auto pObject = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_PLAYER, 0);

	if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
	{
		pObject->Get_StateMachineCom().lock()->Set_State<CState_Cloud_LadderDown>();
		pObject->Get_TransformCom().lock()->Set_Look(_float3(0.f, 0.f, -1.f), _float3(0.f, 1.f, 0.f));
		pObject->Get_TransformCom().lock()->Set_Position(1.f, _float4(-102.6f, -32.51f, 32.5f,1.f));

		m_pGameInstance->Erase_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "1st_LadderStart");
		GET_SINGLE(CClient_Manager)->Get_PlayerPtr(AERITH).lock()->TurnOff_State(OBJSTATE::Render);
	}
	return true;
		});

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "1st_LadderEnd", LEVELEVENT_MAKE{

	auto pObject = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_PLAYER, 0);

	if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
	{
		pObject->Get_StateMachineCom().lock()->Set_State<CState_Cloud_LadderDownFinished>();

		m_pGameInstance->Erase_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "1st_LadderEnd");
		GET_SINGLE(CClient_Manager)->Get_PlayerPtr(AERITH).lock()->TurnOn_State(OBJSTATE::Render);
		GET_SINGLE(CClient_Manager)->Get_PlayerPtr(AERITH).lock()->Get_TransformCom().lock()->Set_Position(1.f, _float3(-102.23f, -44.46f, 27.51f));
	}
	return true;
		});

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "2st_LadderStatr", LEVELEVENT_MAKE{
	auto pObject = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_PLAYER, 0);

	if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
	{
		pObject->Get_StateMachineCom().lock()->Set_State<CState_Cloud_LadderDown>();
		pObject->Get_TransformCom().lock()->Set_Look(_float3(0.f, 0.f, 1.f), _float3(0.f, 1.f, 0.f));
		pObject->Get_TransformCom().lock()->Set_Position(1.f, _float4(-137.51f, -47.7f, 22.95f,1.f));

		m_pGameInstance->Erase_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "2st_LadderStatr");
		GET_SINGLE(CClient_Manager)->Get_PlayerPtr(AERITH).lock()->TurnOff_State(OBJSTATE::Render);
	}
	return true;
		});

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "2st_LadderEnd", LEVELEVENT_MAKE{

	if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
	{
		auto pObject = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_PLAYER, 0);

		if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
		{
			pObject->Get_StateMachineCom().lock()->Set_State<CState_Cloud_LadderDownFinished>();

			m_pGameInstance->Erase_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "2st_LadderEnd");
			GET_SINGLE(CClient_Manager)->Get_PlayerPtr(AERITH).lock()->TurnOn_State(OBJSTATE::Render);
			GET_SINGLE(CClient_Manager)->Get_PlayerPtr(AERITH).lock()->Get_TransformCom().lock()->Set_Position(1.f, _float3(-137.6f, -59.48f, 31.19f));

		}
		return true;
	}
	return true;
		});

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "3st_LadderStart", LEVELEVENT_MAKE{
	auto pObject = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_PLAYER, 0);

	if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
	{
		pObject->Get_StateMachineCom().lock()->Set_State<CState_Cloud_LadderDown>();
		pObject->Get_TransformCom().lock()->Set_Look(_float3(-1.f, 0.f, 0.f), _float3(0.f, 1.f, 0.f));
		pObject->Get_TransformCom().lock()->Set_Position(1.f, _float4(-101.03f, -65.8f, 50.56f,1.f));

		m_pGameInstance->Erase_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "3st_LadderStart");
		GET_SINGLE(CClient_Manager)->Get_PlayerPtr(AERITH).lock()->TurnOff_State(OBJSTATE::Render);
	}
	return true;
		});

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "3st_LadderEnd", LEVELEVENT_MAKE{

	int a = 0;
	if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
	{
		auto pObject = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_PLAYER, 0);

		if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
		{
			pObject->Get_StateMachineCom().lock()->Set_State<CState_Cloud_LadderDownFinished>();
			GET_SINGLE(CUI_Manager)->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_MapTitle"), nullptr, nullptr, nullptr, "MapTitle_S2");

			m_pGameInstance->Erase_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "3st_LadderEnd");
			GET_SINGLE(CClient_Manager)->Get_PlayerPtr(AERITH).lock()->TurnOn_State(OBJSTATE::Render);
			GET_SINGLE(CClient_Manager)->Get_PlayerPtr(AERITH).lock()->Get_TransformCom().lock()->Set_Position(1.f, _float3(-109.179f, -77.49f, 50.57f));

		}
		return true;
	}
	return true;
		});

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "1Floor_LadderUp", LEVELEVENT_MAKE{

	auto pObject = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_PLAYER, 0);
	auto pMonsterList = m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CreateLevelIndex(), L_MONSTER);
	if (GET_SINGLE(CClient_Manager)->Get_attachment() && 3 >= pMonsterList->size() && dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
	{
		pObject->Get_StateMachineCom().lock()->Set_State<CState_Cloud_LadderUp>();
		pObject->Get_TransformCom().lock()->Set_Look(_float3(0.f, 0.f, 1.f), _float3(0.f, 1.f, 0.f));
		pObject->Get_TransformCom().lock()->Set_Position(1.f, _float4(-102.6f, -44.5f, 31.42f, 1.f));

		m_pGameInstance->Erase_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "1Floor_LadderUp");
		GET_SINGLE(CClient_Manager)->Get_PlayerPtr(AERITH).lock()->TurnOff_State(OBJSTATE::Render);
	}
	return true;
		});

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "1Floor_LadderUpFin", LEVELEVENT_MAKE{

	if (GET_SINGLE(CClient_Manager)->Get_attachment() && dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
	{
		auto pObject = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_PLAYER, 0);

		if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
		{
			pObject->Get_StateMachineCom().lock()->Set_State<CState_Cloud_LadderUpFinished>();

			m_pGameInstance->Erase_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "1Floor_LadderUpFin");
			GET_SINGLE(CClient_Manager)->Get_PlayerPtr(AERITH).lock()->TurnOn_State(OBJSTATE::Render);
			GET_SINGLE(CClient_Manager)->Get_PlayerPtr(AERITH).lock()->Get_TransformCom().lock()->Set_Position(1.f, _float3(-94.13f, -32.34f, 36.239f));

		}
		return true;
	}
	return true;
		});

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "2_LadderUp", LEVELEVENT_MAKE{
	auto pObject = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_PLAYER, 0);
	auto pMonsterList = m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CreateLevelIndex(), L_MONSTER);

	if (GET_SINGLE(CClient_Manager)->Get_attachment() && 6 >= pMonsterList->size() && dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
	{
		pObject->Get_StateMachineCom().lock()->Set_State<CState_Cloud_LadderUp>();
		pObject->Get_TransformCom().lock()->Set_Look(_float3(0.f, 0.f, -1.f), _float3(0.f, 1.f, 0.f));
		pObject->Get_TransformCom().lock()->Set_Position(1.f, _float4(-137.51f, -59.7f, 24.025f, 1.f));

		m_pGameInstance->Erase_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "2_LadderUp");
		GET_SINGLE(CClient_Manager)->Get_PlayerPtr(AERITH).lock()->TurnOff_State(OBJSTATE::Render);
	}
	return true;
		});

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "2_LadderUpFin", LEVELEVENT_MAKE{

		auto pObject = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_PLAYER, 0);

		if (GET_SINGLE(CClient_Manager)->Get_attachment() && dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
		{
			pObject->Get_StateMachineCom().lock()->Set_State<CState_Cloud_LadderUpFinished>();

			m_pGameInstance->Erase_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "2_LadderUpFin");
			GET_SINGLE(CClient_Manager)->Get_PlayerPtr(AERITH).lock()->TurnOn_State(OBJSTATE::Render);
			GET_SINGLE(CClient_Manager)->Get_PlayerPtr(AERITH).lock()->Get_TransformCom().lock()->Set_Position(1.f, _float3(-140.83f, -44.13f, 9.55f));

		}
		return true;
	return true;
		});

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "3_LadderUp", LEVELEVENT_MAKE{
	auto pObject = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_PLAYER, 0);
	auto pMonsterList = m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CreateLevelIndex(), L_MONSTER);

	if (GET_SINGLE(CClient_Manager)->Get_attachment() && 9 >= pMonsterList->size() && dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
	{
		pObject->Get_StateMachineCom().lock()->Set_State<CState_Cloud_LadderUp>();
		pObject->Get_TransformCom().lock()->Set_Look(_float3(1.f, 0.f, 0.f), _float3(0.f, 1.f, 0.f));
		pObject->Get_TransformCom().lock()->Set_Position(1.f, _float4(-102.1f, -77.78f, 50.56f, 1.f));

		m_pGameInstance->Erase_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "3_LadderUp");
		GET_SINGLE(CClient_Manager)->Get_PlayerPtr(AERITH).lock()->TurnOff_State(OBJSTATE::Render);
	}
	return true;
		});

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "3_LadderUpFin", LEVELEVENT_MAKE{

		auto pObject = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_PLAYER, 0);

		if (GET_SINGLE(CClient_Manager)->Get_attachment() && dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
		{
			pObject->Get_StateMachineCom().lock()->Set_State<CState_Cloud_LadderUpFinished>();

			m_pGameInstance->Erase_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "3_LadderUpFin");
			GET_SINGLE(CClient_Manager)->Get_PlayerPtr(AERITH).lock()->TurnOn_State(OBJSTATE::Render);
			GET_SINGLE(CClient_Manager)->Get_PlayerPtr(AERITH).lock()->Get_TransformCom().lock()->Set_Position(1.f, _float3(-98.84f, -65.63f, 58.68f));
		}
		return true;
	return true;
		});

	/*레이저 생성*/
	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "2Floor_LaserOn", LEVELEVENT_MAKE{

		if (!GET_SINGLE(CClient_Manager)->Get_attachment() && dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
		{
			m_bFirstLaserOn = true;
		}

		else if (GET_SINGLE(CClient_Manager)->Get_attachment() && dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::EXIT)
		{
			m_bFirstLaserOn = false;
		}
		return true;
	return true;
		});

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "2Floor_LaserOff", LEVELEVENT_MAKE{

		if (!GET_SINGLE(CClient_Manager)->Get_attachment() && dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
		{
			m_bFirstLaserOn = false;
			m_bSecLaserOn = true;
		}
		else if (GET_SINGLE(CClient_Manager)->Get_attachment() && dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
		{
			m_bFirstLaserOn = true;
			m_bSecLaserOn = false;
		}

		return true;
	return true;
		});

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "1Floor_LaserOff", LEVELEVENT_MAKE{

		if (!GET_SINGLE(CClient_Manager)->Get_attachment() && dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
		{
			m_bFirstLaserOn = false;
			m_bSecLaserOn = false;
		}
		else if (GET_SINGLE(CClient_Manager)->Get_attachment() && dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
		{
			m_bFirstLaserOn = false;
			m_bSecLaserOn = true;
		}

		return true;
	return true;
		});
}


CLevel_Stage2* CLevel_Stage2::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)

{
	CLevel_Stage2* pInstance = new CLevel_Stage2(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Stage2");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Stage2::Free()
{
	__super::Free();

}