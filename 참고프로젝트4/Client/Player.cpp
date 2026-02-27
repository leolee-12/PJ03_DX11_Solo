#include "stdafx.h"
#include "Player.h"
#include "Client_Manager.h"
#include "StateMachine.h"

IMPLEMENT_CREATE(CPlayer)
IMPLEMENT_CLONE(CPlayer, CGameObject)

CPlayer::CPlayer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CCharacter(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	:CCharacter(rhs)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;
	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	if (FAILED(Ready_Components(pArg)))
		RETURN_EFAIL;

	return S_OK;
}

void CPlayer::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);
}

void CPlayer::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
	
	// 클라이언트 매니저에 플레이어의 상태 이름을 업로드
	//GET_SINGLE(CClient_Manager)->Set_CurrentPlayerState(m_ePlayerType, m_pStateMachineCom->Get_CurStateName());
}

void CPlayer::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CPlayer::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CPlayer::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
}

void CPlayer::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CPlayer::Render()
{
	return S_OK;
}

HRESULT CPlayer::Ready_Components(void* pArg)
{
	return S_OK;
}

HRESULT CPlayer::Bind_ShaderResources()
{
	return S_OK;
}

void CPlayer::OnCollision_Enter(CCollider* pThisCol, CCollider* pOtherCol)
{
	__super::OnCollision_Enter(pThisCol, pOtherCol);
}

void CPlayer::OnCollision_Stay(CCollider* pThisCol, CCollider* pOtherCol)
{
	__super::OnCollision_Stay(pThisCol, pOtherCol);
}

void CPlayer::OnCollision_Exit(CCollider* pThisCol, CCollider* pOtherCol)
{
	__super::OnCollision_Exit(pThisCol, pOtherCol);
}

void CPlayer::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);
}

void CPlayer::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CPlayer::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}

void CPlayer::PhysX_Raycast_Stay(weak_ptr<CGameObject> pOther, _uint iOtherColLayer, _float4 vOrigin, _float4 vDirection, _float4 vColPosition)
{
	__super::PhysX_Raycast_Stay(pOther,iOtherColLayer, vOrigin, vDirection, vColPosition);

}

void CPlayer::Set_State_AIMode(_bool bAIMode)
{
}

void CPlayer::Free()
{
	__super::Free();
}