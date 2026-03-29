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
	const _bool Is_Opened() const { return m_bOpen; }
	void Switch() { m_bOpen = !m_bOpen; }

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

protected:
	_bool Begin_Panel();
	void End_Panel();

protected:
	virtual void Free() override;
};

NS_END