#include "VIBuffer_Particle3D_Instance.h"
#include "GameInstance.h"

CVIBuffer_Particle3D_Instance::CVIBuffer_Particle3D_Instance(ID3D11Device* pDevice,
	ID3D11DeviceContext* pContext, const PARTICLE3D_INSTANCE_DESC& tDesc)
	: CVIBuffer_Instance{ pDevice, pContext }
	, m_tInitDesc{ tDesc }
{
}

CVIBuffer_Particle3D_Instance::CVIBuffer_Particle3D_Instance(const CVIBuffer_Particle3D_Instance&
	Prototype)
	: CVIBuffer_Instance{ Prototype }
	, m_tInitDesc{ Prototype.m_tInitDesc }
{
}

HRESULT CVIBuffer_Particle3D_Instance::Initialize_Prototype()
{
	m_iNumVertexBuffers = 2;
	m_iNumVertices = 4;
	m_iVertexStride = sizeof(VTXTEX);

	m_iNumIndices = 6;
	m_iIndexStride = 2;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_iInstanceStride = sizeof(VTXPARTICLE3D_INSTANCE);
	if (0 == m_tInitDesc.iNumInstance)
		return E_FAIL;

	m_iMaxInstances = m_tInitDesc.iNumInstance;
	m_iNumInstances = 0;
	m_iIndexCountPerInstance = m_iNumIndices;

	/* quad VB: 단위 쿼드 (-0.5 ~ +0.5) - 셰이더의 vCornerOffset로 사용 */
	D3D11_BUFFER_DESC VertexBufferDesc{};
	VertexBufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.CPUAccessFlags = 0;
	VertexBufferDesc.MiscFlags = 0;
	VertexBufferDesc.StructureByteStride = 0;

	VTXTEX* pVertices = new VTXTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXTEX) * m_iNumVertices);

	pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f); pVertices[0].vTexcoord = _float2(0.f, 0.f);
	pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f); pVertices[1].vTexcoord = _float2(1.f, 0.f);
	pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f); pVertices[2].vTexcoord = _float2(1.f, 1.f);
	pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f); pVertices[3].vTexcoord = _float2(0.f, 1.f);

	D3D11_SUBRESOURCE_DATA VertexInitialData{};
	VertexInitialData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB)))
	{
		Safe_Delete_Array(pVertices);
		return E_FAIL;
	}
	Safe_Delete_Array(pVertices);

	/* IB: 2 triangle */
	D3D11_BUFFER_DESC IndexBufferDesc{};
	IndexBufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexBufferDesc.CPUAccessFlags = 0;
	IndexBufferDesc.MiscFlags = 0;
	IndexBufferDesc.StructureByteStride = 0;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);
	pIndices[0] = 0; pIndices[1] = 1; pIndices[2] = 2;
	pIndices[3] = 0; pIndices[4] = 2; pIndices[5] = 3;

	D3D11_SUBRESOURCE_DATA IndexInitialData{};
	IndexInitialData.pSysMem = pIndices;

	if (FAILED(m_pDevice->CreateBuffer(&IndexBufferDesc, &IndexInitialData, &m_pIB)))
	{
		Safe_Delete_Array(pIndices);
		return E_FAIL;
	}
	Safe_Delete_Array(pIndices);

	/* instance VB desc (Initialize에서 실제 생성) */
	m_InstanceBufferDesc.ByteWidth = m_iMaxInstances * m_iInstanceStride;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;
	m_InstanceBufferDesc.StructureByteStride = 0;

	return S_OK;
}

HRESULT CVIBuffer_Particle3D_Instance::Initialize(void* pArg)
{
	if (FAILED(m_pDevice->CreateBuffer(&m_InstanceBufferDesc, nullptr, &m_pVBInstance)))
		return E_FAIL;
	return S_OK;
}

HRESULT CVIBuffer_Particle3D_Instance::Update_Particle3D_Instances(const VTXPARTICLE3D_INSTANCE*
	pInstances, _uint iNumInstances)
{
	if (iNumInstances > m_iMaxInstances)
		return E_FAIL;

	if (0 == iNumInstances)
	{
		m_iNumInstances = 0;
		return S_OK;
	}

	if (nullptr == pInstances || nullptr == m_pVBInstance || 0 == m_iInstanceStride)
		return E_FAIL;

	D3D11_MAPPED_SUBRESOURCE SubResource{};
	if (FAILED(m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource)))
		return E_FAIL;

	memcpy(SubResource.pData, pInstances, sizeof(VTXPARTICLE3D_INSTANCE) * iNumInstances);

	m_pContext->Unmap(m_pVBInstance, 0);

	m_iNumInstances = iNumInstances;
	return S_OK;
}

CVIBuffer_Particle3D_Instance* CVIBuffer_Particle3D_Instance::Create(ID3D11Device* pDevice,
	ID3D11DeviceContext* pContext, void* pInitialDesc)
{
	auto pDesc = static_cast<PARTICLE3D_INSTANCE_DESC*>(pInitialDesc);

	CVIBuffer_Particle3D_Instance* pInstance = new CVIBuffer_Particle3D_Instance(pDevice, pContext,
		*pDesc);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Particle3D_Instance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CVIBuffer_Particle3D_Instance::Clone(void* pArg)
{
	CVIBuffer_Particle3D_Instance* pInstance = new CVIBuffer_Particle3D_Instance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Particle3D_Instance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Particle3D_Instance::Free()
{
	__super::Free();
}