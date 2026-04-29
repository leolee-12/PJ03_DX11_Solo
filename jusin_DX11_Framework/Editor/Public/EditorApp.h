#pragma once
#include "Base.h"
#include "Editor_Defines.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Editor)
class CEditInstance;

class CEditorApp final : public CBase
{
private:
	CEditorApp();
	virtual ~CEditorApp() = default;

public:
	HRESULT Initialize();
	void Update(_float fTimeDelta);
	HRESULT Render();

	void Request_Resize(_uint iNewWidth, _uint iNewHeight);
	HRESULT Apply_Resize();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };
	CEditInstance* m_pEditInstance = { nullptr };

	_bool m_bResizePending = { false };
	_uint m_iPendingWidth = {};
	_uint m_iPendingHeight = {};

public:
	static CEditorApp* Create();

protected:
	virtual void Free() override;
};

NS_END