#include "stdafx.h"
#include "..\Public\CharaSelect_Camera.h"

#include "GameInstance.h"

CCharaSelect_Camera::CCharaSelect_Camera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext }
{

}

CCharaSelect_Camera::CCharaSelect_Camera(const CCharaSelect_Camera& Prototype)
	: CCamera{ Prototype }
{

}

HRESULT CCharaSelect_Camera::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCharaSelect_Camera::Initialize(void* pArg)
{
	CAMERA_DESC Desc{};

	Desc.fFovy = XMConvertToRadians(60.0f);
	Desc.fNear = 0.1f;
	Desc.fFar = 1000.f;
	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);

	m_fMouseSensor = 0.1f;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	m_pTransformCom->Set_State_Position(_float3(0.f, 0.f, 0.f));
	m_pTransformCom->LookAt(XMVectorSet(0.f, 0.f, 1.f, 0.f));

	
	return S_OK;
}

void CCharaSelect_Camera::Camera_Update(_float fTimeDelta)
{

	//기본 이동 속도
	_float fMoveSpeed = 1.f;
	
	// 오른쪽 버튼이 눌렸는지 확인
	if (m_pGameInstance->Mouse_Pressing(1))
	{
		// Shift 키가 눌렸는지 확인하고, 눌렸다면 이동 속도를 증가
		if (m_pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			fMoveSpeed *= 10.f;
		}
	
		if (m_pGameInstance->Key_Pressing(DIK_A))
		{
			m_pTransformCom->Go_Left(fTimeDelta * fMoveSpeed);
		}
	
		if (m_pGameInstance->Key_Pressing(DIK_D))
		{
			m_pTransformCom->Go_Right(fTimeDelta * fMoveSpeed);
		}
	
		if (m_pGameInstance->Key_Pressing(DIK_W))
		{
			m_pTransformCom->Go_Straight(fTimeDelta * fMoveSpeed);
		}
	
		if (m_pGameInstance->Key_Pressing(DIK_S))
		{
			m_pTransformCom->Go_Backward(fTimeDelta * fMoveSpeed);
		}
	
		if (m_pGameInstance->Key_Pressing(DIK_Q))
		{
			m_pTransformCom->Go_Down(fTimeDelta * fMoveSpeed);
		}
	
		if (m_pGameInstance->Key_Pressing(DIK_E))
		{
			m_pTransformCom->Go_Up(fTimeDelta * fMoveSpeed);
		}
	
		_long MouseMove = {};
	
		if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMM_X))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fMouseSensor * MouseMove * fTimeDelta);
		}
	
		if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMM_Y))
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), m_fMouseSensor * MouseMove * fTimeDelta);
		}
	}
	
	__super::Camera_Update(fTimeDelta);
}

void CCharaSelect_Camera::Update(_float fTimeDelta)
{
}

void CCharaSelect_Camera::Late_Update(_float fTimeDelta)
{
}

HRESULT CCharaSelect_Camera::Render(_float fTimeDelta)
{
	return S_OK;
}

CCharaSelect_Camera* CCharaSelect_Camera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCharaSelect_Camera* pInstance = new CCharaSelect_Camera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CCharaSelect_Camera"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCharaSelect_Camera::Clone(void* pArg)
{
	CCharaSelect_Camera* pInstance = new CCharaSelect_Camera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CCharaSelect_Camera"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCharaSelect_Camera::Free()
{
	__super::Free();

}
