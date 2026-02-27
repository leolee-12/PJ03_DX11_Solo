#include "stdafx.h"
#include "Cloud_Weapon.h"
#include "GameInstance.h"
#include "Client_Manager.h"
#include "Particle.h"
#include "State_List_Cloud.h"
#include "StatusComp.h"
#include "Effect_Manager.h"

#include "Sound_Manager.h"
#include "Light.h"

IMPLEMENT_CREATE(CCloud_Weapon)
IMPLEMENT_CLONE(CCloud_Weapon, CGameObject)

CCloud_Weapon::CCloud_Weapon(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: BASE(pDevice, pContext)
{
}

CCloud_Weapon::CCloud_Weapon(const CCloud_Weapon& rhs)
	: BASE(rhs)
{
}

HRESULT CCloud_Weapon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCloud_Weapon::Initialize(void* pArg)
{
	m_ePartObjType = PART_OBJTYPE::PART_WEAPON;

	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	if (FAILED(Ready_Components(pArg)))
		RETURN_EFAIL;

	m_iSocketBoneIndices[HAND] = m_iBoneIndex;

	if (m_pBoneGroup != nullptr && m_pBoneGroup->CRef_BoneDatas_Count() > 0)
	{
		m_iSocketBoneIndices[BACK] = m_pBoneGroup->Find_BoneData(L"C_Weapon_b")->iID;
	}

	m_TimeChecker = FTimeChecker(0.2f);

	m_TImeChecker_Limit_Always_Spark = FTimeChecker(0.1f);

	m_TImeChecker_Limit_Always_Splash = FTimeChecker(0.05f);

	return S_OK;
}

void CCloud_Weapon::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);

	//Transform에 기록된 크기, 위치, 회전값으로 BOX 생성
	PHYSXCOLLIDER_DESC ColliderDesc = {};
	PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc, PHYSXCOLLIDER_TYPE::BOX, CL_PLAYER_ATTACK, m_pTransformCom, _float3(1.8f, 0.4f, 0.1f), false, nullptr, true);
	m_vPhysXColliderLocalOffset = { 0.9f,0.0f,0.0f };

	if (FAILED(__super::Add_Component(0, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
		return;

	m_pTransformCom->Rotation(0.f, PI * 0.5f, 0.f);

	m_pPhysXColliderCom->PutToSleep();

	// 임시 트레일 테스트
//#if EFFECT_LOAD == 0
//    m_pPartEffect = GET_SINGLE(CEffect_Manager)->Create_Effect<CTrail_Buffer>(L"ET_Cloud_Basic_Trail", shared_from_this());
//    TurnOff_PartEffect();
//
//    //m_pAlwaysEffect = GET_SINGLE(CEffect_Manager)->Create_Mesh_VTX_Particle(L"Cloud_Weapon_Always4", shared_from_this(), CEffect::USE_FOLLOW_PARTS);
//#endif
}

void CCloud_Weapon::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	if (m_pGameInstance->Get_CreateLevelIndex() == LEVEL_STAGE1 && m_bCheck)
	{
		m_bCheck = false;

#if EFFECT_LOAD == 0
		m_pPartEffect = GET_SINGLE(CEffect_Manager)->Create_Effect<CTrail_Buffer>(L"ET_Cloud_Basic_Trail", shared_from_this());
		TurnOff_PartEffect();

		//m_pAlwaysEffect = GET_SINGLE(CEffect_Manager)->Create_Mesh_VTX_Particle(L"Cloud_Weapon_Always4", shared_from_this(), CEffect::USE_FOLLOW_PARTS);
#endif
	}
	if (m_pGameInstance->Get_CurrentLevelIndex() == LEVEL_VILLAGE2)
	{
		if (m_pPartEffect.lock() != nullptr)
		{
			m_pPartEffect.lock()->Set_Dead();
			m_pPartEffect.lock().reset();
			m_bCheck = true;
		}
	}
	if (m_pGameInstance->Get_CreateLevelIndex() == LEVEL_BOSS1 && m_bCheck)
	{
		m_pPartEffect = GET_SINGLE(CEffect_Manager)->Create_Effect<CTrail_Buffer>(L"ET_Cloud_Basic_Trail", shared_from_this());
		TurnOff_PartEffect();
		m_bCheck = false;
	}
}

void CCloud_Weapon::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

#if EFFECT_LOAD == 0

	Ascension_Mode(fTimeDelta);

#endif
}

void CCloud_Weapon::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CCloud_Weapon::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this())))
		return;

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW, shared_from_this())))
		return;
}

void CCloud_Weapon::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);

	if (m_pAlwaysEffect)
		m_pAlwaysEffect->Set_Dead();
}

HRESULT CCloud_Weapon::Render()
{
	if (m_pModelCom)
	{
		if (FAILED(Bind_ShaderResources()))
			RETURN_EFAIL;

		auto ActiveMeshes = m_pModelCom->Get_ActiveMeshes();
		for (_uint i = 0; i < ActiveMeshes.size(); ++i)
		{
			// 뼈 바인딩
			//if (FAILED(m_pModelCom->Bind_BoneToShader(ActiveMeshes[i], "g_BoneMatrices")))
			  //  RETURN_EFAIL;

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

HRESULT CCloud_Weapon::Ready_Components(void* pArg)
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_CommonModel"),
		TEXT("Com_ModelCom"), &(m_pModelCom), nullptr)))
		RETURN_EFAIL;

	if (m_pModelCom)
	{
		m_pModelCom->Link_Model(CCommonModelComp::TYPE_NONANIM, TEXT("WE0000_00"));
		if (FAILED(m_pModelCom->Link_Shader(L"Shader_Model", VTXMESH::Elements, VTXMESH::iNumElements)))
			RETURN_EFAIL;
	}

	return S_OK;
}

HRESULT CCloud_Weapon::Bind_ShaderResources()
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

void CCloud_Weapon::Ascension_Mode(_cref_time fTimeDelta)
{

	if (m_TimeChecker.Update(fTimeDelta))
	{
		if (m_bAlwaysParticleRepeat)
		{
			m_bAlwaysParticleRepeat = false;

			GET_SINGLE(CEffect_Manager)->Create_Effect<CParticle>(L"Cloud_Weapon_Always5", shared_from_this(),
				CEffect::USE_FOLLOW_PARTS, _float3(0.5f, 0.f, 0.f));

			GET_SINGLE(CEffect_Manager)->Create_Effect<CParticle>(L"Cloud_Weapon_Always5", shared_from_this(),
				CEffect::USE_FOLLOW_PARTS, _float3(1.0f, 0.2f, 0.f));

			GET_SINGLE(CEffect_Manager)->Create_Effect<CParticle>(L"Cloud_Weapon_Always5", shared_from_this(),
				CEffect::USE_FOLLOW_PARTS, _float3(1.5f, 0.f, 0.2f));
		}
		else {
			m_bAlwaysParticleRepeat = true;

			GET_SINGLE(CEffect_Manager)->Create_Effect<CParticle>(L"Cloud_Weapon_Always5", shared_from_this(),
				CEffect::USE_FOLLOW_PARTS, _float3(0.3f, 0.f, 0.2f));

			GET_SINGLE(CEffect_Manager)->Create_Effect<CParticle>(L"Cloud_Weapon_Always5", shared_from_this(),
				CEffect::USE_FOLLOW_PARTS, _float3(0.6f, 0.2f, 0.f));

			GET_SINGLE(CEffect_Manager)->Create_Effect<CParticle>(L"Cloud_Weapon_Always5", shared_from_this(),
				CEffect::USE_FOLLOW_PARTS, _float3(1.2f, 0.f, 0.2f));
		}
	}

	if (!m_bAscensionMode)
		return;

	if (m_TImeChecker_Limit_Always_Spark.Update(fTimeDelta))
	{
		GET_SINGLE(CEffect_Manager)->Create_Effect<CParticle>(L"Cloud_Ascension_Weapon_Always_Spark1", shared_from_this(),
			CEffect::USE_FOLLOW_PARTS, _float3(0.5f, 0.f, 0.f));

		GET_SINGLE(CEffect_Manager)->Create_Effect<CParticle>(L"Cloud_Ascension_Weapon_Always_Spark1", shared_from_this(),
			CEffect::USE_FOLLOW_PARTS, _float3(1.0f, 0.2f, 0.f));

		GET_SINGLE(CEffect_Manager)->Create_Effect<CParticle>(L"Cloud_Ascension_Weapon_Always_Spark1", shared_from_this(),
			CEffect::USE_FOLLOW_PARTS, _float3(1.5f, 0.f, 0.2f));


		/*GET_SINGLE(CEffect_Manager)->Create_Effect<CParticle>(L"Cloud_Ascension_Weapon_Always_Spark1", shared_from_this(),
			CEffect::USE_FOLLOW_PARTS, _float3(0.3f, 0.f, 0.2f));

		GET_SINGLE(CEffect_Manager)->Create_Effect<CParticle>(L"Cloud_Ascension_Weapon_Always_Spark1", shared_from_this(),
			CEffect::USE_FOLLOW_PARTS, _float3(0.6f, 0.2f, 0.f));

		GET_SINGLE(CEffect_Manager)->Create_Effect<CParticle>(L"Cloud_Ascension_Weapon_Always_Spark1", shared_from_this(),
			CEffect::USE_FOLLOW_PARTS, _float3(1.2f, 0.f, 0.2f));*/
	}

	/*if (m_TImeChecker_Limit_Always_Splash.Update(fTimeDelta))
	{
		GET_SINGLE(CEffect_Manager)->Create_Effect<CParticle>(L"Cloud_Ascension_Weapon_Always_Splash", shared_from_this(),
			CEffect::USE_FOLLOW_PARTS, _float3(1.5f, 0.f, 0.f));

		GET_SINGLE(CEffect_Manager)->Create_Effect<CParticle>(L"Cloud_Ascension_Weapon_Always_Splash", shared_from_this(),
			CEffect::USE_FOLLOW_PARTS, _float3(1.3f, 0.f, 0.2f));

		GET_SINGLE(CEffect_Manager)->Create_Effect<CParticle>(L"Cloud_Ascension_Weapon_Always_Splash", shared_from_this(),
			CEffect::USE_FOLLOW_PARTS, _float3(1.2f, 0.1f, 0.f));

		GET_SINGLE(CEffect_Manager)->Create_Effect<CParticle>(L"Cloud_Ascension_Weapon_Always_Splash", shared_from_this(),
			CEffect::USE_FOLLOW_PARTS, _float3(1.f, 0.f, 0.2f));

		GET_SINGLE(CEffect_Manager)->Create_Effect<CParticle>(L"Cloud_Ascension_Weapon_Always_Splash", shared_from_this(),
			CEffect::USE_FOLLOW_PARTS, _float3(0.8f, 0.1f, 0.f));

		GET_SINGLE(CEffect_Manager)->Create_Effect<CParticle>(L"Cloud_Ascension_Weapon_Always_Splash", shared_from_this(),
			CEffect::USE_FOLLOW_PARTS, _float3(0.7f, 0.f, 0.2f));

		GET_SINGLE(CEffect_Manager)->Create_Effect<CParticle>(L"Cloud_Ascension_Weapon_Always_Splash", shared_from_this(),
			CEffect::USE_FOLLOW_PARTS, _float3(0.5f, 0.1f, 0.f));

		GET_SINGLE(CEffect_Manager)->Create_Effect<CParticle>(L"Cloud_Ascension_Weapon_Always_Splash", shared_from_this(),
			CEffect::USE_FOLLOW_PARTS, _float3(0.4f, 0.f, 0.2f));
	}*/
}

void CCloud_Weapon::OnCollision_Enter(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CCloud_Weapon::OnCollision_Stay(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CCloud_Weapon::OnCollision_Exit(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CCloud_Weapon::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	CState* pCurState = m_pOwner.lock()->Get_StateMachineCom().lock()->Get_CurState();

	if (typeid(*pCurState) != typeid(CState_Cloud_AbilityAttack_Infinity)
		&& typeid(*pCurState) != typeid(CState_Cloud_AbilityAttack_Blade)
		&& typeid(*pCurState) != typeid(CState_Cloud_AbilityAttack_Slash))
		//&& typeid(*pCurState) != typeid(CState_Cloud_LimitAttack))
	{
		PxContactPairPoint* pContactPoint = new PxContactPairPoint[ContactInfo.contactCount];
		PxU32 nbContacts = ContactInfo.extractContacts(pContactPoint, ContactInfo.contactCount);

		// 몬스터와 상호작용
		if (pOtherCol->Get_ColliderDesc().iFilterType == CL_MONSTER_BODY)
		{
			// HP가 0일 때는 대미지와 이펙트를 터뜨리지 않음.
			if (DynPtrCast<IStatusInterface>(pOtherCol->Get_Owner().lock())->Get_StatusCom().lock()->IsZeroHP())
				return;
			Status_DamageTo(m_strSkillName, pOtherCol, pOtherCol->Get_Owner(), pThisCol->Get_Owner());

			DynPtrCast<IStatusInterface>(Get_Owner().lock())->Get_StatusCom().lock()->Add_LimitBreak(5.f);
			int a = 0;
			weak_ptr< CParticle> pEffect;
			if (nbContacts >= 1)
			{
				if (GET_SINGLE(CClient_Manager)->IsPlayable(CLOUD))
				{
					GET_SINGLE(CClient_Manager)->Register_Event_AdjustTimeDelta(m_pOwner, 0.3f, 0.1f);
					GET_SINGLE(CClient_Manager)->Register_Event_MotionBlur(m_pOwner, 0.3f);
				}

				GET_SINGLE(CEffect_Manager)->Create_Hit_Effect<CParticle>(L"CloudSpark4", shared_from_this(),
					Convert_Vector(pContactPoint[0].position), Convert_Vector(pContactPoint[0].normal));
				GET_SINGLE(CEffect_Manager)->Create_Hit_Effect<CParticle>(L"Glare3", shared_from_this(),
					Convert_Vector(pContactPoint[0].position), Convert_Vector(pContactPoint[0].normal));

				GET_SINGLE(CEffect_Manager)->Create_Hit_Effect<CTrail_Effect>(L"CloudHitSwordLineMesh4", shared_from_this(),
					Convert_Vector(pContactPoint[1].position), Convert_Vector(pContactPoint[1].normal));
				GET_SINGLE(CEffect_Manager)->Create_Hit_Effect<CTrail_Effect>(L"CloudHitSwordLineMesh5", shared_from_this(),
					Convert_Vector(pContactPoint[0].position), Convert_Vector(pContactPoint[0].normal));

				if (typeid(*pCurState) == typeid(CState_Cloud_LimitAttack))
				{
					GET_SINGLE(CEffect_Manager)->Create_Hit_Effect<CEffect_Group>(L"Cloud_Ascension_Ht_Group", shared_from_this(),
						Convert_Vector(pContactPoint[0].position), Convert_Vector(pContactPoint[0].normal));
				}

				// 사운드 출력
				_int iRandom = m_pGameInstance->RandomInt(0, 2);
				wstring strSoundName = L"";
				switch (iRandom)
				{
				case 0:
					strSoundName = L"016_SE_BusterSword (Par_Hit_01).wav";
					break;
				case 1:
					strSoundName = L"017_SE_BusterSword (Par_Hit_01).wav";
					break;
				case 2:
					strSoundName = L"018_SE_BusterSword (Par_Hit_01).wav";
					break;
				}

				_float3 vPos = { pContactPoint->position.x, pContactPoint->position.y, pContactPoint->position.z };
				m_pGameInstance->Play_3DSound(TEXT("FF7_Weapon_BusterSword"), strSoundName, ESoundGroup::Effect, 1.f, vPos);



				LIGHT_DESC LightDesc = {};
				LightDesc.bUseVolumetric = true;
				LightDesc.eType = LIGHT_DESC::TYPE_POINT;
				LightDesc.fRange = 6.f;
				LightDesc.strLightName = "Light" + to_string(m_pGameInstance->RandomInt(0, 10000000));
				LightDesc.vDiffuse = { 0.2f, 0.2f, 0.16f, 1.f };
				LightDesc.fSpotPower = 10.f;
				LightDesc.vPosition = { vPos.x, vPos.y, vPos.z, 1.f };
				LightDesc.vAmbient = { 0.2f, 0.2f, 0.2f, 1.f };
				LightDesc.vEmissive = { 0.2f, 0.2f, 0.16f, 1.f };
				LightDesc.vSpecular = { 0.2f, 0.2f, 0.16f, 1.f };

				/*FRapidJson::Open_Json(GI()->MainJSONScriptPath() + L"Test.json")
					.Read_Data("LightTest", LightDesc.fSpotPower);*/

				shared_ptr<CLight>    pLight = nullptr;
				m_pGameInstance->Add_Light(LightDesc, &pLight);
				pLight->Set_RangeLinearDecrease(3.f);
				pLight->Set_RangeQuadDecrease(3.f);
				pLight->Set_LightDamping(3.f);
				pLight->Set_LightVolumeQuadDamping(3.f);
				pLight->Set_Dead();
			}
		}
		else if (pOtherCol->Get_ColliderDesc().iFilterType == CL_MAP_DYNAMIC)
		{
			_float3 vForce = pContactPoint[0].impulse == PxVec3(0.f) ? Convert_Vector(pContactPoint[0].normal * 50.f) : Convert_Vector(pContactPoint[0].impulse);
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

void CCloud_Weapon::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
}

void CCloud_Weapon::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
}

void CCloud_Weapon::Free()
{
	__super::Free();
}
