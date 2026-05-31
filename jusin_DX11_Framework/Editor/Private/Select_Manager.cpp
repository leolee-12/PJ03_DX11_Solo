#include "Select_Manager.h"

HRESULT CSelect_Manager::Initialize()
{
	return S_OK;
}

void CSelect_Manager::Select(CGameObject* pObj, bool bMultiSelect)
{
	if (!pObj) return;

	if (!bMultiSelect)
	{
		m_Selected.clear();
		m_Selected.push_back(pObj);
	}
	else
	{   // 탐색
		auto iter = find(m_Selected.begin(), m_Selected.end(), pObj);

		if (iter != m_Selected.end())
			m_Selected.erase(iter); // 이미 선택된 객체면 선택 해제
		else
			m_Selected.push_back(pObj); // 없으면 추가
	}

	Notify();
}

void CSelect_Manager::Deselect(CGameObject* pObj)
{
	auto iter = find(m_Selected.begin(), m_Selected.end(), pObj);

	if (iter == m_Selected.end()) return;   // 없으면 return

	m_Selected.erase(iter); // 있으면 선택 해제 후 Notify
	Notify();
}

void CSelect_Manager::Clear()
{
	if (m_Selected.empty()) return; // 비어있으면 return (불필요한 콜백 방지)

	m_Selected.clear();
	Notify();
}

CGameObject* CSelect_Manager::Get_Primary() const
{
	return m_Selected.empty() ? nullptr : m_Selected.front();
}

bool CSelect_Manager::Is_Selected(CGameObject* pObj) const
{
	return find(m_Selected.cbegin(), m_Selected.cend(), pObj) != m_Selected.cend();
	// cbegin, cend는 const_iterator 반환 : 가리키는 대상 수정 불가
}

void CSelect_Manager::Register_Callback(const _string& strKey, SelectionChangedCB cb)
{
	m_Callbacks.insert_or_assign(strKey, move(cb));
	// insert_or_assign : strKey가 이미 있으면 cb로 덮어쓰기, 없으면 새로 추가 (중복 방지) - move로 cb 전달하여 불필요한 복사 방지
	// * operator[] : strKey가 이미 있으면 cb로 덮어쓰기, 없으면 새로 추가 (중복 방지) - value_type 복사 발생
	// * insert : strKey 이미 존재하면 삽입 실패 (반환값으로 성공 여부 알 수 있음)
	// * emplace : strKey 이미 존재하면 무시
}

void CSelect_Manager::Unregister_Callback(const _string& strKey)
{
	m_Callbacks.erase(strKey);
	// erase : strKey가 있으면 삭제, 없으면 무시하고 0 반환 (find 불필요)
}

void CSelect_Manager::Notify()
{
	for (auto& [key, cb] : m_Callbacks)
		cb(m_Selected);
}

CSelect_Manager* CSelect_Manager::Create()
{
	CSelect_Manager* pInstance = new CSelect_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CSelect_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSelect_Manager::Free()
{
	__super::Free();

	// 객체의 선택 여부만 관리, 소유권은 없으므로 해제X
	m_Selected.clear();
	m_Callbacks.clear();
}
