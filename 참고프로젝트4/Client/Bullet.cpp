#include "stdafx.h"
#include "Bullet.h"

#include "Character.h"
#include "Boss/Boss_Parts.h"

#include "StatusComp.h"
#include "PhysX_Collider.h"
#include "UI_Manager.h"

IMPLEMENT_CREATE(CBullet)
IMPLEMENT_CLONE(CBullet, CGameObject)

CBullet::CBullet(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CDynamicObject(pDevice, pContext)
{
}

CBullet::CBullet(const CBullet& rhs)
	: CDynamicObject(rhs)
{
}

HRESULT CBullet::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;
	return S_OK;
}

HRESULT CBullet::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;
	if (FAILED(Ready_Components(pArg)))
		RETURN_EFAIL;

	return S_OK;
}

void CBullet::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);
}

void CBullet::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CBullet::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CBullet::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CBullet::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
}

void CBullet::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CBullet::Render()
{
	return S_OK;
}

HRESULT CBullet::Ready_Components(void* pArg)
{
	return S_OK;
}

void CBullet::OnCollision_Enter(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CBullet::OnCollision_Stay(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CBullet::OnCollision_Exit(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CBullet::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);
}

void CBullet::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);

}

void CBullet::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}

HRESULT CBullet::Set_StatusComByGameObject(shared_ptr<CGameObject> pGameObject)
{
	CGameObject* pObj = pGameObject.get();

	CCharacter* pChr = DynCast<CCharacter*>(pObj);
	if (nullptr != pChr)
	{
		m_pStatusCom = pChr->Get_StatusCom().lock();
	}
	else
		assert(false);

	return S_OK;
}

HRESULT CBullet::Set_StatusComByOwner(const string& strSkillName)
{
	if (m_pOwner.expired())
		return E_FAIL;

	CGameObject* pObj = m_pOwner.lock().get();

	CCharacter* pChr = DynCast<CCharacter*>(pObj);
	CBoss_Parts* pBossParts = DynCast<CBoss_Parts*>(pObj);
	if (nullptr != pChr)
	{
		m_pStatusCom = pChr->Get_StatusCom().lock();
	}
	else if (pBossParts != nullptr)
	{
		m_pStatusCom = pBossParts->Get_StatusCom().lock();
	}
	else 
		assert(false);

	m_strSkillName = strSkillName;

	return S_OK;
}

weak_ptr<class CStatusComp> CBullet::Get_StatusCom()
{
	if (!m_pShaderCom)
		Set_StatusComByOwner(m_strSkillName);

	return m_pStatusCom;
}

void CBullet::Free()
{
	__super::Free();
}
