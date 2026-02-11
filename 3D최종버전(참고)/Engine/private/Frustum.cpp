#include "Frustum.h"
#include "GameInstance.h"

CFrustum::CFrustum()
	: m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CFrustum::Initialize()
{
	m_vOriginalPoints[0] = _float3(-1.f, 1.f, 0.f);
	m_vOriginalPoints[1] = _float3(1.f, 1.f, 0.f);
	m_vOriginalPoints[2] = _float3(1.f, -1.f, 0.f);
	m_vOriginalPoints[3] = _float3(-1.f, -1.f, 0.f);

	m_vOriginalPoints[4] = _float3(-1.f, 1.f, 1.f);
	m_vOriginalPoints[5] = _float3(1.f, 1.f, 1.f);
	m_vOriginalPoints[6] = _float3(1.f, -1.f, 1.f);
	m_vOriginalPoints[7] = _float3(-1.f, -1.f, 1.f);

    return S_OK;
}

void CFrustum::Update()
{
	_matrix			ViewMatrixInverse = m_pGameInstance->Get_Transform_Matrix_Inverse(D3DTS::VIEW);
	_matrix			ProjMatrixInverse = m_pGameInstance->Get_Transform_Matrix_Inverse(D3DTS::PROJECTION);

	_float3			vPoints[8] = {};

	for (size_t i = 0; i < 8; i++)
	{
		XMStoreFloat3(&vPoints[i], XMVector3TransformCoord(XMLoadFloat3(&m_vOriginalPoints[i]), ProjMatrixInverse));
		XMStoreFloat3(&vPoints[i], XMVector3TransformCoord(XMLoadFloat3(&vPoints[i]), ViewMatrixInverse));
	}

	Make_Planes(vPoints, m_vWorldPlanes);

	memcpy(m_vWorldPoints, vPoints, sizeof(_float3) * 8);
}

void CFrustum::Transform_ToLocalSpace(_fmatrix WorldMatrixInverse)
{
	_float3			vPoints[8] = {};


	for (size_t i = 0; i < 8; i++)
	{
		XMStoreFloat3(&vPoints[i], XMVector3TransformCoord(XMLoadFloat3(&m_vWorldPoints[i]), WorldMatrixInverse));

	}
	Make_Planes(vPoints, m_vLocalPlanes);
}


_bool CFrustum::isIn_WorldSpace(_fvector vWorldPos, _float fRadius)
{
	for (size_t i = 0; i < 6; i++)
	{
		/* A, B, C, D */
		/* X, Y, Z, 1 */
		if (fRadius < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_vWorldPlanes[i]), vWorldPos)))
			return false;
	}

    return true;
}

_bool CFrustum::isIn_LocalSpace(_fvector vLocalPos, _float fRadius)
{
	for (size_t i = 0; i < 6; i++)
	{
		/* A, B, C, D */
		/* X, Y, Z, 1 */
		if (fRadius < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_vLocalPlanes[i]), vLocalPos)))
			return false;
	}

	return true;
}
void CFrustum::Make_Planes(const _float3* pPoints, _float4* pPlanes)
{
	XMStoreFloat4(&pPlanes[0], XMPlaneFromPoints(
		XMVectorSetW(XMLoadFloat3(&pPoints[1]), 1.f), 
		XMVectorSetW(XMLoadFloat3(&pPoints[5]), 1.f),
		XMVectorSetW(XMLoadFloat3(&pPoints[6]), 1.f)
	));

	XMStoreFloat4(&pPlanes[1], XMPlaneFromPoints(
		XMVectorSetW(XMLoadFloat3(&pPoints[4]), 1.f),
		XMVectorSetW(XMLoadFloat3(&pPoints[0]), 1.f),
		XMVectorSetW(XMLoadFloat3(&pPoints[3]), 1.f)	
	));

	XMStoreFloat4(&pPlanes[2], XMPlaneFromPoints(
		XMVectorSetW(XMLoadFloat3(&pPoints[4]), 1.f),
		XMVectorSetW(XMLoadFloat3(&pPoints[5]), 1.f),
		XMVectorSetW(XMLoadFloat3(&pPoints[1]), 1.f)
	));

	XMStoreFloat4(&pPlanes[3], XMPlaneFromPoints(
		XMVectorSetW(XMLoadFloat3(&pPoints[3]), 1.f),
		XMVectorSetW(XMLoadFloat3(&pPoints[2]), 1.f),
		XMVectorSetW(XMLoadFloat3(&pPoints[6]), 1.f)
	));

	XMStoreFloat4(&pPlanes[4], XMPlaneFromPoints(
		XMVectorSetW(XMLoadFloat3(&pPoints[5]), 1.f),
		XMVectorSetW(XMLoadFloat3(&pPoints[4]), 1.f),
		XMVectorSetW(XMLoadFloat3(&pPoints[7]), 1.f)
	));

	XMStoreFloat4(&pPlanes[5], XMPlaneFromPoints(
		XMVectorSetW(XMLoadFloat3(&pPoints[0]), 1.f),
		XMVectorSetW(XMLoadFloat3(&pPoints[1]), 1.f),
		XMVectorSetW(XMLoadFloat3(&pPoints[2]), 1.f)
	));
}

CFrustum* CFrustum::Create()
{
	CFrustum* pInstance = new CFrustum();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CFrustum");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFrustum::Free()
{
    __super::Free();

	Safe_Release(m_pGameInstance);
}
