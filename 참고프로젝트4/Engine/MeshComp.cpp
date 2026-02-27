#include "MeshComp.h"

#include "GameInstance.h"

#include "Shader.h"
#include "MeshContainer.h"
#include "BoneContainer.h"


IMPLEMENT_CLONE(CMeshComp, CComponent)
IMPLEMENT_CREATE(CMeshComp)

CMeshComp::CMeshComp(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: Base(pDevice, pContext)
{
}

CMeshComp::CMeshComp(const CMeshComp& rhs)
    : Base(rhs)
	, m_pMeshData(rhs.m_pMeshData)
{
	Safe_AddRef(m_pMeshData);
}

HRESULT CMeshComp::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CMeshComp::Initialize(void* Arg)
{

    return S_OK;
}

void CMeshComp::Free()
{
	__super::Free();

	Safe_Release(m_pMeshData);
}

_uint CMeshComp::Get_MeshMaterialIndex() const
{
	if (nullptr == m_pMeshData)
		return 0;

	return m_pMeshData->iMaterialIndex;
}

HRESULT CMeshComp::Load_Mesh(const wstring& strModelFilePath, _uint iIndex)
{
	if (nullptr == m_pGameInstance)
		RETURN_EFAIL;

	auto pMeshData = m_pGameInstance->Find_MeshData(strModelFilePath, iIndex);
	if (nullptr == pMeshData)
		RETURN_EFAIL;

	// 메시 데이터를 저장한다.
	m_pMeshData = ConCast<FMeshData*>(pMeshData);
	Safe_AddRef(m_pMeshData);

	return S_OK;
}

HRESULT CMeshComp::Bind_BoneMatricesToShader(CShader* pShader, const _char* pConstantName, const CBoneGroup& BoneGroup)
{
	if (nullptr == pShader || nullptr == m_pMeshData)
		RETURN_EFAIL;

	vector<_float4x4> matBind;
	matBind.resize(800, {});

	_uint iSize = BoneGroup.CRef_BoneDatas_Count();
	const vector<_float4x4>& CombinedTransforms = BoneGroup.CRef_BoneCombinedTransforms();

	size_t iMeshBoneCount = m_pMeshData->vecMeshBoneDatas.size();
	for (size_t i = 0; i < iMeshBoneCount; i++)
	{
		const _float4x4 CombinedMatrix = CombinedTransforms[m_pMeshData->vecMeshBoneDatas[i].iBoneID];

		XMStoreFloat4x4(&matBind[i],
			XMLoadFloat4x4(&m_pMeshData->vecMeshBoneDatas[i].matOffset) * XMLoadFloat4x4(&CombinedMatrix));
	}

	return pShader->Bind_Matrices(pConstantName, matBind.data(), iSize);
}

HRESULT CMeshComp::Bind_VIBuffer()
{
	if (nullptr == m_pGameInstance || nullptr == m_pMeshData)
		RETURN_EFAIL;

	if (nullptr == m_pMeshData->pVB || nullptr == m_pMeshData->pIB)
		RETURN_EFAIL;

	ID3D11Buffer* pVBs[] = {
		m_pMeshData->pVB.Get()
	};

	_uint           iStrides[] = {
		m_pMeshData->iVertexStride
	};

	_uint           iOffsets[] = {
		0,
	};

	auto pContext = m_pContext.Get();

	/* 어떤 버텍스 버퍼들을 이용할거다. */
	pContext->IASetVertexBuffers(0, 1, pVBs, iStrides, iOffsets);

	/* 어떤 인덱스 버퍼를 이용할거다. */
	pContext->IASetIndexBuffer(m_pMeshData->pIB.Get(), m_pMeshData->eIndexFormat, 0);

	/* 정점을 어떤식으로 이어서 그릴거다. */
	pContext->IASetPrimitiveTopology(m_pMeshData->eTopology);

	return S_OK;
}

HRESULT CMeshComp::Render_VIBuffer()
{
	if (nullptr == m_pGameInstance || nullptr == m_pMeshData)
		RETURN_EFAIL;

	m_pContext->DrawIndexed(m_pMeshData->iNumIndices, 0, 0);

	return S_OK;
}

HRESULT CMeshComp::Bind_VTFBuffer()
{
	if (nullptr == m_pGameInstance || nullptr == m_pMeshData)
		RETURN_EFAIL;

	m_pContext->DrawIndexed(m_pMeshData->iNumIndices, 0, 0);

	return S_OK;
}

HRESULT CMeshComp::Render_VTFBuffer()
{
	if (nullptr == m_pGameInstance || nullptr == m_pMeshData)
		RETURN_EFAIL;

	m_pContext->DrawIndexed(m_pMeshData->iNumIndices, 0, 0);

	return S_OK;
}


_bool CMeshComp::Intersect_MousePos(SPACETYPE eSpacetype, _float3* pOut, _matrix matWorld, _float* pLengthOut)
{
	/*if (typeid(*this) == typeid(CMesh))
	{
		if (static_cast<CMesh*>(this)->Get_ModelType() == MODELTYPE::ANIM)
		{
			D3D11_MAPPED_SUBRESOURCE	SubResource;
			if (FAILED(m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource)))
				return false;

			for (_uint i = 0; i < m_vecVertexInfo.size(); i++) {
				m_vecVertexInfo[i] = ((VTXMESH*)(SubResource.pData))[i].vPosition;
			}

			m_pContext->Unmap(m_pVB, 0);
		}
	}*/

	if (m_pMeshData->iNumVertices == 0)
		return false;

	_float fMinLength = INFINITY;
	_float3 vOut = {};
	_bool  bCheck = false;
	for (_uint i = 0; i < m_pMeshData->iNumIndices / 3.f; i++) 
	{
		_uint3 iIndices = {};
		iIndices.iX = m_pMeshData->vecIndices[i * 3];
		iIndices.iY = m_pMeshData->vecIndices[i * 3 + 1];
		iIndices.iZ = m_pMeshData->vecIndices[i * 3 + 2];
		

		_vector vVec1, vVec2, vVec3;

		vVec1 = XMLoadFloat3(&m_pMeshData->vecVertices[iIndices.iX]);
		vVec2 = XMLoadFloat3(&m_pMeshData->vecVertices[iIndices.iY]);
		vVec3 = XMLoadFloat3(&m_pMeshData->vecVertices[iIndices.iZ]);

		if (m_pGameInstance->Mouse_RayIntersect(eSpacetype, pOut, vVec1, vVec2, vVec3, matWorld, pLengthOut))
		{
			if (*pLengthOut < fMinLength)
			{
				fMinLength = *pLengthOut;
				vOut = *pOut;
				bCheck = true;
			}
		}
	}

	*pOut = vOut;
	*pLengthOut = fMinLength;

	return bCheck;
}