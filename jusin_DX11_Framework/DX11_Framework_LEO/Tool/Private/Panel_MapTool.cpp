#include "Panel_MapTool.h"

CPanel_MapTool::CPanel_MapTool()
	: CPanel_Base()
{
}

HRESULT CPanel_MapTool::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	return S_OK;
}

void CPanel_MapTool::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CPanel_MapTool::Render()
{
	__super::Render();
}

CPanel_MapTool* CPanel_MapTool::Create(void* pArg)
{
	CPanel_MapTool* pInstance = new CPanel_MapTool();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Create : CPanel_MapTool");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPanel_MapTool::Free()
{
}
