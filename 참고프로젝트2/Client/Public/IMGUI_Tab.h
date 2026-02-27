#pragma once

#include "Base.h"
#include "Client_Defines.h"
#include "imgui.h"
#include "imnodes.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Renderer)
class CRenderInstance;
END

BEGIN(Client)

class CIMGUI_Tab : public CBase
{

protected:
	CIMGUI_Tab(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CIMGUI_Tab() = default;

public:
	virtual HRESULT Initialize() = 0;
	virtual void Render(_float fTimeDelta) = 0;

public:
	virtual const _char* GetTabName() const = 0;
	virtual void Free();

protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class Renderer::CRenderInstance* m_pRenderInstance = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };
};

END
