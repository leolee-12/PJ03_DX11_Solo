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
