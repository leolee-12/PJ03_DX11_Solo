#include "stdafx.h"
#include "..\Public\QTE_Continuous_Attack_Space_Particle.h"

#include "GameInstance.h"
#include "RenderInstance.h"
CQTE_Continuous_Attack_Space_Particle::CQTE_Continuous_Attack_Space_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CQTE_Continuous_Attack_Space_Particle::CQTE_Continuous_Attack_Space_Particle(const CQTE_Continuous_Attack_Space_Particle& Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CQTE_Continuous_Attack_Space_Particle::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CQTE_Continuous_Attack_Space_Particle::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	QTE_Continuous_Attack_Space_Particle_DESC* desc = static_cast<QTE_Continuous_Attack_Space_Particle_DESC*>(pArg);

	m_fSizeX = desc->fSizeX;
	m_fSizeY = desc->fSizeY;
	m_fX = desc->fX;
	m_fY = desc->fY;

	m_pTransformCom->Set_Scaled(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f));

	SetActive(true);

	return S_OK;
}

void CQTE_Continuous_Attack_Space_Particle::Camera_Update(_float fTimeDelta)
{
}

void CQTE_Continuous_Attack_Space_Particle::Update(_float fTimeDelta)
{
	if (!m_bIsActive)
		return;

	_bool isComplete = m_pVIBufferCom->Half_Spread_Y_2D(fTimeDelta);

	if (isComplete)
		SetActive(false);
}

void CQTE_Continuous_Attack_Space_Particle::Late_Update(_float fTimeDelta)
{
	if (!m_bIsActive)
		return;

	RENDER_OBJECT tDesc{};
	tDesc.tGlowDesc.iPassIndex = 2;
	tDesc.tGlowDesc.fGlowFactor = 5.f;

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_MULTY_GLOW, this, &tDesc);
}

HRESULT CQTE_Continuous_Attack_Space_Particle::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(1)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CQTE_Continuous_Attack_Space_Particle::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Particle_VtxPoint"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Continuous_Space_Spread_QTE"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CQTE_Continuous_Attack_Space_Particle::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	_float4 color = _float4(1.f, 1.f, 0.f, 0.5f);

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &color, sizeof(_float4))))
		return S_OK;
}

CQTE_Continuous_Attack_Space_Particle* CQTE_Continuous_Attack_Space_Particle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CQTE_Continuous_Attack_Space_Particle* pInstance = new CQTE_Continuous_Attack_Space_Particle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CQTE_Continuous_Attack_Space_Particle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CQTE_Continuous_Attack_Space_Particle::Clone(void* pArg)
{
	CQTE_Continuous_Attack_Space_Particle* pInstance = new CQTE_Continuous_Attack_Space_Particle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CQTE_Continuous_Attack_Space_Particle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CQTE_Continuous_Attack_Space_Particle::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);

	__super::Free();
}
