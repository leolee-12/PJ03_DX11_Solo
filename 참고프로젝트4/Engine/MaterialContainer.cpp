#include "MaterialContainer.h"

FMaterialData* FMaterialData::Create()
{
	ThisClass* pInstance = new ThisClass();

	if (nullptr == pInstance)
	{
		MSG_BOX("FMeshData Create Failed");
	}

	return pInstance;
}

void FMaterialData::Free()
{
}

CMaterialGroup* CMaterialGroup::Create()
{
	ThisClass* pInstance = new ThisClass();

	if (nullptr == pInstance)
	{
		MSG_BOX("CMaterialGroup Create Failed");
	}

	return pInstance;
}

void CMaterialGroup::Free()
{
	for (auto& Mat : vecMaterialDatas)
		Safe_Release(Mat);
	vecMaterialDatas.clear();
	mapMaterialDatas.clear();
}

FMaterialData* CMaterialGroup::Find_Material(const _uint iIndex)
{
	if (iIndex < 0 && iIndex >= vecMaterialDatas.size())
		return nullptr;

	return vecMaterialDatas[iIndex];
}

FMaterialData* CMaterialGroup::Find_Material(const wstring& strName)
{
	auto iter = mapMaterialDatas.find(strName);
	if (iter == mapMaterialDatas.end())
		return nullptr;

	return (*iter).second;
}

HRESULT CMaterialGroup::Add_Material(const wstring& strName, FMaterialData* pMatData)
{
	/*auto iter = mapMaterialDatas.find(strName);
	if (iter != mapMaterialDatas.end())
	{
		Safe_Release(pMatData);
		return E_FAIL;
	}*/

	mapMaterialDatas.emplace(strName, pMatData);
	vecMaterialDatas.push_back(pMatData);

	++m_iNumMaterials;

	return S_OK;
}
