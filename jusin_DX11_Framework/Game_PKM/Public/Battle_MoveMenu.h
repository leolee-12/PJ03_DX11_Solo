#pragma once
#include "UIController.h"
#include "Battle_Data.h"

NS_BEGIN(Engine)
class CUIButton;
class CUIText;
class CUIImage;
NS_END

NS_BEGIN(Game_PKM)

class CBattle_Manager;

/* UI_BattleMove.uiseq 전용 컨트롤러.
   - 4기술 버튼 입력 처리 (LINEAR + WrapAround)
   - 매 프레임 플레이어 측 포켓몬의 기술 이름 / PP / 타입 아이콘 lazy 갱신
   - 빈 슬롯은 버튼/텍스트/아이콘 모두 Set_Visible(false)
   - PP 0 슬롯 선택 제한은 Director 가 후처리 (본 메뉴는 표시 책임만). */
class CBattle_MoveMenu final : public CUIController
{
public:
	enum class SLOT { SLOT1, SLOT2, SLOT3, SLOT4, END };

private:
	CBattle_MoveMenu();
	virtual ~CBattle_MoveMenu() = default;

public:
	void Bind(CBattle_Manager* pManager);
	SLOT Get_FocusedSlot() const;

protected:
	virtual HRESULT Resolve_Widgets() override;
	virtual HRESULT Resolve_Buttons() override;
	virtual HRESULT Build_Group()     override;
	virtual void    On_Refresh()      override;
	virtual void    On_Update(_float fTimeDelta) override;

private:
	void Refresh_Slots();

private:
	CBattle_Manager* m_pManager = { nullptr };  // weak

	CUIButton* m_Buttons[ETOUI(SLOT::END)]{ nullptr };
	CUIText* m_Names[ETOUI(SLOT::END)]{ nullptr };
	CUIText* m_PPs[ETOUI(SLOT::END)]{ nullptr };
	CUIImage* m_Icons[ETOUI(SLOT::END)]{ nullptr };

public:
	static CBattle_MoveMenu* Create();

private:
	virtual void Free() override;
};

NS_END