#include "stdafx.h"
#include "..\Public\Shadow_Camera.h"

#include "GameInstance.h"
#include "Main_Camera.h"
#include "Virtual_Camera.h"
#include "Transform.h"

CShadow_Camera::CShadow_Camera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext }
{

}

CShadow_Camera::CShadow_Camera(const CShadow_Camera& Prototype)
	: CCamera{ Prototype }
{

}

HRESULT CShadow_Camera::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CShadow_Camera::Initialize(void* pArg)
{

	CAMERA_DESC Desc{};

	Desc.fSpeedPerSec = 1.f;

	Desc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	m_vEye = _float3(0.f, 10.f, 50.f);
	m_vAt = _float3(0.f, 0.f, 0.f);
	m_fMouseSensor = 0.1f;
	m_fFovy = XMConvertToRadians(40.0f);
	m_fNear = 0.1f;
	m_fFar = 10000.f;
	m_fViewportWidth = 1920.f;
	m_fViewportHeight = 1080.f;

	m_pTransformCom->Set_State_Position(_float3(0.f, 5.f,10.f));
	m_pTransformCom->LookAt(XMVectorSet(0.f, 0.f, 0.f, 1.f));
	//m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));

	return S_OK;
}

void CShadow_Camera::Camera_Update(_float fTimeDelta)
{
	_vector vCamPos = static_cast<CTransform*>(
		static_cast<CVirtual_Camera*>(
			static_cast<CMain_Camera*>(*(m_pGameInstance->Get_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")).begin()))
			->m_vecVirtualCamera[static_cast<CMain_Camera*>(*(m_pGameInstance->Get_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")).begin()))
			->Get_Virtual_Camera()])->Get_Component(TEXT("Com_Transform")))
		->Get_State(CTransform::STATE_POSITION);
	_vector vCamLook = vCamPos;
	vCamPos = XMVectorSetZ(vCamPos, XMVectorGetZ(vCamPos) * -2);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCamPos); // ·è¾Ü Á¶Á¤ÇÊ¿ä
	vCamLook = XMVectorSetY(vCamLook, 0.f);
	m_pTransformCom->LookAt(vCamLook);

	m_pGameInstance->Set_ShadowTransform(CPipeLine::D3DTS_VIEW, m_pTransformCom->Get_WorldMatrix_Inverse());

	m_pGameInstance->Set_ShadowTransform(CPipeLine::D3DTS_PROJ, XMMatrixPerspectiveFovLH(m_fFovy, m_fViewportWidth / m_fViewportHeight, m_fNear, m_fFar));

}

void CShadow_Camera::Player_Update(_float fTimeDelta)
{
	
}

void CShadow_Camera::Update(_float fTimeDelta)
{

}

void CShadow_Camera::Late_Update(_float fTimeDelta)
{
}

HRESULT CShadow_Camera::Render(_float fTimeDelta)
{
	return S_OK;
}

CShadow_Camera* CShadow_Camera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CShadow_Camera* pInstance = new CShadow_Camera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CShadow_Camera"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CShadow_Camera::Clone(void* pArg)
{
	CShadow_Camera* pInstance = new CShadow_Camera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CShadow_Camera"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShadow_Camera::Free()
{
	__super::Free();

}
