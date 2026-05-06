#include "Body_Hero.h"
#include "GameInstance.h"

#include "Player.h"

CBody_Hero::CBody_Hero(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBody{ pDevice, pContext }
{
}

CBody_Hero::CBody_Hero(const CBody_Hero& Prototype)
	: CBody{ Prototype }
	, m_RenderTable{ Prototype.m_RenderTable }
{
}

HRESULT CBody_Hero::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Ready_DefaultVariant();

	return S_OK;
}

HRESULT CBody_Hero::Render()
{
	if (FAILED(__super::Bind_ShaderResources_Common()))
		return E_FAIL;

	size_t iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		_uint matIdx = m_pModelCom->Get_MeshMaterialIndex(i);

		m_pModelCom->Bind_Material(m_pShaderCom, "g_TexDiff", i, MATERIAL_TYPE::DIFFUSE,
			m_RenderTable.variants[matIdx][ETOUI(MATERIAL_TYPE::DIFFUSE)]);
		m_pModelCom->Bind_Material(m_pShaderCom, "g_TexEmit", i, MATERIAL_TYPE::EMISSIVE,
			m_RenderTable.variants[matIdx][ETOUI(MATERIAL_TYPE::EMISSIVE)]);
		m_pModelCom->Bind_Material(m_pShaderCom, "g_TexLycl", i, MATERIAL_TYPE::LAYER_COLOR,
			m_RenderTable.variants[matIdx][ETOUI(MATERIAL_TYPE::LAYER_COLOR)]);

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(m_RenderTable.passes[matIdx])))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	m_pGameInstance->Draw_Text(FONT_MALGUN, to_wstring(m_pModelCom->Get_CurrAnimIndex()).c_str(), _float2{ 10.f, 10.f });

	return S_OK;
}

void CBody_Hero::Ready_DefaultVariant()
{
	//enum MESH { BAG, BOTTOMS, CAP, R_EYE, L_EYE, SKIN, HAIR, SHOES, TOPS, END };

	m_RenderTable.Ready_RenderTable(ETOUI(CBody_Hero::END));	// 0 ÃÊ±âÈ­
	m_RenderTable.passes[ETOUI(CBody_Hero::R_EYE)] = m_RenderTable.passes[ETOUI(CBody_Hero::L_EYE)] = 1;
	m_RenderTable.passes[ETOUI(CBody_Hero::SKIN)] = m_RenderTable.passes[ETOUI(CBody_Hero::HAIR)]
		= m_RenderTable.passes[ETOUI(CBody_Hero::SHOES)] = m_RenderTable.passes[ETOUI(CBody_Hero::TOPS)] = 1;
}


CBody_Hero* CBody_Hero::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Hero* pInstance = new CBody_Hero(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBody_Hero");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody_Hero::Clone(void* pArg)
{
	CBody_Hero* pInstance = new CBody_Hero(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBody_Hero");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Hero::Free()
{
	__super::Free();
}
