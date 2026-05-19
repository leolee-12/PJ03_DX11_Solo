#pragma once
#include "Panel_Base.h"

NS_BEGIN(Editor)
class CEffectEditorSession;

class CPanel_Effect final : public CPanel_Base
{
private:
	CPanel_Effect();
	virtual ~CPanel_Effect() = default;

public:
	HRESULT Initialize() override;
	void Update(_float fTimeDelta) override;
	HRESULT Render() override;

private:
	CEffectEditorSession* m_pSession = { nullptr };

public:
	static CPanel_Effect* Create();

private:
	virtual void Free() override;
};

NS_END