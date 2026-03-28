#include "Object_Registry.h"
#include "GameInstance.h"
#include "EditInstance.h"

HRESULT CObject_Registry::Initialize()
{
	m_pGameInstance = CGameInstance::GetInstance();
	m_pEditInstance = CEditInstance::GetInstance();

	return S_OK;
}

void CObject_Registry::Register_Object(_uint iProtoLevel, WNameID strProtoTag, _uint iLayerLevel, WNameID strLayerTag, void* pArg)
{
	CGameObject* pObj = dynamic_cast<CGameObject*>(
		m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iProtoLevel, strProtoTag, pArg));
	if (!pObj) return;

	m_pGameInstance->Add_GameObject_Ex(iLayerLevel, strLayerTag, pObj);	// 레이어에 등록
	m_EditorObjects.push_back(pObj);	// 에디터 트래킹
}

void CObject_Registry::Unregister_Object(CGameObject* pObj)
{
	auto iter = find(m_EditorObjects.begin(), m_EditorObjects.end(), pObj);

	if (iter != m_EditorObjects.end())
		m_EditorObjects.erase(iter);

	m_pEditInstance->Deselect(pObj);	// 선택 해제

	pObj->Set_Dead();
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

	m_Selected.clear();

	for(auto& pObj : m_EditorObjects)
		Safe_Release(pObj);

	m_EditorObjects.clear();
}
