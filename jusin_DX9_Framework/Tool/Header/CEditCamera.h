#pragma once
#include "CCamera.h"
#include "Engine_Define.h"

class CEditCamera : public CCamera
{
private:
	explicit CEditCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CEditCamera(const CEditCamera& rhs);
	virtual ~CEditCamera();

public:
	HRESULT		Ready_GameObject(	const _vec3* pEye,
									const _vec3* pAt,
									const _vec3* pUp,
									const _float& fFov,
									const _float& fAspect,
									const _float& fNear,
									const _float& fFar);

	_int		Update_GameObject(const _float& fTimeDelta) override;
	void		LateUpdate_GameObject(const _float& fTimeDelta) override;

private:
	_float		m_fSpeed;

public:
	static CEditCamera* Create(	LPDIRECT3DDEVICE9 pGraphicDev,
								const _vec3* pEye,
								const _vec3* pAt,
								const _vec3* pUp,
								const _float& fFov = D3DXToRadian(60.f),
								const _float& fAspect = (_float)WINCX / WINCY,
								const _float& fNear = 0.1f,
								const _float& fFar = 1000.f);

private:
	void				Free() override;


};

