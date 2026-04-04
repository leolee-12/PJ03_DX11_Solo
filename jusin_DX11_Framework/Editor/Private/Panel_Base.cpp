#include "Panel_Base.h"
#include "GameInstance.h"
#include "EditInstance.h"

CPanel_Base::CPanel_Base()
	: m_pGameInstance(CGameInstance::GetInstance())
	, m_pEditInstance(CEditInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pEditInstance);
}

HRESULT CPanel_Base::Initialize()
{
	m_pEditInstance->Register_Callback(m_strTitle,
		[this](const vector<CGameObject*>& sel)
		{
			m_pSelected = sel.empty() ? nullptr : sel[0];
		});

	return S_OK;
}

_bool CPanel_Base::Begin_Panel()
{
	// ImGui_Manager에서 Is_Opened() 체크 후 호출 : m_bOpen은 항상 true
    return ImGui::Begin(m_strTitle.c_str(), &m_bOpen, m_iWindowFlags);
	// 패널이 접혀있으면 false 반환
}

void CPanel_Base::End_Panel()
{
	ImGui::End();
}

void CPanel_Base::Free()
{
	__super::Free();

	m_pEditInstance->Unregister_Callback(m_strTitle);
	Safe_Release(m_pEditInstance);
	Safe_Release(m_pGameInstance);
}
