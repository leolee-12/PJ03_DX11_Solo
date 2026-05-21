#pragma once
#include "Game_PKM_Defines.h"
#include "Game_LevelEntry.h"
#include "Battle_Session.h"
#include "Capture_Session.h"
#include "Level.h"

NS_BEGIN(Engine)
class CUIImage;
class CUIButton;
class CUIProgressBar;
class CUIText;
class CUISequence;
class CGameObject;
NS_END

NS_BEGIN(Game_PKM)
class CMenu;
class CBattleMsg;
class CEvent_Manager;

class CLevel_GamePlay : public CLevel
{
private:
	CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_GamePlay() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void OnPause() override;
	virtual void OnResume() override;

	_bool Request_Battle(const BATTLE_ENV& tEnv);
	void Request_Capture(const CAPTURE_ENV& tEnv, CGameObject* pTarget);
	_bool Start_Dialogue(const _wstring& strDialogueKey);
	_bool Start_Dialogue_Text(const _wstring& strMessage);
	_bool Is_Dialogue_Playing() const;
	_bool Is_Dialogue_Done() const;
	_bool Is_Event_Playing() const;
	void Close_Dialogue();

	CEvent_Manager* Get_EventManager() const { return m_pEventMgr; }

private:
	CUISequence* m_pRuntimeUI = { nullptr };
	CUISequence* m_pCursorSeq = { nullptr };
	CUISequence* m_pFadeBattleSeq = { nullptr };
	CMenu* m_pMenu = { nullptr };
	CEvent_Manager* m_pEventMgr = { nullptr };

	CUISequence* m_pDialogueSeq = { nullptr };   // weak
	CBattleMsg* m_pDialogueMsg = { nullptr };    // weak - Hub owns
	_bool m_bDialogueActive = { false };
	_wstring m_strActiveDialogueKey;

	vector<_wstring> m_DialoguePages;
	_uint m_iDialoguePageIndex = { 0 };

	// F6 트랜지션 상태 머신
	enum class TRANSITION_STATE { IDLE, BUSY, END };
	TRANSITION_STATE m_eTransition{ TRANSITION_STATE::IDLE };
	_float m_fTransitionElapsed{ 0.f };
	LEVEL_ENTRY_DESC m_PendingEntryDesc;
	CGameObject* m_pPendingDeleteWild = { nullptr };   // weak - Push 직전 Set_Dead 호출 후 nullptr 초기화
	static constexpr _float TRANSITION_PUSH_AT_SEC = 5.0f;

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera(WNameID strLayerTag);
	HRESULT Ready_Layer_BackGround(WNameID strLayerTag);
	HRESULT Ready_Layer_Player(WNameID strLayerTag);
	HRESULT Ready_Layer_Monster(WNameID strLayerTag);
	HRESULT Ready_Layer_NPC(WNameID strLayerTag);
	HRESULT Ready_Layer_Wild(WNameID strLayerTag);
	HRESULT Ready_Layer_Effect(WNameID strLayerTag);
	HRESULT Ready_Layer_UI(WNameID strLayerTag);
	HRESULT Ready_EventSystem();

public:
	static CLevel_GamePlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

protected:
	virtual void Free() override;
};

NS_END
