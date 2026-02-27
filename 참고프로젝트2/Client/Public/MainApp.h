#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "RenderInstance.h"
#include "Imgui_Manager.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Renderer)
class CRenderInstance;
END

BEGIN(Client)

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize();
	void Update(_float fTimeDelta);
	void Fixed_Update(_float fTimeDelta);

	HRESULT Render(_float fTimeDelta);

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

	CGameInstance*				m_pGameInstance = { nullptr };
	CRenderInstance*			m_pRenderInstance = { nullptr };
	CImgui_Manager* m_pImgui_Manager = { nullptr };
	static _bool  m_bIsLevelPrepared;
private:
	HRESULT Open_Level(LEVELID eStartLevelID);
	HRESULT Create_IMGUI_Manager();
	HRESULT Ready_Prototype_Component_ForStatic();
	HRESULT Ready_Fonts();

public:
	static CMainApp* Create();
	virtual void Free() override;
};

END