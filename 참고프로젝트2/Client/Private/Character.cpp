#include "stdafx.h"
#include "..\Public\Character.h"

#include "RenderInstance.h"
#include "GameInstance.h"
#include "iostream"
#include "AttackObject.h"
#include "UI_Manager.h"
#include "Effect_Manager.h"

#include "AttackObject.h"
#include "BattleInterface.h"
#include "Main_Camera.h"

#include "Animation.h"
#include <cmath>
#include "SpaceMeteoBreak.h"
#include "Effect_Layer.h"
#include "Map_Manager.h"

#include "QTE_Manager.h"
#include "Particle_Manager.h"

#include "Level_GamePlay.h"
const _float CCharacter::fGroundHeight = 0.f; //0
const _float CCharacter::fJumpPower = 3.f; //0


vector<CInput> CCharacter::Command_236Attack =
{
	{MOVEKEY_DOWN, ATTACK_NONE},
	{MOVEKEY_DOWN_RIGHT, ATTACK_NONE},
	{MOVEKEY_RIGHT, ATTACK_NONE},
	{MOVEKEY_NEUTRAL, ATTACK_LIGHT}
};
vector<CInput> CCharacter::Command_236Attack_Extra =
{
	{MOVEKEY_DOWN, ATTACK_NONE},
	{MOVEKEY_DOWN_RIGHT, ATTACK_NONE},
	{MOVEKEY_RIGHT, ATTACK_NONE},
	{MOVEKEY_RIGHT, ATTACK_LIGHT}
};

vector<CInput> CCharacter::Command_236Attack_Heavy =
{
	{MOVEKEY_DOWN, ATTACK_NONE},
	{MOVEKEY_DOWN_RIGHT, ATTACK_NONE},
	{MOVEKEY_RIGHT, ATTACK_NONE},
	{MOVEKEY_NEUTRAL, ATTACK_HEAVY}
};
vector<CInput> CCharacter::Command_236Attack_Heavy_Extra =
{
	{MOVEKEY_DOWN, ATTACK_NONE},
	{MOVEKEY_DOWN_RIGHT, ATTACK_NONE},
	{MOVEKEY_RIGHT, ATTACK_NONE},
	{MOVEKEY_RIGHT, ATTACK_HEAVY}
};


vector<CInput> CCharacter::Command_214Attack =
{
	{MOVEKEY_DOWN, ATTACK_NONE},
	{MOVEKEY_DOWN_LEFT, ATTACK_NONE},
	{MOVEKEY_LEFT, ATTACK_NONE},
	{MOVEKEY_NEUTRAL, ATTACK_LIGHT}
};
vector<CInput> CCharacter::Command_214Attack_Extra =
{
	{MOVEKEY_DOWN, ATTACK_NONE},
	{MOVEKEY_DOWN_LEFT, ATTACK_NONE},
	{MOVEKEY_LEFT, ATTACK_NONE},
	{MOVEKEY_LEFT, ATTACK_LIGHT}
};

vector<CInput> CCharacter::Command_236Special =
{
	{MOVEKEY_DOWN, ATTACK_NONE},
	{MOVEKEY_DOWN_RIGHT, ATTACK_NONE},
	{MOVEKEY_RIGHT, ATTACK_NONE},
	{MOVEKEY_RIGHT, ATTACK_SPECIAL}
};

vector<CInput> CCharacter::Command_236Special_Side =
{
	{MOVEKEY_DOWN, ATTACK_NONE},
	{MOVEKEY_DOWN_RIGHT, ATTACK_NONE},
	{MOVEKEY_RIGHT, ATTACK_NONE},
	{MOVEKEY_DOWN, ATTACK_SPECIAL}
};

vector<CInput> CCharacter::Command_236Special_Side_Extra =
{
	{MOVEKEY_DOWN, ATTACK_NONE},
	{MOVEKEY_DOWN_RIGHT, ATTACK_NONE},
	{MOVEKEY_RIGHT, ATTACK_NONE},
	{MOVEKEY_DOWN, ATTACK_NONE},
	{MOVEKEY_DOWN, ATTACK_SPECIAL}
};

vector<CInput> CCharacter::Command_214Special =
{
	{MOVEKEY_DOWN, ATTACK_NONE},
	{MOVEKEY_DOWN_LEFT, ATTACK_NONE},
	{MOVEKEY_LEFT, ATTACK_NONE},
	{MOVEKEY_NEUTRAL, ATTACK_SPECIAL}
};

vector<CInput> CCharacter::Command_214Special_Extra =
{
	{MOVEKEY_DOWN, ATTACK_NONE},
	{MOVEKEY_DOWN_LEFT, ATTACK_NONE},
	{MOVEKEY_LEFT, ATTACK_NONE},
	{MOVEKEY_LEFT, ATTACK_SPECIAL}
};




vector<CInput> CCharacter::Command_214FinalAttack =
{
	{MOVEKEY_DOWN, ATTACK_NONE},
	{MOVEKEY_DOWN_LEFT, ATTACK_NONE},
	{MOVEKEY_LEFT, ATTACK_NONE},
	{MOVEKEY_LEFT, ATTACK_GRAB}
};


vector<CInput> CCharacter::Command_236UltimateAttack =
{
	{MOVEKEY_DOWN, ATTACK_NONE},
	{MOVEKEY_DOWN_RIGHT, ATTACK_NONE},
	{MOVEKEY_RIGHT, ATTACK_NONE},
	{MOVEKEY_RIGHT, ATTACK_GRAB}
};


vector<CInput> CCharacter::Command_236UltimateAttack_Side =
{
	{MOVEKEY_DOWN, ATTACK_NONE},
	{MOVEKEY_DOWN_RIGHT, ATTACK_NONE},
	{MOVEKEY_RIGHT, ATTACK_NONE},
	{MOVEKEY_DOWN, ATTACK_GRAB}
};

vector<CInput> CCharacter::Command_236UltimateAttack_Side_Extra =
{
	{MOVEKEY_DOWN, ATTACK_NONE},
	{MOVEKEY_DOWN_RIGHT, ATTACK_NONE},
	{MOVEKEY_RIGHT, ATTACK_NONE},
	{MOVEKEY_DOWN, ATTACK_NONE},
	{MOVEKEY_DOWN, ATTACK_GRAB}
};

vector<CInput> CCharacter::Command_BackDash =
{
	{MOVEKEY_LEFT, ATTACK_NONE},
	{MOVEKEY_NEUTRAL, ATTACK_NONE},
	{MOVEKEY_LEFT, ATTACK_NONE}
};

vector<CInput> CCharacter::Command_Forward =
{
	{MOVEKEY_RIGHT, ATTACK_NONE},
	{MOVEKEY_NEUTRAL, ATTACK_NONE},
	{ MOVEKEY_RIGHT, ATTACK_NONE }

};
vector<CInput> CCharacter::Command_Crouch_Crouch_SpecialAttack =
{
	{MOVEKEY_DOWN, ATTACK_NONE},
	{MOVEKEY_NEUTRAL, ATTACK_NONE},
	{MOVEKEY_DOWN, ATTACK_NONE},
	{MOVEKEY_DOWN, ATTACK_SPECIAL},

};



vector<CInput> CCharacter::Command_LightAttack = { {MOVEKEY_NEUTRAL, ATTACK_LIGHT} };
vector<CInput> CCharacter::Command_MediumAttack = { {MOVEKEY_NEUTRAL, ATTACK_MEDIUM} };

vector<CInput> CCharacter::Command_MediumAttack_Extra = { {MOVEKEY_RIGHT, ATTACK_MEDIUM} };

vector<CInput> CCharacter::Command_HeavyAttack = { {MOVEKEY_NEUTRAL, ATTACK_HEAVY} };
vector<CInput> CCharacter::Command_SpecialAttack = { {MOVEKEY_NEUTRAL, ATTACK_SPECIAL} };

vector<CInput> CCharacter::Command_HeavyAttack_Extra = { {MOVEKEY_RIGHT, ATTACK_HEAVY} };

vector<CInput> CCharacter::Command_Grab = { {MOVEKEY_NEUTRAL, ATTACK_GRAB} };


vector<CInput> CCharacter::Command_Crouch_LightAttack = { {MOVEKEY_DOWN, ATTACK_LIGHT} };
vector<CInput> CCharacter::Command_Crouch_MediumAttack = { {MOVEKEY_DOWN, ATTACK_MEDIUM} };
vector<CInput> CCharacter::Command_Crouch_HeavyAttack = { {MOVEKEY_DOWN, ATTACK_HEAVY} };
vector<CInput> CCharacter::Command_Crouch_SpecialAttack = { {MOVEKEY_DOWN, ATTACK_SPECIAL} };


vector<CInput> CCharacter::Command_Crouch_MediumAttack_Extra = { {MOVEKEY_DOWN_RIGHT, ATTACK_MEDIUM} };
vector<CInput> CCharacter::Command_Crouch_HeavyAttack_Extra = { {MOVEKEY_DOWN_RIGHT, ATTACK_HEAVY} };

vector<CInput> CCharacter::Command_Reflect = { {MOVEKEY_LEFT, ATTACK_SPECIAL} };

vector<CInput> CCharacter::Command_Up_SpecialAttack = { {MOVEKEY_UP, ATTACK_SPECIAL} };


vector<CInput> CCharacter::Command_LowBenishingAttack = { {MOVEKEY_DOWN, ATTACK_BENISHING} };
vector<CInput> CCharacter::Command_BenishingAttack = { {MOVEKEY_NEUTRAL, ATTACK_BENISHING} };


vector<CInput> CCharacter::Command_Transform = { {MOVEKEY_DOWN, ATTACK_TRANSFORM} };






CCharacter::CCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
	, m_pUI_Manager{ CUI_Manager::Get_Instance() }
{
	Safe_AddRef(m_pUI_Manager);
}

CCharacter::CCharacter(const CCharacter& Prototype)
	: CGameObject{ Prototype }
	, m_pFrameEvent{ Prototype.m_pFrameEvent }
	, m_pUI_Manager{ CUI_Manager::Get_Instance() }
	, m_pEffect_Manager{ CEffect_Manager::Get_Instance() }
{
	Safe_AddRef(m_pUI_Manager);
	Safe_AddRef(m_pEffect_Manager);
}

HRESULT CCharacter::Initialize_Prototype()
{
	//m_pFrameEvent = CFrameEvent_Manager::Get_Instance()->Get_pFrameEventMap();
	  //첫 실행일때, 루프돌았을때 ZeroFrame을 어떻게 구분하는가


	return S_OK;
}

HRESULT CCharacter::Initialize(void* pArg)
{
	Character_DESC* pDesc = static_cast<Character_DESC*>(pArg);
	//m_strModelName = pDesc->strModelName;

	if (pArg == nullptr)
	{
		FlipDirection(1);
		return S_OK;
	}

	m_pFrameEvent = CFrameEvent_Manager::Get_Instance()->Get_pFrameEventMap();

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Character_DESC* pSlotDesc = static_cast<Character_DESC*>(pArg);
	m_ePlayerSlot = pSlotDesc->ePlayerSlot;
	m_tCharacterDesc.ePlayer_Slot = m_ePlayerSlot;


	m_iPlayerTeam = pDesc->iTeam;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (m_iPlayerTeam == 1)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, -3.f, 0.f, 1.f));
		FlipDirection(1);
	}
	else
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(2.f, -3.f, 0.f, 1.f));
		FlipDirection(-1);
	}

	//모델 로드를 하위 클래스로 옮겼으니 각자 처리하기 
	//m_pModelCom->SetUp_Animation(0, false);
	//m_pModelCom->Play_Animation(0.f);


	inputBuffer.push_back(CInput(MOVEKEY_NEUTRAL, ATTACK_NONE));


	//CBattleInterface_Manager::Get_Instance()->Regist_Character(m_iPlayerTeam, this, m_ePlayerSlot);
	//if (m_ePlayerSlot != CUI_Define::PLAYER_SLOT::LPLAYER1 && m_ePlayerSlot != CUI_Define::PLAYER_SLOT::RPLAYER1)
	//{
	//	m_bPlaying = false;
	//	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(5.f * m_iPlayerTeam, 100.f, 0.f, 1.f));
	//
	//	//캐릭터 사이즈에 맞게 각자 추가하느라 m_pColliderCom이 없음
	//	//m_pColliderCom->Update(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	//}
	//else
	//	m_bPlaying = true;

	return S_OK;
}

void CCharacter::Player_Update(_float fTimeDelta)
{
	m_tCharacterDesc.bStun = m_bRedHp;

	m_tCharacterDesc.bAttBuf = m_bAttBuf;
	m_tCharacterDesc.iHp = m_iHP;
	m_tCharacterDesc.bHit = m_bHit;

	m_tCharacterDesc.iComboCount = CBattleInterface_Manager::Get_Instance()->Get_HitCount(m_iPlayerTeam);

	if (m_iPrevComboCount < m_tCharacterDesc.iComboCount)
	{
		m_bHit = false;
	}

	//if (m_bPlaying && m_iHP <= 0)
	//{
	//	CBattleInterface_Manager::Get_Instance()->Reset_HitCount(m_iPlayerTeam);
	//}

	if (m_bPlaying && m_bDying)
	{
		CBattleInterface_Manager::Get_Instance()->Reset_HitCount(m_iPlayerTeam);
	}

	m_tCharacterDesc.iSKillCount = m_iSKillCount;
	m_tCharacterDesc.iSKillPoint = m_iSKillPoint;
	m_tCharacterDesc.ePlayer_Slot = m_ePlayerSlot;
	m_tCharacterDesc.ePlayerID = m_eCharacterID;
	m_tCharacterDesc.iTeam = m_iPlayerTeam;

	m_iPrevComboCount = m_tCharacterDesc.iComboCount;
}

void CCharacter::Camera_Update(_float fTimeDelta)
{


}

void CCharacter::Update(_float fTimeDelta)
{
	if (m_bAnimationLock == false)
	{

	}
	else
	{
		m_fAccAnimationLock += fTimeDelta;
		if (m_fAccAnimationLock > m_fMaxAnimationLock)
		{
			m_fAccAnimationLock = 0.f;
			m_bAnimationLock = false;
		}
	}

}

void CCharacter::Late_Update(_float fTimeDelta)
{
	//if (m_bDestructiveFinish == true)
	//{
	//	//m_fAccDyingTime += fTimeDelta;
	//	//if (m_fAccDyingTime > 7)
	//	if (m_fAccDyingTime > 8.f)
	//	{
	//		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_vDyingPosition));
	//
	//		m_bDestructiveFinish = false;
	//		//m_fAccDyingTime = 0.f;
	//
	//
	//
	//		//Set_fImpulse({ 0.f,0.f });
	//	}
	//}

	if (m_bPlaying || m_bTag_In)
	{
		m_pRenderInstance->Add_RenderObject(CRenderer::RG_PLAYER, this, &m_RendererDesc);


	}

#ifdef _DEBUG
	m_pRenderInstance->Add_DebugComponent(m_pColliderCom);
#endif

}

HRESULT CCharacter::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		/* 모델이 가지고 있는 머테리얼 중 i번째 메시가 사용해야하는 머테리얼구조체의 aiTextureType_DIFFUSE번째 텍스쳐를 */
		/* m_pShaderCom에 있는 g_DiffuseTexture변수에 던져. */
		if (FAILED(m_pModelCom->Bind_MaterialSRV(m_pShaderCom, aiTextureType_DIFFUSE, "g_DiffuseTexture", i)))
			return E_FAIL;
		// m_pModelCom->Bind_MaterialSRV(m_pShaderCom, aiTextureType_NORMALS, "g_NormalTexture", i);

		/* 모델이 가지고 있는 뼈들 중에서 현재 렌더링할려고 했던 i번째ㅑ 메시가 사용하는 뼈들을 배열로 만들어서 쉐이더로 던져준다.  */
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}



_bool CCharacter::CompareNextAnimation(_uint iAnimationIndex, _float fNextPosition)
{
	_bool bCompare = true;

	if (fNextPosition != 0)
	{
		if (m_fNextAnimationCurrentPosition != fNextPosition)
			bCompare = false;
	}

	if (iAnimationIndex != m_iNextAnimation.first)
		bCompare = false;

	return bCompare;
}

void CCharacter::Set_CurrentAnimationPositionJump(_float fAnimationPosition)
{
	m_pModelCom->CurrentAnimationPositionJump(fAnimationPosition);
}

void CCharacter::ProcessEventsFramesZero(_uint characterIndex, _uint animationIndex)
{

	//애니메이션 변경시 0번째 프레임이벤트를 실행하기 위함

	auto characterIt = m_pFrameEvent->find(m_eCharacterIndex);   //테스트중
	if (characterIt != m_pFrameEvent->end())
	{
		auto& animationMap = characterIt->second;
		auto animationIt = animationMap.find(animationIndex);
		if (animationIt != animationMap.end())
		{
			auto& frameMap = animationIt->second;

			auto frameIt = frameMap[0];

			for (auto event : frameIt)
			{

				CFrameEvent_Manager::Get_Instance()->UseEvent(event, this);
			}

		}
	}



}

void CCharacter::ProcessEventsBetweenFrames2(int characterIndex, int animationIndex, float prevFrame, float currentFrame)
{

	//Frame이 멈춰서 중복처리되는경우는?
   // 프레임이 0인경우를 제외하고 프레임이 멈췄는데 정확하게 이벤트가 있을 수 있나?
	   // 0.001초단위로 보면 불가능한건 아님

   //해결방법
   // 1. 이 함수를 play animation 주변에 넣어서, 애니메이션이 정지한경우 이 함수 호출 자체를 안함.  << 정석
   // 2. bool값을 만들어서 1회만 처리

   //일단 보류.




   //frame이 0인경우는 애니메이션 실행 말고, 애니메이션 변경시 0의 이벤트만 찾아서 하는걸로 처리.   (ProcessEventsFramesZero)
	if (prevFrame == 0)
	{

		//과거,현재 프레임이 모두 0으로 애니메이션 자체가 정지된 경우 함수 끝
		if (currentFrame == 0)
			return;

		//아니면 0프레임 이벤트 제외하고 실행하기 위해  과거프레임 조금 증가
		else
			prevFrame += 0.001;
	}





	// 캐릭터 인덱스 탐색

	auto characterIt = m_pFrameEvent->find(m_eCharacterIndex);   //테스트중
	if (characterIt != m_pFrameEvent->end())
	{
		auto& animationMap = characterIt->second;
		auto animationIt = animationMap.find(animationIndex);
		if (animationIt != animationMap.end())
		{
			auto& frameMap = animationIt->second;


			//0일때를 따로 만들었으니 제외해야하나?
			for (auto frameIt = frameMap.lower_bound(prevFrame); frameIt != frameMap.end() && frameIt->first <= currentFrame; ++frameIt)
			{
				// 해당 프레임에서의 이벤트 리스트를 출력
				for (string event : frameIt->second)
				{
					//MSG_BOX(TEXT("프레임 " + frameIt->first + "에서 이벤트 발생: " + event));

					_bool bdebug1 = false;

					//m_pSelectedModelCom->m_Animations[m_pSelectedModelCom->m_iCurrentAnimationIndex]->m_fTickPerSecond = 100.f;
					//현재 25에서 돌아오지 않는 문제가 있음...?


					CFrameEvent_Manager::Get_Instance()->UseEvent(event, this);

				}
			}

		}
	}


}

_bool CCharacter::InputCommand()
{



	ButtonInput iAttackkey = ATTACK_NONE;
	DirectionInput iMoveKey = MOVEKEY_NEUTRAL;

	_int DirectionX = 0;
	_int DirectionY = 0;


	if (m_iPlayerTeam == 1)
	{

		if (m_pGameInstance->Key_Pressing(DIK_W))
		{
			DirectionY = 1;
		}

		else if (m_pGameInstance->Key_Pressing(DIK_S))
		{
			DirectionY = -1;
		}

		if (m_pGameInstance->Key_Pressing(DIK_A))
		{
			DirectionX -= m_iLookDirection;
		}

		else if (m_pGameInstance->Key_Pressing(DIK_D))
		{
			DirectionX += m_iLookDirection;
		}

		//if(DirectionX==0 && DirectionY ==0)

		if (DirectionX == -1 && DirectionY == 0)
			iMoveKey = MOVEKEY_LEFT;

		else if (DirectionX == 0 && DirectionY == 1)
			iMoveKey = MOVEKEY_UP;

		else if (DirectionX == -1 && DirectionY == -1)
			iMoveKey = MOVEKEY_DOWN_LEFT;

		else if (DirectionX == 0 && DirectionY == -1)
			iMoveKey = MOVEKEY_DOWN;

		else if (DirectionX == 1 && DirectionY == -1)
			iMoveKey = MOVEKEY_DOWN_RIGHT;

		else if (DirectionX == 1 && DirectionY == 0)
			iMoveKey = MOVEKEY_RIGHT;

		else if (DirectionX == -1 && DirectionY == 1)
			iMoveKey = MOVEKEY_UP_LEFT;

		else if (DirectionX == 1 && DirectionY == 1)
			iMoveKey = MOVEKEY_UP_RIGHT;

		//둘 다 0인경우는 기본값이므로 지정하지 않음 대각선 위는 쓰이는 커맨드가 없으므로 지정하지 않음



		if (m_pGameInstance->Key_Down(DIK_U))
		{
			iAttackkey = ATTACK_LIGHT;
		}

		if (m_pGameInstance->Key_Down(DIK_I))
		{
			iAttackkey = ATTACK_MEDIUM;

		}
		if (m_pGameInstance->Key_Down(DIK_J))
		{
			iAttackkey = ATTACK_SPECIAL;

		}
		if (m_pGameInstance->Key_Down(DIK_K))
		{
			iAttackkey = ATTACK_HEAVY;

		}


		//if (m_pGameInstance->Key_Pressing(DIK_O))
		if (m_pGameInstance->Key_Down(DIK_O))
		{
			iAttackkey = ATTACK_GRAB;

		}
		if (m_pGameInstance->Key_Down(DIK_L))
		{
			iAttackkey = ATTACK_BENISHING;
		}
		if (m_pGameInstance->Key_Down(DIK_M))
		{
			iAttackkey = ATTACK_TRANSFORM;
		}
		// if (m_pGameInstance->Key_Pressing(DIK_Y))
		// {
		//	 iAttackkey = ATTACK_LIGHT;
		//
		// }

		GetUI_Input(iMoveKey, iAttackkey, 1);

	}
	else  //2팀
	{
		if (m_pGameInstance->Key_Pressing(DIK_UP))
		{
			DirectionY = 1;
		}

		else if (m_pGameInstance->Key_Pressing(DIK_DOWN))
		{
			DirectionY = -1;
		}

		if (m_pGameInstance->Key_Pressing(DIK_LEFT))
		{
			DirectionX -= m_iLookDirection;
		}

		else if (m_pGameInstance->Key_Pressing(DIK_RIGHT))
		{
			DirectionX += m_iLookDirection;
		}

		//if(DirectionX==0 && DirectionY ==0)

		if (DirectionX == -1 && DirectionY == 0)
			iMoveKey = MOVEKEY_LEFT;

		else if (DirectionX == 0 && DirectionY == 1)
			iMoveKey = MOVEKEY_UP;

		else if (DirectionX == -1 && DirectionY == -1)
			iMoveKey = MOVEKEY_DOWN_LEFT;

		else if (DirectionX == 0 && DirectionY == -1)
			iMoveKey = MOVEKEY_DOWN;

		else if (DirectionX == 1 && DirectionY == -1)
			iMoveKey = MOVEKEY_DOWN_RIGHT;

		else if (DirectionX == 1 && DirectionY == 0)
			iMoveKey = MOVEKEY_RIGHT;

		else if (DirectionX == -1 && DirectionY == 1)
			iMoveKey = MOVEKEY_UP_LEFT;

		else if (DirectionX == 1 && DirectionY == 1)
			iMoveKey = MOVEKEY_UP_RIGHT;


		//둘 다 0인경우는 기본값이므로 지정하지 않음 대각선 위는 쓰이는 커맨드가 없으므로 지정하지 않음



		if (m_pGameInstance->Key_Down(DIK_NUMPAD7) || m_pGameInstance->Key_Down(DIK_7))
		{
			iAttackkey = ATTACK_LIGHT;
		}

		if (m_pGameInstance->Key_Down(DIK_NUMPAD8) || m_pGameInstance->Key_Down(DIK_8))
		{
			iAttackkey = ATTACK_MEDIUM;

		}
		if (m_pGameInstance->Key_Down(DIK_NUMPAD4) || m_pGameInstance->Key_Down(DIK_4))
		{
			iAttackkey = ATTACK_SPECIAL;

		}
		if (m_pGameInstance->Key_Down(DIK_NUMPAD5) || m_pGameInstance->Key_Down(DIK_5))
		{
			iAttackkey = ATTACK_HEAVY;

		}


		if (m_pGameInstance->Key_Down(DIK_NUMPAD9) || m_pGameInstance->Key_Down(DIK_9))
		{
			iAttackkey = ATTACK_GRAB;

		}
		if (m_pGameInstance->Key_Down(DIK_NUMPAD6))
		{
			iAttackkey = ATTACK_BENISHING;
		}
		if (m_pGameInstance->Key_Down(DIK_NUMPAD1))
		{
			iAttackkey = ATTACK_TRANSFORM;
		}
		// if (m_pGameInstance->Key_Pressing(DIK_Y))
		// {
		//	 iAttackkey = ATTACK_LIGHT;
		//
		// }
		GetUI_Input(iMoveKey, iAttackkey, 2);
	}


	_bool bNewKey = false;

	CInput newInput(iMoveKey, iAttackkey);

	if (inputBuffer.size() > 0)
	{
		//마지막 상태와 키 입력 상태가 똑같으면 갱신하지 않음
		if (inputBuffer.back() == newInput)
		{

		}
		else
		{
			UpdateInputBuffer(CInput(iMoveKey, iAttackkey));
			bNewKey = true;
		}
	}
	else
	{
		UpdateInputBuffer(CInput(iMoveKey, iAttackkey));
		bNewKey = true;
	}




	return bNewKey;
}

void CCharacter::InputedCommandUpdate(_float fTimeDelta)
{



	inputBuffer.erase(
		remove_if(inputBuffer.begin(), inputBuffer.end(), [fTimeDelta](CInput& input) {
			input.frameTime += fTimeDelta;  // frameTime에 fTimeDelta 더하기
			return input.frameTime >= 0.35f; // 0.3 이상인 경우 삭제
			}),
		inputBuffer.end()
	);



}


_bool CCharacter::Character_Play_Animation(_float fTimeDelta)
{

	_bool bAnimationEnd = false;

	_float fPrePosition = m_pModelCom->m_fCurrentAnimPosition;

	//_int iOneFrameTeest = 0;

	if (fPrePosition == 0)
	{

		ProcessEventsFramesZero(m_eCharacterIndex, m_pModelCom->m_iCurrentAnimationIndex);
		fPrePosition += 0.001;

		//iOneFrameTeest++;
	}

	if (m_pModelCom->Play_Animation_Lick(fTimeDelta))
		//if (m_pModelCom->Play_Animation(fTimeDelta))
	{
		//모션이 끝났으면, 루프면    (아까까진 루프가 아니였는데 이번에 루프면 어쩌지?)
		if (m_pModelCom->m_isLoopAnim)
		{
			fPrePosition = 0.001;
			ProcessEventsFramesZero(m_eCharacterIndex, m_pModelCom->m_iCurrentAnimationIndex);
			//iOneFrameTeest++;
		}
		bAnimationEnd = true;
		m_bMotionPlaying = false;
	}
	else
		m_bMotionPlaying = true;


	_float fCurPosition = m_pModelCom->m_fCurrentAnimPosition;


	ProcessEventsBetweenFrames2(0, m_pModelCom->m_iCurrentAnimationIndex, fPrePosition, fCurPosition);



	return bAnimationEnd;
}

_bool CCharacter::Character_Play_Animation_NoXZ(_float fTimeDelta)
{
	_bool bAnimationEnd = false;

	_float fPrePosition = m_pModelCom->m_fCurrentAnimPosition;

	//_int iOneFrameTeest = 0;

	if (fPrePosition == 0)
	{

		ProcessEventsFramesZero(m_eCharacterIndex, m_pModelCom->m_iCurrentAnimationIndex);
		fPrePosition += 0.001;

		//iOneFrameTeest++;
	}

	if (m_pModelCom->Play_Animation_Lick2(fTimeDelta, m_pTransformCom))
		//if (m_pModelCom->Play_Animation(fTimeDelta))
	{
		//모션이 끝났으면, 루프면    (아까까진 루프가 아니였는데 이번에 루프면 어쩌지?)
		if (m_pModelCom->m_isLoopAnim)
		{
			fPrePosition = 0.001;
			ProcessEventsFramesZero(m_eCharacterIndex, m_pModelCom->m_iCurrentAnimationIndex);
			//iOneFrameTeest++;
		}
		bAnimationEnd = true;
		m_bMotionPlaying = false;
	}
	else
		m_bMotionPlaying = true;


	_float fCurPosition = m_pModelCom->m_fCurrentAnimPosition;


	ProcessEventsBetweenFrames2(0, m_pModelCom->m_iCurrentAnimationIndex, fPrePosition, fCurPosition);



	return bAnimationEnd;
}

_bool CCharacter::Check_bCurAnimationisAirMove(_uint iAnimation)
{
	_uint iModelIndex = iAnimation;

	if (iAnimation == 1000)
		iModelIndex = m_pModelCom->m_iCurrentAnimationIndex;



	//if (iModelIndex == m_iHit_Air_LightAnimationIndex || iModelIndex == m_iHit_Away_LeftAnimationIndex || iModelIndex == m_iHit_Away_UpAnimationIndex || iModelIndex == m_iHit_Air_FallAnimationIndex)
	if (iModelIndex == m_iJumpAnimationIndex || iModelIndex == m_iFallAnimationIndex)
	{
		return true;
	}

	return false;
}

bool CCharacter::CheckCommandSkippingExtras(const vector<CInput>& pattern, int timeWindow)
{
	if (inputBuffer.size() < pattern.size()) return false;

	int patternIndex = 0;

	// 입력 버퍼에서 패턴을 찾기 시작
	for (int i = 0; i < inputBuffer.size(); ++i) {
		const CInput& input = inputBuffer[i];
		const CInput& expected = pattern[patternIndex];

		// 불필요한 입력은 건너뜀
		if (input.direction == expected.direction && input.button == expected.button) {
			// 패턴이 일치하면 다음 패턴으로 넘어감
			patternIndex++;

			// 모든 패턴이 일치하면 성공
			if (patternIndex >= pattern.size())
			{
				inputBuffer.clear();
				return true;
			}
		}


	}

	return false;
}

bool CCharacter::CheckCommand_Exactly(const std::vector<CInput>& pattern, int timeWindow)
{
	if (inputBuffer.size() < pattern.size()) return false;  // 입력 버퍼가 패턴보다 짧으면 실패


	// 입력 버퍼 전체를 순회하면서 패턴을 찾음
	for (int i = 0; i <= inputBuffer.size() - pattern.size(); ++i)
	{
		bool isPatternMatched = true;

		// 현재 위치부터 패턴을 비교
		for (int j = 0; j < pattern.size(); ++j) {
			const CInput& inputCheck = inputBuffer[i + j];
			const CInput& expected = pattern[j];

			// 패턴과 일치하지 않으면 실패
			if (inputCheck.direction != expected.direction || inputCheck.button != expected.button) {
				isPatternMatched = false;
				break;
			}


		}

		// 패턴이 일치했으면 true 반환
		if (isPatternMatched)
		{
			inputBuffer.clear();
			return true;
		}
	}

	return false;  // 패턴을 찾지 못하면 실패
}


_uint CCharacter::CheckAllCommands()
{


	for (const auto& command : MoveCommandPatternsFunction)
	{
		if (CheckCommandSkippingExtras(command.pattern, 0)) {
			command.action();  // 해당 패턴이 매칭되면 해당 기술 실행

			return 0;
		}
	}


	//정확해야하만 하는 패턴 따로 구분
	for (const auto& command : MoveCommandPatternsFunction_Exactly)
	{
		if (CheckCommand_Exactly(command.pattern, 0)) {
			command.action();  // 해당 패턴이 매칭되면 해당 기술 실행

			return 0;
		}
	}
	return 0;


}


void CCharacter::ShowInputBuffer()
{
	inputBuffer;
	m_fGravityTime;
	m_pModelCom->m_iCurrentAnimationIndex;
	m_pModelCom->m_fCurrentAnimPosition;
	_float fHeight = Get_fHeight();
	_bool bDebug = true;
}

void CCharacter::DebugPositionReset(_short iDirection)
{
	_float fHegiht = Get_fHeight();

	if (iDirection == 0)
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, { (_float)m_iPlayerTeam,fHegiht,0,1 });
	else if (iDirection == -1)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, { -10.5f + (_float)m_iPlayerTeam,fHegiht,0,1 });
	}
	else
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, { 10.f + (_float)m_iPlayerTeam,fHegiht,0,1 });
	}

}

void CCharacter::FlipDirection(_int iDirection)
{
	if (iDirection == 0)
	{
		m_iLookDirection = -m_iLookDirection;

		m_pTransformCom->Set_Scaled(-1, 1, 1);
	}
	else if (m_iLookDirection != iDirection)
	{

		m_iLookDirection = iDirection;

		m_pTransformCom->Set_Scaled(-1, 1, 1);
	}

	//m_pTransformCom->Set_Scaled(-1, 1, 1);

}

void CCharacter::Create_Effect(_int iEffectIndex)
{
	switch (iEffectIndex)
	{
	case 0:
		//Create_kamehameha();
		break;

	case 1:
		//Create_Smash();
		break;

	default:
		break;
	}
}

_float CCharacter::Get_fHeight()
{
	return 	XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

}

CCharacter* CCharacter::Get_pEnemy()
{
	return m_pEnemy;
}

void CCharacter::Set_ForcedGravityDown()
{
	//중력 최고점
	//if (m_fGravityTime < 0.305)
	//	m_fGravityTime = 0.305f;

	//공중 앞대시 
	m_fGravityTime = 0.255f;


}

void CCharacter::Set_ForcedGravityTime_LittleUp()
{
	m_fGravityTime = 0.135f;

	if (m_fJumpPower == 0)
	{
		m_fJumpPower = fJumpPower;
	}
}

void CCharacter::Set_ForcveGravityTime(_float fGravityTime)
{
	m_fGravityTime = fGravityTime;
}

void CCharacter::Set_bAttackGravity(_bool bAttackGravity)
{
	m_bAttackGravity = bAttackGravity;

	if (bAttackGravity)
		_bool bDebug = false;


}

void CCharacter::AttckCancleJump()
{
	if (m_pModelCom->m_iCurrentAnimationIndex == m_iStandingMidAttackAnimationIndex && m_bAttackBackEvent)
		//if (m_pModelCom->m_iCurrentAnimationIndex == m_iStandingMidAttackAnimationIndex)
	{

		//1팀
		if (m_iPlayerTeam == 1 && m_pGameInstance->Key_Down(DIK_W))
		{

			//Set_fJumpPower(4.f); //중력Ver1 기준
			Set_fJumpPower(3.f); //중력Ver2 기준

			//Add_Move({ 0.f,0.2f });

			//if (m_pModelCom->m_iCurrentAnimationIndex == m_iJumpAnimationIndex && m_bDoubleJumpEnable)
			//{
			//	//Set_NextAnimation(m_iJumpAnimationIndex, 0.5f);
			//}

			//Set_NextAnimation(m_iJumpAnimationIndex, 0.5f);
			Set_Animation(m_iJumpAnimationIndex, false);


			if (m_pGameInstance->Key_Pressing(DIK_A))
			{
				Set_fImpulse(-5.f);
			}

			else if (m_pGameInstance->Key_Pressing(DIK_D))
			{
				Set_fImpulse(5.f);
			}
		}

		//2팀
		else if (m_iPlayerTeam == 2 && m_pGameInstance->Key_Down(DIK_UP))
		{
			//Set_fJumpPower(4.f); //중력Ver1 기준
			Set_fJumpPower(3.f); //중력Ver2 기준

			//if(m_pModelCom->m_iCurrentAnimationIndex == m_iJumpAnimationIndex && m_bDoubleJumpEnable)
			//{
			//	//Set_NextAnimation(m_iJumpAnimationIndex, 0.5f);
			//}
			//Set_NextAnimation(m_iJumpAnimationIndex, 0.5f);
			Set_Animation(m_iJumpAnimationIndex, false);


			if (m_pGameInstance->Key_Pressing(DIK_LEFT))
			{
				Set_fImpulse(-5.f);
			}

			else if (m_pGameInstance->Key_Pressing(DIK_RIGHT))
			{
				Set_fImpulse(5.f);
			}
		}

	}


	else if (m_bDoubleJumpEnable && (
		m_pModelCom->m_iCurrentAnimationIndex == m_iJumpAnimationIndex || m_pModelCom->m_iCurrentAnimationIndex == m_iFallAnimationIndex ||
		m_pModelCom->m_iCurrentAnimationIndex == m_iAttack_Air1 || m_pModelCom->m_iCurrentAnimationIndex == m_iAttack_Air2 || m_pModelCom->m_iCurrentAnimationIndex == m_iAttack_AirUpper))
	{

		//1팀
		if (m_iPlayerTeam == 1 && m_pGameInstance->Key_Down(DIK_W))
		{

			//Set_fJumpPower(4.f); //중력Ver1 기준
			Set_fJumpPower(3.f); //중력Ver2 기준
			//Set_ForcveGravityTime(0.08f); //묘하게 낮은듯
			Set_ForcveGravityTime(0.03f);


			//Set_ForcedGravityTime_LittleUp() //0.135f

			//if (m_pModelCom->m_iCurrentAnimationIndex == m_iJumpAnimationIndex && m_bDoubleJumpEnable)
			//{
			//	//Set_NextAnimation(m_iJumpAnimationIndex, 0.5f);
			//}
			//Set_NextAnimation(m_iJumpAnimationIndex, 0.5f);

			if (m_pModelCom->m_iCurrentAnimationIndex == m_iJumpAnimationIndex || m_pModelCom->m_iCurrentAnimationIndex == m_iFallAnimationIndex)
				m_pModelCom->CurrentAnimationPositionJump(0.f);

			else
				Set_NextAnimation(m_iJumpAnimationIndex, 0.5f);



			if (m_pGameInstance->Key_Pressing(DIK_A))
			{
				Set_fImpulse(-5.f);
			}

			else if (m_pGameInstance->Key_Pressing(DIK_D))
			{
				Set_fImpulse(5.f);
			}
			else
				Set_fImpulse(0.f);


			m_bDoubleJumpEnable = false;

		}

		//2팀
		else if (m_iPlayerTeam == 2 && m_pGameInstance->Key_Down(DIK_UP))
		{
			//Set_fJumpPower(4.f); //중력Ver1 기준
			Set_fJumpPower(3.f); //중력Ver2 기준
			Set_ForcveGravityTime(0.03f);

			if (m_pModelCom->m_iCurrentAnimationIndex == m_iJumpAnimationIndex || m_pModelCom->m_iCurrentAnimationIndex == m_iFallAnimationIndex)
				m_pModelCom->CurrentAnimationPositionJump(0.f);

			else
				Set_NextAnimation(m_iJumpAnimationIndex, 0.5f);




			Set_ForcveGravityTime(0.03f);

			if (m_pGameInstance->Key_Pressing(DIK_LEFT))
			{
				Set_fImpulse(-5.f);
			}

			else if (m_pGameInstance->Key_Pressing(DIK_RIGHT))
			{
				Set_fImpulse(5.f);
			}
			else
				Set_fImpulse(0.f);


			m_bDoubleJumpEnable = false;

		}


	}

}

/*

void CCharacter::Chase(_float fTimeDelta)
{

	m_pModelCom->SetUp_Animation(m_iChaseAnimationIndex, false);
	m_pModelCom->Play_Animation(0.f);


	CTransform* pTarget = static_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Target"), TEXT("Com_Transform")));
	_vector vTargetPos = pTarget->Get_State(CTransform::STATE_POSITION);

	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	m_fAccChaseTime += fTimeDelta;

	_vector vDir = XMVector4Normalize(vTargetPos - vMyPos);
	Set_fImpulse(XMVectorGetX(vDir) * 2.f);

	//_float angle = atan2(XMVectorGetY(vDir), XMVectorGetX(vDir));
	_float angle = atan2(XMVectorGetY(vDir), XMVectorGetX(vDir)) * (180.0 / 3.14);

	angle = (angle + 90) * 0.5f;
	//cout << angle << endl;



	if (0 < angle && angle < 90)  //적이 오른쪽에 있는 경우
	{
		FlipDirection(1);
	}

	else if (angle > 90)   //적이 왼쪽 위에 있는 경우
	{
		//110의 경우 70으로 바꿔야 한다.    초과값 20.   90으로부터 초과값 만큼 빼면 됨
		// angle = 90 - (90 - angle);    =  180-angle;

		FlipDirection(-1);
		angle = 180 - angle;
	}
	else if (angle < 0)   //적이 왼쪽에 아래에 있는 경우
	{
		FlipDirection(-1);
		angle = -angle;
	}


	//추적 속도를 점점 빠르게
	vDir = vDir * m_fAccChaseTime * 0.5f;

	m_pTransformCom->Add_MoveVector(vDir);
	Set_CurrentAnimationPositionJump(angle);
}
*/
void CCharacter::Chase2(_float fTimeDelta)
{




	//디버그용 예외처리.  멈춰버리면 지랄남
	if (fTimeDelta > 1)
	{
		return;
	}

	if (m_bStun)
	{
		m_bChase = false;

	}

	//NextMotion일때 미리 시간 더하는중이길래 아래로 내림
	//m_fAccChaseTime += fTimeDelta;

	//준비자세면 이렇게 한다.
	if (m_pModelCom->m_iCurrentAnimationIndex == m_iFallAnimationIndex)
	{

		//쓰던것
		//Add_Move({ 0.f,fTimeDelta * 1.2f });


		//원작이랑 가깝긴 한데, 21호, chase 공격충돌 등 이런저런 문제로 취소.   이걸로 할꺼면 준비시간 늘리고 초기속도 빠르게하고 조정해야할게 많음
		//사전준비자세만으로는 괜찮음 초기속도 어쩌지?
		Add_Move({ -1.f * fTimeDelta,fTimeDelta * 4.8f });

		m_fAccChaseTime += fTimeDelta;

		if (m_fAccChaseTime > 0.2f)
		{
			m_pModelCom->SetUp_Animation(m_iChaseAnimationIndex, false);
			m_fJumpPower = fJumpPower;

			//Character_Make_Effect(TEXT("BurstR-02"));

			//m_pChaseEffectLayer= m_pEffect_Manager->Copy_Layer_AndGet(TEXT("BurstR-02"), m_pTransformCom->Get_WorldMatrixPtr());

			//XMStoreFloat4x4(&m_IdentityMatrix, XMMatrixIdentity());
			CEffect_Layer::COPY_DESC tDesc{};

			tDesc.pPlayertMatrix = m_pModelCom->Get_BoneMatrixPtr("G_head");
			tDesc.pTransformCom = m_pTransformCom;
			tDesc.m_isPlayerDirRight = m_iLookDirection;


			m_pChaseEffectLayer = CEffect_Manager::Get_Instance()->Copy_Layer_AndGet(m_ChaseEffectName, &tDesc);

			//m_pChaseEffectLayer = m_pEffect_Manager->Copy_Layer_AndGet(TEXT("BurstR-02_Rotated_Left"), &tDesc);

			//BurstR-02_Rotated_Left

			//m_pEffect_Manager->Copy_Layer(TEXT("BurstR-02"), m_pTransformCom->Get_WorldMatrixPtr());

			//if (m_bChaseAttackEnable)
			{
				//공격판정 테스트
				{

					CAttackObject::ATTACK_DESC Desc{};

					if (m_iPlayerTeam == 1)
						Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
					else
						Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
					Desc.ColliderDesc.pMineGameObject = this;
					Desc.ColliderDesc.vCenter = { 0.f,0.5f,0.f };
					Desc.ColliderDesc.vExtents = { 1.f,1.f,0.2f };


					Desc.fhitCharacter_Impus = { 0.7f * m_iLookDirection,0.3f };

					Desc.fhitCharacter_StunTime = 0.6f;
					Desc.iDamage = 300 * Get_DamageScale();
					Desc.fLifeTime = 5.f;
					Desc.ihitCharacter_Motion = { HitMotion::HIT_CHASE };

					Desc.iTeam = m_iPlayerTeam;
					Desc.fAnimationLockTime = 0.1f;
					Desc.pOwner = this;

					Desc.bReflect = true;
					//Desc.bDrawNoneStop = true;

					m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Chase"), TEXT("Layer_AttackObject"), &Desc);

					//m_pChaseAttackObejct = static_cast<CAttackObject*>(m_pGameInstance->Add_GameObject_ToLayer_AndGet(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Chase"), TEXT("Layer_AttackObject"), &Desc));


					//m_pChaseAttackObejct = { nullptr };
					//Set_RemoteDestory()

				}
				//m_bChaseAttackEnable = false;
			}
		}
		else
		{
			return;
		}
	}

	//돌진중이면 이렇게 한다
	else if (m_pModelCom->m_iCurrentAnimationIndex == m_iChaseAnimationIndex)
	{

		m_fAccChaseTime += fTimeDelta;

		if (m_fAccChaseTime > 20.f)
		{
			m_bChase = false;
			//if (m_pChaseAttackObejct != nullptr)
			//{
			//   m_pChaseAttackObejct->Set_RemoteDestory();
			//   m_pChaseAttackObejct = nullptr;
			//}
			return;
		}
	}
	else   //돌진중도 준비자세도 아니면
	{
		return;

	}


	//CTransform* pTarget = static_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Target"), TEXT("Com_Transform")));
	CTransform* pTarget = static_cast<CTransform*>(m_pEnemy->Get_Component(TEXT("Com_Transform")));

	_vector vTargetPos = pTarget->Get_State(CTransform::STATE_POSITION);

	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);



	_float vLength = GetVectorLength((vTargetPos - vMyPos));
	//m_pChaseAttackObejct

	if (vLength < 0.5f) //0.3
	{
		m_bChase = false;


		//테스트
		m_fAccChaseTime = 0.f;
		m_fGravityTime = 0.185f;
		m_pModelCom->SetUp_Animation(m_iFallAnimationIndex, false);


		return;
	}


	m_vChaseDir = XMVector4Normalize(vTargetPos - vMyPos);
	Set_fImpulse(XMVectorGetX(m_vChaseDir) * 2.f);




	//애니메이션 이용을 위해 각도값을 특수 처리 할 필요가 있음
	_float angle = atan2(XMVectorGetY(m_vChaseDir), XMVectorGetX(m_vChaseDir)) * (180.0 / 3.14);

	_float EffectAngle = angle;

	if (m_pChaseEffectLayer != nullptr && m_fAccChaseTime < 2.f)
	{
		/* 체이스 2P 수정 */

		if (m_iLookDirection == 1)
			m_pChaseEffectLayer->Set_Copy_Layer_Rotation({ 0.f, 0.f, EffectAngle });
		else if (m_iLookDirection == -1)
			m_pChaseEffectLayer->Set_Copy_Layer_Rotation({ 0.f, 0.f, 180 - EffectAngle });


		_float xdegree = XMVectorGetX(m_vChaseDir);

		//m_pChaseEffectLayer->Set_Layer_Position({ XMVectorGetX(m_vChaseDir)*1.5f, XMVectorGetY(m_vChaseDir)*1.5f,0.f });


		cout << angle << endl;

		_float closeness = 70.0f / (1.0f + abs(EffectAngle - 90));

		//m_pChaseEffectLayer->Set_Copy_Layer_Position({ XMVectorGetX(m_vChaseDir) * closeness , XMVectorGetY(m_vChaseDir) * 1.5f,0.f });
		//m_pChaseEffectLayer->Set_Copy_Layer_Position({ XMVectorGetX(m_vChaseDir) * closeness , XMVectorGetY(m_vChaseDir) * 1.5f,0.f});


		//디버깅

	}

	angle = (angle + 90) * 0.5f;

	if (0 < angle && angle < 90)  //적이 오른쪽에 있는 경우
	{
		//캐릭터 보는 방향 오른쪽으로 변경
		FlipDirection(1);
	}

	else if (angle > 90)   //적이 왼쪽 위에 있는 경우 
	{
		//110의 경우 70으로 바꿔야 한다.    초과값 20.   90으로부터 초과값 만큼 빼면 됨
		// angle = 90 - (90 - angle);    =  180-angle;

		FlipDirection(-1);
		angle = 180 - angle;
	}
	else if (angle < 0)   //적이 왼쪽에 아래에 있는 경우 
	{
		FlipDirection(-1);
		angle = -angle;
	}


	//추적 속도를 점점 빠르게
	//m_pTransformCom->Add_MoveVector(m_vChaseDir * m_fAccChaseTime * 0.5f);
	m_pTransformCom->Add_MoveVector(m_vChaseDir * m_fAccChaseTime * m_fAccChaseTime * 0.7f);



	//애니메이션의 position이 각도를 의미함 (1:1은 아니고 특수처리되어있음)
	Set_CurrentAnimationPositionJump(angle);



}
//반드시 Set이던 시절 코드 백업용.
//m_bChase 가 true일 때만 들어올것





/*
void CCharacter::Chase_Ready(_float fTimeDelta)
{

	if (m_bChaseEnable == false || m_pModelCom->m_iCurrentAnimationIndex == m_iSparkingAnimationIndex)
		return;

	//if(Check_bCurAnimationisCanChase())

	_short iCheck = Check_bCurAnimationisCanChase();
	if (iCheck == 0)
		return;
	else
		m_fAccChaseTime = iCheck * 0.01f;


	if (iCheck == 1)
	{
		Character_Make_Effect(TEXT("BurstR-01"));
	}

	m_bChaseEnable = false;

	if (m_pModelCom->m_iCurrentAnimationIndex == m_iAttack_LightLast)
	{
		m_bChaseStoping = false;

		Set_NextAnimation(m_iFallAnimationIndex, 5.f);

		m_bChase = true;

		m_fAccChaseTime = 0.5f;

	}


	if (Check_bCurAnimationisGroundMove() || m_pModelCom->m_iCurrentAnimationIndex == m_iJumpAnimationIndex || m_pModelCom->m_iCurrentAnimationIndex == m_iFallAnimationIndex)
	{

		m_bChaseStoping = false;

		m_pModelCom->SetUp_Animation(m_iFallAnimationIndex, false);

		m_bChase = true;

		if (XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) < 0.5)
		{
			//m_pTransformCom->Add_Move({ 0.f,0.6f,0.f });

		}



	}
	else
	{
		Set_NextAnimation(m_iFallAnimationIndex, 1.f);
		m_bChase = true;
	}








}
*/

void CCharacter::Chase_Ready(_float fTimeDelta, _bool bNoReady)
{
	if (m_bChaseEnable == false || m_pModelCom->m_iCurrentAnimationIndex == m_iSparkingAnimationIndex)
		return;

	//if(Check_bCurAnimationisCanChase())


	_short iCheck = Check_bCurAnimationisCanChase();
	if (iCheck == 0)
		return;
	else
		m_fAccChaseTime = iCheck * 0.01f;


	if (iCheck == 1 && bNoReady != true)
	{
		//Character_Make_Effect(TEXT("BurstR-01"));
		CEffect_Layer::COPY_DESC tDesc{};
		tDesc.pPlayertMatrix = m_pTransformCom->Get_WorldMatrixPtr();
		m_pEffect_Manager->Copy_Layer(TEXT("BurstR-01"), &tDesc);

		//Character_Make_BoneEffect("GD_fist_R", TEXT("BurstR-01"));

	}

	m_bChaseEnable = false;

	if (m_pModelCom->m_iCurrentAnimationIndex == m_iAttack_LightLast)
	{
		m_bChaseStoping = false;

		Set_NextAnimation(m_iFallAnimationIndex, 5.f);

		m_bChase = true;

		m_fAccChaseTime = 0.5f;

	}


	if (Check_bCurAnimationisGroundMove() || m_pModelCom->m_iCurrentAnimationIndex == m_iJumpAnimationIndex || m_pModelCom->m_iCurrentAnimationIndex == m_iFallAnimationIndex)
	{

		m_bChaseStoping = false;

		m_pModelCom->SetUp_Animation(m_iFallAnimationIndex, false);

		m_bChase = true;

		if (XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) < 0.5)
		{
			//m_pTransformCom->Add_Move({ 0.f,0.6f,0.f });

		}



	}
	else
	{
		Set_NextAnimation(m_iFallAnimationIndex, 1.f);
		m_bChase = true;
	}

}


void CCharacter::Chase_Grab(_float fTimeDelta)
{

	//디버그용 예외처리.  멈춰버리면 지랄남
	if (fTimeDelta > 1)
	{
		return;
	}

	//잡기 성공시 적을 추적하지 않음
	if (m_pModelCom->m_fCurrentAnimPosition > 25)
	{




		return;
	}


	//준비자세면 이렇게 한다.
	if (m_pModelCom->m_iCurrentAnimationIndex == m_iGrabReadyAnimationIndex)
	{
		//이펙트생성 1회?
		//if (m_fAccGrabTime == fTimeDelta)
		if (m_fAccGrabTime == 0.f)
		{
			;
		}
		return;
	}

	//준비자세도 잡기자세도 아닌데 이 함수에 들어온 경우 초기화 후 종료.
	else if (m_pModelCom->m_iCurrentAnimationIndex != m_iGrabAnimationIndex)
	{
		m_fAccGrabTime = 0.f;
		m_bGrab = false;
		return;
	}
	//else if (m_fAccGrabTime > 1.2) //시간제한으로 푸는게 아니라  1.2초 이상 지속될 수 없으니 강제해제.    였는데 1.2초 이상 지속될 일이 많음 10초까지 가기도 하는데 이러면 쓸모없지 않나
	//{
	//	m_fAccGrabTime = 0.f;
	//	m_bGrab = false;
	//	return;
	//}
	m_fAccGrabTime += fTimeDelta;








	CTransform* pTarget = static_cast<CTransform*>(m_pEnemy->Get_Component(TEXT("Com_Transform")));

	_vector vTargetPos = pTarget->Get_State(CTransform::STATE_POSITION);
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);


	m_vChaseDir = XMVector4Normalize(vTargetPos - vMyPos);
	Set_fImpulse(XMVectorGetX(m_vChaseDir) * 2.f);

	if (m_bGrab_Air == true)
	{
		m_vChaseDir = XMVectorSetX(m_vChaseDir, XMVectorGetX(m_vChaseDir) * 1.5f);

		//m_pTransformCom->Add_MoveVector(m_vChaseDir * (20 - m_fAccChaseTime * m_fAccChaseTime * 10.f) * fTimeDelta);
		m_pTransformCom->Add_MoveVector(m_vChaseDir * (15 - m_fAccChaseTime * m_fAccChaseTime * 10.f) * fTimeDelta);

	}
	else
	{
		m_vChaseDir = XMVectorSetY(m_vChaseDir, 0.f);
		m_pTransformCom->Add_MoveVector(m_vChaseDir * (15 - m_fAccChaseTime * m_fAccChaseTime * 10.f) * fTimeDelta);
	}


	//m_pTransformCom->Add_MoveVector(m_vChaseDir * (15 - m_fAccChaseTime*m_fAccChaseTime*3.f) * fTimeDelta);


	//기존에 쓰던것. 공중에서X길이 늘리려고 다시 만들것
	//m_pTransformCom->Add_MoveVector(m_vChaseDir * (15 - m_fAccChaseTime * m_fAccChaseTime * 10.f) * fTimeDelta);





}


void CCharacter::Character_Attack_Grab(_float fTimeDelta)
{
	//공격중인경우
	//if(m_pModelCom->m_fCurrentAnimPosition > 50 || m_pModelCom->m_fCurrentAnimPosition < 270)
	if (m_pModelCom->m_fCurrentAnimPosition > 120 && m_pModelCom->m_fCurrentAnimPosition < 138)
	{

		//우다다 횟수 소모할때까지 공격.   
		if (m_iGrabLoof > 0)
		{
			//cout << m_iGrabLoof << endl;
			m_iGrabLoof--;
			m_pModelCom->CurrentAnimationPositionJump(26.f);
		}
		else //다 썻으면 현재 상태에 따라 마지막 공격
		{
			m_bGrab = false;


			//잡기 공격이 비겼으면
			if (m_bGrabDraw)
			{

				//QTE 비기면 뒤로 낙법
				if (m_iQTE == 0)
				{

					Add_Move({ -0.4f * m_iLookDirection,0.3f });
					//BreakFall_Air();

					Set_Animation(m_iBreakFall_Air);
					Set_NextAnimation(m_iIdleAnimationIndex, 2.f);
					Set_ForcedGravityDown();

					Reset_AttackStep();
					Set_fImpulse({ -5.f * m_iLookDirection, 2.f });
				}

				//QTE 이겼으면 공격
				else if (m_iQTE == 1)
				{
					//Add_Move({ -0.4f * m_iLookDirection,0.f });
					Add_Move({ -0.2f * m_iLookDirection,0.f });


					if (Get_fHeight() == 0)
						Set_Animation(m_iAttack_Heavy);
					else
					{
						Set_Animation(m_iAttack_Air3);
						Set_CurrentAnimationPositionJump(m_fAIrGrabEndAnimationPositon);
						Set_ForcedGravityDown();
					}
				}
				//QTE 졌으면 쳐맞을 준비
				else if (m_iQTE == -1)
				{
					Set_AnimationStop(0.3f);
				}

				m_bCreateQTE = false;
				m_iQTE = -1;

			}
			else //공격 성공시
			{
				Add_Move({ -0.4f * m_iLookDirection,0.f });

				if (Get_fHeight() == 0)
					Set_Animation(m_iAttack_Heavy);
				else
				{
					Set_Animation(m_iAttack_Air3);
					Set_CurrentAnimationPositionJump(m_fAIrGrabEndAnimationPositon);
					Set_ForcedGravityDown();
				}
			}

			//else if (m_bGrab_Air)
			//{
			//	Set_Animation(m_iAttack_Air3);
			//}
			//else

		}
	}

}

void CCharacter::Grab_LateDraw()
{
	//Add_Move({ 0.3f * m_iLookDirection,0.3f });
	Add_Move({ 0.f,0.2f });
	Set_GrabLoofCount(1);
	Set_bGrabDraw(true);



	Set_Animation(m_iGrabAnimationIndex, false);
	Set_CurrentAnimationPositionJump(25.f);  	//캐릭터마다 다를수도있음


}


void CCharacter::Move(_float fTimeDelta)
{


	if (Check_bCurAnimationisGroundMove() || m_pModelCom->m_iCurrentAnimationIndex == m_iForwardDashAnimationIndex)
	{
		if (Check_bCurAnimationisGroundMove(m_iNextAnimation.first))
		{



			Reset_AttackCount();

			if (m_iPlayerTeam == 1)
				MoveKey1Team(fTimeDelta);
			else
				MoveKey2Team(fTimeDelta);



			//그래서 벽이면 어쩔껀데
			//if (Check_bWall())
			//{
			//	//(Get_fPositionX() < -12.f || Get_fPositionX() > 12.f || fabsf(Get_fPositionX() - m_pEnemy->Get_fPositionX()) > 8);
			//	Move_ForWall();
			//}

		}
	}
}

void CCharacter::MoveKey1Team(_float fTimeDelta)
{
	_short MoveKey = 0;
	if (m_pGameInstance->Key_Pressing(DIK_W) && m_bJumpLock == false)
		//if (m_pGameInstance->Key_Down(DIK_W) && m_bJumpLock == false)
	{
		//점프 먼지
		CEffect_Layer::COPY_DESC tDesc{};
		tDesc.pPlayertMatrix = m_pTransformCom->Get_WorldMatrixPtr();


		//점프 먼지
		m_pEffect_Manager->Copy_Layer(TEXT("Smoke01"), &tDesc);
		m_pEffect_Manager->Copy_Layer(TEXT("Smoke01_BackZ"), &tDesc);
		m_pEffect_Manager->Copy_Layer(TEXT("Smoke02"), &tDesc);
		m_pEffect_Manager->Copy_Layer(TEXT("Smoke02_Small"), &tDesc);
		m_pEffect_Manager->Copy_Layer(TEXT("Smoke04"), &tDesc);


		m_pTransformCom->Add_Move({ 0,0.3f,0 });

		//Set_fJumpPower(4.f); //중력Ver1 기준
		Set_fJumpPower(3.f); //중력Ver2 기준


		m_fGravityTime = 0.f;

		Set_Animation(m_iJumpAnimationIndex);


		if (m_pGameInstance->Key_Pressing(DIK_A))
		{
			Set_fImpulse(-5.f);
		}

		else if (m_pGameInstance->Key_Pressing(DIK_D))
		{
			Set_fImpulse(5.f);
		}

		//점프 시작
		m_pGameInstance->Stop_Sound(CSound_Manager::SOUND_KEY_NAME::Common_Dash_Start_SFX);
		m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Common_Jump_SFX, false, 1.f);
		//m_pEffect_Manager->Copy_Layer(TEXT("EnergieSAO-01"), m_pTransformCom->Get_WorldMatrixPtr());

	}

	else if (m_pGameInstance->Key_Pressing(DIK_S))
	{
		if (m_pModelCom->m_iCurrentAnimationIndex != m_iForwardDashAnimationIndex)
		{
			m_pModelCom->SetUp_Animation(m_iCrouchAnimationIndex, true);
		}
	}

	else
	{
		if (m_pGameInstance->Key_Pressing(DIK_A))
		{
			MoveKey -= m_iLookDirection;
		}

		else if (m_pGameInstance->Key_Pressing(DIK_D))
		{
			MoveKey += m_iLookDirection;
		}


		if (MoveKey == -1)
		{
			m_pModelCom->SetUp_Animation(m_iBackWalkAnimationIndex, false);

			m_iNextAnimation.first = m_iIdleAnimationIndex;

			m_iNextAnimation.second = 100.f;

		}
		else if (MoveKey == 1)
		{
			if (m_pModelCom->m_iCurrentAnimationIndex == m_iForwardDashAnimationIndex)
			{
				m_pModelCom->SetUp_Animation(m_iForwardDashAnimationIndex, true);

				m_fAccSmokeTime += fTimeDelta;
				if (m_fAccSmokeTime > 0.2f)
				{
					m_fAccSmokeTime = 0.f;
					Character_Make_Effect(TEXT("Smoke_Run"));

				}

			}
			else
				m_pModelCom->SetUp_Animation(m_iForwardWalkAnimationIndex, false);

			m_iNextAnimation.first = m_iIdleAnimationIndex;
			m_iNextAnimation.second = 100.f;
		}
		else
		{
			if (m_pModelCom->m_iCurrentAnimationIndex == m_iForwardDashAnimationIndex)
			{
				m_pModelCom->SetUp_Animation(m_iForwardDashEndAnimationIndex, false);
				//포워드 대시 끊겼을 때
				m_pGameInstance->Stop_Sound(CSound_Manager::SOUND_KEY_NAME::Common_DownHook_Dash_SFX);
				CEffect_Layer::COPY_DESC tDesc{};
				tDesc.pPlayertMatrix = m_pTransformCom->Get_WorldMatrixPtr();
				m_pEffect_Manager->Copy_Layer(TEXT("Smoke03_Stop"), &tDesc);

			}
			else
				m_pModelCom->SetUp_Animation(m_iIdleAnimationIndex, true);

			m_iNextAnimation.first = m_iIdleAnimationIndex;
			m_iNextAnimation.second = 100.f;
		}
	}
}

void CCharacter::MoveKey2Team(_float fTimeDelta)
{
	_short MoveKey = 0;
	if (m_pGameInstance->Key_Pressing(DIK_UP) && m_bJumpLock == false)
	{

		CEffect_Layer::COPY_DESC tDesc{};
		tDesc.pPlayertMatrix = m_pTransformCom->Get_WorldMatrixPtr();
		//점프 먼지
		m_pEffect_Manager->Copy_Layer(TEXT("Smoke01"), &tDesc);
		m_pEffect_Manager->Copy_Layer(TEXT("Smoke01_BackZ"), &tDesc);
		m_pEffect_Manager->Copy_Layer(TEXT("Smoke02"), &tDesc);
		m_pEffect_Manager->Copy_Layer(TEXT("Smoke02_Small"), &tDesc);
		m_pEffect_Manager->Copy_Layer(TEXT("Smoke04"), &tDesc);

		m_pTransformCom->Add_Move({ 0,0.3f,0 });

		//Set_fJumpPower(4.f); //중력Ver1 기준
		Set_fJumpPower(3.f); //중력Ver2 기준


		Set_Animation(m_iJumpAnimationIndex);

		m_fGravityTime = 0.f;

		if (m_pGameInstance->Key_Pressing(DIK_LEFT))
		{
			Set_fImpulse(-5.f);
		}

		else if (m_pGameInstance->Key_Pressing(DIK_RIGHT))
		{
			Set_fImpulse(5.f);
		}

		//점프 시작
		m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Common_Dash_SFX, false, 1.f);
	}

	else if (m_pGameInstance->Key_Pressing(DIK_DOWN))
	{
		if (m_pModelCom->m_iCurrentAnimationIndex != m_iForwardDashAnimationIndex)
		{
			m_pModelCom->SetUp_Animation(m_iCrouchAnimationIndex, true);
		}
	}

	else
	{
		if (m_pGameInstance->Key_Pressing(DIK_LEFT))
		{
			MoveKey -= m_iLookDirection;
		}

		else if (m_pGameInstance->Key_Pressing(DIK_RIGHT))
		{
			MoveKey += m_iLookDirection;
		}


		if (MoveKey == -1)
		{
			m_pModelCom->SetUp_Animation(m_iBackWalkAnimationIndex, false);

			m_iNextAnimation.first = m_iIdleAnimationIndex;

			m_iNextAnimation.second = 100.f;

		}
		else if (MoveKey == 1)
		{
			if (m_pModelCom->m_iCurrentAnimationIndex == m_iForwardDashAnimationIndex)
			{
				m_pModelCom->SetUp_Animation(m_iForwardDashAnimationIndex, true);
				m_fAccSmokeTime += fTimeDelta;
				if (m_fAccSmokeTime > 0.2f)
				{
					m_fAccSmokeTime = 0.f;
					Character_Make_Effect(TEXT("Smoke_Run"));

				}
			}
			else
				m_pModelCom->SetUp_Animation(m_iForwardWalkAnimationIndex, false);

			m_iNextAnimation.first = m_iIdleAnimationIndex;
			m_iNextAnimation.second = 100.f;
		}
		else
		{
			if (m_pModelCom->m_iCurrentAnimationIndex == m_iForwardDashAnimationIndex)
			{
				m_pModelCom->SetUp_Animation(m_iForwardDashEndAnimationIndex, false);
				//포워드 대시 끊겼을 때

				CEffect_Layer::COPY_DESC tDesc{};
				tDesc.pPlayertMatrix = m_pTransformCom->Get_WorldMatrixPtr();
				m_pEffect_Manager->Copy_Layer(TEXT("Smoke03_Stop"), &tDesc);

			}
			else
				m_pModelCom->SetUp_Animation(m_iIdleAnimationIndex, true);

			m_iNextAnimation.first = m_iIdleAnimationIndex;
			m_iNextAnimation.second = 100.f;
		}
	}

}

void CCharacter::Reset_AttackCount()
{
	m_bChaseEnable = true;
	m_bAriDashEnable = true;
}

void CCharacter::Set_bInivisible(_bool bInvisible)
{
	m_bInvisible = bInvisible;
}


/*
AttackColliderResult CCharacter::Set_Hit3(_uint eAnimation, AttackGrade eAttackGrade, AttackType eAttackType, _float fStunTime, _uint iDamage, _float fStopTime, _float2 Impus)
{


	if (m_pModelCom->m_iCurrentAnimationIndex == m_iBreakFall_Air || m_pModelCom->m_iCurrentAnimationIndex == m_iBreakFall_Ground || m_pModelCom->m_iCurrentAnimationIndex == m_iBound_Ground)
		return RESULT_MISS;

	//스턴상태, 땅바닥에 꽂혔을때 검사
	else if (m_bHitGroundSmashed && Get_fHeight() == 0)
	{
		if (eAttackGrade != GRADE_ULTIMATE)
			return RESULT_MISS;
	}

	//스턴 상태가 아니면 가드 체크
	if (m_bStun == false)
	{

		AttackColliderResult eResult = Guard_Check3(eAttackType);

		if (eResult == RESULT_GUARD)
		{
			if (m_pModelCom->m_iCurrentAnimationIndex == m_iCrouchAnimationIndex)
				Set_Animation(m_iGuard_CrouchAnimationIndex);

			else if (m_pModelCom->m_iCurrentAnimationIndex == m_iIdleAnimationIndex || m_pModelCom->m_iCurrentAnimationIndex == m_iBackWalkAnimationIndex)
				Set_Animation(m_iGuard_GroundAnimationIndex);

			else if (m_pModelCom->m_iCurrentAnimationIndex == m_iJumpAnimationIndex || m_pModelCom->m_iCurrentAnimationIndex == m_iFallAnimationIndex)
				Set_Animation(m_iGuard_AirAnimationIndex);

			return RESULT_GUARD;

		}
		else if (eResult != RESULT_HIT)    //회피나 비긴 경우 아니면 일단 속행.   저 경우는 나중에 따로 처리
			return eResult;


	}



	m_bStun = true;

	m_fMaxStunTime = fStunTime;
	m_fAccStunTime = 0.f;

	Set_HitAnimation(eAnimation, Impus);
	Set_AnimationStop(fStopTime);

	Set_bRedHP(true);

	//m_iHP -= iDamage;  // 여기에 콤보계수 곱할것
	m_iHP -= iDamage;  // 여기에 콤보계수 곱할것


	m_iDebugComoboDamage += iDamage;
	cout << "Dagage : " << iDamage << "  ,  Total : " << m_iDebugComoboDamage << endl;


	if (m_iHP < 0)
	{
		m_iHP = 0;
	}

	return RESULT_HIT;
}
*/

//AttackColliderResult CCharacter::Set_Hit4(_uint eAnimation, AttackGrade eAttackGrade, AttackType eAttackType, _float fStunTime, _uint iDamage, _float fStopTime, _short iDirection, _float2 Impus)
AttackColliderResult CCharacter::Set_Hit4(_uint eAnimation, AttackGrade eAttackGrade, AttackType eAttackType, _float fStunTime, _uint iDamage, _float fStopTime, _short iDirection, _float2 Impus, _bool bParticle)
{

	cout << "HP : " << m_iHP << " , Damage :" << iDamage << endl;

	if (m_pModelCom->m_iCurrentAnimationIndex == m_iBreakFall_Air || m_pModelCom->m_iCurrentAnimationIndex == m_iBreakFall_Ground || m_pModelCom->m_iCurrentAnimationIndex == m_iBound_Ground
		|| m_pModelCom->m_iCurrentAnimationIndex == m_iLayUp)
		return RESULT_MISS;

	//스턴상태, 땅바닥에 꽂혔을때 검사
	else if (m_bHitGroundSmashed && Get_fHeight() == 0)
	{
		if (eAttackGrade != GRADE_ULTIMATE)
			return RESULT_MISS;
	}

	m_bInvisible = false;
	HitStopEffect();

	//스턴 상태가 아니면 가드 체크
	if (m_bStun == false)
	{

		AttackColliderResult eResult = Guard_Check3(eAttackType);

		//공격자의 방향이 iDirection  피격자는 마주봐야하니 그 반대.


		if (eResult == RESULT_GUARD)
		{


			if (m_pModelCom->m_iCurrentAnimationIndex == m_iCrouchAnimationIndex)
				Set_Animation(m_iGuard_CrouchAnimationIndex);

			else if (m_pModelCom->m_iCurrentAnimationIndex == m_iIdleAnimationIndex || m_pModelCom->m_iCurrentAnimationIndex == m_iBackWalkAnimationIndex)
				Set_Animation(m_iGuard_GroundAnimationIndex);

			else if (m_pModelCom->m_iCurrentAnimationIndex == m_iJumpAnimationIndex || m_pModelCom->m_iCurrentAnimationIndex == m_iFallAnimationIndex)
				Set_Animation(m_iGuard_AirAnimationIndex);


			//HitMotion { HIT_LIGHT, HIT_CHASE, HIT_MEDIUM, HIT_HEAVY, HIT_HEAVY_DOWN, HIT_CROUCH_MEDIUM, 
			// HIT_KNOCK_AWAY_LEFT, HIT_KNOCK_AWAY_UP, HIT_KNOCK_AWAY_LEFTDOWN, HIT_SPIN_AWAY_LEFTUP, HIT_WALLBOUNCE, HIT_NONE };


			CEffect_Layer::COPY_DESC tDesc{};
			tDesc.pPlayertMatrix = m_pTransformCom->Get_WorldMatrixPtr();
			//가드 이펙트
			switch (eAnimation)
			{
			case Client::HIT_LIGHT:
			case Client::HIT_CHASE:
				m_pEffect_Manager->Copy_Layer(TEXT("Guard01"), &tDesc);
				break;


			case Client::HIT_CROUCH_MEDIUM:
			case Client::HIT_MEDIUM:
				m_pEffect_Manager->Copy_Layer(TEXT("Guard02"), &tDesc);
				break;

			case Client::HIT_HEAVY:
			case Client::HIT_HEAVY_DOWN:
			case Client::HIT_KNOCK_AWAY_LEFT:
			case Client::HIT_KNOCK_AWAY_UP:
			case Client::HIT_KNOCK_AWAY_UP_GRAVITY:
			case Client::HIT_KNOCK_AWAY_LEFTDOWN:
			case Client::HIT_SPIN_AWAY_LEFTUP:
			case Client::HIT_SPIN_AWAY_UP:
				m_pEffect_Manager->Copy_Layer(TEXT("Guard03"), &tDesc);
				break;

			case Client::HIT_WALLBOUNCE:
			case Client::HIT_NONE:
			default:
				break;
			}


			Set_CurrentAnimationPositionJump(0.f);

			return RESULT_GUARD;

		}
		else if (eResult != RESULT_HIT)    //회피나 비긴 경우 아니면 일단 속행.   저 경우는 나중에 따로 처리
			return eResult;


	}


	Set_bAura(false);

	m_bHit = TRUE;
	m_bStun = true;

	//m_fMaxStunTime = fStunTime;

	//누적  콤보수 비례.7배까지,  땅에서는 또 0.9배? 대신 공중에서 1.2배?

	m_fMaxStunTime = fStunTime;// *(30 - CBattleInterface_Manager::Get_Instance()->Get_HitAttackStep(m_iPlayerTeam);

	if (Get_fHeight() > 1)
	{
		m_fMaxStunTime *= 1.1f;
	}

	m_fMaxStunTime *= 1.f - 0.01f * CBattleInterface_Manager::Get_Instance()->Get_HitAttackStep(m_iPlayerTeam);



	m_fAccStunTime = 0.f;

	Set_HitAnimation(eAnimation, Impus,bParticle);
	Set_AnimationStop(fStopTime);

	Set_bRedHP(true);

	if (iDirection == 1)
		FlipDirection(-1);
	else if (iDirection == -1)
		FlipDirection(1);

	//0인경우는 뒤집지 않음


	//m_iHP -= iDamage;  // 여기에 콤보계수 곱할것
	m_iHP -= iDamage;  // 여기에 콤보계수 곱할것


	m_iDebugComoboDamage += iDamage;
	//cout << "Dagage : " << iDamage << "  ,  Total : " << m_iDebugComoboDamage << endl;


	//AttackObject로 옮겨야한다?
	////공격 성공시 히트 이펙트
	//switch (eAnimation)
	//{
	//case Client::HIT_LIGHT:
	//case Client::HIT_CHASE:
	//	m_pEffect_Manager->Copy_Layer(TEXT("BurstU-1"), m_pTransformCom->Get_WorldMatrixPtr());
	//	break;
	//
	//
	//case Client::HIT_CROUCH_MEDIUM:
	//case Client::HIT_MEDIUM:
	//	m_pEffect_Manager->Copy_Layer(TEXT("BurstU-2"), m_pTransformCom->Get_WorldMatrixPtr());
	//	break;
	//
	//case Client::HIT_HEAVY:
	//case Client::HIT_HEAVY_DOWN:
	//case Client::HIT_KNOCK_AWAY_LEFT:
	//case Client::HIT_KNOCK_AWAY_UP:
	//case Client::HIT_KNOCK_AWAY_LEFTDOWN:
	//case Client::HIT_SPIN_AWAY_LEFTUP:
	//	m_pEffect_Manager->Copy_Layer(TEXT("BurstU-2"), m_pTransformCom->Get_WorldMatrixPtr());
	//	break;
	//
	//case Client::HIT_WALLBOUNCE:
	//case Client::HIT_NONE:
	//default:
	//	break;
	//}

	if (m_iHP < 0)
	{
		m_iHP = 0;
	}

	return RESULT_HIT;
}





//void CCharacter::Set_HitAnimation(_uint eAnimation, _float2 Impus)
void CCharacter::Set_HitAnimation(_uint eAnimation, _float2 Impus, _bool bParticle)
{

	//하나라도 0이 아니면 적용
	if (Impus.x != 0 || Impus.y != 0)
	{
		m_fImpuse = Impus;
	}


	m_bWallBounce = true;

	switch (eAnimation)
	{
	case Client::HitMotion::HIT_CHASE:  //break 없음. 고의임
	{
		if (Get_fHeight() == 0)
			m_pTransformCom->Add_Move({ 0.f,0.3f,0.f });
	}

	case Client::HitMotion::HIT_LIGHT:
	{
		_vector position = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector FinalPosition = XMVectorAdd(position, XMVectorSet(0.f, 1.f, 0.f, 0.f));
		if(bParticle)
			CParticle_Manager::Get_Instance()->Play(CParticle_Manager::COMMON_HIT_PARTICLE, FinalPosition);

		if (m_pModelCom->m_iCurrentAnimationIndex == m_iCrouchAnimationIndex)
		{
			Set_Animation(m_iHit_Crouch_AnimationIndex, false);
			//if 적으려다가 취소 
			m_fImpuse.y = 0;


			Set_CurrentAnimationPositionJump(0.f);
		}
		else if (Get_fHeight() > 0)
		{
			Set_Animation(m_iHit_Air_LightAnimationIndex, false);
			Set_ForcedGravityTime_LittleUp();
			Set_CurrentAnimationPositionJump(0.f);

		}
		else
		{
			Set_Animation(m_iHit_Stand_LightAnimationIndex, false);
			m_fImpuse.y = 0;
			Set_CurrentAnimationPositionJump(0.f);

		}
	}
	break;


	case Client::HitMotion::HIT_MEDIUM:
	{
		_vector position = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector FinalPosition = XMVectorAdd(position, XMVectorSet(0.f, 1.f, 0.f, 0.f));
		if (bParticle)
			CParticle_Manager::Get_Instance()->Play(CParticle_Manager::COMMON_HIT_PARTICLE, FinalPosition);

		if (m_pModelCom->m_iCurrentAnimationIndex == m_iCrouchAnimationIndex)
		{
			Set_Animation(m_iHit_Crouch_AnimationIndex, false);
		}
		else if (Get_fHeight() > 0)
		{
			Set_Animation(m_iHit_Air_LightAnimationIndex, false);
			//Set_ForcedGravityTime_LittleUp();
			Set_ForcveGravityTime(0.05f);

		}
		else
			Set_Animation(m_iHit_Stand_LightAnimationIndex, false);

		Set_CurrentAnimationPositionJump(0.f);

	}
	break;
	case Client::HitMotion::HIT_HEAVY:
		break;
	case Client::HitMotion::HIT_HEAVY_DOWN:
	{
		_vector position = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector FinalPosition = XMVectorAdd(position, XMVectorSet(0.f, 1.f, 0.f, 0.f));
		if (bParticle)
			CParticle_Manager::Get_Instance()->Play(CParticle_Manager::COMMON_HIT_PARTICLE, FinalPosition);

		if (Get_fHeight() > 0)
		{
			Set_Animation(m_iHit_Away_LeftDownAnimationIndex, false);
			Set_ForcveGravityTime(0.3f);
			//Set_CurrentAnimationPositionJump(17.f);
			Set_CurrentAnimationPositionJump(0.f);

		}
		else
		{
			Add_Move({ 0.f,0.1f });
			Set_Animation(m_iHit_Air_LightAnimationIndex, false);
			Set_ForcedGravityDown();

			Set_CurrentAnimationPositionJump(58.f);
		}


	}
	break;
	case Client::HitMotion::HIT_CROUCH_MEDIUM:
	{
		_vector position = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector FinalPosition = XMVectorAdd(position, XMVectorSet(0.f, 1.f, 0.f, 0.f));
		if (bParticle)
			CParticle_Manager::Get_Instance()->Play(CParticle_Manager::COMMON_HIT_PARTICLE, FinalPosition);

		Set_Animation(m_iHit_Air_FallAnimationIndex);
		m_pTransformCom->Add_Move({ 0.f,0.3f,0.f });
		Set_ForcedGravityTime_LittleUp();
	}
	break;

	case Client::HitMotion::HIT_KNOCK_AWAY_LEFT_NONEBOUNDE:
	{
		_vector position = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector FinalPosition = XMVectorAdd(position, XMVectorSet(0.f, 1.f, 0.f, 0.f));

		if (bParticle)
			CParticle_Manager::Get_Instance()->Play(CParticle_Manager::COMMON_HIT_PARTICLE, FinalPosition);

		m_bWallBounce = false;
		Set_Animation(m_iHit_Away_LeftAnimationIndex, false);
		//m_pModelCom->CurrentAnimationPositionJump()
		if (Get_fHeight() == 0)
		{
			Add_Move({ 0.f,0.2f });
		}
		//Set_CurrentAnimationPositionJump(0.f);
		Set_ForcedGravityDown();

	}
	break;
	case Client::HitMotion::HIT_KNOCK_AWAY_LEFT:
	{
		_vector position = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector FinalPosition = XMVectorAdd(position, XMVectorSet(0.f, 1.f, 0.f, 0.f));
		if (bParticle)
			CParticle_Manager::Get_Instance()->Play(CParticle_Manager::COMMON_HIT_PARTICLE, FinalPosition);

		m_bWallBounce = true;

		Set_Animation(m_iHit_Away_LeftAnimationIndex, false);
		//m_pModelCom->CurrentAnimationPositionJump()
		if (Get_fHeight() == 0)
		{
			Add_Move({ 0.f,0.2f });
		}
		//Set_CurrentAnimationPositionJump(0.f);

	}
	break;
	case Client::HitMotion::HIT_KNOCK_AWAY_UP:
	{
		_vector position = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector FinalPosition = XMVectorAdd(position, XMVectorSet(0.f, 1.f, 0.f, 0.f));
		if (bParticle)
			CParticle_Manager::Get_Instance()->Play(CParticle_Manager::COMMON_HIT_PARTICLE, FinalPosition);

		m_bAwayUpGravity = false;
		Set_Animation(m_iHit_Away_UpAnimationIndex, false);
		Set_ForcedGravityTime_LittleUp();
	}
	break;
	case Client::HitMotion::HIT_KNOCK_AWAY_UP_GRAVITY:
	{
		_vector position = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector FinalPosition = XMVectorAdd(position, XMVectorSet(0.f, 1.f, 0.f, 0.f));
		if (bParticle)
			CParticle_Manager::Get_Instance()->Play(CParticle_Manager::COMMON_HIT_PARTICLE, FinalPosition);

		m_bAwayUpGravity = true;
		Set_Animation(m_iHit_Away_UpAnimationIndex, false);
		//Set_ForcedGravityTime_LittleUp();
		if (Get_fHeight() == 0)
		{
			Add_Move({ 0.f,0.3f });
		}
		Set_ForcveGravityTime(0.f);

	}
	break;
	case Client::HitMotion::HIT_KNOCK_AWAY_LEFTDOWN:
	{
		_vector position = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector FinalPosition = XMVectorAdd(position, XMVectorSet(0.f, 1.f, 0.f, 0.f));
		if (bParticle)
			CParticle_Manager::Get_Instance()->Play(CParticle_Manager::COMMON_HIT_PARTICLE, FinalPosition);

		Set_Animation(m_iHit_Away_LeftDownAnimationIndex, false);

		//버그 원인같아서 지움
		//Set_ForcedGravityTime_LittleUp();

		//Set_ForcedGravityDown();
		Set_ForcveGravityTime(0.3f);

		m_fImpuse;
		_bool bDebug = true;
	}
	break;
	case Client::HitMotion::HIT_SPIN_AWAY_LEFTUP:
	{
		_vector position = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector FinalPosition = XMVectorAdd(position, XMVectorSet(0.f, 1.f, 0.f, 0.f));
		if (bParticle)
			CParticle_Manager::Get_Instance()->Play(CParticle_Manager::COMMON_HIT_PARTICLE, FinalPosition);

		Set_Animation(m_iHit_Air_Spin_LeftUp, false);

		if (Get_fHeight() == 0)
			m_pTransformCom->Add_Move({ 0.f,0.3f,0.f });

		//Set_ForcedGravityTime_LittleUp();
		Set_ForcveGravityTime(0.f);

	}
	break;

	case Client::HitMotion::HIT_WALLBOUNCE:
	{
		_vector position = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector FinalPosition = XMVectorAdd(position, XMVectorSet(0.f, 1.f, 0.f, 0.f));
		if (bParticle)
			CParticle_Manager::Get_Instance()->Play(CParticle_Manager::COMMON_HIT_PARTICLE, FinalPosition);

		Set_Animation(m_iHit_WallBouce);
	}
	break;
	case Client::HitMotion::HIT_NONE:
	{

	}
	break;

	case Client::HIT_SPIN_AWAY_UP:
	{
		_vector position = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector FinalPosition = XMVectorAdd(position, XMVectorSet(0.f, 1.f, 0.f, 0.f));
		if (bParticle)
			CParticle_Manager::Get_Instance()->Play(CParticle_Manager::COMMON_HIT_PARTICLE, FinalPosition);

		Set_Animation(m_iHit_Air_Spin_Up);
	}
	break;
	default:
		break;
	}

}



void CCharacter::Set_AnimationStop(_float fStopTime)
{
	if (fStopTime != 0)
	{
		m_bAnimationLock = true;
	}
	m_fMaxAnimationLock = fStopTime;
	m_fAccAnimationLock = 0.f;

	m_pModelCom->Play_Animation(0.f);
}

void CCharacter::Set_UnlockAnimationStop()
{

	m_bAnimationLock = false;
	m_fMaxAnimationLock = 0.f;
	m_fAccAnimationLock = 0.f;
}

void CCharacter::Set_AnimationStopWithoutMe(_float fStopTime)
{
	//_float fTest1 = m_ePlayerSlot - (m_iPlayerTeam - 1) * 2;

	//0->0 1->1   2->0  3->1
	CBattleInterface_Manager::Get_Instance()->Stop_CharacterWithoutMe(m_iPlayerTeam, m_ePlayerSlot - (m_iPlayerTeam - 1) * 2, fStopTime);
}






void CCharacter::Check_StunEnd()
{

}

void CCharacter::Stun_Shake()
{
	m_bStunShakeDirection = !m_bStunShakeDirection;

	m_pTransformCom->Add_Move({ 0.02f - m_bStunShakeDirection * (0.04f),0,0 });

}

void CCharacter::Update_AnimationLock(_float fTimeDelta)
{
	m_fAccAnimationLock += fTimeDelta;
	if (m_fAccAnimationLock > m_fMaxAnimationLock)
	{
		m_bAnimationLock = false;
		m_fAccAnimationLock = 0.f;
		m_fMaxAnimationLock = 0.f;
	}

}

void CCharacter::Update_StunImpus(_float fTimeDelta)
{

	//공중에서 맞고 낙하중에는 중력 비슷하게 적용
	//if (m_pModelCom->m_iCurrentAnimationIndex == m_iHit_Air_LightAnimationIndex)
	if (m_pModelCom->m_iCurrentAnimationIndex == m_iHit_Air_LightAnimationIndex || m_pModelCom->m_iCurrentAnimationIndex == m_iHit_Air_FallAnimationIndex)
	{
		;
	}

	//땅에서 약하게 맞았을경우 가속도 최대값을 제한함
	//else if (m_pModelCom->m_iCurrentAnimationIndex == m_iHit_Stand_LightAnimationIndex)
	else if (Check_bCurAnimationisHitGround())
	{
		//음수일때 제한이 안됨
		if (m_fImpuse.x > 0.5f)
		{
			m_fImpuse.x = 0.5f;
		}
		else if (m_fImpuse.x < -0.5f)
		{
			m_fImpuse.x = -0.5f;
		}



		m_pTransformCom->Add_Move({ m_fImpuse.x * fTimeDelta, m_fImpuse.y * fTimeDelta, 0 });

	}

	//그 외에 맞고서 강하게 날라가는 중에는 전용 가속도를 받음
	else
	{
		//벽에 팅길 수 있는 강공격은 그대로 날아가고, 아닌거는 중력까지 받음. 여기서 처리하려다가 중력으로 가, 고싶은데 힘드네?
		//여기서 처리하면 Height<0 인 경우가 처리가 안되서 1프레임 땅에 묻힘

		if (m_bWallBounce)
			m_pTransformCom->Add_Move({ m_fImpuse.x * fTimeDelta, m_fImpuse.y * fTimeDelta, 0 });

		//벽에 튕길 수 있는 공격
		if (m_pModelCom->m_iCurrentAnimationIndex == m_iHit_Away_LeftAnimationIndex)
		{

			//거리가 멀어져서 생긴 가상의 벽
			//if ( fabsf(Get_fPositionX() - m_pEnemy->Get_fPositionX()) > 8)
			//if (Get_fPositionX() < -12.f || Get_fPositionX() > 12.f || fabsf(Get_fPositionX() - m_pEnemy->Get_fPositionX()) > 8)
			if (Check_bWall())
			{

				if (m_bWallBounce)
				{
					Set_Animation(m_iHit_WallBouce);

					Character_Make_Effect(TEXT("Right_Wall_Crash"));

					CMain_Camera* mainCamera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
					mainCamera->StartCameraShake(0.5f, 0.2f);

					Set_AnimationStop(0.2f);

					//playingAnimation == false 에서 처리?
					//Set_NextAnimation(m_iHit_Air_FallAnimationIndex,1.f);

					//맞는순간 보는 방향이 갱신된다면 LookDirection을 써도 될텐데
					//Set_fImpulse({ m_iLookDirection * 2.f,0.5f });

					//팅겨져나오는 거리 수정중
					//Set_fImpulse({ m_iLookDirection * 4.f,0.5f });
					//Set_fImpulse({ m_iLookDirection * 4.f,0.05f });
					Set_fImpulse({ m_iLookDirection * 4.f,0.1f - Get_fHeight() });


					//Set_ForcedGravityTime_LittleUp();
					Set_ForcveGravityTime(0.f);


					//새로운 스턴
					m_fMaxStunTime = 1.f;
					m_fAccStunTime = 0.f;

					m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Common_Crash_Air_Wall_SFX, false, 1.f);
				}
				else
				{
					//애니메이션 끝으로 이동... 할 필요가 있나?
					//if (m_pModelCom->m_iCurrentAnimationIndex== m_iHit_Away_LeftAnimationIndex && m_pModelCom->m_fCurrentAnimPosition < 18.8)
					//{
					//	Set_CurrentAnimationPositionJump(18.99f);
					//}

				}

			}

		}
	}


}

void CCharacter::Set_BreakFall_Ground()
{



	Set_Animation(m_iBreakFall_Ground, 2.f);
	Set_NextAnimation(m_iIdleAnimationIndex, 2.f);

	Set_bRedHP(false);
	Reset_AttackStep();


	if (inputBuffer.size() == 0)
	{
		inputBuffer.push_back(CInput(MOVEKEY_NEUTRAL, ATTACK_NONE));
	}
	DirectionInput iMoveKey = inputBuffer.back().direction;

	if (iMoveKey == MOVEKEY_UP || iMoveKey == MOVEKEY_UP_LEFT)
	{
		Set_fImpulse({ -3.f * m_iLookDirection,1.f });
		Set_ForcveGravityTime(0.f);
	}
	else if (iMoveKey == MOVEKEY_RIGHT)
	{
		//Set_fImpulse({ 0.f , 0.3f });
		Set_fImpulse({ 0.f , 0.1f });

		Set_ForcedGravityTime_LittleUp();
	}
	else if (iMoveKey == MOVEKEY_DOWN)
	{
		Set_Animation(m_iBound_Ground, 2.f);
		Set_NextAnimation(m_iLayUp, 2.f);
	}

	else //if (iMoveKey == MOVEKEY_LEFT)
	{
		Set_fImpulse({ -5.f * m_iLookDirection, 0.f });
		Set_ForcedGravityTime_LittleUp();
	}





}

void CCharacter::BreakFall_Air()
{
	if (m_bStun)
		return;

	//땅바닥에서 질질 끌리는 모션도 공중피격이라 조건 추가해야함
	if ((m_pModelCom->m_iCurrentAnimationIndex == m_iHit_Air_Spin_LeftUp || Check_bCurAnimationisAirHit()) && m_bHitGroundSmashed == false)
	{
		InputCommand();

		CInput InputKey = inputBuffer.back();

		if (InputKey.button != ATTACK_NONE)
		{
			Set_Animation(m_iBreakFall_Air);
			Set_NextAnimation(m_iIdleAnimationIndex, 2.f);
			Set_ForcedGravityDown();

			Reset_AttackStep();


			if (InputKey.direction == MOVEKEY_UP)
			{
				Set_fImpulse({ 0.f, 5.f });
			}
			else if (InputKey.direction == MOVEKEY_DOWN)
			{
				Set_fImpulse({ 0.f, -15.f });
			}

			else if (InputKey.direction == MOVEKEY_LEFT || InputKey.direction == MOVEKEY_UP_LEFT)
			{
				Set_fImpulse({ -10.f * m_iLookDirection, 1.f });
			}

			else //(InputKey.direction == MOVEKEY_RIGHT || InputKey.direction == MOVEKEY_UP_RIGHT)
			{
				Set_fImpulse({ 10.f * m_iLookDirection, 1.f });
			}
			Set_bRedHP(false);


		}

	}
}

void CCharacter::Set_bNoGravity(_bool bNoGravity, _float MaxfNoGravitySafeTime)
{
	m_bNoGravity = bNoGravity;
	m_fNoGravitySafeTime = 0.f;
	m_fMaxfNoGravitySafeTime = MaxfNoGravitySafeTime;

}

_uint CCharacter::Get_iHP()
{
	return m_iHP;
}

_bool CCharacter::Update_Tag_In(_float fTimeDelta)
{
	if (m_bTag_In == false)
		return false;



	m_fAccTag_InTime += fTimeDelta;
	Add_Move({ 0.f,1.f * m_fAccTag_InTime });

	m_pModelCom->Play_Animation(fTimeDelta);

	if (m_fAccTag_InTime > 0.3f)
	{
		m_bTag_In = false;

		Character_Make_Effect(TEXT("Moving_Line_Down"));
		m_pTransformCom->Set_State_Position({ -100.f,-100.f,0.f });
		m_pColliderCom->Update(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

		return true;
	}

	return false;
}

_uint CCharacter::Get_NewCharacterslot()
{
	//짝수면 +1  홀수면 -1
	if (m_ePlayerSlot % 2 == 0)
	{
		//bTag_Succes = CBattleInterface_Manager::Get_Instance()->Tag_CharacterAIO(m_iPlayerTeam, 1, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		return 1;
	}
	else
		return 0;
	//bTag_Succes = CBattleInterface_Manager::Get_Instance()->Tag_CharacterAIO(m_iPlayerTeam, 0, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

}

void CCharacter::Gain_AttackStep(_ushort iStep)
{
	//{ m_iAttackStepCount += iStep; };

	if (m_iPlayerTeam == 1)
		CBattleInterface_Manager::Get_Instance()->Gain_HitAttackStep(iStep, 2);
	else
		CBattleInterface_Manager::Get_Instance()->Gain_HitAttackStep(iStep, 1);
}

void CCharacter::Gain_HitCount(_ushort iHit)
{

	if (iHit == 0)
		return;

	if (m_iPlayerTeam == 1)
		CBattleInterface_Manager::Get_Instance()->Gain_HitCount(iHit, 2);

	else
		CBattleInterface_Manager::Get_Instance()->Gain_HitCount(iHit, 1);

}

/*
_float CCharacter::Get_DamageScale()
{


	//깡으로 더하는건 쉬운데 1히트당 1이 아닌데 따로 더해도 되나  //스파킹도 있는데 받는쪽에서 더하는게 아니라 때리는쪽에 더해야하는거 아님?
	//뎀감비율
	//Step Count	0	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	17 +
	//Next Hit		0%	10%	20% 30% 40% 50% 60% 70% 70% 70% 70% 75% 75% 75% 80% 80% 80% 85%
	//데미지비율    1.0 0.9 0.8 0              0.3

	_uint iAttackStepCount;
	if (m_iPlayerTeam == 1)
		iAttackStepCount = CBattleInterface_Manager::Get_Instance()->Get_HitAttackStep(2);
	else
		iAttackStepCount = CBattleInterface_Manager::Get_Instance()->Get_HitAttackStep(1);


	_float fDamageScale;// = 1.f;

	if (iAttackStepCount <= 7)
	{
		fDamageScale = 1.0f - iAttackStepCount * 0.1f;
	}

	else if (iAttackStepCount <= 10)
	{
		fDamageScale = 0.3f;
	}

	else if (iAttackStepCount <= 13)
	{
		fDamageScale = 0.25f;
	}

	else if (iAttackStepCount <= 16)
	{
		fDamageScale = 0.2f;
	}
	else
	{
		fDamageScale = 0.15f;
	}


	if (m_bSparking)
	{
		//fDamageScale += 0.2f;   //합연산. 너무 큰가?  15%->35%
		fDamageScale *= 1.2f;	  //곱연산 .  15%->16%   너무 작은가 싶지만 원작반영.
	}


	//return fDamageScale;
	return fDamageScale * 0.7f;
}
*/

void CCharacter::Set_GroundSmash(_bool bSmash)
{
	m_bHitGroundSmashed = bSmash;
}

/*
void CCharacter::Guard_Update()
{

	if(Check_bCurAnimationisGroundMove() || m_pModelCom->m_iCurrentAnimationIndex == m_iFallAnimationIndex || m_pModelCom->m_iCurrentAnimationIndex == m_iJumpAnimationIndex)
	{
		if (m_iPlayerTeam == 1)
		{
			if (m_iLookDirection == 1 && m_pGameInstance->Key_Pressing(DIK_A))
			{
				m_bGuard = true;
			}

			else if (m_iLookDirection == -1 && m_pGameInstance->Key_Pressing(DIK_D))
			{
				m_bGuard = true;
			}
			else
				m_bGuard = false;
		}

		else
		{
			if (m_iLookDirection == 1 && m_pGameInstance->Key_Pressing(DIK_LEFT))
			{
				m_bGuard = true;
			}

			else if (m_iLookDirection == -1 && m_pGameInstance->Key_Pressing(DIK_RIGHT))
			{
				m_bGuard = true;
			}
			else
				m_bGuard = false;
		}
	}
	else
	{
		m_bGuard = false;
	}


}
*/

_float CCharacter::Get_DamageScale(_bool bUltimate)
{

	//깡으로 더하는건 쉬운데 1히트당 1이 아닌데 따로 더해도 되나  //스파킹도 있는데 받는쪽에서 더하는게 아니라 때리는쪽에 더해야하는거 아님?
	//뎀감비율
	//Step Count	0	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	17 +
	//Next Hit		0%	10%	20% 30% 40% 50% 60% 70% 70% 70% 70% 75% 75% 75% 80% 80% 80% 85%
	//데미지비율    1.0 0.9 0.8 0              0.3   

	_uint iAttackStepCount;
	if (m_iPlayerTeam == 1)
		iAttackStepCount = CBattleInterface_Manager::Get_Instance()->Get_HitAttackStep(2);
	else
		iAttackStepCount = CBattleInterface_Manager::Get_Instance()->Get_HitAttackStep(1);


	_float fDamageScale;// = 1.f;

	if (iAttackStepCount <= 7)
	{
		fDamageScale = 1.0f - iAttackStepCount * 0.1f;
	}

	else if (iAttackStepCount <= 10)
	{
		fDamageScale = 0.3f;
	}

	else if (iAttackStepCount <= 13)
	{
		fDamageScale = 0.25f;
	}

	else if (iAttackStepCount <= 16)
	{
		fDamageScale = 0.2f;
	}
	else
	{
		fDamageScale = 0.15f;
	}



	//필살기의 경우 최소 35%는 보장
	if (bUltimate)
	{
		if (fDamageScale < 0.35f)
			fDamageScale = 0.35f;

	}


	if (m_bSparking)
	{
		//fDamageScale += 0.2f;   //합연산. 너무 큰가?  15%->35%
		//fDamageScale *= 1.2f;	  //곱연산 .  15%->16%   너무 작은가 싶지만 원작반영.
		fDamageScale += 0.1f;
	}


	//return fDamageScale;
	return fDamageScale * 0.7f;
}

void CCharacter::Set_GrabLoofCount(_ushort iLoofCount)
{
	m_iGrabLoof = iLoofCount;
}

AttackColliderResult CCharacter::Guard_Check3(AttackType eAttackType)
{
	//가드 중에는 어떤 공격 들어와도 무조건 가드 성공
	if (Check_bCurAnimationisGuard())
	{
		//가드 소리
		m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Common_Medium_Guard_SFX, false, 1.f);
		return RESULT_GUARD;
	}


	if (eAttackType == ATTACKTYPE_GRAB_GROUND || eAttackType == ATTACKTYPE_GRAB_AIR || eAttackType == ATTACKTYPE_COMMANDGRAB)
	{
		return CompareGrabType3(eAttackType);
	}

	// 공격이나 행동을 하지 않고 있을때만 가드 체크.  뭔가 하고있었으면 무조건 가드실패.
	if (Check_bCurAnimationisGroundMove() || m_pModelCom->m_iCurrentAnimationIndex == m_iFallAnimationIndex || m_pModelCom->m_iCurrentAnimationIndex == m_iJumpAnimationIndex)
	{




		if (m_iPlayerTeam == 1)
		{
			//보고 있는 방향의 반대를 누르고 있을 시,   상중하단 체크 (잡기 포함)
			if (m_iLookDirection == 1 && m_pGameInstance->Key_Pressing(DIK_A))
			{
				return CompareGuardType3(eAttackType);

			}

			else if (m_iLookDirection == -1 && m_pGameInstance->Key_Pressing(DIK_D))
			{
				return CompareGuardType3(eAttackType);
			}

			//안눌렀으면 가드실패
			else
				return RESULT_HIT;
		}

		else
		{
			//보고 있는 방향의 반대를 누르고 있을 시,   상중하단 체크 (잡기 포함)
			if (m_iLookDirection == 1 && m_pGameInstance->Key_Pressing(DIK_LEFT))
			{
				return CompareGuardType3(eAttackType);
			}

			else if (m_iLookDirection == -1 && m_pGameInstance->Key_Pressing(DIK_RIGHT))
			{
				return CompareGuardType3(eAttackType);
			}

			//안눌렀으면 가드실패
			else
				return RESULT_HIT;
		}
	}
	else
	{
		return RESULT_HIT;
	}
}

_bool CCharacter::CompareGuardType(AttackType eAttackType)
{
	//상단은 무조건 가드 가능
	if (eAttackType == ATTACKTYPE_HIGH)
		return true;

	//중단은 앉아있으면 가드 실패
	else if (eAttackType == ATTACKTYPE_MIDDLE)
	{
		if (m_pModelCom->m_iCurrentAnimationIndex == m_iCrouchAnimationIndex)
			return false;

		else
			return true;
	}

	//하단은 서있으면 가드 실패.  걷는중도 포함되는데  바꿔야한다.      앉아있거나 공중이면 성공?
	else if (eAttackType == ATTACKTYPE_LOW)
	{
		//if (m_pModelCom->m_iCurrentAnimationIndex == m_iIdleAnimationIndex)
		//	return false;
		//
		//else
		//	return true;

		if (m_pModelCom->m_iCurrentAnimationIndex == m_iCrouchAnimationIndex || Get_fHeight() > 0)
			return true;

		else
			return false;



	}

	//잡기는 잡기버튼으로 상쇄 가능
	else if (eAttackType == ATTACKTYPE_GRAB_GROUND)
	{

		//피격된 상대의 높이가 0이 아니거나 잡기 버튼으로 상쇄했으면 실패.
		//if (Get_fHeight != 0 || m_pGameInstance->Key_Pressing(DIK_O))
		//	return false;

		//상대의 높이가 0이 아니면 실패
		if (Get_fHeight() != 0)
			return false;

		//잡기 버튼으로 상쇄했으면 실패.
		else if (m_iPlayerTeam == 1 && m_pGameInstance->Key_Pressing(DIK_O))
			return false;

		else if (m_iPlayerTeam == 2 && m_pGameInstance->Key_Pressing(DIK_NUMPAD9))
			return false;

		else
			return true;


		//대치상태가 아닌 피격중에는 높이 구분 없이 맞는게 정상임

	}

	//공중에서는  공중에 있는 상대한테만 통함.
	else if (eAttackType == ATTACKTYPE_GRAB_AIR)
	{
		//피격된 상대의 높이가 0이거나 잡기 버튼으로 상쇄했으면 실패.
		//if (Get_fHeight == 0 || m_pGameInstance->Key_Pressing(DIK_O))


		//상대가 공중이 아니면 실패
		if (Get_fHeight() == 0)
			return false;

		//상대가 잡기 버튼으로 상쇄했으면 실패.
		else if (m_iPlayerTeam == 1 && m_pGameInstance->Key_Pressing(DIK_O))
			return false;

		else if (m_iPlayerTeam == 2 && m_pGameInstance->Key_Pressing(DIK_NUMPAD9))
			return false;

		else
			return true;
	}

	//기술에 의한 잡기는  점프로만 가능.
	else if (eAttackType == ATTACKTYPE_COMMANDGRAB)
	{
		//높이조건 대신 애니메이션으로 해야하나? 
		if (Get_fHeight() > 0)
			return false;
		else
			return true;
	}



}

AttackColliderResult CCharacter::CompareGuardType3(AttackType eAttackType)
{
	//상단은 무조건 가드 가능
	if (eAttackType == ATTACKTYPE_HIGH)
	{
		//가드 소리
		m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Common_Medium_Guard_SFX, false, 1.f);
		return RESULT_GUARD;
	}

	//중단은 앉아있으면 가드 실패
	else if (eAttackType == ATTACKTYPE_MIDDLE)
	{
		if (m_pModelCom->m_iCurrentAnimationIndex == m_iCrouchAnimationIndex)
			return RESULT_HIT;

		else
			return RESULT_GUARD;
	}

	//하단은 서있으면 가드 실패.  걷는중도 포함되는데  바꿔야한다.      앉아있거나 공중이면 성공?
	else if (eAttackType == ATTACKTYPE_LOW)
	{
		//if (m_pModelCom->m_iCurrentAnimationIndex == m_iIdleAnimationIndex)
		//	return false;
		//
		//else
		//	return true;

		if (m_pModelCom->m_iCurrentAnimationIndex == m_iCrouchAnimationIndex || Get_fHeight() > 0)
			return RESULT_GUARD;

		else
			return RESULT_HIT;



	}





}

AttackColliderResult CCharacter::CompareGrabType3(AttackType eAttackType)
{
	//잡기는 잡기버튼으로 상쇄 가능
	if (eAttackType == ATTACKTYPE_GRAB_GROUND)
	{

		//피격된 상대의 높이가 0이 아니거나 잡기 버튼으로 상쇄했으면 실패.
		//if (Get_fHeight != 0 || m_pGameInstance->Key_Pressing(DIK_O))
		//	return false;

		//상대의 높이가 0이 아니면 실패
		if (Get_fHeight() != 0)
			return RESULT_MISS;

		//잡기 버튼으로 상쇄했으면 실패.
		else if (m_iPlayerTeam == 1 && m_pGameInstance->Key_Pressing(DIK_O) && (m_pModelCom->m_iCurrentAnimationIndex == m_iGrabReadyAnimationIndex || Check_bCurAnimationisGroundMove()))
			return RESULT_DRAW;

		else if (m_iPlayerTeam == 2 && m_pGameInstance->Key_Pressing(DIK_NUMPAD9) && (m_pModelCom->m_iCurrentAnimationIndex == m_iGrabReadyAnimationIndex || Check_bCurAnimationisGroundMove()))
			return RESULT_DRAW;

		else
			return RESULT_HIT;


		//대치상태가 아닌 피격중에는 높이 구분 없이 맞는게 정상임

	}

	//공중에서는  공중에 있는 상대한테만 통함.
	else if (eAttackType == ATTACKTYPE_GRAB_AIR)
	{
		//피격된 상대의 높이가 0이거나 잡기 버튼으로 상쇄했으면 실패.
		//if (Get_fHeight == 0 || m_pGameInstance->Key_Pressing(DIK_O))


		//상대가 공중이 아니면 실패
		if (Get_fHeight() == 0)
			return RESULT_MISS;

		//상대가 잡기 버튼으로 상쇄했으면 실패.
		else if (m_iPlayerTeam == 1 && m_pGameInstance->Key_Pressing(DIK_O) && (m_pModelCom->m_iCurrentAnimationIndex == m_iGrabReadyAnimationIndex || Check_bCurAnimationisAirMove()))
			return RESULT_DRAW;

		else if (m_iPlayerTeam == 2 && m_pGameInstance->Key_Pressing(DIK_NUMPAD9) && (m_pModelCom->m_iCurrentAnimationIndex == m_iGrabReadyAnimationIndex || Check_bCurAnimationisAirMove()))
			return RESULT_DRAW;

		else
			return RESULT_HIT;
	}

	//기술에 의한 잡기는  점프로만 가능.
	else if (eAttackType == ATTACKTYPE_COMMANDGRAB)
	{
		//높이조건 대신 애니메이션으로 해야하나? 
		if (Get_fHeight() > 0)
			return RESULT_MISS;
		else
		{
			//땅에 끌리는중만 아니면 됨
			if (m_pModelCom->m_iCurrentAnimationIndex == m_iHit_Air_LightAnimationIndex && m_pModelCom->m_fCurrentAnimPosition > 55.f && m_bHitGroundSmashed)
				return RESULT_MISS;
		}
		return RESULT_HIT;
	}
}

void CCharacter::Teleport_ToEnemy(_float OffsetX, _float OffsetY)
{

	_vector vTargetPos = static_cast<CTransform*>(m_pEnemy->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);

	vTargetPos += {OffsetX* m_iLookDirection, OffsetY, 0, 0};

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vTargetPos);


}

void CCharacter::Set_Grab(_bool bAir)
{
	m_bGrab = true;
	m_bGrab_Air = bAir;
	m_bGrabDraw = false;
}

void CCharacter::Set_ChaseStop()
{
	m_bChase = false;

	m_fAccChaseTime = 0.f;


	m_fGravityTime = 0.185 - m_pModelCom->m_fCurrentAnimPosition * 0.002f;



}

void CCharacter::Set_ChaseStoping()
{
	m_bChaseStoping = true;

	if (m_pChaseEffectLayer != nullptr)
	{
		m_pChaseEffectLayer->m_bIsDoneAnim = true;
		m_pChaseEffectLayer = nullptr;
	}
}

_ushort CCharacter::Get_BreakFall_AirAnimationIndex()
{
	return m_iBreakFall_Air;
}

_ushort CCharacter::Get_JumpAirAnimationIndex()
{
	return m_iJumpAnimationIndex;
}

_bool CCharacter::Get_bStun()
{
	return m_bStun;
}



void CCharacter::Update_PreviousXPosition()
{
	m_fPreviousX = XMVectorGetX(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
}

_float CCharacter::Get_fCalculatePreviousXPosition()
{
	return  m_fPreviousX - XMVectorGetX(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
}

_float CCharacter::Get_fAbsCalculatePreviousXPosition()
{
	return  abs(m_fPreviousX - XMVectorGetX(m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
}



void CCharacter::OnCollisionEnter(CCollider* other, _float fTimeDelta)
{

	//잡기중에는 겹쳐도 됨
	//if (m_bGrabbed || static_cast<CCharacter*>(other->GetMineGameObject())->Get_bGrabbed() || m_bPlaying == false)
	if (m_bGrabbed || m_bPlaying == false)
		return;


	if (m_iPlayerTeam == 1 && other->m_ColliderGroup == CCollider_Manager::COLLIDERGROUP::CG_2P_BODY)
	{
		//CTransform* pTransofrm = static_cast<CTransform*>(other->GetMineGameObject()->Get_Component(TEXT("Com_Transform")));
		//pTransofrm->Add_Move({ 0.1f,0.f,0.f });


		CCharacter* pCharacter = static_cast<CCharacter*>(other->GetMineGameObject());

		//1. 일단 stun인놈 있으면 그놈이 밀려남
		//if (pCharacter->Get_bStun() == true)
		//{
		//	pCharacter->G
		//}



		_float fMyabsPos = fabsf(Get_fPositionX());
		_float fCharacterabsPos = fabsf(pCharacter->Get_fPositionX());

		if (fMyabsPos > 11.f || fCharacterabsPos > 11.f)
		{
			if (fMyabsPos < fCharacterabsPos)
			{
				//얼마나 밀리는가? 겹친만큼? 
				m_pTransformCom->Add_Move({ -m_iLookDirection * m_pColliderCom->Get_Overlap_X(other) ,0.f,0.f });
			}

		}

		if (m_bStun == true || Check_bCurAnimationisAirHit())
		{
			m_pTransformCom->Add_Move({ -m_iLookDirection * m_pColliderCom->Get_Overlap_X(other),0.f,0.f });
		}

		//else if (m_bStun == true || Check_bCurAnimationisAirHit() || Check_bCurAnimationisGroundSmash())
		//{
		//	m_pTransformCom->Add_Move({ -m_iLookDirection * m_pColliderCom->Get_Overlap_X(other),0.f,0.f });
		//}
		//둘 다 stun상태가 아니고, 땅에있으면
		else //if (pCharacter->Get_fHeight() == 0 && Get_fHeight() == 0)
		{



			//누구 하나 벽에있으면 벽 우선, 절대값이 작은 사람이 밀림.



			//이동량이 적은쪽이 밀려남 .   어느쪽으로? 겹친분량만큼?  양수방향? 음수방향?  보고있는곳의 뒤 겠지 뭐
			//밀려난쪽은 이 이동량을 기반으로 다시 밀어내지 않게 이동량을 업데이트해줘야함
			_float CompareMoveX = pCharacter->Get_fAbsCalculatePreviousXPosition() - Get_fAbsCalculatePreviousXPosition();

			if (CompareMoveX > 0)
			{
				//m_pTransformCom->Add_Move({ m_iLookDirection * -0.05f,0.f,0.f });
				m_pTransformCom->Add_Move({ -m_iLookDirection * m_pColliderCom->Get_Overlap_X(other),0.f,0.f });
				Update_PreviousXPosition();

				pCharacter->Update_PreviousXPosition();
			}
			else if (CompareMoveX == 0)
			{
				//CTransform* pTransofrm = static_cast<CTransform*>(pCharacter->Get_Component(TEXT("Com_Transform")));
				//pTransofrm->Add_Move({ pCharacter->Get_iDirection() * -0.1f,0.f,0.f });
				//pCharacter->Update_PreviousXPosition();


				//공중인 캐릭터가 있으면 그 캐릭터가 밀리기

				_float fEnemyHeight = pCharacter->Get_fHeight();
				_float fMyHeight = Get_fHeight();

				if (fMyHeight > fEnemyHeight)
				{
					m_pTransformCom->Add_Move({ -m_iLookDirection * m_pColliderCom->Get_Overlap_X(other),0.f,0.f });
				}
				//같은경우는 불가능,  낮은경우는 아래서 처리

			}



		}

	}
	else if (m_iPlayerTeam == 2 && other->m_ColliderGroup == CCollider_Manager::COLLIDERGROUP::CG_1P_BODY)
	{
		//CTransform* pTransofrm = static_cast<CTransform*>(other->GetMineGameObject()->Get_Component(TEXT("Com_Transform")));
		//pTransofrm->Add_Move({ 0.1f,0.f,0.f });


		CCharacter* pCharacter = static_cast<CCharacter*>(other->GetMineGameObject());
		//가속도가 높은쪽대로 이동?  공중 vs 땅이면 땅이 우선?

		//1.높이비교.   공중 vs 땅인 경우 땅이 우선.    공중에 있는 캐릭이 밀려남
		//2. 둘 다 땅인경우 속도높은쪽?  달리기가 속도로 처리하진 않을텐데  진짜 이전 x좌표 필요해?
		_float fMyabsPos = fabsf(Get_fPositionX());
		_float fCharacterabsPos = fabsf(pCharacter->Get_fPositionX());

		if (fMyabsPos > 11.f || fCharacterabsPos > 11.f)
		{
			if (fMyabsPos < fCharacterabsPos)
			{
				//얼마나 밀리는가? 겹친만큼? 
				m_pTransformCom->Add_Move({ -m_iLookDirection * m_pColliderCom->Get_Overlap_X(other) ,0.f,0.f });
			}

		}

		if (m_bStun == true || Check_bCurAnimationisAirHit())
		{
			m_pTransformCom->Add_Move({ -m_iLookDirection * m_pColliderCom->Get_Overlap_X(other),0.f,0.f });
		}



		//둘 다 stun상태가 아니고, 땅에있으면
		else //if (pCharacter->Get_fHeight() == 0 && Get_fHeight() == 0)
		{


			//누구 하나 벽에있으면 벽 우선, 절대값이 작은 사람이 밀림.

			_float fMyabsPos = fabsf(Get_fPositionX());
			_float fCharacterabsPos = fabsf(pCharacter->Get_fPositionX());

			if (fMyabsPos > 11.f || fCharacterabsPos > 11.f)
			{
				if (fMyabsPos < fCharacterabsPos)
				{
					//얼마나 밀리는가? 겹친만큼? 
					m_pTransformCom->Add_Move({ -m_iLookDirection * m_pColliderCom->Get_Overlap_X(other)  ,0.f,0.f });
				}

			}

			//이동량이 적은쪽이 밀려남 .   어느쪽으로? 겹친분량만큼?  양수방향? 음수방향?  보고있는곳의 뒤 겠지 뭐
			//밀려난쪽은 이 이동량을 기반으로 다시 밀어내지 않게 이동량을 업데이트해줘야함


			_float CompareMoveX = pCharacter->Get_fAbsCalculatePreviousXPosition() - Get_fAbsCalculatePreviousXPosition();

			if (CompareMoveX > 0)
			{
				//m_pTransformCom->Add_Move({ m_iLookDirection * -0.05f,0.f,0.f });
				m_pTransformCom->Add_Move({ -m_iLookDirection * m_pColliderCom->Get_Overlap_X(other),0.f,0.f });
				Update_PreviousXPosition();

				pCharacter->Update_PreviousXPosition();
			}
			else if (CompareMoveX == 0)
			{
				//CTransform* pTransofrm = static_cast<CTransform*>(pCharacter->Get_Component(TEXT("Com_Transform")));
				//pTransofrm->Add_Move({ pCharacter->Get_iDirection() * -0.1f,0.f,0.f });
				//pCharacter->Update_PreviousXPosition();



			}



		}
	}
}


void CCharacter::OnCollisionStay(CCollider* other, _float fTimeDelta)
{



	//잡기중에는 겹쳐도 됨
	if (m_bGrabbed || static_cast<CCharacter*>(other->GetMineGameObject())->Get_bGrabbed() || m_bPlaying == false || m_bDebugInputLock)
		return;


	_bool debugA = true;
	if (m_iPlayerTeam == 1 && other->m_ColliderGroup == CCollider_Manager::COLLIDERGROUP::CG_2P_BODY)
	{
		//CTransform* pTransofrm = static_cast<CTransform*>(other->GetMineGameObject()->Get_Component(TEXT("Com_Transform")));
		//pTransofrm->Add_Move({ 0.1f,0.f,0.f });


		CCharacter* pCharacter = static_cast<CCharacter*>(other->GetMineGameObject());

		//1. 일단 stun인놈 있으면 그놈이 밀려남
		//if (pCharacter->Get_bStun() == true)
		//{
		//	pCharacter->G
		//}



		_float fMyabsPos = fabsf(Get_fPositionX());
		_float fCharacterabsPos = fabsf(pCharacter->Get_fPositionX());

		if (fMyabsPos > 11.f || fCharacterabsPos > 11.f)
		{
			if (fMyabsPos < fCharacterabsPos)
			{
				//얼마나 밀리는가? 겹친만큼? 
				m_pTransformCom->Add_Move({ -m_iLookDirection * m_pColliderCom->Get_Overlap_X(other) ,0.f,0.f });

				if (pCharacter->Get_bStun())
				{
					m_fImpuse.x = 0;
				}
			}

		}

		if (m_bStun == true || Check_bCurAnimationisAirHit())
		{
			m_pTransformCom->Add_Move({ -m_iLookDirection * m_pColliderCom->Get_Overlap_X(other),0.f,0.f });
			//cout << "AddMove : " << -m_iLookDirection * m_pColliderCom->Get_Overlap_X(other) << endl;
		}

		//둘 다 stun상태가 아니고, 땅에있으면
		else //if (pCharacter->Get_fHeight() == 0 && Get_fHeight() == 0)
		{



			//누구 하나 벽에있으면 벽 우선, 절대값이 작은 사람이 밀림.



			//이동량이 적은쪽이 밀려남 .   어느쪽으로? 겹친분량만큼?  양수방향? 음수방향?  보고있는곳의 뒤 겠지 뭐
			//밀려난쪽은 이 이동량을 기반으로 다시 밀어내지 않게 이동량을 업데이트해줘야함
			_float CompareMoveX = pCharacter->Get_fAbsCalculatePreviousXPosition() - Get_fAbsCalculatePreviousXPosition();

			if (CompareMoveX > 0)
			{
				//m_pTransformCom->Add_Move({ m_iLookDirection * -0.05f,0.f,0.f });
				m_pTransformCom->Add_Move({ -m_iLookDirection * m_pColliderCom->Get_Overlap_X(other),0.f,0.f });
				Update_PreviousXPosition();

				pCharacter->Update_PreviousXPosition();
			}
			else if (CompareMoveX == 0)
			{
				//CTransform* pTransofrm = static_cast<CTransform*>(pCharacter->Get_Component(TEXT("Com_Transform")));
				//pTransofrm->Add_Move({ pCharacter->Get_iDirection() * -0.1f,0.f,0.f });
				//pCharacter->Update_PreviousXPosition();

				_float fEnemyHeight = pCharacter->Get_fHeight();
				_float fMyHeight = Get_fHeight();

				if (fMyHeight > fEnemyHeight)
				{
					m_pTransformCom->Add_Move({ -m_iLookDirection * m_pColliderCom->Get_Overlap_X(other),0.f,0.f });
				}

			}



		}

	}
	else if (m_iPlayerTeam == 2 && other->m_ColliderGroup == CCollider_Manager::COLLIDERGROUP::CG_1P_BODY)
	{
		//CTransform* pTransofrm = static_cast<CTransform*>(other->GetMineGameObject()->Get_Component(TEXT("Com_Transform")));
		//pTransofrm->Add_Move({ 0.1f,0.f,0.f });


		CCharacter* pCharacter = static_cast<CCharacter*>(other->GetMineGameObject());
		//가속도가 높은쪽대로 이동?  공중 vs 땅이면 땅이 우선?

		//1.높이비교.   공중 vs 땅인 경우 땅이 우선.    공중에 있는 캐릭이 밀려남
		//2. 둘 다 땅인경우 속도높은쪽?  달리기가 속도로 처리하진 않을텐데  진짜 이전 x좌표 필요해?
		_float fMyabsPos = fabsf(Get_fPositionX());
		_float fCharacterabsPos = fabsf(pCharacter->Get_fPositionX());

		if (fMyabsPos > 11.f || fCharacterabsPos > 11.f)
		{
			if (fMyabsPos < fCharacterabsPos)
			{
				//얼마나 밀리는가? 겹친만큼? 
				m_pTransformCom->Add_Move({ -m_iLookDirection * m_pColliderCom->Get_Overlap_X(other) ,0.f,0.f });

				if (pCharacter->Get_bStun())
				{
					m_fImpuse.x = 0;
				}
			}

		}

		if (m_bStun == true || Check_bCurAnimationisAirHit())
		{
			m_pTransformCom->Add_Move({ -m_iLookDirection * m_pColliderCom->Get_Overlap_X(other),0.f,0.f });
			//cout << "AddMove : " << -m_iLookDirection * m_pColliderCom->Get_Overlap_X(other) << endl;
		}



		//둘 다 stun상태가 아니고, 땅에있으면
		else //if (pCharacter->Get_fHeight() == 0 && Get_fHeight() == 0)
		{


			//누구 하나 벽에있으면 벽 우선, 절대값이 작은 사람이 밀림.

			_float fMyabsPos = fabsf(Get_fPositionX());
			_float fCharacterabsPos = fabsf(pCharacter->Get_fPositionX());

			if (fMyabsPos > 11.f || fCharacterabsPos > 11.f)
			{
				if (fMyabsPos < fCharacterabsPos)
				{
					//얼마나 밀리는가? 겹친만큼? 
					m_pTransformCom->Add_Move({ -m_iLookDirection * m_pColliderCom->Get_Overlap_X(other)  ,0.f,0.f });
				}

			}

			//이동량이 적은쪽이 밀려남 .   어느쪽으로? 겹친분량만큼?  양수방향? 음수방향?  보고있는곳의 뒤 겠지 뭐
			//밀려난쪽은 이 이동량을 기반으로 다시 밀어내지 않게 이동량을 업데이트해줘야함


			_float CompareMoveX = pCharacter->Get_fAbsCalculatePreviousXPosition() - Get_fAbsCalculatePreviousXPosition();

			if (CompareMoveX > 0)
			{
				//m_pTransformCom->Add_Move({ m_iLookDirection * -0.05f,0.f,0.f });
				m_pTransformCom->Add_Move({ -m_iLookDirection * m_pColliderCom->Get_Overlap_X(other),0.f,0.f });
				Update_PreviousXPosition();

				pCharacter->Update_PreviousXPosition();
			}
			else if (CompareMoveX == 0)
			{
				//CTransform* pTransofrm = static_cast<CTransform*>(pCharacter->Get_Component(TEXT("Com_Transform")));
				//pTransofrm->Add_Move({ pCharacter->Get_iDirection() * -0.1f,0.f,0.f });
				//pCharacter->Update_PreviousXPosition();

				_float fEnemyHeight = pCharacter->Get_fHeight();
				_float fMyHeight = Get_fHeight();

				if (fMyHeight > fEnemyHeight)
				{
					m_pTransformCom->Add_Move({ -m_iLookDirection * m_pColliderCom->Get_Overlap_X(other),0.f,0.f });
				}

			}



		}
	}

}

void CCharacter::OnCollisionExit(CCollider* other)
{
	_bool debugA = true;

}

_float CCharacter::Get_fPositionX()
{
	return 	XMVectorGetX(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
}

_vector CCharacter::Get_vPosition()
{
	return m_pTransformCom->Get_State(CTransform::STATE_POSITION);
}


void CCharacter::Set_bGrabbed(_bool bGrabbed)
{
	m_bGrabbed = bGrabbed;

	//if (m_bGrabbed == false)
	//{
	//	m_bGrabbedGravity = false;
	//}
}

void CCharacter::Set_bGrabbedGravity(_bool bGrabbedGravity)
{
	m_bGrabbedGravity = bGrabbedGravity;
}

_bool CCharacter::Get_bGrabbed()
{
	return m_bGrabbed;
}

void CCharacter::Set_bRedHP(_bool bRedHP)
{
	if (bRedHP == true)
		m_tCharacterDesc.iHp = m_iHP;

	m_bRedHp = bRedHP;
	m_tCharacterDesc.bStun = m_bRedHp;
}

void CCharacter::Set_GrabAnimation()
{
	Set_Animation(m_iGrabAnimationIndex);
}

void CCharacter::Set_bBenishingAttack(_bool bBenishing)
{
	m_bBenishingAttack = bBenishing;
}

_bool CCharacter::Get_bBenishingAttack()
{
	return m_bBenishingAttack;
}

void CCharacter::Add_Move(_float2 fMovement)
{
	m_pTransformCom->Add_Move({ fMovement.x, fMovement.y,0 });
}

void CCharacter::Sparking_ON(_float fTimeDelta)
{

	//스파킹이 꺼져있는경우 켜는지 확인
	if (m_bSparking == false)
	{
		if (m_iPlayerTeam == 1)
		{
			if (m_pGameInstance->Key_Down(DIK_R) && m_pGameInstance->Key_Pressing(DIK_O) && CBattleInterface_Manager::Get_Instance()->Get_bSparkingEnable(m_iPlayerTeam))
			{
				if (Check_bCurAnimationisGroundMove() || Check_bCurAnimationisAirMove() || m_pModelCom->m_iCurrentAnimationIndex == m_iGrabReadyAnimationIndex)
				{
					Set_Animation(m_iSparkingAnimationIndex);
					Set_NextAnimation(m_iIdleAnimationIndex, 3.f);
					CBattleInterface_Manager::Get_Instance()->Set_bSparkingEnable(false, m_iPlayerTeam);
					m_bSparking = true;
					Set_bAura(true);
					//인원수 조건문

					_ushort iAliveMemberCount = CBattleInterface_Manager::Get_Instance()->Get_iAliveMemberCount(m_iPlayerTeam);


					if (m_pGameInstance->Key_Pressing(DIK_HOME))
						m_fMaxSparkingTime = 2000.f;

					else if (iAliveMemberCount == 1)
						m_fMaxSparkingTime = 30.f;
					else
						m_fMaxSparkingTime = 15.f;


					//m_fMaxSparkingTime = 2005.f;

					//UI한테 켠다고 전해주기
					CUI_Manager::Get_Instance()->UsingAttckBuff(m_ePlayerSlot);
					Character_Make_Effect(TEXT("RO"), { 0.f,0.8f });

				}


				//Set_NextAnimation(m_iSparkingAnimationIndex, 100.f);
				//CBattleInterface_Manager::Get_Instance()->Set_bSparkingEnable(false, m_iPlayerTeam);
			}

		}
		else
		{
			if (m_pGameInstance->Key_Down(DIK_PGDN) && m_pGameInstance->Key_Pressing(DIK_NUMPAD9) && CBattleInterface_Manager::Get_Instance()->Get_bSparkingEnable(m_iPlayerTeam))
			{
				if (Check_bCurAnimationisGroundMove() || Check_bCurAnimationisAirMove() || m_pModelCom->m_iCurrentAnimationIndex == m_iGrabReadyAnimationIndex)
				{
					Set_Animation(m_iSparkingAnimationIndex);
					Set_NextAnimation(m_iIdleAnimationIndex, 3.f);
					CBattleInterface_Manager::Get_Instance()->Set_bSparkingEnable(false, m_iPlayerTeam);
					m_bSparking = true;
					Set_bAura(true);

					//인원수 조건문

					_ushort iAliveMemberCount = CBattleInterface_Manager::Get_Instance()->Get_iAliveMemberCount(m_iPlayerTeam);

					if (m_pGameInstance->Key_Pressing(DIK_HOME))
						m_fMaxSparkingTime = 2000.f;

					if (iAliveMemberCount == 1)
						m_fMaxSparkingTime = 30.f;
					else
						m_fMaxSparkingTime = 15.f;

					//UI한테 켠다고 전해주기
					CUI_Manager::Get_Instance()->UsingAttckBuff(m_ePlayerSlot);
					Character_Make_Effect(TEXT("RO"),{0.f,0.8f});
				}
			}
		}
	}



	////이미 켜져있는경우 타이머 체크
	//else
	//{
	//	m_fAccSparkingTime += fTimeDelta;
	//	if (m_fAccSparkingTime > m_fMaxSparkingTime)
	//	{
	//		m_bSparking = false;
	//
	//	}
	//
	//}


}

void CCharacter::Sparking_TimeCount(_float fTimeDelta)
{
	//이미 켜져있는경우 타이머 체크
	if (m_bSparking)
	{
		m_fAccSparkingTime += fTimeDelta;
		if (m_fAccSparkingTime > m_fMaxSparkingTime)
		{
			m_bSparking = false;
			m_fAccSparkingTime = 0.f;
			//UI한테 끈다고 전해주기
			CUI_Manager::Get_Instance()->UsingAttackDestroy(m_ePlayerSlot);
			Set_bAura(false);

		}
		//추가분량.
		else
			Set_bAura(true);

	}
}

_bool CCharacter::Get_bSparking()
{
	return m_bSparking;
}

void CCharacter::Gain_KiAmount(_ushort iKiAmount)
{
	CBattleInterface_Manager::Get_Instance()->Gain_KiGuage(iKiAmount, m_iPlayerTeam);
}

_bool CCharacter::Get_bCharacterDead()
{
	return m_bCharacterDead;
}

void CCharacter::Tag_KeyCheck()
{

	if (m_bAnimationLock)
		return;

	if (m_iPlayerTeam == 1)
	{


		//F3을 눌렀고 << 확정
		// 서있거나,     특정 모션중에 m_bAttackBack==true일때   

		//if (m_pGameInstance->Key_Down(DIK_F3) && m_pModelCom->m_iCurrentAnimationIndex == m_iIdleAnimationIndex)
		if (m_pGameInstance->Key_Down(DIK_F3) &&
			(m_pModelCom->m_iCurrentAnimationIndex == m_iIdleAnimationIndex ||
				((m_pModelCom->m_iCurrentAnimationIndex == m_iAttack_LightLast ||
					m_pModelCom->m_iCurrentAnimationIndex == m_iAttack_Heavy ||
					m_pModelCom->m_iCurrentAnimationIndex == m_iAttack_Crouch_Heavy) && m_bAttackBackEvent)))
		{
			//m_pUI_Manager->UsingChangeCharacher(static_cast<CUI_Define::PLAYER_SLOT>(0));

			//앞 누르고있으면 교대 아니면 어시스트?
			Tag_In(0);
		}
	}
	else if (m_iPlayerTeam == 2)
	{
		//if (m_pGameInstance->Key_Down(DIK_F4) && m_pModelCom->m_iCurrentAnimationIndex == m_iIdleAnimationIndex)
		if (m_pGameInstance->Key_Down(DIK_F4) &&
			(m_pModelCom->m_iCurrentAnimationIndex == m_iIdleAnimationIndex ||
				((m_pModelCom->m_iCurrentAnimationIndex == m_iAttack_LightLast ||
					m_pModelCom->m_iCurrentAnimationIndex == m_iAttack_Heavy ||
					m_pModelCom->m_iCurrentAnimationIndex == m_iAttack_Crouch_Heavy) && m_bAttackBackEvent))) {
			//m_pUI_Manager->UsingChangeCharacher(static_cast<CUI_Define::PLAYER_SLOT>(3));
			Tag_In(0);
		}
	}
}

void CCharacter::Tag_In(_ubyte iTagSlot)
{

	_bool bTag_Succes = false;





	//상대 캐릭터들의 Enemy를 변경함


	//이건 PLAYER_SLOT
	//0->1   1->0,   2->3, 3->2  

	//interface 전용 slot으로 바꾸려면
	//0->1 1->0,   2->1   3->0

	//짝수면 +1  홀수면 -1
	if (m_ePlayerSlot % 2 == 0)
	{
		bTag_Succes = CBattleInterface_Manager::Get_Instance()->Tag_CharacterAIO(m_iPlayerTeam, 1, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	}
	else
		bTag_Succes = CBattleInterface_Manager::Get_Instance()->Tag_CharacterAIO(m_iPlayerTeam, 0, m_pTransformCom->Get_State(CTransform::STATE_POSITION));


	if (bTag_Succes)
	{
		m_bPlaying = false;

		//순간이동 이펙트
		m_bTag_In = true;
		m_fAccTag_InTime = 0.f;

		Set_Animation(m_iJumpAnimationIndex);
		m_pModelCom->Play_Animation(0.f);

		//업데이트끝나면하는걸로?
		//Character_Make_Effect(TEXT("Moving_Line_Down"));
		//m_pTransformCom->Set_State_Position({ -100.f,-100.f,0.f });
		//m_pColliderCom->Update(m_pTransformCom->Get_State(CTransform::STATE_POSITION));


		if (m_bSparking)
		{
			m_bSparking = false;

			//UI한테 끈다고 전해주기
			CUI_Manager::Get_Instance()->UsingAttackDestroy(m_ePlayerSlot);

		}

		if (m_iPlayerTeam == 1)
		{
			m_pUI_Manager->UsingChangeCharacher(static_cast<CUI_Define::PLAYER_SLOT>(0));
		}
		else if (m_iPlayerTeam == 2)
		{
			m_pUI_Manager->UsingChangeCharacher(static_cast<CUI_Define::PLAYER_SLOT>(3));
		}

		Set_bRedHP(false);
	}
}

void CCharacter::RegisterEnemy(CCharacter* pEnemy)
{
	m_pEnemy = pEnemy;
}

void CCharacter::pEnemyCheck()
{
	if (m_pEnemy == nullptr)
	{
		m_pEnemy = CBattleInterface_Manager::Get_Instance()->EnemyInitalize(m_iPlayerTeam);
	}

}


void CCharacter::Tag_Out(_vector vPosition)
{
	//일단은 임시로
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);


	//보고있는 방향쪽으로 돌진할 수 있게 보고있는 방향의 - 16정도 땡겨버려도 될듯
	//근데 이놈이 보고있는방향을 갱신했는가?

	//1. Look값 던져주기

	//맵 밖에서 튀어나올 수 있게?  그런데 이러다가 처맞으면?
	//m_bDynamicMove = true;

	pEnemyCheck();

	//2. 나오는순간 계산하기
	CTransform* pEnemyTransform = static_cast<CTransform*>(m_pEnemy->Get_Component(TEXT("Com_Transform")));

	//적 방향의 X값 체크
	_float fX = XMVectorGetX(pEnemyTransform->Get_State(CTransform::STATE_POSITION) - vPosition);


	//차이가 좁으면 반전 안함. 둘 다 벽에 붙어있을 때 대비.
	if (fabsf(fX) > 0.05)
	{

		if (fX > 0)
		{
			FlipDirection(1);
		}
		else
		{
			FlipDirection(-1);
		}

	}

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition + _vector{ -6.f * m_iLookDirection,0.5f,0.f,0.f });

	m_bPlaying = true;

	m_bTag_In = false;


	m_bAnimationLock = false;
	m_fMaxAnimationLock = 0.f;
	m_fAccAnimationLock = 0.f;


	Chase_Ready(0.4f, true);
	//Chase2(0.2f);
	Chase2(0.3f);



}

void CCharacter::Set_AttackBackEvent(_bool bEvent)
{
	m_bAttackBackEvent = bEvent;
}

void CCharacter::Set_ReflectAttackBackEvent(_bool bEvent)
{
	m_bAttackBackEvent = bEvent;
	m_bReflect = bEvent;

	if (m_pModelCom->m_iCurrentAnimationIndex == m_iReflectAnimationIndex)
	{
		//Set_CurrentAnimationPositionJump(14.f);
		Set_CurrentAnimationPositionJump(25.f);
		//Set_CurrentAnimationPositionJump(30.f);


	}
}

_bool CCharacter::Get_bAttackBackEvent()
{
	return m_bAttackBackEvent;
}

void CCharacter::Set_bGrabDraw(_bool bGrabDraw)
{
	m_bGrabDraw = bGrabDraw;
}

_bool CCharacter::Check_bWall()
{


	if (m_bDynamicMove)
		return false;

	if (Get_fPositionX() < -12.f || Get_fPositionX() > 12.f || fabsf(Get_fPositionX() - m_pEnemy->Get_fPositionX()) > 8)
		return true;

	return false;

}

void CCharacter::Move_ForWall()
{

	_float fPosX = Get_fPositionX();

	if (fPosX < -12.f)
	{
		//-14라면 +2를 해야한다.   14-12
		Add_Move({ -fPosX - 12.f,0.f });

		//그냥 -12로 설정하기엔 y값 구해야함
		//어차피 GetTransform하는데 문제없지않나?

	}
	else if (fPosX > 12.f)
	{
		//14라면 -2를 해야한다.   -14+12

		Add_Move({ -fPosX + 12.f,0.f });
	}
	else  //여기로 들어온 이상 어차피 적과의 거리가 8 이상인거니 그부분은 검사하지 않음
	{
		_float fEnemyfPosX = m_pEnemy->Get_fPositionX();

		//누가 왼쪽? 자신인 경우 더이상 왼쪽으로 못가게
		if (fPosX < fEnemyfPosX)
		{
			Add_Move({ fEnemyfPosX - fPosX - 8.f,0.f });
		}
		else
		{
			Add_Move({ fEnemyfPosX - fPosX + 8.f,0.f });
		}

	}


}

void CCharacter::Set_bDynamicMove(_bool bDynamicMove)
{
	m_bDynamicMove = bDynamicMove;
}

void CCharacter::Update_Dying(_float fTimeDelta)
{

	//m_fMaxDyingTime 바꾸기


	if (m_bDying == false)
	{
		if (m_bGrabbed == true || m_bUnDying == true)
			return;

		if (m_iHP < 1)
		{
			m_bDying = true;

			if (m_bFinalSkillRoundEnd == false)
			{
				if (m_pModelCom->m_iCurrentAnimationIndex == m_iHit_Away_LeftAnimationIndex)
				{
					m_bDynamicMove = true;
					static_cast<CMain_Camera*>(*(m_pGameInstance->Get_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")).begin()))->Set_DyingTeam(m_iPlayerTeam);

					_float2 fMapToImpulse = CMap_Manager::Get_Instance()->Active_DestructiveFinish(m_iLookDirection == -1 ? true : false);

					m_pEnemy->Set_bDynamicMove(true);

					XMStoreFloat4(&m_vDyingPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

					Set_fImpulse(fMapToImpulse);

					m_bDestructiveFinish = true;
					m_fMaxDyingTime = 5.f;
				}

				Set_AnimationStopWithoutMe(2.f);
				Set_AnimationStop(2.f);

				CUI_Manager::Get_Instance()->UsingCreateEndUI();

				_uint iModelAnimationIndex = m_pModelCom->m_iCurrentAnimationIndex;

				//땅에서 맞는모션-> 그냥 주저앉기
				//공중에서 맞았으면 땅바닥에서 Bound 하고 못일어나기


				if (iModelAnimationIndex == m_iHit_Stand_LightAnimationIndex || iModelAnimationIndex == m_iHit_Stand_MediumAnimationIndex || iModelAnimationIndex == m_iHit_Crouch_AnimationIndex)
				{
					Set_Animation(m_iDyingStandingAnimationIndex);
				}
				else// if (Check_bCurAnimationisAirHit())
				{
					Set_NextAnimation(m_iBound_Ground, 200.f);
				}

				Set_bRedHP(false);
			}
			else  // 3필로 끝나면
			{
				//m_fMaxDyingTime = 12.f;
				//m_fMaxDyingTime = 11.5f;

				m_fMaxDyingTime = 11.8f;


				//근데 마지막이면
				if (m_bFinalSkillRoundEndSolo)
				{
					m_fMaxDyingTime = 5.f;
				}

			}




		}


	}

}

_bool CCharacter::Get_bDying()
{
	return m_bDying;
}

void CCharacter::Set_FinalSkillRoundEnd(_bool bSkillRoundEnd, _ushort iIndex)
{
	m_bFinalSkillRoundEnd = bSkillRoundEnd;

	//살아있는 팀원이 없으면
	if (CBattleInterface_Manager::Get_Instance()->Check_survivor(m_iPlayerTeam, Get_NewCharacterslot()) == false)
	{
		m_bFinalSkillRoundEndSolo = true;
		//m_bSoloFinalEndCount = true;
	}
}

void CCharacter::Play_WinAnimation()
{
	//맵 좌표이동, 카메라컷씬, ui나 다른 캐릭터들 안보이게


	//다른캐릭터들 안보이게

	_short iMySlot = Get_NewCharacterslot();

	if (iMySlot == 0)
		iMySlot = 1;
	else if (iMySlot == 1)
		iMySlot = 0;

	CBattleInterface_Manager::Get_Instance()->Set_InvisibleWithoutMe(m_iPlayerTeam, iMySlot);


	//위치는 이게 아니겠지만 일단 설정
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, { -3.f,0,0,1.f });

	Set_Animation(m_iWinAnimationIndex);

	//m_bOnlyCutSceneNoMove = true;

	//CMain_Camera* main_Camera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
	////main_Camera->StartCameraShake(0.2f, 0.2f);
	//main_Camera->StartCameraShake(10.f, 10.f);


	m_bDynamicMove = true;
	m_pEnemy->Set_bDynamicMove(true);

	FlipDirection(1);


	{
		//Change_Level_ForCharacter
		static_cast<CLevel_GamePlay*>(m_pGameInstance->Get_Level())->Change_Level_ForCharacter();
	}


	CUI_Manager::Get_Instance()->CutSceneUI(false);
}

void CCharacter::Play_NewRound_Loser()
{
	//if(m_iPlayerTeam)
	//	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-2.f + (m_iPlayerTeam * 2), 0.f, 0.f, 1.f));

	//m_bDynamicMove = true;

	m_bGrabbed = true;
	m_bOpening = true;
	m_fMaxOpeningTime = 3.5f;

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, {-1.f+(m_iPlayerTeam * 2),0.5f, 0.f, 1.f });
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, { 0.f,0.5f, 0.f, 1.f });
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, { (1.5f - m_iPlayerTeam),0.5f, 0.f, 1.f });
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, { (1.5f - m_iPlayerTeam) * 0.5f,0.5f, 0.f, 1.f });

	Set_Animation(m_iNextRound_RightHandAppear_Cutscene_AnimationIndex);
	//Set_NextAnimation(m_iNextRound_RightHand_AnimationIndex,20.f);
	//Set_NextAnimation(m_iBreakFall_Air, 20.f);
	//Set_fImpulse({ (1.5f - m_iPlayerTeam) * -3.f * m_iLookDirection,2.f });




	CMain_Camera* pMainCamera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
	pMainCamera->Set_Player(this);

	if (m_iPlayerTeam == 1)
	{
		pMainCamera->Play(CMain_Camera::VIRTUAL_CAMERA_COMMON_APPEAR, 0, this, nullptr, true);
		FlipDirection(1);
	}
	else
	{
		pMainCamera->Play(CMain_Camera::VIRTUAL_CAMERA_COMMON_APPEAR, 0, this);
		FlipDirection(-1);
	}

	pMainCamera->StartCameraShake(2.f, 0.2f);
}

void CCharacter::Play_NewRound_Winner()
{
	m_bGrabbed = true;
	m_bOpening = true;
	m_fMaxOpeningTime = 3.5f;

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, { -1.f + (m_iPlayerTeam * 2),0.5f, 0.f, 1.f });

	//되긴 하는데 불안함
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, { 0.f,0.5f, 0.f, 1.f });
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, { (1.5f - m_iPlayerTeam)*0.5f,0.5f, 0.f, 1.f });


	Set_Animation(m_iNextRound_LeftHand_Cutscene_AnimationIndex);
	//Set_NextAnimation(m_iNextRound_LeftHand_AnimationIndex, 20.f);
	//Set_NextAnimation(m_iBreakFall_Air, 20.f);

	//1팀이면 왼쪽 2팀이면 오른쪽

	//-일땐 됐는데 왜 갑자기 이러냐
	//Set_fImpulse({ (1.5f - m_iPlayerTeam) * -3.f * m_iLookDirection,2.f });

	//Set_fImpulse({ (1.5f - m_iPlayerTeam) * -3.f * m_iLookDirection,2.f });

	CMain_Camera* pMainCamera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
	pMainCamera->Set_Player(this);


	if (m_iPlayerTeam == 1)
	{
		FlipDirection(1);
	}
	else
		FlipDirection(-1);
}

void CCharacter::Update_Collider()
{
	m_pColliderCom->Update(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
}

_bool CCharacter::Play_FirstOpening()
{

	Set_AnimationStopWithoutMe(30.f);
	Set_AnimationStop(1.f);

	m_bDynamicMove = true;
	m_bGrabbed = true;
	Set_Animation(m_iStartAnimatonIndex);

	//디버그용임, 테스트하고 나중에 지울 것
	Set_CurrentAnimationPositionJump(0.f);

	m_bInvisible = false;


	//캐릭터마다 카메라 분기처리
	CMain_Camera* mainCamera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
	switch (m_eCharacterIndex)
	{
	case Client::PLAY_GOKU:
		if (m_iPlayerTeam == 1)
		{
			mainCamera->Play(CMain_Camera::VIRTUAL_CAMERA_GOKU_ENTRY, 0, this);
		}
		else if (m_iPlayerTeam == 2)
		{
			//Flip버전
			mainCamera->Play(CMain_Camera::VIRTUAL_CAMERA_GOKU_ENTRY, 1, this, nullptr, true);
		}
		break;
	case Client::PLAY_FRN:
		if (m_iPlayerTeam == 1)
		{
			mainCamera->Play(CMain_Camera::VIRTUAL_CAMERA_FRIEZA_ENTRY, 0, this);
		}
		else if (m_iPlayerTeam == 2)
		{
			mainCamera->Play(CMain_Camera::VIRTUAL_CAMERA_FRIEZA_ENTRY, 1, this, nullptr, true);
		}
		break;
	case Client::PLAY_21:
		if (m_iPlayerTeam == 1)
		{
			mainCamera->Play(CMain_Camera::VIRTUAL_CAMERA_21_ENTRY, 0, this);

		}
		else if (m_iPlayerTeam == 2)
		{
			mainCamera->Play(CMain_Camera::VIRTUAL_CAMERA_21_ENTRY, 1, this, nullptr, true);
		}
		break;
	case Client::PLAY_HIT:
		if (m_iPlayerTeam == 1)
		{
			mainCamera->Play(CMain_Camera::VIRTUAL_CAMERA_HIT_ENTRY, 0, this);
		}
		else if (m_iPlayerTeam == 2)
		{
			mainCamera->Play(CMain_Camera::VIRTUAL_CAMERA_HIT_ENTRY, 1, this, nullptr, true);
		}
		break;
	}


	return true;
}


void CCharacter::Update_Opening(_float fTimeDelta)
{
	if (m_bOpening == false)
		return;


	m_fAccOpeningTime += fTimeDelta;

	if (m_fAccOpeningTime > m_fMaxOpeningTime)
	{
		m_fAccOpeningTime = 0.f;
		m_bOpening = false;
		m_bGrabbed = false;
		m_bDynamicMove = false;

		Add_Move({ (-1.5f + m_iPlayerTeam) * 0.5f,0.f });
		Set_Animation(m_iBreakFall_Air);
		Set_fImpulse({ (-1.5f + m_iPlayerTeam) * 8.f, 4.f });
	}


}

void CCharacter::Set_bPlaying(_bool bPlaying)
{
	m_bPlaying = bPlaying;
}


void CCharacter::Set_StopAllAttackObject(_float fStopTime)
{
	CBattleInterface_Manager::Get_Instance()->Stop_AllAttackObject(fStopTime);
}

_bool CCharacter::Get_bReflect()
{
	return m_bReflect;
}



void CCharacter::Set_bBeReflecting(_short iDirection)
{
	m_bBeReflecting = true;
	//m_fImpuse = { 3.f * iDirection,0.f };
	m_fAccBeReflectingTime = 0.f;

}

_bool CCharacter::Update_BeReflecting(_float fTimeDelta)
{
	if (m_bBeReflecting)
	{
		m_fAccBeReflectingTime += fTimeDelta;



		Add_Move({ (0.3f - m_fAccBeReflectingTime) * m_iLookDirection * -20.f * fTimeDelta, 0.f });

		if (m_fAccBeReflectingTime > 0.3)
		{
			m_bBeReflecting = false;
			m_fAccBeReflectingTime = 0.f;
		}

	}

	return true;
}

void CCharacter::Set_bFinalSkillQTE(_bool bFinalSkillQTE)
{
	m_iQTE = bFinalSkillQTE;
}

CHARACTER_INDEX CCharacter::Get_eCharacterIndex()
{
	return m_eCharacterIndex;
}




void CCharacter::Set_AnimationMoveXZ(_bool bValue)
{
	m_pModelCom->m_bNoMoveXZ = bValue;
	m_bCinematic_NoMoveXZ = bValue;


}

void CCharacter::Set_bHeavySkill(_bool bHeavySkill)
{
	m_bHeavySkill = bHeavySkill;
}

void CCharacter::Set_bForcedAura(_bool bForcedAura)
{
	m_bForcedAura = bForcedAura;
}

void CCharacter::Set_bAura(_bool bAura)
{
	if (bAura != m_bAura)
	{
		if (m_bAura == true && bAura == false)
		{
			LIGHT_DESC* pLight_Desc = m_pRenderInstance->Get_LightDesc(CLight_Manager::LIGHT_PLAYER, 0, m_strName);
			pLight_Desc->vAuraColor = _float4(0.f, 0.f, 0.f, 0.f);
			m_bAura = false;
		}

		else if (m_bAura == false && bAura == true)
		{
			LIGHT_DESC* pLight_Desc = m_pRenderInstance->Get_LightDesc(CLight_Manager::LIGHT_PLAYER, 0, m_strName);
			pLight_Desc->vAuraColor = m_fAuraColor;
			m_bAura = true;
		}
	}



}

void CCharacter::Set_fAuraColor(_float4 fAuraColor)
{
	LIGHT_DESC* pLight_Desc = m_pRenderInstance->Get_LightDesc(CLight_Manager::LIGHT_PLAYER, 0, m_strName);
	pLight_Desc->vAuraColor = fAuraColor;
}

void CCharacter::Reset_AttackStep()
{
	CBattleInterface_Manager::Get_Instance()->Reset_HitCount(m_iPlayerTeam);
	CBattleInterface_Manager::Get_Instance()->Reset_HitAttackStep(m_iPlayerTeam);

	Set_bRedHP(false);
}

void CCharacter::Update_NoEventAnimationLoof(_float fTimeDelta)
{

	_float fTickPersecond = m_pModelCom->m_Animations[0]->m_fTickPerSecond;
	_float fAfterAnimationPostion = m_pModelCom->m_fCurrentAnimPosition + fTickPersecond * fTimeDelta;


	if (fAfterAnimationPostion >= m_fNoEventLoofMaxPosition)
	{
		fAfterAnimationPostion = m_fNoEventLoofMinPosition + fmod(fAfterAnimationPostion - m_fNoEventLoofMaxPosition, m_fNoEventLoofMaxPosition - m_fNoEventLoofMinPosition);
		Set_CurrentAnimationPositionJump(fAfterAnimationPostion);
	}
	else
		m_pModelCom->Play_Animation_Lick(fTimeDelta);


}

void CCharacter::Update_ForcedEventAnimationLoof(_float fTimeDelta)
{

	_float fPrePosition = m_pModelCom->m_fCurrentAnimPosition;

	_float fTickPersecond = m_pModelCom->m_Animations[0]->m_fTickPerSecond;
	_float fAfterAnimationPostion = m_pModelCom->m_fCurrentAnimPosition + fTickPersecond * fTimeDelta;


	if (fAfterAnimationPostion >= m_fNoEventLoofMaxPosition)
	{
		fAfterAnimationPostion = m_fNoEventLoofMinPosition + fmod(fAfterAnimationPostion - m_fNoEventLoofMaxPosition, m_fNoEventLoofMaxPosition - m_fNoEventLoofMinPosition);
		Set_CurrentAnimationPositionJump(fAfterAnimationPostion);
	}
	else
		m_pModelCom->Play_Animation_Lick(fTimeDelta);


	_float fCurPosition = m_pModelCom->m_fCurrentAnimPosition;

	//평범하게 진행됐으면 이벤트도 평범하게 사용
	if (fPrePosition < fCurPosition)
		ProcessEventsBetweenFrames2(0, m_pModelCom->m_iCurrentAnimationIndex, fPrePosition, fCurPosition);
	else if (fPrePosition > fCurPosition) //한바퀴 돈 경우 최소부터 현재 사이에 있는 이벤트 사용
	{
		ProcessEventsBetweenFrames2(0, m_pModelCom->m_iCurrentAnimationIndex, m_fNoEventLoofMinPosition, fCurPosition);
	}
}

void CCharacter::Update_NoEventTime(_float fTimeDelta)
{
	m_fAccNoEventLoofTime += fTimeDelta;
	if (m_fAccNoEventLoofTime > m_fMaxNoEventLoofTime)
	{
		m_bNoEventLoofAnimation = false;
		m_fAccNoEventLoofTime = 0.f;
		m_fMaxNoEventLoofTime = 0.f;

		m_bForcedEventLoofAnimation = false;
	}
}

void CCharacter::Set_NoEventAnmationLoof(_float fMinPosition, _float fMaxPosition, _float fTime)
{
	m_bNoEventLoofAnimation = true;
	m_fNoEventLoofMinPosition = fMinPosition;
	m_fNoEventLoofMaxPosition = fMaxPosition;
	m_fAccNoEventLoofTime = 0.f;
	m_fMaxNoEventLoofTime = fTime;

}

void CCharacter::Set_EventAnmationLoof(_float fMinPosition, _float fMaxPosition, _float fTime)
{
	m_bNoEventLoofAnimation = true;
	m_fNoEventLoofMinPosition = fMinPosition;
	m_fNoEventLoofMaxPosition = fMaxPosition;
	m_fAccNoEventLoofTime = 0.f;
	m_fMaxNoEventLoofTime = fTime;

	m_bForcedEventLoofAnimation = true;
}

_float4x4 CCharacter::Make_BoneMatrix(char* BoneName)
{


	//XMMATRIX matrixBone = XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr("GD_fist_R"));
	//XMMATRIX matrixBody = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
	//XMMATRIX resultMatrix = matrixBone * matrixBody;

	_float4x4 tFinalMatrix;
	//XMStoreFloat4x4(&tFinalMatrix, resultMatrix);

	//XMStoreFloat4x4(&tFinalMatrix, matrixBone * matrixBody);

	XMStoreFloat4x4(&tFinalMatrix, XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr(BoneName)) * XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));



	return tFinalMatrix;

}

void CCharacter::Character_Make_BoneEffect_Offset(char* BoneName, _wstring strEffectName, _float2 fOffset, _bool bFlipDirection)
{

	_float4x4 Result4x4;

	if (fOffset.x == 0 && fOffset.y == 0 && bFlipDirection == false)
	{
		Result4x4 = Make_BoneMatrix(BoneName);
	}
	else
	{
		Result4x4 = Make_BoneMatrix_Offset(BoneName, fOffset, bFlipDirection);
	}

	CEffect_Layer::COPY_DESC tDesc{};
	tDesc.pPlayertMatrix = &Result4x4;
	tDesc.pTransformCom = m_pTransformCom;
	CEffect_Manager::Get_Instance()->Copy_Layer(strEffectName, &tDesc);


}

_float4x4 CCharacter::Make_BoneMatrix_Offset(char* BoneName, _float2 fOffset, _bool bFlipDirection)
{
	//XMMATRIX matrixBone = XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr("GD_fist_R"));
	XMMATRIX matrixBody = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
	//XMMATRIX resultMatrix = matrixBone * matrixBody;

	//Body의 x좌표
	matrixBody.r[3].m128_f32[0] += fOffset.x * m_iLookDirection;
	matrixBody.r[3].m128_f32[1] += fOffset.y;


	if (bFlipDirection)
		matrixBody.r[0].m128_f32[0] *= -1.f;

	_float4x4 tFinalMatrix;
	XMStoreFloat4x4(&tFinalMatrix, XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr(BoneName)) * matrixBody);

	return tFinalMatrix;
}

CEffect_Layer* CCharacter::Character_Make_BoneEffect(char* BoneName, _wstring strEffectName)
{
	//CEffect_Manager::Get_Instance()->Copy_Layer(strEffectName, &Make_BoneMatrix(BoneName));

	//static XMFLOAT4X4 s = Make_BoneMatrix(BoneName);

	//CEffect_Manager::Get_Instance()->Copy_Layer(strEffectName, m_pTransformCom->Get_WorldMatrixPtr());
	CEffect_Layer::COPY_DESC tDesc{};
	tDesc.pPlayertMatrix = m_pModelCom->Get_BoneMatrixPtr(BoneName);
	tDesc.pTransformCom = m_pTransformCom;
	return CEffect_Manager::Get_Instance()->Copy_Layer_AndGet(strEffectName, &tDesc);
}






_float4x4 CCharacter::Character_Make_Matrix(_float2 fOffset, _bool bFlipDirection)
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float3 fPos;
	XMStoreFloat3(&fPos, vPos);

	//_float ScaleX = (_float)Get_iDirection() * (1 - (2 * bFlipDirection));


	_matrix ovelapMatrix = XMMatrixScaling((_float)Get_iDirection() * (1 - (2 * bFlipDirection)), 1.f, 1.f) * XMMatrixTranslation(fPos.x + (fOffset.x * Get_iDirection()), fPos.y + fOffset.y, fPos.z);

	XMFLOAT4X4 Result4x4;
	XMStoreFloat4x4(&Result4x4, ovelapMatrix);

	return Result4x4;



}



/*
_float4x4 CCharacter::Character_Make_Matrix(_float2 fOffset, _bool bFlipDirection, _float fYRotation)
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float3 fPos;
	XMStoreFloat3(&fPos, vPos);

	//_float ScaleX = (_float)Get_iDirection() * (1 - (2 * bFlipDirection));

	_matrix ovelapMatrix;

	if(fYRotation == 1000)
		 ovelapMatrix = XMMatrixScaling((_float)Get_iDirection() * (1- (2 * bFlipDirection)), 1.f, 1.f)* XMMatrixTranslation(fPos.x + (fOffset.x * Get_iDirection()), fPos.y + fOffset.y, fPos.z);
	else
		ovelapMatrix = XMMatrixScaling((_float)Get_iDirection() * (1 - (2 * bFlipDirection)), 1.f, 1.f)
		* XMMatrixRotationY(fYRotation)  // Replace with desired rotation function and angle
		* XMMatrixTranslation(fPos.x + (fOffset.x * Get_iDirection()), fPos.y + fOffset.y, fPos.z);

	XMFLOAT4X4 Result4x4;
	XMStoreFloat4x4(&Result4x4, ovelapMatrix);

	return Result4x4;
}
*/
/*
_float4x4 CCharacter::Character_Make_Matrix(_float2 fOffset, _bool bFlipDirection, _float3 fScale)
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float3 fPos;
	XMStoreFloat3(&fPos, vPos);

	//_float ScaleX = (_float)Get_iDirection() * (1 - (2 * bFlipDirection));


	_matrix ovelapMatrix = XMMatrixScaling((_float)Get_iDirection() * (1 - (2 * bFlipDirection))*fScale.x, fScale.y, fScale.z) * XMMatrixTranslation(fPos.x + (fOffset.x * Get_iDirection()), fPos.y + fOffset.y, fPos.z);


	XMFLOAT4X4 Result4x4;
	XMStoreFloat4x4(&Result4x4, ovelapMatrix);

	return Result4x4;
}
*/


CEffect_Layer* CCharacter::Character_Make_Effect(_wstring strEffectName, _float2 fOffset, _bool bFlipDirection)
{


	_float4x4 Result4x4;

	if (fOffset.x == 0 && fOffset.y == 0 && bFlipDirection == false)
	{
		XMStoreFloat4x4(&Result4x4, m_pTransformCom->Get_WorldMatrix());
	}
	else
		Result4x4 = Character_Make_Matrix(fOffset, bFlipDirection);


	//CEffect_Manager::Get_Instance()->Copy_Layer(strEffectName, &Result4x4);
	CEffect_Layer::COPY_DESC tDesc{};
	tDesc.pPlayertMatrix = &Result4x4;

	return CEffect_Manager::Get_Instance()->Copy_Layer_AndGet(strEffectName, &tDesc);

}


/*
CEffect_Layer* CCharacter::Character_Make_Effect(_wstring strEffectName, _float2 fOffset, _bool bFlipDirection, _float fYRotation)
{
	_float4x4 Result4x4;

	if (fOffset.x == 0 && fOffset.y == 0 && bFlipDirection == false && fYRotation == 0.f)
	{
		XMStoreFloat4x4(&Result4x4, m_pTransformCom->Get_WorldMatrix());
	}
	else
		Result4x4 = Character_Make_Matrix(fOffset, bFlipDirection,fYRotation);

	//CEffect_Manager::Get_Instance()->Copy_Layer(strEffectName, &Result4x4);
	return CEffect_Manager::Get_Instance()->Copy_Layer_AndGet(strEffectName, &Result4x4);
}
*/

/*
void CCharacter::Character_Make_Effect(_wstring strEffectName, _float2 fOffset, _bool bFlipDirection, _float3 fScale)
{
	_float4x4 Result4x4;

	if (fOffset.x == 0 && fOffset.y == 0 && bFlipDirection == false && fScale.x == 1.f && fScale.y == 1.f && fScale.z == 1.f)
	{
		XMStoreFloat4x4(&Result4x4, m_pTransformCom->Get_WorldMatrix());
	}
	else
		Result4x4 = Character_Make_Matrix(fOffset, bFlipDirection, fScale);

	CEffect_Manager::Get_Instance()->Copy_Layer(strEffectName, &Result4x4);
}
*/

void CCharacter::Character_Create_Distortion(_float3 vDir, _float2 vOffSetPos, _float2 vOffSetScale, _float fLifeTime)
{
	_float4 fPos{};
	XMStoreFloat4(&fPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pRenderInstance->Create_HitDistortion(fPos, { vDir.x * m_iLookDirection, vDir.y, vDir.z }, vOffSetPos, vOffSetScale, fLifeTime);

}

void CCharacter::Character_Create_Distortion(_float3 vDir, _float3 vOffSetPos, _float2 vOffSetScale, _float fLifeTime)
{
	_float4 fPos{};
	XMStoreFloat4(&fPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	fPos.z += vOffSetPos.z;

	m_pRenderInstance->Create_HitDistortion(fPos, { vDir.x * m_iLookDirection, vDir.y, vDir.z }, { vOffSetPos.x, vOffSetPos.y }, vOffSetScale, fLifeTime);

}

void CCharacter::Set_LoofAnimationCreate(_wstring strEffectName, _float fMaxTime, _float fPeriodTime, _float2 fOffset, _bool bFlipDirection)
{

	m_bEffectLoofCreate = true;;
	m_fAccEffectLoofCreateTime = 0.f;
	m_fMaxEffectLoofCreateTime = fMaxTime;
	m_strEffectLoofCreateName = strEffectName;


	//m_fAccEffectPeriodTime = 0.f;
	m_fAccEffectPeriodTime = fPeriodTime;

	m_fMaxEffectPeriodTime = fPeriodTime;

	m_fEffectLoofCreateOffset = fOffset;
	m_bEffectLoofCreateFlip = bFlipDirection;

}

void CCharacter::Update_LoofAnimationCreate(_float fTimeDelta)
{
	if (m_bEffectLoofCreate)
	{
		m_fAccEffectLoofCreateTime += fTimeDelta;
		if (m_fAccEffectLoofCreateTime > m_fMaxEffectLoofCreateTime)
		{
			m_bEffectLoofCreate = false;
			m_fAccEffectLoofCreateTime = 0.f;
		}

		m_fAccEffectPeriodTime += fTimeDelta;
		if (m_fAccEffectPeriodTime > m_fMaxEffectPeriodTime)
		{
			m_fAccEffectPeriodTime = 0.f;
			Character_Make_Effect(m_strEffectLoofCreateName, m_fEffectLoofCreateOffset, m_bEffectLoofCreateFlip);
		}
	}

}

const _float4x4* CCharacter::Get_pTransformMatrix()
{
	return m_pTransformCom->Get_WorldMatrixPtr();

}






_uint* CCharacter::Get_pAnimationIndex()
{
	return &(m_pModelCom->m_iCurrentAnimationIndex);
}

_short CCharacter::Get_iAnimationIndex()
{
	return m_pModelCom->m_iCurrentAnimationIndex;
}



_bool CCharacter::Check_bCurAnimationisAirHit(_uint iAnimation)
{
	_uint iModelIndex = iAnimation;

	if (iAnimation == 1000)
		iModelIndex = m_pModelCom->m_iCurrentAnimationIndex;



	//if (iModelIndex == m_iHit_Air_LightAnimationIndex || iModelIndex == m_iHit_Away_LeftAnimationIndex || iModelIndex == m_iHit_Away_UpAnimationIndex || iModelIndex == m_iHit_Air_FallAnimationIndex)
	if (iModelIndex == m_iHit_Air_LightAnimationIndex || iModelIndex == m_iHit_Air_FallAnimationIndex)
	{
		return true;
	}

	return false;
}

_bool CCharacter::Check_bCurAnimationisHitAway(_uint iAnimation)
{
	_uint iModelIndex = iAnimation;

	if (iAnimation == 1000)
		iModelIndex = m_pModelCom->m_iCurrentAnimationIndex;



	if (iModelIndex == m_iHit_Away_LeftAnimationIndex || iModelIndex == m_iHit_Away_UpAnimationIndex || iModelIndex == m_iHit_Away_LeftDownAnimationIndex)
	{
		return true;
	}

	return false;
}

_bool CCharacter::Check_bCurAnimationisHitGround(_uint iAnimation)
{
	_uint iModelIndex = iAnimation;

	if (iAnimation == 1000)
		iModelIndex = m_pModelCom->m_iCurrentAnimationIndex;



	if (iModelIndex == m_iHit_Stand_LightAnimationIndex || iModelIndex == m_iHit_Stand_MediumAnimationIndex || iModelIndex == m_iHit_Crouch_AnimationIndex)
	{
		return true;
	}

	return false;
}

_bool CCharacter::Check_bCurAnimationisChase(_uint iAnimation)
{
	_uint iModelIndex = iAnimation;

	if (iAnimation == 1000)
		iModelIndex = m_pModelCom->m_iCurrentAnimationIndex;



	if (iModelIndex == m_iChaseAnimationIndex || m_bChase)
	{
		return true;
	}

	return false;
}

_bool CCharacter::Check_bCurAnimationisReflect(_uint iAnimation)
{

	//if (m_pModelCom->m_iCurrentAnimationIndex == m_iReflectAnimationIndex )
	//{
	//	return true;
	//}

	return m_bReflect;

	return false;
}

_bool CCharacter::Check_bCurAnimationisGuard(_uint iAnimation)
{
	//_uint iModelIndex = iAnimation;
	//if (iAnimation == 1000)

	_uint iModelIndex = iModelIndex = m_pModelCom->m_iCurrentAnimationIndex;


	if (iModelIndex == m_iGuard_AirAnimationIndex || iModelIndex == m_iGuard_CrouchAnimationIndex || iModelIndex == m_iGuard_GroundAnimationIndex)
	{
		return true;
	}

	return false;
}

_bool CCharacter::Check_bCurAnimationisGrab(_uint iAnimation)
{
	//_uint iModelIndex = iAnimation;
	//if (iAnimation == 1000)

	_uint iModelIndex = iModelIndex = m_pModelCom->m_iCurrentAnimationIndex;


	if (iModelIndex == m_iGrabAnimationIndex || iModelIndex == m_iGrabReadyAnimationIndex)
	{
		return true;
	}

	return false;
}

_bool CCharacter::Check_bCurAnimationisGroundSmash(_uint iAnimation)
{
	_uint iModelIndex = iModelIndex = m_pModelCom->m_iCurrentAnimationIndex;


	if (m_bHitGroundSmashed)
		return true;

	if (iModelIndex == m_iHit_Air_LightAnimationIndex && m_pModelCom->m_fCurrentAnimPosition >= 55)
	{
		return true;
	}

	return false;
}


void CCharacter::Set_NextAnimation(_uint iAnimationIndex, _float fLifeTime, _float fAnimationPosition)
{
	m_iNextAnimation.first = iAnimationIndex;
	m_iNextAnimation.second = fLifeTime;


	m_fNextAnimationCurrentPosition = fAnimationPosition;

}

void CCharacter::AttackNextMoveCheck()
{
	if (m_iNextAnimation.first != m_iIdleAnimationIndex)
	{


		if (m_bNextAnimationGravityEvent) 	//Set_Animation보다 위에 있어야함
		{
			m_bNextAnimationGravityEvent = false;
			Set_ForcedGravityTime_LittleUp();
		}

		Set_Animation(m_iNextAnimation.first);

		m_iNextAnimation.first = m_iIdleAnimationIndex;
		m_iNextAnimation.second = 1000.f;

		if (m_fNextAnimationCurrentPosition != 0)
		{
			m_pModelCom->CurrentAnimationPositionJump(m_fNextAnimationCurrentPosition);
			m_fNextAnimationCurrentPosition = 0.f;
		}

	}
}

void CCharacter::AnimeEndNextMoveCheck()
{

	Set_Animation(m_iNextAnimation.first);

	m_iNextAnimation.first = m_iIdleAnimationIndex;
	m_iNextAnimation.second = 1000.f;



}

void CCharacter::Set_Animation(_uint iAnimationIndex, _bool bloof)
{



	m_bMotionPlaying = true;

	m_bAttackBackEvent = false;

	if (iAnimationIndex == m_iIdleAnimationIndex)
		m_pModelCom->SetUp_Animation(iAnimationIndex, true);
	else
		m_pModelCom->SetUp_Animation(iAnimationIndex, bloof);

	if (iAnimationIndex == m_iHit_Air_LightAnimationIndex || iAnimationIndex == m_iHit_Stand_LightAnimationIndex || iAnimationIndex == m_iHit_Stand_MediumAnimationIndex)
	{
		m_pModelCom->CurrentAnimationPositionJump(0.f);
	}

}

void CCharacter::Set_IdleAnimation()
{
	Set_Animation(m_iIdleAnimationIndex);
}

void CCharacter::Gravity(_float fTimeDelta)
{




	if (m_bChase == true)
	{
		return;
	}

	if (m_bNoGravity == true)
	{
		m_fNoGravitySafeTime += fTimeDelta;

		if (m_fNoGravitySafeTime > m_fMaxfNoGravitySafeTime)
		{
			m_bNoGravity = false;
			m_fNoGravitySafeTime = 0.f;
		}

		return;
	}

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float fHeight = XMVectorGetY(vPos);



	if (fHeight > 0)
	{



		// IDLE이면 공중 하강모션으로 변경


		if (m_bStun)
		{

		}


		else if (Check_bCurAnimationisGroundMove())
			m_pModelCom->SetUp_Animation(m_iFallAnimationIndex, false);


		// if (Check_bCurAnimationisGroundMove())
		//m_pModelCom->SetUp_Animation(m_iFallAnimationIndex, false);

		//중력 ver1 일때는 항상 더해야 자연스러운데 2에서는 아님
		//if (m_fGravityTime < m_fJumpPower)
		//{
		//	m_fGravityTime += fTimeDelta;
		//}


		//ver1  괜찮은데 너무 둥실둥실함  점프력4 베이스.
		//_float fGravity = ( - 0.3f * (m_fGravityTime - m_fJumpPower) * (m_fGravityTime - m_fJumpPower) + 2) *0.03f;

		//ver  점프에 속도감이 좀 느껴지고 전체적으로 낮음, 점프력 3 베이스.
		//_float fGravity = (-0.7f * (m_fGravityTime - m_fJumpPower) * (m_fGravityTime - m_fJumpPower) + 4) * 0.03f;

		//Ver3  점프력 3
		//_float fGravity = (-0.7f * (m_fGravityTime - m_fJumpPower) * (m_fGravityTime - m_fJumpPower) + 4) * 0.05f;


		//1~3짜리 전부 실수로 Gravity 두번씩 호출함.  

		//Ver4 점프력3.   마지막 값을 *0.1 대신 *0.08해도 자연스러움
		//_float fGravity = (-0.7f * (m_fGravityTime - m_fJumpPower) * (m_fGravityTime - m_fJumpPower) + 4) * 0.1;
		_float fGravity = (-0.7f * (2 * m_fGravityTime - m_fJumpPower) * (2 * m_fGravityTime - m_fJumpPower) + 4) * 0.1;

		if (m_pModelCom->m_iCurrentAnimationIndex == m_iHit_Away_LeftDownAnimationIndex && fGravity < 0)
		{
			_bool bDebug = true;

		}



		//점프시작, 점프중에 중력 따름 
		//if (m_pModelCom->m_iCurrentAnimationIndex == m_iFallAnimationIndex || m_pModelCom->m_iCurrentAnimationIndex == m_iJumpAnimationIndex)

		//공중 약공격으로 맞는중 추가
		//if (m_pModelCom->m_iCurrentAnimationIndex == m_iFallAnimationIndex || m_pModelCom->m_iCurrentAnimationIndex == m_iJumpAnimationIndex || m_pModelCom->m_iCurrentAnimationIndex == m_iHit_Air_LightAnimationIndex)

		//공중 기본공격중 추가( 기탄제외)
		//if (m_pModelCom->m_iCurrentAnimationIndex == m_iFallAnimationIndex || m_pModelCom->m_iCurrentAnimationIndex == m_iJumpAnimationIndex ||
		//	m_pModelCom->m_iCurrentAnimationIndex == m_iAttack_Air1 || m_pModelCom->m_iCurrentAnimationIndex == m_iAttack_Air2 || m_pModelCom->m_iCurrentAnimationIndex == m_iAttack_Air3 ||
		//	m_pModelCom->m_iCurrentAnimationIndex == m_iHit_Air_LightAnimationIndex)
		if (m_pModelCom->m_iCurrentAnimationIndex == m_iFallAnimationIndex || m_pModelCom->m_iCurrentAnimationIndex == m_iJumpAnimationIndex ||
			m_pModelCom->m_iCurrentAnimationIndex == m_iAttack_Air1 || m_pModelCom->m_iCurrentAnimationIndex == m_iAttack_Air2 || m_pModelCom->m_iCurrentAnimationIndex == m_iAttack_Air3 ||
			m_pModelCom->m_iCurrentAnimationIndex == m_iAttack_AirUpper || m_pModelCom->m_iCurrentAnimationIndex == m_iBreakFall_Ground ||
			m_pModelCom->m_iCurrentAnimationIndex == m_iHit_Air_Spin_LeftUp || m_pModelCom->m_iCurrentAnimationIndex == m_iHit_Air_Spin_Up ||
			m_pModelCom->m_iCurrentAnimationIndex == m_iHit_WallBouce ||
			Check_bCurAnimationisAirHit() || Check_bCurAnimationisHitAway() || m_pModelCom->m_iCurrentAnimationIndex == m_iGuard_AirAnimationIndex
			|| Check_bCurAnimationisHalfGravityStop())
		{




			//스매시 당했으면 시간 더하지 않음.   공중 아래강 중에도 더하지 않음
			// 
			//다만 벽에 안팅기는 우측 스매시의 경우 더함
			if (m_pModelCom->m_iCurrentAnimationIndex == m_iHit_Away_LeftAnimationIndex && m_bWallBounce == false)
			{
				m_fGravityTime += fTimeDelta;
				m_pTransformCom->Add_Move({ m_fImpuse.x * fTimeDelta,-fGravity,0 });
			}

			else if (m_pModelCom->m_iCurrentAnimationIndex == m_iHit_Away_UpAnimationIndex && m_bAwayUpGravity)
			{
				m_fGravityTime += fTimeDelta;
				m_pTransformCom->Add_Move({ m_fImpuse.x * fTimeDelta,-fGravity,0 });
			}

			else if (Check_bCurAnimationisHitAway() || m_pModelCom->m_iCurrentAnimationIndex == m_iAttack_AirUpper || (m_bAttackGravity == false && Check_bCurAnimationisHalfGravityStop()))
			{
				;
			}

			else if (m_fGravityTime * 2.f < m_fJumpPower)
			{
				m_fGravityTime += fTimeDelta;

				//일시정지해서 여기서 넘어가면 값이 확 높아질 수 있음.  예외처리해서 시간값 한번 더?
			}
			else if (m_fGravityTime * 2.f > m_fJumpPower)
			{
				m_fGravityTime = m_fJumpPower * 0.5f;
			}


			//일부 공격의 경우  Gravity 가 false면 중력 정용 안함
			if (m_bAttackGravity == false && Check_bCurAnimationisHalfGravityStop())
			{
				;
			}

			else if (m_pModelCom->m_iCurrentAnimationIndex == m_iBreakFall_Ground || m_pModelCom->m_iCurrentAnimationIndex == m_iHit_Air_Spin_LeftUp || m_pModelCom->m_iCurrentAnimationIndex == m_iHit_WallBouce)
			{
				m_pTransformCom->Add_Move({ m_fImpuse.x * fTimeDelta,-fGravity + m_fImpuse.y * fTimeDelta,0 });
			}
			else if (Check_bCurAnimationisHitAway() == false && m_pModelCom->m_iCurrentAnimationIndex != m_iAttack_AirUpper)
				m_pTransformCom->Add_Move({ m_fImpuse.x * fTimeDelta,-fGravity,0 });


		}
		else
		{

			//중력Ver2 전용 처리.  올라가다가 공격때문에 멈췄는데  공격 끝나고 다시 올라가는거 이상해서 처리
			//if (fGravity < 0 && m_fGravityTime < m_fJumpPower)


			if (m_pModelCom->m_iCurrentAnimationIndex == m_iBreakFall_Air)
			{
				m_pTransformCom->Add_Move({ m_fImpuse.x * fTimeDelta, m_fImpuse.y * fTimeDelta,0.f });
			}



			if (m_bAttackGravity == true)
			{

				if (fGravity < 0 && m_fGravityTime * 2 < m_fJumpPower)
				{
					m_fGravityTime += fTimeDelta;
				}

			}
			//가속만 받고 중력은 냅두는 코드. 모든 모션에 가속도 적용할꺼 아니면 굉장히 이상하게 보임.
			//m_pTransformCom->Add_Move({ m_fImpuse * fTimeDelta,0,0 });


		}



	}




	//이부분은 Gravity랑 다르게 분리해도될지도 

	//	else if (fHeight <0)   //왜 else if 였는지 기억이 안남. 실수였을지도
	//if (fHeight <0)
	if (fHeight < 0)
	{
		//m_pEffect_Manager->Copy_Layer(TEXT("Smoke01"), m_pTransformCom->Get_WorldMatrixPtr());
		//m_pEffect_Manager->Copy_Layer(TEXT("Smoke01_BackZ"), m_pTransformCom->Get_WorldMatrixPtr());
		//m_pEffect_Manager->Copy_Layer(TEXT("Smoke02"), m_pTransformCom->Get_WorldMatrixPtr());
		//m_pEffect_Manager->Copy_Layer(TEXT("Smoke02_Small"), m_pTransformCom->Get_WorldMatrixPtr());
		//m_pEffect_Manager->Copy_Layer(TEXT("Smoke04"), m_pTransformCom->Get_WorldMatrixPtr());

		//m_pEffect_Manager->Copy_Layer(TEXT("Smoke05"), m_pTransformCom->Get_WorldMatrixPtr());
		//m_pEffect_Manager->Copy_Layer(TEXT("Aura01"), m_pTransformCom->Get_WorldMatrixPtr());

		//m_pEffect_Manager->Copy_Layer(TEXT("BurstU-2"), m_pTransformCom->Get_WorldMatrixPtr());

		//m_pEffect_Manager->Copy_Layer(TEXT("Smoke03_Stop"), m_pTransformCom->Get_WorldMatrixPtr());

		//m_pEffect_Manager->Copy_Layer(TEXT("Smoke_Run"), m_pTransformCom->Get_WorldMatrixPtr());

		//m_pEffect_Manager->Copy_Layer(TEXT("Guard03"), m_pTransformCom->Get_WorldMatrixPtr());

		//if (m_pModelCom->m_iCurrentAnimationIndex == m_iFallAnimationIndex || Check_bCurAnimationisAirAttack())

		if (m_iPlayerTeam == 1)
		{
			_bool bDebug = true;
		}


		//먼지
		CEffect_Layer::COPY_DESC tDesc{};
		tDesc.pPlayertMatrix = m_pTransformCom->Get_WorldMatrixPtr();
		m_pEffect_Manager->Copy_Layer(TEXT("Smoke05"), &tDesc);
		m_pEffect_Manager->Copy_Layer(TEXT("Aura01"), &tDesc);


		if (m_pModelCom->m_iCurrentAnimationIndex == m_iFallAnimationIndex || Check_bCurAnimationisAirAttack() || m_pModelCom->m_iCurrentAnimationIndex == m_iBreakFall_Air)
		{
			m_pModelCom->SetUp_Animation(m_iIdleAnimationIndex, true);

			Set_fGravityTime(0.f);
			//Set_fJumpPower(0.f);
			Set_fImpulse(0.f);

			Set_NextAnimation(m_iIdleAnimationIndex, 2.f);
			m_bAriDashEnable = true;
			m_bChaseEnable = true;
			Set_bAttackGravity(true);

			if (m_bJumpLock == false)
			{
				m_bJumpLock = true;
			}

			//사뿐히 점프해서 땅 착지
			m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Common_Dash_SFX, false, 1.f);
		}
		else if (Check_bCurAnimationisAirHit() || Check_bCurAnimationisHitAway())
		{
			//m_pModelCom->SetUp_Animation(m_iIdleAnimationIndex, true);

			if (m_bHitGroundSmashed)
			{
				Set_Animation(m_iHit_Air_LightAnimationIndex);
				m_pModelCom->CurrentAnimationPositionJump(55.f);
				Set_NextAnimation(m_iBound_Ground, 5.f);

				//m_bHitGroundSmashed = false;
				//m_pTransformCom->Add_Move({ -1 * fTimeDelta * m_iLookDirection, 0, 0 });

				//쎄게 꼬라박힐 떄
				m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Smash_Hit_SFX, false, 1.f);
				CMain_Camera* mainCamera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
				mainCamera->StartCameraShake(0.3f, 0.2f);
			}
			else
			{
				//m_pModelCom->SetUp_Animation(m_iBound_Ground, false);

				//백업
				//Set_Animation(m_iBreakFall_Ground, 2.f);
				//Set_fImpulse({ 5.f,0.f });
				Set_BreakFall_Ground();

				Set_NextAnimation(m_iIdleAnimationIndex, 2.f);
				//Set_fGravityTime(0.f);

				//천천히 꼬라박힐 때
				m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Smash_Hit_SFX, false, 1.f);
			}


			if (m_pModelCom->m_iCurrentAnimationIndex != m_iBreakFall_Ground)
			{
				Set_fGravityTime(0.f);
				Set_fImpulse(0.f);
			}


			m_bAriDashEnable = true;
			Set_bAttackGravity(true);

			if (m_bJumpLock == false)
			{
				m_bJumpLock = true;
			}

		}
		if (m_bHitGroundSmashed == false)
		{
			m_bStun = false;
			m_fAccStunTime = 0.f;
		}
		//m_pTransformCom->Add_Move({ 0,-fHeight,0 });

		if (m_bDying)
		{
			Set_Animation(m_iBound_Ground, false);
		}

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, { XMVectorGetX(vPos),0.f,XMVectorGetZ(vPos),1.f });

	}

	else if (fHeight == 0)
	{

		if (m_bDying)
			return;

		if (m_pModelCom->m_iCurrentAnimationIndex == m_iFallAnimationIndex || m_pModelCom->m_iCurrentAnimationIndex == m_iJumpAnimationIndex)
		{
			m_pModelCom->SetUp_Animation(m_iIdleAnimationIndex, true);

			Set_fGravityTime(0.f);
			//Set_fJumpPower(0.f);
			Set_fImpulse(0.f);
			m_bAriDashEnable = true;
			Set_bAttackGravity(true);
		}

		//else if (m_bHitGroundSmashed == true && m_pModelCom->m_iCurrentAnimationIndex == m_iHit_Air_LightAnimationIndex)
		//{
		//	if (m_bMotionPlaying == false)
		//	{
		//		Set_Animation(m_iBreakFall_Ground, 2.f);
		//		Set_NextAnimation(m_iIdleAnimationIndex, 2.f);
		//		m_bHitGroundSmashed = false;
		//	}
		//
		//}
		//else if (m_bHitGroundSmashed == true && m_pModelCom->m_iCurrentAnimationIndex == m_iHit_Air_LightAnimationIndex)
		else if (m_bHitGroundSmashed == true)
		{
			if (m_pModelCom->m_iCurrentAnimationIndex == m_iHit_Air_LightAnimationIndex)
			{
				//바닥에 꼬라박혔을때 질질 끌림.  보기에 이상하다면 맞았을 시에 FlipDirection 해야한다
				m_pTransformCom->Add_Move({ -2 * fTimeDelta * m_iLookDirection, 0, 0 });

				m_fAccSmokeTime += fTimeDelta;
				if (m_fAccSmokeTime > 0.2f)
				{
					m_fAccSmokeTime = 0.f;


					Character_Make_Effect(TEXT("Smoke_Run"), { -0.2f,0.f }, true);

				}


				if (m_bMotionPlaying == false)
				{
					Set_Animation(m_iBound_Ground, 2.f);

					//Set_NextAnimation(m_iBreakFall_Ground, 2.f);
					//
					//if (inputBuffer.back() == CInput{ MOVEKEY_UP, ATTACK_LIGHT })
					//{
					//	m_fImpuse = { -3.f * m_iLookDirection, 2.f };
					//}
					//m_bHitGroundSmashed = false;

					m_bHitGroundSmashed = false;
					Set_BreakFall_Ground();

					Set_bAttackGravity(true);

					//Set_NextAnimation(m_iBreakFall_Ground, 2.f);
					//DirectionInput iMoveKey = inputBuffer.back().direction;
					//
					//if (iMoveKey == MOVEKEY_UP || iMoveKey == MOVEKEY_UP_LEFT)
					//{
					//	Set_fImpulse({ -3.f * m_iLookDirection,1.f });
					//	Set_ForcveGravityTime(0.f);
					//}
					//else if (iMoveKey == MOVEKEY_RIGHT)
					//{
					//	//Set_fImpulse({ 0.f , 0.3f });
					//	Set_fImpulse({ 0.f , 0.1f });
					//
					//	Set_ForcedGravityTime_LittleUp();
					//}
					//
					//
					//else //if (iMoveKey == MOVEKEY_LEFT)
					//{
					//	Set_fImpulse({ -5.f * m_iLookDirection, 0.f });
					//	Set_ForcedGravityTime_LittleUp();
					//}
				}

			}
		}
		else if (Check_bCurAnimationisHitAway() || Check_bCurAnimationisAirHit())
		{
			//낙법 또는 스매시 
			if (m_bHitGroundSmashed)
			{
				//Set_Animation(m_iHit_Air_LightAnimationIndex);
				//m_pModelCom->CurrentAnimationPositionJump(55.f);
				//Set_NextAnimation(m_iBound_Ground, 5.f);

				//m_bHitGroundSmashed = false;
			}
			else
			{
				//Set_Animation(m_iBound_Ground);
				if (inputBuffer.size() == 0)
				{
					inputBuffer.push_back(CInput(MOVEKEY_NEUTRAL, ATTACK_NONE));
				}
				Set_Animation(m_iBreakFall_Ground, 2.f);
				if (inputBuffer.back() == CInput{ MOVEKEY_UP, ATTACK_LIGHT })
				{
					m_fImpuse = { -3.f * m_iLookDirection, 2.f };
				}
				Set_NextAnimation(m_iIdleAnimationIndex, 2.f);
			}
		}
		//else if (m_bHitGroundSmashed == true && m_pModelCom->m_iCurrentAnimationIndex == m_iHit_Air_LightAnimationIndex)
		//{
		//	if (m_bMotionPlaying == false)
		//	{
		//		Set_Animation(m_iBreakFall_Ground, 2.f);
		//		Set_NextAnimation(m_iIdleAnimationIndex, 2.f);
		//		m_bHitGroundSmashed = false;
		//	}
		//
		//}
		else if (m_pModelCom->m_iCurrentAnimationIndex == m_iBound_Ground)
		{
			if (m_bMotionPlaying == false)
			{
				Set_Animation(m_iBreakFall_Ground);

			}

		}
		if (m_pModelCom->m_iCurrentAnimationIndex == m_iBreakFall_Ground)
		{

			//낙법체크
			//1회만 실행될것
			m_fGravityTime += fTimeDelta;
			if (m_fGravityTime * 2.f > m_fJumpPower)
			{
				m_fGravityTime = m_fJumpPower * 0.5f;
			}
			//_float fGravity = (-0.7f * (2 * m_fGravityTime - m_fJumpPower) * (2 * m_fGravityTime - m_fJumpPower) + 4) * 0.1;
			//m_pTransformCom->Add_Move({ m_fImpuse.x * fTimeDelta,-fGravity + m_fImpuse.y * fTimeDelta,0 });
			m_pTransformCom->Add_Move({ m_fImpuse.x * fTimeDelta,0.3f,0 });

		}

		if (m_bJumpLock)
		{
			m_fAccAnimationLock += fTimeDelta;

			if (m_fAccAnimationLock > 0.15f)
			{
				m_bJumpLock = false;
				m_fAccAnimationLock = 0.f;
			}
		}

		m_bDoubleJumpEnable = true;

	}




}



HRESULT CCharacter::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Model */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_untitled"),
	//	TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	//	return E_FAIL;

	/* Com_Model */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_strModelName,
	//	TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	//	return E_FAIL;




	//CCollider_Test::COLLIDER_DESC ColliderDesc{};
	//ColliderDesc.pTransform = m_pTransformCom;
	//ColliderDesc.fSizeX = 1.2f;
	//ColliderDesc.fSizeY = 1.5f;
	//ColliderDesc.fSizeZ = 0.7f;
	//ColliderDesc.Offset = { 0.f, 0.7f, 0.f };


	//CBounding_AABB::BOUNDING_AABB_DESC ColliderDesc{};
	//if (m_iPlayerTeam == 1)
	//	ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_BODY;
	//else
	//	ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_BODY;
	//ColliderDesc.pMineGameObject = this;
	//ColliderDesc.vCenter = { 0.f,0.8f,0.f };
	//ColliderDesc.vExtents = { 0.5f,0.7f,1.f };
	//
	//
	////Com_Collider
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
	//	TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
	//	return E_FAIL;
	//
	//m_pGameInstance->Add_ColliderObject(ColliderDesc.colliderGroup, m_pColliderCom);



	return S_OK;
}

HRESULT CCharacter::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pOutLineCom->Bind_ShaderResource(m_pShaderCom, "g_OutLineTexture", 1)))
		return E_FAIL;

	const LIGHT_DESC* pLightDesc = m_pRenderInstance->Get_LightDesc(CLight_Manager::LIGHT_PLAYER, 0, m_strName);
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_iPlayerDirection", pLightDesc->pPlayerDirection, sizeof(_int))))
		return E_FAIL;

	return S_OK;
}

void CCharacter::Map_DestructiveFinish()
{
	CGameObject* pGameObject = *(m_pGameInstance->Get_Layer(LEVEL_GAMEPLAY, TEXT("Layer_MeteoBreak")).begin());

	/* 나중에 맵 조건에 따른 분기 처리 해야함*/
	static_cast<CSpaceMeteoBreak*>(pGameObject)->Start_Space_DestructiveFinish();

}

void CCharacter::GetUI_Input(DirectionInput eDirInput, ButtonInput eBtnInput, _uint iTeam)
{
	if (iTeam == 1)
	{

		if (m_iLookDirection == -1)
		{
			switch (eDirInput)
			{
			case Client::MOVEKEY_LEFT:
				eDirInput = MOVEKEY_RIGHT;
				break;
			case Client::MOVEKEY_RIGHT:
				eDirInput = MOVEKEY_LEFT;
				break;
			case Client::MOVEKEY_UP_LEFT:
				eDirInput = MOVEKEY_UP_RIGHT;
				break;
			case Client::MOVEKEY_UP_RIGHT:
				eDirInput = MOVEKEY_UP_LEFT;
				break;
			case Client::MOVEKEY_DOWN_LEFT:
				eDirInput = MOVEKEY_DOWN_RIGHT;
				break;
			case Client::MOVEKEY_DOWN_RIGHT:
				eDirInput = MOVEKEY_DOWN_LEFT;
				break;
			default:
				break;
			}
		}

		m_pUI_Manager->m_eDirInput = eDirInput;
		m_pUI_Manager->m_eBtnInput = eBtnInput;
	}
	else if (iTeam == 2)
	{
		if (m_iLookDirection == 1)
		{
			switch (eDirInput)
			{
			case Client::MOVEKEY_LEFT:
				eDirInput = MOVEKEY_RIGHT;
				break;
			case Client::MOVEKEY_RIGHT:
				eDirInput = MOVEKEY_LEFT;
				break;
			case Client::MOVEKEY_UP_LEFT:
				eDirInput = MOVEKEY_UP_RIGHT;
				break;
			case Client::MOVEKEY_UP_RIGHT:
				eDirInput = MOVEKEY_UP_LEFT;
				break;
			case Client::MOVEKEY_DOWN_LEFT:
				eDirInput = MOVEKEY_DOWN_RIGHT;
				break;
			case Client::MOVEKEY_DOWN_RIGHT:
				eDirInput = MOVEKEY_DOWN_LEFT;
				break;
			default:
				break;
			}
		}
			m_pUI_Manager->m_eDirInput2 = eDirInput;
			m_pUI_Manager->m_eBtnInput2 = eBtnInput;
		
	}


}

void CCharacter::Notify_QTE_Same_Grab(_int result)
{
	m_iQTE = result;

	//switch (result)
	//{
	//	//승
	//case 1:
	//	break;
	//	//패
	//case -1:
	//	break;
	//	//비김
	//case 0:
	//	break;
	//}
}

void CCharacter::Notify_QTE_1p_Grab(_int result)
{
	m_iQTE = result;

	//switch (result)
	//{
	//	//승
	//case 1:
	//	break;
	//	//패
	//case -1:
	//	break;
	//}
}


void CCharacter::Notify_QTE_Hit(_int result)
{
	m_iQTE = result;

	//switch (result)
	//{
	//	//승
	//case 1:
	//	break;
	//	//패
	//case -1:
	//	break;
	//}
}

void CCharacter::Notify_QTE_Continuous_Attack(_int result)
{
	m_iQTE = result;

	//switch (result)
	//{
	//	//승
	//case 1:
	//	break;
	//	//패
	//case -1:
	//	break;
	//}
}

void CCharacter::Character_Start_QTE(_uint iQTEID)
{
	/*
	enum QTE_ID
	{
		QTE_ID_SAME_GRAB,
		QTE_ID_HIT,
		QTE_ID_CONTINUOUS_ATTACK,
		QTE_ID_END
	};
	*/
	m_bCreateQTE = true;
	m_iQTE = -1;
	CQTE_Manager::Get_Instance()->Start_QTE((CQTE_Manager::QTE_ID)iQTEID, this);

}
void CCharacter::Add_ChaseLight()
{
	LIGHT_DESC			LightDesc{};

	ZeroMemory(&LightDesc, sizeof(LIGHT_DESC));
	LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	LightDesc.vPosition = _float4(0.f, 0.f, 0.f, 1.f);
	LightDesc.fRange = 30.f;
	LightDesc.vDiffuse = m_vChaseLight;
	//LightDesc.vDiffuse = _float4(1.0f, 0.f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(0.1f, 0.1f, 0.1f, 1.f);
	LightDesc.vSpecular = _float4(1.0f, 0.95f, 0.45f, 1.f);

	LightDesc.fAccTime = 0.f;
	LightDesc.fLifeTime = 1.f;
	LightDesc.strName = "Chase";
	/*LightDesc.pisDone = &m_bChase;*/


	if (FAILED(m_pRenderInstance->Add_Effect_Light(LightDesc.strName, LightDesc)))
		return;
}

CCharacter* CCharacter::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCharacter* pInstance = new CCharacter(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CCharacter"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCharacter::Clone(void* pArg)
{
	CCharacter* pInstance = new CCharacter(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CCharacter"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCharacter::Free()
{
	__super::Free();
	Safe_Release(m_pDetailTextureCom);
	Safe_Release(m_p1PTextureCom);
	Safe_Release(m_pOutLineCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	Safe_Release(m_pUI_Manager);
	Safe_Release(m_pEffect_Manager);
	Safe_Release(m_pOutLineCom);
	Safe_Release(m_pColliderCom);

}
