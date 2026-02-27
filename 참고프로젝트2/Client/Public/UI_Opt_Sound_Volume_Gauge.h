#pragma once

#include "UI_Opt_Sound.h"

BEGIN(Client)

class CUI_Opt_Sound_Volume_Gauge final :public CUI_Opt_Sound
{

private:
	CUI_Opt_Sound_Volume_Gauge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Opt_Sound_Volume_Gauge(const CUI_Opt_Sound_Volume_Gauge& Prototype);
	virtual ~CUI_Opt_Sound_Volume_Gauge() = default;

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
	void PostionUpdate();
	void SetVolume(_float fTimeDelta);
	void KeyInput(SOUND_MENU eSound, _float fTimeDelta);
	void NumberFont();

private:
	_uint m_iNumUI = { 0 };
	_float m_fVolumeRadio = { 0.f };

	_int m_fVolumeValue[MENU_END] = {100 , 100 , 100};

	_float m_fSoundDelay = { 0.f };
	_bool m_bSoundEnable = { TRUE };
	_float m_fSoundWeight = { 0.f };
	
	
public:
	static CUI_Opt_Sound_Volume_Gauge* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END