#include "VIBuffer_Point_Instance.h"
#include "GameInstance.h"

CVIBuffer_Point_Instance::CVIBuffer_Point_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const POINT_INSTANCE_DESC& tDesc)
	: CVIBuffer_Instance{ pDevice, pContext }
	, m_tInitDesc{ tDesc }
{
}

CVIBuffer_Point_Instance::CVIBuffer_Point_Instance(const CVIBuffer_Point_Instance& Prototype)
	: CVIBuffer_Instance{ Prototype }
	, m_tInitDesc{ Prototype.m_tInitDesc }
	, m_pSpeeds{ Prototype.m_pSpeeds }
	, m_isLoop{ Prototype.m_isLoop }
	, m_iVertexCountPerInstance{ Prototype.m_iVertexCountPerInstance }
	, m_vPivot{ Prototype.m_vPivot }
{
}

HRESULT CVIBuffer_Point_Instance::Initialize_Prototype()
{
	m_iNumVertexBuffers = 2;
	m_iNumVertices = 1;
	m_iVertexStride = sizeof(VTXPOS);

	m_iNumIndices = 0;
	m_iIndexStride = 0;
	m_eIndexFormat = DXGI_FORMAT_UNKNOWN;
	m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;

	m_iInstanceStride = sizeof(VTXPARTICLE_INSTANCE);
	m_iNumInstances = m_tInitDesc.iNumInstance;
	m_iVertexCountPerInstance = 1;



	D3D11_BUFFER_DESC VertexBufferDesc{};
	VertexBufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.CPUAccessFlags = 0;
	VertexBufferDesc.MiscFlags = 0;
	VertexBufferDesc.StructureByteStride = 0;

	VTXPOS* pVertices = new VTXPOS[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOS) * m_iNumVertices);

	D3D11_SUBRESOURCE_DATA VertexInitialData{};
	VertexInitialData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB)))
	{
		Safe_Delete_Array(pVertices);
		return E_FAIL;
	}

	Safe_Delete_Array(pVertices);



	m_InstanceBufferDesc.ByteWidth = m_iNumInstances * m_iInstanceStride;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;
	m_InstanceBufferDesc.StructureByteStride = 0;

	m_pInstanceVertices = new VTXPARTICLE_INSTANCE[m_iNumInstances];
	ZeroMemory(m_pInstanceVertices, sizeof(VTXPARTICLE_INSTANCE) * m_iNumInstances);
	m_pSpeeds = new _float[m_iNumInstances];
	m_isLoop = m_tInitDesc.isLoop;
	m_vPivot = m_tInitDesc.vPivot;

	const _vector vCenter = XMLoadFloat3(&m_tInitDesc.vCenter);
	const _vector vHalfRange = XMLoadFloat3(&m_tInitDesc.vPosOffset) * 0.5f;
	
	_float3 vMinPos, vMaxPos;
	XMStoreFloat3(&vMinPos, vCenter - vHalfRange);
	XMStoreFloat3(&vMaxPos, vCenter + vHalfRange);

	for (size_t i = 0; i < m_iNumInstances; i++)
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

HRESULT CVIBuffer_Point_Instance::Initialize(void* pArg)
{
	D3D11_SUBRESOURCE_DATA InstanceInitialData{};
	InstanceInitialData.pSysMem = m_pInstanceVertices;

	if (FAILED(m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &InstanceInitialData, &m_pVBInstance)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Point_Instance::Bind_Resources()
{
	ID3D11Buffer* pVertexBuffers[] =
	{
		   m_pVB,
		   m_pVBInstance
	};

	_uint		iVertexStrides[] =
	{
		m_iVertexStride,
		m_iInstanceStride,
	};

	_uint		iOffsets[] =
	{
		0, 0
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveType);

	return S_OK;
}

HRESULT CVIBuffer_Point_Instance::Render()
{
	m_pContext->DrawInstanced(m_iVertexCountPerInstance, m_iNumInstances, 0, 0);

	return S_OK;
}

void CVIBuffer_Point_Instance::Drop(_float fTimeDelta)
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

void CVIBuffer_Point_Instance::Spread(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXPARTICLE_INSTANCE* pInstanceVertices = static_cast<VTXPARTICLE_INSTANCE*>(SubResource.pData);

	for (size_t i = 0; i < m_iNumInstances; i++)
	{
		_vector vDir = XMVectorSetW(XMLoadFloat4(&pInstanceVertices[i].vTranslation) - XMLoadFloat3(&m_vPivot), 0.f);

		XMStoreFloat4(&pInstanceVertices[i].vTranslation,
			XMLoadFloat4(&pInstanceVertices[i].vTranslation) + XMVector3Normalize(vDir) * m_pSpeeds[i] * fTimeDelta);

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

CVIBuffer_Point_Instance* CVIBuffer_Point_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pInitialDesc)
{
	auto pDesc = static_cast<POINT_INSTANCE_DESC*>(pInitialDesc);

	CVIBuffer_Point_Instance* pInstance = new CVIBuffer_Point_Instance(pDevice, pContext, *pDesc);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Point_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Point_Instance::Clone(void* pArg)
{
	CVIBuffer_Point_Instance* pInstance = new CVIBuffer_Point_Instance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Point_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CVIBuffer_Point_Instance::Free()
{
	__super::Free();

	if (false == m_isCloned)
		Safe_Delete_Array(m_pSpeeds);
}
