#include "stdafx.h"
#include "..\Public\Particle.h"

#include "RenderInstance.h"
#include "GameInstance.h"

CParticle::CParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CParticle::CParticle(const CParticle& Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CParticle::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParticle::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CParticle::Camera_Update(_float fTimeDelta)
{

}

void CParticle::Update(_float fTimeDelta)
{

}

void CParticle::Late_Update(_float fTimeDelta)
{

}

HRESULT CParticle::Render(_float fTimeDelta)
{
	return S_OK;
}

void CParticle::Set_Position(_float3 position)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(position.x, position.y, position.z, 1.f));
}

void CParticle::Set_Position(_vector position)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, position);
}


void CParticle::Set_Particle_Active(_bool isActive)
{
}


CGameObject* CParticle::Clone(void* pArg)
{
	return nullptr;
}

void CParticle::Free()
{
	__super::Free();
}
