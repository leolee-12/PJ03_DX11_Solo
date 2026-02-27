#include "stdafx.h"
#include "..\Public\QTE_Continuous_Attack.h"

#include "RenderInstance.h"
#include "GameInstance.h"
#include "QTE_Continuous_Attack_Space.h"
#include "QTE_Continuous_Attack_Gauge.h"
#include "QTE_Continuous_Attack_Effect.h"
#include "QTE_Continuous_Attack_Particle.h"
#include "QTE_Continuous_Attack_Space_Particle.h"
#include "Main_Camera.h"
#include "Character.h"
CQTE_Continuous_Attack::CQTE_Continuous_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CQTE_Continuous_Attack::CQTE_Continuous_Attack(const CQTE_Continuous_Attack& Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CQTE_Continuous_Attack::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CQTE_Continuous_Attack::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fSizeX = 50.f;
	m_fSizeY = 50.f;
	m_fX = 960.f;
	m_fY = 700.f;

	m_fSizeX = 50.f;
	m_fSizeY = 50.f;

	// 기본 위치 설정
	m_fDefaultY = m_fY;
	// 타겟 위치 설정 (예시로 Y 좌표를 100만큼 아래로 이동)
	m_fTargetY = m_fDefaultY + 30.f;


	m_pTransformCom->Set_Scaled(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.9f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

	return S_OK;
}

void CQTE_Continuous_Attack::Camera_Update(_float fTimeDelta)
{

}

void CQTE_Continuous_Attack::Update(_float fTimeDelta)
{
#pragma region 디버그
	//// F5 키 입력 감지
	//if (m_pGameInstance->Key_Down(DIK_F1))
	//{
	//	if (m_bIsQTEActive)
	//	{
	//		// QTE가 활성화되어 있으면 즉시 종료
	//		End_QTE();
	//	}
	//	else
	//	{
	//		// QTE가 비활성화되어 있으면 시작
	//		Start_QTE();
	//	}
	//}
#pragma endregion

#pragma region 활성화

	if (m_bIsQTEActive)
	{
		//인풋 관리
		Handle_QTEInput();

		// 목표 연타 수 달성 확인
		if (m_eMissionState == MISSION_SUCCESS)
		{
			CQTE_Continuous_Attack_Effect::CONTINUOUS_ATTACK_EFFECT_DESC Desc{};

			Desc.fX = m_fX;
			Desc.fY = 750.f;
			Desc.fSizeX = 300.f;
			Desc.fSizeY = 200.f;
			Desc.fTimer = 0.5f;
			Desc.iTextureNum = 1;
			m_pContinuous_Effect = static_cast<CQTE_Continuous_Attack_Effect*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_QTE_Continuous_Attack_Effect"), &Desc));
			m_bOffsetActive = true;

			End_QTE();
			return;
		}
		// 타이머 확인
		else if (m_eMissionState == MISSION_FAILED)
		{
			CQTE_Continuous_Attack_Effect::CONTINUOUS_ATTACK_EFFECT_DESC Desc{};

			Desc.fX = m_fX;
			Desc.fY = 750.f;
			Desc.fSizeX = 300.f;
			Desc.fSizeY = 200.f;
			Desc.fTimer = 0.5f;
			Desc.iTextureNum = 0;
			m_pContinuous_Effect = static_cast<CQTE_Continuous_Attack_Effect*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_QTE_Continuous_Attack_Effect"), &Desc));
			m_bOffsetActive = true;

			End_QTE();
			return;
		}

		m_pContinuous_Space->Update(fTimeDelta);
		m_pContinuous_Gauge->Update(fTimeDelta);

		for (auto& iter : m_UIParticles)
			iter->Update(fTimeDelta);

		for (auto& iter : m_UISpaceParticles)
			iter->Update(fTimeDelta);

		// 애니메이션 업데이트
		if (m_bIsMoving)
		{
			Update_Animation(fTimeDelta);
		}
	}

#pragma endregion
	else if (m_bOffsetActive)
	{
		End_Offset_QTE(fTimeDelta);
	}
}

void CQTE_Continuous_Attack::Start_QTE(CGameObject* callObject)
{
	if (m_bIsQTEActive)
		return; // 이미 QTE가 활성화되어 있으면 무시

	//카메라 쉐이킹용으로 필요함
	m_pMain_Camera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));

	//활성화
	m_bIsQTEActive = true;
	m_pCall_Object = callObject;

	//스페이스 객체 생성
	CQTE_Continuous_Attack_Space::CONTINUOUS_ATTACK_DESC Desc{};
	Desc.fX = 960.f;
	Desc.fY = 750.f;
	Desc.fSizeX = 300.f;
	Desc.fSizeY = 200.f;
	m_pContinuous_Space = static_cast<CQTE_Continuous_Attack_Space*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_QTE_Continuous_Attack_Space"), &Desc));
	m_pContinuous_Space->SetActive(true);

	//게이지 객체 생성
	CQTE_Continuous_Attack_Gauge::CONTINUOUS_ATTACK_GAUGE_DESC Gauge_Desc{};
	Gauge_Desc.fX = 960.f;
	Gauge_Desc.fY = 810.f;
	Gauge_Desc.fSizeX = 300.f;
	Gauge_Desc.fSizeY = 20.f;
	Gauge_Desc.pContinuous_Attack = this;

	m_pContinuous_Gauge = static_cast<CQTE_Continuous_Attack_Gauge*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_QTE_Continuous_Gauge"), &Gauge_Desc));
	m_pContinuous_Gauge->SetActive(true);


	//파티클 생성
	CQTE_Continuous_Attack_Particle::QTE_Continuous_Attack_Particle_DESC Particle_Desc{};

	Particle_Desc.fX = 960.f;
	Particle_Desc.fY = 810.f;
	Particle_Desc.fSizeX = 50.f;
	Particle_Desc.fSizeY = 50.f;
	//객체 게이지바 끝에 있어야 하니까
	Particle_Desc.pfGaugeRatio = m_pContinuous_Gauge->Get_GaugeProcess();
	//파티클 객체 생성
	m_UIParticles.push_back(static_cast<CQTE_Continuous_Attack_Particle*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_QTE_Continuous_Attack_Particle"), &Particle_Desc)));
}

void CQTE_Continuous_Attack::End_QTE()
{
#pragma region 초기화

	// 미션 상태에 따라 hitResult 설정
	// 성공
 	if (m_eMissionState == MISSION_SUCCESS)
	{
		static_cast<CCharacter*>(m_pCall_Object)->Notify_QTE_Continuous_Attack(1);
		m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::QTE_SUCCESS_SFX, false, 0.7f);
	}
	// 실패
	else if (m_eMissionState == MISSION_FAILED)
	{
		static_cast<CCharacter*>(m_pCall_Object)->Notify_QTE_Continuous_Attack(-1);
		m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::QTE_SAMEGRAB_FAIL_SFX, false, 0.7f);
	}

	m_pCall_Object = nullptr;

	//Space 객체는 삭제
	Safe_Release(m_pContinuous_Space);
	m_pContinuous_Space = nullptr;
	//게이지 객체는 삭제
	Safe_Release(m_pContinuous_Gauge);
	m_pContinuous_Gauge = nullptr;

	//활성화 여부 초기화
	m_bIsQTEActive = false;

	// 미션 상태 초기화
	m_eMissionState = MISSION_NOT_DECIDED;

#pragma endregion

}

void CQTE_Continuous_Attack::End_Offset_QTE(_float fTimeDelta)
{
	// 오프셋 기간 처리
	m_fOffsetTimer -= fTimeDelta;

	m_pContinuous_Effect->Update(fTimeDelta);
	for (auto& iter : m_UIParticles)
		iter->Update(fTimeDelta);
	for (auto& iter : m_UISpaceParticles)
		iter->Update(fTimeDelta);

	if (m_fOffsetTimer <= 0.0f)
	{
		Safe_Release(m_pContinuous_Effect);
		m_pContinuous_Effect = nullptr;

		//파티클 모음들 전부 삭제
		for (auto& iter : m_UIParticles)
			Safe_Release(iter);
		m_UIParticles.clear();

		for (auto& iter : m_UISpaceParticles)
			Safe_Release(iter);
		m_UISpaceParticles.clear();

		m_fOffsetTimer = 2.f;
		m_bOffsetActive = false; // 오프셋 기간 종료
	}
}

void CQTE_Continuous_Attack::Handle_QTEInput()
{
	if (m_iCharacterSide == 1)
	{
		if (m_pGameInstance->Key_Down(DIK_SPACE))
		{
			Process_Command();
		}
	}
	else if (m_iCharacterSide == 2)
	{
		if (m_pGameInstance->Key_Down(DIK_NUMPAD7))
		{
			Process_Command();
		}
	}
}

void CQTE_Continuous_Attack::Process_Command()
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

	m_pContinuous_Space->Process_Command();

	m_pMain_Camera->StartCameraShake(0.05f, 0.01f);

	//Gauge에 신호를 보내서 연타를 해야함
	m_pContinuous_Gauge->Process_Command();

	m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::QTE_SAMEGRAB_SUCCESS_SFX, false, 0.7f);
}

void CQTE_Continuous_Attack::Update_Animation(_float fTimeDelta)
{
	// 현재 진행 시간 업데이트
	m_fCurrentTime += fTimeDelta;

	if (m_bIsMovingDown)
	{
		// 내려가는 애니메이션 진행
		_float fProgress = m_fCurrentTime / m_fMoveDownTime;

		// 내려가기 완료, 올라가기 시작
		if (fProgress >= 1.0f)
		{
			fProgress = 1.0f;
			m_bIsMovingDown = false;
			m_fCurrentTime = 0.0f;

			//파티클 생성
			CQTE_Continuous_Attack_Space_Particle::QTE_Continuous_Attack_Space_Particle_DESC Particle_SpaceDesc{};

			Particle_SpaceDesc.fX = 960.f;
			Particle_SpaceDesc.fY = 790.f;
			Particle_SpaceDesc.fSizeX = 50.f;
			Particle_SpaceDesc.fSizeY = 50.f;
			//파티클 객체 생성
			m_UISpaceParticles.push_back(static_cast<CQTE_Continuous_Attack_Space_Particle*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_QTE_Continuous_Attack_Space_Particle"), &Particle_SpaceDesc)));
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

void CQTE_Continuous_Attack::Late_Update(_float fTimeDelta)
{
	//활성화
	if (m_bIsQTEActive)
	{
		m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
		m_pContinuous_Space->Late_Update(fTimeDelta);
		m_pContinuous_Gauge->Late_Update(fTimeDelta);
		for (auto& iter : m_UIParticles)
			iter->Late_Update(fTimeDelta);
		for (auto& iter : m_UISpaceParticles)
			iter->Late_Update(fTimeDelta);
	}
	else if (m_bOffsetActive)
	{
		m_pContinuous_Effect->Late_Update(fTimeDelta);
		for (auto& iter : m_UIParticles)
			iter->Late_Update(fTimeDelta);
		for (auto& iter : m_UISpaceParticles)
			iter->Late_Update(fTimeDelta);
	}

}

HRESULT CQTE_Continuous_Attack::Render(_float fTimeDelta)
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

void CQTE_Continuous_Attack::Start(CGameObject* callObject)
{
	if (m_bIsQTEActive)
	{
		// QTE가 활성화되어 있으면 즉시 종료
		End_QTE();
	}
	else
	{
		// QTE가 비활성화되어 있으면 시작
		Start_QTE(callObject);
	}
}

HRESULT CQTE_Continuous_Attack::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_QTE_VtxRect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_QTE_Arrow"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CQTE_Continuous_Attack::Bind_ShaderResources()
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



CQTE_Continuous_Attack* CQTE_Continuous_Attack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CQTE_Continuous_Attack* pInstance = new CQTE_Continuous_Attack(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CQTE_Continuous_Attack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CQTE_Continuous_Attack::Clone(void* pArg)
{
	CQTE_Continuous_Attack* pInstance = new CQTE_Continuous_Attack(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CQTE_Continuous_Attack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CQTE_Continuous_Attack::Free()
{
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pContinuous_Space);
	Safe_Release(m_pContinuous_Gauge);
	Safe_Release(m_pContinuous_Effect);

	for (auto& iter : m_UIParticles)
		Safe_Release(iter);

	for (auto& iter : m_UISpaceParticles)
		Safe_Release(iter);

	__super::Free();
}
