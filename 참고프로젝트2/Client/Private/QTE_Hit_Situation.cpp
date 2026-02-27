#include "stdafx.h"
#include "..\Public\QTE_Hit_Situation.h"

#include "RenderInstance.h"
#include "GameInstance.h"
#include "QTE_Hit_UI_Icon.h"
#include "QTE_Hit_UI_Result.h"
#include "QTE_Hit_UI_Particle.h"
#include "Main_Camera.h"
#include "Sound_Manager.h"
CQTE_Hit_Situation::CQTE_Hit_Situation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CQTE_Hit_Situation::CQTE_Hit_Situation(const CQTE_Hit_Situation& Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CQTE_Hit_Situation::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CQTE_Hit_Situation::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//랜덤시드 발생
	srand(static_cast<unsigned>(std::time(0)));

	QTE_HIT_SITUATION_DESC* Desc = static_cast<QTE_HIT_SITUATION_DESC*>(pArg);
	//해당 상황의 전체 라이프타임
	m_fLifeTime = Desc->lifeTime;
	m_fTimer = m_fLifeTime;

	//해당 상황의 만들어낼 숫자
	m_iCreate_Num = Desc->create_Num;
	//해당 상황 ID
	m_currentSituationID = Desc->ID;

	//초기화
	m_fElapsedTime = 0.0f;
	m_iNextIconIndex = 0;

	switch (m_currentSituationID)
	{
	case Client::CQTE_Hit::Hit_Situation_ID_Goku:
		//m_iCreate_Num - 1개가 나와야함
		m_vecIconCreationTimes = { 1.f, 1.f };
		break;
	case Client::CQTE_Hit::Hit_Situation_ID_21:
		//m_iCreate_Num - 1개가 나와야함
		m_vecIconCreationTimes = { 1.f, 1.f };
		break;
	case Client::CQTE_Hit::Hit_Situation_ID_Frieza:
		//m_iCreate_Num - 1개가 나와야함
		m_vecIconCreationTimes = { 1.f, 1.f };
		break;
	case Client::CQTE_Hit::Hit_Situation_ID_Hit:
		//m_iCreate_Num - 1개가 나와야함
		m_vecIconCreationTimes = { 1.f, 1.f };
		break;
	}

	return S_OK;
}

void CQTE_Hit_Situation::Camera_Update(_float fTimeDelta)
{

}

void CQTE_Hit_Situation::Update(_float fTimeDelta)
{
#pragma region 디버그
	//// F5 키 입력 감지
	//if (m_pGameInstance->Key_Down(DIK_F5))
	//{
	//	if (m_bIsQTEActive)
	//	{
	//		// QTE가 활성화되어 있으면 즉시 종료
	//		End_QTE();

	//		// UI 객체 삭제 시간
	//		for (auto& iter : m_vecHitUIIcon)
	//			Safe_Release(iter);

	//		for (auto& iter : m_vecHitResult)
	//			Safe_Release(iter);

	//		for (auto& iter : m_vecHitParticle)
	//			Safe_Release(iter);

	//		m_vecHitUIIcon.clear();
	//		m_vecHitResult.clear();
	//		m_vecHitParticle.clear();

	//		m_fOffsetTimer = 0.f;
	//		m_bOffsetActive = false; // 오프셋 기간 종료

	//	}
	//	else
	//	{
	//		// QTE가 비활성화되어 있으면 시작
	//		// 첫번째 아이콘을 즉각 만들어버림
	//		Start_QTE();
	//	}
	//}
#pragma endregion

#pragma region 활성화
	if (m_bIsQTEActive)
	{
		//전체 타이머가 전부 시간 소요되거나
		//마지막 UI 객체 처리가 완료됬거나
		if (m_fTimer <= 0.0f || m_bUI_Final_Complete)
		{
			// QTE 종료
			End_QTE();
		}

		// 사용자 입력 처리
		Handle_QTEInput();

		// 각 Hit_UI_Icon 업데이트
		for (auto& iter : m_vecHitUIIcon)
			iter->Update(fTimeDelta);

		for (auto& iter : m_vecHitResult)
			iter->Update(fTimeDelta);

		for (auto& iter : m_vecHitParticle)
			iter->Update(fTimeDelta);

		// 다음 아이콘 생성할게 남아있는지 체크
		if (m_iNextIconIndex < m_vecIconCreationTimes.size())
		{
			//경과시간이 딜레이 시간보다 늘어났다면 아이콘 생성
			if (m_fElapsedTime >= m_vecIconCreationTimes[m_iNextIconIndex])
			{
				// 아이콘 생성
				Create_UIIcon();
				m_iNextIconIndex++;
				m_fElapsedTime = 0.f;
			}
		}

		// 경과 시간 업데이트
		m_fElapsedTime += fTimeDelta;

		// 타이머 업데이트
		m_fTimer -= fTimeDelta;
	}
#pragma endregion

#pragma region Offset 기간 활성화

	//offSet 기간 처리
	else if (m_bOffsetActive)
	{
		End_Offset_QTE(fTimeDelta);
	}
#pragma endregion
}

void CQTE_Hit_Situation::Late_Update(_float fTimeDelta)
{
	//QTE가 활성화되었다면
	if (m_bIsQTEActive || m_bOffsetActive)
	{
		for (auto& iter : m_vecHitUIIcon)
			iter->Late_Update(fTimeDelta);

		for (auto& iter : m_vecHitResult)
			iter->Late_Update(fTimeDelta);

		for (auto& iter : m_vecHitParticle)
			iter->Late_Update(fTimeDelta);
	}
}

HRESULT CQTE_Hit_Situation::Render(_float fTimeDelta)
{
	return S_OK;
}

void CQTE_Hit_Situation::Notify_Faild_Result(CQTE_Hit_UI_Icon* icon)
{
	Create_ResultObject(icon);
}

void CQTE_Hit_Situation::Start(CGameObject* pCall_Object)
{
	if (m_bIsQTEActive)
	{
		// QTE가 활성화되어 있으면 즉시 종료
		End_QTE();

		// UI 객체 삭제 시간
		for (auto& iter : m_vecHitUIIcon)
			Safe_Release(iter);

		for (auto& iter : m_vecHitResult)
			Safe_Release(iter);

		for (auto& iter : m_vecHitParticle)
			Safe_Release(iter);

		m_vecHitUIIcon.clear();
		m_vecHitResult.clear();
		m_vecHitParticle.clear();

		m_fOffsetTimer = 0.f;
		m_bOffsetActive = false; // 오프셋 기간 종료

	}
	else
	{
		// QTE가 비활성화되어 있으면 시작
		// 첫번째 아이콘을 즉각 만들어버림
		Start_QTE(pCall_Object);
	}
}

void CQTE_Hit_Situation::Start_QTE(CGameObject* pCall_Object)
{
	if (m_bIsQTEActive)
		return; // 이미 QTE가 활성화되어 있으면 무시

	//디버그용
	if (pCall_Object == nullptr)
	{
		//1p로 받기
		m_pCall_Object = m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Character"));
	}
	else
		m_pCall_Object = pCall_Object;

	m_iCharacterSide = static_cast<CCharacter*>(m_pCall_Object)->Get_iPlayerTeam();

	//카메라 쉐이킹용으로 필요함
	m_pMain_Camera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));

	//활성화
	m_bIsQTEActive = true;
	//마지막 객체 완료 처리 여부 초기화
	m_bUI_Final_Complete = false;
	// 첫 번째 아이콘 즉시 생성
	Create_UIIcon();
}

void CQTE_Hit_Situation::End_QTE()
{
#pragma region 점수 산정
	_bool isSuccess = true;

	for (auto& iter : m_vecHitUIIcon)
	{
		//FAIL이거나 결정되지 않았거나(예외처리) 하나라도 있으면 성공 실패
		if (iter->m_currentResult_ID == CQTE_Hit_UI_Icon::RESULT_ID::HIT_RESULT_FAILED ||
			iter->m_currentResult_ID == CQTE_Hit_UI_Icon::RESULT_ID::HIT_RESULT_NOT_YET_DECIDED)
		{
			isSuccess = false;
			break;
		}
	}

	// isSuccess를 int로 변환 (true -> 1, false -> -1)
	_int hitResult = isSuccess ? 1 : -1;
	// 결과를 전달
	static_cast<CCharacter*>(m_pCall_Object)->Notify_QTE_Hit(hitResult);

#pragma endregion

#pragma region 초기화
	//활성화 여부 초기화
	m_bIsQTEActive = false;

	// 오프셋 타이머 시작
	m_bOffsetActive = true;
	m_fOffsetTimer = 2.f; // 원하는 오프셋 지속 시간 설정

	//경과시간 초기화
	m_fElapsedTime = 0.0f;
	//다음 객체를 만드는데 필요한 변수 초기화
	m_iNextIconIndex = 0;
	//타이머는 원래 LifeTime으로 초기화
	m_fTimer = m_fLifeTime;
	//마지막 객체 완료 처리 여부 초기화
	m_bUI_Final_Complete = false;
	m_pCall_Object = nullptr;
#pragma endregion

}

void CQTE_Hit_Situation::End_Offset_QTE(_float fTimeDelta)
{
	// 오프셋 기간 처리
	m_fOffsetTimer -= fTimeDelta;

	// 페이드 아웃 효과를 위한 UI 객체 업데이트
	for (auto& iter : m_vecHitUIIcon)
		iter->Update(fTimeDelta);

	for (auto& iter : m_vecHitResult)
		iter->Update(fTimeDelta);

	for (auto& iter : m_vecHitParticle)
		iter->Update(fTimeDelta);

	if (m_fOffsetTimer <= 0.0f)
	{
		// UI 객체 삭제 시간
		for (auto& iter : m_vecHitUIIcon)
			Safe_Release(iter);

		for (auto& iter : m_vecHitResult)
			Safe_Release(iter);

		for (auto& iter : m_vecHitParticle)
			Safe_Release(iter);

		m_vecHitUIIcon.clear();
		m_vecHitResult.clear();
		m_vecHitParticle.clear();

		m_fOffsetTimer = 0.f;
		m_bOffsetActive = false; // 오프셋 기간 종료
	}
}

void CQTE_Hit_Situation::Create_UIIcon()
{
	CQTE_Hit_UI_Icon::QTE_Hit_UI_ICON_DESC Desc{};
	Desc.fSizeX = { 100.f };
	Desc.fSizeY = { 100.f };

	// 위치 범위 설정
	// 이 범위 내에서 확률적으로 뜰 것
	_float minX = 480.f;
	_float maxX = 1440.f; // 화면 너비 - 아이콘 너비를 고려
	_float minY = 300.f;
	_float maxY = 780.f; // 화면 높이 - 아이콘 높이를 고려

#pragma region 겹치지 않는 위치를 찾기 위한 최대 시도 횟수

	const _int maxAttempts = 100;
	_int attempts = 0;
	_bool positionFound = false;

	while (!positionFound && attempts < maxAttempts)
	{
		// 랜덤 위치 생성
		Desc.fX = minX + static_cast<_float>(rand()) / RAND_MAX * (maxX - minX);
		Desc.fY = minY + static_cast<_float>(rand()) / RAND_MAX * (maxY - minY);

		// 아이콘이 겹치는지 검사
		positionFound = true;
		for (auto& existingIcon : m_vecHitUIIcon)
		{
			// 기존 아이콘의 위치와 크기 가져오기
			_float existingX = existingIcon->m_fX;
			_float existingY = existingIcon->m_fY;
			_float existingSizeX = existingIcon->m_fSizeX * 3.f;
			_float existingSizeY = existingIcon->m_fSizeY * 3.f;

			// AABB 충돌 검사
			if (Desc.fX < existingX + existingSizeX &&
				Desc.fX + Desc.fSizeX * 3.f > existingX &&
				Desc.fY < existingY + existingSizeY &&
				Desc.fY + Desc.fSizeY * 3.f > existingY)
			{
				// 겹침 발생
				positionFound = false;
				break;
			}
		}
		attempts++;
	}

	if (!positionFound)
	{
		// 위치를 찾지 못한 경우 기본 위치 설정 또는 처리
		Desc.fX = 960.f - Desc.fSizeX / 2; // 화면 중앙 등
		Desc.fY = 540.f - Desc.fSizeY / 2;
	}

#pragma endregion


	// fTimer를 최소 및 최대 값 사이에서 랜덤하게 설정
	_float minTimer = 1.0f; // 최소 시간
	_float maxTimer = 1.9f; // 최대 시간
	Desc.fTimer = minTimer + static_cast<_float>(rand()) / RAND_MAX * (maxTimer - minTimer);


#pragma region 키설정

	vector<CQTE_Hit_UI_Icon::KEY_ID> possibleKeys = {
		CQTE_Hit_UI_Icon::KEY_ID::HIT_KEY_LIGHT,
		CQTE_Hit_UI_Icon::KEY_ID::HIT_KEY_MEDIUM,
		CQTE_Hit_UI_Icon::KEY_ID::HIT_KEY_ULTIMATE,
		CQTE_Hit_UI_Icon::KEY_ID::HIT_KEY_HEAVY
	};


	// 랜덤하게 키 하나 생성
	for (auto& iter : m_vecHitUIIcon)
	{
		_bool isActive = iter->IsActive();
		if (!isActive)
			continue;

		CQTE_Hit_UI_Icon::KEY_ID key = iter->m_Key;

		auto it = std::find(possibleKeys.begin(), possibleKeys.end(), key);
		if (it != possibleKeys.end())
			possibleKeys.erase(it);
	}

	// 가능한 키 목록이 비어있는지 확인
	if (possibleKeys.empty())
		return;

	// 남은 키 중에서 랜덤하게 선택
	_int randomIndex = rand() % possibleKeys.size();
	Desc.key = possibleKeys[randomIndex];
	Desc.iTextureNumber = (_int)possibleKeys[randomIndex];

#pragma endregion

	Desc.Hit_Situation = this;

	// 아이콘 생성
	CQTE_Hit_UI_Icon* ui_Icon = static_cast<CQTE_Hit_UI_Icon*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_QTE_Hit_UI_Icon"), &Desc));

	m_vecHitUIIcon.push_back(ui_Icon);
}


void CQTE_Hit_Situation::Handle_QTEInput()
{
	//해당 기술을 시전하는 캐릭터
	//이건 나중에 가져와야함

	if (m_iCharacterSide == 1)
	{
		if (m_pGameInstance->Key_Down(DIK_U))
		{
			Process_Command(CQTE_Hit_UI_Icon::HIT_KEY_LIGHT);
		}
		else if (m_pGameInstance->Key_Down(DIK_I))
		{
			Process_Command(CQTE_Hit_UI_Icon::HIT_KEY_MEDIUM);
		}
		else if (m_pGameInstance->Key_Down(DIK_J))
		{
			Process_Command(CQTE_Hit_UI_Icon::HIT_KEY_ULTIMATE);
		}
		else if (m_pGameInstance->Key_Down(DIK_K))
		{
			Process_Command(CQTE_Hit_UI_Icon::HIT_KEY_HEAVY);
		}
	}
	else if (m_iCharacterSide == 2)
	{
		if (m_pGameInstance->Key_Down(DIK_NUMPAD7))
		{
			Process_Command(CQTE_Hit_UI_Icon::HIT_KEY_LIGHT);
		}
		else if (m_pGameInstance->Key_Down(DIK_NUMPAD8))
		{
			Process_Command(CQTE_Hit_UI_Icon::HIT_KEY_MEDIUM);
		}
		else if (m_pGameInstance->Key_Down(DIK_NUMPAD4))
		{
			Process_Command(CQTE_Hit_UI_Icon::HIT_KEY_ULTIMATE);
		}
		else if (m_pGameInstance->Key_Down(DIK_NUMPAD5))
		{
			Process_Command(CQTE_Hit_UI_Icon::HIT_KEY_HEAVY);
		}
	}
}


void CQTE_Hit_Situation::Process_Command(CQTE_Hit_UI_Icon::KEY_ID input)
{
	//키를 입력할 때, 현재 살아있는 QTE에 한해 작동해야 한다
	for (auto& iter : m_vecHitUIIcon)
	{
		_bool isActive = iter->IsActive();
		if (!isActive)
			continue;

		//살아있는 객체의 키
		CQTE_Hit_UI_Icon::KEY_ID key = iter->m_Key;

		//for문을 돌리면서 Input과 안맞으면 패스
		if (input != key)
			continue;
		//Input을 보내서 결과 판단하라고 함
		else
		{
			_bool isFinal = false;

			//이미 나와야할 객체가 전부 나와있고
			if (m_vecHitUIIcon.size() == m_iCreate_Num)
			{
				isFinal = true;

				for (auto& iter : m_vecHitUIIcon)
				{
					if (iter->m_Key != input)
					{
						_bool isActive = iter->IsActive();

						//Braek는 루프를 빠져나가며 if문은 루프가 아니다.
						if (isActive)
						{
							isFinal = false;
							break;
						}
					}
				}
			}

			iter->Send_Input(input, isFinal);


			//입력을 눌렀을 때는 Situation에서 Result 객체를 처리
			//그러나 입력을 안눌러서 시간이 다 지난거면 해당 아이콘 객체가 Situation 에게 Notify한다.
#pragma region Result 객체 생성

			// Result 객체 생성 함수 호출
			Create_ResultObject(iter);
			Create_ParticleObject(iter);
#pragma endregion
		}
	}
}

void CQTE_Hit_Situation::Create_ResultObject(CQTE_Hit_UI_Icon* pIcon)
{
	int iTextureNum = -1;
	switch (pIcon->m_currentResult_ID)
	{
	case CQTE_Hit_UI_Icon::HIT_RESULT_FAILED:
		m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::QTE_SAMEGRAB_FAIL_SFX, false, 0.7f);
		iTextureNum = 0;
		break;
	case CQTE_Hit_UI_Icon::HIT_RESULT_GOOD:
		iTextureNum = 1;
		m_pMain_Camera->StartCameraShake(0.1, 0.1);
		m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::QTE_SUCCESS_SFX, false, 0.7f);

		break;
	case CQTE_Hit_UI_Icon::HIT_RESULT_EXCELLENT:
		iTextureNum = 2;
		m_pMain_Camera->StartCameraShake(0.1, 0.1);
		m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::QTE_SUCCESS_SFX, false, 0.7f);
		break;
	case CQTE_Hit_UI_Icon::HIT_RESULT_PERFECT:
		iTextureNum = 3;
		m_pMain_Camera->StartCameraShake(0.1, 0.1);
		m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::QTE_SUCCESS_SFX, false, 0.7f);
		break;
	default:
		iTextureNum = 0;
		break;
	}

	CQTE_Hit_UI_Result::Hit_RESULT_DESC Desc{};
	_float OffsetY = -70.f;

	Desc.fX = pIcon->m_fX;
	Desc.fY = pIcon->m_fY + OffsetY;
	Desc.fSizeX = 300.f;
	Desc.fSizeY = 200.f;
	Desc.iTextureNum = iTextureNum;
	Desc.fTimer = 1.5f;

	CQTE_Hit_UI_Result* Result = static_cast<CQTE_Hit_UI_Result*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_QTE_Hit_UI_Result"), &Desc));
	m_vecHitResult.push_back(Result);
}

void CQTE_Hit_Situation::Create_ParticleObject(CQTE_Hit_UI_Icon* pIcon)
{
	//실패할 땐 끔
	if (pIcon->m_currentResult_ID == CQTE_Hit_UI_Icon::HIT_RESULT_FAILED)
		return;

	CQTE_Hit_UI_Particle::Hit_PARTICLE_DESC Desc{};
	Desc.fX = pIcon->m_fX;
	Desc.fY = pIcon->m_fY;
	Desc.fSizeX = 600.f;
	Desc.fSizeY = 600.f;

	CQTE_Hit_UI_Particle* effect = static_cast<CQTE_Hit_UI_Particle*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_QTE_Hit_UI_Particle"), &Desc));
	m_vecHitParticle.push_back(effect);

}

CQTE_Hit_Situation* CQTE_Hit_Situation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CQTE_Hit_Situation* pInstance = new CQTE_Hit_Situation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CQTE_Hit_Situation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CQTE_Hit_Situation::Clone(void* pArg)
{
	CQTE_Hit_Situation* pInstance = new CQTE_Hit_Situation(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CQTE_Hit_Situation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CQTE_Hit_Situation::Free()
{
	for (auto& iter : m_vecHitUIIcon)
		Safe_Release(iter);

	for (auto& iter : m_vecHitResult)
		Safe_Release(iter);

	for (auto& iter : m_vecHitParticle)
		Safe_Release(iter);

	m_vecHitUIIcon.clear();
	m_vecHitResult.clear();
	m_vecHitParticle.clear();

	__super::Free();
}
