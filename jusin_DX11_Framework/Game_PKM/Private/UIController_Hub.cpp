#include "UIController_Hub.h"
#include "UIController.h"

CUIController_Hub::CUIController_Hub()
{
}

HRESULT CUIController_Hub::Initialize()
{
	return S_OK;
}

HRESULT CUIController_Hub::Register(CUIController* pCtrl)
{/* 컨트롤러 등록(AddRef). 이미 등록된 포인터면 S_FALSE. */
	if (nullptr == pCtrl)
		return E_FAIL;

	/* 중복 등록 방지 */
	for (CUIController* p : m_Controllers)
	{
		if (p == pCtrl)
			return S_FALSE;
	}

	Safe_AddRef(pCtrl);
	m_Controllers.push_back(pCtrl);
	return S_OK;
}

void CUIController_Hub::Unregister(CUIController* pCtrl)
{/* 등록 해제(Safe_Release). 미등록 포인터면 무시. */
	if (nullptr == pCtrl)
		return;

	for (auto it = m_Controllers.begin(); it != m_Controllers.end(); ++it)
	{
		if (*it == pCtrl)
		{
			m_Controllers.erase(it);
			Safe_Release(pCtrl);
			return;
		}
	}
}

void CUIController_Hub::Update_All(_float fTimeDelta)
{	/* 모든 등록 컨트롤러에 Update 전파 (iterator 무효화 주의 - Unregister/Close_All) */
	for (CUIController* pCtrl : m_Controllers)
	{
		if (nullptr != pCtrl)
			pCtrl->Update(fTimeDelta);
	}
}

void CUIController_Hub::Close_All()
{	/* 레벨 종료/전환 시 일괄 정리용 */
	for (CUIController* pCtrl : m_Controllers)
		Safe_Release(pCtrl);
	m_Controllers.clear();
}

CUIController_Hub* CUIController_Hub::Create()
{
	CUIController_Hub* pInstance = new CUIController_Hub();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create : CUIController_Hub");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CUIController_Hub::Free()
{
	__super::Free();

	Close_All();
}