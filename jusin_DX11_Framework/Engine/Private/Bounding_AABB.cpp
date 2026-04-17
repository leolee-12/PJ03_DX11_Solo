#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"
#include "DebugDraw.h"

CBounding_AABB::CBounding_AABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBounding{ pDevice, pContext }
{
}

HRESULT CBounding_AABB::Initialize(const CBounding::BOUNDING_DESC* pBoundingDesc)
{
	auto pDesc = static_cast<const CBounding_AABB::BOUNDING_AABB_DESC*>(pBoundingDesc);

	m_pOriginalDesc = new BoundingBox(pDesc->vCenter, _float3(pDesc->vSize.x * 0.5f, pDesc->vSize.y * 0.5f, pDesc->vSize.z * 0.5f));
	m_pDesc = new BoundingBox(*m_pOriginalDesc);

	return S_OK;
}

void XM_CALLCONV CBounding_AABB::Update(_fmatrix TransformMatrix)
{
	_matrix Matrix = TransformMatrix;

	Matrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVector3Length(Matrix.r[0]);
	Matrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVector3Length(Matrix.r[1]);
	Matrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVector3Length(Matrix.r[2]);

	m_pOriginalDesc->Transform(*m_pDesc, Matrix);
}

#ifdef _DEBUG
HRESULT CBounding_AABB::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
	DX::Draw(pBatch, *m_pDesc, true == m_isColl ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));

	return S_OK;
}
#endif

CBounding_AABB* CBounding_AABB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CBounding::BOUNDING_DESC* pDesc)
{
	CBounding_AABB* pInstance = new CBounding_AABB(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pDesc)))
	{
		MSG_BOX("Failed to Created : CBounding_AABB");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_AABB::Free()
{
	__super::Free();

	Safe_Delete(m_pDesc);
	Safe_Delete(m_pOriginalDesc);
}
