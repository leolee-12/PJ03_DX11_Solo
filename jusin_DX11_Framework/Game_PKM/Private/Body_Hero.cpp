#include "Body_Hero.h"
#include "GameInstance.h"

#include "Player.h"

CBody_Hero::CBody_Hero(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBody{ pDevice, pContext }
{
}

CBody_Hero::CBody_Hero(const CBody_Hero& Prototype)
	: CBody{ Prototype }
{
}

HRESULT CBody_Hero::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(m_RenderProfile.Build(m_pModelCom, nullptr)))
		return E_FAIL;

	Ready_DefaultVariant();

	return S_OK;
}

void CBody_Hero::Update(_float fTimeDelta)
{
	//if (m_pGameInstance->Key_Down(DIK_Z))
	//{
	//	m_iDummy++;
	//	if (m_iDummy > 77) m_iDummy = 0;
	//	m_pModelCom->Set_AnimationIndex(m_iDummy, false, 0.2f);
	//}

	__super::Update(fTimeDelta);


}

void CBody_Hero::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CBody_Hero::Render()
{
	if (FAILED(__super::Bind_ShaderResources_Common()))
		return E_FAIL;

	vector<CRenderProfile::MATERIAL_SLOT> Slots =
	{
			{ MATERIAL_TYPE::DIFFUSE, "g_TexDiff" },
			{ MATERIAL_TYPE::EMISSIVE, "g_TexEmit" },
			{ MATERIAL_TYPE::LAYER_COLOR, "g_TexLycl" },
	};

	if (FAILED(m_RenderProfile.Bind_AndDraw(m_pShaderCom, Slots, "g_BoneMatrices")))
		return E_FAIL;

	m_pGameInstance->Draw_Text(FONT_MALGUN, to_wstring(m_pModelCom->Get_CurrAnimIndex()).c_str(),
		_float2{ 10.f, 10.f });

	return S_OK;
}

HRESULT CBody_Hero::Render_Shadow()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Shadow_Transform(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Shadow_Transform(D3DTS::PROJ))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_Shadow_FarZ(m_pShaderCom)))
		return E_FAIL;

	size_t iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(3)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CBody_Hero::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pTransformCom->Bind_ShaderResourceCombinedWIT(m_pShaderCom, "g_WITMatrix", XMLoadFloat4x4(&m_CombinedWorldMatrix))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(D3DTS::PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFarZ", m_pGameInstance->Get_FarZPtr(), sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

void CBody_Hero::Ready_DefaultVariant()
{
	//enum MESH { BAG, BOTTOMS, CAP, R_EYE, L_EYE, SKIN, HAIR, SHOES, TOPS, END };

	m_RenderProfile.Set_Pass(ETOUI(CBody_Hero::R_EYE), 1);
	m_RenderProfile.Set_Pass(ETOUI(CBody_Hero::L_EYE), 1);

	m_RenderProfile.Set_Pass(ETOUI(CBody_Hero::SKIN), 2);
	m_RenderProfile.Set_Pass(ETOUI(CBody_Hero::HAIR), 2);
	m_RenderProfile.Set_Pass(ETOUI(CBody_Hero::SHOES), 2);
	m_RenderProfile.Set_Pass(ETOUI(CBody_Hero::TOPS), 2);
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
