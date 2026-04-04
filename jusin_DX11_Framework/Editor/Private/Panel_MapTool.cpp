#include "Panel_MapTool.h"

CPanel_MapTool::CPanel_MapTool()
	: CPanel_Base()
{
}

HRESULT CPanel_MapTool::Initialize()
{
	m_strTitle = "Map";

	if (FAILED(__super::Initialize()))
		return E_FAIL;

	return S_OK;
}

void CPanel_MapTool::Update(_float fTimeDelta)
{
}

HRESULT CPanel_MapTool::Render()
{
	return S_OK;
}

CPanel_MapTool* CPanel_MapTool::Create()
{
	CPanel_MapTool* pInstance = new CPanel_MapTool();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create : CPanel_MapTool");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPanel_MapTool::Free()
{
	__super::Free();
}
