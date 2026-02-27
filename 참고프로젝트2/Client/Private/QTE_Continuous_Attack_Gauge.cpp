#include "stdafx.h"
#include "..\Public\QTE_Continuous_Attack_Gauge.h"

#include "RenderInstance.h"
#include "GameInstance.h"
#include "QTE_Continuous_Attack.h"

CQTE_Continuous_Attack_Gauge::CQTE_Continuous_Attack_Gauge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CQTE_Continuous_Attack_Gauge::CQTE_Continuous_Attack_Gauge(const CQTE_Continuous_Attack_Gauge& Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CQTE_Continuous_Attack_Gauge::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CQTE_Continuous_Attack_Gauge::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	CONTINUOUS_ATTACK_GAUGE_DESC* Desc = static_cast<CONTINUOUS_ATTACK_GAUGE_DESC*>(pArg);

	m_fX = Desc->fX;
	m_fY = Desc->fY;
	m_fSizeX = Desc->fSizeX;
	m_fSizeY = Desc->fSizeY;
	m_pContinuous_Attack = Desc->pContinuous_Attack;

	m_pTransformCom->Set_Scaled(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.9f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

	SetActive(true);

	return S_OK;
}

void CQTE_Continuous_Attack_Gauge::Camera_Update(_float fTimeDelta)
{

}

void CQTE_Continuous_Attack_Gauge::Update(_float fTimeDelta)
{
	if (!m_bIsActive)
		return;

	// 시간에 따라 게이지 감소
	m_fGaugeProgress -= m_fGaugeDecreaseRate * fTimeDelta;
	m_fGaugeProgress = max(m_fGaugeProgress, 0.0f); // 0으로 클램프

	// 실패 조건 확인
	if (m_fGaugeProgress <= 0.0f)
	{
		m_pContinuous_Attack->Notify_Result(CQTE_Continuous_Attack::MISSION_STATE::MISSION_FAILED);
		SetActive(false);
	}

}

void CQTE_Continuous_Attack_Gauge::Late_Update(_float fTimeDelta)
{
	if (!m_bIsActive)
		return;

	RENDER_OBJECT tDesc{};
	tDesc.tGlowDesc.iPassIndex = 2;
	tDesc.tGlowDesc.fGlowFactor = 2.f;

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_MULTY_GLOW, this, &tDesc);
}

HRESULT CQTE_Continuous_Attack_Gauge::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(4)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CQTE_Continuous_Attack_Gauge::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_QTE_VtxRect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_SKillGaugeBar"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom[0]))))
		return E_FAIL;

	/* For.Com_NextTexture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_SKillGaugeBar"),
		TEXT("Com_NextTexture"), reinterpret_cast<CComponent**>(&m_pTextureCom[1]))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CQTE_Continuous_Attack_Gauge::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom[0]->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pTextureCom[1]->Bind_ShaderResource(m_pShaderCom, "g_NextTexture", 7)))
		return E_FAIL;

	// m_fGaugeProgress를 셰이더의 g_Ratio에 바인딩
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Ratio", &m_fGaugeProgress, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

void CQTE_Continuous_Attack_Gauge::Process_Command()
{
	// 키 입력 시 게이지 증가
	m_fGaugeProgress += m_fGaugeIncreaseAmount;
	m_fGaugeProgress = min(m_fGaugeProgress, 1.0f); // 1로 클램프

	// 성공 조건 확인
	if (m_fGaugeProgress >= 1.0f)
	{
		m_pContinuous_Attack->Notify_Result(CQTE_Continuous_Attack::MISSION_STATE::MISSION_SUCCESS);
		SetActive(false);
	}
}



CQTE_Continuous_Attack_Gauge* CQTE_Continuous_Attack_Gauge::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CQTE_Continuous_Attack_Gauge* pInstance = new CQTE_Continuous_Attack_Gauge(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CQTE_Continuous_Attack_Gauge"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CQTE_Continuous_Attack_Gauge::Clone(void* pArg)
{
	CQTE_Continuous_Attack_Gauge* pInstance = new CQTE_Continuous_Attack_Gauge(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CQTE_Continuous_Attack_Gauge"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CQTE_Continuous_Attack_Gauge::Free()
{
	for (auto& iter : m_pTextureCom)
		Safe_Release(iter);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);

	__super::Free();
}
