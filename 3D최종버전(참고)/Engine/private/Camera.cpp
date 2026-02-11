#include "Camera.h"

#include "GameInstance.h"

CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CCamera::CCamera(const CCamera& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CCamera::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCamera::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    CAMERA_DESC* pDesc = static_cast<CAMERA_DESC*>(pArg);

    m_fFovy = pDesc->fFovy;
    m_fNear = pDesc->fNear;
    m_fFar = pDesc->fFar;

    _uint       iNumViewports = { 1 };
    D3D11_VIEWPORT      ViewportDesc{};

    m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

    m_fAspect = static_cast<_float>(ViewportDesc.Width) / ViewportDesc.Height;

    /* 사용자가 셋팅하고 싶은 카메라의 초기 상태를 트랜스폼에 동기화했다. */
    /* 뷰행렬을 만들기위한 어느정도의 준비는 됐다. */
    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vEye), 1.f));
    m_pTransformCom->LookAt(XMVectorSetW(XMLoadFloat3(&pDesc->vAt), 1.f));

   /* 초기데이터 파이플아ㅣㄴ에 셋팅. */
    Update_PipeLine();

    return S_OK;
}

void CCamera::Priority_Update(_float fTimeDelta)
{

}

void CCamera::Update(_float fTimeDelta)
{
}

void CCamera::Late_Update(_float fTimeDelta)
{
}

HRESULT CCamera::Render()
{

    return S_OK;
}

void CCamera::Update_PipeLine()
{
    m_pGameInstance->Set_Transform(D3DTS::VIEW, m_pTransformCom->Get_WorldMatrix_Inverse());
    m_pGameInstance->Set_Transform(D3DTS::PROJECTION, XMMatrixPerspectiveFovLH(m_fFovy, m_fAspect, m_fNear, m_fFar));


}

void CCamera::Free()
{
    __super::Free();


}
