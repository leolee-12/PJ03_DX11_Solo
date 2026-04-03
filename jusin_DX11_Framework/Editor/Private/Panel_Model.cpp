#include "Panel_Model.h"
#include "GameInstance.h"
#include "EditInstance.h"

CPanel_Model::CPanel_Model()
	: CPanel_Base()
{
}

HRESULT CPanel_Model::Initialize()
{
	m_strTitle = "Model";

	if (FAILED(__super::Initialize()))
		return E_FAIL;

	return S_OK;
}

void CPanel_Model::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

HRESULT CPanel_Model::Render()
{
    if (!Begin_Panel()) { End_Panel(); return S_OK; }

    End_Panel();
    return S_OK;
}

CPanel_Model* CPanel_Model::Create()
{
	CPanel_Model* pInstance = new CPanel_Model();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create : CPanel_Model");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPanel_Model::Free()
{
	__super::Free();
}
