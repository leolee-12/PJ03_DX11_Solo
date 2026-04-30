#pragma once
#include "Base.h"
#include "Editor_Defines.h"

NS_BEGIN(Editor)
class CEditInstance;
class CVP_CoordMapper;

class CVP_OverlayDrawer final : public CBase
{
private:
	CVP_OverlayDrawer();
	virtual ~CVP_OverlayDrawer() = default;

public:
	HRESULT Initialize(CVP_CoordMapper* pMapper);
	void Draw(_bool bHovered);

private:
	CEditInstance* m_pEditInstance = { nullptr };
	CVP_CoordMapper* m_pMapper = { nullptr };

public:
	static CVP_OverlayDrawer* Create(CVP_CoordMapper* pMapper);

private:
	virtual void Free() override;
};

NS_END