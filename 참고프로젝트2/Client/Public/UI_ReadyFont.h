#pragma once

#include "UI_GameState.h"
#include <queue>

BEGIN(Client)

class CUI_ReadyFont final :public CUI_GameState
{
private:
	CUI_ReadyFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_ReadyFont(const CUI_ReadyFont& Prototype);
	virtual ~CUI_ReadyFont() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Camera_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render(_float fTimeDelta);

private:
	virtual HRESULT Ready_Components();
	virtual HRESULT Bind_ShaderResources();

private:
	void Action_StartAnim(_float fTimeDelta);

private:
	_float m_fMaskTimer = { 0.f };
	_float m_fAlphaTimer = { 0.f };

	_bool m_bStart = { FALSE};
	_bool m_bEnd = { FALSE };

	_float m_fStartAnimTimer = { 0.f };
	_float m_fOffSetPosY = {0.f};
	
	CTexture* m_pMaskTexture = { nullptr };


public:
	static CUI_ReadyFont* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END