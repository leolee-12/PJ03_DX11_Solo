#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CShadow_Camera final : public CCamera
{
public:
	typedef struct : public CCamera::CAMERA_DESC
	{
		_float		fSensor = {};
	}Shadow_Camera_DESC;

private:
	CShadow_Camera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShadow_Camera(const CShadow_Camera& Prototype);
	virtual ~CShadow_Camera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Camera_Update(_float fTimeDelta) override;
	virtual void Player_Update(_float fTimeDelta) override;
	
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;

private:
	_float				m_fMouseSensor = {};

public:
	static CShadow_Camera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END