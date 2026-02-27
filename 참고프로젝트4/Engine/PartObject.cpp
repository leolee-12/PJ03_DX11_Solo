#include "PartObject.h"
#include "Transform.h"
#include "BoneContainer.h"
#include "GameInstance.h"
#include "BlendObject.h"

HRESULT CPartObject::Ready_Components()
{

	return S_OK;
}

void CPartObject::TurnOff_PartEffect()
{
	if (m_pPartEffect.lock())
	{
		m_pPartEffect.lock()->TurnOff_State(OBJSTATE::Active);
	}
}

void CPartObject::TurnOn_PartEffect()
{
	if (m_pPartEffect.lock())
		m_pPartEffect.lock()->TurnOn_State(OBJSTATE::Active);
}

HRESULT CPartObject::Bind_ShaderResources()
{
	return S_OK;
}


CPartObject::CPartObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CGameObject(pDevice,pContext)
{
}

CPartObject::CPartObject(const CPartObject& rhs)
	:CGameObject(rhs)
{
}

HRESULT CPartObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPartObject::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;
	
	PARTOBJECT_DESC* pDesc = (PARTOBJECT_DESC*)pArg;
	m_pParentTransform = pDesc->pParentTransform;
	m_pBoneGroup = pDesc->pBoneGroup;
	if (m_pBoneGroup != nullptr && m_pBoneGroup->CRef_BoneDatas_Count() > 0 && pDesc->strBoneName != L"")
	{
		m_iBoneIndex = m_pBoneGroup->Find_BoneData(pDesc->strBoneName)->iID;
	}	

	return S_OK;
}

void CPartObject::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);
}

void CPartObject::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CPartObject::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CPartObject::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CPartObject::Before_Render(_cref_time fTimeDelta)
{
	if (m_pBoneGroup)
	{
		//vSocketMatrix Normalize 해야함
		_matrix vSocketMatrix = m_pBoneGroup->CRef_BoneCombinedTransforms()[m_iBoneIndex];
		vSocketMatrix.r[0] = XMVector3Normalize(vSocketMatrix.r[0]);
		vSocketMatrix.r[1] = XMVector3Normalize(vSocketMatrix.r[1]);
		vSocketMatrix.r[2] = XMVector3Normalize(vSocketMatrix.r[2]);

		XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * vSocketMatrix * m_pParentTransform.lock()->Get_WorldMatrix());
	}
	else
		XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * m_pParentTransform.lock()->Get_WorldMatrix());

	if (m_pPhysXColliderCom)
	{
		m_pPhysXColliderCom->Synchronize_Collider(m_WorldMatrix, m_vPhysXColliderLocalOffset,m_vPhysXColliderWorldOffset);
	}

	__super::Before_Render(fTimeDelta);
}

void CPartObject::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CPartObject::Render()
{
	return S_OK;
}

HRESULT CPartObject::Render_Shadow()
{
	if (m_pModelCom)
	{
		// Shadow용 LightProj 행렬 바인딩
		if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrices("g_ShadowVIewProjMatrices", m_pGameInstance->Get_Renderer()->Get_Shadow_LightViewProjMatrices(), 4)))
			RETURN_EFAIL;

		// 월드 행렬 바인딩
		if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
			RETURN_EFAIL;
	
		auto ActiveMeshes = m_pModelCom->Get_ActiveMeshes();
		for (_uint i = 0; i < ActiveMeshes.size(); ++i)
		{
			// 패스, 버퍼 바인딩
			if (FAILED(m_pModelCom->ShaderComp()->Begin(3)))
				RETURN_EFAIL;
			//버퍼 렌더
			if (FAILED(m_pModelCom->BindAndRender_Mesh(ActiveMeshes[i])))
				RETURN_EFAIL;
		}
	}
	return S_OK;
}

void CPartObject::OnCollision_Enter(CCollider* pThisCol, CCollider* pOtherCol)
{
	m_pOwner.lock()->OnCollision_Enter(pThisCol, pOtherCol);
}

void CPartObject::OnCollision_Stay(CCollider* pThisCol, CCollider* pOtherCol)
{
	m_pOwner.lock()->OnCollision_Stay(pThisCol, pOtherCol);
}

void CPartObject::OnCollision_Exit(CCollider* pThisCol, CCollider* pOtherCol)
{
	m_pOwner.lock()->OnCollision_Exit(pThisCol, pOtherCol);
}

void CPartObject::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	m_pOwner.lock()->PhysX_OnCollision_Enter(pThisCol, pOtherCol,ContactInfo);
}

void CPartObject::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	m_pOwner.lock()->PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CPartObject::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	m_pOwner.lock()->PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}

_float4x4 CPartObject::Get_WorldFloat4x4WithParent()
{
	if (m_pParentTransform.expired())
		return Get_WorldMatrix();

	return Get_WorldMatrix() * m_pParentTransform.lock()->Get_WorldMatrix();
}

_matrix CPartObject::Get_WorldMatrixWithParent()
{
	if (m_pParentTransform.expired())
		return Get_WorldMatrix();

	return Get_WorldMatrix() * m_pParentTransform.lock()->Get_WorldMatrix();
}

_float4x4 CPartObject::Get_WorldFloat4x4FromBone(const wstring& strBoneName)
{
	if (!m_pModelCom)
		return Get_WorldMatrix();

	return m_pModelCom->Get_BoneTransformMatrixWithParents(strBoneName) * Get_WorldMatrix();
}

_matrix CPartObject::Get_WorldMatrixFromBone(const wstring& strBoneName)
{
	if (!m_pModelCom)
		return Get_WorldMatrix();

	return m_pModelCom->Get_BoneTransformMatrixWithParents(strBoneName) * Get_WorldMatrix();
}

void CPartObject::Free()
{
	__super::Free();
}