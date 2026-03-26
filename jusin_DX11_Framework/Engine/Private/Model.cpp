#include "Model.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath)
	: CComponent{ pDevice, pContext }
	, m_strModelFilePath{ pModelFilePath }
{
}

CModel::CModel(const CModel& Prototype)
	: CComponent{ Prototype }
	, m_strModelFilePath{ Prototype.m_strModelFilePath }
{
}

HRESULT CModel::Initialize_Prototype()
{
	_uint iFlag = { aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast };

	m_pAIScene = m_Importer.ReadFile(m_strModelFilePath.c_str(), iFlag);
	if (nullptr == m_pAIScene)
		return E_FAIL;




	return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
	return S_OK;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath)
{
	CModel* pInstance = new CModel(pDevice, pContext, pModelFilePath);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CModel::Clone(void* pArg)
{
	CModel* pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	__super::Free();


}
