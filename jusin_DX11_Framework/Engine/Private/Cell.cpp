#include "Cell.h"

CCell::CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CCell::Initialize(const _float3* pPoints)
{
	memcpy(m_vPoints, pPoints, sizeof(_float3) * ETOUI(VTXPOINT::END));



	return S_OK;
}

CCell* CCell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints)
{
	CCell* pInstance = new CCell(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pPoints)))
	{
		MSG_BOX("Failed to Created : CCell");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCell::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
