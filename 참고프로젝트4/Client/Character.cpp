#include "stdafx.h"
#include "Character.h"
#include "GameInstance.h"

#include "PartObject.h"
#include "StatusComp.h"

#include "UI_Manager.h"
#include "Trail_Buffer.h"

#include "Effect_Manager.h"

IMPLEMENT_CREATE(CCharacter)
IMPLEMENT_CLONE(CCharacter, CGameObject)

CCharacter::CCharacter(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CDynamicObject(pDevice, pContext)
{
}

CCharacter::CCharacter(const CCharacter& rhs)
	:CDynamicObject(rhs)
{
}

HRESULT CCharacter::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;
	return S_OK;
}

HRESULT CCharacter::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	if (FAILED(Ready_Components(pArg)))
		RETURN_EFAIL;

	return S_OK;
}

void CCharacter::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);
}

void CCharacter::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pStateMachineCom->Priority_Tick(fTimeDelta);
} 

void CCharacter::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_pStateMachineCom->Tick(fTimeDelta);

	m_fDamageFont_Delay.Increase(fTimeDelta);
}

void CCharacter::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pStateMachineCom->Late_Tick(fTimeDelta);

	m_pModelCom->Tick_AnimTime(fTimeDelta);
	m_pTransformCom->Move_To_AnimationPos(m_pModelCom, fTimeDelta);
}

void CCharacter::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW, shared_from_this())))
		return;
}

void CCharacter::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CCharacter::Render()
{
	return S_OK;
}

HRESULT CCharacter::Render_Shadow()
{
	if (m_pModelCom)
	{
		// Shadow용 LightProj 행렬 바인딩
		if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrices("g_ShadowVIewProjMatrices", m_pGameInstance->Get_Renderer()->Get_Shadow_LightViewProjMatrices(), 4)))
			RETURN_EFAIL;

		// 월드 행렬 바인딩
		if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4())))
			RETURN_EFAIL;

		auto ActiveMeshes = m_pModelCom->Get_ActiveMeshes();
		for (_uint i = 0; i < ActiveMeshes.size(); ++i)
		{
			// 뼈 바인딩
			if (FAILED(m_pModelCom->Bind_BoneToShader(ActiveMeshes[i], "g_BoneMatrices")))
				RETURN_EFAIL;

			// 패스, 버퍼 바인딩
			if (FAILED(m_pModelCom->ShaderComp()->Begin(4)))
				RETURN_EFAIL;
			//버퍼 렌더
			if (FAILED(m_pModelCom->BindAndRender_Mesh(ActiveMeshes[i])))
				RETURN_EFAIL;
		}
	}
	return S_OK;
}

HRESULT CCharacter::Ready_Components(void* pArg)
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_StateMachine"),
		TEXT("Com_StateMachineCom"), &m_pStateMachineCom)))
		RETURN_EFAIL;

	// 스테이터스 컴포넌트
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Status"),
		TEXT("Com_StatusCom"), &(m_pStatusCom), nullptr)))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CCharacter::Bind_ShaderResources()
{
	return S_OK;
}

weak_ptr<CPartObject> CCharacter::TurnOn_Weapon(wstring strTag)
{
	weak_ptr<CPartObject> pWeapon;

	if (strTag != L"")
		pWeapon = Find_PartObject(strTag);
	else
	{
		for (auto& iter : m_PartObjects)
		{
			shared_ptr<CPartObject> pPartObject = static_pointer_cast<CPartObject>(iter.second);
			if (pPartObject->Get_PartObjType() == PART_WEAPON)
			{
				pWeapon = pPartObject;
				break;
			}
		}
	}

	if (pWeapon.lock())
	{
		weak_ptr<CPhysX_Collider> pCollider = pWeapon.lock()->Get_PhysXColliderCom();
		if (pCollider.lock())
		{
			pCollider.lock()->WakeUp();
		}
	}

	return pWeapon;
}

weak_ptr<CPartObject> CCharacter::TurnOff_Weapon(wstring strTag)
{
	weak_ptr<CPartObject> pWeapon;

	if (strTag != L"")
		pWeapon = Find_PartObject(strTag);
	else
	{
		for (auto& iter : m_PartObjects)
		{
			shared_ptr<CPartObject> pPartObject = static_pointer_cast<CPartObject>(iter.second);
			if (pPartObject->Get_PartObjType() == PART_WEAPON)
			{
				pWeapon = pPartObject;
				break;
			}
		}
	}

	if (pWeapon.lock())
	{
		weak_ptr<CPhysX_Collider> pCollider = pWeapon.lock()->Get_PhysXColliderCom();
		if (pCollider.lock())
		{
			pCollider.lock()->PutToSleep();
		}
	}

	return pWeapon;
}

weak_ptr<CPartObject> CCharacter::TurnOn_PartEffect(wstring strTag)
{
	weak_ptr<CPartObject> pWeapon;

	if (strTag != L"")
		pWeapon = Find_PartObject(strTag);
	else
	{
		for (auto& iter : m_PartObjects)
		{
			shared_ptr<CPartObject> pPartObject = static_pointer_cast<CPartObject>(iter.second);
			if (pPartObject->Get_PartObjType() == PART_WEAPON)
			{
				pWeapon = pPartObject;
				break;
			}
		}
	}

	if (pWeapon.lock())
	{
		pWeapon.lock()->TurnOn_PartEffect();

		weak_ptr<CTrail_Buffer> pTrailBuffer = dynamic_pointer_cast<CTrail_Buffer>(pWeapon.lock()->Get_PartEffect().lock());
		if (pTrailBuffer.lock() != nullptr)
		{
			pTrailBuffer.lock()->Reset(pWeapon.lock()->Get_WorldMatrix());
		}
	}

	return pWeapon;
}

weak_ptr<CPartObject> CCharacter::TurnOff_PartEffect(wstring strTag)
{
	weak_ptr<CPartObject> pWeapon;

	if (strTag != L"")
		pWeapon = Find_PartObject(strTag);
	else
	{
		for (auto& iter : m_PartObjects)
		{
			shared_ptr<CPartObject> pPartObject = static_pointer_cast<CPartObject>(iter.second);
			if (pPartObject->Get_PartObjType() == PART_WEAPON)
			{
				pWeapon = pPartObject;
				break;
			}
		}
	}

	if (pWeapon.lock())
	{
		pWeapon.lock()->TurnOff_PartEffect();
	}

	return pWeapon;
}

void CCharacter::Update_Dissolve(_cref_time fTimeDelta,_float fAmountSpeed, _float fGradiationDistanceSpeed)
{
	if (!m_bDissovleStart)
		return;

	if ((m_fDissolveAmout >= 1.f) && (m_fDissolveGradiationDistance >= 1.f))
	{
		Set_Dead();
		return;
	}
		
	m_fDissolveAmout += fTimeDelta * fAmountSpeed;
	m_fDissolveGradiationDistance += fTimeDelta * fGradiationDistanceSpeed;
	// 디졸브 조절
}

HRESULT CCharacter::Ready_DissovleTex()
{
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CreateLevelIndex(), TEXT("Prototype_Component_Material"),
		TEXT("Com_Material"), &(m_pMaterialCom))))
		RETURN_EFAIL;

	if (FAILED(m_pMaterialCom->Ready_CustomSingleMaterial(MATERIALTYPE::MATERIATYPE_DISSOLVE,
		m_strDissolveTextureTag, 1))) RETURN_EFAIL;

	return S_OK;
}

void CCharacter::Dead_DissolveType(wstring strParticleDataName)
{
	if (m_bDissovleCheck)
		return;

	m_iShaderPassIndex = 5; // DIssolve 적용
	m_bDissovleCheck = true;

	shared_ptr<CParticle> m_pParticle = GET_SINGLE(CEffect_Manager)->Create_Mesh_VTX_Particle(strParticleDataName, shared_from_this());

	shared_ptr<CVIBuffer_Instancing> pBuffer = dynamic_pointer_cast<CVIBuffer_Instancing>(m_pParticle->Get_VIBufferCom().lock());

	_float4 fColor = m_pGameInstance->RandomFloat4(pBuffer->Get_InstancingDesc()->vColor[0]
		, pBuffer->Get_InstancingDesc()->vColor[1]);

	m_vDissolveGradiationStartColor = m_vDissolveGradiationEndColor = fColor;
	// 색 파티클과 같게 설정

	m_bDissovleStart = true;
}

void CCharacter::OnCollision_Enter(CCollider* pThisCol, CCollider* pOtherCol)
{
	__super::OnCollision_Enter(pThisCol, pOtherCol);
		
}

void CCharacter::OnCollision_Stay(CCollider* pThisCol, CCollider* pOtherCol)
{
	__super::OnCollision_Stay(pThisCol, pOtherCol);
}

void CCharacter::OnCollision_Exit(CCollider* pThisCol, CCollider* pOtherCol)
{
	__super::OnCollision_Exit(pThisCol, pOtherCol);
}

void CCharacter::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);

	//GET_SINGLE(CUI_Manager)->Make_DamageFont(pThisCol);
}

void CCharacter::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CCharacter::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}

void CCharacter::PhysX_Raycast_Stay(weak_ptr<CGameObject> pOther, _uint iOtherColLayer, _float4 vOrigin, _float4 vDirection, _float4 vColPosition)
{
	__super::PhysX_Raycast_Stay(pOther,iOtherColLayer, vOrigin, vDirection, vColPosition);

	/*if (m_fDamageFont_Delay.IsMax())
	{
		auto pThisCol = this->Get_PhysXColliderCom().lock().get();
		GET_SINGLE(CUI_Manager)->Make_DamageFont(pThisCol);
		m_fDamageFont_Delay.ReserveReset();
	}*/
}


void CCharacter::Free()
{
	__super::Free();
}


