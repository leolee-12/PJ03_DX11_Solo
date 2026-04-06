#pragma once
#include "Base.h"
#include "Editor_Defines.h"

NS_BEGIN(Engine)
class CGameInstance;
class CGameObject;
NS_END

NS_BEGIN(Editor)
class CEditInstance;

class CPanel_Base abstract : public CBase
{
protected:
	CPanel_Base();
	virtual ~CPanel_Base() = default;

public:
	const _bool Is_Hovered() const { return m_bHovered; }
	const _bool Is_Focused() const { return m_bFocused; }

	_bool* Get_OpenPtr() { return &m_bOpen; }
	const _bool Is_Opened() const { return m_bOpen; }
	void Switch() { m_bOpen = !m_bOpen; }
	const _string& Get_Title() { return m_strTitle; }

	virtual HRESULT Initialize() PURE;
	virtual void Update(_float fTimeDelta) PURE;
	virtual HRESULT Render() PURE;

protected:
	_bool m_bOpen = { true };
	_string m_strTitle = { "Default" };
	ImGuiWindowFlags m_iWindowFlags = { };
	ImVec4 m_vClear_color = { };

	CGameInstance* m_pGameInstance = { nullptr };
	CEditInstance* m_pEditInstance = { nullptr };
	CGameObject* m_pSelected = { nullptr };

	_bool m_bHovered = { false };
	_bool m_bFocused = { false };

protected:
	_bool Begin_Panel();
	void End_Panel();

protected:
	virtual void Free() override;
};

NS_END