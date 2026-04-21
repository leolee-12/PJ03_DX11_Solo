#include "VIBuffer_Rect_Instance.h"
#include "GameInstance.h"

CVIBuffer_Rect_Instance::CVIBuffer_Rect_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const RECT_INSTANCE_DESC& tDesc)
	: CVIBuffer_Instance{ pDevice, pContext }
	, m_tInitDesc{ tDesc }
{
}

CVIBuffer_Rect_Instance::CVIBuffer_Rect_Instance(const CVIBuffer_Rect_Instance& Prototype)
	: CVIBuffer_Instance{ Prototype }
	, m_tInitDesc{ Prototype.m_tInitDesc }
{
}

HRESULT CVIBuffer_Rect_Instance::Initialize_Prototype()
{
	m_iNumVertexBuffers = 2;
	m_iNumVertices = 4;
	m_iVertexStride = sizeof(VTXTEX);

	m_iNumIndices = 6;
	m_iIndexStride = 2;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_iInstanceStride = sizeof(VTXPARTICLE_INSTANCE);
	m_iNumInstances = m_tInitDesc.iNumInstance;
	m_iIndexCountPerInstance = m_iNumIndices;



	D3D11_BUFFER_DESC VertexBufferDesc{};
	VertexBufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.CPUAccessFlags = 0;
	VertexBufferDesc.MiscFlags = 0;
	VertexBufferDesc.StructureByteStride = 0;

	VTXTEX* pVertices = new VTXTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXTEX) * m_iNumVertices);

	pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
	pVertices[0].vTexcoord = _float2(0.f, 0.f);

	pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
	pVertices[1].vTexcoord = _float2(1.f, 0.f);

	pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
	pVertices[2].vTexcoord = _float2(1.f, 1.f);

	pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
	pVertices[3].vTexcoord = _float2(0.f, 1.f);

	D3D11_SUBRESOURCE_DATA VertexInitialData{};
	VertexInitialData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB)))
	{
		Safe_Delete_Array(pVertices);
		return E_FAIL;
	}

	Safe_Delete_Array(pVertices);



	D3D11_BUFFER_DESC IndexBufferDesc{};
	IndexBufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexBufferDesc.CPUAccessFlags = 0;
	IndexBufferDesc.MiscFlags = 0;
	IndexBufferDesc.StructureByteStride = 0;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;

	pIndices[3] = 0;
	pIndices[4] = 2;
	pIndices[5] = 3;

	D3D11_SUBRESOURCE_DATA IndexInitialData{};
	IndexInitialData.pSysMem = pIndices;

	if (FAILED(m_pDevice->CreateBuffer(&IndexBufferDesc, &IndexInitialData, &m_pIB)))
	{
		Safe_Delete_Array(pIndices);
		return E_FAIL;
	}

	Safe_Delete_Array(pIndices);



	D3D11_BUFFER_DESC InstanceBufferDesc{};
	InstanceBufferDesc.ByteWidth = m_iNumInstances * m_iInstanceStride;
	InstanceBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	InstanceBufferDesc.CPUAccessFlags = 0;
	InstanceBufferDesc.MiscFlags = 0;
	InstanceBufferDesc.StructureByteStride = 0;

	VTXPARTICLE_INSTANCE* pInstanceVertices = new VTXPARTICLE_INSTANCE[m_iNumInstances];
	ZeroMemory(pInstanceVertices, sizeof(VTXPARTICLE_INSTANCE) * m_iNumInstances);
	
	const _vector vCenter = XMLoadFloat3(&m_tInitDesc.vCenter);
	const _vector vHalfRange = XMLoadFloat3(&m_tInitDesc.vRange) * 0.5f;
	
	_float3 vMinPos, vMaxPos;
	XMStoreFloat3(&vMinPos, vCenter - vHalfRange);
	XMStoreFloat3(&vMaxPos, vCenter + vHalfRange);

	for (size_t i = 0; i < m_iNumInstances; i++)
	{
		_float fSize = m_pGameInstance->Random(m_tInitDesc.fMinSize, m_tInitDesc.fMaxSize);

		pInstanceVertices[i].vRight			= _float4(fSize, 0.f, 0.f, 0.f);
		pInstanceVertices[i].vUp			= _float4(0.f, fSize, 0.f, 0.f);
		pInstanceVertices[i].vLook			= _float4(0.f, 0.f, fSize, 0.f);
		pInstanceVertices[i].vTranslation	= _float4(
			m_pGameInstance->Random(vMinPos.x, vMaxPos.x),
			m_pGameInstance->Random(vMinPos.y, vMaxPos.y),
			m_pGameInstance->Random(vMinPos.z, vMaxPos.z),
			1.f
		);
	}

	D3D11_SUBRESOURCE_DATA InstanceInitialData{};
	InstanceInitialData.pSysMem = pInstanceVertices;

	if (FAILED(m_pDevice->CreateBuffer(&InstanceBufferDesc, &InstanceInitialData, &m_pVBInstance)))
	{
		Safe_Delete_Array(pInstanceVertices);
		return E_FAIL;
	}

	Safe_Delete_Array(pInstanceVertices);

	return S_OK;
}

HRESULT CVIBuffer_Rect_Instance::Initialize(void* pArg)
{
	return S_OK;
}

CVIBuffer_Rect_Instance* CVIBuffer_Rect_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pInitialDesc)
{
	auto pDesc = static_cast<RECT_INSTANCE_DESC*>(pInitialDesc);

	CVIBuffer_Rect_Instance* pInstance = new CVIBuffer_Rect_Instance(pDevice, pContext, *pDesc);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Rect_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Rect_Instance::Clone(void* pArg)
{
	CVIBuffer_Rect_Instance* pInstance = new CVIBuffer_Rect_Instance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Rect_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CVIBuffer_Rect_Instance::Free()
{
	__super::Free();



}
