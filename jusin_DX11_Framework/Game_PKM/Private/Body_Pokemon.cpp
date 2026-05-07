#include "Body_Pokemon.h"

#include "Model.h"
#include "Shader.h"

CBody_Pokemon::CBody_Pokemon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBody{ pDevice, pContext }
{
	m_strName = L"Body_Pokemon";
}

CBody_Pokemon::CBody_Pokemon(const CBody_Pokemon& Prototype)
	: CBody{ Prototype }
{
}

HRESULT CBody_Pokemon::Render()
{
	if (FAILED(Bind_ShaderResources_Common()))
		return E_FAIL;

	const size_t iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_TexDiff", i, MATERIAL_TYPE::DIFFUSE, 0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

CBody_Pokemon* CBody_Pokemon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Pokemon* pInstance = new CBody_Pokemon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBody_Pokemon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody_Pokemon::Clone(void* pArg)
{
	CBody_Pokemon* pInstance = new CBody_Pokemon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBody_Pokemon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Pokemon::Free()
{
	__super::Free();
}