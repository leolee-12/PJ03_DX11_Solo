#include "..\Public\Component_Manager.h"

#include "Component.h"

CComponent_Manager::CComponent_Manager()
{

}

HRESULT CComponent_Manager::Initialize(_uint iNumLevels)
{
	m_iNumLevels = iNumLevels;

	m_pPrototypes = new map<const _wstring, class CComponent*>[iNumLevels];

	return S_OK;
}

HRESULT CComponent_Manager::Add_Prototype(_uint iLevelIndex, const _wstring & strPrototypeTag, CComponent * pPrototype)
{
	if (iLevelIndex >= m_iNumLevels ||
		nullptr != Find_Prototype(iLevelIndex, strPrototypeTag))
	{
			wstring Message{};
		Message = TEXT("Failed to Created PrototypeKey :  \n : ") + wstring(strPrototypeTag);
		MessageBox(nullptr, Message.c_str(), L"System Message", MB_OK);
			return E_FAIL;
	}

	m_pPrototypes[iLevelIndex].emplace(strPrototypeTag, pPrototype);

	return S_OK;
}

CComponent * CComponent_Manager::Clone_Component(_uint iLevelIndex, const _wstring & strPrototypeTag, void * pArg)
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;

	CComponent*		pPrototype = Find_Prototype(iLevelIndex, strPrototypeTag);
	if (nullptr == pPrototype)
		return nullptr;

	CComponent*		pComponent = pPrototype->Clone(pArg);
	if (nullptr == pComponent)
		return nullptr;

	return pComponent;
}

HRESULT CComponent_Manager::Clear_Resources(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return E_FAIL;

	for (auto& Pair : m_pPrototypes[iLevelIndex])
		Pair.second->m_bDead = true;
		//Safe_Release(Pair.second);

	m_pPrototypes[iLevelIndex].clear();

	return S_OK;
}

vector<const _wstring*>* CComponent_Manager::Find_Prototype_Include_Key(_uint iLevelIndex, const _wstring& strIncludeTag)
{
	m_pIncludeKeys.clear();

	for (const auto& pair : m_pPrototypes[iLevelIndex]) {
		if (pair.first.find(strIncludeTag) != wstring::npos) {
			m_pIncludeKeys.emplace_back(&pair.first);
		}
	}

	if (m_pIncludeKeys.empty())
		return nullptr;

	return &m_pIncludeKeys;
}

CComponent * CComponent_Manager::Find_Prototype(_uint iLevelIndex, const _wstring & strPrototypeTag)
{
	auto	iter = m_pPrototypes[iLevelIndex].find(strPrototypeTag);

	if (iter == m_pPrototypes[iLevelIndex].end())
		return nullptr;

	return iter->second;	
}

CComponent_Manager * CComponent_Manager::Create(_uint iNumLevels)
{
	CComponent_Manager*		pInstance = new CComponent_Manager();

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX(TEXT("Failed to Created : CComponent_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CComponent_Manager::Free()
{
	__super::Free();

	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pPrototypes[i])
			Safe_Release(Pair.second);

		m_pPrototypes[i].clear();
	}

	Safe_Delete_Array(m_pPrototypes);
}


