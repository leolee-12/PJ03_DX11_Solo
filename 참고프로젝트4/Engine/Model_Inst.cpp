#include "..\Public\Model_Inst.h"

#include "GameInstance.h"
#include "Mesh_Data.h"
#include "ModelContainer.h"
#include "Texture_Data.h"
#include "MeshContainer.h"

#include "GameObject.h"

IMPLEMENT_CLONE(CModel_Inst, CComponent)
IMPLEMENT_CREATE(CModel_Inst)

CModel_Inst::CModel_Inst(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CVIBuffer_Model_Instancing(pDevice, pContext)
{
}

CModel_Inst::CModel_Inst(const CModel_Inst& rhs)
	: CVIBuffer_Model_Instancing(rhs), m_iMeshesNum(rhs.m_iMeshesNum), m_iMaterialsNum(rhs.m_iMaterialsNum),
	m_matPivot(rhs.m_matPivot), m_vecMesh(rhs.m_vecMesh), m_strModelFilePath(rhs.m_strModelFilePath),
	m_vecMaterial(rhs.m_vecMaterial)
{

}

HRESULT CModel_Inst::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CModel_Inst::Initialize(void* pArg)
{

	MESH_INSTANCE_DESC* pInstanceDesc = ((MESH_INSTANCE_DESC*)pArg);
	m_strModelFilePath = pInstanceDesc->strName;

	auto pModelData = m_pGameInstance->Find_ModelData(StrToWstr(m_strModelFilePath));

	if (pModelData != nullptr)
	{
		if (FAILED(Ready_Meshes(pModelData, m_strModelFilePath)))
			RETURN_EFAIL;

		if (FAILED(Ready_Materials(pModelData, m_strModelFilePath)))
			RETURN_EFAIL;

		if (pArg == nullptr)
			RETURN_EFAIL;

		m_vecInstanceVertex = pInstanceDesc->vecInstanceVertex;

		m_iNumInstance = Cast<_uint>(pInstanceDesc->vecInstanceVertex.size());
		m_iInstanceStride = sizeof(VTXINSTANCEMESH);

		if (FAILED(__super::Init_InstanceBuffer()))
			RETURN_EFAIL;
	}
	return S_OK;
}

void CModel_Inst::Update(_cref_time fTimeDelta)
{
	__super::Update(fTimeDelta);
}

HRESULT CModel_Inst::Render(_uint iMeshIndx)
{

	if (iMeshIndx >= m_vecMesh.size())
		RETURN_EFAIL;


	ID3D11Buffer* pVerTexBuffers[] = { m_vecMesh[iMeshIndx]->Get_MeshData()->pVB.Get(),m_pVBInstance };


	_uint			iStrides[] = { m_vecMesh[iMeshIndx]->Get_MeshData()->iVertexStride,m_iInstanceStride };


	_uint			iOffsets[] = {
		0,
		0
	};

	m_iNumVertexBuffers = 2;

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVerTexBuffers, iStrides, iOffsets);

	m_pContext->IASetIndexBuffer(m_vecMesh[iMeshIndx]->Get_MeshData()->pIB.Get(),
		m_vecMesh[iMeshIndx]->Get_MeshData()->eIndexFormat, 0);

	m_pContext->IASetPrimitiveTopology(m_vecMesh[iMeshIndx]->Get_MeshData()->eTopology);

	m_pContext->DrawIndexedInstanced(m_vecMesh[iMeshIndx]->Get_MeshData()->iNumIndices,
		m_iNumInstance, 0, 0, 0);

	return S_OK;
}

HRESULT CModel_Inst::Bind_ShaderResources(CShader* pShader, const _char* pName, _uint iMeshIndex, TEXTURETYPE eType)
{
	if (pShader == nullptr || iMeshIndex >= m_iMeshesNum)
		RETURN_EFAIL;

	_uint	iMaterialIndex = m_vecMesh[iMeshIndex]->Get_MeshData()->iMaterialIndex;

	if (m_vecMaterial[iMaterialIndex].pMtrlTextures[eType] == nullptr)
		return S_OK;

	//return m_vecMaterial[iMaterialIndex].pMtrlTextures[eType]->Bind_ShaderResource(pShader, pName);
	RETURN_EFAIL;
}

HRESULT CModel_Inst::Ready_Meshes(const FModelData* ModelData, const string& strModelFilePath)
{
	m_iMeshesNum = ModelData->pMeshGroup->Get_NumMeshes();

	m_vecMesh.reserve(m_iMeshesNum);

	CModel::BONES vecBones;

	for (_uint i = 0; i < m_iMeshesNum; i++)
	{
		shared_ptr<CMeshComp> pMesh = CMeshComp::Create(m_pDevice, m_pContext);
		if (pMesh == nullptr)
			RETURN_EFAIL;

		pMesh->Load_Mesh(StrToWstr(strModelFilePath), i);
		m_vecMesh.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel_Inst::Ready_Materials(const FModelData* ModelData, const string& strModelFilePath)
{
	m_iMaterialsNum = ModelData->pMaterialGroup->Get_NumMaterials();

	for (_uint i = 0; i < m_iMaterialsNum; i++)
	{
		MATERIAL_DESC Material_Desc = {};

		for (_uint j = 1; j < (int)TEXTURETYPE_MAX; j++)
		{
			_char szDdrive[MAX_PATH] = "";
			_char szDirectory[MAX_PATH] = "";

			_splitpath_s(strModelFilePath.c_str(), szDdrive, MAX_PATH, szDirectory, MAX_PATH,
				nullptr, 0, nullptr, 0);

			string	szGetPath = WstrToStr(ModelData->pMaterialGroup->Get_MaterialDatas()[i]->strTexture[j]);
			if (!strcmp(szGetPath.c_str(), ""))
				continue;

			_char	szFileName[MAX_PATH] = "";
			_char	szExc[MAX_PATH] = "";

			_splitpath_s(szGetPath.c_str(), nullptr, 0, nullptr, 0,
				szFileName, MAX_PATH, szExc, MAX_PATH);

			_char	szTmp[MAX_PATH] = "";

			strcpy_s(szTmp, szDdrive);
			strcat_s(szTmp, szDirectory);
			strcat_s(szTmp, szFileName);
			strcat_s(szTmp, szExc);

			_tchar	szFullPath[MAX_PATH] = TEXT("");

			MultiByteToWideChar(CP_ACP, 0, szTmp, (int)strlen(szTmp), szFullPath, MAX_PATH);

			Material_Desc.pMtrlTextures[j] = CTexture_Data::Create(m_pDevice, m_pContext);
			if (Material_Desc.pMtrlTextures[j] == nullptr)
			{
				//RETURN_EFAIL;
				continue;
			}

		}
		m_vecMaterial.push_back(Material_Desc);
	}

	return S_OK;
}


void CModel_Inst::Free()
{
	__super::Free();

	//for (auto& iter1 : m_vecMaterial)
	//{
	//	for (auto& iter2 : iter1.pMtrlTexture)
	//		Safe_Release(iter2);
	//}

	//for (auto* iter : m_vecMesh)
	//	Safe_Release(iter);
	//m_vecMesh.clear();
}
