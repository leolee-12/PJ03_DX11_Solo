#pragma once
#include "Base.h"
#include "Tool_Defines.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Tool)

class CToolApp final : public CBase
{
private:
	CToolApp();
	virtual ~CToolApp() = default;

public:
	HRESULT		Initialize();
	void		Update(_float fTimeDelta);
	HRESULT		Render();

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	CGameInstance*			m_pGameInstance = { nullptr };

private:
	HRESULT Start_Level(LEVEL eStartLevelID);

public:
	static CToolApp*	Create();

protected:
	virtual void		Free() override;
};

NS_END