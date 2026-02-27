#include "stdafx.h"
#include "..\Public\QTE_Continuous_Attack_Space.h"

#include "RenderInstance.h"
#include "GameInstance.h"

CQTE_Continuous_Attack_Space::CQTE_Continuous_Attack_Space(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CQTE_Continuous_Attack_Space::CQTE_Continuous_Attack_Space(const CQTE_Continuous_Attack_Space& Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CQTE_Continuous_Attack_Space::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CQTE_Continuous_Attack_Space::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	CONTINUOUS_ATTACK_DESC* Desc = static_cast<CONTINUOUS_ATTACK_DESC*>(pArg);

	m_fX = Desc->fX;
	m_fY = Desc->fY;
	m_fSizeX = Desc->fSizeX;
	m_fSizeY = Desc->fSizeY;

	// 기본 위치 설정
	m_fDefaultY = m_fY;
	// 타겟 위치 설정 (예시로 Y 좌표를 100만큼 아래로 이동)
	m_fTargetY = m_fDefaultY + 30.f;
	// 초기 위치를 기본 위치로 설정

	m_pTransformCom->Set_Scaled(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.9f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

	SetActive(true);

	return S_OK;
}

void CQTE_Continuous_Attack_Space::Camera_Update(_float fTimeDelta)
{

}

void CQTE_Continuous_Attack_Space::Update(_float fTimeDelta)
{
   	if (!m_bIsActive)
		return;

	Update_Animation(fTimeDelta);
}

void CQTE_Continuous_Attack_Space::Late_Update(_float fTimeDelta)
{
	if (!m_bIsActive)
		return;

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

void CQTE_Continuous_Attack_Space::Update_Animation(_float fTimeDelta)
{
	// 현재 진행 시간 업데이트
	m_fCurrentTime += fTimeDelta;

	if (m_bIsMovingDown)
	{
		// 내려가는 애니메이션 진행
		_float fProgress = m_fCurrentTime / m_fMoveDownTime;

		if (fProgress >= 1.0f)
		{
			// 내려가기 완료, 올라가기 시작
			fProgress = 1.0f;
			m_bIsMovingDown = false;
			m_fCurrentTime = 0.0f;
		}

		// 이징 함수 적용 (Ease-In-Out)
		_float fEaseProgress = EaseInOut(fProgress);

		// 위치 계산
		m_fY = Lerp(m_fDefaultY, m_fTargetY, fEaseProgress);
	}
	else
	{
		// 올라가는 애니메이션 진행
		_float fProgress = m_fCurrentTime / m_fMoveUpTime;

		if (fProgress >= 1.0f)
		{
			// 애니메이션 종료
			fProgress = 1.0f;
			m_bIsMoving = false;
		}

		// 이징 함수 적용 (Ease-In-Out)
		_float fEaseProgress = EaseInOut(fProgress);

		// 위치 계산
		m_fY = Lerp(m_fTargetY, m_fDefaultY, fEaseProgress);
	}

	// 실제 Transform에 위치 적용
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.9f, 1.f));
}

void CQTE_Continuous_Attack_Space::Process_Command()
{
	// 애니메이션 상태 초기화
	m_bIsMoving = true;
	m_bIsMovingDown = true;   // 먼저 내려가기 시작
	m_fCurrentTime = 0.0f;    // 진행 시간 초기화

	// 위치를 기본 위치로 초기화
	m_fY = m_fDefaultY;

	// Transform에 위치 적용
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.9f, 1.f));
}

HRESULT CQTE_Continuous_Attack_Space::Render(_float fTimeDelta)
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


HRESULT CQTE_Continuous_Attack_Space::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI_VtxRect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_QTE_Space"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CQTE_Continuous_Attack_Space::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	return S_OK;
}



CQTE_Continuous_Attack_Space* CQTE_Continuous_Attack_Space::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CQTE_Continuous_Attack_Space* pInstance = new CQTE_Continuous_Attack_Space(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CQTE_Continuous_Attack_Space"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CQTE_Continuous_Attack_Space::Clone(void* pArg)
{
	CQTE_Continuous_Attack_Space* pInstance = new CQTE_Continuous_Attack_Space(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CQTE_Continuous_Attack_Space"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CQTE_Continuous_Attack_Space::Free()
{
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);

	__super::Free();
}
