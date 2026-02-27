#pragma once

#include "StateMachine.h"
#include "CommonModelComp.h"
#include "Client_Manager.h"
#include "GameInstance.h"
#include "Character.h"
#include "UI_Command.h"
#include "Camera_Manager.h"
#include "Camera_ThirdPerson.h"
#include "AttackCollider.h"

#include "State_Cloud_Idle.h"
#include "State_Cloud_Run.h"
#include "State_Cloud_Walk.h"
#include "State_Cloud_WalkB.h"
#include "State_Cloud_WalkL.h"
#include "State_Cloud_WalkR.h"
#include "State_Cloud_AssertAttack01.h"
#include "State_Cloud_AssertAttack02.h"
#include "State_Cloud_AssertAttack03.h"
#include "State_Cloud_AssertAttack04.h"
#include "State_Cloud_AssertAttack05.h"
#include "State_Cloud_BraveAttack01.h"
#include "State_Cloud_BraveAttack02.h"
#include "State_Cloud_BraveAttack03.h"
#include "State_Cloud_BraveAttack04.h"
#include "State_Cloud_AbilityAttack_Slash.h"
#include "State_Cloud_AbilityAttack_Infinity.h"
#include "State_Cloud_AbilityAttack_Blade.h"
#include "State_Cloud_LimitAttack.h"

#include "State_Cloud_Abnormal.h"
#include "State_Cloud_Avoid.h"
#include "State_Cloud_Damage_Back.h"
#include "State_Cloud_Damage_Front.h"
#include "State_Cloud_Damage_Catch.h"
#include "State_Cloud_Die.h"
#include "State_Cloud_Event_PassOver.h"
#include "State_Cloud_Guard.h"
#include "State_Cloud_GuardRelease.h"
#include "State_Cloud_Item_Potion.h"
#include "State_Cloud_LadderDown.h"
#include "State_Cloud_LadderUp.h"
#include "State_Cloud_LadderDownFinished.h"
#include "State_Cloud_LadderUpFinished.h"
#include "State_Cloud_LeverDown.h"
#include "State_Cloud_Switch.h"
#include "State_Cloud_SneakIn.h"

#include "State_Cloud_Dance.h"


#include "StateUpperBody_Cloud_Idle.h"
#include "StateUpperBody_Cloud_Guard.h"

#include "State_Cloud_AI_AttackMode.h"

#include "UI_Manager.h"

BEGIN(STATE_CLOUD)


enum VIRTUAL_ACTIONKEY { ASSERT_ATTACK1, ASSERT_ATTACK2, ASSERT_ATTACK3, ASSERT_ATTACK4, ASSERT_ATTACK5, BRAVE_ATTACK1,BRAVE_ATTACK2, BRAVE_ATTACK3, BRAVE_ATTACK4, ABILITY_ATTACK_INFINITY, ABILITY_ATTACK_BLADE, ABILITY_ATTACK_SLASH, LIMIT_ATTACK,
  GUARD, ABNORMAL, AVOID,  EVENT_PASSOVER,ITEM_POTION, LADDER_DOWN, LADDER_UP, LADDER_DOWN_FINISHED, LADDER_UP_FINISHED, LEVER_DOWN, SWITCH, VIRTUAL_ACTIONKEY_NONE,VIRTUAL_ACTIONKEY_END = -1};
enum VIRTUAL_MOVEKEY { WALK, WALKB, WALKL, WALKR,RUN, VIRTUAL_MOVEKEY_NONE,VIRTUAL_MOVEKEY_END = -1};

static VIRTUAL_ACTIONKEY eVirtualActionKey = VIRTUAL_ACTIONKEY_END;
static VIRTUAL_MOVEKEY eVirtualMoveKey = VIRTUAL_MOVEKEY_END;
static DIRKEYINFO eDirKeyInfo = {};

static void Set_VirtualActionKey(VIRTUAL_ACTIONKEY eKey) { eVirtualActionKey = eKey; eVirtualMoveKey = VIRTUAL_MOVEKEY_NONE; };
static void Set_VirtualMoveKey(VIRTUAL_MOVEKEY eKey) { eVirtualActionKey = VIRTUAL_ACTIONKEY_NONE; eVirtualMoveKey = eKey; };
static void Reset_VirtualKey() { eVirtualActionKey = VIRTUAL_ACTIONKEY_NONE; eVirtualMoveKey = VIRTUAL_MOVEKEY_NONE; };

static _bool Action_Key_Input(CGameInstance* pGameInstance, weak_ptr<CStateMachine> pStateMachineCom, _bool bPlayable)
{
	_bool bIsAI = !bPlayable;

	CUI_Command::COMMAND_RESULT eCommand = (CUI_Command::COMMAND_RESULT)GET_SINGLE(CClient_Manager)->Get_Command_Result();
	_bool bUseCommand = (eCommand != CUI_Command::COMMAND_RESULT::CMDR_NONE && eCommand != CUI_Command::COMMAND_RESULT::C_CMDR_A_ATTACK && eCommand != CUI_Command::COMMAND_RESULT::C_CMDR_B_ATTACK);

	if (bIsAI && !bUseCommand && (eVirtualMoveKey != VIRTUAL_MOVEKEY_NONE || eVirtualActionKey == VIRTUAL_ACTIONKEY_NONE ))
		return false;

	// 상체 상태머신
	auto pUpperBodyFSM = pStateMachineCom.lock()->Get_RefStateMachine("UpperBody");

	if (bIsAI || bUseCommand)
	{
		_bool bAction = false;

		if ((eCommand == CUI_Command::COMMAND_RESULT::C_CMDR_ABILITY_INFINITEEND) || eVirtualActionKey == ABILITY_ATTACK_INFINITY)
		{
			pStateMachineCom.lock()->Wait_State<CState_Cloud_AbilityAttack_Infinity>();
			pUpperBodyFSM.lock()->Wait_State<CStateUpperBody_Cloud_Idle>();
			if (!bIsAI) GET_SINGLE(CClient_Manager)->Set_BattleMode(true);
			bAction = true;
		}
		else if ((eCommand == CUI_Command::COMMAND_RESULT::C_CMDR_ABILITY_BUSTSLASH
			) || eVirtualActionKey == ABILITY_ATTACK_SLASH)
		{
			pStateMachineCom.lock()->Wait_State<CState_Cloud_AbilityAttack_Slash>();
			pUpperBodyFSM.lock()->Wait_State<CStateUpperBody_Cloud_Idle>();
			if (!bIsAI) GET_SINGLE(CClient_Manager)->Set_BattleMode(true);
			bAction = true;
		}
		else if ((eCommand == CUI_Command::COMMAND_RESULT::C_CMDR_ABILITY_BLADEBEAM) || eVirtualActionKey == ABILITY_ATTACK_BLADE)
		{
			pStateMachineCom.lock()->Wait_State<CState_Cloud_AbilityAttack_Blade>();
			pUpperBodyFSM.lock()->Wait_State<CStateUpperBody_Cloud_Idle>();
			if (!bIsAI) GET_SINGLE(CClient_Manager)->Set_BattleMode(true);
			bAction = true;
		}
		else if ((eCommand == CUI_Command::COMMAND_RESULT::C_CMDR_LIMIT_ASCENSION) || eVirtualActionKey == LIMIT_ATTACK)
		{
			pStateMachineCom.lock()->Wait_State<CState_Cloud_LimitAttack>();
			pUpperBodyFSM.lock()->Wait_State<CStateUpperBody_Cloud_Idle>();
			if (!bIsAI) GET_SINGLE(CClient_Manager)->Set_BattleMode(true);
			bAction = true;
		}
		else if ((eCommand == CUI_Command::COMMAND_RESULT::C_CMDR_ITEM) || eVirtualActionKey == ITEM_POTION)
		{
			pStateMachineCom.lock()->Wait_State<CState_Cloud_Item_Potion>();
			pUpperBodyFSM.lock()->Enter_State<CStateUpperBody_Cloud_Idle>();
			if (!bIsAI) GET_SINGLE(CClient_Manager)->Set_BattleMode(true);
			bAction = true;
		}

		if (bAction)
		{			
			return true;
		}
	}

	_bool bIsAssertAttack = bIsAI && (eVirtualActionKey == ASSERT_ATTACK1 || eVirtualActionKey == ASSERT_ATTACK2 || eVirtualActionKey == ASSERT_ATTACK3 || eVirtualActionKey == ASSERT_ATTACK4 || eVirtualActionKey == ASSERT_ATTACK5);
	_bool bIsBraveAttack = bIsAI && (eVirtualActionKey == BRAVE_ATTACK1 || eVirtualActionKey == BRAVE_ATTACK2 || eVirtualActionKey == BRAVE_ATTACK3 || eVirtualActionKey == BRAVE_ATTACK4);

	if ((!bIsAI && pGameInstance->Mouse_Pressing(DIM_LB)) || bIsAssertAttack || bIsBraveAttack)
	{
		//유진 추가
		if (!GET_SINGLE(CUI_Manager)->IsOnSoundWindow()) {

			if ((!bIsAI && GET_SINGLE(CClient_Manager)->Get_AssertMode()) || bIsAssertAttack)
				pStateMachineCom.lock()->Enter_State<CState_Cloud_AssertAttack01>();
			else
				pStateMachineCom.lock()->Enter_State<CState_Cloud_BraveAttack01>();
			pUpperBodyFSM.lock()->Enter_State<CStateUpperBody_Cloud_Idle>();

			if (!bIsAI)
			{

				GET_SINGLE(CCamera_Manager)->Set_ThirdCam_OffsetType(OFFSET_TYPE::TYPE_ATTACK1);
				GET_SINGLE(CClient_Manager)->Set_BattleMode(true);
				GET_SINGLE(CClient_Manager)->Set_Command_Result(CUI_Command::COMMAND_RESULT::CMDR_NONE);
			}
		}
		return true;
	}
	else if ((!bIsAI && pGameInstance->Key_Down(DIK_T)) || eVirtualActionKey == GUARD)
	{
		pStateMachineCom.lock()->Enter_State<CState_Cloud_Guard>();
		if (!bIsAI) GET_SINGLE(CClient_Manager)->Set_BattleMode(true);

		return true;
	}
	else if ((!bIsAI && pGameInstance->Key_Down(DIK_R)) || eVirtualActionKey == AVOID)
	{
		pStateMachineCom.lock()->Enter_State<CState_Cloud_Avoid>();
		pUpperBodyFSM.lock()->Enter_State<CStateUpperBody_Cloud_Idle>();
		if (!bIsAI) GET_SINGLE(CClient_Manager)->Set_BattleMode(true);

		return true;
	}
	else if ((!bIsAI && pGameInstance->Key_Down(DIK_H)) || eVirtualActionKey == EVENT_PASSOVER)
	{
		pStateMachineCom.lock()->Enter_State<CState_Cloud_Event_PassOver>();
		pUpperBodyFSM.lock()->Enter_State<CStateUpperBody_Cloud_Idle>();
		return true;
	}
	else if ((!bIsAI && pGameInstance->Key_Down(DIK_J)) || eVirtualActionKey == ITEM_POTION)
	{
		pStateMachineCom.lock()->Enter_State<CState_Cloud_Item_Potion>();
		pUpperBodyFSM.lock()->Enter_State<CStateUpperBody_Cloud_Idle>();
		if (!bIsAI) GET_SINGLE(CClient_Manager)->Set_BattleMode(true);

		return true;
	}
	else if ((!bIsAI && pGameInstance->Key_Down(DIK_U)) || eVirtualActionKey == LADDER_DOWN)
	{
		if (typeid(*pStateMachineCom.lock()->Get_CurState()) == typeid(CState_Cloud_LadderDown))
			pStateMachineCom.lock()->Enter_State<CState_Cloud_LadderDownFinished>();
		else
			pStateMachineCom.lock()->Enter_State<CState_Cloud_LadderUp>();
		pUpperBodyFSM.lock()->Enter_State<CStateUpperBody_Cloud_Idle>();

		return true;
	}
	else if ((!bIsAI && pGameInstance->Key_Down(DIK_I)) || eVirtualActionKey == LADDER_UP)
	{
		if (typeid(*pStateMachineCom.lock()->Get_CurState()) == typeid(CState_Cloud_LadderUp))
			pStateMachineCom.lock()->Enter_State<CState_Cloud_LadderUpFinished>();
		else
			pStateMachineCom.lock()->Enter_State<CState_Cloud_LadderDown>();
		pUpperBodyFSM.lock()->Enter_State<CStateUpperBody_Cloud_Idle>();

		return true;
	}

	return false;
}

END