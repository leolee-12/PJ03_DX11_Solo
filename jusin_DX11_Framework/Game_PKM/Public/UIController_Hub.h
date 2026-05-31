#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CUISequence;
class CUIButton;
NS_END

NS_BEGIN(Game_PKM)
class CUIController;

class CUIController_Hub final : public CBase
{
private:
	CUIController_Hub();
	virtual ~CUIController_Hub() = default;

public:
	HRESULT Initialize();

	HRESULT Register(CUIController* pCtrl, _uint iOwnerLevel);
	void    Unregister(CUIController* pCtrl);
	void    Update_All(_float fTimeDelta);
	void    Close_All();
	void    Cleanup_Level(_uint iOwnerLevel);
	/* 등록된 컨트롤러 중 하나라도 Open 상태면 true.
   레벨 전환 가드용 - UI 열린 상태에서의 Push/Pop 차단. */
	_bool   Is_AnyOpen() const;

	/* 커서 시퀀스 주입 (weak 보관). 단일 인스턴스 공용.
   nullptr 을 넘기면 해제. */
	void    Set_Cursor_Sequence(class Engine::CUISequence* pSeq);

	/* 디버그/테스트 용 */
	_uint   Get_Count() const { return static_cast<_uint>(m_Controllers.size()); }

private:
	struct CONTROLLER_ENTRY
	{
		_uint iOwnerLevel = {};
		CUIController* pCtrl = { nullptr };
	};

	vector<CONTROLLER_ENTRY> m_Controllers;  // owned (Register 시 AddRef)
	Engine::CUISequence* m_pCursor{ nullptr };  // weak (level owns)

	/* 활성 컨트롤러 전환 시 짧은 딜레이 동안 커서 숨김.
   이전 활성 컨트롤러 포인터는 비교용 weak - Safe_Release 안 함. */
	CUIController* m_pLastActiveCtrl{ nullptr };
	_float         m_fCursorShowDelay{ 0.333f };
	_float         m_fCursorShowTimer{ 0.f };

private:
	CUIController* Find_Active_Controller() const;
	void           Update_Cursor(_float fTimeDelta);

public:
	static CUIController_Hub* Create();

protected:
	virtual void Free() override;
};

NS_END