#include "GameObjectComp.h"

#include "GameInstance.h"

CGameObjectComp::CGameObjectComp(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : Base(pDevice, pContext)
{
}

CGameObjectComp::CGameObjectComp(const CGameObjectComp& rhs)
    : Base(rhs)
{
}

HRESULT CGameObjectComp::Initialize_Prototype()
{
    return S_OK;
}


void CGameObjectComp::Free()
{
    __super::Free();

}
