#include "stdafx.h"
#include "Sweeper_RushColl.h"
#include "GameInstance.h"
#include "Particle.h"
#include "Data_Manager.h"

#include "Sound_Manager.h"

IMPLEMENT_CREATE(CSweeper_RushColl)
IMPLEMENT_CLONE(CSweeper_RushColl, CGameObject)

CSweeper_RushColl::CSweeper_RushColl(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : BASE(pDevice, pContext)
{
}

CSweeper_RushColl::CSweeper_RushColl(const CSweeper_RushColl& rhs)
    : BASE(rhs)
{
}

HRESULT CSweeper_RushColl::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSweeper_RushColl::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        RETURN_EFAIL;

    if (FAILED(Ready_Components(pArg)))
        RETURN_EFAIL;

    return S_OK;
}

void CSweeper_RushColl::Begin_Play(_cref_time fTimeDelta)
{
    __super::Begin_Play(fTimeDelta);

    PHYSXCOLLIDER_DESC ColliderDesc = {};
    PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc, PHYSXCOLLIDER_TYPE::BOX, CL_MONSTER_ATTACK, m_pTransformCom, { 4.f,3.f,4.f }, false, nullptr, true);
     m_vPhysXColliderLocalOffset = { 0.f,-1.f,-1.f };

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
        TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
        return;

    m_pPhysXColliderCom->PutToSleep();

    // 스킬 설정
    Set_StatusComByOwner("Sweeper_Rush");
}

void CSweeper_RushColl::Priority_Tick(_cref_time fTimeDelta)
{
    __super::Priority_Tick(fTimeDelta);
}

void CSweeper_RushColl::Tick(_cref_time fTimeDelta)
{
    __super::Tick(fTimeDelta);
}

void CSweeper_RushColl::Late_Tick(_cref_time fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

void CSweeper_RushColl::Before_Render(_cref_time fTimeDelta)
{
    __super::Before_Render(fTimeDelta);

    //if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this())))
    //    return;
}

void CSweeper_RushColl::End_Play(_cref_time fTimeDelta)
{
    __super::End_Play(fTimeDelta);
}

HRESULT CSweeper_RushColl::Render()
{
    return S_OK;
}

HRESULT CSweeper_RushColl::Ready_Components(void* pArg)
{
    return S_OK;
}

HRESULT CSweeper_RushColl::Bind_ShaderResources()
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

void CSweeper_RushColl::OnCollision_Enter(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CSweeper_RushColl::OnCollision_Stay(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CSweeper_RushColl::OnCollision_Exit(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CSweeper_RushColl::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
    __super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);

    _bool bUseDamage = false;
    if (bUseDamage = (pOtherCol->Get_ColliderDesc().iFilterType == CL_PLAYER_BODY))
    {
        // 대미지 주는 함수, IStatusInterface 인터페이스로 구현됨.
        if (bUseDamage)
            Status_DamageTo(m_strSkillName, pOtherCol, pOtherCol->Get_Owner(), pThisCol->Get_Owner());

        m_pGameInstance->Play_3DSound(TEXT("FF7_Sweeper_Effect")
            , { "058_SE_Sweeper (Par_ChargeAtk_Hit_01).wav" }
        , ESoundGroup::Effect, shared_from_this());
    }
}

void CSweeper_RushColl::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
    __super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CSweeper_RushColl::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
    __super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}

void CSweeper_RushColl::Free()
{
    __super::Free();
}
