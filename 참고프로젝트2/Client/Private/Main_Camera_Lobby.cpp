#include "stdafx.h"
#include "..\Public\Main_Camera_Lobby.h"

#include "GameInstance.h"

CMain_Camera_Lobby::CMain_Camera_Lobby(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext }
{

}

CMain_Camera_Lobby::CMain_Camera_Lobby(const CMain_Camera_Lobby& Prototype)
	: CCamera{ Prototype }
{

}

HRESULT CMain_Camera_Lobby::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMain_Camera_Lobby::Initialize(void* pArg)
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

	m_pTransformCom->Set_State_Position(_float3(0.f, 52.f, -108.f));
	m_pTransformCom->LookAt(XMVectorSet(0.f, 0.f, 0.f, 1.f));

	m_pLobby_Goku_Transform = static_cast<CTransform*>(m_pGameInstance->Get_GameObject(LEVEL_LOBBY, TEXT("Layer_Lobby_Goku"))->Get_Component(TEXT("Com_Transform")));

	return S_OK;
}

void CMain_Camera_Lobby::Camera_Update(_float fTimeDelta)
{
	_vector playerPosition = m_pLobby_Goku_Transform->Get_State(CTransform::STATE_POSITION);
	_vector offset = XMVectorSet(0.f, 15.f, -15.f, 0.f);
	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, playerPosition + offset);
	m_pTransformCom->LookAt(playerPosition + XMVectorSet(0.f, 0.f, 10.f, 0.f));
	
	//기본 이동 속도
	//_float fMoveSpeed = 1.f;
	//
	//// 오른쪽 버튼이 눌렸는지 확인
	//
	//if (m_pGameInstance->Mouse_Pressing(1))
	//{
	//	// Shift 키가 눌렸는지 확인하고, 눌렸다면 이동 속도를 증가
	//	if (m_pGameInstance->Key_Pressing(DIK_LSHIFT))
	//	{
	//		fMoveSpeed *= 10.f;
	//	}
	//
	//	if (m_pGameInstance->Key_Pressing(DIK_A))
	//	{
	//		m_pTransformCom->Go_Left(fTimeDelta * fMoveSpeed);
	//	}
	//
	//	if (m_pGameInstance->Key_Pressing(DIK_D))
	//	{
	//		m_pTransformCom->Go_Right(fTimeDelta * fMoveSpeed);
	//	}
	//
	//	if (m_pGameInstance->Key_Pressing(DIK_W))
	//	{
	//		m_pTransformCom->Go_Straight(fTimeDelta * fMoveSpeed);
	//	}
	//
	//	if (m_pGameInstance->Key_Pressing(DIK_S))
	//	{
	//		m_pTransformCom->Go_Backward(fTimeDelta * fMoveSpeed);
	//	}
	//
	//	if (m_pGameInstance->Key_Pressing(DIK_Q))
	//	{
	//		m_pTransformCom->Go_Down(fTimeDelta * fMoveSpeed);
	//	}
	//
	//	if (m_pGameInstance->Key_Pressing(DIK_E))
	//	{
	//		m_pTransformCom->Go_Up(fTimeDelta * fMoveSpeed);
	//	}
	//
	//	_long MouseMove = {};
	//
	//	if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMM_X))
	//	{
	//		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fMouseSensor * MouseMove * fTimeDelta);
	//	}
	//
	//	if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMM_Y))
	//	{
	//		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), m_fMouseSensor * MouseMove * fTimeDelta);
	//	}
	//}
	__super::Camera_Update(fTimeDelta);
}

void CMain_Camera_Lobby::Update(_float fTimeDelta)
{
}

void CMain_Camera_Lobby::Late_Update(_float fTimeDelta)
{
}

HRESULT CMain_Camera_Lobby::Render(_float fTimeDelta)
{
	return S_OK;
}

CMain_Camera_Lobby* CMain_Camera_Lobby::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMain_Camera_Lobby* pInstance = new CMain_Camera_Lobby(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMain_Camera_Lobby"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMain_Camera_Lobby::Clone(void* pArg)
{
	CMain_Camera_Lobby* pInstance = new CMain_Camera_Lobby(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMain_Camera_Lobby"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMain_Camera_Lobby::Free()
{
	__super::Free();

}
