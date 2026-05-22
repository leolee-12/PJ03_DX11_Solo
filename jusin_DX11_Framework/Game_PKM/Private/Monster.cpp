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

		m_fIdleVariantBaseInterval =
			(pDesc->fIdleVariantBaseInterval > 0.f) ? pDesc->fIdleVariantBaseInterval : 4.f;
		m_fIdleVariantJitter = max(0.f, pDesc->fIdleVariantJitter);
	}

	if (PROTO_COM_SHADER_POKEMON == m_strShaderProtoTag)
		m_iOutlineMaskPass = 11;
	else if (PROTO_COM_SHADER_HUMAN == m_strShaderProtoTag)
		m_iOutlineMaskPass = 4;
	else if (PROTO_COM_SHADER_PLAYER_LGPE == m_strShaderProtoTag)
		m_iOutlineMaskPass = 4;
	else
		m_bUseOutline = false;

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

	Return_To_Idle();

	m_pTransformCom->ScaleTo(2.f, 2.f, 2.f);
	m_pTransformCom->Rotation(XMConvertToRadians(-5.f), XMConvertToRadians(190.f), 0.f);
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.5f, -1.f, -2.8f, 1.f));
	
	if (nullptr != pDesc && pDesc->bActivateOnCreate)
		Activate(pDesc->eInitialSpecialKind);

	return S_OK;
}

void CMonster::Priority_Update(_float fTimeDelta)
{

}

void CMonster::Update(_float fTimeDelta)
{
	const _bool bAnimFinished = m_pModelCom->Play_Animation(fTimeDelta);

	if (ANIM_KIND::IDLE != m_eCurrentAnimKind)
	{
		if (bAnimFinished)
			Return_To_Idle();

		return;
	}

	if (m_bActive)
	{
		m_fIdleVariantElapsed += fTimeDelta;

		if (m_fIdleVariantElapsed >= m_fNextIdleVariantTime)
			Play_RandomIdleVariant();
	}
}

void CMonster::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERID::NONBLEND, this);

	if (m_bUseOutline)
		m_pGameInstance->Add_RenderGroup(RENDERID::OUTLINEMASK, this);
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

HRESULT CMonster::Render_OutlineMask()
{
	if (0 == m_iOutlineMaskPass)
		return S_OK;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = static_cast<_uint>(m_pModelCom->Get_NumMeshes());
	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(m_iOutlineMaskPass)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

void CMonster::Activate(ANIM_KIND eInitialSpecialKind)
{
	m_bActive = true;
	Schedule_NextIdleVariant();
	m_fIdleVariantElapsed = 0.f;

	if (ANIM_KIND::END != eInitialSpecialKind)
		Play_SpecialAnim(eInitialSpecialKind);
}

void CMonster::Deactivate()
{
	m_bActive = false;
	m_fIdleVariantElapsed = 0.f;
	m_fNextIdleVariantTime = 0.f;
}

_bool CMonster::Play_SpecialAnim(ANIM_KIND eKind)
{
	if (ANIM_KIND::EVENT_1 != eKind &&
		ANIM_KIND::EVENT_2 != eKind &&
		ANIM_KIND::EVENT_3 != eKind)
		return false;

	if (!Is_CustomAnimDefined(eKind))
		return false;

	const _uint iIndex = BattleAnim::Find_AnimIndex(m_strModelProtoTag, eKind);
	m_pModelCom->Set_AnimationIndex(iIndex, false, 0.2f);

	m_eCurrentAnimKind = eKind;
	m_fIdleVariantElapsed = 0.f;
	return true;
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

_bool CMonster::Play_IdleVariant(ANIM_KIND eKind)
{
	if (ANIM_KIND::IDLE_1 != eKind &&
		ANIM_KIND::IDLE_2 != eKind &&
		ANIM_KIND::IDLE_3 != eKind)
		return false;

	if (!Is_CustomAnimDefined(eKind))
		return false;

	const _uint iIndex = BattleAnim::Find_AnimIndex(m_strModelProtoTag, eKind);
	m_pModelCom->Set_AnimationIndex(iIndex, false, 0.2f);

	m_eCurrentAnimKind = eKind;
	m_fIdleVariantElapsed = 0.f;
	return true;
}

_bool CMonster::Play_RandomIdleVariant()
{
	static const ANIM_KIND kPool[] =
	{
			ANIM_KIND::IDLE_1,
			ANIM_KIND::IDLE_2,
			ANIM_KIND::IDLE_3
	};

	ANIM_KIND eCandidates[_countof(kPool)] = {};
	_uint iCandidateCount = 0;

	for (ANIM_KIND eKind : kPool)
	{
		if (Is_CustomAnimDefined(eKind))
			eCandidates[iCandidateCount++] = eKind;
	}

	if (0 == iCandidateCount)
	{
		Schedule_NextIdleVariant();
		m_fIdleVariantElapsed = 0.f;
		return false;
	}

	const _uint iRoll = static_cast<_uint>(
		m_pGameInstance->Random(0.f, static_cast<_float>(iCandidateCount) - 0.0001f));
	const _uint iPick = (iRoll < iCandidateCount) ? iRoll : 0u;

	return Play_IdleVariant(eCandidates[iPick]);
}

void CMonster::Return_To_Idle()
{
	if (nullptr == m_pModelCom)
		return;

	const _uint iIdle = BattleAnim::Find_AnimIndex(m_strModelProtoTag, ANIM_KIND::IDLE);

	const _float fBlendDuration =
		(m_pModelCom->Get_CurrAnimIndex() < m_pModelCom->Get_NumAnimations()) ? 0.2f : 0.f;

	m_pModelCom->Set_AnimationIndex(iIdle, true, fBlendDuration);

	m_eCurrentAnimKind = ANIM_KIND::IDLE;
	m_fIdleVariantElapsed = 0.f;

	if (m_bActive)
		Schedule_NextIdleVariant();
}

_bool CMonster::Is_CustomAnimDefined(ANIM_KIND eKind) const
{
	return 0 != BattleAnim::Find_AnimIndex(m_strModelProtoTag, eKind);
}

void CMonster::Schedule_NextIdleVariant()
{
	const _float fJitter = (m_fIdleVariantJitter > 0.f)
		? m_pGameInstance->Random(-m_fIdleVariantJitter, m_fIdleVariantJitter)
		: 0.f;

	m_fNextIdleVariantTime = max(0.5f, m_fIdleVariantBaseInterval + fJitter);
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
