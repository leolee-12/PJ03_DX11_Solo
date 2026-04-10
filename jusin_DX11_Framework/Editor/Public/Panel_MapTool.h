#pragma once
#include "Panel_Base.h"

NS_BEGIN(Editor)

class CPanel_MapTool final : public CPanel_Base
{
protected:
	CPanel_MapTool();
	virtual ~CPanel_MapTool() = default;

public:
	HRESULT Initialize() override;
	void Update(_float fTimeDelta) override;
	HRESULT Render() override;

private:
	HRESULT Ready_EditableTexture(const _tchar* pFileDir);

public:
	static CPanel_MapTool* Create();
	
private:
	virtual void Free() override;
};

NS_END