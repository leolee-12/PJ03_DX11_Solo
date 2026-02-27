#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Lobby final : public CLevel
{
private:
	CLevel_Lobby(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Lobby() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;

	void Change_Level();

private:
	HRESULT Ready_Sound();

	_bool m_bChangeLevel = { false };
public:
	static CLevel_Lobby* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END