#include "PartObject.h"

CPartObject::CPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject { pDevice, pContext }
{
}

CPartObject::CPartObject(const CPartObject& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CPartObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPartObject::Initialize(void* pArg)
{
    PART_OBJECT_DESC* pDesc = static_cast<PART_OBJECT_DESC*>(pArg);

    m_pParentMatrix = pDesc->pParentMatrix;

    XMStoreFloat4x4(&m_CombinedWorldMatrix, XMMatrixIdentity());

    if (FAILED(__super::Initialize(pArg)))
            return E_FAIL;

    return S_OK;
}

void CPartObject::Priority_Update(_float fTimeDelta)
{
}

void CPartObject::Update(_float fTimeDelta)
{
}

void CPartObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CPartObject::Render()
{
    return S_OK;
}


void CPartObject::Free()
{
    __super::Free();


}
