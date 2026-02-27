#include "stdafx.h"
#include "Guard_Hound_BiteColl.h"
#include "GameInstance.h"
#include "Effect_Manager.h"
#include "Particle.h"
#include "Sound_Manager.h"

IMPLEMENT_CREATE(CGuard_Hound_BiteColl)
IMPLEMENT_CLONE(CGuard_Hound_BiteColl, CGameObject)

CGuard_Hound_BiteColl::CGuard_Hound_BiteColl(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : BASE(pDevice, pContext)
{
}

CGuard_Hound_BiteColl::CGuard_Hound_BiteColl(const CGuard_Hound_BiteColl& rhs)
    : BASE(rhs)
{
}

HRESULT CGuard_Hound_BiteColl::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CGuard_Hound_BiteColl::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        RETURN_EFAIL;

    if (FAILED(Ready_Components(pArg)))
        RETURN_EFAIL;

    return S_OK;
}

void CGuard_Hound_BiteColl::Begin_Play(_cref_time fTimeDelta)
{
    __super::Begin_Play(fTimeDelta);

    PHYSXCOLLIDER_DESC ColliderDesc = {};
    PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc, PHYSXCOLLIDER_TYPE::BOX, CL_MONSTER_ATTACK, m_pTransformCom, { 1.f,1.f,1.f }, false, nullptr, true);
   // m_vPhysXColliderLocalOffset = { 0.f,1.0f,0.f };

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
        TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
        return;

    m_pPhysXColliderCom->PutToSleep();

    Set_StatusComByOwner("Hound_Bite");
}

void CGuard_Hound_BiteColl::Priority_Tick(_cref_time fTimeDelta)
{
    __super::Priority_Tick(fTimeDelta);
}

void CGuard_Hound_BiteColl::Tick(_cref_time fTimeDelta)
{
    __super::Tick(fTimeDelta);
}

void CGuard_Hound_BiteColl::Late_Tick(_cref_time fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

void CGuard_Hound_BiteColl::Before_Render(_cref_time fTimeDelta)
{
    __super::Before_Render(fTimeDelta);

    //if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this())))
    //    return;
}

void CGuard_Hound_BiteColl::End_Play(_cref_time fTimeDelta)
{
    __super::End_Play(fTimeDelta);
}

HRESULT CGuard_Hound_BiteColl::Render()
{
    return S_OK;
}

HRESULT CGuard_Hound_BiteColl::Ready_Components(void* pArg)
{
    return S_OK;
}

HRESULT CGuard_Hound_BiteColl::Bind_ShaderResources()
{
    // 공통 행렬 바인딩
    if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_VIEW))))
        RETURN_EFAIL;
    if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_PROJ))))
        RETURN_EFAIL;

    // 월드 행렬 바인딩
    if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        RETURN_EFAIL;

    return S_OK;
}

void CGuard_Hound_BiteColl::OnCollision_Enter(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CGuard_Hound_BiteColl::OnCollision_Stay(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CGuard_Hound_BiteColl::OnCollision_Exit(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CGuard_Hound_BiteColl::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
    __super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);

    _bool bCollidePlayer = false;
    if ((bCollidePlayer = (pOtherCol->Get_ColliderDesc().iFilterType == CL_PLAYER_BODY)))
    {
        PxContactPairPoint* pContactPoint = new PxContactPairPoint[ContactInfo.contactCount];
        PxU32 nbContacts = ContactInfo.extractContacts(pContactPoint, ContactInfo.contactCount);
        // 대미지 주는 함수, IStatusInterface 인터페이스로 구현됨.
        if (bCollidePlayer)
            Status_DamageTo(m_strSkillName, pOtherCol, pOtherCol->Get_Owner(), pThisCol->Get_Owner());

        if (nbContacts >= 1)
        {
            GET_SINGLE(CEffect_Manager)->Create_Hit_Effect<CEffect_Group>(L"PlayerHit(Jiho)", shared_from_this(),
                Convert_Vector(pContactPoint[0].position), Convert_Vector(pContactPoint[0].normal));
        }

        // 사운드
        m_pGameInstance->Play_3DSound(TEXT("FF7_GuardHound_Effect"),
            { "038_SE_GuardHound (FX_Claw_Hit_01).wav" },
            ESoundGroup::Effect, shared_from_this());
    }
}

void CGuard_Hound_BiteColl::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
    __super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CGuard_Hound_BiteColl::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
    __super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}

void CGuard_Hound_BiteColl::Free()
{
    __super::Free();
}
