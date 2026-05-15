#pragma once
#include "Game_PKM_Defines.h"
#include "Camera.h"

NS_BEGIN(Game_PKM)

class CCamera_Free final : public CCamera
{
public:
	struct CAMERA_FREE_DESC final : public CCamera::CAMERA_DESC
	{
		_float fMouseSensor;
		_bool bControlEnabled = { false };
	};

private:
	CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Free(const CCamera_Free& Prototype);
	virtual ~CCamera_Free() = default;

public:
	virtual void Set_ControlEnabled(_bool bEnabled) override { m_bControlEnabled = bEnabled; }
	virtual _bool Is_ControlEnabled() const override { return m_bControlEnabled; }
	virtual _string Get_TypeName() const { return "Camera_Free"; }
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_float	m_fMouseSensor = {};
	_bool m_bControlEnabled = { false };

public:
	static CCamera_Free* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

private:
	virtual void Free() override;

};

NS_END