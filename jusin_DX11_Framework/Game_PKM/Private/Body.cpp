#include "Body.h"

#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"

CBody::CBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CBody::CBody(const CBody& Prototype)
	: CPartObject{ Prototype }
{
}

const _float4x4* CBody::Get_BoneMatrixPtr(const _char* pBoneName) const
{
	return m_pModelCom->Get_BoneMatrixPtr(pBoneName);
}

const _float3& CBody::Get_RootMotionDelta() const
{
	return m_pModelCom->Get_RootMotionDelta();
}

void CBody::Set_Anim(_uint iAnimIdx, _bool isLoop, _float fBlendDuration)
{
	if (nullptr != m_pModelCom)
		m_pModelCom->Set_AnimationIndex(iAnimIdx, isLoop, fBlendDuration);
}

HRESULT CBody::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBody::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	auto pDesc = static_cast<BODY_DESC*>(pArg);
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
	m_pTransformCom->Set_State(STATE::POSITION,
		XMVectorSet(pDesc->vLocalOffset.x, pDesc->vLocalOffset.y, pDesc->vLocalOffset.z, 1.f));
	m_pModelCom->Set_AnimationIndex(pDesc->iDefaultAnim, pDesc->bLoop);
	m_pModelCom->Set_RootMotionBoneIndex(pDesc->iRootMotionBoneIndex);
	m_pModelCom->Set_EnableRootMotion(pDesc->bEnableRootMotion);

	return S_OK;
}

void CBody::Priority_Update(_float fTimeDelta)
{
}

void CBody::Update(_float fTimeDelta)
{
	if (nullptr != m_pModelCom)
		m_pModelCom->Play_Animation(fTimeDelta);
}

void CBody::Late_Update(_float fTimeDelta)
{
	__super::Compute_CombinedWorldMatrix(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));


	_vector vPos = XMVectorSet(m_CombinedWorldMatrix._41, m_CombinedWorldMatrix._42, m_CombinedWorldMatrix._43, 1.f);
	if (true == m_pGameInstance->isIn_Frustum_WorldSpace(vPos, 5.f))
	{
		m_pGameInstance->Add_RenderGroup(RENDERID::NONBLEND, this);
		m_pGameInstance->Add_RenderGroup(RENDERID::SHADOW, this);
	}
}

HRESULT CBody::Ready_Components()
{
	if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), m_strShaderProtoTag,
		COM_SHADER, reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), m_strModelProtoTag,
		COM_MODEL, reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody::Bind_ShaderResources_Common()
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

void CBody::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}