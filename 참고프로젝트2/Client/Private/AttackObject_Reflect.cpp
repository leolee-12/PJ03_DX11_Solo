#include "stdafx.h"
#include "..\Public\AttackObject_Reflect.h"

#include "RenderInstance.h"
#include "GameInstance.h"

#include "Character.h"
#include "Main_Camera.h"
CAttackObject_Reflect::CAttackObject_Reflect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CAttackObject{ pDevice, pContext }
{

}

CAttackObject_Reflect::CAttackObject_Reflect(const CAttackObject_Reflect& Prototype)
	: CAttackObject{ Prototype }
{

}

HRESULT CAttackObject_Reflect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAttackObject_Reflect::Initialize(void* pArg)
{

	if (nullptr == pArg)
		return E_FAIL;


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;







	return S_OK;
}

void CAttackObject_Reflect::Camera_Update(_float fTimeDelta)
{

}

void CAttackObject_Reflect::Update(_float fTimeDelta)
{

	if (Check_UpdateStop(fTimeDelta))
		return;


	m_fAccLifeTime += fTimeDelta;

	if (m_fAccLifeTime > m_fLifeTime)
	{
		if (m_bEnableDestory)
		{
			//m_pGameInstance->Release_Collider(m_pColliderCom);
			CGameInstance::Get_Instance()->Destroy_Reserve(m_pColliderCom);
			Destory();
			m_bEnableDestory = false;

			

		}
	}
	else
		m_pColliderCom->Update(m_pOwnerTransform->Get_State(CTransform::STATE_POSITION));



}

void CAttackObject_Reflect::Late_Update(_float fTimeDelta)
{

	
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CAttackObject_Reflect::Render(_float fTimeDelta)
{

#ifdef _DEBUG
	m_pColliderCom->Render(fTimeDelta);
#endif // DEBUG


	return S_OK;
}

/*
void CAttacKObject_Reflect::Set_RemoteDestory()
{
	if (m_bEnableDestory)
	{
		m_pGameInstance->Release_Collider(m_pColliderCom);
		CGameInstance::Get_Instance()->Destroy_Reserve(m_pColliderCom);
		m_bEnableDestory = false;
	}
}
*/


void CAttackObject_Reflect::OnCollisionEnter(CCollider* other, _float fTimeDelta)
{


	if (m_pOwner->Get_bStun() == true)
	{

		return;
	}

	//리플렉터 vs 근접공격
	if (other->m_ColliderGroup == CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack || other->m_ColliderGroup == CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack)
	{

		m_pOwner->Character_Make_Effect(TEXT("Parrying_Hit"), { 0.5f,0.f });

		CAttackObject* pAttackObject = static_cast<CAttackObject*>(other->GetMineGameObject());

		CCharacter* pCharacter = static_cast<CCharacter*>(pAttackObject->Get_pOwner());
		//pCharacter->Set_AnimationStop(0.1f);
		//한번에 확 밀려나서 이상할텐데
		//pCharacter->Add_Move({ m_pOwner->Get_iDirection() * 0.5f, 0.f });
		pCharacter->Set_fImpulse(2.f * m_pOwner->Get_iDirection());

		m_pOwner->Set_ReflectAttackBackEvent(true);


		m_pOwner->Gain_KiAmount(15);
		
		pAttackObject->Set_RemoteDestory();
		pCharacter->Set_bBeReflecting(1);

	}
	//리플렉터 vs 에너지파 
	else if (other->m_ColliderGroup == CCollider_Manager::COLLIDERGROUP::CG_1P_Energy_Attack || other->m_ColliderGroup == CCollider_Manager::COLLIDERGROUP::CG_2P_Energy_Attack)
	{
		m_pOwner->Set_ReflectAttackBackEvent(true);
		//없음

		m_pOwner->Gain_KiAmount(10);

		//CAttackObject* pAttackObject = static_cast<CAttackObject*>(other->GetMineGameObject());
		//pAttackObject->Set_RemoteDestory();
		m_pOwner->Character_Make_Effect(TEXT("Parrying_Hit"), { 0.5f,0.f });
	}
	
	
	//리플렉터 vs 원거리   는 원거리에서 처리중.   원거리 공격 상대로 Reflect 기능 달린 개체가 더 있고 Ragned의 정보가 필요하기 때문
	//else if (other->m_ColliderGroup == CCollider_Manager::COLLIDERGROUP::CG_1P_Energy_Attack || other->m_ColliderGroup == CCollider_Manager::COLLIDERGROUP::CG_2P_Energy_Attack)


}

void CAttackObject_Reflect::OnCollisionStay(CCollider* other, _float fTimeDelta)
{
	_bool Debug = true;

}

void CAttackObject_Reflect::OnCollisionExit(CCollider* other)
{
	_bool Debug = true;

}

void CAttackObject_Reflect::Erase()
{
	if (m_bEnableDestory)
	{
		CGameInstance::Get_Instance()->Destroy_Reserve(m_pColliderCom);
		m_bEnableDestory = false;
		Destory();
	}
}

void CAttackObject_Reflect::CollisingAttack()
{
}

void CAttackObject_Reflect::CollisingPlayer()
{
}







CAttackObject_Reflect* CAttackObject_Reflect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAttackObject_Reflect* pInstance = new CAttackObject_Reflect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CAttackObject_Reflect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CAttackObject_Reflect::Clone(void* pArg)
{
	CAttackObject_Reflect* pInstance = new CAttackObject_Reflect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CAttackObject_Reflect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAttackObject_Reflect::Free()
{
	__super::Free();
	Safe_Release(m_pColliderCom);

}
