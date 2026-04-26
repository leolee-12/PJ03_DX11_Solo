#include "Level_GamePlay.h"
#include "Camera_Free.h"
#include "Player_LGPE.h"
#include "UI_RTSequence.h"

#include "GameInstance.h"

NS_BEGIN(Game_PKM)
static constexpr _uint CURRENT_LEVEL = ETOUI(LEVEL::GAMEPLAY);
NS_END

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(LAYER_CAMERA)))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(LAYER_BACKGROUND)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(LAYER_PLAYER)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(LAYER_MONSTER)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(LAYER_EFFECT)))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(LAYER_UI)))
		return E_FAIL;

	CCamera* pCamera = static_cast<CCamera*>(*(m_pGameInstance->Get_ObjectList(ETOUI(LEVEL::GAMEPLAY), LAYER_CAMERA)->begin()));
	CPlayer_LGPE* pPlayer = static_cast<CPlayer_LGPE*>(*(m_pGameInstance->Get_ObjectList(ETOUI(LEVEL::GAMEPLAY), LAYER_PLAYER)->begin()));
	pCamera->Set_FollowTarget(pPlayer->Get_Transform());
	pCamera->Set_FollowOffset({ 0.f, 6.5f, -7.5f });
	m_pGameInstance->Set_MainCamera(pCamera);

	return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_F4))
		m_pGameInstance->Toggle_CameraFollow();

	if (m_pRuntimeUI)
		m_pRuntimeUI->Update(fTimeDelta);

	if (m_pGameInstance->Key_Down(DIK_F2) && m_pRuntimeUI)
		m_pRuntimeUI->Play();
}

HRESULT CLevel_GamePlay::Render()
{
#ifdef _DEBUG
	SetWindowText(m_pGameInstance->Get_HWND(), TEXT("게임플레이레벨입니다."));
#endif

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	LIGHT_DESC LightDesc{};

	LightDesc.eType = LIGHT::DIRECTIONAL;
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(WNameID strLayerTag)
{
	CCamera_Free::CAMERA_FREE_DESC CameraDesc = {};

	CameraDesc.vEye = _float3(0.f, 8.f, -7.f);
	CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
	CameraDesc.fFovy = XMConvertToRadians(35.f);
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 500.f;
	CameraDesc.fSpeedPerSec = 20.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(180.f);
	CameraDesc.fMouseSensor = 0.05f;

	if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_CAMERA_FREE,
		ETOUI(LEVEL::GAMEPLAY), strLayerTag, &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(WNameID strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_SKY, ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_TOWN01, ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_ROAD01, ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_SNOW,
		ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(WNameID strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_PLAYER_LGPE, ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(WNameID strLayerTag)
{
	for (size_t i = 0; i < 20; i++)
	{
		if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_MONSTER, ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Effect(WNameID strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_EXPLOSION,
		ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI(WNameID strLayerTag)
{
	m_pRuntimeUI = CUI_RTSequence::Create(
		m_pDevice, m_pContext,
		"../../DataFiles/UI/UI_Title.uiseq",
		CURRENT_LEVEL,
		strLayerTag);

	if (nullptr == m_pRuntimeUI)
		return E_FAIL;

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

	Safe_Release(m_pRuntimeUI);
}
