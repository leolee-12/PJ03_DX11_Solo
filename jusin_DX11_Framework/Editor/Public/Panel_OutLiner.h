#pragma once
#include "Panel_Base.h"

NS_BEGIN(Editor)

class CPanel_OutLiner final : public CPanel_Base
{
protected:
	CPanel_OutLiner();
	virtual ~CPanel_OutLiner() = default;

public:
	HRESULT Initialize() override;
	void Update(_float fTimeDelta) override;
	HRESULT Render() override;

private:
	_char m_szSearchBuffer[128] = { };
	_char m_szRenameBuffer[128] = { };
	_bool m_bOpenRenamePopup = false;
	CGameObject* m_pRenameTarget = { nullptr };

private:
	void Draw_ObjectNode(class CGameObject* pObj);
	void Draw_ContextMenu(class CGameObject* pObj);
	_bool Passes_Filter(const _string& strName);

public:
	static CPanel_OutLiner* Create();

private:
	virtual void Free() override;
};

NS_END