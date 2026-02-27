#include "UIObject.h"

#include "Transform.h"

CUIObject::CUIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject { pDevice, pContext }
{
}

CUIObject::CUIObject(const CUIObject& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CUIObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUIObject::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    UIOBJECT_DESC* pDesc = static_cast<UIOBJECT_DESC*>(pArg);

    m_fX = pDesc->fX;
    m_fY = pDesc->fY;
    m_fSizeX = pDesc->fSizeX;
    m_fSizeY = pDesc->fSizeY;

    _uint                   iNumViewports = { 1 };
    D3D11_VIEWPORT			ViewPortDesc{};

    m_pContext->RSGetViewports(&iNumViewports, &ViewPortDesc);

    m_fViewportSizeX = ViewPortDesc.Width;
    m_fViewportSizeY = ViewPortDesc.Height;

    Update_State();

    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(m_fViewportSizeX, m_fViewportSizeY, 0.f, 1.f));

    return S_OK;
}

void CUIObject::Priority_Update(_float fTimeDelta)
{
}

void CUIObject::Update(_float fTimeDelta)
{
}

void CUIObject::Late_Update(_float fTimeDelta)
{
    Update_State();
}

HRESULT CUIObject::Render()
{
    return S_OK;
}


void CUIObject::Update_State()
{
    m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY);

    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(
        m_fX - m_fViewportSizeX * 0.5f, 
        -m_fY + m_fViewportSizeY * 0.5f, 
        0.0f, 
        1.f
    ));
}

void CUIObject::Free()
{
    __super::Free();


}
