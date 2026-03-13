#pragma once
#include "Base.h"
#include "Tool_Defines.h"

NS_BEGIN(Tool)

class CPanel_Base abstract : public CBase
{
protected:
	CPanel_Base();
	virtual ~CPanel_Base() = default;

public:
	const _bool		Is_Opened() const { return m_bOpen; }
	void			Switch() { m_bOpen = !m_bOpen; }

	virtual HRESULT Initialize(void* pArg = nullptr) PURE;
	virtual void	Update(_float fTimeDelta) PURE;
	virtual void	Render() PURE;

protected:
	_bool				m_bOpen = { true };
	_string				m_strTitle = { };
	ImGuiWindowFlags_	m_iWindowFlags = { };
	ImVec4				m_vClear_color = { };

protected:
	_bool	Begin_Panel();
	void	End_Panel();

protected:
	virtual void Free() override;
};

NS_END