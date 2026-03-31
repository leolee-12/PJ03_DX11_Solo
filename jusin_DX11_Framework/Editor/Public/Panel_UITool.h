#pragma once
#include "Panel_Base.h"

NS_BEGIN(Editor)

class CPanel_UITool final : public CPanel_Base
{
private:
	CPanel_UITool();
	virtual ~CPanel_UITool() = default;

public:
	HRESULT Initialize() override;
	void Update(_float fTimeDelta) override;
	HRESULT Render() override;

private:
	vector<UI_ELEMENT>  m_Elements;
	_int m_iSelectedIdx = { -1 };

private:
	void Draw_UIList();
	void Draw_Canvas();
	void Draw_ElementProps(UI_ELEMENT& tElement);

public:
	static CPanel_UITool* Create();

private:
	virtual void Free() override;
};

NS_END