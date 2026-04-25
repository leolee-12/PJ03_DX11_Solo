#pragma once
#include "Panel_Base.h"

NS_BEGIN(Editor)

class CPanel_UIAnim final : public CPanel_Base
{
private:
	CPanel_UIAnim();
	virtual ~CPanel_UIAnim() = default;

public:
	HRESULT Initialize() override;
	void Update(_float fTimeDelta) override;
	HRESULT Render() override;

private:
	class CUIEditorSession* m_pSession = { nullptr };

private:
	void Draw_Inspector();

public:
	static CPanel_UIAnim* Create();

private:
	virtual void Free() override;
};

NS_END