#include "stdafx.h"

#include "Layer.h"
#include "UI_Manager.h"

#include "UIObject.h"

#include "UI_Define.h"
#include "GameInstance.h"
#include "RenderInstance.h"
#include "Character.h"
#include "UI_BaseAttBuf.h"
#include "UI_Opt_Sound.h"

IMPLEMENT_SINGLETON(CUI_Manager)

CUI_Manager::CUI_Manager()
	: m_pGameInstance{ CGameInstance::Get_Instance() }
{
 	Safe_AddRef(m_pGameInstance);
}

void CUI_Manager::Add_UIOjbect(CUI_Define::UI_LISTPOS ePos, CUIObject* pObject)
{
	switch (ePos)
	{
	case Client::CUI_Define::TOP:
		m_ListTopUI.push_back(pObject);
		break;
	case Client::CUI_Define::MID:
		m_ListMidUI.push_back(pObject);
		break;
	case Client::CUI_Define::BOT:
		m_ListBotUI.push_back(pObject);
		break;

	default:
		break;
	}
}

void CUI_Manager::GamePlayUpdate(_float fTimeDelta)
{
	if(m_bStartUI == FALSE)
		m_fStartUITimer += fTimeDelta;

	if (fTimeDelta >= 0.1f)
		m_fStartUITimer = 0.f;

	if (m_fStartUITimer >= 0.5f && m_bStartUI == FALSE)
	{
		//UsingCreateStartUI();
		m_bStartUI = TRUE;
	}

	if (m_pGameInstance->Key_Down(DIK_F2))
	{
		UsingCreateEndUI();
	}


	if (m_pGameInstance->Key_Down(DIK_TAB))
	{
		if (m_bOnOption == FALSE)
		{
			CreateOption();
			m_bOnOption = TRUE;
		}
		else
		{
			DestroyOption();
			m_bOnOption = FALSE;
		}


	}
}

void CUI_Manager::UsingAttckBuff(CUI_Define::PLAYER_SLOT eSlotID)
{

	CUIObject::UI_DESC tAttBufDesc = {};

	if (eSlotID == CUI_Define::LPLAYER1 || eSlotID == CUI_Define::LPLAYER2)
		tAttBufDesc.eLRPos = CUIObject::LEFT;
	else if (eSlotID == CUI_Define::RPLAYER1 || eSlotID == CUI_Define::RPLAYER2)
		tAttBufDesc.eLRPos = CUIObject::RIGHT;

	m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_AttBufEffect"), TEXT("Layer_UI_AttBufEffect"), &tAttBufDesc);
	m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_AttBufThunderEffect"), TEXT("Layer_UI_AttBufEffect"), &tAttBufDesc);
	m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_AttBufIconEff"), TEXT("Layer_UI_AttBufEffect"), &tAttBufDesc);
	
}

void CUI_Manager::UsingAttackDestroy(CUI_Define::PLAYER_SLOT eSlotID)
{
	CUIObject::UI_DESC tAttBufDesc = {};

	if (eSlotID == CUI_Define::LPLAYER1 || eSlotID == CUI_Define::LPLAYER2)
		tAttBufDesc.eLRPos = CUIObject::LEFT;
	else if (eSlotID == CUI_Define::RPLAYER1 || eSlotID == CUI_Define::RPLAYER2)
		tAttBufDesc.eLRPos = CUIObject::RIGHT;

	
	for (auto& iter : m_pGameInstance->Get_Layer(LEVEL_GAMEPLAY, TEXT("Layer_UI_AttBufEffect")))
	{
		if(dynamic_cast<CUI_BaseAttBuf*>(iter)->Get_UIPos() == tAttBufDesc.eLRPos)
			iter->Destory();
	}
}

void CUI_Manager::UsingChangeCharacher(CUI_Define::PLAYER_SLOT eCurrSlotID)
{
	m_bChange[0] = FALSE;
	m_bChange[1] = FALSE;

	if (eCurrSlotID == CUI_Define::LPLAYER1 || eCurrSlotID == CUI_Define::LPLAYER2)
	{
		if (m_pPawnArray[CUI_Define::LPLAYER2] == nullptr)
			return;

		CCharacter* pSwapPanw = m_pPawnArray[CUI_Define::LPLAYER1];
		m_pPawnArray[CUI_Define::LPLAYER1] = m_pPawnArray[CUI_Define::LPLAYER2];
		m_pPawnArray[CUI_Define::LPLAYER2] = pSwapPanw;

		m_bChange[0] = TRUE;
	}

	else if (eCurrSlotID == CUI_Define::RPLAYER1 || eCurrSlotID == CUI_Define::RPLAYER2)
	{
		if (m_pPawnArray[CUI_Define::RPLAYER2] == nullptr)
			return;

		CCharacter* pSwapPanw = m_pPawnArray[CUI_Define::RPLAYER1];
		m_pPawnArray[CUI_Define::RPLAYER1] = m_pPawnArray[CUI_Define::RPLAYER2];
		m_pPawnArray[CUI_Define::RPLAYER2] = pSwapPanw;

		m_bChange[1] = TRUE;
	}
}

void CUI_Manager::UsingCreateStartUI()
{
	m_fTotalDuration = 0.f;
	m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Common_FIGHT_SFX, false, 0.2f);
	CUIObject::UI_DESC StartDesc = {};
	StartDesc.fSpeedPerSec = 50.f;
	StartDesc.fRotationPerSec = XMConvertToRadians(90.f);

	for (int i = 0; i < 8; i++)
	{
		StartDesc.iNumUI = i;
		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_GameStartCircle"), TEXT("Layer_UI_GameStartCircle"), &StartDesc);
	}
	m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_ReadyFont"), TEXT("Layer_UI_GameStartFont"));
}

void CUI_Manager::UsingCreateEndUI()
{
	m_fTotalDuration = 0.f;
	m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Common_FINISH_SFX, false, 0.2f);
	m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_KOPanel"), TEXT("Layer_UI_KOFont"));
	m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_KOFont"), TEXT("Layer_UI_KOFont"));
	m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_KOFontEffect"), TEXT("Layer_UI_KOFont"));
	m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_KOParticle"), TEXT("Layer_UI_KOFont"));

}

void CUI_Manager::CutSceneUI(_bool bState)
{
	if (bState == FALSE)
	{
		for (auto& Input : m_pGameInstance->Get_Layer(LEVEL_GAMEPLAY, TEXT("Layer_DirInput")))
		{
			Input->Destory();
		}


		//for (auto& Input : m_pGameInstance->Get_Layer(LEVEL_GAMEPLAY, TEXT("Layer_UI_Combo_Number")))
		//{
		//	Input->Destory();
		//}

		//for (auto& Input : m_pGameInstance->Get_Layer(LEVEL_GAMEPLAY, TEXT("Layer_UI_Combo_Font")))
		//{
		//	Input->Destory();
		//}

		//for (auto& Input : m_pGameInstance->Get_Layer(LEVEL_GAMEPLAY, TEXT("Layer_UI_Combo_Effect")))
		//{
		//	Input->Destory();
		//}
	}

	for (auto& Top : m_ListTopUI)
	{
		Top->SetActive(bState);
	}

	//for (auto& Mid : m_ListMidUI)
	//{
	//	Mid->SetActive(bState);
	//}

	for (auto& Bot : m_ListBotUI)
	{
		Bot->SetActive(bState);
	}

	

}

void CUI_Manager::WinUI(LEVELID CreateLevel)
{
	CUIObject::UI_DESC WinDesc = {};
	WinDesc.eLRPos = CUIObject::LEFT;
	WinDesc.fRotationPerSec = XMConvertToRadians(45.f);
	for (int i = 0; i < 8; i++)
	{
		WinDesc.iNumUI = i;
		m_pGameInstance->Add_GameObject_ToLayer(CreateLevel, TEXT("Prototype_GameObject_UI_WIn_Circle"), TEXT("Layer_Lobby_Win"), &WinDesc);
	}
	m_pGameInstance->Add_GameObject_ToLayer(CreateLevel, TEXT("Prototype_GameObject_UI_WIn_Font"), TEXT("Layer_Lobby_Win"),&WinDesc);
	m_pGameInstance->Add_GameObject_ToLayer(CreateLevel, TEXT("Prototype_GameObject_UI_WIn_Team"), TEXT("Layer_Lobby_Win"), &WinDesc);
}


void CUI_Manager::UI_Setting_Debug(_float& fSizeX, _float& fSizeY, _float& fPosX, _float& fPosY, _float fSizeRadio)
{
	if (m_pGameInstance->Get_DIKeyState(DIK_NUMPAD7))
	{
		fSizeY--;
		fSizeX = fSizeRadio * fSizeY;
	}

	if (m_pGameInstance->Get_DIKeyState(DIK_NUMPAD9))
	{
		fSizeY++;
		fSizeX = fSizeRadio * fSizeY;
	}

	if (m_pGameInstance->Get_DIKeyState(DIK_NUMPAD4))
	{
		fPosX--;
	}

	if (m_pGameInstance->Get_DIKeyState(DIK_NUMPAD6))
	{
		fPosX++;
	}

	if (m_pGameInstance->Get_DIKeyState(DIK_NUMPAD8))
	{
		fPosY--;
	}

	if (m_pGameInstance->Get_DIKeyState(DIK_NUMPAD2))
	{
		fPosY++; 
	}
}

void CUI_Manager::InitUIObject()
{
	list<CGameObject*>CharacterList = m_pGameInstance->Get_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Character"));

	for (auto& iter : CharacterList)
	{
		CUI_Define::PLAYER_SLOT eSlot = dynamic_cast<CCharacter*>(iter)->Get_PawnDesc().ePlayer_Slot;

		if (eSlot == CUI_Define::SLOT_END)
			continue;

		m_pPawnArray[eSlot] = dynamic_cast<CCharacter*>(iter);
	}
}

void CUI_Manager::CreateOption()
{
	m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::UI_MENU_OPEN, false, 1.f);

	list<class CGameObject*> UIOptionList = m_pGameInstance->Get_Layer(LEVEL_GAMEPLAY, TEXT("Layer_UI_Option_Sound"));

	for (auto& iter : UIOptionList)
	{
		dynamic_cast<CUI_Opt_Sound*>(iter)->SetActive(TRUE);
		dynamic_cast<CUI_Opt_Sound*>(iter)->OnSwitch(TRUE);
		
	}

}

void CUI_Manager::DestroyOption()
{
	m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::UI_MENU_CLOSE, false, 1.f);

	list<class CGameObject*> UIOptionList  = m_pGameInstance->Get_Layer(LEVEL_GAMEPLAY, TEXT("Layer_UI_Option_Sound"));

	for (auto& iter : UIOptionList)
	{
		iter->SetActive(FALSE);
	}

}

void CUI_Manager::Free()
{
	Safe_Release(m_pGameInstance);

	__super::Free();
}
