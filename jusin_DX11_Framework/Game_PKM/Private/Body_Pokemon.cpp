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

HRESULT CBody_Pokemon::Initialize(void* pArg)
{
	const BODY_POKEMON_DESC* pDesc = static_cast<const BODY_POKEMON_DESC*>(pArg);
	if (nullptr == pDesc)
		return E_FAIL;

	BODY_POKEMON_DESC Desc = *pDesc;

	if (0 == Desc.strShaderProtoTag)
		Desc.strShaderProtoTag = PROTO_COM_SHADER_POKEMON;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(m_RenderProfile.Build(m_pModelCom, Desc.pRenderRule)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Pokemon::Render()
{
	if (FAILED(Bind_ShaderResources_Common()))
		return E_FAIL;

	vector<CRenderProfile::MATERIAL_SLOT> Slots =
	{
		{ MATERIAL_TYPE::DIFFUSE, "g_TexDiff" },
		{ MATERIAL_TYPE::AMBIENT_OCCLUSION, "g_TexAO" },
		{ MATERIAL_TYPE::NORMALS, "g_TexNorm" },
		{ MATERIAL_TYPE::SHININESS, "g_TexRough" },
		{ MATERIAL_TYPE::LAYER_COLOR, "g_TexLycl" },
		{ MATERIAL_TYPE::LAYER_MASK, "g_TexMask" },
	};

	if (FAILED(m_RenderProfile.Bind_AndDraw(m_pShaderCom, Slots, "g_BoneMatrices")))
		return E_FAIL;

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