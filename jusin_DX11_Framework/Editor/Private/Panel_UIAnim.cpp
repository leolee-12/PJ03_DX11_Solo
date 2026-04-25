#include "Panel_UIAnim.h"
#include "UIEditorSession.h"

#include "EditInstance.h"

CPanel_UIAnim::CPanel_UIAnim()
	: CPanel_Base()
	, m_pSession(m_pEditInstance->Get_UISession())
{
	Safe_AddRef(m_pSession);
}

HRESULT CPanel_UIAnim::Initialize()
{
	m_strTitle = "UI_Anim";

	if (FAILED(__super::Initialize()))
		return E_FAIL;

	return S_OK;
}

void CPanel_UIAnim::Update(_float fTimeDelta)
{
}

HRESULT CPanel_UIAnim::Render()
{
	if (!Begin_Panel())
	{
		End_Panel();
		return S_OK;
	}

	ImGui::Text("Selected widget : %d", m_pSession->Get_SelectedWidget());
	ImGui::Text("Selected anim   : %d", m_pSession->Get_SelectedAnimation());
	ImGui::Text("Selected step   : %d", m_pSession->Get_SelectedStep());
	ImGui::Text("widgets=%zu  steps=%zu",
		m_pSession->Get_Doc().vWidgets.size(),
		m_pSession->Get_Doc().vSteps.size());
	ImGui::TextDisabled("[Phase D] animation/timeline editor will be moved here.");

	End_Panel();
	return S_OK;
}

CPanel_UIAnim* CPanel_UIAnim::Create()
{
	CPanel_UIAnim* pInstance = new CPanel_UIAnim();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create : CPanel_UIAnim");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPanel_UIAnim::Free()
{
	__super::Free();

	Safe_Release(m_pSession);
}
