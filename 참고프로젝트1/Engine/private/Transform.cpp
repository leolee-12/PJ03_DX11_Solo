#include "Transform.h"

#include "Shader.h"
#include "Navigation.h"

CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent { pDevice, pContext }
{
}



void CTransform::Set_Scale(_float fSizeX, _float fSizeY, _float fSizeZ)
{
    Set_State(STATE::RIGHT, XMVector3Normalize(Get_State(STATE::RIGHT)) * fSizeX);
    Set_State(STATE::UP, XMVector3Normalize(Get_State(STATE::UP)) * fSizeY);
    Set_State(STATE::LOOK, XMVector3Normalize(Get_State(STATE::LOOK)) * fSizeZ);
}

HRESULT CTransform::Initialize_Prototype()
{
  
	return S_OK;
}

HRESULT CTransform::Initialize(void* pArg)
{


    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
    if (nullptr == pArg)
        return S_OK;
    TRANSFORM_DESC* pDesc = static_cast<TRANSFORM_DESC*>(pArg);

    m_fSpeedPerSec = pDesc->fSpeedPerSec;
    m_fRotationPerSec = pDesc->fRotationPerSec;

	return S_OK;
}

HRESULT CTransform::Bind_ShaderResource(CShader* pShader, const _char* pConstantName)
{
    return pShader->Bind_Matrix(pConstantName, &m_WorldMatrix);
}



void CTransform::Go_Straight(_float fTimeDelta, CNavigation* pNavigation)
{
    /* 현재 바라보고 있는 방향으로 나의 속도만큼 이동한다. */
    _vector     vPosition = Get_State(STATE::POSITION);
    _vector     vLook = Get_State(STATE::LOOK);    

    vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

    if(nullptr == pNavigation || 
        true == pNavigation->isMove(vPosition))
        Set_State(STATE::POSITION, vPosition);
}

void CTransform::Go_Left(_float fTimeDelta)
{
    _vector     vPosition = Get_State(STATE::POSITION);
    _vector     vRight = Get_State(STATE::RIGHT);

    vPosition -= XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

    Set_State(STATE::POSITION, vPosition);
}

void CTransform::Go_Right(_float fTimeDelta)
{
    _vector     vPosition = Get_State(STATE::POSITION);
    _vector     vRight = Get_State(STATE::RIGHT);

    vPosition += XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

    Set_State(STATE::POSITION, vPosition);
}

void CTransform::Go_Backward(_float fTimeDelta)
{    /* 현재 바라보고 있는 방향으로 나의 속도만큼 이동한다. */
    _vector     vPosition = Get_State(STATE::POSITION);
    _vector     vLook = Get_State(STATE::LOOK);

    vPosition -= XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

    Set_State(STATE::POSITION, vPosition);
}

/* 항등상태 기준으로 정해준 Radian만큼 회전한다.  */
void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
    _float3 vScaled = Get_Scaled();

    _vector vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x;
    _vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y;
    _vector vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z;

    _matrix RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

    vRight = XMVector3TransformNormal(vRight, RotationMatrix);
    vUp = XMVector3TransformNormal(vUp, RotationMatrix);
    vLook = XMVector3TransformNormal(vLook, RotationMatrix);

    Set_State(STATE::RIGHT, vRight);
    Set_State(STATE::UP, vUp);
    Set_State(STATE::LOOK, vLook);
}

void CTransform::Rotation(_float fRotX, _float fRotY, _float fRotZ)
{
    _float3 vScaled = Get_Scaled();

    _vector vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x;
    _vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y;
    _vector vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z;


    _vector     vQuaternion = XMQuaternionRotationRollPitchYaw(fRotX, fRotY, fRotZ);

    _matrix     RotationMatrix = XMMatrixRotationQuaternion(vQuaternion);

    vRight = XMVector3TransformNormal(vRight, RotationMatrix);
    vUp = XMVector3TransformNormal(vUp, RotationMatrix);
    vLook = XMVector3TransformNormal(vLook, RotationMatrix);

    Set_State(STATE::RIGHT, vRight);
    Set_State(STATE::UP, vUp);
    Set_State(STATE::LOOK, vLook);
}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
    _vector vRight = Get_State(STATE::RIGHT);
    _vector vUp = Get_State(STATE::UP);
    _vector vLook = Get_State(STATE::LOOK);

    _matrix RotationMatrix = XMMatrixRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta);

    vRight = XMVector3TransformNormal(vRight, RotationMatrix);
    vUp = XMVector3TransformNormal(vUp, RotationMatrix);
    vLook = XMVector3TransformNormal(vLook, RotationMatrix);

    Set_State(STATE::RIGHT, vRight);
    Set_State(STATE::UP, vUp);
    Set_State(STATE::LOOK, vLook);
}

void CTransform::LookAt(_fvector vFocus)
{
    _vector     vLook = vFocus - Get_State(STATE::POSITION);
    _vector     vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
    _vector     vUp = XMVector3Cross(vLook, vRight);

    _float3     vScaled = Get_Scaled();

    Set_State(STATE::RIGHT, XMVector3Normalize(vRight) * vScaled.x);
    Set_State(STATE::UP, XMVector3Normalize(vUp) * vScaled.y);
    Set_State(STATE::LOOK, XMVector3Normalize(vLook) * vScaled.z);
}

void CTransform::Chase(_fvector vDest, _float fTimeDelta, _float fLimitDistance)
{
    _vector     vPosition = Get_State(STATE::POSITION);

    _vector     vMoveDir = vDest - vPosition;

    _float      fDistance = XMVectorGetX(XMVector3Length(vMoveDir));

    if (fDistance < fLimitDistance)
        return;

    vPosition += XMVector3Normalize(vMoveDir) * m_fSpeedPerSec * fTimeDelta;

    Set_State(STATE::POSITION, vPosition);
}

CTransform* CTransform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTransform* pInstance = new CTransform(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CTransform");
        Safe_Release(pInstance);
    }

    return pInstance;
}


void CTransform::Free()
{
	__super::Free();

}
