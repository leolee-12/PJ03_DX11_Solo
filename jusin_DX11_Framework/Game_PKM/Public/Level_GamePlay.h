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
class CTexture;
NS_END

NS_BEGIN(Game_PKM)
class CMenu;
class CBattleMsg;
class CEvent_Manager;
class CRegion_Manager;
class CEntry;

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
	void Lock_DialogueInput(_float fSeconds);
	void Close_Dialogue();

	/* 컷신 페이드. bFadeOut=true 면 UI_FadeOut(투명→검정·유지), false 면 UI_Fade(검정→투명).
	   두 오버레이는 상호 배타로, 하나를 재생하면 다른 하나는 즉시 숨긴다(검정↔검정 순간이라 비가시). */
	_bool Play_CutsceneFade(_bool bFadeOut);
	_bool Is_CutsceneFade_Playing(_bool bFadeOut) const;

	CEvent_Manager* Get_EventManager() const { return m_pEventMgr; }

private:
	CUISequence* m_pRuntimeUI = { nullptr };
	CUISequence* m_pCursorSeq = { nullptr };
	CUISequence* m_pFadeCutSceneSeq = { nullptr }; // UI_Fade.uiseq
	CUISequence* m_pFadeCutSceneOutSeq = { nullptr }; // UI_FadeOut.uiseq
	CUISequence* m_pFadeBattleSeq = { nullptr };   // UI_FadeBattle.uiseq
	CUISequence* m_pFadeCaptureSeq = { nullptr };  // UI_FadeCapture.uiseq
	CUISequence* m_pTransitionFadeSeq = { nullptr };
	CMenu* m_pMenu = { nullptr };
	CEntry* m_pEntry = { nullptr };
	CUISequence* m_pEntrySeq = { nullptr };
	CUISequence* m_pDialogueSeq = { nullptr };   // weak
	CBattleMsg* m_pDialogueMsg = { nullptr };    // weak - Hub owns
	CUISequence* m_pRegionSeq = { nullptr };
	CUIText* m_pRegionNameText = { nullptr };

	CEvent_Manager* m_pEventMgr = { nullptr };
	CRegion_Manager* m_pRegionMgr = { nullptr };

	_bool m_bDialogueActive = { false };
	_wstring m_strActiveDialogueKey;

	vector<_wstring> m_DialoguePages;
	_uint m_iDialoguePageIndex = { 0 };
	_float m_fDialogueInputLockRemain = { 0.f };
	_bool m_bPendingRockBattleReturnEvent = { false };

	// F6 트랜지션 상태 머신
	enum class TRANSITION_STATE { IDLE, BUSY, END };
	TRANSITION_STATE m_eTransition{ TRANSITION_STATE::IDLE };
	_float m_fTransitionElapsed{ 0.f };
	LEVEL_ENTRY_DESC m_PendingEntryDesc;
	CGameObject* m_pPendingDeleteWild = { nullptr };   // weak - Push 직전 Set_Dead 호출 후 nullptr 초기화
	static constexpr _float TRANSITION_PUSH_AT_SEC = 5.0f;

	CTexture* m_pCloudTexture = { nullptr };
	DECAL_PARAM m_CloudParam{};

	// 포트폴리오 아웃트로 — END 키로 검정 페이드 + BGM 페이드아웃 (영상 종료 컷)
	_bool  m_bOutroFading = { false };
	_float m_fOutroElapsed = { 0.f };
	static constexpr _float OUTRO_FADE_DURATION = 2.0f;

#ifdef _DEBUG
	OUTLINE_PARAM m_DebugOutlineParam{};

	_float m_fDebugFpsAccum = { 0.f };
	_uint  m_iDebugFpsFrames = { 0 };
	_float m_fDebugFps = { 0.f };
	_float m_fDebugRendererMSAccum = { 0.f };
	_float m_fDebugRendererMS = { 0.f };
#endif

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
	HRESULT Ready_RegionSystem();
	HRESULT Ready_MainCamera();
	HRESULT Ready_Cloud();

	_bool Tick_Dialogue(_float fTimeDelta);
	_bool Tick_Event(_float fTimeDelta);
	_bool Tick_Transition(_float fTimeDelta);
	void Reset_Transition();
	void Prime_CutsceneFadeInBlack();
	void Tick_Gameplay(_float fTimeDelta);
	void Tick_Outro(_float fTimeDelta);

#ifdef _DEBUG
	void Debug_Common();
	void Debug_Outline();
	void Debug_Event();
	void Debug_Culling();
	void Debug_Decal();
	void Debug_TickFPS(_float fTimeDelta);
	void Debug_RenderFPS();
#endif

public:
	static CLevel_GamePlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

protected:
	virtual void Free() override;
};

NS_END
