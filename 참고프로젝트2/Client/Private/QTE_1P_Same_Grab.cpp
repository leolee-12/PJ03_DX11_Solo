#include "stdafx.h"
#include "..\Public\QTE_1P_Same_Grab.h"

#include "RenderInstance.h"
#include "GameInstance.h"
#include "QTE_Same_Grab_UI_Icon.h"
#include "QTE_Same_Grab_UI_Gauge.h"
#include "Main_Camera.h"
#include "QTE_Same_Grab_UI_Particle.h"

CQTE_1P_Same_Grab::CQTE_1P_Same_Grab(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CQTE_1P_Same_Grab::CQTE_1P_Same_Grab(const CQTE_1P_Same_Grab& Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CQTE_1P_Same_Grab::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CQTE_1P_Same_Grab::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	srand(static_cast<_uint>(time(nullptr))); // 랜덤 시드 초기화

	return S_OK;
}

void CQTE_1P_Same_Grab::Camera_Update(_float fTimeDelta)
{

}

void CQTE_1P_Same_Grab::Update(_float fTimeDelta)
{
#pragma region 디버그
	//// F5 키 입력 감지
	//if (m_pGameInstance->Key_Down(DIK_F6))
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

#pragma region QTE 종료 프로세스 진행 중인 경우

	// QTE 종료 프로세스 진행 중인 경우
	if (m_bIsEndQTE)
	{
		m_fEndQTE_Timer += fTimeDelta;
		if (m_fEndQTE_Timer >= m_fEndQTE_Delay)
		{
			Final_End_QTE();
		}

		// Ascend 애니메이션 업데이트
		for (auto& iter : m_UIIcons)
			iter->Update(fTimeDelta);
		for (auto& iter : m_UIParticles)
			iter->Update(fTimeDelta);
	}
#pragma endregion

#pragma region QTE 활성화

	else if (m_bIsQTEActive)
	{
		// UI 아이콘 업데이트
		for (auto& iter : m_UIIcons)
			iter->Update(fTimeDelta);
		for (auto& iter : m_UIParticles)
			iter->Update(fTimeDelta);

		//마지막 UI가 떨어져야 그때부터 시작임
		if (m_bUI_Final_Complate)
		{
			if (m_UIGauge != nullptr)
				m_UIGauge->Update(fTimeDelta);

			// QTE가 활성화된 경우 기존 로직 유지
			// 타이머 감소
			m_fTimer -= fTimeDelta;

			// 시간 조건 확인
			if (m_fTimer <= 0.0f || m_iCorrectInputs == m_iSequenceLength)
				End_QTE();

			// 사용자 입력 처리
			Handle_QTEInput();

			// 쿨다운 타이머 감소
			if (m_fCooldown > 0.0f)
				m_fCooldown -= fTimeDelta;
		}
	}
#pragma endregion
}

void CQTE_1P_Same_Grab::Late_Update(_float fTimeDelta)
{
	if (m_bIsQTEActive)
	{
		for (auto& iter : m_UIIcons)
			iter->Late_Update(fTimeDelta);
		for (auto& iter : m_UIParticles)
			iter->Late_Update(fTimeDelta);

		if (m_UIGauge != nullptr && m_bUI_Final_Complate)
			m_UIGauge->Late_Update(fTimeDelta);
	}
}

void CQTE_1P_Same_Grab::Start(CGameObject* callObject, _float _lifeTime)
{
	if (m_bIsQTEActive)
	{
		// QTE가 활성화되어 있으면 즉시 종료
		End_QTE();
	}
	else
	{
		Start_QTE(callObject, _lifeTime);
	}
}

void CQTE_1P_Same_Grab::Start_QTE(CGameObject* callObject, _float _lifeTime)
{
	if (m_bIsQTEActive)
		return; // 이미 QTE가 활성화되어 있으면 무시

	//카메라 쉐이킹용으로 필요함
	m_pMain_Camera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
	//캐릭터 멤버 변수 대입
	m_pCharacter = static_cast<CCharacter*>(callObject);
	m_iCharacterTeam = m_pCharacter->Get_iPlayerTeam();

	//기존 객체들 전부 삭제
	Clear_UIIcons();

	m_bIsQTEActive = true;

	//lifeTime을 따로 넣어주지 않았으면 갱신 X
	if (_lifeTime != -1.f)
		m_iTotalTime = _lifeTime;

	m_fTimer = static_cast<_float>(m_iTotalTime);

	// 1P 초기화
	// Queue 다 뽑아내기
	while (!m_CommandQueue.empty()) m_CommandQueue.pop();
	// vector 다 뽑아내기
	m_CurrentSequence.clear();
	// 1P가 얼마나 맞췄는지 Input 초기화
	m_iCorrectInputs = 0;

	vector<UI_COMMAND> sequence;
	for (_int i = 0; i < m_iSequenceLength; ++i)
	{
		UI_COMMAND cmd = static_cast<UI_COMMAND>(rand() % UI_COMMAND_END);
		m_CommandQueue.push(cmd);
		m_CurrentSequence.push_back(cmd);
		sequence.push_back(cmd);
	}

	// UI 아이콘 생성
	Create_UIIcons(sequence);
}

void CQTE_1P_Same_Grab::End_QTE()
{
	// Ascend 애니메이션 시작
	Ascend_UIIcons(m_UIIcons);

	// QTE 종료 프로세스 시작 표시
	m_bIsEndQTE = true;
	m_fEndQTE_Timer = 0.0f;

	//결과 판정
	_bool isTimerEnded = (m_fTimer <= 0.0f);
	_bool isCompleted = (m_iCorrectInputs == m_iSequenceLength);

	if (isTimerEnded)
		//시간 지나서 패배
		m_pCharacter->Notify_QTE_1p_Grab(-1);
	else if (isCompleted)
		//성공
		m_pCharacter->Notify_QTE_1p_Grab(1);

	m_pCharacter = nullptr;
	m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::QTE_SUCCESS_SFX, false, 0.7f);
}

void CQTE_1P_Same_Grab::Handle_QTEInput()
{
	if (m_fCooldown <= 0.0f)
	{
		if (m_iCharacterTeam == 1)
		{
			if (m_pGameInstance->Key_Down(DIK_U))
			{
				Process_Command(UI_COMMAND_LIGHT);
			}
			else if (m_pGameInstance->Key_Down(DIK_I))
			{
				Process_Command(UI_COMMAND_MIDDLE);
			}
			else if (m_pGameInstance->Key_Down(DIK_J))
			{
				Process_Command(UI_COMMAND_ULTIMATE);
			}
			else if (m_pGameInstance->Key_Down(DIK_K))
			{
				Process_Command(UI_COMMAND_HEAVY);
			}
		}
		else if (m_iCharacterTeam == 2)
		{
			if (m_pGameInstance->Key_Down(DIK_NUMPAD7))
			{
				Process_Command(UI_COMMAND_LIGHT);
			}
			else if (m_pGameInstance->Key_Down(DIK_NUMPAD8))
			{
				Process_Command(UI_COMMAND_MIDDLE);
			}
			else if (m_pGameInstance->Key_Down(DIK_NUMPAD4))
			{
				Process_Command(UI_COMMAND_ULTIMATE);
			}
			else if (m_pGameInstance->Key_Down(DIK_NUMPAD5))
			{
				Process_Command(UI_COMMAND_HEAVY);
			}
		}
	}
}

void CQTE_1P_Same_Grab::Process_Command(UI_COMMAND input)
{
	if (m_CommandQueue.empty())
		return; // 모든 명령을 이미 처리한 경우

	UI_COMMAND expected = m_CommandQueue.front();
	//정확한 Input을 던졌을 때
	if (input == expected)
	{
		m_CommandQueue.pop();
		m_iCorrectInputs++;

		// 현재 선택된 아이콘의 선택 상태 해제
		if (m_CurrentIndex < m_UIIcons.size())
		{
			CQTE_Same_Grab_UI_Particle::QTE_Same_Grab_UI_Particle_DESC Desc{};
			_float offsetY = -50.f;
			Desc.fX = m_UIIcons[m_CurrentIndex]->m_fX;
			Desc.fY = m_UIIcons[m_CurrentIndex]->m_fY + offsetY;
			Desc.fSizeX = 50.f;
			Desc.fSizeY = 50.f;

			CQTE_Same_Grab_UI_Particle* particle = static_cast<CQTE_Same_Grab_UI_Particle*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_QTE_Same_Grab_UI_Particle"), &Desc));
			m_UIParticles.push_back(particle);

			m_UIIcons[m_CurrentIndex]->Set_State(CQTE_Same_Grab_UI_Icon::ALREADY_PRESSED);
			m_CurrentIndex++;
		}

		// 다음 아이콘을 선택 상태로 설정
		if (m_CurrentIndex < m_UIIcons.size())
		{
			m_UIIcons[m_CurrentIndex]->Set_State(CQTE_Same_Grab_UI_Icon::SELECTED);
		}

		// 모든 명령을 완료한 경우 QTE 종료 (선택 사항)
		if (m_CommandQueue.empty())
		{
			//EndQTE();
		}

		m_pMain_Camera->StartCameraShake(0.1, 0.1);
		m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::QTE_SAMEGRAB_SUCCESS_SFX, false, 0.7f);

	}
	else
	{
		// 잘못된 입력 시 QTE 실패 처리
		Handle_WrongInput();
		m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::QTE_SAMEGRAB_FAIL_SFX, false, 0.7f);
		return;
	}
}

void CQTE_1P_Same_Grab::Create_UIIcons(const vector<UI_COMMAND>& sequence)
{
#pragma region 여러 UI 객체 생성
	// 각 플레이어의 UI 아이콘 벡터에 추가

	// 플레이어 ID와 1P의 방향에 따라 centerX 설정
	_float centerX = 960.f;

	// y 위치 고정
	_float centerY = 300.f;

	// 아이콘 간격 설정 (필요에 따라 조정)
	_float spacing = 50.f;

	// 아이콘 수
	_int numIcons = static_cast<_int>(sequence.size());

	// 아이콘의 x 위치를 저장할 벡터
	vector<_float> iconPositionsX;

	// 홀수 개의 아이콘 배치
	if (numIcons % 2 == 1)
	{
		_int mid = numIcons / 2;
		for (_int i = 0; i < numIcons; ++i)
		{
			_float offset = spacing * (i - mid);
			iconPositionsX.push_back(centerX + offset);
		}
	}
	// 짝수 개의 아이콘 배치
	else
	{
		_int mid = numIcons / 2;
		for (_int i = 0; i < numIcons; ++i)
		{
			// 짝수일 경우 중앙 두 아이콘을 기준으로 좌우 배치
			_float offset = spacing * (i - mid + 0.5f);
			iconPositionsX.push_back(centerX + offset);
		}
	}

	// 떨어지기 시작하는 지연 시간 간격 설정 (초)
	const _float FALL_DELAY_INTERVAL = 0.1f; // 필요에 따라 조정 가능

	// 각 아이콘을 생성하고 위치 설정
	for (_int i = 0; i < numIcons; ++i)
	{
		CQTE_Same_Grab_UI_Icon::QTE_Same_Grab_UI_ICON_DESC Desc{};
		Desc.iTextureNumber = static_cast<_int>(sequence[i]);
		Desc.fSizeX = 50.f; // 필요에 따라 크기 조정
		Desc.fSizeY = 50.f;
		Desc.fX = iconPositionsX[i];
		Desc.fY = centerY;
		Desc.fAlpha = 1.f;
		Desc.fFallDelay = i * FALL_DELAY_INTERVAL; // 각 아이콘의 떨어지기 시작하는 지연 시간 설정
		Desc.SameGrab = this;

		//첫번째 녀석은 떨어지는 위치가 달라야함
		if (i == 0)
			Desc.isFirst = true;

		//마지막 녀석이 떨어지고 SameGrab에게 비로소 시작되었다고 알려야함
		if (i == numIcons - 1)
			Desc.isLast = true;

		// UI 아이콘 클론
		CQTE_Same_Grab_UI_Icon* pIcon = dynamic_cast<CQTE_Same_Grab_UI_Icon*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_QTE_Same_Grab_UI_Icon"), &Desc));
		if (pIcon)
		{
			// 벡터에 저장
			m_UIIcons.push_back(pIcon);
		}
	}
#pragma endregion

	// 기존의 게이지 객체 해제
	if (m_UIGauge != nullptr)
	{
		Safe_Release(m_UIGauge);
		m_UIGauge = nullptr;
	}

#pragma region 게이지 객체 생성
	CQTE_Same_Grab_UI_Gauge::QTE_UI_Gauge_DESC Desc{};
	Desc.fSizeX = 400.f;
	Desc.fSizeY = 20.f;
	Desc.fX = 960.f;
	Desc.fY = 810.f;
	Desc.playTime = m_iTotalTime;
	m_UIGauge = dynamic_cast<CQTE_Same_Grab_UI_Gauge*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_QTE_Same_Grab_UI_Gauge"), &Desc));
#pragma endregion
}

void CQTE_1P_Same_Grab::Ascend_UIIcons(vector<CQTE_Same_Grab_UI_Icon*>& icons)
{
	// 아이콘을 역순으로 처리하여 마지막 아이콘부터 ASCEND 상태로 전환
	const float ASCEND_DELAY_INTERVAL = 0.1f; // 각 아이콘 간의 상승 시작 지연 시간 (초)
	for (int i = static_cast<int>(icons.size()) - 1; i >= 0; --i)
	{
		CQTE_Same_Grab_UI_Icon* pIcon = icons[i];
		if (pIcon)
		{
			// ASCEND 상태로 전환하면서 지연 시간을 설정
			float ascendDelay = (icons.size() - 1 - i) * ASCEND_DELAY_INTERVAL;
			pIcon->Set_State(CQTE_Same_Grab_UI_Icon::ASCEND);
			pIcon->Set_AscendDelay(ascendDelay);
		}
	}
}

void CQTE_1P_Same_Grab::Final_End_QTE()
{
	// 초기화 작업 수행
	m_bIsEndQTE = false;
	m_bIsQTEActive = false;
	m_fTimer = 0.0f;
	m_bUI_Final_Complate = false;

	// 큐와 시퀀스 초기화
	while (!m_CommandQueue.empty()) m_CommandQueue.pop();
	m_CurrentSequence.clear();
	m_iCorrectInputs = 0;
	m_CurrentIndex = 0;

	// UI 아이콘 제거
	Clear_UIIcons();
}



void CQTE_1P_Same_Grab::Clear_UIIcons()
{
	// 1P UI 아이콘 제거
	for (auto& icon : m_UIIcons)
	{
		if (icon)
		{
			Safe_Release(icon);
		}
	}
	m_UIIcons.clear();

	for (auto& icon : m_UIParticles)
	{
		if (icon)
		{
			Safe_Release(icon);
		}
	}
	m_UIParticles.clear();

	//Gauge 제거
	Safe_Release(m_UIGauge);
}

void CQTE_1P_Same_Grab::Handle_WrongInput()
{
	if (m_CurrentIndex < m_UIIcons.size())
	{
		m_UIIcons[m_CurrentIndex]->Set_State(CQTE_Same_Grab_UI_Icon::WRONG_PRESSED);
	}

	m_fCooldown = COOLDOWN_DURATION;
}


HRESULT CQTE_1P_Same_Grab::Render(_float fTimeDelta)
{
	return S_OK;
}

CQTE_1P_Same_Grab* CQTE_1P_Same_Grab::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CQTE_1P_Same_Grab* pInstance = new CQTE_1P_Same_Grab(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CQTE_1P_Same_Grab"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CQTE_1P_Same_Grab::Clone(void* pArg)
{
	CQTE_1P_Same_Grab* pInstance = new CQTE_1P_Same_Grab(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CQTE_1P_Same_Grab"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CQTE_1P_Same_Grab::Free()
{
	Clear_UIIcons();

	__super::Free();
}
