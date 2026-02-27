#include "..\Public\Frustum.h"
#include "GameInstance.h"

CFrustum::CFrustum()
	: m_pGameInstance { CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CFrustum::Initialize()
{
	/* 투영공간상의 절두체의 여덟개 모서리점을 정의한다. */
	m_vPoints[0] = _float3(-1.f, 1.f, 0.f);
	m_vPoints[1] = _float3(1.f, 1.f, 0.f);
	m_vPoints[2] = _float3(1.f, -1.f, 0.f);
	m_vPoints[3] = _float3(-1.f, -1.f, 0.f);

	m_vPoints[4] = _float3(-1.f, 1.f, 1.f);
	m_vPoints[5] = _float3(1.f, 1.f, 1.f);
	m_vPoints[6] = _float3(1.f, -1.f, 1.f);
	m_vPoints[7] = _float3(-1.f, -1.f, 1.f);

	return S_OK;
}

void CFrustum::Update()
{
	_matrix			ViewMatrixInverse = m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_VIEW);
	_matrix			ProjMatrixInverse = m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_PROJ);

	for (size_t i = 0; i < 8; i++)
	{
		XMStoreFloat3(&m_vWorldPoints[i], XMVector3TransformCoord(XMLoadFloat3(&m_vPoints[i]), ProjMatrixInverse));
		XMStoreFloat3(&m_vWorldPoints[i], XMVector3TransformCoord(XMLoadFloat3(&m_vWorldPoints[i]), ViewMatrixInverse));
	}

	Make_Planes(m_vWorldPlanes, m_vWorldPoints);
}

void CFrustum::Get_ParallelVectorsInPlane(_float3& tangent1, _float3& tangent2, _float fov)
{
	// 시야각에 따른 Y와 Z의 비율 계산
	float tanHalfFovY = tanf(fov / 2.0f);

	// 평면 방정식: A*x + B*y + C*z + D = 0
	// 여기서 x=0으로 고정된 상태에서 y와 z만 고려

	// Y와 Z의 비율을 기반으로 평행 벡터 계산
	// 시야각이 넓을수록 Y가 커지고 Z는 작아짐
	tangent1 = _float3(0.f, 1.f, -tanHalfFovY);  // 첫 번째 평행 벡터
	tangent2 = _float3(0.f, 0.f, 1.f);           // 두 번째 평행 벡터 (단순 Z축 기준)

	// 벡터 정규화
	XMVECTOR vec1 = XMLoadFloat3(&tangent1);
	vec1 = XMVector3Normalize(vec1);
	XMStoreFloat3(&tangent1, vec1);

	XMVECTOR vec2 = XMLoadFloat3(&tangent2);
	vec2 = XMVector3Normalize(vec2);
	XMStoreFloat3(&tangent2, vec2);
}

void CFrustum::Make_Planes(_float4 * pPlanes, const _float3 * pPoints)
{
	/* +x */
	XMStoreFloat4(&pPlanes[0], 
		XMPlaneFromPoints(XMLoadFloat3(&pPoints[1]), XMLoadFloat3(&pPoints[5]), XMLoadFloat3(&pPoints[6])));

	/* -x */
	XMStoreFloat4(&pPlanes[1],
		XMPlaneFromPoints(XMLoadFloat3(&pPoints[4]), XMLoadFloat3(&pPoints[0]), XMLoadFloat3(&pPoints[3])));


	/* +y */
	XMStoreFloat4(&pPlanes[2],
		XMPlaneFromPoints(XMLoadFloat3(&pPoints[4]), XMLoadFloat3(&pPoints[5]), XMLoadFloat3(&pPoints[1])));

	/* -y */
	XMStoreFloat4(&pPlanes[3],
		XMPlaneFromPoints(XMLoadFloat3(&pPoints[3]), XMLoadFloat3(&pPoints[2]), XMLoadFloat3(&pPoints[6])));


	/* +z */
	XMStoreFloat4(&pPlanes[4],
		XMPlaneFromPoints(XMLoadFloat3(&pPoints[5]), XMLoadFloat3(&pPoints[4]), XMLoadFloat3(&pPoints[7])));

	/* -z */
	XMStoreFloat4(&pPlanes[5],
		XMPlaneFromPoints(XMLoadFloat3(&pPoints[0]), XMLoadFloat3(&pPoints[1]), XMLoadFloat3(&pPoints[2])));
}

CFrustum * CFrustum::Create()
{
	CFrustum*		pInstance = new CFrustum();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CFrustum"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFrustum::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
