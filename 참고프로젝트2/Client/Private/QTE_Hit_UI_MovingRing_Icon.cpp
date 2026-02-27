#include "stdafx.h"
#include "..\Public\QTE_Hit_UI_MovingRing_Icon.h"

#include "RenderInstance.h"
#include "GameInstance.h"
#include "QTE_Hit_Situation.h"
CQTE_Hit_UI_MovingRing_Icon::CQTE_Hit_UI_MovingRing_Icon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CQTE_Hit_UI_MovingRing_Icon::CQTE_Hit_UI_MovingRing_Icon(const CQTE_Hit_UI_MovingRing_Icon& Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CQTE_Hit_UI_MovingRing_Icon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CQTE_Hit_UI_MovingRing_Icon::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Hit_MovingRing_DESC* Desc = static_cast<Hit_MovingRing_DESC*>(pArg);
	m_fSizeX = Desc->fSizeX;
	m_fSizeY = Desc->fSizeY;
	m_fX = Desc->fX;
	m_fY = Desc->fY;
	m_pfTimer = Desc->pfTimer;
	m_pfElaspedTime = Desc->pfElaspedTime;

	m_pTransformCom->Set_Scaled(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.9f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

	SetActive(true);

	return S_OK;
}

void CQTE_Hit_UI_MovingRing_Icon::Camera_Update(_float fTimeDelta)
{

}

void CQTE_Hit_UI_MovingRing_Icon::Update(_float fTimeDelta)
{
	if (!m_bIsActive)
		return;
}

void CQTE_Hit_UI_MovingRing_Icon::Late_Update(_float fTimeDelta)
{
	if (!m_bIsActive)
		return;

	RENDER_OBJECT tDesc{};
	tDesc.tGlowDesc.iPassIndex = 2;
	tDesc.tGlowDesc.fGlowFactor = 10.f;

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_MULTY_GLOW, this, &tDesc);
}

HRESULT CQTE_Hit_UI_MovingRing_Icon::Render(_float fTimeDelta)
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


HRESULT CQTE_Hit_UI_MovingRing_Icon::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_QTE_VtxRect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_QTE_Hit_Circle"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CQTE_Hit_UI_MovingRing_Icon::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 6)))
		return E_FAIL;

	// 시간 값을 셰이더로 전달
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Time", m_pfElaspedTime, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_MaxTime", m_pfTimer, sizeof(_float))))
		return E_FAIL;
	
	return S_OK;
}

CQTE_Hit_UI_MovingRing_Icon* CQTE_Hit_UI_MovingRing_Icon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CQTE_Hit_UI_MovingRing_Icon* pInstance = new CQTE_Hit_UI_MovingRing_Icon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CQTE_Hit_UI_MovingRing_Icon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CQTE_Hit_UI_MovingRing_Icon::Clone(void* pArg)
{
	CQTE_Hit_UI_MovingRing_Icon* pInstance = new CQTE_Hit_UI_MovingRing_Icon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CQTE_Hit_UI_MovingRing_Icon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CQTE_Hit_UI_MovingRing_Icon::Free()
{
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);

	__super::Free();
}
