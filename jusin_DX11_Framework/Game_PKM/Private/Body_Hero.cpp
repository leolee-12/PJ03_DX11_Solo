#include "Body_Hero.h"
#include "GameInstance.h"

#include "Player.h"

CBody_Hero::CBody_Hero(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{

}

CBody_Hero::CBody_Hero(const CBody_Hero& Prototype)
	: CPartObject{ Prototype }
	, m_RenderTable{ Prototype.m_RenderTable }
{

}

const _float4x4* CBody_Hero::Get_BoneMatrixPtr(const _char* pBoneName) const
{
	return m_pModelCom->Get_BoneMatrixPtr(pBoneName);
}

void CBody_Hero::Set_Anim(_uint iAnimIdx, _bool isLoop, _float fBlendDuration)
{
	m_pModelCom->Set_AnimationIndex(iAnimIdx, isLoop, fBlendDuration);
}

const _float3& CBody_Hero::Get_RootMotionDelta() const
{
	return m_pModelCom->Get_RootMotionDelta();
}

HRESULT CBody_Hero::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBody_Hero::Initialize(void* pArg)
{
	auto pDesc = static_cast<BODY_HERO_DESC*>(pArg);

	m_pParentState = pDesc->pParentState;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->ScaleTo(0.4f, 0.4f, 0.4f);

	m_pModelCom->Set_AnimationIndex(0, true);
	m_pModelCom->Set_RootMotionBoneIndex(3);
	m_pModelCom->Set_EnableRootMotion(true);

	Ready_DefaultVariant();


	return S_OK;
}

void CBody_Hero::Priority_Update(_float fTimeDelta)
{

}

void CBody_Hero::Update(_float fTimeDelta)
{
	//if (m_pGameInstance->Key_Down(DIK_Z))
	//{
	//	m_iDummy++;
	//	if (m_iDummy > 77) m_iDummy = 0;
	//	m_pModelCom->Set_AnimationIndex(m_iDummy, false, 0.2f);
	//}

	if (true == m_pModelCom->Play_Animation(fTimeDelta))
		int a = 10;

}

void CBody_Hero::Late_Update(_float fTimeDelta)
{
	__super::Compute_CombinedWorldMatrix(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

	m_pGameInstance->Add_RenderGroup(RENDERID::NONBLEND, this);
}

HRESULT CBody_Hero::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	size_t iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		_uint matIdx = m_pModelCom->Get_MeshMaterialIndex(i);

		m_pModelCom->Bind_Material(m_pShaderCom, "g_TexDiff", i, MATERIAL_TYPE::DIFFUSE, m_RenderTable.variants[matIdx][ETOUI(MATERIAL_TYPE::DIFFUSE)]);
		m_pModelCom->Bind_Material(m_pShaderCom, "g_TexSpec", i, MATERIAL_TYPE::SPECULAR, m_RenderTable.variants[matIdx][ETOUI(MATERIAL_TYPE::SPECULAR)]);
		m_pModelCom->Bind_Material(m_pShaderCom, "g_TexAmbt_R", i, MATERIAL_TYPE::AMBIENT, 0);
		m_pModelCom->Bind_Material(m_pShaderCom, "g_TexAmbt_G", i, MATERIAL_TYPE::AMBIENT, 1);
		m_pModelCom->Bind_Material(m_pShaderCom, "g_TexAmbt_B", i, MATERIAL_TYPE::AMBIENT, 2);
		m_pModelCom->Bind_Material(m_pShaderCom, "g_TexEmit", i, MATERIAL_TYPE::EMISSIVE, m_RenderTable.variants[matIdx][ETOUI(MATERIAL_TYPE::EMISSIVE)]);
		m_pModelCom->Bind_Material(m_pShaderCom, "g_TexLycl", i, MATERIAL_TYPE::LAYER_COLOR, m_RenderTable.variants[matIdx][ETOUI(MATERIAL_TYPE::LAYER_COLOR)]);

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(m_RenderTable.passes[matIdx])))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	m_pGameInstance->Draw_Text(FONT_MALGUN, to_wstring(m_iDummy).c_str(), _float2(10.f, 10.f));

	return S_OK;
}

HRESULT CBody_Hero::Ready_Components()
{

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_SHADER_PLAYER_LGPE,
		COM_SHADER, reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_MODEL_HERO,
		COM_MODEL, reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;




	return S_OK;
}

HRESULT CBody_Hero::Bind_ShaderResources()
{

	//if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
	//    return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pTransformCom->Bind_ShaderResourceCombinedWIT(m_pShaderCom, "g_WITMatrix", XMLoadFloat4x4(&m_CombinedWorldMatrix))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(D3DTS::PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPos", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiff", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbt", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpec", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;

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

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
