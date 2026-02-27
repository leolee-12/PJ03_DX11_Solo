#include "stdafx.h"
#include "..\Public\QTE_Continuous_Attack_Effect.h"

#include "RenderInstance.h"
#include "GameInstance.h"

CQTE_Continuous_Attack_Effect::CQTE_Continuous_Attack_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CQTE_Continuous_Attack_Effect::CQTE_Continuous_Attack_Effect(const CQTE_Continuous_Attack_Effect& Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CQTE_Continuous_Attack_Effect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CQTE_Continuous_Attack_Effect::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	CONTINUOUS_ATTACK_EFFECT_DESC* Desc = static_cast<CONTINUOUS_ATTACK_EFFECT_DESC*>(pArg);

	m_fX = Desc->fX;
	m_fY = Desc->fY;
	m_fSizeX = Desc->fSizeX;
	m_fSizeY = Desc->fSizeY;
	m_fLifeTime = Desc->fTimer;
	m_iTextureNum = Desc->iTextureNum;
	m_fTimer = m_fLifeTime;

	m_pTransformCom->Set_Scaled(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.9f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

	SetActive(true);

	return S_OK;
}

void CQTE_Continuous_Attack_Effect::Camera_Update(_float fTimeDelta)
{

}

void CQTE_Continuous_Attack_Effect::Update(_float fTimeDelta)
{
   	if (!m_bIsActive)
		return;

	// 남은 시간을 감소시키고 경과 시간을 누적
	m_fTimer -= fTimeDelta;
	m_fElapsedTime += fTimeDelta;

	// 타이머가 끝나면 이펙트를 비활성화
	if (m_fTimer <= 0)
	{
		m_fTimer = 0;
		SetActive(false);
	}
}

void CQTE_Continuous_Attack_Effect::Late_Update(_float fTimeDelta)
{
	if (!m_bIsActive)
		return;

	RENDER_OBJECT tDesc{};
	tDesc.tGlowDesc.iPassIndex = 2;
	tDesc.tGlowDesc.fGlowFactor = 5.f;

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_MULTY_GLOW, this, &tDesc);
}

HRESULT CQTE_Continuous_Attack_Effect::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(5)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}


HRESULT CQTE_Continuous_Attack_Effect::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_QTE_VtxRect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_QTE_Result"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CQTE_Continuous_Attack_Effect::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureNum)))
		return E_FAIL;

	// 시간 값을 셰이더로 전달
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Time", &m_fElapsedTime, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_MaxTime", &m_fLifeTime, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}



CQTE_Continuous_Attack_Effect* CQTE_Continuous_Attack_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CQTE_Continuous_Attack_Effect* pInstance = new CQTE_Continuous_Attack_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CQTE_Continuous_Attack_Effect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CQTE_Continuous_Attack_Effect::Clone(void* pArg)
{
	CQTE_Continuous_Attack_Effect* pInstance = new CQTE_Continuous_Attack_Effect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CQTE_Continuous_Attack_Effect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CQTE_Continuous_Attack_Effect::Free()
{
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);

	__super::Free();
}
