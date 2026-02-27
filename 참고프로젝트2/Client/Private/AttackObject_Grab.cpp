#include "stdafx.h"
#include "..\Public\AttackObject_Grab.h"

#include "RenderInstance.h"
#include "GameInstance.h"
#include "Main_Camera.h"
#include "Character.h"

CAttackObject_Grab::CAttackObject_Grab(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CAttackObject{ pDevice, pContext }
{

}

CAttackObject_Grab::CAttackObject_Grab(const CAttackObject_Grab& Prototype)
	: CAttackObject{ Prototype }
{

}

HRESULT CAttackObject_Grab::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAttackObject_Grab::Initialize(void* pArg)
{

	if (nullptr == pArg)
		return E_FAIL;

	ATTACK_Grab_DESC* pDesc = static_cast<ATTACK_Grab_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;



	m_fDistance = pDesc->fDistance;
	//m_iGrabAnimationIndex = pDesc->iGrabAnimationIndex;
	m_fGrabAnimationPosition = pDesc->fGrabAnimationPosition;

	return S_OK;
}

void CAttackObject_Grab::Camera_Update(_float fTimeDelta)
{

}

void CAttackObject_Grab::Update(_float fTimeDelta)
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

}

void CAttackObject_Grab::Late_Update(_float fTimeDelta)
{



	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CAttackObject_Grab::Render(_float fTimeDelta)
{
	//
#ifdef _DEBUG
	m_pColliderCom->Render(fTimeDelta);
#endif // DEBUG


	return S_OK;
}

/*
void CAttacKObject_Grab::Set_RemoteDestory()
{
	if (m_bEnableDestory)
	{
		m_pGameInstance->Release_Collider(m_pColliderCom);
		CGameInstance::Get_Instance()->Destroy_Reserve(m_pColliderCom);
		m_bEnableDestory = false;
	}
}
*/


void CAttackObject_Grab::OnCollisionEnter(CCollider* other, _float fTimeDelta)
{


	//잡기가 끊겼으면 실패
	if (m_pOwner->Check_bCurAnimationisGrab() == false || m_pOwner->Get_bStun())
		return;


	//잡기 vs 잡기
	if (other->m_ColliderGroup == CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack || other->m_ColliderGroup == CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack)
	{

		AttackType eAttackType = static_cast<CAttackObject*>(other->GetMineGameObject())->Get_AttackType();
		if (eAttackType == ATTACKTYPE_GRAB_GROUND || eAttackType == ATTACKTYPE_GRAB_AIR)
		{

			m_pOwner->Set_Animation(m_iOnwerNextAnimationIndex, false);
			m_pOwner->Set_CurrentAnimationPositionJump(m_fGrabAnimationPosition);
			m_pOwner->Character_Start_QTE(0);
			

			//pCharacter->Set_Animation(m_iOnwerNextAnimationIndex, false);
			//pCharacter->Set_CurrentAnimationPositionJump(m_fGrabAnimationPosition);

			//m_pOwner->Add_Move({ 0.0f * m_pOwner->Get_iDirection(),0.3f});

			//m_pOwner->Add_Move({ -0.15f * m_pOwner->Get_iDirection(),0.3f});
			//m_pOwner->Add_Move({ -0.14f * m_pOwner->Get_iDirection(),0.f });

			

			_float fEnemyPosX = static_cast<CAttackObject_Grab*>(other->GetMineGameObject())->Get_pOwner()->Get_fPositionX();
			_float MyPosX = m_pOwner->Get_fPositionX();

			_float currentDistance = fEnemyPosX - MyPosX;
			_float fDistance = m_pOwner->Get_iDirection() - (m_pOwner->Get_iDirection() > 0 ? 0.8 : -0.8); // 방향 고려
			m_pOwner->Add_Move({ fDistance,0.f });

			//m_pOwner->Set_GrabLoofCount(1);
			//m_pOwner->Set_GrabLoofCount(10);
			m_pOwner->Set_GrabLoofCount(15);

			//m_pOwner->Add_Move({ -0.15f * m_pOwner->Get_iDirection(),0.3f });


			m_pOwner->Set_bGrabDraw(true);
			CCharacter* pCharacter = static_cast<CCharacter*>(other->GetMineGameObject());
			Camera_Same_Grab(m_pOwner, pCharacter);
		}

	}


	//잡기 vs 사람 
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

			pCharacter->Set_GroundSmash(m_bGroundSmash);

			if (m_bOnwerHitNoneStop == false)
				m_pOwner->Set_AnimationStop(m_fAnimationLockTime);

			m_pOwner->Gain_AttackStep(m_iGainAttackStep);
			m_pOwner->Gain_HitCount(m_iGainHitCount);

			m_pOwner->Set_GrabLoofCount(2);

			pCharacter->Set_bGrabbed(true);

			_vector vPos = static_cast<CTransform*>(m_pOwner->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
			vPos += _vector{ m_fDistance.x, m_fDistance.y , 0.f, 0.f };
			static_cast<CTransform*>(pCharacter->Get_Component(TEXT("Com_Transform")))->Set_State(CTransform::STATE_POSITION, vPos);

			//if (m_bOwnerNextAnimation)
			{
				//m_pOwner->Set_NextAnimation(m_iOnwerNextAnimationIndex, 1.f);

				m_pOwner->Set_Animation(m_iOnwerNextAnimationIndex, false);
				m_pOwner->Set_CurrentAnimationPositionJump(m_fGrabAnimationPosition);

			}

			//카메라 그랩
			Camera_Grab(m_pOwner, pCharacter);

		}
		else if (eResult == RESULT_GUARD) //가드당해도 충돌은 했으니 시간정지연출
		{
			m_pOwner->Set_AnimationStop(0.08f);
			pCharacter->Set_AnimationStop(0.08f);
		}

		else if (eResult == RESULT_DRAW)
		{
			//둘다 같이 잡기 했을 때 카메라 셋팅,, 그다음에 여기 말고 아래 Draw때 한번
			//만든 각도는 동일하게

			//m_pOwner->Set_AnimationStop(0.3f);
			//pCharacter->Set_AnimationStop(0.3f);

			m_pOwner->Add_Move({ 0.0f * m_pOwner->Get_iDirection(),0.3f });
			m_pOwner->Set_GrabLoofCount(1);
			m_pOwner->Set_bGrabDraw(true);

			m_pOwner->Set_Animation(m_iOnwerNextAnimationIndex, false);
			m_pOwner->Set_CurrentAnimationPositionJump(m_fGrabAnimationPosition);

			//거리조
			_vector vPos = static_cast<CTransform*>(m_pOwner->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
			vPos += _vector{ m_fDistance.x * 2.3f,0.f , 0.f, 0.f };
			static_cast<CTransform*>(pCharacter->Get_Component(TEXT("Com_Transform")))->Set_State(CTransform::STATE_POSITION, vPos);

			pCharacter->Grab_LateDraw();

			Camera_Same_Grab(m_pOwner, pCharacter);
		}

		else if (eResult == RESULT_MISS)
		{
			//잡기는 한번 빗나가면 끝
			//if (m_eAttackType == ATTACKTYPE_GRAB_GROUND || m_eAttackType == ATTACKTYPE_GRAB_AIR)
			{
				//if (m_bEnableDestory)
				//{
				//	CGameInstance::Get_Instance()->Destroy_Reserve(m_pColliderCom);
				//	m_bEnableDestory = false;
				//}
			}

		}

		//if (m_bEnableDestory)
		//{
		//	CGameInstance::Get_Instance()->Destroy_Reserve(m_pColliderCom);
		//	m_bEnableDestory = false;
		//}
	}





}

void CAttackObject_Grab::OnCollisionStay(CCollider* other, _float fTimeDelta)
{
	_bool Debug = true;

}

void CAttackObject_Grab::OnCollisionExit(CCollider* other)
{
	_bool Debug = true;

}

void CAttackObject_Grab::CollisingAttack()
{
}

void CAttackObject_Grab::CollisingPlayer()
{
}

void CAttackObject_Grab::Camera_Grab(CCharacter* pOwner, CCharacter* pHitOwner)
{
	CMain_Camera* main_Camera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));

	CCharacter::Character_INFO_DESC characterDesc = pOwner->Get_PawnDesc();
	CUI_Define::PLAYER_ID PlayerID = characterDesc.ePlayerID;

	m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Common_Grab_Rush_SFX, false, 1.f);

	switch (PlayerID)
	{
	case Client::CUI_Define::GOKU:
		main_Camera->Play(CMain_Camera::VIRTUAL_CAMERA::VIRTUAL_CAMERA_SON_GRAB, 0, pOwner);
		main_Camera->StartCameraShake(2.5f, 0.07f);
		break;
	case Client::CUI_Define::ANDROID21:
		main_Camera->Play(CMain_Camera::VIRTUAL_CAMERA::VIRTUAL_CAMERA_21_GRAB, 0, pOwner);
		main_Camera->StartCameraShake(2.5f, 0.07f);
		m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Grab_Attack_21, false, 1.f);

		break;
	case Client::CUI_Define::FRIEZA:
		main_Camera->Play(CMain_Camera::VIRTUAL_CAMERA::VIRTUAL_CAMERA_FRIEZA_GRAB, 0, pOwner);
		main_Camera->StartCameraShake(2.5f, 0.07f);
		m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Frieza_Grab, false, 1.f);
		break;
	case Client::CUI_Define::HIT:
		main_Camera->Play(CMain_Camera::VIRTUAL_CAMERA::VIRTUAL_CAMERA_HIT_GRAB, 0, pOwner);
		main_Camera->StartCameraShake(2.5f, 0.07f);
		break;
	}
}

void CAttackObject_Grab::Camera_Same_Grab(CCharacter* pOwner, CCharacter* pHitOwner)
{
	CMain_Camera* main_Camera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));

	CCharacter::Character_INFO_DESC characterDesc = pOwner->Get_PawnDesc();
	CUI_Define::PLAYER_ID PlayerID = characterDesc.ePlayerID;

	m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Common_Grab_Rush_SFX, false, 1.f);

	switch (PlayerID)
	{
	case Client::CUI_Define::GOKU:
		main_Camera->Play(CMain_Camera::VIRTUAL_CAMERA::VIRTUAL_CAMERA_SON_SAME_GRAB, 0, pOwner);
		main_Camera->StartCameraShake(2.5f, 0.07f);
		break;
	case Client::CUI_Define::ANDROID21:
		main_Camera->Play(CMain_Camera::VIRTUAL_CAMERA::VIRTUAL_CAMERA_21_SAME_GRAB, 0, pOwner);
		main_Camera->StartCameraShake(2.5f, 0.07f);
		break;
	case Client::CUI_Define::FRIEZA:
		main_Camera->Play(CMain_Camera::VIRTUAL_CAMERA::VIRTUAL_CAMERA_SON_SAME_GRAB, 0, pOwner);
		main_Camera->StartCameraShake(2.5f, 0.07f);
		break;
	case Client::CUI_Define::HIT:
		main_Camera->Play(CMain_Camera::VIRTUAL_CAMERA::VIRTUAL_CAMERA_21_SAME_GRAB, 0, pOwner);
		main_Camera->StartCameraShake(2.5f, 0.07f);
		break;
	}
}







CAttackObject_Grab* CAttackObject_Grab::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAttackObject_Grab* pInstance = new CAttackObject_Grab(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CAttackObject_Grab"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CAttackObject_Grab::Clone(void* pArg)
{
	CAttackObject_Grab* pInstance = new CAttackObject_Grab(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CAttackObject_Grab"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAttackObject_Grab::Free()
{
	__super::Free();
	Safe_Release(m_pColliderCom);

}
