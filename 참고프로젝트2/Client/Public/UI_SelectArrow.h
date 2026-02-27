#pragma once

#include "UIObject.h"
#include "UI_Define.h"

BEGIN(Client)

class CUI_SelectArrow final :public CUIObject
{
private:
	CUI_SelectArrow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_SelectArrow(const CUI_SelectArrow& Prototype);
	virtual ~CUI_SelectArrow() = default;

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
	void PositionUpdate(_float fTimeDelta);
	void Move(_float fTimeDelta);

public:
	void SelectChoice() { m_iNumChoice++; m_iTextureIndex++; }
	_uint Get_TextrueIndex() { return m_iTextureIndex; }
	_uint Get_NumChoice() { return m_iNumChoice; }
	void SetPlayerID(CUI_Define::PLAYER_ID eID) { m_ePlayerID = eID; }
	CUI_Define::PLAYER_ID  GetPlayerID() { return m_ePlayerID ; }
	

private:
	CUI_Define::PLAYER_ID m_ePlayerID = { CUI_Define::PAWN_END };
	_bool m_bMoveStop = { FALSE };

	_uint m_iTextureIndex = { 0 };
	_uint m_iNumChoice = { 0 };

	_float m_fSpeedOffset = { 1.f };


public:
	static CUI_SelectArrow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END