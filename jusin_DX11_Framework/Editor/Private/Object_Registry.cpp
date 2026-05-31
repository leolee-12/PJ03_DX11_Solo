#include "Object_Registry.h"
#include "GameInstance.h"
#include "EditInstance.h"

#include "MapObject.h"
#include "ForkLift.h"
#include "Monster.h"

CObject_Registry::CObject_Registry()
	: m_pGameInstance(CGameInstance::GetInstance())
	, m_pEditInstance(CEditInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pEditInstance);
}

HRESULT CObject_Registry::Initialize()
{
	return S_OK;
}

void CObject_Registry::Register_Object(_uint iProtoLevel, WNameID strProtoTag, _uint iLayerLevel, WNameID strLayerTag, void* pArg)
{
	CGameObject* pObj = dynamic_cast<CGameObject*>(
		m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iProtoLevel, strProtoTag, pArg));
	if (nullptr == pObj) return;

	pObj->Set_Name(Make_UniqueName(StoW(pObj->Get_TypeName())));
	m_pGameInstance->Add_GameObject_Ex(iLayerLevel, strLayerTag, pObj);	// 레이어에 등록
	m_Records.push_back({ iProtoLevel, strProtoTag, iLayerLevel, strLayerTag, pObj });
	m_EditorObjects.push_back(pObj);	// 에디터 트래킹

	EDITOR_OBJECT_ENTRY tEntry{};
	if (TryBuildEditorEntry(pObj, &tEntry))
		m_EditorEntries.push_back(tEntry);

	Safe_AddRef(pObj); // Editor 참조
}

void CObject_Registry::Unregister_Object(CGameObject* pObj)
{
	// ← 추가
	auto rIter = find_if(m_Records.begin(), m_Records.end(),
		[pObj](const OBJ_RECORD& record) { return record.pObj == pObj; });
	if (rIter != m_Records.end())
		m_Records.erase(rIter);

	auto iter = find(m_EditorObjects.begin(), m_EditorObjects.end(), pObj);
	if (iter != m_EditorObjects.end())
		m_EditorObjects.erase(iter);

	auto entryIter = find_if(m_EditorEntries.begin(), m_EditorEntries.end(),
		[pObj](const EDITOR_OBJECT_ENTRY& tEntry) { return tEntry.pObj == pObj; });
	if (entryIter != m_EditorEntries.end())
		m_EditorEntries.erase(entryIter);

	pObj->Set_Dead(); // DEAD -> Layer에서 제거
	m_pEditInstance->Deselect(pObj); // 선택 해제
}

void CObject_Registry::Clone_Object(CGameObject* pObj)
{
	auto iter = find_if(m_Records.begin(), m_Records.end(),
		[pObj](const OBJ_RECORD& record) { return record.pObj == pObj; });
	if (iter == m_Records.end()) return;

	CGameObject* pClone = dynamic_cast<CGameObject*>(
		m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iter->iProtoLevel, iter->strProtoTag, nullptr));
	if (!pClone) return;

	pClone->Set_Name(Make_UniqueName(pObj->Get_Name()));

	m_pGameInstance->Add_GameObject_Ex(iter->iLayerLevel, iter->strLayerTag, pClone);
	m_Records.push_back({ iter->iProtoLevel, iter->strProtoTag, iter->iLayerLevel, iter->strLayerTag, pClone });
	m_EditorObjects.push_back(pClone);

	EDITOR_OBJECT_ENTRY tEntry{};
	if (TryBuildEditorEntry(pClone, &tEntry))
		m_EditorEntries.push_back(tEntry);

	Safe_AddRef(pClone);
}

void CObject_Registry::Sync_LevelObjects(_uint iLevel)
{
	vector<CGameObject*> vecObjects = m_pGameInstance->Get_LevelObjects(iLevel);

	for (CGameObject* pObj : vecObjects)
	{
		if (nullptr == pObj)
			continue;

		if (Contains_EditorObject(pObj))
			continue;

		m_EditorObjects.push_back(pObj);
		Safe_AddRef(pObj);

		EDITOR_OBJECT_ENTRY tEntry{};
		if (TryBuildEditorEntry(pObj, &tEntry))
			m_EditorEntries.push_back(tEntry);
	}
}

_wstring CObject_Registry::Make_UniqueName(const _wstring& wStrBaseName) const
{
	for (_int i = 1; ; ++i)
	{
		_wstring candidate = wStrBaseName + L"_" + to_wstring(i);
		_bool bDuplicate = false;

		for (auto pObj : m_EditorObjects)
		{
			if (pObj->Get_Name() == candidate)
			{
				bDuplicate = true;
				break;
			}
		}
		if (!bDuplicate) return candidate;
	}
}

_bool CObject_Registry::TryBuildEditorEntry(CGameObject* pObj, EDITOR_OBJECT_ENTRY* pOutEntry) const
{
	if (nullptr == pObj || nullptr == pOutEntry)
		return false;

	*pOutEntry = {};

	pOutEntry->pObj = pObj;

	if (auto pMap = dynamic_cast<Game_PKM::CMapObject*>(pObj))
	{
		pOutEntry->pModel = pMap->Get_Model();
		pOutEntry->bPickable = (pOutEntry->pModel != nullptr);
		pOutEntry->bSelectable = false;
		pOutEntry->bPlacementSurface = (pOutEntry->pModel != nullptr);
	}
	else if (auto pForkLift = dynamic_cast<Game_PKM::CForkLift*>(pObj))
	{
		pOutEntry->pModel = pForkLift->Get_Model();
		pOutEntry->bPickable = (pOutEntry->pModel != nullptr);
		pOutEntry->bSelectable = (pOutEntry->pModel != nullptr);
		pOutEntry->bPlacementSurface = false;
	}
	else if (auto pMonster = dynamic_cast<Game_PKM::CMonster*>(pObj))
	{
		pOutEntry->pModel = pMonster->Get_Model();
		pOutEntry->bPickable = (pOutEntry->pModel != nullptr);
		pOutEntry->bSelectable = (pOutEntry->pModel != nullptr);
		pOutEntry->bPlacementSurface = false;
	}
	else
	{
		return false;
	}

	return pOutEntry->pModel != nullptr;
}

_bool CObject_Registry::Contains_EditorObject(CGameObject* pObj) const
{
	return find(m_EditorObjects.begin(), m_EditorObjects.end(), pObj) != m_EditorObjects.end();
}

CObject_Registry* CObject_Registry::Create()
{
	CObject_Registry* pInstance = new CObject_Registry();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CObject_Registry");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CObject_Registry::Free()
{
	__super::Free();

	m_Records.clear();

	for(auto& pObj : m_EditorObjects)
		Safe_Release(pObj);

	m_EditorObjects.clear();
	m_EditorEntries.clear();

	Safe_Release(m_pEditInstance);
	Safe_Release(m_pGameInstance);
}
