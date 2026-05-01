#pragma once
#include "Base.h"
#include "Editor_Defines.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Editor)
class CEditInstance;
class CVP_CoordMapper;

class CVP_UIEditCtrl final : public CBase
{
private:
	CVP_UIEditCtrl();
	virtual ~CVP_UIEditCtrl() = default;

public:
	HRESULT Initialize(CVP_CoordMapper* pMapper);

	void Handle_UIPick();
	void Handle_UIDrag();
	void Cancel_Drag() { m_bUIDragging = false; }

private:
	CGameInstance* m_pGameInstance = { nullptr };
	CEditInstance* m_pEditInstance = { nullptr };
	CVP_CoordMapper* m_pMapper = { nullptr };

	_bool   m_bUIDragging = { false };
	_string m_strDragId = {};
	ImVec2  m_vDragStartMouse = {};
	_bool   m_bDragWasAnchored = { false };
	_float  m_fDragStartCenterX = 0.f;
	_float  m_fDragStartCenterY = 0.f;

public:
	static CVP_UIEditCtrl* Create(CVP_CoordMapper* pMapper);

private:
	virtual void Free() override;
};

NS_END