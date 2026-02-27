#include "stdafx.h"
#include "Guard_Hound_WhipColl.h"
#include "GameInstance.h"
#include "Particle.h"
#include "Sound_Manager.h"

IMPLEMENT_CREATE(CGuard_Hound_WhipColl)
IMPLEMENT_CLONE(CGuard_Hound_WhipColl, CGameObject)

CGuard_Hound_WhipColl::CGuard_Hound_WhipColl(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : BASE(pDevice, pContext)
{
}

CGuard_Hound_WhipColl::CGuard_Hound_WhipColl(const CGuard_Hound_WhipColl& rhs)
    : BASE(rhs)
{
}

HRESULT CGuard_Hound_WhipColl::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CGuard_Hound_WhipColl::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        RETURN_EFAIL;

    if (FAILED(Ready_Components(pArg)))
        RETURN_EFAIL;

    return S_OK;
}

void CGuard_Hound_WhipColl::Begin_Play(_cref_time fTimeDelta)
{
    __super::Begin_Play(fTimeDelta);

    PHYSXCOLLIDER_DESC ColliderDesc = {};
    PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc, PHYSXCOLLIDER_TYPE::BOX, CL_MONSTER_ATTACK, 
        m_pTransformCom, { 3.f,0.7f,0.5f}, false, nullptr, true);
    m_vPhysXColliderLocalOffset = { -1.f,0.0f,0.f };

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
        TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
        return;

   m_pPhysXColliderCom->PutToSleep();

   // 부모 스테이터스 연결 및 스킬 이름 설정(고정 스킬이름 사용시 여기에 사용)
   Set_StatusComByOwner("Hound_Whip");
}

void CGuard_Hound_WhipColl::Priority_Tick(_cref_time fTimeDelta)
{
    __super::Priority_Tick(fTimeDelta);
}

void CGuard_Hound_WhipColl::Tick(_cref_time fTimeDelta)
{
    __super::Tick(fTimeDelta);
}

void CGuard_Hound_WhipColl::Late_Tick(_cref_time fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

void CGuard_Hound_WhipColl::Before_Render(_cref_time fTimeDelta)
{
    __super::Before_Render(fTimeDelta);

    //if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this())))
    //    return;
}

void CGuard_Hound_WhipColl::End_Play(_cref_time fTimeDelta)
{
    __super::End_Play(fTimeDelta);
}

HRESULT CGuard_Hound_WhipColl::Render()
{
    return S_OK;
}

HRESULT CGuard_Hound_WhipColl::Ready_Components(void* pArg)
{
    return S_OK;
}

HRESULT CGuard_Hound_WhipColl::Bind_ShaderResources()
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

void CGuard_Hound_WhipColl::OnCollision_Enter(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CGuard_Hound_WhipColl::OnCollision_Stay(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CGuard_Hound_WhipColl::OnCollision_Exit(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CGuard_Hound_WhipColl::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
    __super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);

    if (pOtherCol->Get_ColliderDesc().iFilterType == CL_PLAYER_BODY)
    {
        // 대미지 주는 함수, IStatusInterface 인터페이스로 구현됨.
        Status_DamageTo(m_strSkillName, pOtherCol, pOtherCol->Get_Owner(), pThisCol->Get_Owner());

        // 사운드
        m_pGameInstance->Play_3DSound(TEXT("FF7_GuardHound_Effect"),
            { "045_SE_GuardHound (FX_DWhip_Hit_01).wav" },
            ESoundGroup::Effect, shared_from_this());

        /*PxContactPairPoint* pContactPoint = new PxContactPairPoint[ContactInfo.contactCount];
        PxU32 nbContacts = ContactInfo.extractContacts(pContactPoint, ContactInfo.contactCount);

        weak_ptr< CParticle> pEffect;
        if (nbContacts >= 1)
        {
            pEffect = GET_SINGLE(CObjPool_Manager)->Create_Object<CParticle>(TEXT("Cloud_KickHit"), L_EFFECT, nullptr, nullptr,
                Convert_Vector(pContactPoint[0].position));

            pEffect.lock()->Get_TransformCom().lock()->Set_Up(Convert_Vector(pContactPoint[0].normal));
        }

        Safe_Delete(pContactPoint);*/
    }
}

void CGuard_Hound_WhipColl::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
    __super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CGuard_Hound_WhipColl::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
    __super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}

void CGuard_Hound_WhipColl::Free()
{
    __super::Free();
}
