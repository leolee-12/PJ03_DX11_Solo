#include "stdafx.h"
#include "Aerith/Parts/Aerith_Weapon.h"
#include "Aerith/State/State_List_Aerith.h"

#include "GameInstance.h"
#include "Client_Manager.h"
#include "Particle.h"
#include "StatusComp.h"

IMPLEMENT_CREATE(CAerith_Weapon)
IMPLEMENT_CLONE(CAerith_Weapon, CGameObject)

CAerith_Weapon::CAerith_Weapon(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : BASE(pDevice, pContext)
{
}

CAerith_Weapon::CAerith_Weapon(const CAerith_Weapon& rhs)
    : BASE(rhs)
{
}

HRESULT CAerith_Weapon::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CAerith_Weapon::Initialize(void* pArg)
{
	m_ePartObjType = PART_OBJTYPE::PART_WEAPON;

    if (FAILED(__super::Initialize(pArg)))
        RETURN_EFAIL;

    if (FAILED(Ready_Components(pArg)))
        RETURN_EFAIL;

    return S_OK;
}

void CAerith_Weapon::Begin_Play(_cref_time fTimeDelta)
{
    __super::Begin_Play(fTimeDelta);

    //Transform에 기록된 크기, 위치, 회전값으로 BOX 생성
    PHYSXCOLLIDER_DESC ColliderDesc = {};
    PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc, PHYSXCOLLIDER_TYPE::BOX, CL_PLAYER_ATTACK, m_pTransformCom, _float3(1.6f, 0.3f, 0.1f), false, nullptr, true);
    m_vPhysXColliderLocalOffset = { 0.8f,0.0f,0.0f };

    if (FAILED(__super::Add_Component(0, TEXT("Prototype_Component_PhysX_Collider"),
        TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
        return;


    m_pPhysXColliderCom->PutToSleep();
}

void CAerith_Weapon::Priority_Tick(_cref_time fTimeDelta)
{
    __super::Priority_Tick(fTimeDelta);
}

void CAerith_Weapon::Tick(_cref_time fTimeDelta)
{
    __super::Tick(fTimeDelta);

    if (m_fTrailTimer.Update(fTimeDelta))
    {
        _float3 vPos = Get_WorldMatrixFromBone(TEXT("L_RodAGimmickS_End")).r[3];
        _float3 vLook = Get_TransformCom().lock()->Get_State(CTransform::STATE_LOOK);
        /*GET_SINGLE(CEffect_Manager)
            ->Create_Hit_Effect<CParticle>(L"AerithRod_Trail", shared_from_this(), vPos, vLook);*/
		GET_SINGLE(CEffect_Manager)
			->Create_Effect<CParticle>(L"Aerith_Weapon_Always", shared_from_this(),CEffect::USE_FOLLOW_PARTS);
    }
}

void CAerith_Weapon::Late_Tick(_cref_time fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);

    /*if (m_pModelCom)
        m_pModelCom->Tick_AnimTime(fTimeDelta);*/
}

void CAerith_Weapon::Before_Render(_cref_time fTimeDelta)
{
    __super::Before_Render(fTimeDelta);

    if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this())))
        return;

    if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW, shared_from_this())))
        return;

}

void CAerith_Weapon::End_Play(_cref_time fTimeDelta)
{
    __super::End_Play(fTimeDelta);
}

HRESULT CAerith_Weapon::Render()
{
    if (m_pModelCom)
    {
        if (FAILED(Bind_ShaderResources()))
            RETURN_EFAIL;

        auto ActiveMeshes = m_pModelCom->Get_ActiveMeshes();
        for (_uint i = 0; i < ActiveMeshes.size(); ++i)
        {
            // 뼈 바인딩
            if (FAILED(m_pModelCom->Bind_BoneToShader(ActiveMeshes[i], "g_BoneMatrices")))
                RETURN_EFAIL;

            // 텍스처 바인딩
            if (FAILED(m_pModelCom->Bind_MeshMaterialToShader(ActiveMeshes[i], TextureType_DIFFUSE, "g_DiffuseTexture")))
                RETURN_EFAIL;
            if (FAILED(m_pModelCom->Bind_MeshMaterialToShader(ActiveMeshes[i], TextureType_NORMALS, "g_NormalTexture")))
                RETURN_EFAIL;
            if (FAILED(m_pModelCom->Bind_MeshMaterialToShader(ActiveMeshes[i], TextureType_DIFFUSE_ROUGHNESS, "g_MRVTexture")))
                RETURN_EFAIL;
            if (FAILED(m_pModelCom->Bind_MeshMaterialToShader(ActiveMeshes[i], TextureType_EMISSION_COLOR, "g_EmissiveTexture")))
                RETURN_EFAIL;
            if (FAILED(m_pModelCom->Bind_MeshMaterialToShader(ActiveMeshes[i], TextureType_AMBIENT_OCCLUSION, "g_AOTexture")))
                RETURN_EFAIL;
            if (FAILED(m_pModelCom->Bind_MeshMaterialToShader(ActiveMeshes[i], TextureType_OPACITY, "g_AlphaTexture")))
                RETURN_EFAIL;


            // 패스, 버퍼 바인딩
            if (FAILED(m_pModelCom->ShaderComp()->Begin(0)))
                RETURN_EFAIL;

            //버퍼 렌더
            if (FAILED(m_pModelCom->BindAndRender_Mesh(ActiveMeshes[i])))
                RETURN_EFAIL;
        }
    }
    return S_OK;
}

HRESULT CAerith_Weapon::Ready_Components(void* pArg)
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_AnimCommonModel"),
        TEXT("Com_ModelCom"), &(m_pModelCom), nullptr)))
        RETURN_EFAIL;

    if (m_pModelCom)
    {
       if (FAILED( m_pModelCom->Link_Model(CCommonModelComp::TYPE_ANIM, TEXT("WE0003_00_Aerith_GuardRod"))))
           RETURN_EFAIL;
        if (FAILED(m_pModelCom->Link_Shader(L"Shader_AnimModel", nullptr, 0)))
           RETURN_EFAIL;

        m_pModelCom->Set_PreRotate(
            XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f)));

        //m_pModelCom->Set_Animation(0, 1.f, false);
        //m_pModelCom->Tick_AnimTime(0.f);
    }

    return S_OK;
}

HRESULT CAerith_Weapon::Bind_ShaderResources()
{
    // 월드 행렬 바인딩
    if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        RETURN_EFAIL;
    // 공통 행렬 바인딩
    if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_VIEW))))
        RETURN_EFAIL;
    if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_PROJ))))
        RETURN_EFAIL;

    return S_OK;
}

void CAerith_Weapon::OnCollision_Enter(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CAerith_Weapon::OnCollision_Stay(CCollider* pThisCol, CCollider* pOtherCol)
{
}                  

void CAerith_Weapon::OnCollision_Exit(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CAerith_Weapon::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
    CState* pCurState = m_pOwner.lock()->Get_StateMachineCom().lock()->Get_CurState();
 
    if (typeid(*pCurState) != typeid(CState_Aerith_AbilityAttack_RayOfJudge)
        && typeid(*pCurState) != typeid(CState_Aerith_AbilityAttack_SorcerousStorm)
        && typeid(*pCurState) != typeid(CState_Aerith_AbilityAttack_LustrousShield)
        && typeid(*pCurState) != typeid(CState_Aerith_LimitAttack_GuardianOfGuns))
    {
        PxContactPairPoint* pContactPoint = new PxContactPairPoint[ContactInfo.contactCount];
        PxU32 nbContacts = ContactInfo.extractContacts(pContactPoint, ContactInfo.contactCount);

        if (pOtherCol->Get_ColliderDesc().iFilterType == CL_MONSTER_BODY)
        {
            if (DynPtrCast<IStatusInterface>(pOtherCol->Get_Owner().lock())->Get_StatusCom().lock()->IsZeroHP())
                return;

            weak_ptr< CParticle> pEffect;
            if (nbContacts >= 1)
            {
                GET_SINGLE(CClient_Manager)->Register_Event_AdjustTimeDelta(m_pOwner, 0.3f, 0.1f);
                pEffect = GET_SINGLE(CObjPool_Manager)->Create_Object<CParticle>(TEXT("CloudSpark4"), L_EFFECT, nullptr, nullptr,
                    Convert_Vector(pContactPoint[0].position));

                pEffect.lock()->Get_TransformCom().lock()->Set_Up(Convert_Vector(pContactPoint[0].normal));
            }
        }
        else if (pOtherCol->Get_ColliderDesc().iFilterType == CL_MAP_DYNAMIC)
        {
            _float3 vForce = pContactPoint[0].impulse == PxVec3(0.f) ?  Convert_Vector(pContactPoint[0].normal * 500.f) : Convert_Vector(pContactPoint[0].impulse);
            pOtherCol->Add_Force(vForce);
        }

        Safe_Delete(pContactPoint);
         
        
      

        /*if (typeid(m_pOwner.lock()->Get_StateMachineCom().lock()->Get_CurState()) == typeid(CState_Cloud_AbilityAttack_Infinity))
        {
            pEffect = GET_SINGLE(CObjPool_Manager)->Create_Object<CEffect_Group>(TEXT("InfiniteEndHit2"), L_EFFECT, nullptr, nullptr,
                pOtherCol->Get_Position() - pOtherCol->Get_Offset());
            pEffect.lock()->Get_TransformCom().lock()->Set_Look(m_pOwner.lock()->Get_TransformCom().lock()->Get_State(CTransform::STATE_LOOK));
        }*/
    }  
}

void CAerith_Weapon::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
}

void CAerith_Weapon::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
}

void CAerith_Weapon::Free()
{
    __super::Free();
}
