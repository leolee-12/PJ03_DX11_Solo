//Collider.cpp
#include "..\Public\Collider.h"

#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"
#include "Bounding_Ray.h"

#include "GameInstance.h"

_uint CCollider::m_iNewColliderID = 0;

IMPLEMENT_CREATE_EX1(CCollider,TYPE, eType)
IMPLEMENT_CLONE(CCollider,CComponent)

CCollider::CCollider(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CComponent(pDevice, pContext)
{
}

CCollider::CCollider(const CCollider& rhs)
	: CComponent(rhs)
	, m_eType(rhs.m_eType)
#ifdef _DEBUG
	, m_pBatch(rhs.m_pBatch)
	, m_pEffect(rhs.m_pEffect)
	, m_pInputLayout(rhs.m_pInputLayout)
#endif
{
#ifdef _DEBUG
	Safe_AddRef(m_pInputLayout);
#endif
}

HRESULT CCollider::Initialize_Prototype(TYPE eType)
{
	m_eType = eType;

#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext.Get());
	m_pEffect = new BasicEffect(m_pDevice.Get());
	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCode = { nullptr };
	size_t	iShaderCodeLength = { 0 };

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderCodeLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iShaderCodeLength, &m_pInputLayout)))
		RETURN_EFAIL;

#endif

	return S_OK;
}

HRESULT CCollider::Initialize(void* pArg)
{
	Add_Bounding(pArg);

	m_iColliderID = m_iNewColliderID ++;
	return S_OK;
}


HRESULT CCollider::Add_Bounding(void* pArg)
{
	CBounding::BOUNDING_DESC* pBoundingDesc = (CBounding::BOUNDING_DESC*)pArg;

	switch (m_eType)
	{
	case TYPE_SPHERE:
		m_vecBounding.push_back(CBounding_Sphere::Create(m_pDevice, m_pContext, pBoundingDesc));
		break;
	case TYPE_AABB:
		m_vecBounding.push_back(CBounding_AABB::Create(m_pDevice, m_pContext, pBoundingDesc));
		break;
	case TYPE_OBB:
		m_vecBounding.push_back(CBounding_OBB::Create(m_pDevice, m_pContext, pBoundingDesc));
		break;
	case TYPE_RAY:
		m_vecBounding.push_back(CBounding_Ray::Create(m_pDevice, m_pContext, pBoundingDesc));
		break;
	}

	return S_OK;
}

COLLISIONINFO CCollider::Update_CollisionInfo(CCollider* pTargetCollider,_vector& vDirection, _float& fOverapDist)
{
	if (m_eType == CCollider::TYPE_AABB && pTargetCollider->Get_ColliderType() == CCollider::TYPE_AABB)
	{
		const BoundingBox* pMyBounding = static_cast<CBounding_AABB*>(m_vecBounding.front())->Get_Bounding();
		const BoundingBox* pTargetBounding = static_cast<CBounding_AABB*>(pTargetCollider->Get_Bounding().front())->Get_Bounding();
		
		_float fOverapX = 0.5f*(pMyBounding->Extents.x + pMyBounding->Extents.x) - fabs(pMyBounding->Center.x - pTargetBounding->Center.x);
		_float fOverapZ = 0.5f*(pMyBounding->Extents.z + pMyBounding->Extents.z) - fabs(pMyBounding->Center.z - pTargetBounding->Center.z);

		pTargetBounding->Intersects(XMLoadFloat3(&pMyBounding->Center), vDirection, fOverapDist);
		if (fOverapX < fOverapZ)
		{			
			m_CollisionInfo.vLine = { 0.f,0.f,-1.f};
			if (pMyBounding->Center.x > pTargetBounding->Center.x)
				m_CollisionInfo.vNormal = { 1.f,0.f,0.f };
			else
				m_CollisionInfo.vNormal = { -1.f,0.f,0.f };
		}
		else
		{			
			m_CollisionInfo.vLine = { 1.f,0.f,0.f};
			if (pMyBounding->Center.z > pTargetBounding->Center.z)
				m_CollisionInfo.vNormal = { 0.f,0.f,1.f };
			else
				m_CollisionInfo.vNormal = { 0.f,0.f,-1.f };
		}
	}
	else if (m_eType == CCollider::TYPE_SPHERE && pTargetCollider->Get_ColliderType() == CCollider::TYPE_SPHERE)
	{
		const BoundingSphere* pMyBounding = static_cast<CBounding_Sphere*>(m_vecBounding.front())->Get_Bounding();
		const BoundingSphere* pTargetBounding = static_cast<CBounding_Sphere*>(pTargetCollider->Get_Bounding().front())->Get_Bounding();

		_vector vCenterToCenterXZ = XMVectorSetY(XMLoadFloat3(&pTargetBounding->Center), 0.f) - XMVectorSetY(XMLoadFloat3(&pMyBounding->Center), 0.f);
		fOverapDist = (pMyBounding->Radius + pTargetBounding->Radius) - XMVectorGetX(XMVector3Length(vCenterToCenterXZ));

		vCenterToCenterXZ = XMVector3Normalize(vCenterToCenterXZ);
		XMStoreFloat3(&m_CollisionInfo.vLine, XMVector3Cross(vCenterToCenterXZ, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
		XMStoreFloat3(&m_CollisionInfo.vNormal, vCenterToCenterXZ);
		vDirection = vCenterToCenterXZ;

	}
	return m_CollisionInfo;
}

#ifdef _DEBUG

HRESULT CCollider::Render()
{
	if (m_vecBounding.empty())
		RETURN_EFAIL;

	m_pContext->GSSetShader(nullptr, nullptr, 0);

	m_pBatch->Begin();

	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView(m_pGameInstance->Get_TransformMatrix(CPipeLine::TS_VIEW));
	m_pEffect->SetProjection(m_pGameInstance->Get_TransformMatrix(CPipeLine::TS_PROJ));

	m_pContext->IASetInputLayout(m_pInputLayout);

	m_pEffect->Apply(m_pContext.Get());

	for(auto& iter : m_vecBounding)
		iter->Render(m_pBatch, m_isCollision == true ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));

	m_pBatch->End();

	return S_OK;
}
#endif 

void CCollider::Update(_fmatrix TransformMatrix)
{
	for (auto& iter : m_vecBounding)
		iter->Update(TransformMatrix);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugRender(shared_from_this());
#endif
}


_bool CCollider::Collision(CCollider* pTargetCollider)
{
	for (auto& pThisBounding : m_vecBounding)
	{
		if (pThisBounding->Collision(pTargetCollider, &m_isCollision))
		{			
			m_vCollisionCenter = pThisBounding->Get_Center();
			return true;
		}
		
	}
	return false;
}


void CCollider::OnCollision_Enter(CCollider* pThisCol, CCollider* pOtherCol)
{
	m_pOwner.lock()->OnCollision_Enter(pThisCol, pOtherCol);
}

void CCollider::OnCollision_Stay(CCollider* pThisCol, CCollider* pOtherCol)
{
	m_pOwner.lock()->OnCollision_Stay(pThisCol, pOtherCol);
}

void CCollider::OnCollision_Exit(CCollider* pThisCol, CCollider* pOtherCol)
{
	m_pOwner.lock()->OnCollision_Exit(pThisCol, pOtherCol);
	m_isCollision = false;
	m_vCollisionCenter = { 0.f,0.f,0.f };
}


void CCollider::Free()
{
	__super::Free();

#ifdef _DEBUG

	if (false == m_isCloned)
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
	}

	Safe_Release(m_pInputLayout);

#endif
	for (auto& iter : m_vecBounding)
		Safe_Release(iter);
}