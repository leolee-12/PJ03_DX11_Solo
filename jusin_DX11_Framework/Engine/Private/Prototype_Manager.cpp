#include "Prototype_Manager.h"
#include "GameObject.h"
#include "Component.h"

CPrototype_Manager::CPrototype_Manager()
{
}

HRESULT CPrototype_Manager::Initialize(_uint iNumLevels)
{
	if (nullptr != m_pPrototypes)
		return E_FAIL;

	m_iNumLevels = iNumLevels;

	m_pPrototypes = new PROTOTYPES[iNumLevels];

	return S_OK;
}

HRESULT CPrototype_Manager::Add_Prototype(_uint iLevelIndex, WNameID strProtoTag, CBase* pPrototype)
{
	lock_guard<mutex> lock(m_Mutex);

	if (nullptr == pPrototype ||
		nullptr == m_pPrototypes ||
		iLevelIndex >= m_iNumLevels ||
		nullptr != Find_Prototype_NoLock(iLevelIndex, strProtoTag))
		return E_FAIL;

	m_pPrototypes[iLevelIndex].emplace(strProtoTag, pPrototype);

	return S_OK;
}

CBase* CPrototype_Manager::Clone_Prototype(PROTOTYPE eType, _uint iLevelIndex, WNameID strProtoTag, void* pArg)
{
	CBase* pPrototype = Find_Prototype_NoLock(iLevelIndex, strProtoTag);
	if (nullptr == pPrototype)
		return nullptr;

	CBase* pInstance = { nullptr };

	if (PROTOTYPE::GAMEOBJECT == eType)
 		pInstance = dynamic_cast<CGameObject*>(pPrototype)->Clone(pArg);
	else
		pInstance = dynamic_cast<CComponent*>(pPrototype)->Clone(pArg);

	if (nullptr == pInstance)
		return nullptr;

	return pInstance;
}

void CPrototype_Manager::Clear(_uint iLevelIndex)
{
	m_pPrototypes[iLevelIndex].for_each([](auto& pair) { Safe_Release(pair.second); });
	m_pPrototypes[iLevelIndex].clear();
}

CBase* CPrototype_Manager::Find_Prototype(_uint iLevelIndex, WNameID strProtoTag)
{
	lock_guard<mutex> lock(m_Mutex);

	auto pp = m_pPrototypes[iLevelIndex].find(strProtoTag);

	return pp ? *pp : nullptr;
}

CBase* CPrototype_Manager::Find_Prototype_NoLock(_uint iLevelIndex, WNameID strProtoTag)
{
	auto pp = m_pPrototypes[iLevelIndex].find(strProtoTag);

	return pp ? *pp : nullptr;
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
		m_pPrototypes[i].for_each([](auto& pair) { Safe_Release(pair.second); });
		m_pPrototypes[i].clear();
	}

	Safe_Delete_Array(m_pPrototypes);
}
