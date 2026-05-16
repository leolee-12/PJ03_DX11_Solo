#include "Interaction_Dialogue.h"

CInteraction_Dialogue::CInteraction_Dialogue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInteraction{ pDevice, pContext }
{
}

CInteraction_Dialogue::CInteraction_Dialogue(const CInteraction_Dialogue& Prototype)
	: CInteraction{ Prototype }
{
}

HRESULT CInteraction_Dialogue::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInteraction_Dialogue::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	const INTERACTION_DIALOGUE_DESC* pDesc = static_cast<const INTERACTION_DIALOGUE_DESC*>(pArg);
	m_strDialogueKey = pDesc->strDialogueKey;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_bool CInteraction_Dialogue::Supports(INTERACTION_EVENT eEvent) const
{
	return INTERACTION_EVENT::TALK == eEvent;
}

_bool CInteraction_Dialogue::CanInteract(const INTERACTION_CONTEXT& ctx) const
{
	if (!Supports(ctx.eEvent))
		return false;

	// TODO: 어댑터 도입 후
	// if (m_pGameInstance->Get_DialogueService()->Is_Playing())
	//     return false;

	return true;
}

void CInteraction_Dialogue::Execute(const INTERACTION_CONTEXT& ctx)
{
	// TODO: 어댑터 도입 후
	// m_pGameInstance->Get_DialogueService()->Start(m_strDialogueKey);
}

CInteraction_Dialogue* CInteraction_Dialogue::Create(ID3D11Device* pDevice, ID3D11DeviceContext*
	pContext)
{
	CInteraction_Dialogue* pInstance = new CInteraction_Dialogue(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CInteraction_Dialogue");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CInteraction_Dialogue::Clone(void* pArg)
{
	CInteraction_Dialogue* pInstance = new CInteraction_Dialogue(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CInteraction_Dialogue");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInteraction_Dialogue::Free()
{
	__super::Free();
}