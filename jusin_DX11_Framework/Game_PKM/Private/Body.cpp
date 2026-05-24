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

_bool CBody::Get_BoneWorldPosition(const _char* pBoneName, _float3* pOutPosition) const
{
	if (nullptr == pOutPosition || nullptr == m_pModelCom)
		return false;

	const _float4x4* pBoneMatrix = m_pModelCom->Get_BoneMatrixPtr(pBoneName);
	if (nullptr == pBoneMatrix)
		return false;

	const _matrix BoneMatrix = XMLoadFloat4x4(pBoneMatrix);
	const _matrix BodyLocalMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
	const _matrix ParentWorldMatrix = XMLoadFloat4x4(m_pParentMatrix);
	const _matrix WorldMatrix = BodyLocalMatrix * ParentWorldMatrix;
	const _matrix BoneWorldMatrix = BoneMatrix * WorldMatrix;

	XMStoreFloat3(pOutPosition,
		XMVector3TransformCoord(XMVectorZero(), BoneWorldMatrix));

	return true;
}

void CBody::Refresh_AnimationPose()
{
	if (nullptr != m_pModelCom)
		m_pModelCom->Play_Animation(0.f);
}

_bool CBody::Set_Anim(_uint iAnimIdx, _bool isLoop, _float fBlendDuration)
{
	if (nullptr == m_pModelCom)
		return false;

	if (iAnimIdx >= m_pModelCom->Get_NumAnimations())
		return false;

	m_pModelCom->Set_AnimationIndex(iAnimIdx, isLoop, fBlendDuration);
	m_bAnimFinishedThisFrame = false;

	return true;
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

	m_pTransformCom->ScaleTo(pDesc->fScale, pDesc->fScale, pDesc->fScale);
	m_pTransformCom->Set_State(STATE::POSITION,
		XMVectorSet(pDesc->vLocalOffset.x, pDesc->vLocalOffset.y, pDesc->vLocalOffset.z, 1.f));
	m_pModelCom->Set_AnimationIndex(pDesc->iDefaultAnim, pDesc->bLoop);

	_uint iRootBoneIndex = pDesc->iRootMotionBoneIndex;
	if (!pDesc->strRootMotionBoneName.empty())
	{
		const _int iResolved = m_pModelCom->Get_BoneIndex(pDesc->strRootMotionBoneName.c_str());

#ifdef _DEBUG
		{
			char szLog[256] = {};
			sprintf_s(szLog, "[Body] RootBone name=\"%s\" resolved=%d fallback=%u\n",
				pDesc->strRootMotionBoneName.c_str(), iResolved, pDesc->iRootMotionBoneIndex);
			OutputDebugStringA(szLog);
		}
#endif

		if (iResolved >= 0)
			iRootBoneIndex = static_cast<_uint>(iResolved);
	}

	m_pModelCom->Set_RootMotionBoneIndex(iRootBoneIndex);
	m_pModelCom->Set_EnableRootMotion(pDesc->bEnableRootMotion);

	return S_OK;
}

void CBody::Priority_Update(_float fTimeDelta)
{
}

void CBody::Update(_float fTimeDelta)
{
	m_bAnimFinishedThisFrame = false;

	if (nullptr != m_pModelCom)
		m_bAnimFinishedThisFrame = m_pModelCom->Play_Animation(fTimeDelta);
}

void CBody::Late_Update(_float fTimeDelta)
{
	__super::Compute_CombinedWorldMatrix(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));


	_vector vPos = XMVectorSet(m_CombinedWorldMatrix._41, m_CombinedWorldMatrix._42,
		m_CombinedWorldMatrix._43, 1.f);
	if (true == m_pGameInstance->isIn_Frustum_WorldSpace(vPos, 5.f))
	{
		m_pGameInstance->Add_RenderGroup(RENDERID::NONBLEND, this);
		m_pGameInstance->Add_RenderGroup(RENDERID::SHADOW, this);

		if (m_bUseOutline)
			m_pGameInstance->Add_RenderGroup(RENDERID::OUTLINEMASK, this);
	}
}

HRESULT CBody::Render_OutlineMask()
{
	if (0 == m_iOutlineMaskPass)
		return S_OK;

	if (FAILED(Bind_ShaderResources_Common()))
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
	const _matrix ShaderWorldMatrix = Resolve_ShaderWorldMatrix();

	_float4x4 ShaderWorldFloat4x4{};
	XMStoreFloat4x4(&ShaderWorldFloat4x4, ShaderWorldMatrix);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &ShaderWorldFloat4x4)))
		return E_FAIL;
	if (FAILED(m_pTransformCom->Bind_ShaderResourceCombinedWIT(m_pShaderCom, "g_WITMatrix", ShaderWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(D3DTS::PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFarZ", m_pGameInstance->Get_FarZPtr(), sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

_matrix CBody::Resolve_ShaderWorldMatrix() const
{
	return XMLoadFloat4x4(&m_CombinedWorldMatrix);
}

void CBody::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}