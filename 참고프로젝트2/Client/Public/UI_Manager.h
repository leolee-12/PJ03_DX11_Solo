#pragma once

#include "Base.h"
#include "UI_Define.h"
#include "Client_Defines.h"
#include "input.h"

BEGIN(Engine)

class CGameInstance;

END

BEGIN(Client)


class CUI_Manager final : public CBase
{
public:
	enum ThreadPool_For_Loading
	{
		THREAD_TEXTURE_0,
		THREAD_TEXTURE_1,
		THREAD_TEXTURE_2,
		THREAD_MODEL_0,
		THREAD_MODEL_1,
		THREAD_OBJECT_0,
		THREAD_COMPONENT_0,
		THREAD_END
	};

	DECLARE_SINGLETON(CUI_Manager)

private:
	CUI_Manager();
	virtual ~CUI_Manager() = default;

	//구조
public:
	void Add_UIOjbect(CUI_Define::UI_LISTPOS ePos ,class CUIObject* pObject);

	//기능
public:
	void GamePlayUpdate(_float fTimeDelta);
	void UsingAttckBuff(CUI_Define::PLAYER_SLOT eSlotID);
	void UsingAttackDestroy(CUI_Define::PLAYER_SLOT eSlotID);

	void UsingSelectCharacher(class CCharacter* pPawn, CUI_Define::PLAYER_SLOT eSlotID) { m_pPawnArray[eSlotID] = pPawn; }
	void UsingChangeCharacher(CUI_Define::PLAYER_SLOT eCurrSlotID);
	void UsingCreateStartUI();
	void UsingCreateEndUI();

	void CutSceneUI(_bool bState);

	void WinUI(LEVELID CreateLevel);
	//void CutSceneFinish();

public:
	//디버깅 위치 설정
	void UI_Setting_Debug(_float& fSizeX , _float& fSizeY , _float& fPosX , _float& fPosY , _float fSizeRadio);
	//값 초기화
	void InitUIObject();

	//쓰레드 관련
public:
	void AddToQueue(ThreadPool_For_Loading taskType)
	{
		lock_guard<mutex> lock(m_queueMutex);
		m_QueueThreadPool.push(taskType);
	}

	ThreadPool_For_Loading Get_Thread()
	{
		if (m_QueueThreadPool.empty() == FALSE)
		{
		
			ThreadPool_For_Loading TheadID =  m_QueueThreadPool.front();
			m_QueueThreadPool.pop();
			m_iNumThreadFinish++;
			return TheadID;
		}

		return THREAD_END;
	}

private:
	queue<ThreadPool_For_Loading> m_QueueThreadPool;
	mutex m_queueMutex;

private:
	void CreateOption();
	void DestroyOption();

public:
	_bool m_bStun = { FALSE };
	_bool m_bHit = { FALSE };
	_uint m_iHp = {0};
	_uint m_iNumThreadFinish = { 0 };

	_bool m_bGamePlayLoadingFinish = { FALSE };
	
public:
	class CCharacter* m_pPawnArray[CUI_Define::SLOT_END] = {nullptr,nullptr ,nullptr ,nullptr };

	//UIObject 가 false 일때 (로딩 더 줄이기 위해) 바로 게임플레이 넘어가게 끔 하는 코드
	_bool m_bActive = { FALSE };

	//Anim
	_float m_fTotalDuration = { 0.f };

	//Input
	DirectionInput	m_eDirInput = {};
	ButtonInput		m_eBtnInput = {};

	DirectionInput	m_eDirInput2 = {};
	ButtonInput		m_eBtnInput2 = {};

	_uint m_iNumCommandList = { 0 };
	_uint m_iNumCommandList2 = { 0 };

	//RedBlueTimer
	_float m_fColorValue = { 0.f };
	_bool m_bChange[2] = {FALSE , FALSE };
	 
private:
	_float m_fStartTimer = { 0.f };
	_float m_fEndTimer = { 0.f };

	_bool m_bStartFinish = { FALSE };
	_bool m_bEndFinish = { FALSE };

private:
	_float m_fStartUITimer = { 0.f };
	_bool m_bStartUI = { FALSE };
public:
	_bool m_bOnOption = { FALSE };
	CUI_Define::NPC_ID m_eNpcID = {};

public:
	LEVELID m_eLevelID = { LEVEL_END };
	_bool m_bCharaSelectFinish = { FALSE };

public:
	list<class CUIObject*> m_ListTopUI = {};
	list<class CUIObject*> m_ListMidUI = {};
	list<class CUIObject*> m_ListBotUI = {};

public:
	_bool m_bZeroHp = { FALSE };


private:
	CGameInstance* m_pGameInstance = { nullptr };

public:
	virtual void Free() override;
};

END