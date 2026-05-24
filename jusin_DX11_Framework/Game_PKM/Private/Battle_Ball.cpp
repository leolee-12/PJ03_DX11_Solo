#include "Battle_Ball.h"

#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"

CBattle_Ball::CBattle_Ball(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
	m_strName = L"Battle_Ball";
	XMStoreFloat4x4(&m_RotationCorrection, XMMatrixIdentity());
}

CBattle_Ball::CBattle_Ball(const CBattle_Ball& Prototype)
	: CPartObject{ Prototype }
	, m_RotationCorrection{ Prototype.m_RotationCorrection }
{
}

HRESULT CBattle_Ball::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBattle_Ball::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	auto pDesc = static_cast<BATTLE_BALL_DESC*>(pArg);
	if (nullptr == pDesc->pParentMatrix)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fScale = pDesc->fScale;
	m_pTransformCom->ScaleTo(m_fScale, m_fScale, m_fScale);
	m_pTransformCom->Set_State(STATE::POSITION,
		XMVectorSet(pDesc->vLocalOffset.x, pDesc->vLocalOffset.y, pDesc->vLocalOffset.z, 1.f));
	
	m_pModelCom->Set_AnimationIndex(0, false, 0.f);
	m_pModelCom->Play_Animation(0.f);

	Hide();

	return S_OK;
}

void CBattle_Ball::Priority_Update(_float fTimeDelta)
{
}

void CBattle_Ball::Update(_float fTimeDelta)
{
	m_bAnimFinishedThisFrame = false;

	if (!m_bVisible || nullptr == m_pModelCom)
		return;

	m_bAnimFinishedThisFrame = m_pModelCom->Play_Animation(fTimeDelta);

	const _float3& vDelta = m_pModelCom->Get_RootMotionDelta();

	if (m_bAnimFinishedThisFrame)
	{
#ifdef _DEBUG
		if (m_bDebugHoldVisible)
			return;
#endif
		Hide();
	}
}

void CBattle_Ball::Late_Update(_float fTimeDelta)
{
	const _matrix CorrectionMatrix = XMLoadFloat4x4(&m_RotationCorrection);
	const _matrix LocalMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());

	__super::Compute_CombinedWorldMatrix(CorrectionMatrix * LocalMatrix);

	if (!m_bVisible)
		return;

	m_pGameInstance->Add_RenderGroup(RENDERID::NONBLEND, this);
	m_pGameInstance->Add_RenderGroup(RENDERID::SHADOW, this);
}

HRESULT CBattle_Ball::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	const _uint iNumMeshes = static_cast<_uint>(m_pModelCom->Get_NumMeshes());

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_TexDiff", i, MATERIAL_TYPE::DIFFUSE, 0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_TexNorm", i, MATERIAL_TYPE::NORMALS, 0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CBattle_Ball::Render_Shadow()
{
	if (FAILED(Bind_ShadowResources()))
		return E_FAIL;

	const _uint iNumMeshes = static_cast<_uint>(m_pModelCom->Get_NumMeshes());

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(1)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

void CBattle_Ball::Show()
{
	m_bVisible = true;

#ifdef _DEBUG
	OutputDebugStringA("[BattleBall] Show\n");
#endif
}

void CBattle_Ball::Hide()
{
	m_bVisible = false;

#ifdef _DEBUG
	OutputDebugStringA("[BattleBall] Hide\n");
#endif
}

void CBattle_Ball::Set_LocalOffset(const _float3& vLocalOffset)
{
	m_pTransformCom->Set_State(STATE::POSITION,
		XMVectorSet(vLocalOffset.x, vLocalOffset.y, vLocalOffset.z, 1.f));
}

_bool CBattle_Ball::Set_Anim(_uint iAnimIdx, _bool isLoop, _float fBlendDuration)
{
	if (nullptr == m_pModelCom)
		return false;

	const _uint iNumAnims = m_pModelCom->Get_NumAnimations();
	if (iAnimIdx >= iNumAnims)
		return false;

	if (m_pModelCom->Get_CurrAnimIndex() == iAnimIdx && iNumAnims > 1)
		m_pModelCom->Set_AnimationIndex((iAnimIdx + 1) % iNumAnims, false, 0.f);

	m_pModelCom->Set_AnimationIndex(iAnimIdx, isLoop, fBlendDuration);
	m_pModelCom->Play_Animation(0.f);

	m_bAnimFinishedThisFrame = false;

	return true;
}

_uint CBattle_Ball::Get_NumAnims() const
{
	return (nullptr != m_pModelCom) ? m_pModelCom->Get_NumAnimations() : 0u;
}

_uint CBattle_Ball::Get_CurrAnim() const
{
	return (nullptr != m_pModelCom) ? m_pModelCom->Get_CurrAnimIndex() : 0u;
}

void XM_CALLCONV CBattle_Ball::Set_RotationCorrection(_fmatrix RotationMatrix)
{
	XMStoreFloat4x4(&m_RotationCorrection, RotationMatrix);
}

void CBattle_Ball::Clear_RotationCorrection()
{
	XMStoreFloat4x4(&m_RotationCorrection, XMMatrixIdentity());
}

HRESULT CBattle_Ball::Ready_Components()
{
	if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), PROTO_COM_SHADER_VTXANIMMESH,
		COM_SHADER, reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), PROTO_COM_MODEL_MONSTER_BALL,
		COM_MODEL, reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CBattle_Ball::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResourceCombinedWIT(
		m_pShaderCom, "g_WITMatrix", XMLoadFloat4x4(&m_CombinedWorldMatrix))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(D3DTS::VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(D3DTS::PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFarZ", m_pGameInstance->Get_FarZPtr(), sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CBattle_Ball::Bind_ShadowResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResourceCombinedWIT(
		m_pShaderCom, "g_WITMatrix", XMLoadFloat4x4(&m_CombinedWorldMatrix))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix",
		m_pGameInstance->Get_Shadow_Transform(D3DTS::VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix",
		m_pGameInstance->Get_Shadow_Transform(D3DTS::PROJ))))
		return E_FAIL;

	return S_OK;
}

CBattle_Ball* CBattle_Ball::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBattle_Ball* pInstance = new CBattle_Ball(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBattle_Ball");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBattle_Ball::Clone(void* pArg)
{
	CBattle_Ball* pInstance = new CBattle_Ball(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBattle_Ball");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBattle_Ball::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}