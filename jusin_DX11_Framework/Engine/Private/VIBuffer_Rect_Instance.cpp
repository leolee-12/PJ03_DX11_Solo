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
	, m_pSpeeds{ Prototype.m_pSpeeds }
	, m_isLoop{ Prototype.m_isLoop }
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
	m_iMaxInstances = m_tInitDesc.iNumInstance;
	m_iNumInstances = m_iMaxInstances;
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



	m_InstanceBufferDesc.ByteWidth = m_iMaxInstances * m_iInstanceStride;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;
	m_InstanceBufferDesc.StructureByteStride = 0;

	m_pInstanceVertices = new VTXPARTICLE_INSTANCE[m_iMaxInstances];
	ZeroMemory(m_pInstanceVertices, sizeof(VTXPARTICLE_INSTANCE) * m_iMaxInstances);
	m_pSpeeds = new _float[m_iMaxInstances];
	m_isLoop = m_tInitDesc.isLoop;

	const _vector vCenter = XMLoadFloat3(&m_tInitDesc.vCenter);
	const _vector vHalfRange = XMLoadFloat3(&m_tInitDesc.vPosOffset) * 0.5f;
	
	_float3 vMinPos, vMaxPos;
	XMStoreFloat3(&vMinPos, vCenter - vHalfRange);
	XMStoreFloat3(&vMaxPos, vCenter + vHalfRange);

	for (size_t i = 0; i < m_iMaxInstances; i++)
	{
		m_pSpeeds[i] = m_pGameInstance->Random(m_tInitDesc.vSpeedRange.x, m_tInitDesc.vSpeedRange.y);

		_float fSize = m_pGameInstance->Random(m_tInitDesc.vSizeRange.x, m_tInitDesc.vSizeRange.y);

		m_pInstanceVertices[i].vRight		= _float4(fSize, 0.f, 0.f, 0.f);
		m_pInstanceVertices[i].vUp			= _float4(0.f, fSize, 0.f, 0.f);
		m_pInstanceVertices[i].vLook		= _float4(0.f, 0.f, fSize, 0.f);
		m_pInstanceVertices[i].vTranslation	= _float4(
			m_pGameInstance->Random(vMinPos.x, vMaxPos.x),
			m_pGameInstance->Random(vMinPos.y, vMaxPos.y),
			m_pGameInstance->Random(vMinPos.z, vMaxPos.z),
			1.f
		);

		m_pInstanceVertices[i].vLifeTime = _float2(
			m_pGameInstance->Random(m_tInitDesc.vLifeRange.x, m_tInitDesc.vLifeRange.y),
			0.f
		);
	}

	return S_OK;
}

HRESULT CVIBuffer_Rect_Instance::Initialize(void* pArg)
{
	D3D11_SUBRESOURCE_DATA InstanceInitialData{};
	InstanceInitialData.pSysMem = m_pInstanceVertices;

	if (FAILED(m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &InstanceInitialData, &m_pVBInstance)))
		return E_FAIL;

	return S_OK;
}

void CVIBuffer_Rect_Instance::Drop(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXPARTICLE_INSTANCE* pInstanceVertices = static_cast<VTXPARTICLE_INSTANCE*>(SubResource.pData);

	for (size_t i = 0; i < m_iNumInstances; i++)
	{
		pInstanceVertices[i].vTranslation.y -= m_pSpeeds[i] * fTimeDelta;
		pInstanceVertices[i].vLifeTime.y += fTimeDelta;

		if (true == m_isLoop &&
			pInstanceVertices[i].vLifeTime.x < pInstanceVertices[i].vLifeTime.y)
		{
			pInstanceVertices[i].vTranslation = m_pInstanceVertices[i].vTranslation;
			pInstanceVertices[i].vLifeTime.y = 0.f;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Rect_Instance::Spread(_float fTimeDelta)
{
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

	if (false == m_isCloned)
		Safe_Delete_Array(m_pSpeeds);
}
