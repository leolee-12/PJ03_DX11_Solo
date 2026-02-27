#include "BlendObject.h"
#include "GameInstance.h"

CBlendObject::CBlendObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CDynamicObject(pDevice, pContext)
{
}

CBlendObject::CBlendObject(const CBlendObject& rhs)
	: CDynamicObject(rhs)
{
}

HRESULT CBlendObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBlendObject::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	return S_OK;
}

void CBlendObject::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);
}

void CBlendObject::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CBlendObject::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CBlendObject::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	Compute_CamDistance();
}

void CBlendObject::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
}

HRESULT CBlendObject::Render()
{
	return S_OK;
}


void CBlendObject::Compute_CamDistance()
{
	_vector		vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector		vCamPosition = XMLoadFloat4(&m_pGameInstance->Get_CamPosition());

	m_fCamDistance = XMVectorGetX(XMVector3Length(vPosition - vCamPosition));
}

void CBlendObject::Free()
{
	__super::Free();
}