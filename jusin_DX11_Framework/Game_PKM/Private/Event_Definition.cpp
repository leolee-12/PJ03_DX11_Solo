#include "Event_Definition.h"

CEvent_Definition::CEvent_Definition()
{
}

HRESULT CEvent_Definition::Initialize(const _wstring& strSequenceID)
{
	if (true == strSequenceID.empty())
		return E_FAIL;

	m_strSequenceID = strSequenceID;
	m_Groups.clear();

	return S_OK;
}

void CEvent_Definition::Add_Step(const EVENT_STEP_DESC& tStep)
{
	EVENT_STEP_GROUP tGroup{};
	tGroup.eMode = EVENT_STEP_MODE::SEQUENTIAL;
	tGroup.Steps.push_back(tStep);

	m_Groups.push_back(tGroup);
}

void CEvent_Definition::Add_Group(const EVENT_STEP_GROUP& tGroup)
{
	m_Groups.push_back(tGroup);
}

CEvent_Definition* CEvent_Definition::Create(const _wstring& strSequenceID)
{
	CEvent_Definition* pInstance = new CEvent_Definition();

	if (FAILED(pInstance->Initialize(strSequenceID)))
	{
		MSG_BOX("Failed to Created : CEvent_Definition");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Definition::Free()
{
	m_Groups.clear();

	__super::Free();
}