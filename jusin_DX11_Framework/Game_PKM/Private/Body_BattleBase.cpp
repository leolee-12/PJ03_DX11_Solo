#include "Body_BattleBase.h"

#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"

CBody_BattleBase::CBody_BattleBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CBody_BattleBase::CBody_BattleBase(const CBody_BattleBase& Prototype)
	: CPartObject{ Prototype }
{
}

const _float4x4* CBody_BattleBase::Get_BoneMatrixPtr(const _char* pBoneName) const
{
	return m_pModelCom->Get_BoneMatrixPtr(pBoneName);
}

const _float3& CBody_BattleBase::Get_RootMotionDelta() const
{
	return m_pModelCom->Get_RootMotionDelta();
}

void CBody_BattleBase::Set_Anim(_uint iAnimIdx, _bool isLoop, _float fBlendDuration)
{
	if (nullptr != m_pModelCom)
		m_pModelCom->Set_AnimationIndex(iAnimIdx, isLoop, fBlendDuration);
}

HRESULT CBody_BattleBase::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBody_BattleBase::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	auto pDesc = static_cast<BODY_BATTLE_DESC*>(pArg);
	if (nullptr == pDesc->pParentMatrix || 0 == pDesc->strModelProtoTag)
		return E_FAIL;

	m_strModelProtoTag = pDesc->strModelProtoTag;
	m_strShaderProtoTag = (0 != pDesc->strShaderProtoTag) ? pDesc->strShaderProtoTag :
		PROTO_COM_SHADER_VTXANIMMESH;
	m_pParentState = pDesc->pParentState;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->ScaleTo(pDesc->fScale, pDesc->fScale, pDesc->fScale);
	m_pModelCom->Set_AnimationIndex(pDesc->iDefaultAnim, pDesc->bLoop);
	m_pModelCom->Set_RootMotionBoneIndex(pDesc->iRootMotionBoneIndex);
	m_pModelCom->Set_EnableRootMotion(pDesc->bEnableRootMotion);

	return S_OK;
}

void CBody_BattleBase::Priority_Update(_float fTimeDelta)
{
}

void CBody_BattleBase::Update(_float fTimeDelta)
{
	if (nullptr != m_pModelCom)
		m_pModelCom->Play_Animation(fTimeDelta);
}

void CBody_BattleBase::Late_Update(_float fTimeDelta)
{
	__super::Compute_CombinedWorldMatrix(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
	m_pGameInstance->Add_RenderGroup(RENDERID::NONBLEND, this);
}

HRESULT CBody_BattleBase::Ready_Components()
{
	if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), m_strShaderProtoTag,
		COM_SHADER, reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), m_strModelProtoTag,
		COM_MODEL, reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_BattleBase::Bind_ShaderResources_Common()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pTransformCom->Bind_ShaderResourceCombinedWIT(m_pShaderCom, "g_WITMatrix",
		XMLoadFloat4x4(&m_CombinedWorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(D3DTS::PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFarZ", m_pGameInstance->Get_FarZPtr(), sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

void CBody_BattleBase::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}