#include "Body_BattleHeroLGPE.h"

#include "Model.h"
#include "Shader.h"

CBody_BattleHeroLGPE::CBody_BattleHeroLGPE(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBody{ pDevice, pContext }
{
	m_strName = L"Body_BattleHeroLGPE";
}

CBody_BattleHeroLGPE::CBody_BattleHeroLGPE(const CBody_BattleHeroLGPE& Prototype)
	: CBody{ Prototype }
	, m_RenderTable{ Prototype.m_RenderTable }
{
}

void CBody_BattleHeroLGPE::Set_Variant(unsigned int iMatIdx, MATERIAL_TYPE eType, unsigned int iMatNum)
{
	m_RenderTable.variants[iMatIdx][ETOUI(eType)] = iMatNum;
}

void CBody_BattleHeroLGPE::Set_Pass(unsigned int iMatIdx, unsigned int iPassIdx)
{
	m_RenderTable.passes[iMatIdx] = iPassIdx;
}

HRESULT CBody_BattleHeroLGPE::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Ready_DefaultVariant();

	return S_OK;
}

HRESULT CBody_BattleHeroLGPE::Render()
{
	if (FAILED(Bind_ShaderResources_Common()))
		return E_FAIL;

	const size_t iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		const _uint iMatIdx = m_pModelCom->Get_MeshMaterialIndex(i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_TexDiff", i, MATERIAL_TYPE::DIFFUSE,
			m_RenderTable.variants[iMatIdx][ETOUI(MATERIAL_TYPE::DIFFUSE)])))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_TexEmit", i, MATERIAL_TYPE::EMISSIVE,
			m_RenderTable.variants[iMatIdx][ETOUI(MATERIAL_TYPE::EMISSIVE)])))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_TexLycl", i,
			MATERIAL_TYPE::LAYER_COLOR,
			m_RenderTable.variants[iMatIdx][ETOUI(MATERIAL_TYPE::LAYER_COLOR)])))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(m_RenderTable.passes[iMatIdx])))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

void CBody_BattleHeroLGPE::Ready_DefaultVariant()
{
	m_RenderTable.Ready_RenderTable(ETOUI(CBody_BattleHeroLGPE::END));

	m_RenderTable.passes[ETOUI(CBody_BattleHeroLGPE::R_EYE)] = 1;
	m_RenderTable.passes[ETOUI(CBody_BattleHeroLGPE::L_EYE)] = 1;
	m_RenderTable.passes[ETOUI(CBody_BattleHeroLGPE::SKIN)] = 1;
	m_RenderTable.passes[ETOUI(CBody_BattleHeroLGPE::HAIR)] = 1;
	m_RenderTable.passes[ETOUI(CBody_BattleHeroLGPE::SHOES)] = 1;
	m_RenderTable.passes[ETOUI(CBody_BattleHeroLGPE::TOPS)] = 1;
}

CBody_BattleHeroLGPE* CBody_BattleHeroLGPE::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_BattleHeroLGPE* pInstance = new CBody_BattleHeroLGPE(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBody_BattleHeroLGPE");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody_BattleHeroLGPE::Clone(void* pArg)
{
	CBody_BattleHeroLGPE* pInstance = new CBody_BattleHeroLGPE(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBody_BattleHeroLGPE");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_BattleHeroLGPE::Free()
{
	__super::Free();
}