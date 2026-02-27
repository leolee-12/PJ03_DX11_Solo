#include "stdafx.h"
#include "Sweeper_FlameGas.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Client_Manager.h"
#include "Data_Manager.h"

IMPLEMENT_CREATE(CSweeper_FlameGas)
IMPLEMENT_CLONE(CSweeper_FlameGas, CGameObject)
CSweeper_FlameGas::CSweeper_FlameGas(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: CBullet(pDevice, pDeviceContext)
{
}

CSweeper_FlameGas::CSweeper_FlameGas(const CSweeper_FlameGas& rhs)
	: CBullet(rhs)
{
}

HRESULT CSweeper_FlameGas::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CSweeper_FlameGas::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	return S_OK;
}

void CSweeper_FlameGas::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);

	weak_ptr<CEffect> pEffect = GET_SINGLE(CEffect_Manager)->Create_Effect<CParticle>(TEXT("SweeperFlameGas"), shared_from_this());
}

void CSweeper_FlameGas::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CSweeper_FlameGas::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);


	m_fAccTime += fTimeDelta;
}

void CSweeper_FlameGas::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (2.5f <= m_fAccTime)
		Set_Dead();
}

void CSweeper_FlameGas::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
}

void CSweeper_FlameGas::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CSweeper_FlameGas::Render()
{
	return S_OK;
}

HRESULT CSweeper_FlameGas::Ready_Components(void* pArg)
{
	return S_OK;
}

void CSweeper_FlameGas::OnCollision_Enter(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CSweeper_FlameGas::OnCollision_Stay(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CSweeper_FlameGas::OnCollision_Exit(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CSweeper_FlameGas::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);
}

void CSweeper_FlameGas::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CSweeper_FlameGas::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}


void CSweeper_FlameGas::Free()
{
	__super::Free();
}
