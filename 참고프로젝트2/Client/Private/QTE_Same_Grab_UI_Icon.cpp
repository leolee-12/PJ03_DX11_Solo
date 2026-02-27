#include "stdafx.h"
#include "..\Public\QTE_Same_Grab_UI_Icon.h"

#include "RenderInstance.h"
#include "GameInstance.h"
#include "QTE_Same_Grab.h"
#include "QTE_1P_Same_Grab.h"

CQTE_Same_Grab_UI_Icon::CQTE_Same_Grab_UI_Icon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CQTE_Same_Grab_UI_Icon::CQTE_Same_Grab_UI_Icon(const CQTE_Same_Grab_UI_Icon& Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CQTE_Same_Grab_UI_Icon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CQTE_Same_Grab_UI_Icon::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	CQTE_Same_Grab_UI_Icon::QTE_Same_Grab_UI_ICON_DESC* desc = static_cast<CQTE_Same_Grab_UI_Icon::QTE_Same_Grab_UI_ICON_DESC*>(pArg);
	m_fSizeX = desc->fSizeX;
	m_fSizeY = desc->fSizeY;
	m_fX = desc->fX;
	m_fY = desc->fY;
	m_fAlpha = desc->fAlpha;
	m_iTextureNumber = desc->iTextureNumber;
	m_fFallDelay = desc->fFallDelay; // 추가: 쿨다운 지연 시간 설정
	m_bIsFirst = desc->isFirst;
	m_bIsLast = desc->isLast;
	m_pSameGrab = desc->SameGrab;

	// 초기 Y 위치 설정
	m_fCurrentY = m_fStartY;    // 초기 위치는 -100
	m_fTargetY = m_fDefault_Y;  // 최종 위치는 m_fDefault_Y

	m_pTransformCom->Set_Scaled(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fCurrentY + g_iWinSizeY * 0.5f, 0.9f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

	// 떨어지기 애니메이션 시작
	Set_State(FALLING);

	return S_OK;
}

void CQTE_Same_Grab_UI_Icon::Camera_Update(_float fTimeDelta)
{

}

void CQTE_Same_Grab_UI_Icon::Update(_float fTimeDelta)
{
	switch (m_State)
	{
	case FALLING:
		Update_Falling(fTimeDelta);
		break;

	case SELECTED:
		Update_Selected(fTimeDelta);
		break;

	case ALREADY_PRESSED:
		Update_AlreadyPressed(fTimeDelta);
		break;

	case WRONG_PRESSED:
		Update_WrongPressed(fTimeDelta);
		break;

	case ASCEND:
		Update_Ascend(fTimeDelta);
		break;

	case NOT_SELECTED:
	default:
		Update_NotSelected(fTimeDelta);
		break;
	}
}

void CQTE_Same_Grab_UI_Icon::Late_Update(_float fTimeDelta)
{
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CQTE_Same_Grab_UI_Icon::Render(_float fTimeDelta)
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

void CQTE_Same_Grab_UI_Icon::Set_State(IconState state)
{
	m_State = state;

	switch (m_State)
	{
	case NOT_SELECTED:
		m_fTargetY = m_fDefault_Y;
		m_bIsShaking = false;
		break;

	case FALLING:
		//Falling일때, 첫번째 녀석은 TargetY가 달라야함
		m_fTargetY = m_bIsFirst == true ? m_fSelected_Y : m_fDefault_Y;
		m_fElapsedTime = 0.f; // 애니메이션 시간 초기화
		m_bIsShaking = false;
		break;

	case SELECTED:
		m_fTargetY = m_fSelected_Y;
		m_fElapsedTime = 0.f; // 애니메이션 시간 초기화
		m_bIsShaking = false;
		break;

	case ALREADY_PRESSED:
		m_fTargetY = m_fDefault_Y;
		m_fElapsedTime = 0.f; // 애니메이션 시간 초기화
		m_bIsShaking = false;
		break;

	case WRONG_PRESSED:
		m_fTargetY = m_fSelected_Y;
		m_fShakeTime = 0.0f;        // 초기 흔들림 시간
		m_bIsShaking = true;         // 흔들림 시작
		break;

	case ASCEND:
		m_fTargetY = m_fStartY;      // 기본 Y 위치로 설정
		m_fAscendDelay = m_fElapsedTime; // ASCEND 상태 시작 시 지연 시간 설정 (외부에서 설정)
		m_fElapsedTime = 0.f; // 애니메이션 시간 초기화
		m_bIsAscending = false; // 올라가기 애니메이션 시작 전
		break;

	default:
		break;
	}
}

HRESULT CQTE_Same_Grab_UI_Icon::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_QTE_VtxRect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_ActionInput"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CQTE_Same_Grab_UI_Icon::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureNumber)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_IconState", &m_State, sizeof(_int))))
		return E_FAIL;

	return S_OK;
}

void CQTE_Same_Grab_UI_Icon::Update_Falling(_float fTimeDelta)
{
	if (m_bIsFalling)
	{
		m_fElapsedTime += fTimeDelta;
		float t = Clamp(m_fElapsedTime / m_fAnimationDuration, 0.f, 1.f);
		// 선형 보간 (linear interpolation)
		m_fCurrentY = Lerp(m_fStartY, m_fTargetY, t);

		// 애니메이션 완료 시 상태 변경
		if (t >= 1.0f)
		{
			m_bIsFalling = false;

			//Same_Grab에게 마지막 객체가 Final_UI가 떨어졌다고 알려야함
			if (m_bIsLast)
			{
				CQTE_Same_Grab* IsSameGrab = dynamic_cast<CQTE_Same_Grab*>(m_pSameGrab);
				if (IsSameGrab)
					IsSameGrab->Notify_Last_UI_Final_Complete();
				else
				{
					CQTE_1P_Same_Grab* is1pSameGrab = dynamic_cast<CQTE_1P_Same_Grab*>(m_pSameGrab);
					is1pSameGrab->Notify_Last_UI_Final_Complete();
				}
			}

			Set_State(NOT_SELECTED); // 애니메이션 완료 후 기본 상태로 전환
		}

		// 위치 업데이트
		m_pTransformCom->Set_State(CTransform::STATE_POSITION,
			XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fCurrentY + g_iWinSizeY * 0.5f, 0.9f, 1.f));
	}
	else
	{
		// 떨어지기 시작 전 지연 시간 처리
		m_fElapsedTime += fTimeDelta;
		if (m_fElapsedTime >= m_fFallDelay)
		{
			m_bIsFalling = true;
			m_fElapsedTime = 0.f; // 애니메이션 시간 초기화
		}
	}
}

void CQTE_Same_Grab_UI_Icon::Update_Selected(_float fTimeDelta)
{
	// 부드러운 이동을 위한 애니메이션 처리
	m_fElapsedTime += fTimeDelta;
	float t = Clamp(m_fElapsedTime / m_fAnimationDuration, 0.f, 1.f);

	// 비선형 애니메이션 커브 적용 (ease-out)
	t = 1.0f - powf(1.0f - t, 3);

	// 선형 보간 대신 비선형 커브를 적용한 t를 사용하여 Y 위치 업데이트
	m_fCurrentY = Lerp(m_fCurrentY, m_fTargetY, t);

	// 위치 업데이트
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fCurrentY + g_iWinSizeY * 0.5f, 0.9f, 1.f));
}

void CQTE_Same_Grab_UI_Icon::Update_AlreadyPressed(_float fTimeDelta)
{
	// 애니메이션 진행 시간 증가
	m_fElapsedTime += fTimeDelta;

	// 애니메이션 진행 비율 계산 (0.0f ~ 1.0f)
	float t = Clamp(m_fElapsedTime / m_fAnimationDuration, 0.f, 1.f);

	// 비선형 애니메이션 커브 적용 (ease-out)
	t = 1.0f - powf(1.0f - t, 3);

	// Y 위치 부드럽게 이동 (LERP 사용)
	m_fCurrentY = Lerp(m_fCurrentY, m_fTargetY, t);

	// 위치 업데이트
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fCurrentY + g_iWinSizeY * 0.5f, 0.9f, 1.f));
}

void CQTE_Same_Grab_UI_Icon::Update_WrongPressed(_float fTimeDelta)
{
	// 흔들림 애니메이션 처리
	if (m_bIsShaking)
	{
		m_fShakeTime += fTimeDelta;

		if (m_fShakeTime >= m_fShakeDuration)
		{
			// 흔들림 종료 후 상태를 SELECTED로 변경
			m_bIsShaking = false;
			Set_State(SELECTED);
		}
		else
		{
			// 흔들림 계산
			float shakeOffsetX = m_fShakeAmplitude * sinf(2.0f * XM_PI * m_fShakeFrequency * m_fShakeTime);
			// 감쇠 적용
			float damping = 1.0f - (m_fShakeTime / m_fShakeDuration);
			shakeOffsetX *= damping;

			// 흔들림이 적용된 X 위치 계산
			float shakenX = m_fX + shakeOffsetX;

			// 흔들림이 적용된 위치 설정
			m_pTransformCom->Set_State(CTransform::STATE_POSITION,
				XMVectorSet(shakenX - g_iWinSizeX * 0.5f, -m_fCurrentY + g_iWinSizeY * 0.5f, 0.9f, 1.f));
		}
	}
}

void CQTE_Same_Grab_UI_Icon::Update_Ascend(_float fTimeDelta)
{
	// 상승 시작 전 지연 시간 처리
	if (!m_bIsAscending)
	{
		m_fElapsedTime += fTimeDelta;
		if (m_fElapsedTime >= m_fAscendDelay)
		{
			m_bIsAscending = true;
			m_fElapsedTime = 0.f; // 애니메이션 시간 초기화
		}
		return;
	}

	// 상승 애니메이션 처리
	m_fElapsedTime += fTimeDelta;
	float t = Clamp(m_fElapsedTime / m_fAnimationDuration, 0.f, 1.f);

	// 비선형 애니메이션 커브 적용 (ease-out)
	t = 1.0f - powf(1.0f - t, 3);

	// Y 위치 부드럽게 이동 (LERP 사용)
	m_fCurrentY = Lerp(m_fCurrentY, m_fTargetY, t);

	// 위치 업데이트
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fCurrentY + g_iWinSizeY * 0.5f, 0.9f, 1.f));

}

void CQTE_Same_Grab_UI_Icon::Update_NotSelected(_float fTimeDelta)
{
	// 위치 업데이트
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fCurrentY + g_iWinSizeY * 0.5f, 0.9f, 1.f));
}

CQTE_Same_Grab_UI_Icon* CQTE_Same_Grab_UI_Icon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CQTE_Same_Grab_UI_Icon* pInstance = new CQTE_Same_Grab_UI_Icon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CQTE_Same_Grab_UI_Icon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CQTE_Same_Grab_UI_Icon::Clone(void* pArg)
{
	CQTE_Same_Grab_UI_Icon* pInstance = new CQTE_Same_Grab_UI_Icon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CQTE_Same_Grab_UI_Icon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CQTE_Same_Grab_UI_Icon::Free()
{
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);

	__super::Free();
}
