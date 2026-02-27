#include "stdafx.h"
#include "..\Public\Opening_Model.h"

#include "RenderInstance.h"
#include "GameInstance.h"
#include "iostream"
#include "AttackObject.h"
#include "UI_Manager.h"
#include "Effect_Manager.h"

#include "AttackObject.h"
#include "BattleInterface.h"
#include "Main_Camera.h"

#include "Animation.h"
#include <cmath>
#include "SpaceMeteoBreak.h"
#include "Effect_Layer.h"
#include "Map_Manager.h"

#include "QTE_Manager.h"


COpening_Model::COpening_Model(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
	, m_pUI_Manager{ CUI_Manager::Get_Instance() }
{
	Safe_AddRef(m_pUI_Manager);
}

COpening_Model::COpening_Model(const COpening_Model& Prototype)
	: CGameObject{ Prototype }
	, m_pFrameEvent{ Prototype.m_pFrameEvent }
	, m_pUI_Manager{ CUI_Manager::Get_Instance() }
	, m_pEffect_Manager{ CEffect_Manager::Get_Instance() }
{
	Safe_AddRef(m_pUI_Manager);
	Safe_AddRef(m_pEffect_Manager);
}

HRESULT COpening_Model::Initialize_Prototype()
{
	return S_OK;
}

HRESULT COpening_Model::Initialize(void* pArg)
{
	Opening_Model_DESC* pDesc = static_cast<Opening_Model_DESC*>(pArg);

	m_pFrameEvent = CFrameEvent_Manager::Get_Instance()->Get_pFrameEventMap();

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	
	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vStartPostion);




	return S_OK;
}

void COpening_Model::Player_Update(_float fTimeDelta)
{
}

void COpening_Model::Camera_Update(_float fTimeDelta)
{
}

void COpening_Model::Update(_float fTimeDelta)
{
}

void COpening_Model::Late_Update(_float fTimeDelta)
{
}

HRESULT COpening_Model::Render(_float fTimeDelta)
{
	if (m_bInvisible == true)
		return S_OK;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		/* 모델이 가지고 있는 머테리얼 중 i번째 메시가 사용해야하는 머테리얼구조체의 aiTextureType_DIFFUSE번째 텍스쳐를 */
		/* m_pShaderCom에 있는 g_DiffuseTexture변수에 던져. */
		if (FAILED(m_pModelCom->Bind_MaterialSRV(m_pShaderCom, aiTextureType_DIFFUSE, "g_DiffuseTexture", i)))
			return E_FAIL;
		
		//m_pModelCom->Bind_MaterialSRV(m_pShaderCom, aiTextureType_NORMALS, "g_NormalTexture", i);

	   /* 모델이 가지고 있는 뼈들 중에서 현재 렌더링할려고 했던 i번째ㅑ 메시가 사용하는 뼈들을 배열로 만들어서 쉐이더로 던져준다.  */
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pShaderCom->Begin(1)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;

	}



}

_bool COpening_Model::Character_Play_Animation(_float fTimeDelta)
{
	_bool bAnimationEnd = false;

	_float fPrePosition = m_pModelCom->m_fCurrentAnimPosition;

	//_int iOneFrameTeest = 0;

	if (fPrePosition == 0)
	{

		ProcessEventsFramesZero(m_eCharacterIndex, m_pModelCom->m_iCurrentAnimationIndex);
		fPrePosition += 0.001;

		//iOneFrameTeest++;
	}

	if (m_pModelCom->Play_Animation_Lick(fTimeDelta))
		//if (m_pModelCom->Play_Animation(fTimeDelta))
	{
		//모션이 끝났으면, 루프면    (아까까진 루프가 아니였는데 이번에 루프면 어쩌지?)
		if (m_pModelCom->m_isLoopAnim)
		{
			fPrePosition = 0.001;
			ProcessEventsFramesZero(m_eCharacterIndex, m_pModelCom->m_iCurrentAnimationIndex);
			//iOneFrameTeest++;
		}
		bAnimationEnd = true;
		m_bMotionPlaying = false;
	}
	else
		m_bMotionPlaying = true;


	_float fCurPosition = m_pModelCom->m_fCurrentAnimPosition;

	
	ProcessEventsBetweenFrames2(0, m_pModelCom->m_iCurrentAnimationIndex, fPrePosition, fCurPosition);



	return bAnimationEnd;
}

HRESULT COpening_Model::Ready_Components(Opening_Model_DESC* pDesc)
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, pDesc->strModelname, TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;


	return S_OK;
}

HRESULT COpening_Model::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pOutLineCom->Bind_ShaderResource(m_pShaderCom, "g_OutLineTexture", 1)))
		return E_FAIL;


	//여기 문제있을듯
	const LIGHT_DESC* pLightDesc = m_pRenderInstance->Get_LightDesc(CLight_Manager::LIGHT_PLAYER, 0, m_strName);
	if (nullptr == pLightDesc)
		return E_FAIL;



	if (FAILED(m_pShaderCom->Bind_RawValue("g_iPlayerDirection", pLightDesc->pPlayerDirection, sizeof(_int))))
		return E_FAIL;

	return S_OK;
}

COpening_Model* COpening_Model::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	COpening_Model* pInstance = new COpening_Model(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : COpening_Model"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* COpening_Model::Clone(void* pArg)
{
	COpening_Model* pInstance = new COpening_Model(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : COpening_Model"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COpening_Model::Free()
{
	__super::Free();
	Safe_Release(m_pDetailTextureCom);
	Safe_Release(m_p1PTextureCom);
	Safe_Release(m_pOutLineCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	Safe_Release(m_pUI_Manager);
	Safe_Release(m_pEffect_Manager);
	Safe_Release(m_pOutLineCom);

}

void COpening_Model::Set_Animation(_uint iAnimationIndex, _bool bloof)
{
	m_pModelCom->SetUp_Animation(iAnimationIndex, bloof);


}

void COpening_Model::Set_CurrentAnimationPositionJump(_float fAnimationPosition)
{
	m_pModelCom->CurrentAnimationPositionJump(fAnimationPosition);
}

void COpening_Model::ProcessEventsFramesZero(_uint characterIndex, _uint animationIndex)
{



}

void COpening_Model::ProcessEventsBetweenFrames2(int characterIndex, int animationIndex, float prevFrame, float currentFrame)
{
	

}

void COpening_Model::FlipDirection(_int iDirection)
{
	if (iDirection == 0)
	{
		m_iLookDirection = -m_iLookDirection;

		m_pTransformCom->Set_Scaled(-1, 1, 1);
	}
	else if (m_iLookDirection != iDirection)
	{

		m_iLookDirection = iDirection;

		m_pTransformCom->Set_Scaled(-1, 1, 1);
	}
}

void COpening_Model::Add_Move(_float2 fMovement)
{
	m_pTransformCom->Add_Move({ fMovement.x, fMovement.y,0 });
}
