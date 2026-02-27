#include "stdafx.h"
#include "..\Public\QTE_Hit_UI_Result.h"

#include "RenderInstance.h"
#include "GameInstance.h"
#include "QTE_Hit_Situation.h"
CQTE_Hit_UI_Result::CQTE_Hit_UI_Result(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CQTE_Hit_UI_Result::CQTE_Hit_UI_Result(const CQTE_Hit_UI_Result& Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CQTE_Hit_UI_Result::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CQTE_Hit_UI_Result::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Hit_RESULT_DESC* Desc = static_cast<Hit_RESULT_DESC*>(pArg);
	m_fX = Desc->fX;
	m_fY = Desc->fY;
	m_fSizeX = Desc->fSizeX;
	m_fSizeY = Desc->fSizeY;
	m_iTextureNumber = Desc->iTextureNum;
	m_fLifeTime = Desc->fTimer;
	m_fTimer = m_fLifeTime;

	m_fDefaultY = m_fY;         // 초기 Y 위치
	m_fTargetY = m_fY - 30.0f; // 목표 Y 위치 (예: 100.0f 추가하여 높이 설정)

	m_pTransformCom->Set_Scaled(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.9f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

	SetActive(true);

	return S_OK;
}

void CQTE_Hit_UI_Result::Camera_Update(_float fTimeDelta)
{

}

void CQTE_Hit_UI_Result::Update(_float fTimeDelta)
{
	if (!m_bIsActive)
		return;

	// 남은 시간을 감소시키고 경과 시간을 누적
	m_fTimer -= fTimeDelta;
	m_fElapsedTime += fTimeDelta;

	// Ratio 계산 (0에서 1까지)
	_float ratio = Clamp(m_fElapsedTime / m_fLifeTime, 0.f, 1.f);

	// 로그 함수를 통해 비율 조정 (처음에는 급격히 증가, 점점 완만해짐)
	_float growthFactor = log(1.0f + 9.0f * ratio) / log(10.0f); // 0에서 1로 점진적 증가

	// Y 위치 계산 (기본 Y에서 목표 Y까지)
	_float currentY = m_fDefaultY + (m_fTargetY - m_fDefaultY) * growthFactor;

	// 변환 컴포넌트에 Y 위치 적용
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -currentY + g_iWinSizeY * 0.5f, 0.9f, 1.f));

	// 타이머가 끝나면 이펙트를 비활성화
	if (m_fTimer <= 0)
	{
		m_fTimer = 0;
		SetActive(false);
	}
}

void CQTE_Hit_UI_Result::Late_Update(_float fTimeDelta)
{
	if (!m_bIsActive)
		return;

	RENDER_OBJECT tDesc{};
	tDesc.tGlowDesc.iPassIndex = 2;
	tDesc.tGlowDesc.fGlowFactor = 5.f;

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_MULTY_GLOW, this, &tDesc);
}

HRESULT CQTE_Hit_UI_Result::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}


HRESULT CQTE_Hit_UI_Result::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI_VtxRect"),
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

HRESULT CQTE_Hit_UI_Result::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureNumber)))
		return E_FAIL;

	return S_OK;
}

CQTE_Hit_UI_Result* CQTE_Hit_UI_Result::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CQTE_Hit_UI_Result* pInstance = new CQTE_Hit_UI_Result(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CQTE_Hit_UI_Result"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CQTE_Hit_UI_Result::Clone(void* pArg)
{
	CQTE_Hit_UI_Result* pInstance = new CQTE_Hit_UI_Result(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CQTE_Hit_UI_Result"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CQTE_Hit_UI_Result::Free()
{
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);

	__super::Free();
}
