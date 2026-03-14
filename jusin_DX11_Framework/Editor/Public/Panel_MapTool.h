#pragma once
#include "Panel_Base.h"

NS_BEGIN(Editor)

class CPanel_MapTool final : public CPanel_Base
{
protected:
	CPanel_MapTool();
	virtual ~CPanel_MapTool() = default;

public:
	HRESULT Initialize(void* pArg) override;
	void	Update(_float fTimeDelta) override;
	void	Render() override;

public:
	static CPanel_MapTool*	Create(void* pArg = nullptr);
	
protected:
	virtual void			Free() override;
};

NS_END