#include "Prototype_Manager.h"

#include "GameObject.h"
#include "Component.h"

CPrototype_Manager::CPrototype_Manager()
{
}

HRESULT CPrototype_Manager::Initialize(_uint iNumLevels)
{
	m_iNumLevels = iNumLevels;

	m_pPrototypes = new /*map<const _wstring, CBase*>*/PROTOTYPES[iNumLevels];

	return S_OK;
}

HRESULT CPrototype_Manager::Add_Prototype(_uint iLevelID, const _wstring& strPrototypeTag, CBase* pPrototype)
{
	if (iLevelID >= m_iNumLevels || 
		nullptr != Find_Prototype(iLevelID, strPrototypeTag))
		return E_FAIL;

	m_pPrototypes[iLevelID].emplace(strPrototypeTag, pPrototype);

	return S_OK;
}

CBase* CPrototype_Manager::Clone_Prototype(PROTOTYPE eType, _uint iLevelID, const _wstring& strPrototypeTag, void* pArg)
{
	/* 원형의 종류가 두 타입이라 베이스로 저장. */
	CBase*		pPrototype = Find_Prototype(iLevelID, strPrototypeTag);
	if (nullptr == pPrototype)
		return nullptr;	

	/* 베이스로 클론호출이 불가하기때문에 호출ㄹ이 가능한 타입으로 형변환이 필요하다. */
	if(PROTOTYPE::GAMEOBJECT == eType)
	{
		return dynamic_cast<CGameObject*>(pPrototype)->Clone(pArg);
	}
	else
		return dynamic_cast<CComponent*>(pPrototype)->Clone(pArg);	
}

void CPrototype_Manager::Clear(_uint iLevelID)
{
	if (iLevelID >= m_iNumLevels)
		return;

	for (auto& Pair : m_pPrototypes[iLevelID])
		Safe_Release(Pair.second);

	m_pPrototypes[iLevelID].clear();
}

CBase* CPrototype_Manager::Find_Prototype(_uint iLevelID, const _wstring& strPrototypeTag)
{
	auto	iter = m_pPrototypes[iLevelID].find(strPrototypeTag);

	if (iter == m_pPrototypes[iLevelID].end())
		return nullptr;

	return iter->second;
}

CPrototype_Manager* CPrototype_Manager::Create(_uint iNumLevels)
{
	CPrototype_Manager* pInstance = new CPrototype_Manager();

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX("Failed to Created : CPrototype_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPrototype_Manager::Free()
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
