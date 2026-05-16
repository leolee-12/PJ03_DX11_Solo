#include "Body_Human.h"

#include "Model.h"
#include "Shader.h"

CBody_Human::CBody_Human(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBody{ pDevice, pContext }
{
	m_strName = L"Body_Human";
}

CBody_Human::CBody_Human(const CBody_Human& Prototype)
	: CBody{ Prototype }
{
}

HRESULT CBody_Human::Initialize(void* pArg)
{
	const BODY_HUMAN_DESC* pDesc = static_cast<const BODY_HUMAN_DESC*>(pArg);
	if (nullptr == pDesc)
		return E_FAIL;

	BODY_HUMAN_DESC Desc = *pDesc;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(m_RenderProfile.Build(m_pModelCom, Desc.pRenderRule)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Human::Render()
{
	if (FAILED(Bind_ShaderResources_Common()))
		return E_FAIL;

	vector<CRenderProfile::MATERIAL_SLOT> Slots =
	{
			{ MATERIAL_TYPE::DIFFUSE, "g_TexDiff" },
			//{ MATERIAL_TYPE::AMBIENT_OCCLUSION, "g_TexAO" },
			//{ MATERIAL_TYPE::NORMALS, "g_TexNorm" },
			//{ MATERIAL_TYPE::SHININESS, "g_TexRough" },
			//{ MATERIAL_TYPE::LAYER_COLOR, "g_TexLycl" },
			//{ MATERIAL_TYPE::LAYER_MASK, "g_TexMask" },
	};

	if (FAILED(m_RenderProfile.Bind_AndDraw(m_pShaderCom, Slots, "g_BoneMatrices")))
		return E_FAIL;

	return S_OK;
}

CBody_Human* CBody_Human::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Human* pInstance = new CBody_Human(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBody_Human");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody_Human::Clone(void* pArg)
{
	CBody_Human* pInstance = new CBody_Human(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBody_Human");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Human::Free()
{
	__super::Free();
}