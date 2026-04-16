#include "Collider.h"

CCollider::CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, COLLIDER eType)
	: CComponent{ pDevice, pContext }
	, m_eType{ eType }
{
}

CCollider::CCollider(const CCollider& Prototype)
	: CComponent{ Prototype }
	, m_eType{ Prototype.m_eType }
{
}

HRESULT CCollider::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCollider::Initialize(void* pArg)
{
	auto    pBoundingDesc = static_cast<CBounding::BOUNDING_DESC*>(pArg);

	switch (m_eType)
	{
	case COLLIDER::AABB:
		m_pBounding = CBounding_AABB::Create(m_pDevice, m_pContext, pBoundingDesc);
		break;
	case COLLIDER::OBB:
		break;
	case COLLIDER::SPHERE:
		break;
	}


	return S_OK;
}

CCollider* CCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, COLLIDER eType)
{
	CCollider* pInstance = new CCollider(pDevice, pContext, eType);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CCollider::Clone(void* pArg)
{
	CCollider* pInstance = new CCollider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CCollider::Free()
{
	__super::Free();

}
