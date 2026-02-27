#include "stdafx.h"
#include "WeaponPart.h"

#include "Character.h"

CWeaponPart::CWeaponPart(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : BASE(pDevice, pContext)
{
}

CWeaponPart::CWeaponPart(const CWeaponPart& rhs)
    : BASE(rhs)
{
}

HRESULT CWeaponPart::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        RETURN_EFAIL;

    return S_OK;
}

HRESULT CWeaponPart::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        RETURN_EFAIL;

    return S_OK;
}

void CWeaponPart::Begin_Play(_cref_time fTimeDelta)
{
    __super::Begin_Play(fTimeDelta);

}

void CWeaponPart::Priority_Tick(_cref_time fTimeDelta)
{
    __super::Priority_Tick(fTimeDelta);

}

void CWeaponPart::Tick(_cref_time fTimeDelta)
{
    __super::Tick(fTimeDelta);

}

void CWeaponPart::Late_Tick(_cref_time fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);

}

void CWeaponPart::Before_Render(_cref_time fTimeDelta)
{
    __super::Before_Render(fTimeDelta);

}

void CWeaponPart::End_Play(_cref_time fTimeDelta)
{
    __super::End_Play(fTimeDelta);

}

HRESULT CWeaponPart::Render()
{

    return S_OK;
}


void CWeaponPart::OnCollision_Enter(CCollider* pThisCol, CCollider* pOtherCol)
{
    __super::OnCollision_Enter(pThisCol, pOtherCol);

}

void CWeaponPart::OnCollision_Stay(CCollider* pThisCol, CCollider* pOtherCol)
{
    __super::OnCollision_Stay(pThisCol, pOtherCol);

}

void CWeaponPart::OnCollision_Exit(CCollider* pThisCol, CCollider* pOtherCol)
{
    __super::OnCollision_Exit(pThisCol, pOtherCol);

}

void CWeaponPart::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
    __super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);

}

void CWeaponPart::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
    __super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);

}

void CWeaponPart::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
    __super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);

}

void CWeaponPart::Activate_Trail(_bool bCheck)
{
    if (m_pWeaponTrail == nullptr)
        return;

	if (bCheck)
	{
		if (!m_pWeaponTrail->IsState(OBJSTATE::Active))
		{
            m_pWeaponTrail->TurnOn_State(OBJSTATE::Active);
		}
	}
	else {
		if (m_pWeaponTrail->IsState(OBJSTATE::Active))
		{
            m_pWeaponTrail->Trail_Pos_Reset();
            m_pWeaponTrail->TurnOff_State(OBJSTATE::Active);
		}
	}
}

HRESULT CWeaponPart::Set_StatusComByGameObject(shared_ptr<CGameObject> pGameObject)
{
    CGameObject* pObj = pGameObject.get();

    CCharacter* pChr = DynCast<CCharacter*>(pObj);
    if (nullptr != pChr)
    {
        m_pStatusCom = pChr->Get_StatusCom();
        if (m_pStatusCom.expired())
            assert(false);
    }
    else
        assert(false);

    return S_OK;
}

HRESULT CWeaponPart::Set_StatusComByOwner(const string& strSkillName)
{
    if (m_pOwner.expired())
        return E_FAIL;

    CGameObject* pObj = m_pOwner.lock().get();

    CCharacter* pChr = DynCast<CCharacter*>(pObj);
    if (nullptr != pChr)
    {
        m_pStatusCom = pChr->Get_StatusCom();
        if (m_pStatusCom.expired())
            assert(false);
    }
    else
        assert(false);

    m_strSkillName = strSkillName;

    return S_OK;
}

weak_ptr<class CStatusComp> CWeaponPart::Get_StatusCom()
{
    if (!m_pShaderCom)
        Set_StatusComByOwner(m_strSkillName);

    return m_pStatusCom;
}

void CWeaponPart::Free()
{
    __super::Free();

}
