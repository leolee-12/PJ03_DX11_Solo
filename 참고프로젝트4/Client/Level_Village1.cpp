#include "stdafx.h"
#include "Level_Village1.h"

#include "GameInstance.h"

#include "Client_Manager.h"

#include "Camera_Manager.h"
#include "UI_Manager.h"
#include "UI_EventActionMarker.h"

#include "Data_Manager.h"
#include "PhysX_Trigger.h"

#include "Level_Loading.h"
#include "ObjPool_Manager.h"
#include "UI_Rect.h"
#include "Utility_File.h"
#include "Event_Manager.h"
#include "Player.h"

CLevel_Village1::CLevel_Village1(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Village1::Initialize()
{
	__super::Initialize();

#ifndef _DEBUG
	::ShowCursor(FALSE);
#endif

#if LEVEL_SOUND == 0
	m_pGameInstance->Reset_SoundPoints();
	m_pGameInstance->Stop_SoundAll();
	m_pGameInstance->Add_SoundPoint(m_pGameInstance->MainDataPath() + L"SoundPoint_Data/Village1_1.json");
#endif

	if (!m_pGameInstance->IsVisitedLevel(LEVEL_VILLAGE1))
	{
		if (FAILED(CCamera_Manager::GetInstance()->Initialize(m_pDevice, m_pContext)))
			RETURN_EFAIL;
	}
	else
	{
		GET_SINGLE(CCamera_Manager)->Add_GameInstance(m_pDevice, m_pContext);

		GET_SINGLE(CCamera_Manager)->Set_Target_ThirdPersonCamera(CLOUD, GET_SINGLE(CClient_Manager)->GET_SINGLE(CClient_Manager)->Get_PlayerPtr(static_cast<PLAYER_TYPE>(CLOUD)).lock()->Get_TransformCom().lock());
	}

	if (FAILED(CUI_Manager::GetInstance()->Initialize(m_pDevice, m_pContext)))
		RETURN_EFAIL;


	GET_SINGLE(CClient_Manager)->Set_BattleMode_Natural(true);

	GET_SINGLE(CClient_Manager)->Get_PlayerPtr(AERITH).lock()->TurnOff_State(OBJSTATE::Tick);
	GET_SINGLE(CClient_Manager)->Get_PlayerPtr(AERITH).lock()->TurnOff_State(OBJSTATE::Render);

	m_pGameInstance->Play_BGM(TEXT("FF7_BGM"), TEXT("bgm_plat7_01 (Music-Section0).mp3"), 1.f);

	return S_OK;
}

void CLevel_Village1::Tick(_cref_time fTimeDelta)
{
	CClient_Manager::GetInstance()->Tick_Client();

	CCamera_Manager::GetInstance()->Tick(fTimeDelta);
	CUI_Manager::GetInstance()->Tick(fTimeDelta);

#ifdef _DEBUG
	m_pGameInstance->Render_FPS(fTimeDelta, g_hWnd, TEXT("[GAMEPLAY_LEVEL] "));

#endif
	if (m_pGameInstance->Key_Down(DIK_RBRACKET))
	{
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_STAGE1))))
			return;
		return;
	}
	if (m_pGameInstance->Key_Down(DIK_LBRACKET))
	{
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_TOOL))))
			return;
		return;
	}


	if (m_bNextLevel)
	{
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_STAGE1))))
			return;
		return;
	}

}

void CLevel_Village1::Late_Tick(_cref_time fTimeDelta)
{
	CUI_Manager::GetInstance()->Tick(fTimeDelta);
}

HRESULT CLevel_Village1::Render()
{
	if (FAILED(CCamera_Manager::GetInstance()->Render()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CLevel_Village1::Ready_Layer_Environment(const LAYERTYPE& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), strLayerTag, TEXT("Prototype_GameObject_SkyBox_Test"))))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CLevel_Village1::Ready_Layer_Object(const LAYERTYPE& strLayerTag)
{
	/*=======================TestStage=========================== 삭제금지*/
	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/Map_Data/Village/New/Village1Fin_V2.json", LEVEL_VILLAGE1, L_OBJECT)))
		RETURN_EFAIL;

	if (FAILED(CData_Manager::GetInstance()->Load_InstanceData("../Bin/Data/Instance_Data/Village1/Village1Fin_V5.InstBin", LEVEL_VILLAGE1)))
		RETURN_EFAIL;

	//if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/Map_Data/EffectTestMap.json", LEVEL_VILLAGE1, strLayerTag)))
	//	RETURN_EFAIL;

	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/AnimObject_Data/Object/Village1Fin/Village1Fin_V1.json", LEVEL_VILLAGE1, L_ANIMOBJECT)))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CLevel_Village1::Ready_Layer_Player(const LAYERTYPE& strLayerTag)
{
	if (!m_pGameInstance->IsVisitedLevel(LEVEL_VILLAGE1))
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
			GET_SINGLE(CClient_Manager)->Add_Player_In_Layer(LEVEL_VILLAGE1, _float3(15.f, 2.f, 0.f), _float3(15.f, 2.f, -5.f));
		else
		{
			if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), strLayerTag, TEXT("Prototype_GameObject_Cloud"))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), strLayerTag, TEXT("Prototype_GameObject_Aerith"))))
				RETURN_EFAIL;
		}

	}

	return S_OK;
}

HRESULT CLevel_Village1::Ready_Layer_Monster(const LAYERTYPE& strLayerTag)
{

	//if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/AnimObject_Data/Monster/test2.json", LEVEL_VILLAGE1, strLayerTag)))
	//	RETURN_EFAIL;

	return S_OK;
}

HRESULT CLevel_Village1::Ready_Layer_NPC(const LAYERTYPE& strLayerTag)
{
	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/AnimObject_Data/NPC/Village1NPC.json", LEVEL_VILLAGE1, L_NPC)))
		RETURN_EFAIL;

	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/AnimObject_Data/Mob/Village1.json", LEVEL_VILLAGE1, L_MOB)))
		RETURN_EFAIL;

	return S_OK;
}


HRESULT CLevel_Village1::Ready_Layer_Effect(const LAYERTYPE& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Village1::Ready_Layer_Camera(const LAYERTYPE& strLayerTag)
{
#if INTRO_CUTSCENE == 0
	//m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "IntroCutScene", LEVELEVENT_MAKE {
	//	if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
	//	{
	//		GET_SINGLE(CCamera_Manager)->Call_CutSceneCamera("Village1_Intro_Main", (_int)CCamera::tagCameraMotionData::INTERPOLATION_TYPE::INTER_LERP);
	//
	//		GET_SINGLE(CUI_Manager)->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_MapTitle"), nullptr, nullptr, nullptr, "MapTitle_V1");
	//
	//		m_pGameInstance->Erase_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "IntroCutScene");
	//	}
	//	return true;
	//	});
#endif

	return S_OK;
}

HRESULT CLevel_Village1::Ready_Layer_UI(const LAYERTYPE& strLayerTag)
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

	pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_FloorDir"), &(PosTemp = { -3.061f, 0.428f, 43.149f }), nullptr, &(PosDir = 1), "FloorDir1_UnderStair");
	pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_FloorDir"), &(PosTemp = { 9.506f, 5.126f, 30.853f }), nullptr, &(PosDir = 3), "FloorDir2_UpStair");
	pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_FloorDir"), &(PosTemp = { 75.29f, 1.91f, 21.65f }), nullptr, &(PosDir = 0));

	pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_MapTitle"), nullptr, nullptr, nullptr, "MapTitle_V1");

	pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_EventActionMarker"), &(PosTemp = { 18.9f, 1.494f, -41.286f }), &m_pCreateUIObject, &(PosDir = 1), "AcMarker01_Store");
	dynamic_pointer_cast<CUI_EventActionMarker>(m_pCreateUIObject)->Add_Action(ACTION_MAKE{
			//GET_SINGLE(CUI_Manager)->Call_Dialog("U_Dialog_V1_D3_");
			GET_SINGLE(CUI_Manager)->Open_Shop();
			return true;
			}
		);

		m_pCreateUIObject = nullptr;
	pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_EventActionMarker"), &(PosTemp = { -0.215f, 1.494f, 34.043f }), &m_pCreateUIObject, &(PosDir = 0), "AcMarker02_Weapon");
	dynamic_pointer_cast<CUI_EventActionMarker>(m_pCreateUIObject)->Add_Action(ACTION_MAKE{
				GET_SINGLE(CUI_Manager)->Call_Dialog("U_Dialog_V1_D1_");
				return true;
			}
		);
		m_pCreateUIObject = nullptr;
	pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_EventActionMarker"), &(PosTemp = { -4.989f, 6.090f, 41.050f }), &m_pCreateUIObject, &(PosDir = 0), "AcMarker03_Abalanchi");
	dynamic_pointer_cast<CUI_EventActionMarker>(m_pCreateUIObject)->Add_Action(ACTION_MAKE{
			GET_SINGLE(CUI_Manager)->Call_Dialog("U_Dialog_V1_D2_");
			return true;
			}
		);

		

	return S_OK;
}

HRESULT CLevel_Village1::Ready_Layer_Collider(const LAYERTYPE& strLayerTag)
{
	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/Collider_Data/Village1/Village1_Fin4.json", LEVEL_VILLAGE1, L_COLLIDER)))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CLevel_Village1::Ready_Layer_Trigger(const LAYERTYPE& strLayerTag)
{
	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/Trigger_Data/Village1/Village1.json", LEVEL_VILLAGE1, strLayerTag)))
		RETURN_EFAIL;

	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/Trigger_Data/Village1/UI/Village1UIFin.json", LEVEL_VILLAGE1, strLayerTag)))
		RETURN_EFAIL;
	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/Trigger_Data/Village1/Camera/Village1CameraFin.json", LEVEL_VILLAGE1, strLayerTag)))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CLevel_Village1::Ready_Light()
{
	m_pGameInstance->Reset_Light();
	m_pGameInstance->Add_Light("../Bin/Data/Light_Data/Village1/Village1_Fin4.json");

	return S_OK;
}

void CLevel_Village1::Ready_Event()
{	
	m_pGameInstance->Add_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "Next_Stage", LEVELEVENT_MAKE{

	if (dynamic_pointer_cast<CPhysX_Trigger>(pCaller.lock())->Get_CurCollisionType() == CPhysX_Trigger::ENTER)
	{
		m_bNextLevel = true;
	}
	return true;
		});
}


CLevel_Village1* CLevel_Village1::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)

{
	CLevel_Village1* pInstance = new CLevel_Village1(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Village1");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Village1::Free()
{
	__super::Free();
}