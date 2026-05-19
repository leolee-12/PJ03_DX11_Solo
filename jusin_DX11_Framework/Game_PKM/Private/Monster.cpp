#include "Monster.h"
#include "GameInstance.h"
#include "RenderRule_Manager.h"

CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
	m_strName = { L"Monster_Default" };
}

CMonster::CMonster(const CMonster& Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CMonster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		auto pDesc = static_cast<MONSTER_DESC*>(pArg);
		m_iComponentLevel = pDesc->iComponentLevel;
		m_strShaderProtoTag = pDesc->strShaderProtoTag;
		m_strModelProtoTag = pDesc->strModelProtoTag;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	auto pDesc = static_cast<MONSTER_DESC*>(pArg);

	m_pRenderRule = pDesc ? pDesc->pRenderRule : nullptr;

	if (nullptr == m_pRenderRule && pDesc && nullptr != pDesc->pRenderMappingPath)
		m_pRenderRule =
		CRenderRule_Manager::GetInstance()->Find_OrLoadMappingRule(pDesc->pRenderMappingPath);

	if (nullptr == m_pRenderRule)
		m_pRenderRule = CRenderRule_Manager::GetInstance()->Find_Rule(RENDER_RULE_KEY::POKEMON_DEFAULT);

	if (FAILED(m_RenderProfile.Build(m_pModelCom, m_pRenderRule)))
		return E_FAIL;

	if (pDesc)
		m_pTransformCom->ScaleTo(pDesc->fScale, pDesc->fScale, pDesc->fScale);

	m_pModelCom->Set_AnimationIndex(m_iCurrAnim, true);

	m_pTransformCom->ScaleTo(2.f, 2.f, 2.f);
	m_pTransformCom->Rotation(XMConvertToRadians(-5.f), XMConvertToRadians(190.f), 0.f);
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.5f, -1.f, -2.8f, 1.f));
	return S_OK;
}

void CMonster::Priority_Update(_float fTimeDelta)
{

}

void CMonster::Update(_float fTimeDelta)
{
	// 0 : Idle
	// 19 : Sleep_Loop
	// 20 : Sleep_End

	if (m_pGameInstance->Key_Down(DIK_1))
	{
		m_iCurrAnim++;
		if (m_iCurrAnim >= m_pModelCom->Get_NumAnimations()) m_iCurrAnim = 0;
		m_pModelCom->Set_AnimationIndex(m_iCurrAnim, false, 0.2f);
	}

	if (true == m_pModelCom->Play_Animation(fTimeDelta))
		int a = 10; // 중단점용 임시 코드
}

void CMonster::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERID::NONBLEND, this);
}

HRESULT CMonster::Render()
{
	if (FAILED(Bind_ShaderResources()))
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

HRESULT CMonster::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(m_iComponentLevel, m_strShaderProtoTag,
		COM_SHADER, reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_iComponentLevel, m_strModelProtoTag,
		COM_MODEL, reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pTransformCom->Bind_ShaderResourceWIT(m_pShaderCom, "g_WITMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(D3DTS::PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFarZ", m_pGameInstance->Get_FarZPtr(), sizeof(_float))))
		return E_FAIL;

	return S_OK;
}


CMonster* CMonster::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonster* pInstance = new CMonster(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMonster");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMonster::Clone(void* pArg)
{
	CMonster* pInstance = new CMonster(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMonster");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
