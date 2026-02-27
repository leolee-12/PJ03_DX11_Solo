#include "stdafx.h"
#include "Level_BossStage.h"

#include "GameInstance.h"

#include "Client_Manager.h"

#include "Camera_Manager.h"
#include "UI_Manager.h"

#include "Data_Manager.h"

#include "Level_Loading.h"
#include "ObjPool_Manager.h"
#include "UI_Rect.h"
#include "Utility_File.h"

#include "Effect_Manager.h"
#include "Camera_Manager.h"
#include "Camera_ThirdPerson.h"

CLevel_BossStage::CLevel_BossStage(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_BossStage::Initialize()
{
	__super::Initialize();

#ifndef _DEBUG
	::ShowCursor(FALSE);
#endif

#if LEVEL_SOUND == 0
	m_pGameInstance->Reset_SoundPoints();
	m_pGameInstance->Stop_SoundAll();
	//m_pGameInstance->Add_SoundPoint(m_pGameInstance->MainDataPath() + L"SoundPoint_Data/Stage1_1.json");
#endif

	m_pGameInstance->Set_LightCulling(false);
	m_pGameInstance->Reset_Light();
	m_pGameInstance->Add_Light("../Bin/Data/Light_Data/Boss2/Boss2_Fin.json");
	m_pGameInstance->Make_Light_On_Owner(static_pointer_cast<CGameObject>(GET_SINGLE(CClient_Manager)->Get_PlayerPtr(Client::CLOUD).lock()), _float4(0.22f, 0.22f, 0.22f, 1.f), 2.f, _float3(0.f, 1.826f, 0.f), false, _float4(0.5f, 0.5f, 0.5f, 1.f));
	m_pGameInstance->Make_Light_On_Owner(static_pointer_cast<CGameObject>(GET_SINGLE(CClient_Manager)->Get_PlayerPtr(Client::AERITH).lock()), _float4(0.22f, 0.22f, 0.22f, 1.f), 2.f, _float3(0.f, 1.826f, 0.f), false, _float4(0.5f, 0.5f, 0.5f, 1.f));

	
	GET_SINGLE(CClient_Manager)->Add_Player_In_Layer(LEVEL_BOSS1, _float3(-40.f,1.f,14.f), _float3(-40.f, 1.f, 12.f));

	GET_SINGLE(CCamera_Manager)->Add_Camera_in_Layer(LEVEL_BOSS1);

	GET_SINGLE(CCamera_Manager)->Get_ThirdPersonCamera(CLOUD).lock()->Set_OffsetSetType(CCamera_ThirdPerson::OFFSET_BAHAMUT);
	GET_SINGLE(CCamera_Manager)->Get_ThirdPersonCamera(AERITH).lock()->Set_OffsetSetType(CCamera_ThirdPerson::OFFSET_BAHAMUT);

	GET_SINGLE(CEffect_Manager)->Create_Map_Effect<CParticle>(L"Boss2_Stage");

	GET_SINGLE(CClient_Manager)->Set_BattleMode_Natural(false);

	m_pGameInstance->Play_BGM(TEXT("FF7_BGM"), TEXT("bgm_lastboss_04#1 (Music-Section0).mp3"), 1.f);

	//GET_SINGLE(CClient_Manager)->Set_BattleMode(true);

	return S_OK;
}

void CLevel_BossStage::Tick(_cref_time fTimeDelta)
{
	CClient_Manager::GetInstance()->Tick_Client();

	CCamera_Manager::GetInstance()->Tick(fTimeDelta);
	CUI_Manager::GetInstance()->Tick(fTimeDelta);

	
	

	if (m_pGameInstance->Key_Down(DIK_LBRACKET))
	{
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_VILLAGE2))))
			return;
		return;
	}

#ifdef _DEBUG
	m_pGameInstance->Render_FPS(fTimeDelta, g_hWnd, TEXT("[GAMEPLAY_LEVEL] "));
#endif

}

void CLevel_BossStage::Late_Tick(_cref_time fTimeDelta)
{
	CUI_Manager::GetInstance()->Tick(fTimeDelta);
}

HRESULT CLevel_BossStage::Render()
{
	if (FAILED(CCamera_Manager::GetInstance()->Render()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CLevel_BossStage::Ready_Layer_Environment(const LAYERTYPE& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), strLayerTag, TEXT("Prototype_GameObject_SkyBox_Test"))))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CLevel_BossStage::Ready_Layer_Object(const LAYERTYPE& strLayerTag)
{

	/*=======================TestStage=========================== 삭제금지*/
	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/Map_Data/Boss2/Boss2_5.json", LEVEL_BOSS1, L_OBJECT)))
		RETURN_EFAIL;
	
	return S_OK;
}

HRESULT CLevel_BossStage::Ready_Layer_Player(const LAYERTYPE& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_BossStage::Ready_Layer_Monster(const LAYERTYPE& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), strLayerTag, TEXT("Prototype_GameObject_Bahamut"))))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CLevel_BossStage::Ready_Layer_NPC(const LAYERTYPE& strLayerTag)
{
	return S_OK;
}


HRESULT CLevel_BossStage::Ready_Layer_Effect(const LAYERTYPE& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_BossStage::Ready_Layer_Camera(const LAYERTYPE& strLayerTag)
{

	return S_OK;
}

HRESULT CLevel_BossStage::Ready_Layer_UI(const LAYERTYPE& strLayerTag)
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

	pUI_Manager->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_LockOnMarker"), &(PosTemp = { 0.f, -10.f, 0.f }), &m_pCreateUIObject);

	if (m_pCreateUIObject != nullptr)
		pUI_Manager->Set_LockOnMarker(m_pCreateUIObject);
	return S_OK;
}

HRESULT CLevel_BossStage::Ready_Layer_Collider(const LAYERTYPE& strLayerTag)
{
	if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/Collider_Data/BossStage/BossStageFin_V1.json", LEVEL_BOSS1, L_COLLIDER)))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CLevel_BossStage::Ready_Layer_Trigger(const LAYERTYPE& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_BossStage::Ready_Light()
{
	return S_OK;
}

void CLevel_BossStage::Ready_Event()
{	

}

CLevel_BossStage* CLevel_BossStage::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)

{
	CLevel_BossStage* pInstance = new CLevel_BossStage(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_BossStage");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_BossStage::Free()
{
	__super::Free();
}