#include "stdafx.h"
#include "Level_Tool.h"

#include "GameInstance.h"

#include "Imgui_Manager.h"
#include "Camera_Manager.h"
#include "Client_Manager.h"

#include "UI_Manager.h"

#include "Level_Loading.h"
#include "ObjPool_Manager.h"
#include "Particle.h"
#include "InteractionBox.h"

#include "Level_Test_Defines.h"

CLevel_Tool::CLevel_Tool(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Tool::Initialize()
{
	__super::Initialize();

	//::ShowCursor(TRUE);

	if (!m_pGameInstance->IsVisitedLevel(LEVEL_TOOL))
	{
		if (FAILED(CCamera_Manager::GetInstance()->Initialize(m_pDevice, m_pContext)))
			RETURN_EFAIL;
	}
	else
	{
		GET_SINGLE(CCamera_Manager)->Add_Camera_in_Layer(LEVEL_TOOL);
	}
#ifdef _DEBUG
	if (FAILED(CImgui_Manager::GetInstance()->Initialize(m_pDevice, m_pContext)))
		RETURN_EFAIL;
#endif // DEBUGz

	if (FAILED(CUI_Manager::GetInstance()->Initialize(m_pDevice, m_pContext)))
		RETURN_EFAIL;

	//GET_SINGLE(CCamera_Manager)->Add_Camera_in_Layer(LEVEL_TOOL);

	m_pGameInstance->Set_VisitedLevel(LEVEL_TOOL);

	return S_OK;
}

void CLevel_Tool::Tick(_cref_time fTimeDelta)
{
	CClient_Manager::GetInstance()->Tick_Client();

	CCamera_Manager::GetInstance()->Tick(fTimeDelta);

#ifdef _DEBUG
	m_pGameInstance->Render_FPS(fTimeDelta, g_hWnd, TEXT("[GAMEPLAY_TOOL] "));
#endif
	if (m_pGameInstance->Key_Pressing(DIK_RBRACKET))
	{

#if NEXT_STAGE == 0

		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_VILLAGE1))))
			return;

#elif NEXT_STAGE == 1
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_STAGE1))))
			return;

#elif NEXT_STAGE == 3

		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_STAGE3))))
			return; // 스테이지3 에어버스터 전용
#elif NEXT_STAGE == 4
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_BOSS1))))
			return;
#endif

#ifndef _DEBUG
		::ShowCursor(FALSE);
#endif
		return;
	}
	else if (m_pGameInstance->Key_Pressing(DIK_LBRACKET))
	{/*
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_LOGO))))
			return;*/
	}


	if (m_pGameInstance->Key_DownEx(DIK_M,DIK_MD_LCTRL))
	{
		shared_ptr<CGameObject> pGameObject = nullptr;
		if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_PLAYER, TEXT("Prototype_GameObject_Cloud"),nullptr,&pGameObject)))
			return;
		
		pGameObject->Get_TransformCom().lock()->Set_State(CTransform::STATE_POSITION, _float4{ 0.f, 5.f, 0.f,1.f });
	}

	if (m_pGameInstance->Key_DownEx(DIK_N, DIK_MD_LCTRL))
	{
		shared_ptr<CGameObject> pGameObject = nullptr;
		if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_PLAYER, TEXT("Prototype_GameObject_InteractionBox"), nullptr, &pGameObject)))
			return;
		
		_int iType = Random({ 0,1 });
		static_pointer_cast<CInteractionBox>(pGameObject)->Set_BoxType((CInteractionBox::BOX_TYPE)iType);
		pGameObject->Get_TransformCom().lock()->Set_State(CTransform::STATE_POSITION, _float4{ 0.f, 5.f, 0.f,1.f });
	}

}

HRESULT CLevel_Tool::Render()
{
	if (FAILED(CCamera_Manager::GetInstance()->Render()))
		RETURN_EFAIL;
#ifdef _DEBUG
	if (FAILED(CImgui_Manager::GetInstance()->Render()))
		RETURN_EFAIL;
#endif // DEBUG



	_cref_time fTimeDelta = m_pGameInstance->Get_TimeDelta();
	CCamera_Manager::GetInstance()->Late_Tick(fTimeDelta);

	return S_OK;
}


HRESULT CLevel_Tool::Ready_Layer_Environment(const LAYERTYPE& strLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_ENVIRONMENT, TEXT("Prototype_GameObject_PhysTrigger"))))
	//	RETURN_EFAIL;

	if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), strLayerTag, TEXT("Prototype_GameObject_SkyBox_Test"))))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_Object(const LAYERTYPE& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_Player(const LAYERTYPE& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_Monster(const LAYERTYPE& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_NPC(const LAYERTYPE& strLayerTag)
{
	return S_OK;
}
HRESULT CLevel_Tool::Ready_Layer_Effect(const LAYERTYPE& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_Camera(const LAYERTYPE& strLayerTag)
{

	return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_UI(const LAYERTYPE& strLayerTag)
{
	return S_OK;
}


CLevel_Tool* CLevel_Tool::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)

{
	CLevel_Tool* pInstance = new CLevel_Tool(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Tool");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Tool::Free()
{
	__super::Free();
#ifdef _DEBUG
	CImgui_Manager::GetInstance()->DestroyInstance();
#endif // DEBUG

	CCamera_Manager::GetInstance()->DestroyInstance();
	CUI_Manager::GetInstance()->DestroyInstance();
}
