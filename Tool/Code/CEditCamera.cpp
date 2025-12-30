#include "pch.h"
#include "CEditCamera.h"
#include "CDInputMgr.h"

CEditCamera::CEditCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	:	CCamera(pGraphicDev)
{
}

CEditCamera::CEditCamera(const CEditCamera& rhs)
	:	CCamera(rhs)
{
}

CEditCamera::~CEditCamera()
{
}

HRESULT CEditCamera::Ready_GameObject(	const _vec3* pEye,
										const _vec3* pAt,
										const _vec3* pUp,
										const _float& fFov,
										const _float& fAspect,
										const _float& fNear,
										const _float& fFar)
{
	m_vEye = *pEye;
	m_vAt = *pAt;
	m_vUp = *pUp;

	m_fFov = fFov;
	m_fAspect = fAspect;
	m_fNear = fNear;
	m_fFar = fFar;

	if (FAILED(CCamera::Ready_GameObject()))
		return E_FAIL;

	m_fSpeed = 10.f;

	return S_OK;
}

_int CEditCamera::Update_GameObject(const _float& fTimeDelta)
{
	_int	iExit = CCamera::Update_GameObject(fTimeDelta);

	return iExit;
}

void CEditCamera::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CCamera::LateUpdate_GameObject(fTimeDelta);
}

void CEditCamera::Free()
{
	CCamera::Free();
}
