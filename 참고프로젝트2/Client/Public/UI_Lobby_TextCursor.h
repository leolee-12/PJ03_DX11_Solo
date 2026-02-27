#pragma once

#include "UIObject.h"

BEGIN(Client)

class CUI_Lobby_TextCursor final :public CUIObject
{
private:
	CUI_Lobby_TextCursor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Lobby_TextCursor(const CUI_Lobby_TextCursor& Prototype);
	virtual ~CUI_Lobby_TextCursor() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Camera_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render(_float fTimeDelta);

private:
	virtual HRESULT Ready_Components();

private:
	_float m_fOffsetPosY = { 0.f };
	_bool  m_bOffsetSwitch = { FALSE };

public:
	static CUI_Lobby_TextCursor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END