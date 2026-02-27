#include "stdafx.h"
#include "..\Public\AttackObject_Chase.h"

#include "RenderInstance.h"
#include "GameInstance.h"

#include "Character.h"
#include "Main_Camera.h"
CAttackObject_Chase::CAttackObject_Chase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CAttackObject{ pDevice, pContext }
{

}

CAttackObject_Chase::CAttackObject_Chase(const CAttackObject_Chase& Prototype)
	: CAttackObject{ Prototype }
{

}

HRESULT CAttackObject_Chase::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAttackObject_Chase::Initialize(void* pArg)
{

	if (nullptr == pArg)
		return E_FAIL;


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;







	return S_OK;
}

void CAttackObject_Chase::Camera_Update(_float fTimeDelta)
{

}

void CAttackObject_Chase::Update(_float fTimeDelta)
{

	if (Check_UpdateStop(fTimeDelta))
		return;


	m_fAccLifeTime += fTimeDelta;

	if (m_fAccLifeTime > m_fLifeTime)
	{
		if (m_bEnableDestory)
		{
			CGameInstance::Get_Instance()->Destroy_Reserve(m_pColliderCom);
			//m_pGameInstance->Release_Collider(m_pColliderCom);
			m_bEnableDestory = false;
			Destory();

		}
	}
	else
		m_pColliderCom->Update(m_pOwnerTransform->Get_State(CTransform::STATE_POSITION));


	//if(m_bIsActive)
	//{
	//	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pOwnerTransform->Get_State(CTransform::STATE_POSITION));
	//	//m_pColliderCom->Update(m_fOffset);
	//
	//	m_fAccLifeTime += fTimeDelta;
	//
	//	if (m_fAccLifeTime > m_fLifeTime)
	//	{
	//		m_bIsActive = false;
	//	}
	//
	//	//m_pColliderCom->Update(m_pOwnerTransform->Get_WorldMatrix());
	//	m_pColliderCom->Update(m_pOwnerTransform->Get_State(CTransform::STATE_POSITION));
	//}

}

void CAttackObject_Chase::Late_Update(_float fTimeDelta)
{

	//if (m_bIsActive)
	//{
	//
	//
	//	
	//
	//
	//	
	//	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pOwnerTransform->Get_State(CTransform::STATE_POSITION));
	//	//m_pColliderCom->Update(m_fOffset);
	//
	//	_bool bisCollsing = false;
	//
	//	//Layer_캐릭터에 대하여
	//	for (int i = 0; i < m_pGameInstance->Get_LayerSize(LEVEL_GAMEPLAY, TEXT("Layer_Character")); i++)
	//	{
	//
	//		CCharacter* pCharacter = static_cast<CCharacter*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Character"), i));
	//
	//
	//
	//		//생성된 공격과 상대방이 다른팀이라면 충돌검사
	//		if (pCharacter->Get_iPlayerTeam() != m_iTeam)
	//		{
	//
	//
	//
	//			//if (m_pColliderCom->isColliding(pCharacter->Get_Component(TEXT("Com_Collider"))))
	//			//{
	//			//	bisCollsing = true;
	//			//	
	//			//	//공격성공시
	//			//	//if(pCharacter->Set_Hit(m_ihitCharacter_Motion, m_fhitCharacter_StunTime, m_iDamage, m_fAnimationLockTime, m_fhitCharacter_Impus))
	//			//	//if (pCharacter->Set_Hit2(m_ihitCharacter_Motion, m_eAttackGrade, m_eAttackType, m_fhitCharacter_StunTime, m_iDamage, m_fAnimationLockTime, m_fhitCharacter_Impus))
	//			//	//{
	//			//	//	pCharacter->Set_GroundSmash(m_bGroundSmash);
	//			//	//	m_pOwner->Set_AnimationStop(m_fAnimationLockTime);
	//			//	//
	//			//	//	if (m_bGain_AttackStep)
	//			//	//	{
	//			//	//		m_pOwner->Gain_AttackStep(1);
	//			//	//	}
	//			//	//
	//			//	//}
	//			//	//else  //가드당했을시 충돌은 했으니
	//			//	//{
	//			//	//	m_pOwner->Set_AnimationStop(0.05f);
	//			//	//	pCharacter->Set_AnimationStop(0.05f);
	//			//	//}
	//
	//
	//			//	AttackColliderResult eResult =
	//			//		pCharacter->Set_Hit3(m_ihitCharacter_Motion, m_eAttackGrade, m_eAttackType, m_fhitCharacter_StunTime, m_iDamage, m_fAnimationLockTime, m_fhitCharacter_Impus);
	//
	//			//		
	//			//	if (eResult == RESULT_HIT)
	//			//	{
	//			//		pCharacter->Set_GroundSmash(m_bGroundSmash);
	//			//		m_pOwner->Set_AnimationStop(m_fAnimationLockTime);
	//
	//			//		//if (m_bGain_AttackStep)
	//			//		{
	//			//			m_pOwner->Gain_AttackStep(m_iGain_AttackStep);
	//			//		}
	//
	//			//		if (m_bOwnerNextAnimation)
	//			//		{
	//			//			m_pOwner->Set_NextAnimation(m_iOnwerNextAnimationIndex,1.f);
	//			//		}
	//
	//			//	}
	//			//	else if (eResult == RESULT_GUARD) //가드당해도 충돌은 했으니 시간정지연출
	//			//	{
	//			//		m_pOwner->Set_AnimationStop(0.08f);
	//			//		pCharacter->Set_AnimationStop(0.08f);
	//			//	}
	//
	//			//	else if (eResult == RESULT_DRAW)
	//			//	{
	//			//		m_pOwner->Set_AnimationStop(0.3f);
	//			//		pCharacter->Set_AnimationStop(0.3f);
	//			//	}
	//
	//			//	else if (eResult == RESULT_MISS)
	//			//	{
	//			//		//잡기는 한번 빗나가면 끝
	//			//		if (m_eAttackType == ATTACKTYPE_GRAB_GROUND || m_eAttackType == ATTACKTYPE_GRAB_AIR)
	//			//			bisCollsing = true;
	//
	//			//		//그 외에는 공격판정 사라지지 않음
	//			//		else
	//			//			bisCollsing = false;
	//
	//			//	}
	//
	//
	//			//}
	//		}
	//	}
	//
	//	if (bisCollsing)
	//	{
	//		m_bIsActive = false;
	//		//if (m_bOwnerGravityTimeReset)
	//		//{
	//		//	m_pOwner->Set_ForcedGravityTime_LittleUp();
	//		//}
	//		CGameInstance::Get_Instance()->Destroy_Reserve(m_pColliderCom);
	//	}
	//


	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CAttackObject_Chase::Render(_float fTimeDelta)
{

#ifdef _DEBUG
	m_pColliderCom->Render(fTimeDelta);
#endif // DEBUG


	return S_OK;
}

/*
void CAttacKObject_Chase::Set_RemoteDestory()
{
	if (m_bEnableDestory)
	{
		m_pGameInstance->Release_Collider(m_pColliderCom);
		CGameInstance::Get_Instance()->Destroy_Reserve(m_pColliderCom);
		m_bEnableDestory = false;
	}
}
*/


void CAttackObject_Chase::OnCollisionEnter(CCollider* other, _float fTimeDelta)
{


	if (m_pOwner->Get_bStun() == true)
	{

		return;
	}

	//체이스 vs 근접공격
	if (other->m_ColliderGroup == CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack || other->m_ColliderGroup == CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack)
	{
		CAttackObject* pAttackObject = static_cast<CAttackObject*>(other->GetMineGameObject());
		CCharacter* pCharacter = static_cast<CCharacter*>(pAttackObject->Get_pOwner());
		pCharacter->Set_AnimationStop(0.5f);


		m_pOwner->Set_fImpulse(-1.f * m_pOwner->Get_iDirection());
		m_pOwner->Set_ForcedGravityTime_LittleUp();
		m_pOwner->Set_ChaseStop();
		m_pOwner->Set_ChaseStoping();
		//m_pOwner->Set_AnimationStop(0.1f);

		m_pOwner->Set_AnimationStop(0.5f);

		//m_pOwner->Set_Animation(m_pOwner->Get_JumpAirAnimationIndex());
		//m_pOwner->

		//CTransform* pTransform = static_cast<CTransform*>(m_pOwner->Get_Component(TEXT("Com_Transform")));
		//pTransform->Add_Move({ 0.f,0.3f,0.f });


		//pCharacter->Set_fImpulse(-1.f);
		//pCharacter->Set_Animation(m_pOwner->Get_BreakFall_AirAnimationIndex());
		//pCharacter->Set_ForcedGravityTime_LittleUp();


	}

	//체이스 vs 사람 
	else if (other->m_ColliderGroup == CCollider_Manager::COLLIDERGROUP::CG_1P_BODY || other->m_ColliderGroup == CCollider_Manager::COLLIDERGROUP::CG_2P_BODY)
	{
		CCharacter* pCharacter = static_cast<CCharacter*>(other->GetMineGameObject());

		AttackColliderResult eResult =
			pCharacter->Set_Hit4(m_ihitCharacter_Motion, m_eAttackGrade, m_eAttackType, m_fhitCharacter_StunTime, m_iDamage, m_fAnimationLockTime, m_pOwner->Get_iDirection(), m_fhitCharacter_Impus);
		//pCharacter->Set_Hit3(m_ihitCharacter_Motion, m_eAttackGrade, m_eAttackType, m_fhitCharacter_StunTime, m_iDamage, m_fAnimationLockTime, m_fhitCharacter_Impus);

		if (eResult == RESULT_HIT)
		{
			if (m_isfxSoundIndex != 60000)
			{
				m_pGameInstance->Play_Sound((CSound_Manager::SOUND_KEY_NAME)m_isfxSoundIndex, false, m_fsfxVolume);
			}

			//pCharacter->Set_GroundSmash(m_bGroundSmash);
			m_pOwner->Set_AnimationStop(m_fAnimationLockTime);

			m_pOwner->Gain_AttackStep(m_iGainAttackStep);
			m_pOwner->Gain_HitCount(m_iGainHitCount);
			m_pOwner->Gain_KiAmount(4);

			if (m_bOwnerNextAnimation)
			{
				m_pOwner->Set_NextAnimation(m_iOnwerNextAnimationIndex, 1.f);
			}
			m_pOwner->Set_ChaseStop();
			m_pOwner->Set_ChaseStoping();

			m_pOwner->Set_AnimationStop(0.2f);
			pCharacter->Set_AnimationStop(0.2f);

			//여기 Chase 피격시 사운드
			m_pGameInstance->Play_Group_Sound(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_Hit_SFX, false, 1.f);
			static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")))->StartCameraShake(0.5f, 0.2f);

			CGameInstance::Get_Instance()->Destroy_Reserve(m_pColliderCom);
			Destory();

		}
		else if (eResult == RESULT_GUARD) //가드당해도 충돌은 했으니 시간정지연출
		{
			m_pOwner->Set_AnimationStop(0.08f);
			pCharacter->Set_AnimationStop(0.08f);

			m_pOwner->Set_ChaseStop();
			m_pOwner->Set_ChaseStoping();

			CGameInstance::Get_Instance()->Destroy_Reserve(m_pColliderCom);
			Destory();
		}

		//사람 상대로 비길 수 없음
		//else if (eResult == RESULT_DRAW)
		//{
		//	m_pOwner->Set_AnimationStop(0.3f);
		//	pCharacter->Set_AnimationStop(0.3f);
		//}

		else if (eResult == RESULT_MISS)
		{
			m_pOwner->Set_ChaseStop();
			m_pOwner->Set_ChaseStoping();
			return;



		}

		//if (m_bEnableDestory)
		//{
		//	CGameInstance::Get_Instance()->Destroy_Reserve(m_pColliderCom);
		//	m_bEnableDestory = false;
		//}
	}
	


}

void CAttackObject_Chase::OnCollisionStay(CCollider* other, _float fTimeDelta)
{
	_bool Debug = true;

}

void CAttackObject_Chase::OnCollisionExit(CCollider* other)
{
	_bool Debug = true;

}

void CAttackObject_Chase::CollisingAttack()
{
}

void CAttackObject_Chase::CollisingPlayer()
{
}







CAttackObject_Chase* CAttackObject_Chase::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAttackObject_Chase* pInstance = new CAttackObject_Chase(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CAttackObject_Chase"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CAttackObject_Chase::Clone(void* pArg)
{
	CAttackObject_Chase* pInstance = new CAttackObject_Chase(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CAttackObject_Chase"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAttackObject_Chase::Free()
{
	__super::Free();
	Safe_Release(m_pColliderCom);

}
