#include "stdafx.h"
#include "Level_Stage3.h"

#include "GameInstance.h"

#include "Client_Manager.h"

#include "Camera_Manager.h"
#include "UI_Manager.h"

#include "Data_Manager.h"
#include "PhysX_Trigger.h"


#include "Level_Loading.h"
#include "ObjPool_Manager.h"
#include "UI_Rect.h"
#include "Utility_File.h"

#include "AnimMapObject.h"
#include "Event_Manager.h"
#include "../Public/Boss/AirBurster/State/State_AirBurster_Dead.h"


#include "Camera_Manager.h"

CLevel_Stage3::CLevel_Stage3(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Stage3::Initialize()
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

#if NEXT_STAGE == 3

	if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_PLAYER, TEXT("Prototype_GameObject_Cloud"))))
		RETURN_EFAIL;
	if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_PLAYER, TEXT("Prototype_GameObject_Aerith"))))
		RETURN_EFAIL;

	if (!m_pGameInstance->IsVisitedLevel(LEVEL_STAGE3))
	{
		if (FAILED(CCamera_Manager::GetInstance()->Initialize(m_pDevice, m_pContext)))
			RETURN_EFAIL;
	}
	else
	{
		GET_SINGLE(CCamera_Manager)->Add_GameInstance(m_pDevice, m_pContext);

		GET_SINGLE(CCamera_Manager)->Set_Target_ThirdPersonCamera(CLOUD, GET_SINGLE(CClient_Manager)->GET_SINGLE(CClient_Manager)->Get_PlayerPtr(static_cast<PLAYER_TYPE>(CLOUD)).lock()->Get_TransformCom().lock());
	}

	m_pGameInstance->Set_VisitedLevel(LEVEL_STAGE3);

#endif

	GET_SINGLE(CUI_Manager)->ALL_Reset_UI();

	if (FAILED(CUI_Manager::GetInstance()->Initialize(m_pDevice, m_pContext)))
		RETURN_EFAIL;

#if NEXT_STAGE != 3

	GET_SINGLE(CClient_Manager)->Add_Player_In_Layer(LEVEL_STAGE3, _float3(179.f, 6.f, 99.5f), _float3(179.f, 6.f, 98));
	// 플레이어 위치 잡는 코드
	GET_SINGLE(CCamera_Manager)->Add_Camera_in_Layer(LEVEL_STAGE3);

#endif

	/*GET_SINGLE(CCamera_Manager)->Get_ThirdPersonCamera(CLOUD).lock()->Set_OffsetSetType(CCamera_ThirdPerson::OFFSET_AIRBURSTER);
	GET_SINGLE(CCamera_Manager)->Get_ThirdPersonCamera(AERITH).lock()->Set_OffsetSetType(CCamera_ThirdPerson::OFFSET_AIRBURSTER);*/

	GET_SINGLE(CEffect_Manager)->Create_Map_Effect<CEffect_Group>(L"Stage1_1");
	GET_SINGLE(CEffect_Manager)->Create_Map_Effect<CParticle>(L"Boss1_Stage");

	GET_SINGLE(CClient_Manager)->Set_BattleMode_Natural(false);

	m_pGameInstance->Stop_BGM();

	return S_OK;
}

void CLevel_Stage3::Tick(_cref_time fTimeDelta)
{
	CClient_Manager::GetInstance()->Tick_Client();

	CCamera_Manager::GetInstance()->Tick(fTimeDelta);
	CUI_Manager::GetInstance()->Tick(fTimeDelta);

	if (m_bAddTime)
		m_fTimeAcc += fTimeDelta;


#ifdef _DEBUG
	m_pGameInstance->Render_FPS(fTimeDelta, g_hWnd, TEXT("[GAMEPLAY_LEVEL] "));

#endif

	if (m_pGameInstance->Key_Down(DIK_RBRACKET))
	{
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_VILLAGE2))))
			return;
		return;
	}

	if (m_pGameInstance->Key_Down(DIK_LBRACKET))
	{
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_STAGE2))))
			return;
		return;
	}

#if NEXT_STAGE == 3

	if (m_pGameInstance->Key_DownEx(DIK_F12, DIK_MD_LSHIFT))
	{
		GET_SINGLE(CCamera_Manager)->Add_Camera_in_Layer(LEVEL_STAGE3);
	}

#endif


	if (m_bNextLevel)
	{
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_STAGE2))))
			return;
		return;
	}
}

void CLevel_Stage3::Late_Tick(_cref_time fTimeDelta)
{
	if (m_fTimeAcc >= 2.f && m_bOnce)
	{
		auto pObject = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_ANIMOBJECT, 4);
		pObject->Get_ModelCom().lock()->Set_Animation("Main|N_Open_1", 1.1f, false);
		dynamic_pointer_cast<CAnimMapObject>(pObject)->Set_IsAnimPlaying(true);
		m_bOnce = false;
	}
	CUI_Manager::GetInstance()->Tick(fTimeDelta);
}

HRESULT CLevel_Stage3::Render()
{
	if (FAILED(CCamera_Manager::GetInstance()->Render()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CLevel_Stage3::Ready_Light()
{
	m_pGameInstance->Reset_Light();
	m_pGameInstance->Add_Light("../Bin/Data/Light_Data/Stage1/Light_Stage1_ALL_ver5.json");

	return S_OK;
}

HRESULT CLevel_Stage3::Ready_Layer_Environment(const LAYERTYPE& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), strLayerTag, TEXT("Prototype_GameObject_SkyBox_Test"))))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CLevel_Stage3::Ready_Layer_Object(const LAYERTYPE& strLayerTag)
{
	/* =================Stage1=============== 삭제금지*/

	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/Map_Data/Stage1/New/Stage1Fin_V1.json", LEVEL_STAGE3, strLayerTag)))
		RETURN_EFAIL;

	if (FAILED(CData_Manager::GetInstance()->Load_InstanceData("../Bin/Data/Instance_Data/Stage1/Stage1Fin_V2.InstBin", LEVEL_STAGE3)))
		RETURN_EFAIL;

	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/AnimObject_Data/Object/Stage3/Stage3Fin_V1.json", LEVEL_STAGE3, L_ANIMOBJECT)))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CLevel_Stage3::Ready_Layer_Player(const LAYERTYPE& strLayerTag)
{

#if NEXT_STAGE == 3

#endif

	return S_OK;
}

HRESULT CLevel_Stage3::Ready_Layer_Monster(const LAYERTYPE& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_MONSTER, TEXT("Prototype_GameObject_AirBurster"))))
		return S_OK;

	m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_MONSTER, 0)->TurnOff_State(OBJSTATE::Active);


	return S_OK;
}

HRESULT CLevel_Stage3::Ready_Layer_Trigger(const LAYERTYPE& strLayerTag)
{
	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/Trigger_Data/Stage3/Object/Stage3Fin_V1.json", LEVEL_STAGE3, strLayerTag)))
		RETURN_EFAIL;

	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/Trigger_Data/Stage3/UI/Stage3_UITrigger.json", LEVEL_STAGE3, strLayerTag)))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CLevel_Stage3::Ready_Layer_NPC(const LAYERTYPE& strLayerTag)
{
	return S_OK;
}


HRESULT CLevel_Stage3::Ready_Layer_Effect(const LAYERTYPE& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Stage3::Ready_Layer_Camera(const LAYERTYPE& strLayerTag)
{

	return S_OK;
}

HRESULT CLevel_Stage3::Ready_Layer_UI(const LAYERTYPE& strLayerTag)
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

	pUI_Manager->Make_Dialog();
	pUI_Manager->Make_MouseCursor();
	pUI_Manager->Set_CursorUI_OnOff(false);

	pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_MapTitle"), nullptr, nullptr, nullptr, "MapTitle_S1_2");
	
	pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_LockOnMarker"), &(PosTemp = { 0.f, -10.f, 0.f }), &m_pCreateUIObject);

	if (m_pCreateUIObject != nullptr)
		pUI_Manager->Set_LockOnMarker(m_pCreateUIObject);

	return S_OK;
}

HRESULT CLevel_Stage3::Ready_Layer_Collider(const LAYERTYPE& strLayerTag)
{
	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/Collider_Data/Stage3/Stage3Fin_V1.json", LEVEL_STAGE3, strLayerTag)))
		RETURN_EFAIL;

	return S_OK;
}

void CLevel_Stage3::Ready_Event()
{
	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "Elevator_Open", LEVELEVENT_MAKE{
		if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
		{
			m_bAddTime = true;

			m_pGameInstance->Erase_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "Elevator_Open");
		}
		return true;

		});

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "Boss_Start", LEVELEVENT_MAKE{
		if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
		{
			auto pObject = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_MONSTER, 0);

			pObject->TurnOn_State(OBJSTATE::Active);
			GET_SINGLE(CUI_Manager)->Call_Dialog("U_Dialog_S3_D0_");

			m_pGameInstance->Erase_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "Boss_Start");
		}
		return true;
		});

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "Close_Door", LEVELEVENT_MAKE{
		if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
		{
			auto pObject = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_ANIMOBJECT, 0);

			pObject->Get_ModelCom().lock()->Set_Animation("Door2|N_Open_1", 1.3f, false, false, 1.2f, true);
			dynamic_pointer_cast<CAnimMapObject>(pObject)->Set_IsAnimPlaying(true);

			m_pGameInstance->Erase_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "Close_Door");
		}
		return true;
		});

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "Next_Stage", LEVELEVENT_MAKE{

		auto pBoss = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_MONSTER,0);
	if (typeid(pBoss->Get_StateMachineCom().lock()->Get_CurState()) == typeid(CState_AirBurster_Dead))
	{
		if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
		{
			GET_SINGLE(CUI_Manager)->Call_Dialog("U_Dialog_S3_D2_");
			m_bNextLevel = true;
		}
	}
		return true;
		});

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "1stAuto_Door", LEVELEVENT_MAKE
		{
		auto pObject = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_ANIMOBJECT, 3);
		if (pOther != nullptr)
		{
			if (GET_SINGLE(CClient_Manager)->Get_PlayerPtr().lock() == pOther)
			{
				if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
				{
					pObject->Get_ModelCom().lock()->Set_Animation("Main|N_Open_1", 1.2f, false);
					dynamic_pointer_cast<CAnimMapObject>(pObject)->Set_IsAnimPlaying(true);
				}
				else if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::EXIT)
				{
					pObject->Get_ModelCom().lock()->Set_Animation("Main|N_Close_1", 1.2f, false);
					dynamic_pointer_cast<CAnimMapObject>(pObject)->Set_IsAnimPlaying(true);
				}
			}
		}
		return true;
		});

	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "2stAuto_Door", LEVELEVENT_MAKE
		{
			auto pObject = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_ANIMOBJECT, 2);
			if (pOther != nullptr)
			{
				if (GET_SINGLE(CClient_Manager)->Get_PlayerPtr().lock() == pOther)
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
}

CLevel_Stage3* CLevel_Stage3::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)

{
	CLevel_Stage3* pInstance = new CLevel_Stage3(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Stage3");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Stage3::Free()
{
	__super::Free();
}