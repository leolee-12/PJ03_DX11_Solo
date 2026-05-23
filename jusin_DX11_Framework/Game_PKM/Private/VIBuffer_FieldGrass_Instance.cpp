#include "VIBuffer_FieldGrass_Instance.h"

CVIBuffer_FieldGrass_Instance::CVIBuffer_FieldGrass_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const FIELDGRASS_INSTANCE_DESC& tDesc)
	: CVIBuffer_Instance{ pDevice, pContext }
	, m_tInitDesc{ tDesc }
{
}

CVIBuffer_FieldGrass_Instance::CVIBuffer_FieldGrass_Instance(const CVIBuffer_FieldGrass_Instance& Prototype)
	: CVIBuffer_Instance{ Prototype }
	, m_tInitDesc{ Prototype.m_tInitDesc }
{
}

HRESULT CVIBuffer_FieldGrass_Instance::Initialize_Prototype()
{
	if (0 == m_tInitDesc.iNumInstance || nullptr == m_tInitDesc.pModelFilePath)
		return E_FAIL;

	m_iNumVertexBuffers = 2;
	m_iVertexStride = sizeof(VTXMESH);
	m_iIndexStride = sizeof(_uint);
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_iInstanceStride = sizeof(VTXFIELDGRASS_INSTANCE);
	m_iMaxInstances = m_tInitDesc.iNumInstance;
	m_iNumInstances = 0;

	if (FAILED(Ready_Mesh_FromWModel(m_tInitDesc.pModelFilePath)))
		return E_FAIL;

	m_iIndexCountPerInstance = m_iNumIndices;

	if (FAILED(Ready_InstanceBufferDesc()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_FieldGrass_Instance::Initialize(void* pArg)
{
	if (FAILED(m_pDevice->CreateBuffer(&m_InstanceBufferDesc, nullptr, &m_pVBInstance)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_FieldGrass_Instance::Ready_Mesh_FromWModel(const _char * pModelFilePath)
{
	FILE* fp = nullptr;
	if (0 != fopen_s(&fp, pModelFilePath, "rb") || nullptr == fp)
		return E_FAIL;

	WMODEL_HEADER tHeader{};
	fread(&tHeader, sizeof(WMODEL_HEADER), 1, fp);

	if (memcmp(tHeader.szMagic, "WMDL", 4) != 0 ||
		tHeader.iVersion != 2 ||
		static_cast<MODEL>(tHeader.iModelType) != MODEL::NONANIM ||
		0 == tHeader.iNumMeshes)
	{
		fclose(fp);
		return E_FAIL;
	}

	fseek(fp, static_cast<long>(sizeof(WMODEL_BONE) * tHeader.iNumBones), SEEK_CUR);

	for (_uint i = 0; i < tHeader.iNumMaterials; ++i)
	{
		for (_uint j = 0; j < ETOUI(MATERIAL_TYPE::END); ++j)
		{
			_uint iNumTextures = 0;
			fread(&iNumTextures, sizeof(_uint), 1, fp);

			for (_uint k = 0; k < iNumTextures; ++k)
			{
				_uint iLen = 0;
				fread(&iLen, sizeof(_uint), 1, fp);
				fseek(fp, static_cast<long>(iLen), SEEK_CUR);
			}
		}
	}

	_char szMeshName[MAX_PATH] = {};
	_uint iMaterialIndex = 0;
	_uint iNumVertices = 0;
	_uint iNumIndices = 0;
	_uint iNumBones = 0;

	fread(szMeshName, 1, MAX_PATH, fp);
	fread(&iMaterialIndex, sizeof(_uint), 1, fp);
	fread(&iNumVertices, sizeof(_uint), 1, fp);
	fread(&iNumIndices, sizeof(_uint), 1, fp);
	fread(&iNumBones, sizeof(_uint), 1, fp);

	vector<VTXMESH> Vertices(iNumVertices);
	vector<_uint> Indices(iNumIndices);

	fread(Vertices.data(), sizeof(VTXMESH), iNumVertices, fp);
	fread(Indices.data(), sizeof(_uint), iNumIndices, fp);

	fclose(fp);

	m_iNumVertices = iNumVertices;
	m_iNumIndices = iNumIndices;

	D3D11_BUFFER_DESC VertexBufferDesc{};
	VertexBufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA VertexInitialData{};
	VertexInitialData.pSysMem = Vertices.data();

	if (FAILED(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB)))
		return E_FAIL;

	D3D11_BUFFER_DESC IndexBufferDesc{};
	IndexBufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA IndexInitialData{};
	IndexInitialData.pSysMem = Indices.data();

	if (FAILED(m_pDevice->CreateBuffer(&IndexBufferDesc, &IndexInitialData, &m_pIB)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_FieldGrass_Instance::Ready_InstanceBufferDesc()
{
	m_InstanceBufferDesc.ByteWidth = m_iMaxInstances * m_iInstanceStride;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;
	m_InstanceBufferDesc.StructureByteStride = 0;

	return S_OK;
}


HRESULT CVIBuffer_FieldGrass_Instance::Update_FieldGrass_Instances(const VTXFIELDGRASS_INSTANCE * pInstances, _uint iNumInstances)
{
	if (iNumInstances > m_iMaxInstances)
		return E_FAIL;

	if (0 == iNumInstances)
	{
		m_iNumInstances = 0;
		return S_OK;
	}

	if (nullptr == pInstances || nullptr == m_pVBInstance)
		return E_FAIL;

	D3D11_MAPPED_SUBRESOURCE SubResource{};
	if (FAILED(m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource)))
		return E_FAIL;

	memcpy(SubResource.pData, pInstances, sizeof(VTXFIELDGRASS_INSTANCE) * iNumInstances);

	m_pContext->Unmap(m_pVBInstance, 0);

	m_iNumInstances = iNumInstances;
	return S_OK;
}

CVIBuffer_FieldGrass_Instance* CVIBuffer_FieldGrass_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pInitialDesc)
{
	auto pDesc = static_cast<FIELDGRASS_INSTANCE_DESC*>(pInitialDesc);
	if (nullptr == pDesc)
		return nullptr;

	CVIBuffer_FieldGrass_Instance* pInstance =
		new CVIBuffer_FieldGrass_Instance(pDevice, pContext, *pDesc);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_FieldGrass_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_FieldGrass_Instance::Clone(void* pArg)
{
	CVIBuffer_FieldGrass_Instance* pInstance = new CVIBuffer_FieldGrass_Instance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_FieldGrass_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_FieldGrass_Instance::Free()
{
	__super::Free();
}