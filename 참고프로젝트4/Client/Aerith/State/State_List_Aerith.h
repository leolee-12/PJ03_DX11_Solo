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

#include "Aerith/State/State_Aerith_Idle.h"
#include "Aerith/State/State_Aerith_Run.h"
#include "Aerith/State/State_Aerith_Walk.h"
#include "Aerith/State/State_Aerith_WalkR.h"
#include "Aerith/State/State_Aerith_WalkL.h"
#include "Aerith/State/State_Aerith_WalkB.h"
#include "Aerith/State/State_Aerith_NormalAttack01.h"
#include "Aerith/State/State_Aerith_NormalAttack02.h"
#include "Aerith/State/State_Aerith_NormalAttack03.h"
#include "Aerith/State/State_Aerith_NormalAttack04.h"
#include "Aerith/State/State_Aerith_NormalAttack05.h"
#include "Aerith/State/State_Aerith_SpecialAttack01.h"
#include "Aerith/State/State_Aerith_SpecialAttack02.h"
#include "Aerith/State/State_Aerith_SpecialAttack03.h"
#include "Aerith/State/State_Aerith_SpecialAttack_Tempest.h"
#include "Aerith/State/State_Aerith_AbilityAttack_LustrousShield.h"
#include "Aerith/State/State_Aerith_AbilityAttack_RayOfJudge.h"
#include "Aerith/State/State_Aerith_AbilityAttack_SorcerousStorm.h"
#include "Aerith/State/State_Aerith_AbilityAttack_FlameSpell.h"
#include "Aerith/State/State_Aerith_LimitAttack_GuardianOfGuns.h"
		  
#include "Aerith/State/State_Aerith_Abnormal.h"
#include "Aerith/State/State_Aerith_Avoid.h"
#include "Aerith/State/State_Aerith_Damage_Back.h"
#include "Aerith/State/State_Aerith_Damage_Catch.h"
#include "Aerith/State/State_Aerith_Damage_Front.h"
#include "Aerith/State/State_Aerith_Die.h"
#include "Aerith/State/State_Aerith_Event_PassOver.h"
#include "Aerith/State/State_Aerith_Guard.h"
#include "Aerith/State/State_Aerith_Item_Potion.h"
#include "Aerith/State/State_Aerith_LadderDown.h"
#include "Aerith/State/State_Aerith_LadderUp.h"
#include "Aerith/State/State_Aerith_LadderDownFinished.h"
#include "Aerith/State/State_Aerith_LadderUpFinished.h"
#include "Aerith/State/State_Aerith_LeverDown.h"
#include "Aerith/State/State_Aerith_Switch.h"

#include "Aerith/State/State_Aerith_AI_AttackMode.h"
#include "Aerith/State/State_Aerith_AI_MoveMode.h"

#include "UI_Manager.h"

BEGIN(STATE_AERITH)

enum VIRTUAL_ACTIONKEY {
	NORMAL_ATTACK1,NORMAL_ATTACK2,NORMAL_ATTACK3, NORMAL_ATTACK4, NORMAL_ATTACK5, SPECIAL_ATTACK2, SPECIAL_ATTACK3, ABILITY_ATTACK_RAYOFJUDGE, ABILITY_ATTACK_SORCEROUSSTORM, ABILITY_ATTACK_FLAMESPELL, LIMIT_ATTACK_GUARDIANOFGUNS,ABNORMAL,
	AVOID, GUARD, EVENT_PASSOVER, ITEM_POTION, LADDER_DOWN, LADDER_UP, LADDER_DOWN_FINISHED, LADDER_UP_FINISHED, LEVER_DOWN, SWITCH, VIRTUAL_ACTIONKEY_NONE,VIRTUAL_ACTIONKEY_END = -1};

enum VIRTUAL_MOVEKEY { WALK, WALKR, WALKB, WALKL, RUN, VIRTUAL_MOVEKEY_NONE, VIRTUAL_MOVEKEY_END = -1 };

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
	
	if (bIsAI && !bUseCommand && (eVirtualMoveKey != VIRTUAL_MOVEKEY_NONE || eVirtualActionKey == VIRTUAL_ACTIONKEY_NONE))
		return false;

	if (bIsAI || bUseCommand)
	{
		_bool bAction = false;

		if (eCommand == CUI_Command::COMMAND_RESULT::A_CMDR_ABILITY_RAYOFJUDGE || eVirtualActionKey == ABILITY_ATTACK_RAYOFJUDGE)
		{
			pStateMachineCom.lock()->Wait_State<CState_Aerith_AbilityAttack_RayOfJudge>();
			if (!bIsAI) GET_SINGLE(CClient_Manager)->Set_BattleMode(true);
			bAction = true;
		}
		else if (eCommand == CUI_Command::COMMAND_RESULT::A_CMDR_ABILITY_SORCEROUSSTORM || eVirtualActionKey == ABILITY_ATTACK_SORCEROUSSTORM)
		{
			pStateMachineCom.lock()->Wait_State<CState_Aerith_AbilityAttack_SorcerousStorm>();
			if (!bIsAI) GET_SINGLE(CClient_Manager)->Set_BattleMode(true);
			bAction = true;
		}
		// 변경 필요
		else if (eCommand == CUI_Command::COMMAND_RESULT::A_CMDR_ABILITY_FLAMESPELL || eVirtualActionKey == ABILITY_ATTACK_FLAMESPELL)
		{
			pStateMachineCom.lock()->Wait_State<CState_Aerith_AbilityAttack_FlameSpell>();
			if (!bIsAI) GET_SINGLE(CClient_Manager)->Set_BattleMode(true);
			bAction = true;
		}
		else if (eCommand == CUI_Command::COMMAND_RESULT::A_CMDR_LIMIT_GUARDIANOFGUNS || eVirtualActionKey == LIMIT_ATTACK_GUARDIANOFGUNS)
		{
			pStateMachineCom.lock()->Wait_State<CState_Aerith_LimitAttack_GuardianOfGuns>();
			if (!bIsAI) GET_SINGLE(CClient_Manager)->Set_BattleMode(true);
			bAction = true;
		}
		else if ((eCommand == CUI_Command::COMMAND_RESULT::C_CMDR_ITEM) || eVirtualActionKey == ITEM_POTION)
		{
			pStateMachineCom.lock()->Wait_State<CState_Aerith_Item_Potion>();
			if (!bIsAI) GET_SINGLE(CClient_Manager)->Set_BattleMode(true);
			bAction = true;
		}

		if (bAction)
		{
			return true;
		}
	}

	_bool bIsNormalAttack = bIsAI && (eVirtualActionKey == NORMAL_ATTACK1 || eVirtualActionKey == NORMAL_ATTACK2 || eVirtualActionKey == NORMAL_ATTACK3 || eVirtualActionKey == NORMAL_ATTACK4 || eVirtualActionKey == NORMAL_ATTACK5);
	_bool bIsSpecialAttack = bIsAI && (eVirtualActionKey == eVirtualActionKey == SPECIAL_ATTACK2 || eVirtualActionKey == SPECIAL_ATTACK3);

	if ((!bIsAI && pGameInstance->Mouse_Pressing(DIM_LB)) || bIsNormalAttack)
	{		
		//유진 추가
		if (!GET_SINGLE(CUI_Manager)->IsOnSoundWindow()) {

			pStateMachineCom.lock()->Enter_State<CState_Aerith_NormalAttack01>();

			if (!bIsAI)
			{

				GET_SINGLE(CCamera_Manager)->Set_ThirdCam_OffsetType(OFFSET_TYPE::TYPE_ATTACK1);
				GET_SINGLE(CClient_Manager)->Set_BattleMode(true);
				GET_SINGLE(CClient_Manager)->Set_Command_Result(CUI_Command::COMMAND_RESULT::CMDR_NONE);

			}
		}
		return true;
	}
	else if ((!bIsAI && pGameInstance->Key_Pressing(DIK_Q)) || bIsSpecialAttack)
	{
		GET_SINGLE(CCamera_Manager)->Set_ThirdCam_OffsetType(OFFSET_TYPE::TYPE_ATTACK1);

		pStateMachineCom.lock()->Enter_State<CState_Aerith_SpecialAttack01>();
		GET_SINGLE(CClient_Manager)->Set_BattleMode(true);
		return true;
	}
	else if ((!bIsAI && pGameInstance->Key_Down(DIK_T)) || eVirtualActionKey == GUARD)
	{
		pStateMachineCom.lock()->Enter_State<CState_Aerith_Guard>();
		GET_SINGLE(CClient_Manager)->Set_BattleMode(true);
		return true;
	}
	else if ((!bIsAI && pGameInstance->Key_Down(DIK_R)) || eVirtualActionKey == AVOID)
	{
		pStateMachineCom.lock()->Enter_State<CState_Aerith_Avoid>();
		GET_SINGLE(CClient_Manager)->Set_BattleMode(true);
		return true;
	}
	else if (eVirtualActionKey == EVENT_PASSOVER)
	{
		pStateMachineCom.lock()->Enter_State<CState_Aerith_Event_PassOver>();
		return true;
	}
	else if (eVirtualActionKey == ITEM_POTION)
	{
		pStateMachineCom.lock()->Enter_State<CState_Aerith_Item_Potion>();
		GET_SINGLE(CClient_Manager)->Set_BattleMode(true);
		return true;
	}
	else if ((!bIsAI && pGameInstance->Key_Down(DIK_U)) || eVirtualActionKey == LADDER_DOWN)
	{
		if (typeid(*pStateMachineCom.lock()->Get_CurState()) == typeid(CState_Aerith_LadderDown))
			pStateMachineCom.lock()->Enter_State<CState_Aerith_LadderDownFinished>();
		else
			pStateMachineCom.lock()->Enter_State<CState_Aerith_LadderUp>();
		return true;
	}
	else if ((!bIsAI && pGameInstance->Key_Down(DIK_I)) || eVirtualActionKey == LADDER_UP)
	{
		if (typeid(*pStateMachineCom.lock()->Get_CurState()) == typeid(CState_Aerith_LadderUp))
			pStateMachineCom.lock()->Enter_State<CState_Aerith_LadderUpFinished>();
		else
			pStateMachineCom.lock()->Enter_State<CState_Aerith_LadderDown>();
		return true;
	}
#pragma TODO_MSG(위치 변경 필요)
	else if (pGameInstance->Key_DownEx(DIK_1, DIK_MD_NONE))
	{
		GET_SINGLE(CCamera_Manager)->Set_ThirdCam_OffsetType(OFFSET_TYPE::TYPE_ATTACK1);

		pStateMachineCom.lock()->Enter_State<CState_Aerith_SpecialAttack01>();
		GET_SINGLE(CClient_Manager)->Set_BattleMode(true);
		//GET_SINGLE(CClient_Manager)->Set_Command_Result(CUI_Command::COMMAND_RESULT::CMDR_NONE);
		return true;
	}
#pragma TODO_MSG(위치 변경 필요)
	else if (pGameInstance->Key_DownEx(DIK_2, DIK_MD_NONE))
	{
		pStateMachineCom.lock()->Enter_State<CState_Aerith_AbilityAttack_FlameSpell>();
		GET_SINGLE(CClient_Manager)->Set_BattleMode(true);
	}
#pragma TODO_MSG(위치 변경 필요)
	else if (pGameInstance->Key_DownEx(DIK_3, DIK_MD_NONE))
	{
		pStateMachineCom.lock()->Enter_State<CState_Aerith_AbilityAttack_SorcerousStorm>();
		GET_SINGLE(CClient_Manager)->Set_BattleMode(true);
	}
#pragma TODO_MSG(위치 변경 필요)
	else if (pGameInstance->Key_DownEx(DIK_4, DIK_MD_NONE))
	{
		pStateMachineCom.lock()->Enter_State<CState_Aerith_AbilityAttack_RayOfJudge>();
		GET_SINGLE(CClient_Manager)->Set_BattleMode(true);
	}
#pragma TODO_MSG(위치 변경 필요)
	else if (pGameInstance->Key_DownEx(DIK_5, DIK_MD_NONE))
	{
		pStateMachineCom.lock()->Enter_State<CState_Aerith_AbilityAttack_LustrousShield>();
		GET_SINGLE(CClient_Manager)->Set_BattleMode(true);
	}
#pragma TODO_MSG(위치 변경 필요)
	else if (pGameInstance->Key_DownEx(DIK_1, DIK_EX::DIK_MD_LSHIFT))
	{
		pStateMachineCom.lock()->Enter_State<CState_Aerith_LimitAttack_GuardianOfGuns>();
		GET_SINGLE(CClient_Manager)->Set_BattleMode(true);
	}

	return false;
}

END