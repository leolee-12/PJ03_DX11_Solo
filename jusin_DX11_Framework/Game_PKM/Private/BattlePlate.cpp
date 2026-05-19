#include "BattlePlate.h"
#include "Battle_Manager.h"
#include "Battler.h"
#include "UIImage.h"
#include "UIText.h"
#include "UIProgressBar.h"
#include "Capture_Menu.h"

CBattlePlate::CBattlePlate()
{
}

void CBattlePlate::Bind(CBattle_Manager* pManager)
{
	m_pManager = pManager;
}

void CBattlePlate::Snap_HPDisplay()
{
	if (nullptr == m_pManager)
		return;

	auto Read_HP01 = [](CBattler* pBattler) -> _float
		{
			if (nullptr == pBattler)
				return 0.f;

			const _ushort iCur = pBattler->Get_CurrentHP();
			const _ushort iMax = pBattler->Get_MaxHP();
			return (iMax > 0) ? (static_cast<_float>(iCur) / static_cast<_float>(iMax)) : 0.f;
		};

	m_fPlayerHP01_Display = Read_HP01(m_pManager->Get_Battler(g_kBattleSide_Player));
	m_fEnemyHP01_Display = Read_HP01(m_pManager->Get_Battler(g_kBattleSide_Opponent));

	if (nullptr != m_pPlayer_HP)
		m_pPlayer_HP->Set_FillAmount(m_fPlayerHP01_Display);

	if (nullptr != m_pEnemy_HP)
		m_pEnemy_HP->Set_FillAmount(m_fEnemyHP01_Display);
}

HRESULT CBattlePlate::Resolve_Widgets()
{
	m_pPlayer_HP = Find_Widget_As<CUIProgressBar>("widget_005");
	if (nullptr == m_pPlayer_HP) return E_FAIL;

	m_pEnemy_HP = Find_Widget_As<CUIProgressBar>("widget_006");
	if (nullptr == m_pEnemy_HP) return E_FAIL;

	m_pPlayer_Name = Find_Widget_As<CUIText>("widget_008");
	if (nullptr == m_pPlayer_Name) return E_FAIL;

	m_pEnemy_Name = Find_Widget_As<CUIText>("widget_009");
	if (nullptr == m_pEnemy_Name) return E_FAIL;

	m_pPlayer_LV = Find_Widget_As<CUIImage>("widget_010");
	if (nullptr == m_pPlayer_LV) return E_FAIL;

	m_pEnemy_LV = Find_Widget_As<CUIImage>("widget_011");
	if (nullptr == m_pEnemy_LV) return E_FAIL;

	m_pPlayer_LVNum[0] = Find_Widget_As<CUIImage>("widget_012");
	m_pPlayer_LVNum[1] = Find_Widget_As<CUIImage>("widget_013");
	if (nullptr == m_pPlayer_LVNum[0] || nullptr == m_pPlayer_LVNum[1]) return E_FAIL;

	m_pEnemy_LVNum[0] = Find_Widget_As<CUIImage>("widget_014");
	m_pEnemy_LVNum[1] = Find_Widget_As<CUIImage>("widget_015");
	if (nullptr == m_pEnemy_LVNum[0] || nullptr == m_pEnemy_LVNum[1]) return E_FAIL;

	m_pPlayer_Gender = Find_Widget_As<CUIImage>("widget_016");
	if (nullptr == m_pPlayer_Gender) return E_FAIL;

	m_pEnemy_Gender = Find_Widget_As<CUIImage>("widget_017");
	if (nullptr == m_pEnemy_Gender) return E_FAIL;

	m_pPlayer_CurHP[0] = Find_Widget_As<CUIImage>("widget_018");
	m_pPlayer_CurHP[1] = Find_Widget_As<CUIImage>("widget_019");
	if (nullptr == m_pPlayer_CurHP[0] || nullptr == m_pPlayer_CurHP[1]) return E_FAIL;

	m_pPlayer_HPSlash = Find_Widget_As<CUIImage>("widget_020");
	if (nullptr == m_pPlayer_HPSlash) return E_FAIL;

	m_pPlayer_MaxHP[0] = Find_Widget_As<CUIImage>("widget_021");
	m_pPlayer_MaxHP[1] = Find_Widget_As<CUIImage>("widget_022");
	if (nullptr == m_pPlayer_MaxHP[0] || nullptr == m_pPlayer_MaxHP[1]) return E_FAIL;

	return S_OK;
}

void CBattlePlate::On_Refresh()
{
	if (nullptr == m_pManager)
		return;

	Snap_HPDisplay();

	CBattler* pPlayer = m_pManager->Get_Battler(g_kBattleSide_Player);
	CBattler* pEnemy = m_pManager->Get_Battler(g_kBattleSide_Opponent);

	Apply_Name(m_pPlayer_Name, pPlayer);
	Apply_Name(m_pEnemy_Name, pEnemy);

	Apply_LVNum(m_pPlayer_LVNum, pPlayer);
	Apply_LVNum(m_pEnemy_LVNum, pEnemy);

	Apply_Gender(m_pPlayer_Gender, pPlayer);
	Apply_Gender(m_pEnemy_Gender, pEnemy);

	Apply_HPNumbers(pPlayer);
}

void CBattlePlate::On_Update(_float fTimeDelta)
{
	if (nullptr == m_pManager)
		return;

	auto Read_HP01 = [](CBattler* pBattler) -> _float
		{
			if (nullptr == pBattler)
				return 0.f;

			const _ushort iCur = pBattler->Get_CurrentHP();
			const _ushort iMax = pBattler->Get_MaxHP();
			return (iMax > 0) ? (static_cast<_float>(iCur) / static_cast<_float>(iMax)) : 0.f;
		};

	const _float fPlayerTarget = Read_HP01(m_pManager->Get_Battler(g_kBattleSide_Player));
	const _float fEnemyTarget = Read_HP01(m_pManager->Get_Battler(g_kBattleSide_Opponent));

	constexpr _float fLerpSpeed = 0.6f;  // 초당 비율 - 0~1 전체 변화에 약 1.7초
	const _float fStep = fLerpSpeed * fTimeDelta;

	auto Lerp_Step = [fStep](_float fDisplay, _float fTarget) -> _float
		{
			const _float fDelta = fTarget - fDisplay;
			if (fabsf(fDelta) <= fStep)
				return fTarget;

			return fDisplay + ((fDelta > 0.f) ? fStep : -fStep);
		};

	m_fPlayerHP01_Display = Lerp_Step(m_fPlayerHP01_Display, fPlayerTarget);
	m_fEnemyHP01_Display = Lerp_Step(m_fEnemyHP01_Display, fEnemyTarget);

	if (nullptr != m_pPlayer_HP)
		m_pPlayer_HP->Set_FillAmount(m_fPlayerHP01_Display);

	if (nullptr != m_pEnemy_HP)
		m_pEnemy_HP->Set_FillAmount(m_fEnemyHP01_Display);

	CBattler* pPlayer = m_pManager->Get_Battler(g_kBattleSide_Player);
	CBattler* pEnemy = m_pManager->Get_Battler(g_kBattleSide_Opponent);

	Apply_Name(m_pPlayer_Name, pPlayer);
	Apply_Name(m_pEnemy_Name, pEnemy);

	Apply_LVNum(m_pPlayer_LVNum, pPlayer);
	Apply_LVNum(m_pEnemy_LVNum, pEnemy);

	Apply_Gender(m_pPlayer_Gender, pPlayer);
	Apply_Gender(m_pEnemy_Gender, pEnemy);

	Apply_HPNumbers(pPlayer);
}

void CBattlePlate::Apply_Name(CUIText* pText, CBattler* pBattler)
{
	if (nullptr == pText)
		return;

	if (nullptr == pBattler || nullptr == pBattler->Get_Instance())
	{
		pText->Set_Text(TEXT(""));
		return;
	}

	pText->Set_Text(pBattler->Get_Instance()->szNickname);
}

void CBattlePlate::Apply_LVNum(CUIImage* (&pLVNum)[2], CBattler* pBattler)
{
	if (nullptr == pLVNum[0] || nullptr == pLVNum[1])
		return;

	_uint iLevel = 0;

	if (nullptr != pBattler && nullptr != pBattler->Get_Instance())
		iLevel = static_cast<_uint>(pBattler->Get_Instance()->iLevel);

	if (iLevel > 99)
		iLevel = 99;

	Apply_TwoDigitImage(pLVNum, iLevel, true);
}

void CBattlePlate::Apply_Gender(CUIImage* pGender, CBattler* pBattler)
{
	if (nullptr == pGender)
		return;

	(void)pBattler;

	// POKEMON_INSTANCE 에 성별 필드 부재 - 본 트랙에서는 숨김 처리.
	// 데이터 모델 확장 후 P3 후속 단위에서 인덱스(Male=0/Female=1) 적용 예정.
	pGender->Set_Visible(false);
}

void CBattlePlate::Apply_HPNumbers(CBattler* pBattler)
{
	if (nullptr == m_pPlayer_CurHP[0] || nullptr == m_pPlayer_CurHP[1] ||
		nullptr == m_pPlayer_MaxHP[0] || nullptr == m_pPlayer_MaxHP[1])
		return;

	_uint iCur = 0;
	_uint iMax = 0;

	if (nullptr != pBattler)
	{
		iCur = static_cast<_uint>(pBattler->Get_CurrentHP());
		iMax = static_cast<_uint>(pBattler->Get_MaxHP());
	}

	if (iCur > 99) iCur = 99;
	if (iMax > 99) iMax = 99;

	Apply_TwoDigitImage(m_pPlayer_CurHP, iCur, true);
	Apply_TwoDigitImage(m_pPlayer_MaxHP, iMax, true);
}

void CBattlePlate::Apply_TwoDigitImage(CUIImage* (&pDigits)[2], _uint iValue, _bool bHideLeadingZero)
{
	if (nullptr == pDigits[0] || nullptr == pDigits[1])
		return;

	const _uint iTens = iValue / 10;
	const _uint iOnes = iValue % 10;

	if (bHideLeadingZero && 0 == iTens)
	{
		pDigits[0]->Set_Visible(false);
	}
	else
	{
		pDigits[0]->Set_Visible(true);
		pDigits[0]->Set_Texture(pDigits[0]->Get_TextureTag(), iTens);
	}

	pDigits[1]->Set_Visible(true);
	pDigits[1]->Set_Texture(pDigits[1]->Get_TextureTag(), iOnes);
}

CBattlePlate* CBattlePlate::Create()
{
	return new CBattlePlate();
}

void CBattlePlate::Free()
{
	m_pManager = nullptr;

	m_pPlayer_HP = nullptr;
	m_pEnemy_HP = nullptr;

	m_pPlayer_Name = nullptr;
	m_pEnemy_Name = nullptr;

	m_pPlayer_LV = nullptr;
	m_pEnemy_LV = nullptr;

	for (auto& p : m_pPlayer_LVNum) p = nullptr;
	for (auto& p : m_pEnemy_LVNum)  p = nullptr;

	m_pPlayer_Gender = nullptr;
	m_pEnemy_Gender = nullptr;

	for (auto& p : m_pPlayer_CurHP) p = nullptr;
	m_pPlayer_HPSlash = nullptr;
	for (auto& p : m_pPlayer_MaxHP) p = nullptr;

	__super::Free();
}