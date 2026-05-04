#pragma once
#include "Base.h"

NS_BEGIN(Game_PKM)
class CUIController;

class CUIController_Hub final : public CBase
{
private:
	CUIController_Hub();
	virtual ~CUIController_Hub() = default;

public:
	HRESULT Initialize();
	HRESULT Register(CUIController* pCtrl);
	void    Unregister(CUIController* pCtrl);
	void    Update_All(_float fTimeDelta);
	void    Close_All();

	/* 디버그/테스트 용 */
	_uint   Get_Count() const { return static_cast<_uint>(m_Controllers.size()); }

private:
	vector<CUIController*> m_Controllers;  // owned (Register 시 AddRef)

public:
	static CUIController_Hub* Create();

protected:
	virtual void Free() override;
};

NS_END