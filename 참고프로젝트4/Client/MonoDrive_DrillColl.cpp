#include "stdafx.h"
#include "MonoDrive_DrillColl.h"
#include "GameInstance.h"
#include "Particle.h"
#include "Data_Manager.h"

IMPLEMENT_CREATE(CMonoDrive_DrillColl)
IMPLEMENT_CLONE(CMonoDrive_DrillColl, CGameObject)

CMonoDrive_DrillColl::CMonoDrive_DrillColl(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : BASE(pDevice, pContext)
{
}

CMonoDrive_DrillColl::CMonoDrive_DrillColl(const CMonoDrive_DrillColl& rhs)
    : BASE(rhs)
{
}

HRESULT CMonoDrive_DrillColl::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMonoDrive_DrillColl::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        RETURN_EFAIL;

    if (FAILED(Ready_Components(pArg)))
        RETURN_EFAIL;

    return S_OK;
}

void CMonoDrive_DrillColl::Begin_Play(_cref_time fTimeDelta)
{
    __super::Begin_Play(fTimeDelta);

    PHYSXCOLLIDER_DESC ColliderDesc = {};
    PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc, PHYSXCOLLIDER_TYPE::BOX, CL_MONSTER_ATTACK, m_pTransformCom, { 0.5f,0.3f,0.3f }, false, nullptr, true);
   // m_vPhysXColliderLocalOffset = { 0.f,1.0f,0.f };

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
        TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
        return;

    m_pPhysXColliderCom->PutToSleep();

    // 부모 스테이터스 연결 및 스킬 이름 설정(고정 스킬이름 사용시 여기에 사용)
    Set_StatusComByOwner("MonoDrive_Drill");
}

void CMonoDrive_DrillColl::Priority_Tick(_cref_time fTimeDelta)
{
    __super::Priority_Tick(fTimeDelta);

    if (m_bCollision)
    {
        _float3 vLook = XMVector3Normalize(m_pOwner.lock()->Get_TransformCom().lock()->Get_State(CTransform::STATE_LOOK));
        _float3 vPos = m_pOwner.lock()->Get_TransformCom().lock()->Get_State(CTransform::STATE_POSITION);
        vLook *= m_fOffSet;
        vPos.x += vLook.x;
        vPos.z += vLook.z;
        m_pOwner.lock()->Get_TransformCom().lock()->Set_Position(1, vPos);

        m_bCollision = false;
    }
}

void CMonoDrive_DrillColl::Tick(_cref_time fTimeDelta)
{
    __super::Tick(fTimeDelta);
}

void CMonoDrive_DrillColl::Late_Tick(_cref_time fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

void CMonoDrive_DrillColl::Before_Render(_cref_time fTimeDelta)
{
    __super::Before_Render(fTimeDelta);

    //if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this())))
    //    return;
}

void CMonoDrive_DrillColl::End_Play(_cref_time fTimeDelta)
{
    __super::End_Play(fTimeDelta);
}

HRESULT CMonoDrive_DrillColl::Render()
{
    return S_OK;
}

HRESULT CMonoDrive_DrillColl::Ready_Components(void* pArg)
{
    return S_OK;
}

HRESULT CMonoDrive_DrillColl::Bind_ShaderResources()
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

void CMonoDrive_DrillColl::OnCollision_Enter(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CMonoDrive_DrillColl::OnCollision_Stay(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CMonoDrive_DrillColl::OnCollision_Exit(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CMonoDrive_DrillColl::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
    __super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);

    if (pOtherCol->Get_ColliderDesc().iFilterType == CL_PLAYER_BODY)
    {
        // 대미지 주는 함수, IStatusInterface 인터페이스로 구현됨.
        Status_DamageTo(m_strSkillName, pOtherCol, pOtherCol->Get_Owner(), pThisCol->Get_Owner());

        m_bCollision = true;
    }
}

void CMonoDrive_DrillColl::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
    __super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CMonoDrive_DrillColl::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
    __super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}

void CMonoDrive_DrillColl::Free()
{
    __super::Free();
}
