#pragma once
#include "UIController.h"

NS_BEGIN(Engine)
class CUIImage;
class CUIText;
class CUIProgressBar;
NS_END

NS_BEGIN(Game_PKM)
class CBattle_Manager;
class CBattler;

/* CBattlePlate
   - UI_BattlePlate.uiseq 위젯들에 양 측 플레이트의 HP/이름/레벨/성별을 매 프레임 lazy 갱신한다.
   - CBattle_Manager 의 Get_Battler(side) 를 폴링하여 슬롯 상태를 가져온다.
   - HP 바는 목표값(현재 HP 비율) 과 표시값을 분리해 부드럽게 lerp 한다.
   - 이벤트 구독 없음 (페이싱 락 미보유 - UI 표시 책임만). */
class CBattlePlate final : public CUIController
{
private:
	CBattlePlate();
	virtual ~CBattlePlate() = default;

public:
	void Bind(CBattle_Manager* pManager);
	void Snap_HPDisplay();

protected:
	virtual HRESULT Resolve_Widgets() override;
	virtual void    On_Refresh() override;
	virtual void    On_Update(_float fTimeDelta) override;

private:
	CBattle_Manager* m_pManager = { nullptr };  // weak

	// ProgressBar
	CUIProgressBar* m_pPlayer_HP = { nullptr };   // widget_005
	CUIProgressBar* m_pEnemy_HP = { nullptr };   // widget_006

	// 이름 텍스트
	CUIText* m_pPlayer_Name = { nullptr };        // widget_008
	CUIText* m_pEnemy_Name = { nullptr };        // widget_009

	// "Lv" 라벨
	CUIImage* m_pPlayer_LV = { nullptr };         // widget_010
	CUIImage* m_pEnemy_LV = { nullptr };         // widget_011

	// 레벨 숫자 (십의 자리 / 일의 자리)
	CUIImage* m_pPlayer_LVNum[2] = { nullptr, nullptr };   // widget_012, 013
	CUIImage* m_pEnemy_LVNum[2] = { nullptr, nullptr };   // widget_014, 015

	// 성별 아이콘
	CUIImage* m_pPlayer_Gender = { nullptr };     // widget_016
	CUIImage* m_pEnemy_Gender = { nullptr };     // widget_017

	// 플레이어 측 HP 숫자
	CUIImage* m_pPlayer_CurHP[2] = { nullptr, nullptr };   // widget_018, 019
	CUIImage* m_pPlayer_HPSlash = { nullptr };             // widget_020
	CUIImage* m_pPlayer_MaxHP[2] = { nullptr, nullptr };   // widget_021, 022

	// HP 보간 상태 (P3-B 에서 사용)
	_float m_fPlayerHP01_Display = { 1.f };
	_float m_fEnemyHP01_Display = { 1.f };

private:
	void Apply_Name(CUIText* pText, CBattler* pBattler);
	void Apply_LVNum(CUIImage* (&pLVNum)[2], CBattler* pBattler);
	void Apply_Gender(CUIImage* pGender, CBattler* pBattler);
	void Apply_HPNumbers(CBattler* pBattler);
	void Apply_TwoDigitImage(CUIImage* (&pDigits)[2], _uint iValue, _bool bHideLeadingZero);

public:
	static CBattlePlate* Create();

private:
	virtual void Free() override;
};

NS_END