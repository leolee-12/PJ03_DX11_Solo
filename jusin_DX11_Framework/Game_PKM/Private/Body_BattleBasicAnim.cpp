#include "Body_BattleBasicAnim.h"

#include "Model.h"
#include "Shader.h"

CBody_BattleBasicAnim::CBody_BattleBasicAnim(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBody_BattleBase{ pDevice, pContext }
{
	m_strName = L"Body_BattleBasicAnim";
}

CBody_BattleBasicAnim::CBody_BattleBasicAnim(const CBody_BattleBasicAnim& Prototype)
	: CBody_BattleBase{ Prototype }
{
}

HRESULT CBody_BattleBasicAnim::Render()
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

CBody_BattleBasicAnim * CBody_BattleBasicAnim::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_BattleBasicAnim* pInstance = new CBody_BattleBasicAnim(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBody_BattleBasicAnim");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody_BattleBasicAnim::Clone(void* pArg)
{
	CBody_BattleBasicAnim* pInstance = new CBody_BattleBasicAnim(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBody_BattleBasicAnim");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_BattleBasicAnim::Free()
{
	__super::Free();
}