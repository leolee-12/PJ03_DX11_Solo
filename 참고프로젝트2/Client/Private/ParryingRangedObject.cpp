#include "stdafx.h"
#include "..\Public\ParryingRangedObject.h"

#include "RenderInstance.h"
#include "GameInstance.h"

#include "UI_Define.h"
#include "Character.h"
#include "Main_Camera.h"
#include "Effect_Layer.h"
#include "Map_Manager.h"
#include "Effect_Manager.h"
#include "Effect.h"
#include "SpaceRock.h"
#include "Imgui_Manager.h"
CParryingRangedObject::CParryingRangedObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CParryingRangedObject::CParryingRangedObject(const CParryingRangedObject& Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CParryingRangedObject::Initialize_Prototype()
{

	
	return S_OK;
}

HRESULT CParryingRangedObject::Initialize(void* pArg)
{

	if (nullptr == pArg)
		return E_FAIL;


	PARRYING_RANGED_DESC* pDesc = static_cast<PARRYING_RANGED_DESC*>(pArg);


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vPos);


	CEffect_Layer::COPY_DESC tDesc{};
	tDesc.pPlayertMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	m_pEffect_Layer = CEffect_Manager::Get_Instance()->Copy_Layer_AndGet(TEXT("Parrying_Ball"), &tDesc);

	
	_float4 vColor{};

	switch (pDesc->iColor)
	{
	case Client::CParryingRangedObject::RANGED_LIGHT_NONE:
		break;
	case Client::CParryingRangedObject::RANGED_LIGHT_YELLOW:
		vColor = { 300.f,    255.f,    50.66f,   130.f };
		break;
	case Client::CParryingRangedObject::RANGED_LIGHT_PURPLE:
		vColor = { 148.f,    108.f,    259.2,   12.914f };
		break;
	case Client::CParryingRangedObject::RANGED_LIGHT_PINK:
		vColor = { 283.1f,    6.954f,    76.49f,   283.1f };
		break;
	default:
		break;
	}

	for (auto& iter : m_pEffect_Layer->m_MixtureEffects)
	{
		iter->m_iChangePassIndex = 10;
		/* 컬러값 이걸로 바꾸면됨 */
		//iter->m_vColor = CImgui_Manager::Get_Instance()->color;
		/*
		21호 283.1    6.954     76.49    283.1

		손오공 300   255  50.66   130

		프리저 148    108    259.2   12.914
		*/
		iter->m_vColor = vColor;
		
	}

	_float fPosX = XMVectorGetX(pDesc->vPos);


	if(CMap_Manager::Get_Instance()->m_eCurMap == CMap_Manager::MAP_SPACE)
	{

		_bool* bList = static_cast<CSpaceRock*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_SpaceRock")))->m_isBreakRock;

		_bool bBreakBall = false;

		//임시코드
		if(bList[0]==false)
		{
			if (abs(fPosX + 15.092f) < 5.f)
			{
				m_bDestroyObject = true;
				m_fGoalPosXZ = { -15.092f,10.208f };
				bBreakBall = true;
				//static_cast<CSpaceRock*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_SpaceRock")))->m_isBreakRock[0] = true;
				m_iMapBreakIndex = 0;
			}
		}
		if(bBreakBall == false && bList[1] == false)
		{
			if (abs(fPosX - 16.825f) < 7.f)
			{
				m_bDestroyObject = true;
				m_fGoalPosXZ = { 16.825f,16.178f };
				//static_cast<CSpaceRock*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_SpaceRock")))->m_isBreakRock[1] = true;
				m_iMapBreakIndex = 1;

			}

		}
		if (bBreakBall == false && bList[2] == false)
		{
			if (abs(fPosX - 12.263f) < 3.f)
			{
				m_bDestroyObject = true;
				m_fGoalPosXZ = { 12.263f,33.606f };
				//static_cast<CSpaceRock*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_SpaceRock")))->m_isBreakRock[2] = true;
				m_iMapBreakIndex = 2;

			}
		}
		 
	}
	//맵확인, 주변에 부술 수 있는 오브젝트가 있는지 거리검사,  이미 부숴져있는지 검사,  true


	return S_OK;
}

void CParryingRangedObject::Camera_Update(_float fTimeDelta)
{

}

void CParryingRangedObject::Update(_float fTimeDelta)
{

	if (Check_UpdateStop(fTimeDelta))
		return;
;


	m_fAccLifeTime += fTimeDelta;

	
	if(m_bDestroyObject == false)
	{
		if (m_fAccLifeTime < 1.5)
		{
			//m_pTransformCom->Add_Move({ 0.f,15 * fTimeDelta, 10 * fTimeDelta });
			m_pTransformCom->Add_Move({ 0.f,20 * fTimeDelta, 10 * fTimeDelta });
		}
		else
		{
			if (m_bFlipEnable)
			{
				m_pEffect_Layer->Set_Copy_Layer_Rotation({ 0.f,0.f,180.f });

				m_pTransformCom->Add_Move({ rand() % 10 - 5.f,0.f,0.f });
				m_bFlipEnable = false;
			}

			m_pTransformCom->Add_Move({ 0.f,-20 * fTimeDelta,10 * fTimeDelta });
		}

		if (m_fAccLifeTime > 5)
		{
			m_pEffect_Layer->m_bIsDoneAnim = true;
			m_pEffect_Layer = nullptr;
			Destory();
		}
	}
	else
	{
		if (m_fAccLifeTime < 1.5)
		{
			//m_pTransformCom->Add_Move({ 0.f,15 * fTimeDelta, 10 * fTimeDelta });
			m_pTransformCom->Add_Move({ 0.f,20 * fTimeDelta, 10 * fTimeDelta });
		}
		else
		{
			if (m_bFlipEnable)
			{
				m_pEffect_Layer->Set_Copy_Layer_Rotation({ 0.f,0.f,180.f });

				//m_pTransformCom->Add_Move({ rand() % 10 - 5.f,0.f,0.f });
				_vector vPos = { m_fGoalPosXZ.x, 30.f, m_fGoalPosXZ.y };
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
				m_bFlipEnable = false;
			}

			//m_pTransformCom->Add_Move({ 0.f,-20 * fTimeDelta,10 * fTimeDelta });
			m_pTransformCom->Add_Move({ 0.f,-20 * fTimeDelta,0.f });

		}
		

		_float fLength = GetVectorLength(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - _vector{ m_fGoalPosXZ.x, 0.f, m_fGoalPosXZ.y, 1.f });

		cout << fLength << endl;

	//	if (fLength < 5.f)
		if (fLength < 3.f)
		{


			if (static_cast<CSpaceRock*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_SpaceRock")))->m_isBreakRock[m_iMapBreakIndex] == false)
			{
				//bpBreak = true;
				static_cast<CSpaceRock*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_SpaceRock")))->m_isBreakRock[m_iMapBreakIndex] = true;

				m_pEffect_Layer->m_bIsDoneAnim = true;
				m_pEffect_Layer = nullptr;
				//static_cast<CSpaceRock*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_SpaceRock")))->m_isBreakRock[0] = true;


				XMStoreFloat4x4(&CMap_Manager::Get_Instance()->m_RockMatrix[m_iMapBreakIndex], XMMatrixIdentity());
				CMap_Manager::Get_Instance()->m_RockMatrix[m_iMapBreakIndex]._41 = m_fGoalPosXZ.x;
				CMap_Manager::Get_Instance()->m_RockMatrix[m_iMapBreakIndex]._42 = 0.f;
				CMap_Manager::Get_Instance()->m_RockMatrix[m_iMapBreakIndex]._43 = m_fGoalPosXZ.y;
				//for (auto& iter : pLayer->m_MixtureEffects)
				_float4x4 Result4x4;
				CEffect_Layer::COPY_DESC tDesc{};
				//tDesc.pTransformCom = m_pTransformCom;
				//tDesc.pPlayertMatrix = &Result4x4;
				tDesc.pPlayertMatrix = &(CMap_Manager::Get_Instance()->m_RockMatrix[m_iMapBreakIndex]);
				//tDesc.pTransformCom = m_pTransformCom;
				CEffect_Layer* pLayer = CEffect_Manager::Get_Instance()->Copy_Layer_AndGet(TEXT("Crash_Smoke"), &tDesc);
				//{
				//	iter->m_iChangePassIndex = 
				//}
				//_float fLength = GetVectorLength(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - _vector{ m_fGoalPosXZ.x, 0.f, m_fGoalPosXZ.y, 1.f });

				Destory();
			}
		
		}
	

		else if (m_fAccLifeTime > 5)
		{
			m_pEffect_Layer->m_bIsDoneAnim = true;
			m_pEffect_Layer = nullptr;
			//static_cast<CSpaceRock*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_SpaceRock")))->m_isBreakRock[0] = true;
			Destory();
		}


	}


	//m_pTransformCom->Add_Move({ 0.f,5 * fTimeDelta,0.f });

	

}

void CParryingRangedObject::Late_Update(_float fTimeDelta)
{

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CParryingRangedObject::Render(_float fTimeDelta)
{



	return S_OK;
}


_bool CParryingRangedObject::Check_UpdateStop(_float fTimeDelta)
{
	

	return false;
}




HRESULT CParryingRangedObject::Ready_Components(PARRYING_RANGED_DESC* pDesc)
{




	return S_OK;
}



CParryingRangedObject* CParryingRangedObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CParryingRangedObject* pInstance = new CParryingRangedObject(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CParryingRangedObject"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CParryingRangedObject::Clone(void* pArg)
{
	CParryingRangedObject* pInstance = new CParryingRangedObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CParryingRangedObject"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParryingRangedObject::Free()
{
	__super::Free();

}
