#include "..\Public\Bounding_OBB.h"
#include "DebugDraw.h"

#include "Bounding_AABB.h"
#include "Bounding_Sphere.h"

CBounding_OBB::CBounding_OBB(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CBounding { pDevice, pContext }
{

}

HRESULT CBounding_OBB::Initialize(CBounding::BOUNDING_DESC * pDesc)
{
	BOUNDING_OBB_DESC*	pBoundingDesc = static_cast<BOUNDING_OBB_DESC*>(pDesc);

	_float4			vRotation;
	XMStoreFloat4(&vRotation, XMQuaternionRotationRollPitchYaw(pBoundingDesc->vRotation.x, pBoundingDesc->vRotation.y, pBoundingDesc->vRotation.z));

	m_pOriginalDesc = new BoundingOrientedBox(pBoundingDesc->vCenter, pBoundingDesc->vExtents, vRotation);
	m_pDesc = new BoundingOrientedBox(*m_pOriginalDesc);

	return S_OK;
}

void CBounding_OBB::Update(_fmatrix TransformMatrix)
{
	m_pOriginalDesc->Transform(*m_pDesc, TransformMatrix);
}

void CBounding_OBB::Update(_vector vTransform)
{
	_matrix		Transform = XMMatrixAffineTransformation({ 1.f,1.f,1.f }, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 0.f), vTransform);
	m_pOriginalDesc->Transform(*m_pDesc, Transform);
}

HRESULT CBounding_OBB::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor)
{
	DX::Draw(pBatch, *m_pDesc, vColor);

	return S_OK;
}

_bool CBounding_OBB::isCollision(CCollider::TYPE eTargetColliderType, CBounding * pTargetBounding)
{
	_bool		isColl = { false };

	switch (eTargetColliderType)
	{
	case CCollider::TYPE_AABB:
		isColl = m_pDesc->Intersects(static_cast<CBounding_AABB*>(pTargetBounding)->Get_Desc());
		break;
	case CCollider::TYPE_OBB:
		isColl = m_pDesc->Intersects(static_cast<CBounding_OBB*>(pTargetBounding)->Get_Desc());
		break;
	case CCollider::TYPE_SPHERE:
		isColl = m_pDesc->Intersects(static_cast<CBounding_Sphere*>(pTargetBounding)->Get_Desc());
		break;
	}

	return isColl;
}

_bool CBounding_OBB::isRayCollision(const _float3& rayOrigin, const _float3& rayDir)
{
	XMVECTOR invDir = XMVectorReciprocal(XMLoadFloat3(&rayDir));
	XMVECTOR t1 = (XMLoadFloat3(&m_pDesc->Center) - XMLoadFloat3(&rayOrigin)) * invDir;
	XMVECTOR t2 = (XMLoadFloat3(&m_pDesc->Center) + XMLoadFloat3(&m_pDesc->Extents) - XMLoadFloat3(&rayOrigin)) * invDir;

	XMVECTOR tMin = XMVectorMin(t1, t2);
	XMVECTOR tMax = XMVectorMax(t1, t2);

	float tNear = max(max(XMVectorGetX(tMin), XMVectorGetY(tMin)), XMVectorGetZ(tMin));
	float tFar = min(min(XMVectorGetX(tMax), XMVectorGetY(tMax)), XMVectorGetZ(tMax));

	return tNear <= tFar && tFar >= 0.0f;
}

_bool CBounding_OBB::Collision_OBB(CBounding_OBB * pTargetDesc)
{
	OBB_DESC	OBBDesc[2];

	OBBDesc[0] = Compute_OBB();
	OBBDesc[1] = pTargetDesc->Compute_OBB();

	_float		fDistance[3] = {};

	for (size_t i = 0; i < 2; i++)
	{
		for (size_t j = 0; j < 3; j++)
		{
			fDistance[0] = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenter) - XMLoadFloat3(&OBBDesc[0].vCenter),
				XMLoadFloat3(&OBBDesc[i].vAlignDir[j]))));

			fDistance[1] = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterDir[0]), XMLoadFloat3(&OBBDesc[i].vAlignDir[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterDir[1]), XMLoadFloat3(&OBBDesc[i].vAlignDir[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterDir[2]), XMLoadFloat3(&OBBDesc[i].vAlignDir[j]))));

			fDistance[2] = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterDir[0]), XMLoadFloat3(&OBBDesc[i].vAlignDir[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterDir[1]), XMLoadFloat3(&OBBDesc[i].vAlignDir[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterDir[2]), XMLoadFloat3(&OBBDesc[i].vAlignDir[j]))));

			if (fDistance[0] > fDistance[1] + fDistance[2])
				return false;
		}
	}
	
	return true;
}

CBounding_OBB::OBB_DESC CBounding_OBB::Compute_OBB()
{
	OBB_DESC			OBBDesc{};

	_float3				vPoints[8];

	m_pDesc->GetCorners(vPoints);

	OBBDesc.vCenter = m_pDesc->Center;

	XMStoreFloat3(&OBBDesc.vCenterDir[0], (XMLoadFloat3(&vPoints[5]) - XMLoadFloat3(&vPoints[4])) * 0.5f);
	XMStoreFloat3(&OBBDesc.vCenterDir[1], (XMLoadFloat3(&vPoints[7]) - XMLoadFloat3(&vPoints[4])) * 0.5f);
	XMStoreFloat3(&OBBDesc.vCenterDir[2], (XMLoadFloat3(&vPoints[0]) - XMLoadFloat3(&vPoints[4])) * 0.5f);

	for (size_t i = 0; i < 3; i++)
		XMStoreFloat3(&OBBDesc.vAlignDir[i], XMVector3Normalize(XMLoadFloat3(&OBBDesc.vCenterDir[i])));
		
	return OBBDesc;
}

CBounding_OBB * CBounding_OBB::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, CBounding::BOUNDING_DESC * pDesc)
{
	CBounding_OBB*		pInstance = new CBounding_OBB(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pDesc)))
	{
		MSG_BOX(TEXT("Failed to Created : CBounding_OBB"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CBounding_OBB::Free()
{
	__super::Free();

	Safe_Delete(m_pOriginalDesc);
	Safe_Delete(m_pDesc);
}
