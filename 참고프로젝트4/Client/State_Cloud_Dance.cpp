#include "stdafx.h"
#include "State_list_Cloud.h"
#include "Sound_Manager.h"
#include "UI_MiniGame.h"
#include "Light_Manager.h"

CState_Cloud_Dance::CState_Cloud_Dance(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Cloud(pActor, pStatemachine)
{
}

HRESULT CState_Cloud_Dance::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pGameInstance->Reset_Light();
	m_pGameInstance->Add_Light("../Bin/Data/Light_Data/Village2/MiniGame_Start.json");

	m_iDigitType = 0;
	m_iDanceNameNum = 900;
	m_bDanceFinished = false;
	
	GET_SINGLE(CUI_Manager)->Start_MiniGame();
	m_pGameInstance->Stop_SoundAll();
	m_pGameInstance->Play_BGM(TEXT("FF7_BGM_Slu5b"),L"bgm_slu5b_wm_3562_3 (Music-dance_cinema_master).mp3",
		1.f);

	m_pActor_ModelCom.lock()->Set_RootTransBone(L"C_Hip_a");
	
	m_pActor_ModelCom.lock()->Set_Animation(m_strDanceName[m_iDigitType] + to_string(m_iDanceNameNum), 1.0f, false, true, 8.f);
	m_pActor.lock()->Get_PhysXControllerCom().lock()->Enable_Gravity(false);

	m_pActor_TransformCom.lock()->Set_Position(1.f, _float3{ -34.0f, 1.3f, 34.569f });
	m_pActor_TransformCom.lock()->Set_Look(_float3(0.f, 0.f, -1.f));

	m_pActor.lock()->Find_PartObject(L"Part_Weapon")->TurnOff_State(OBJSTATE::Tick);
	m_pActor.lock()->Find_PartObject(L"Part_Weapon")->TurnOff_State(OBJSTATE::Render);

	GET_SINGLE(CCamera_Manager)->Call_CutSceneCamera("Minigame_test13", (_int)CCamera::tagCameraMotionData::INTERPOLATION_TYPE::INTER_NONE, nullptr, 0.f);

	return S_OK;
}

void CState_Cloud_Dance::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
		
	if(!m_bDanceFinished)
	{	
		/* 버튼 생성 */
		m_fMakeButtonTimeAcc += fTimeDelta;
		if (m_fMakeButtonTimeAcc >= m_fMakeButtonTime)
		{
			GET_SINGLE(CUI_Manager)->Make_MiniGameButton(8.f);
			m_fMakeButtonTime = Random({ 0.5f,1.f,1.2f,1.5f,2.f });
			m_fMakeButtonTimeAcc = 0.f;
		}

		if (GET_SINGLE(CUI_Manager)->m_pMiniGame->Get_NowResult() == CUI_MiniGame::M_Bad)
		{
			auto pAnimationComp = m_pActor_ModelCom.lock()->AnimationComp();
			pAnimationComp->Set_ADD_Animation(1, L"Battle00|B_Dmg01_Add", L"C_Hip_a", { L"C_Spine_d" }
			, 4, 1.f, 4.f, false);
			GET_SINGLE(CUI_Manager)->m_pMiniGame->Reset_NowResult();
		}

		if (m_pActor_ModelCom.lock()->IsCurrentAnimation("EV_SLU5B_3562|EV_SLU5B_3562_PC0000_00_C1120"))
		{			
			m_pGameInstance->Reset_Light();
			m_pGameInstance->Add_Light("../Bin/Data/Light_Data/Village2/MiniGame_Finish.json");
			m_bDanceFinished = true;
		}
		else if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		{
			m_iDanceNameNum += 10;
			m_iDanceNameNum / 1000 < 1 ? m_iDigitType = 0 : m_iDigitType = 1;

			m_pActor_ModelCom.lock()->Set_Animation(m_strDanceName[m_iDigitType] + to_string(m_iDanceNameNum), 1.0f, false, true, 8.f);

			if (m_iDanceNameNum == 910)
			{
				m_pGameInstance->Reset_Light();
				m_pGameInstance->Add_Light("../Bin/Data/Light_Data/Village2/MiniGame_0.json");
			}
			else if (m_iDanceNameNum == 950)
			{
				m_pGameInstance->Reset_Light();
				m_pGameInstance->Add_Light("../Bin/Data/Light_Data/Village2/MiniGame_1.json");
			}
			else if (m_iDanceNameNum >= 1000)
			{
				m_pGameInstance->Reset_Light();
				_int m_iLightVer = Random({ 0,0,0,1,1 });
				switch (m_iLightVer)
				{
				case 0:
					m_pGameInstance->Add_Light("../Bin/Data/Light_Data/Village2/MiniGame_2.json");
					break;
				case 1:
					m_pGameInstance->Add_Light("../Bin/Data/Light_Data/Village2/MiniGame_3.json");
					break;
				}
			}			
		}
	}

	if (m_pActor_ModelCom.lock()->IsCurrentAnimation("EV_SLU5B_3562|EV_SLU5B_3562_PC0000_00_C1120"))
	{
		if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		{
			m_pActor_ModelCom.lock()->Set_Animation("EV_SLU5B_3563|EV_SLU5B_3563_PC0000_00_C1190", 1.0f, false, true, 8.f);

			m_pGameInstance->Reset_Light();
			m_pGameInstance->Add_Light("../Bin/Data/Light_Data/Village2/MiniGame_Finish.json");

			GET_SINGLE(CUI_Manager)->Open_MiniGameResultWindow();
		}
	}
	
	if (m_pGameInstance->Key_Down(DIK_0))
		m_pStateMachineCom.lock()->Set_State<CState_Cloud_Idle>();

}

void CState_Cloud_Dance::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Cloud_Dance::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Cloud_Dance::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);

	m_pActor_ModelCom.lock()->Set_RootTransBone();
	m_pActor.lock()->Get_PhysXControllerCom().lock()->Enable_Gravity(true);

	m_pActor.lock()->Find_PartObject(L"Part_Weapon")->TurnOn_State(OBJSTATE::Tick);
	m_pActor.lock()->Find_PartObject(L"Part_Weapon")->TurnOn_State(OBJSTATE::Render);

	GET_SINGLE(CUI_Manager)->Stop_MiniGame();
	GET_SINGLE(CUI_Manager)->Open_MiniGameResultWindow();


	m_pGameInstance->Reset_Light();
	m_pGameInstance->Add_Light("../Bin/Data/Light_Data/Village2/Village2_Fin3.json");
	m_pGameInstance->Stop_BGM();
}

bool CState_Cloud_Dance::isValid_NextState(CState* state)
{
	return true;
}

void CState_Cloud_Dance::Free()
{
}
