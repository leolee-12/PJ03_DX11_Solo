#include "Object_Manager.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Layer.h"

CObject_Manager::CObject_Manager()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

CComponent* CObject_Manager::Get_Component(_uint iLevelIndex, const WNameID strLayerTag, const WNameID strComponentTag, _uint iIndex)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);
	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Get_Component(strComponentTag, iIndex);
}

const list<CGameObject*>* CObject_Manager::Get_ObjectList(_uint iLevel, WNameID strLayerTag)
{
	CLayer* pLayer = Find_Layer(iLevel, strLayerTag);
	if (!pLayer) return nullptr;
	return &pLayer->Get_ObjectList();
}

vector<CGameObject*> CObject_Manager::Get_LevelObjects(_uint iLevel) const
{
	vector<CGameObject*> result;
	if (iLevel >= m_iNumLevels)
		return result;

	m_pLayers[iLevel].for_each(
		[&result](auto& pair)
		{
			const list<CGameObject*>& objects = pair.second->Get_ObjectList();

			for (auto pObj : objects)
				result.push_back(pObj);
		});

	return result;
}

HRESULT CObject_Manager::Initialize(_uint iNumLevels)
{
	if (nullptr != m_pLayers)
		return E_FAIL;

	m_iNumLevels = iNumLevels;

	m_pLayers = new LAYERS[iNumLevels];

	return S_OK;
}


HRESULT CObject_Manager::Add_GameObject(_uint iPrototypeLevelIndex, WNameID strProtoTag, _uint iLayerLevelIndex, WNameID strLayerTag, void* pArg)
{
	if (iLayerLevelIndex >= m_iNumLevels ||
		nullptr == m_pLayers)
		return E_FAIL;

	CGameObject* pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iPrototypeLevelIndex, strProtoTag, pArg));
	
	if (nullptr == pGameObject)
		return E_FAIL;

	CLayer* pLayer = Find_Layer(iLayerLevelIndex, strLayerTag);
	
	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		pLayer->Add_GameObject(pGameObject);
		m_pLayers[iLayerLevelIndex].emplace(strLayerTag, pLayer);
	}
	else
		pLayer->Add_GameObject(pGameObject);

	return S_OK;
}

HRESULT CObject_Manager::Add_GameObject_Ex(_uint iLayerLevel, WNameID strLayerTag, CGameObject* pObj)
{
	if (iLayerLevel >= m_iNumLevels ||
		nullptr == pObj)
		return E_FAIL;

	CLayer* pLayer = Find_Layer(iLayerLevel, strLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		pLayer->Add_GameObject(pObj);
		m_pLayers[iLayerLevel].emplace(strLayerTag, pLayer);
	}
	else
		pLayer->Add_GameObject(pObj);

	return S_OK;
}

void CObject_Manager::Priority_Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
		m_pLayers[i].for_each([fTimeDelta](auto& pair) { pair.second->Priority_Update(fTimeDelta); });
}

void CObject_Manager::Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
		m_pLayers[i].for_each([fTimeDelta](auto& pair) { pair.second->Update(fTimeDelta); });
}

void CObject_Manager::Late_Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
		m_pLayers[i].for_each([fTimeDelta](auto& pair) { pair.second->Late_Update(fTimeDelta); });
}

void CObject_Manager::Clear(_uint iLevelIndex)
{
	m_pLayers[iLevelIndex].for_each([](auto& pair) { Safe_Release(pair.second); });
	m_pLayers[iLevelIndex].clear();
}

CLayer* CObject_Manager::Find_Layer(_uint iLayerLevelIndex, WNameID strLayerTag)
{
	auto pp = m_pLayers[iLayerLevelIndex].find(strLayerTag);

	return pp ? *pp : nullptr;
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
		m_pLayers[i].for_each([](auto& pair) { Safe_Release(pair.second); });
		m_pLayers[i].clear();
	}

	Safe_Release(m_pGameInstance);
	Safe_Delete_Array(m_pLayers);
}
