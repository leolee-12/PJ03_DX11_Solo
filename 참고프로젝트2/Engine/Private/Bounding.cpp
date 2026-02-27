#include "..\Public\Bounding.h"

CBounding::CBounding(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}



_bool CBounding::isPointInAABB(const _float3& point)
{
	return _bool();
}

void CBounding::Free()
{
	__super::Free();
	
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

}
