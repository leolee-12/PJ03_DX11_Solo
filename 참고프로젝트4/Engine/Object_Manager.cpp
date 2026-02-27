#include "..\Public\Object_Manager.h"
#include "GameObject.h"
#include "Layer.h"

CObject_Manager::CObject_Manager()
{
}

HRESULT CObject_Manager::Initialize(_uint iNumLevels)
{
	if (nullptr != m_pLayers)
		RETURN_EFAIL;

	m_iNumLevels = iNumLevels;

	m_pLayers = new LAYERS[iNumLevels];

	return S_OK;
}

HRESULT CObject_Manager::Add_Prototype(const wstring & strPrototypeTag, shared_ptr<class CGameObject> pPrototype)
{
	if (nullptr == pPrototype || 
		nullptr != Find_Prototype(strPrototypeTag))
		RETURN_EFAIL;

	pPrototype->Set_PrototypeTag(strPrototypeTag);

	m_Prototypes.emplace(strPrototypeTag, pPrototype);

	return S_OK;
}

HRESULT CObject_Manager::Add_Object(_uint iLevelIndex, const LAYERTYPE& strLayerTag, shared_ptr<CGameObject> pObject)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);

	/* 아직 해당 이름을 가진 레이어가 없었다. */
	/* 이 이름을 가진 레이어에 최초로 추가하고 있는 상황이다. */
	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		if (nullptr == pLayer)
			RETURN_EFAIL;

		pLayer->Add_GameObject(pObject);

		m_pLayers[iLevelIndex].emplace(strLayerTag, pLayer);
	}
	/* 이미 이름을 가진 레이어가 있었어. */
	else
		pLayer->Add_GameObject(pObject);

	return S_OK;
}

shared_ptr<CGameObject> CObject_Manager::CloneObject(const wstring& strPrototypeTag, void* pArg)
{
	shared_ptr<CGameObject> pPrototype = Find_Prototype(strPrototypeTag);
	if (nullptr == pPrototype)
		return nullptr;

	shared_ptr<CGameObject> pGameObject = pPrototype->Clone(pArg);
	if(pGameObject != nullptr)
		pGameObject->Set_PrototypeTag(strPrototypeTag);

	return pGameObject;
}

shared_ptr<CGameObject> CObject_Manager::Get_Object(_uint iLevelIndex, const LAYERTYPE& strLayerTag, _uint iIndex)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);

	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Get_Object(iIndex);
}

shared_ptr<CComponent> CObject_Manager::Get_Component(_uint iLevelIndex, const LAYERTYPE& strLayerTag, const wstring& strComponentTag, _uint iIndex, const wstring& strPartTag)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);

	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Get_Component(strComponentTag, iIndex,strPartTag);
}

void CObject_Manager::Priority_Tick(_cref_time fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
		{
			Pair.second->Clear_DeadObjects();
		}
	}

	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
		{
			Pair.second->Priority_Tick(fTimeDelta);		
		}
	}
}

void CObject_Manager::Tick(_cref_time fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
			Pair.second->Tick(fTimeDelta);
	}
}

void CObject_Manager::Late_Tick(_cref_time fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
			Pair.second->Late_Tick(fTimeDelta);
	}
}

void CObject_Manager::Before_Render(_cref_time fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
			Pair.second->Before_Render(fTimeDelta);
	}
}

void CObject_Manager::Clear(_uint iLevelIndex)
{
	for (auto& Pair : m_pLayers[iLevelIndex])
		Safe_Release(Pair.second);

	m_pLayers[iLevelIndex].clear();
}

list<class shared_ptr<CGameObject>>* CObject_Manager::Get_ObjectList(_uint iLevelIndex, const LAYERTYPE& strLayerTag)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);

	if (pLayer == nullptr)
		return nullptr;

	return pLayer->Get_ObjectList();
}

shared_ptr<class CGameObject> CObject_Manager::Find_Prototype(const wstring & strPrototypeTag)
{
	auto	iter = m_Prototypes.find(strPrototypeTag);

	if (iter == m_Prototypes.end())
		return nullptr;

	return iter->second;
}

CLayer * CObject_Manager::Find_Layer(_uint iLevelIndex, const LAYERTYPE& strLayerTag)
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;

	auto	iter = m_pLayers[iLevelIndex].find(strLayerTag);
	if (iter == m_pLayers[iLevelIndex].end())
	{
		return nullptr;
	}

	return iter->second;	
}

CObject_Manager * CObject_Manager::Create(_uint iNumLevels)
{
	CObject_Manager*		pInstance = new CObject_Manager;

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX("Failed to Created : CObject_Manager");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CObject_Manager::Free()
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])		
			Safe_Release(Pair.second);

		m_pLayers[i].clear();		
	}

	Safe_Delete_Array(m_pLayers);


	//for (auto& Pair : m_Prototypes)
		//Safe_Release(Pair.second);

	m_Prototypes.clear();
}
