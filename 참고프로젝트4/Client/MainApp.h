#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CMainApp final  : public CBase
{
	INFO_CLASS(CMainApp, CBase)

private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize();
	void Tick(_cref_time fTimeDelta);
	HRESULT Render();

private:
	/* IDirect3DDevice9 == LPDIRECT3DDEVICE9 */
	ComPtr<ID3D11Device>			m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>	m_pContext = { nullptr };
	CGameInstance*			m_pGameInstance = { nullptr };

private:
	HRESULT Open_Level(LEVEL eStartLevelID);
	HRESULT Ready_Prototype_Component_ForStaticLevel();
	HRESULT Add_StaticEvnet();
	HRESULT Ready_Font();

	HRESULT	Ready_LoadingLogo();
	void RecompileShader();

public:
	static CMainApp* Create();
	virtual void Free();

	/*****TEST*****/

	HRESULT Ready_Gara();
};

END