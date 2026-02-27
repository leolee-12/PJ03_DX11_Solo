#pragma once

#include "UI_GameState.h"
#include <queue>

BEGIN(Client)

class CUI_GameStartCircle final :public CUI_GameState
{


public:
	typedef struct
	{
		_uint iPos = {};
		_float m_fSpeed = {};

		_bool bStop = {FALSE};
		_float fStopDuration = {0.f};
		
	}ANIM_INFO;
private:
	CUI_GameStartCircle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_GameStartCircle(const CUI_GameStartCircle& Prototype);
	virtual ~CUI_GameStartCircle() = default;

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
	void Action_Rotaion(_float fTimeDelta);

private:
	_uint m_iTextureIndex = { 0 };

	_bool m_bEmblem = { FALSE };
	_uint m_iTotalAnimSize = { 0 };

	_float m_fTimer = { 0.f };


public:
	static CUI_GameStartCircle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END