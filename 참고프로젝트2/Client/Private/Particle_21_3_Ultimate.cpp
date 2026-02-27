#include "stdafx.h"
#include "..\Public\Particle_21_3_Ultimate.h"

#include "RenderInstance.h"
#include "GameInstance.h"

CParticle_21_3_Ultimate::CParticle_21_3_Ultimate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CParticle{ pDevice, pContext }
{

}

CParticle_21_3_Ultimate::CParticle_21_3_Ultimate(const CParticle_21_3_Ultimate& Prototype)
	: CParticle{ Prototype }
{

}

HRESULT CParticle_21_3_Ultimate::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParticle_21_3_Ultimate::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	PARTICLE_DESC* Desc = static_cast<PARTICLE_DESC*>(pArg);
	m_fParticle_XScale = Desc->fXScale;
	m_fParticle_YScale = Desc->fYScale;
	m_fGlow_Factor = Desc->fGlowFactor;
	m_iPassIndex = Desc->iPassIndex;
	m_vColor = Desc->vColor;


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	return S_OK;
}

void CParticle_21_3_Ultimate::Camera_Update(_float fTimeDelta)
{

}

void CParticle_21_3_Ultimate::Update(_float fTimeDelta)
{
	if (!m_bIsActive)
		return;

	_bool isComplete = m_pVIBufferCom->Spread(fTimeDelta);

	if (isComplete)
	{
		m_pVIBufferCom->Particle_Initialize();
		SetActive(false);
	}
}

void CParticle_21_3_Ultimate::Late_Update(_float fTimeDelta)
{
	if (!m_bIsActive)
		return;

	RENDER_OBJECT tDesc{};
	tDesc.tGlowDesc.iPassIndex = m_iPassIndex;
	tDesc.tGlowDesc.fGlowFactor = m_fGlow_Factor;

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_BACKSIDE_EFFECT, this, &tDesc);
}

HRESULT CParticle_21_3_Ultimate::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(3)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CParticle_21_3_Ultimate::Set_Particle_Active(_bool isActive)
{
	if (isActive == false)
		m_pVIBufferCom->Particle_Initialize();

	SetActive(isActive);
}

HRESULT CParticle_21_3_Ultimate::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Particle_VtxPoint"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_21_Ultimate_3_21_Spread"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle_21_3_Ultimate::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
		return S_OK;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_ScaleX", &m_fParticle_XScale, sizeof(_float))))
		return S_OK;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_ScaleY", &m_fParticle_YScale, sizeof(_float))))
		return S_OK;

	return S_OK;
}

CParticle_21_3_Ultimate* CParticle_21_3_Ultimate::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CParticle_21_3_Ultimate* pInstance = new CParticle_21_3_Ultimate(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CParticle_21_3_Ultimate"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CParticle_21_3_Ultimate::Clone(void* pArg)
{
	CParticle_21_3_Ultimate* pInstance = new CParticle_21_3_Ultimate(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CParticle_21_3_Ultimate"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticle_21_3_Ultimate::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);

	__super::Free();
}
