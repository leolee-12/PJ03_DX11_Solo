#include "..\Public\Bounding_Sphere.h"
#include "DebugDraw.h"

#include "Bounding_AABB.h"
#include "Bounding_OBB.h"

CBounding_Sphere::CBounding_Sphere(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CBounding { pDevice, pContext }
{



}

HRESULT CBounding_Sphere::Initialize(CBounding::BOUNDING_DESC * pDesc)
{
	BOUNDING_SPHERE_DESC*	pBoundingDesc = static_cast<BOUNDING_SPHERE_DESC*>(pDesc);	

	m_pOriginalDesc = new BoundingSphere(pBoundingDesc->vCenter, pBoundingDesc->fRadius);
	m_pDesc = new BoundingSphere(*m_pOriginalDesc);

	return S_OK;
}

void CBounding_Sphere::Update(_fmatrix TransformMatrix)
{
	m_pOriginalDesc->Transform(*m_pDesc, TransformMatrix);
}

void CBounding_Sphere::Update(_vector vTransform)
{
	_matrix		Transform = XMMatrixAffineTransformation({ 1.f,1.f,1.f }, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 0.f), vTransform);
	m_pOriginalDesc->Transform(*m_pDesc, Transform);
}

HRESULT CBounding_Sphere::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor)
{
	DX::Draw(pBatch, *m_pDesc, vColor);

	return S_OK;
}

_bool CBounding_Sphere::isCollision(CCollider::TYPE eTargetColliderType, CBounding * pTargetBounding)
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

_bool CBounding_Sphere::isRayCollision(const _float3& rayOrigin, const _float3& rayDir)
{
	// 구체의 중심점과 반지름
	XMVECTOR sphereCenter = XMLoadFloat3(&m_pDesc->Center);
	_float sphereRadius = m_pDesc->Radius;

	// 레이의 시작점과 방향 벡터
	XMVECTOR rayOriginVec = XMLoadFloat3(&rayOrigin);
	XMVECTOR rayDirVec = XMLoadFloat3(&rayDir);

	// 레이의 시작점에서 구체의 중심점까지의 벡터
	XMVECTOR m = rayOriginVec - sphereCenter;

	// 레이의 방향 벡터와 m 벡터의 내적
	_float b = XMVectorGetX(XMVector3Dot(m, rayDirVec));
	_float c = XMVectorGetX(XMVector3Dot(m, m)) - sphereRadius * sphereRadius;

	// 판별식 계산
	if (c > 0.0f && b > 0.0f)
	{
		return false; // 레이의 시작점이 구체의 바깥에 있으며 레이가 구체를 향하지 않는 경우
	}

	_float discriminant = b * b - c;

	// 판별식이 음수인 경우, 레이가 구체와 교차하지 않음
	if (discriminant < 0.0f)
	{
		return false;
	}

	// 레이가 구체와 교차함
	return true;
}

CBounding_Sphere * CBounding_Sphere::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, CBounding::BOUNDING_DESC * pDesc)
{
	CBounding_Sphere*		pInstance = new CBounding_Sphere(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pDesc)))
	{
		MSG_BOX(TEXT("Failed to Created : CBounding_Sphere"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CBounding_Sphere::Free()
{
	__super::Free();

	Safe_Delete(m_pOriginalDesc);
	Safe_Delete(m_pDesc);
}
