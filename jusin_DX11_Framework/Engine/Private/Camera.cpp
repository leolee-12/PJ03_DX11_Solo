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

void CCamera::Set_FollowTarget(CTransform* pTarget)
{
	if (nullptr != m_pFollowTarget)
		Safe_Release(m_pFollowTarget);

	m_pFollowTarget = pTarget;
	Safe_AddRef(m_pFollowTarget);
}

HRESULT CCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera::Initialize(void* pArg)
{
	auto pDesc = static_cast<CAMERA_DESC*>(pArg);

	D3D11_VIEWPORT ViewPortDesc{};
	_uint iNumViewports = { 1 };
	m_pContext->RSGetViewports(&iNumViewports, &ViewPortDesc);
	m_fAspect = static_cast<_float>(ViewPortDesc.Width) / ViewPortDesc.Height;

	m_fNear = pDesc->fNear;	
	m_fFar = pDesc->fFar;
	m_fFovy = pDesc->fFovy;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vEye), 1.f));
	m_pTransformCom->LookAt(XMVectorSetW(XMLoadFloat3(&pDesc->vAt), 1.f));

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixPerspectiveFovLH(m_fFovy, m_fAspect, m_fNear, m_fFar));
	m_bProjDirty = true;
	Update_PipeLine();

	return S_OK;
}

void CCamera::Priority_Update(_float fTimeDelta)
{
}

void CCamera::Update(_float fTimeDelta)
{
	Update_PipeLine();
}

void CCamera::Late_Update(_float fTimeDelta)
{
	if (m_bFollow && m_pFollowTarget)
	{
		_vector vTargetPos = m_pFollowTarget->Get_State(STATE::POSITION);
		m_pTransformCom->Set_State(STATE::POSITION, vTargetPos + XMLoadFloat3(&m_vFollowOffset));
		m_pTransformCom->LookAt(vTargetPos + XMLoadFloat3(&m_vLookOffset));
	}
}

HRESULT CCamera::Render()
{
	return S_OK;
}

void CCamera::On_ViewportResize(_float2 vNewSize)
{
	m_fAspect = vNewSize.x / vNewSize.y;
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixPerspectiveFovLH(m_fFovy, m_fAspect, m_fNear, m_fFar));
	m_bProjDirty = true;
}

void CCamera::Update_PipeLine()
{
	m_pGameInstance->Set_CameraWorld(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

	if (m_bProjDirty)
	{
		m_pGameInstance->Set_Projection(XMLoadFloat4x4(&m_ProjMatrix));
		m_bProjDirty = false;
	}
}

CGameObject* CCamera::Clone(void* pArg)
{
	return nullptr;
}

void CCamera::Free()
{
	__super::Free();

	Safe_Release(m_pFollowTarget);
	//Safe_Release(m_pPipeLine);
}
