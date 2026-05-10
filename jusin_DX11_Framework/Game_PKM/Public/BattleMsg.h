#pragma once
#include "UIController.h"

NS_BEGIN(Engine)
class CUIImage;
class CUIText;
NS_END

NS_BEGIN(Game_PKM)

class CBattleMsg final : public CUIController
{
private:
	CBattleMsg();
	virtual ~CBattleMsg() = default;

public:
	void  Set_Message(const _wstring& strMessage);
	void  Complete();
	_bool Is_Done() const;

protected:
	virtual HRESULT Resolve_Widgets() override;
	virtual void    On_Refresh() override;
	virtual void    On_Update(_float fTimeDelta) override;

private:
	CUIImage* m_pIcon{ nullptr };  // weak
	CUIImage* m_pBox{ nullptr };   // weak
	CUIText* m_pText{ nullptr };  // weak

	_wstring m_strFull{};
	_float   m_fTimer{ 0.f };
	_float   m_fCharsPerSecond{ 45.f };
	_uint    m_iCharsShown{ 0 };
	_bool    m_bIconActive{ false };

public:
	static CBattleMsg* Create();

private:
	virtual void Free() override;
};

NS_END