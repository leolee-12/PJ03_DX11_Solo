#include "CCamera.h"

CCamera::CCamera(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CCamera::CCamera(const CCamera& rhs)
    : CGameObject(rhs)
{
}

CCamera::~CCamera()
{
}

HRESULT CCamera::Ready_GameObject()
{
    D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
    D3DXMatrixPerspectiveFovLH(&m_matProj, m_fFov, m_fAspect, m_fNear, m_fFar);

    m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);
    m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);

    return S_OK;
}

_int CCamera::Update_GameObject(const _float& fTimeDelta)
{
    D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
    m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);

    return 0;
}

void CCamera::LateUpdate_GameObject(const _float& fTimeDelta)
{
}

void CCamera::Free()
{
    CGameObject::Free();
}
