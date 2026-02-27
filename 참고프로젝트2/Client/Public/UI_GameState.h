#pragma once

#include "UIObject.h"
#include <queue>

BEGIN(Client)

class CUI_GameState abstract : public CUIObject
{
public:
	typedef struct
	{
		_int iPos = {};
		_float fEventFrame = {};
	}ANIM_INFO;

protected:
	CUI_GameState(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_GameState(const CUI_GameState& Prototype);
	virtual ~CUI_GameState() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Camera_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render(_float fTimeDelta);

protected:
	void Set_AnimPosition(_int iPosX, _float fAnimSpeed);
	void Action_Anim(_float fSizeOffSet, _float fTimeDelta);

protected:
	_bool m_bAnimStop = { FALSE };

	_float m_fScaleAnimTimer = { 0.f };
	_float m_fStopTimer = { 0.f };

	deque<ANIM_INFO> m_QueueAnim = {};

	_float m_fAnimFrame = {};
	_float m_fAnimPos = { 0.f};

	_float m_fTotalAnimDuration = { 0.f };

protected:
	virtual HRESULT Bind_ShaderResources();
	virtual HRESULT Ready_Components();

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END

