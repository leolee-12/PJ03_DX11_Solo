#include "Object_Registry.h"
#include "GameInstance.h"
#include "EditInstance.h"

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
	Safe_AddRef(pClone);
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

	Safe_Release(m_pEditInstance);
	Safe_Release(m_pGameInstance);
}
