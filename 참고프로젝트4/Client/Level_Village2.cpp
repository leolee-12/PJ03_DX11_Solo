#include "stdafx.h"
#include "Level_Village2.h"

#include "GameInstance.h"

#include "Client_Manager.h"
#include "Camera_Manager.h"
#include "UI_Manager.h"
#include "UI_EventActionMarker.h"

#include "Data_Manager.h"

#include "Level_Loading.h"
#include "ObjPool_Manager.h"
#include "UI_Rect.h"
#include "Utility_File.h"
#include "Event_Manager.h"

#include "Player.h"
#include "State_Cloud_Dance.h"
#include "Camera_ThirdPerson.h"

CLevel_Village2::CLevel_Village2(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Village2::Initialize()
{
	__super::Initialize();

#ifndef _DEBUG
	::ShowCursor(FALSE);
#endif

#if LEVEL_SOUND == 0
	m_pGameInstance->Reset_SoundPoints();
	m_pGameInstance->Stop_SoundAll();
	m_pGameInstance->Add_SoundPoint(m_pGameInstance->MainDataPath() + L"SoundPoint_Data/Village2_1.json");
#endif

	
	GET_SINGLE(CClient_Manager)->Add_Player_In_Layer(LEVEL_VILLAGE2, _float3(44.f, 4.f, -55.f), _float3(41.f, 4.f, -55.f));

	GET_SINGLE(CCamera_Manager)->Add_Camera_in_Layer(LEVEL_VILLAGE2);

	GET_SINGLE(CClient_Manager)->Set_BattleMode_Natural(true);

	m_pGameInstance->Play_BGM(TEXT("FF7_BGM"), TEXT("bgm_slu5b_16 (Music-bgm_slu5b_12).mp3"), 1.f);

	GET_SINGLE(CClient_Manager)->Set_BattleMode(false);

	GET_SINGLE(CCamera_Manager)->Get_ThirdPersonCamera(CLOUD).lock()->Set_OffsetSetType(CCamera_ThirdPerson::OFFSET_DEFAULT);
	GET_SINGLE(CCamera_Manager)->Get_ThirdPersonCamera(AERITH).lock()->Set_OffsetSetType(CCamera_ThirdPerson::OFFSET_DEFAULT);

	return S_OK;
}

void CLevel_Village2::Tick(_cref_time fTimeDelta)
{
	CClient_Manager::GetInstance()->Tick_Client();

	CCamera_Manager::GetInstance()->Tick(fTimeDelta);
	CUI_Manager::GetInstance()->Tick(fTimeDelta);


#ifdef _DEBUG
	m_pGameInstance->Render_FPS(fTimeDelta, g_hWnd, TEXT("[GAMEPLAY_LEVEL] "));

#endif

	if (m_pGameInstance->Key_Down(DIK_RBRACKET))
	{
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_BOSS1))))
			return;
	
	}

	if (m_pGameInstance->Key_Down(DIK_LBRACKET))
	{
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_STAGE3))))
			return;
		return;
	}

	if (m_bNextLevel)
	{
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_STAGE2))))
			return;
		return;
	}
}

void CLevel_Village2::Late_Tick(_cref_time fTimeDelta)
{
	CUI_Manager::GetInstance()->Tick(fTimeDelta);
}

HRESULT CLevel_Village2::Render()
{
	if (FAILED(CCamera_Manager::GetInstance()->Render()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CLevel_Village2::Ready_Layer_Environment(const LAYERTYPE& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), strLayerTag, TEXT("Prototype_GameObject_SkyBox_Test"))))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CLevel_Village2::Ready_Layer_Object(const LAYERTYPE& strLayerTag)
{
	/*=======================TestStage=========================== 삭제금지*/
	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/Map_Data/Village2/Village2Fin_V1.json", LEVEL_VILLAGE2, L_OBJECT)))
		RETURN_EFAIL;

	if (FAILED(CData_Manager::GetInstance()->Load_InstanceData("../Bin/Data/Instance_Data/Village2/Village2Fin_V3.InstBin", LEVEL_VILLAGE2)))
		RETURN_EFAIL;
	
	return S_OK;
}

HRESULT CLevel_Village2::Ready_Layer_Player(const LAYERTYPE& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Village2::Ready_Layer_Monster(const LAYERTYPE& strLayerTag)
{
	//if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/AnimObject_Data/Monster/test2.json", LEVEL_VILLAGE2, strLayerTag)))
	//	RETURN_EFAIL;

	return S_OK;
}

HRESULT CLevel_Village2::Ready_Layer_NPC(const LAYERTYPE& strLayerTag)
{
	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/AnimObject_Data/NPC/Village2NPC.json", LEVEL_VILLAGE2, L_NPC)))
		RETURN_EFAIL;

	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/AnimObject_Data/Mob/Village2.json", LEVEL_VILLAGE2, L_MOB)))
		RETURN_EFAIL;
	return S_OK;
}


HRESULT CLevel_Village2::Ready_Layer_Effect(const LAYERTYPE& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Village2::Ready_Layer_Camera(const LAYERTYPE& strLayerTag)
{

	return S_OK;
}

HRESULT CLevel_Village2::Ready_Layer_UI(const LAYERTYPE& strLayerTag)
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
	//pUI_Manager->Make_MouseCursor();
	//pUI_Manager->Set_CursorUI_OnOff(false);

	m_pCreateUIObject = nullptr;
	pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_EventActionMarker"), &(PosTemp = { 53.13f, 1.46f, -20.15f }), &m_pCreateUIObject, &(PosDir = 2), "ActionMarker_Abalanchi");
	dynamic_pointer_cast<CUI_EventActionMarker>(m_pCreateUIObject)->Add_Action(ACTION_MAKE{
				GET_SINGLE(CUI_Manager)->Call_Dialog("U_Dialog_V2_D1_");
				return true;
		}
	);

	m_pCreateUIObject = nullptr;
	pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_EventActionMarker"), &(PosTemp = { -8.135f, 1.076f, 0.f }), &m_pCreateUIObject, &(PosDir = 3), "ActionMarker_IntoHall");
	dynamic_pointer_cast<CUI_EventActionMarker>(m_pCreateUIObject)->Add_Action(ACTION_MAKE{
				GET_SINGLE(CClient_Manager)->Get_PlayerPtr(CLOUD).lock()->Get_StateMachineCom().lock()->Set_State<CState_Cloud_Dance>();
				return true;
		}
	);

	pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_FloorDir"), &(PosTemp = { 33.06f, -0.305f, -14.93f }), nullptr, &(PosDir = 0), "FloorDir_GotoDance");
	pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_FloorDir"), &(PosTemp = { 22.97f, -0.31f, 0.f }), nullptr, &(PosDir = 3), "FloorDir_IntoHall");

	return S_OK;
}

HRESULT CLevel_Village2::Ready_Layer_Collider(const LAYERTYPE& strLayerTag)
{
	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/Collider_Data/Village2/Village2Fin_V2.json", LEVEL_VILLAGE2, L_COLLIDER)))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CLevel_Village2::Ready_Layer_Trigger(const LAYERTYPE& strLayerTag)
{

	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/Trigger_Data/Village2/UI/Village2_TriggerUI.json", LEVEL_VILLAGE2, strLayerTag)))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CLevel_Village2::Ready_Light()
{
	m_pGameInstance->Reset_Light();
	m_pGameInstance->Add_Light("../Bin/Data/Light_Data/Village2/Village2_Fin3.json");


	return S_OK;
}

void CLevel_Village2::Ready_Event()
{
}

CLevel_Village2* CLevel_Village2::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)

{
	CLevel_Village2* pInstance = new CLevel_Village2(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Village2");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Village2::Free()
{
	__super::Free();
}