#include "..\Public\Bounding_AABB.h"
#include "DebugDraw.h"

#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"

CBounding_AABB::CBounding_AABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBounding{ pDevice, pContext }
{

}

void CBounding_AABB::Set_Desc(BoundingBox _box)
{
	m_pOriginalDesc->Center = _box.Center;
	m_pOriginalDesc->Extents = _box.Extents;
}

HRESULT CBounding_AABB::Initialize(CBounding::BOUNDING_DESC* pDesc)
{
	BOUNDING_AABB_DESC* pBoundingDesc = static_cast<BOUNDING_AABB_DESC*>(pDesc);

	m_pOriginalDesc = new BoundingBox(pBoundingDesc->vCenter, pBoundingDesc->vExtents);
	m_pDesc = new BoundingBox(*m_pOriginalDesc);

	return S_OK;
}

void CBounding_AABB::Update(_fmatrix TransformMatrix)
{
	_vector		vScale, vRotation, vTranslation;

	XMMatrixDecompose(&vScale, &vRotation, &vTranslation, TransformMatrix);

	_matrix		Transform = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 0.f), vTranslation);

	m_pOriginalDesc->Transform(*m_pDesc, Transform);
}

HRESULT CBounding_AABB::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor)
{
	DX::Draw(pBatch, *m_pDesc, vColor);

	return S_OK;
}

void CBounding_AABB::Update(_vector vTransform)
{
	_matrix		Transform = XMMatrixAffineTransformation({1.f,1.f,1.f}, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 0.f), vTransform);

	m_pOriginalDesc->Transform(*m_pDesc, Transform);
}

_bool CBounding_AABB::isCollision(CCollider::TYPE eTargetColliderType, CBounding* pTargetBounding)
{
	_bool		isColl = { false };

	switch (eTargetColliderType)
	{
	case CCollider::TYPE_AABB:
		isColl = Collision_AABB(static_cast<CBounding_AABB*>(pTargetBounding));
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


_bool CBounding_AABB::isRayCollision(const _float3& rayOrigin, const _float3& rayDir)
{
	// 레이의 시작점과 방향 벡터를 로드합니다.
	XMVECTOR rayOriginVec = XMLoadFloat3(&rayOrigin);
	XMVECTOR rayDirVec = XMLoadFloat3(&rayDir);

	// AABB의 중심점과 확장을 로드합니다.
	XMVECTOR boxCenter = XMLoadFloat3(&m_pDesc->Center);
	XMVECTOR boxExtents = XMLoadFloat3(&m_pDesc->Extents);

	// AABB의 최소 경계와 최대 경계를 계산합니다.
	XMVECTOR boxMin = boxCenter - boxExtents;
	XMVECTOR boxMax = boxCenter + boxExtents;

	// tMin과 tMax를 초기화합니다.
	float tMin = 0.0f;
	float tMax = FLT_MAX;

	// 각 축(x, y, z)에 대해 교차 거리를 계산합니다.
	for (int i = 0; i < 3; ++i)
	{
		_float rayOriginComponent, rayDirComponent, boxMinComponent, boxMaxComponent;

		// 각 축에 대해 성분을 가져옵니다.
		if (i == 0) {
			rayOriginComponent = XMVectorGetX(rayOriginVec);
			rayDirComponent = XMVectorGetX(rayDirVec);
			boxMinComponent = XMVectorGetX(boxMin);
			boxMaxComponent = XMVectorGetX(boxMax);
		}
		else if (i == 1) {
			rayOriginComponent = XMVectorGetY(rayOriginVec);
			rayDirComponent = XMVectorGetY(rayDirVec);
			boxMinComponent = XMVectorGetY(boxMin);
			boxMaxComponent = XMVectorGetY(boxMax);
		}
		else {
			rayOriginComponent = XMVectorGetZ(rayOriginVec);
			rayDirComponent = XMVectorGetZ(rayDirVec);
			boxMinComponent = XMVectorGetZ(boxMin);
			boxMaxComponent = XMVectorGetZ(boxMax);
		}

		// 레이가 축에 평행한 경우를 처리합니다.
		//FLT_EPSILON은 float형 변수의 가장 작은 양의 값으로, 1.0과 구분될 수 있는 값 중 가장 작은 값입니다.이는 컴퓨터가 부동소수점 연산을 할 때 나타나는 오차를 다룰 때 사용됩니다.
		//육면체 그림 그려보면 값 나옴.
		if (fabs(rayDirComponent) < FLT_EPSILON)
		{
			// 레이가 축에 평행하고, 시작점이 박스 경계 밖에 있는 경우 교차하지 않습니다.
			if (rayOriginComponent < boxMinComponent || rayOriginComponent > boxMaxComponent)
			{
				return false;
			}
		}
		else
		{
			// 교차 거리를 계산합니다.
			float t1 = (boxMinComponent - rayOriginComponent) / rayDirComponent;
			float t2 = (boxMaxComponent - rayOriginComponent) / rayDirComponent;

			// t1이 t2보다 큰 경우, 두 값을 교환합니다.
			//t1과 t2의 값을 교환하는 이유는 레이의 방향에 따라 t1이 AABB에 들어가는 지점을, t2가 AABB에서 나가는 지점을 나타내기 때문입니다.
			// 레이의 방향에 따라 교차 지점의 순서가 다를 수 있기 때문에, 이 값을 올바르게 정렬하여 일관된 의미를 갖도록 해야 합니다.
			if (t1 > t2)
			{
				swap(t1, t2);
			}

			// tMin과 tMax를 업데이트합니다.
			tMin = max(tMin, t1);
			tMax = min(tMax, t2);

			// tMin이 tMax보다 큰 경우, 교차하지 않습니다.
			if (tMin > tMax)
			{
				return false;
			}
		}
	}

	// 교차합니다.
	return true;
}

_bool CBounding_AABB::isPointInAABB(const _float3& point)
{
	// 포인트와 AABB의 중심점과 확장을 로드합니다.
	XMVECTOR pointVec = XMLoadFloat3(&point);
	XMVECTOR boxCenter = XMLoadFloat3(&m_pDesc->Center);
	XMVECTOR boxExtents = XMLoadFloat3(&m_pDesc->Extents);

	// AABB의 최소 경계와 최대 경계를 계산합니다.
	XMVECTOR boxMin = boxCenter - boxExtents;
	XMVECTOR boxMax = boxCenter + boxExtents;

	// 각 축(x, y, z)에 대해 포인트가 AABB 범위 내에 있는지 검사합니다.
	for (int i = 0; i < 3; ++i)
	{
		_float pointComponent, boxMinComponent, boxMaxComponent;

		// 각 축에 대해 성분을 가져옵니다.
		if (i == 0) {
			pointComponent = XMVectorGetX(pointVec);
			boxMinComponent = XMVectorGetX(boxMin);
			boxMaxComponent = XMVectorGetX(boxMax);
		}
		else if (i == 1) {
			pointComponent = XMVectorGetY(pointVec);
			boxMinComponent = XMVectorGetY(boxMin);
			boxMaxComponent = XMVectorGetY(boxMax);
		}
		else {
			pointComponent = XMVectorGetZ(pointVec);
			boxMinComponent = XMVectorGetZ(boxMin);
			boxMaxComponent = XMVectorGetZ(boxMax);
		}

		// 포인트가 해당 축에서 AABB 범위 밖에 있다면, AABB 안에 있지 않음
		if (pointComponent < boxMinComponent || pointComponent > boxMaxComponent)
		{
			return false;
		}
	}

	// 모든 축에 대해 범위 내에 있으므로, 포인트는 AABB 내부에 있음
	return true;
}

_float CBounding_AABB::Get_Overlap_X(const CBounding_AABB* pOther) const
{
	// 두 AABB의 최소 및 최대 값을 계산
	_float minA_x = m_pDesc->Center.x - m_pDesc->Extents.x;
	_float maxA_x = m_pDesc->Center.x + m_pDesc->Extents.x;

	_float minB_x = pOther->m_pDesc->Center.x - pOther->m_pDesc->Extents.x;
	_float maxB_x = pOther->m_pDesc->Center.x + pOther->m_pDesc->Extents.x;

	// X 축에서의 겹침 계산
	_float overlapX = min(maxA_x, maxB_x) - max(minA_x, minB_x);

	// 충돌 여부와 겹침 크기를 반환
	if (overlapX > 0)
		// 충돌이 발생한 경우, 겹침 크기를 반환
		return overlapX;
	else
		//충돌 안함
		return 0;
}

_float3 CBounding_AABB::Get_Overlap_Center_Position(const CBounding_AABB* pOther) const
{
	// 두 AABB의 최소 및 최대 값을 계산 (X 축)
	_float minA_x = m_pDesc->Center.x - m_pDesc->Extents.x;
	_float maxA_x = m_pDesc->Center.x + m_pDesc->Extents.x;

	_float minB_x = pOther->m_pDesc->Center.x - pOther->m_pDesc->Extents.x;
	_float maxB_x = pOther->m_pDesc->Center.x + pOther->m_pDesc->Extents.x;

	// 두 AABB의 최소 및 최대 값을 계산 (Y 축)
	_float minA_y = m_pDesc->Center.y - m_pDesc->Extents.y;
	_float maxA_y = m_pDesc->Center.y + m_pDesc->Extents.y;

	_float minB_y = pOther->m_pDesc->Center.y - pOther->m_pDesc->Extents.y;
	_float maxB_y = pOther->m_pDesc->Center.y + pOther->m_pDesc->Extents.y;

	// X 축에서의 겹침 계산
	_float overlapX = min(maxA_x, maxB_x) - max(minA_x, minB_x);
	// Y 축에서의 겹침 계산
	_float overlapY = min(maxA_y, maxB_y) - max(minA_y, minB_y);

	// 겹침이 발생했는지 확인
	if (overlapX > 0 && overlapY > 0)
	{
		// 겹침 사각형의 최소 점과 최대 점 계산
		_float overlapMinX = max(minA_x, minB_x);
		_float overlapMaxX = min(maxA_x, maxB_x);
		_float overlapMinY = max(minA_y, minB_y);
		_float overlapMaxY = min(maxA_y, maxB_y);

		// 겹침 사각형의 중점 계산
		_float3 center;
		center.x = (overlapMinX + overlapMaxX) / 2.0f;
		center.y = (overlapMinY + overlapMaxY) / 2.0f;
		center.z = m_pDesc->Center.z; // Z축은 무시하거나 필요에 따라 설정

		return { center };
	}
	else
	{
		// 겹침이 발생하지 않음, 쓰레기값으로 채우기
		return { _float3(FLT_MAX, FLT_MAX, FLT_MAX) };
	}
}


_bool CBounding_AABB::Collision_AABB(CBounding_AABB* pTargetDesc)
{
	_float3		vSourMin = Compute_Min();
	_float3		vSourMax = Compute_Max();

	_float3		vDestMin = pTargetDesc->Compute_Min();
	_float3		vDestMax = pTargetDesc->Compute_Max();

	/* 너비로 충돌안했냐? */
	if (max(vSourMin.x, vDestMin.x) > min(vSourMax.x, vDestMax.x))
		return false;
	/* 높이로 충돌안했냐? */
	if (max(vSourMin.y, vDestMin.y) > min(vSourMax.y, vDestMax.y))
		return false;

	return true;
}

_float3 CBounding_AABB::Compute_Min()
{
	return _float3(m_pDesc->Center.x - m_pDesc->Extents.x, m_pDesc->Center.y - m_pDesc->Extents.y, m_pDesc->Center.z - m_pDesc->Extents.z);
}

_float3 CBounding_AABB::Compute_Max()
{
	return _float3(m_pDesc->Center.x + m_pDesc->Extents.x, m_pDesc->Center.y + m_pDesc->Extents.y, m_pDesc->Center.z + m_pDesc->Extents.z);
}

CBounding_AABB* CBounding_AABB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pDesc)
{
	CBounding_AABB* pInstance = new CBounding_AABB(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pDesc)))
	{
		MSG_BOX(TEXT("Failed to Created : CBounding_AABB"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CBounding_AABB::Free()
{
	__super::Free();

	Safe_Delete(m_pOriginalDesc);
	Safe_Delete(m_pDesc);
}

