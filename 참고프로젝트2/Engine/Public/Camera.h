#pragma once

#include "GameObject.h"
#include "Transform.h"

/* 클라이언트 개발자가 제작한 카메라들이 상속받아야할 부모클래스. */
/* 모든 카메라에 필요한 데이터와 기능을 구현한다. */

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	typedef struct : public CTransform::TRANSFORM_DESC
	{
		_float		fFovy, fNear, fFar;
	}CAMERA_DESC;

protected:
	CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera(const CCamera& Prototype);
	virtual ~CCamera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Camera_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;

	void Update_Camera(CCamera* camera, _float fTimeDelta);

public:
	_float3					m_vEye{}, m_vAt{};
	_float					m_fFovy{}, m_fNear{}, m_fFar{};
	_float					m_fViewportWidth{}, m_fViewportHeight{};

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END