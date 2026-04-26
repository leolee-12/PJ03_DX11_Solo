#pragma once
#include "Panel_Base.h"

NS_BEGIN(Editor)

class CPanel_UILayout final : public CPanel_Base
{
private:
	CPanel_UILayout();
	virtual ~CPanel_UILayout() = default;

public:
	HRESULT Initialize() override;
	void Update(_float fTimeDelta) override;
	HRESULT Render() override;

private:
	class CUIEditorSession* m_pSession = { nullptr };

	enum class PENDING_ACTION { NONE, NEW_DOC, LOAD };
	PENDING_ACTION m_ePendingAction = PENDING_ACTION::NONE;
	_string m_strPendingPath;

private:
	void Draw_Toolbar();
	void Draw_Hierarchy();
	void Draw_Inspector();

public:
	static CPanel_UILayout* Create();

private:
	virtual void Free() override;
};

NS_END
