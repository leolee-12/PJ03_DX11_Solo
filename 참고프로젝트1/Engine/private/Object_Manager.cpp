#include "Object_Manager.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Layer.h"

CObject_Manager::CObject_Manager()
	: m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

CComponent* CObject_Manager::Get_Component(_uint iLayerLevelID, const _wstring& strLayerTag, _uint iIndex, const _wstring& strComponentTag)
{
	CLayer*		pLayer = Find_Layer(iLayerLevelID, strLayerTag);
	if(nullptr == pLayer )
		return nullptr;

	return pLayer->Get_Component(iIndex, strComponentTag);
}

HRESULT CObject_Manager::Initialize(_uint iNumLevels)
{
	m_iNumLevels = iNumLevels;

	m_pLayers = new map<const _wstring, CLayer*>[iNumLevels];

	return S_OK;
}


HRESULT CObject_Manager::Add_GameObject(_uint iPrototypeLevelID, const _wstring& strPrototypeTag, _uint iLayerLevelID, const _wstring& strLayerTag, void* pArg)
{
	/* 오베즉트 매니져에 추가해야할 사본 객체를 복제햏오낟. */
	CGameObject*		pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iPrototypeLevelID, strPrototypeTag, pArg));
	if (nullptr == pGameObject)
		return E_FAIL;

	/* 그 사본ㄴ객체를 넣어둘 레이어를 검색해본다. */
	CLayer*		pLayer = Find_Layer(iLayerLevelID, strLayerTag);

	/* 축하라려고했던 레이엉가 없었다라면 */
	if (nullptr == pLayer)
	{
		/* 새로만들어서 때려넣자. */
		pLayer = CLayer::Create();

		pLayer->Add_GameObject(pGameObject);

		m_pLayers[iLayerLevelID].emplace(strLayerTag, pLayer);
	}
	else /* 어 있네 */
		pLayer->Add_GameObject(pGameObject);

	return S_OK;
}

void CObject_Manager::Clear(_uint iLevelID)
{
	for (auto& Pair : m_pLayers[iLevelID])
		Safe_Release(Pair.second);

	m_pLayers[iLevelID].clear();
}

void CObject_Manager::Priority_Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
			Pair.second->Priority_Update(fTimeDelta);
	}
}

void CObject_Manager::Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
			Pair.second->Update(fTimeDelta);
	}
}

void CObject_Manager::Late_Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
			Pair.second->Late_Update(fTimeDelta);
	}
}

CLayer* CObject_Manager::Find_Layer(_uint iLevelID, const _wstring& strLayerTag)
{
	auto	iter = m_pLayers[iLevelID].find(strLayerTag);
	if (iter == m_pLayers[iLevelID].end())
		return nullptr;

	return iter->second;	
}

CObject_Manager* CObject_Manager::Create(_uint iNumLevels)
{
	CObject_Manager* pInstance = new CObject_Manager();

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX("Failed to Created : CObject_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CObject_Manager::Free()
{
	__super::Free();

	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
			Safe_Release(Pair.second);
		m_pLayers[i].clear();
	}
	Safe_Delete_Array(m_pLayers);

	m_pGameInstance->DestroyInstance();
}
