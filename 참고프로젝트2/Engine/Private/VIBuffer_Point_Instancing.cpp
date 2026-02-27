#include "..\Public\VIBuffer_Point_Instancing.h"

CVIBuffer_Point_Instancing::CVIBuffer_Point_Instancing(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer_Instancing { pDevice, pContext }
{
}

CVIBuffer_Point_Instancing::CVIBuffer_Point_Instancing(const CVIBuffer_Point_Instancing & Prototype)
	: CVIBuffer_Instancing { Prototype }
{
}

HRESULT CVIBuffer_Point_Instancing::Initialize_Prototype(const CVIBuffer_Instancing::VIBUFFER_INSTANCE_DESC* pInitialDesc)
{
	if (FAILED(__super::Initialize_Prototype(pInitialDesc)))
		return E_FAIL;

	m_iNumVertexBuffers = 2;
	m_iNumVertices = 1;
	m_iVertexStride = sizeof(VTXPOS);
	m_iInstanceVertexStride = sizeof(VTXINSTANCE);
	m_iNumIndexPerInstance = 1;
	m_iNumIndices = m_iNumIndexPerInstance * m_iNumInstance;
	m_iIndexStride = 2;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitive_Topology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

#pragma region VERTEX_BUFFER
	
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적할당한다. 추후 우리가 직접 정점버퍼의 공간을 변경할 일이 없다. */
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	/* 장치가 할당해주는 공간에 채워넣어야할 값들을 내가 미리 준비한다. */
	VTXPOS*			pVertices = new VTXPOS[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOS) * m_iNumVertices);	
	
	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER
	m_BufferDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적할당한다. 추후 우리가 직접 정점버퍼의 공간을 변경할 일이 없다. */
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = /*m_iIndexStride*/0;

	_ushort*	pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);	

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion


#pragma region INSTANCE_BUFFER
	m_pInstanceVertices = new VTXINSTANCE[m_iNumInstance];
	ZeroMemory(m_pInstanceVertices, sizeof(VTXINSTANCE) * m_iNumInstance);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_float		fScale = Get_Random(pInitialDesc->vScale.x, pInitialDesc->vScale.y);

		m_pInstanceVertices[i].vRight = _float4(fScale, 0.f, 0.f, 0.f);
		m_pInstanceVertices[i].vUp = _float4(0.f, fScale, 0.f, 0.f);
		m_pInstanceVertices[i].vLook = _float4(0.f, 0.f, fScale, 0.f);
		m_pInstanceVertices[i].vTranslation = _float4(
			Get_Random(pInitialDesc->vCenter.x - pInitialDesc->vRange.x * 0.5f, pInitialDesc->vCenter.x + pInitialDesc->vRange.x * 0.5f),
			Get_Random(pInitialDesc->vCenter.y - pInitialDesc->vRange.y * 0.5f, pInitialDesc->vCenter.y + pInitialDesc->vRange.y * 0.5f),
			Get_Random(pInitialDesc->vCenter.z - pInitialDesc->vRange.z * 0.5f, pInitialDesc->vCenter.z + pInitialDesc->vRange.z * 0.5f),
			1.f);
		m_pInstanceVertices[i].vLifeTime.x = Get_Random(pInitialDesc->vLifeTime.x, pInitialDesc->vLifeTime.y);
		m_pInstanceVertices[i].vLifeTime.y = -RandomBetween(0.f, m_pInstanceVertices[i].vLifeTime.x);
		m_pInstanceVertices[i].vMoveDir = _float3(0.f, 0.f, 0.f);
	}
#pragma endregion

	return S_OK;
}



HRESULT CVIBuffer_Point_Instancing::Initialize(void * pArg)
{
	if (FAILED(Create_InstanceBuffer()))
		return E_FAIL;

	return S_OK;
}

CVIBuffer_Point_Instancing * CVIBuffer_Point_Instancing::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const CVIBuffer_Instancing::VIBUFFER_INSTANCE_DESC* pInitialDesc)
{
	CVIBuffer_Point_Instancing*		pInstance = new CVIBuffer_Point_Instancing(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pInitialDesc)))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Point_Instancing"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_Point_Instancing::Clone(void * pArg)
{
	CVIBuffer_Point_Instancing*		pInstance = new CVIBuffer_Point_Instancing(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CVIBuffer_Point_Instancing"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CVIBuffer_Point_Instancing::Free()
{
	__super::Free();

	
}
