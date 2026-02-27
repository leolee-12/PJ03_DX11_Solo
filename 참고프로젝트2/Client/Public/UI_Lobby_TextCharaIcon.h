#pragma once

#include "UI_BaseAttBuf.h"

BEGIN(Client)

class CUI_Lobby_TextCharaIcon final :public CUI_BaseAttBuf
{
private:
	CUI_Lobby_TextCharaIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Lobby_TextCharaIcon(const CUI_Lobby_TextCharaIcon& Prototype);
	virtual ~CUI_Lobby_TextCharaIcon() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Camera_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render(_float fTimeDelta);

private:
	virtual HRESULT Ready_Components();

public:
	static CUI_Lobby_TextCharaIcon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END