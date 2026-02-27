#include "Level_GamePlay.h"
#include "GameInstance.h"

#include "Camera_Free.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;
	
	if(FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;	

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{
}

HRESULT CLevel_GamePlay::Render()
{
	SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	LIGHT_DESC		LightDesc{};

	LightDesc.eType = LIGHT::DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(0.6f, 0.6f, 0.6f, 1.f);
	LightDesc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	LightDesc.eType = LIGHT::POINT;
	LightDesc.vPosition = _float4(20.0f, 5.0f, 10.f, 1.f);
	LightDesc.fRange = 10.0f;
	LightDesc.vDiffuse = _float4(1.f, 0.f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.2f, 0.2f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 0.f, 0.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	LightDesc.eType = LIGHT::POINT;
	LightDesc.vPosition = _float4(30.0f, 5.0f, 10.f, 1.f);
	LightDesc.fRange = 10.0f;
	LightDesc.vDiffuse = _float4(0.f, 1.f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(0.2f, 0.4f, 0.2f, 1.f);
	LightDesc.vSpecular = _float4(0.f, 1.f, 0.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;


	SHADOW_DESC		ShadowDesc{};
	ShadowDesc.vEye = _float3(-10.f, 20.f, -10.f);
	ShadowDesc.vAt = _float3(0.f, 0.f, 0.f);
	ShadowDesc.fFovy = XMConvertToRadians(60.0f);
	ShadowDesc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	ShadowDesc.fNear = 0.1f;
	ShadowDesc.fFar = 500.0f;

	if (FAILED(m_pGameInstance->Add_Shadow_Light(ShadowDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _tchar* pLayerTag)
{
	CCamera_Free::CAMERA_FREE_DESC			Desc{};

	Desc.vEye = _float3(0.f, 10.f, -6.f);
	Desc.vAt = _float3(0.f, 0.f, 0.f);
	Desc.fFovy = XMConvertToRadians(60.0f);
	Desc.fNear = 0.1f;
	Desc.fFar = 500.f;
	Desc.fSensor = 0.1f;
	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(120.0f);

	if (FAILED(m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Camera_Free"),
		ENUM_CLASS(LEVEL::GAMEPLAY), pLayerTag, &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _tchar* pLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Player"),
		ENUM_CLASS(LEVEL::GAMEPLAY), pLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const _tchar* pLayerTag)
{
	for (size_t i = 0; i < 10; i++)
	{
		if (FAILED(m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Monster"),
			ENUM_CLASS(LEVEL::GAMEPLAY), pLayerTag)))
			return E_FAIL;
	}

	

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const _tchar* pLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Terrain"),
		ENUM_CLASS(LEVEL::GAMEPLAY), pLayerTag)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Sky"),
		ENUM_CLASS(LEVEL::GAMEPLAY), pLayerTag)))
		return E_FAIL;

	for (size_t i = 0; i < 10; i++)
	{
		if (FAILED(m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_ForkLift"),
			ENUM_CLASS(LEVEL::GAMEPLAY), pLayerTag)))
			return E_FAIL;
	}
	

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Effect(const _tchar* pLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Snow"),
		ENUM_CLASS(LEVEL::GAMEPLAY), pLayerTag)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Particle_Explosion"),
		ENUM_CLASS(LEVEL::GAMEPLAY), pLayerTag)))
		return E_FAIL;

	for (size_t i = 0; i < 30; i++)
	{
		if (FAILED(m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Sprite"),
			ENUM_CLASS(LEVEL::GAMEPLAY), pLayerTag)))
			return E_FAIL;
	}

	return S_OK;
}

CLevel_GamePlay* CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_GamePlay* pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_GamePlay");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();

}
