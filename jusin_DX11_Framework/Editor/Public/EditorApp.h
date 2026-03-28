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

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };
	CEditInstance* m_pEditInstance = { nullptr };

public:
	static CEditorApp* Create();

protected:
	virtual void Free() override;
};

NS_END