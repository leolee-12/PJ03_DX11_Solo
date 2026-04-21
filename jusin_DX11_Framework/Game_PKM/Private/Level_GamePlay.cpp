#include "Level_GamePlay.h"
#include "Camera_Free.h"
#include "Player_LGPE.h"

#include "UIImage.h"
#include "UIButton.h"
#include "UIProgressBar.h"

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
	if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_TERRAIN, ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_SKY, ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_TOWN01, ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_ROAD01, ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	//for (size_t i = 0; i < 10; i++)
	//{
	//	if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_FORKLIFT,
	//		ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
	//		return E_FAIL;
	//}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(WNameID strLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_PLAYER, ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
	//	return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_PLAYER_LGPE, ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(WNameID strLayerTag)
{
	//for (size_t i = 0; i < 20; i++)
	//{
	//	if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_MONSTER, ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
	//		return E_FAIL;
	//}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI(WNameID strLayerTag)
{
	_uint iNumViewport = { 1 };
	D3D11_VIEWPORT ViewportDesc = {};
	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);
	_float fViewWidth = ViewportDesc.Width;
	_float fViewHeight = ViewportDesc.Height;

	CUIImage::UIIMAGE_DESC tDesc{};
	tDesc.fSpeedPerSec = 30.f;
	tDesc.fRotationPerSec = 1.f;
	tDesc.fCenterX = fViewWidth * 0.5f;
	tDesc.fCenterY = fViewHeight * 0.5f;
	tDesc.fSizeX = fViewWidth;
	tDesc.fSizeY = fViewHeight;
	tDesc.iZOrder = 0;

	tDesc.bVisible = true;

	tDesc.strTextureTag = PROTO_COM_TEXTURE_TITLE_LOGO_DIFF;
	tDesc.strShaderTag = PROTO_COM_SHADER_UI;
	tDesc.strVIBufferTag = PROTO_COM_VIBUFFER_RECT;
	tDesc.iTextureLevel = ETOUI(LEVEL::GAMEPLAY);
	tDesc.iShaderLevel = ETOUI(LEVEL::GAMEPLAY);
	tDesc.iVIBufferLevel = ETOUI(LEVEL::STATIC);
	tDesc.iTextureIndex = 2;
	tDesc.vColor = g_kBlack;

	if (FAILED(m_pGameInstance->Add_GameObject(CURRENT_LEVEL, PROTO_UI_IMAGE, CURRENT_LEVEL, strLayerTag, &tDesc)))
		return E_FAIL;

	CUIButton::UIBUTTON_DESC tBtn{};
	tBtn.fCenterX = 200.f; tBtn.fCenterY = 200.f;
	tBtn.fSizeX = 200.f;   tBtn.fSizeY = 80.f;
	tBtn.iZOrder = 10;
	tBtn.bVisible = true;
	tBtn.strTextureTag = PROTO_COM_TEXTURE_TITLE_LOGO_DIFF; // 기존 멀티프레임 텍스처 재사용
	tBtn.strShaderTag = PROTO_COM_SHADER_UI;
	tBtn.strVIBufferTag = PROTO_COM_VIBUFFER_RECT;
	tBtn.iTextureLevel = ETOUI(LEVEL::GAMEPLAY);
	tBtn.iShaderLevel = ETOUI(LEVEL::GAMEPLAY);
	tBtn.iVIBufferLevel = ETOUI(LEVEL::STATIC);
	tBtn.iNormalTextureIndex = 0;
	tBtn.iHoverTextureIndex = 1;
	tBtn.iPressedTextureIndex = 2;
	tBtn.iDisabledTextureIndex = INVALID_INDEX;  // fallback → NORMAL
	tBtn.bInteractable = true;
	tBtn.vColor = g_kWhite;

	m_pGameInstance->Add_GameObject(CURRENT_LEVEL, PROTO_UI_BUTTON,
		CURRENT_LEVEL, strLayerTag, &tBtn);

	CUIProgressBar::UIPROGRESSBAR_DESC tBar{};
	tBar.fCenterX = 640.f; tBar.fCenterY = 400.f;
	tBar.fSizeX = 400.f;   tBar.fSizeY = 40.f;
	tBar.iZOrder = 5;
	tBar.bVisible = true;
	tBar.strBackTextureTag = PROTO_COM_TEXTURE_TITLE_BG_GRAD;
	tBar.iBackTextureIndex = 0;
	tBar.strFillTextureTag = PROTO_COM_TEXTURE_TITLE_LOGO_DIFF;
	tBar.iFillTextureIndex = 0;
	tBar.strShaderTag = PROTO_COM_SHADER_UI;
	tBar.strVIBufferTag = PROTO_COM_VIBUFFER_RECT;
	tBar.iShaderLevel = ETOUI(LEVEL::GAMEPLAY);
	tBar.iVIBufferLevel = ETOUI(LEVEL::STATIC);
	tBar.iBackTextureLevel = ETOUI(LEVEL::GAMEPLAY);
	tBar.iFillTextureLevel = ETOUI(LEVEL::GAMEPLAY);
	tBar.vBackColor = _float4(0.2f, 0.2f, 0.2f, 1.f);  // 어두운 배경
	tBar.vFillColor = _float4(1.0f, 0.3f, 0.3f, 1.f);  // 붉은 HP
	tBar.fFillAmount = 0.6f;
	tBar.eDirection = CUIProgressBar::UI_PROGRESS_DIR::LEFT_TO_RIGHT;

	m_pGameInstance->Add_GameObject(CURRENT_LEVEL, PROTO_UI_PROGRESSBAR,
		CURRENT_LEVEL, strLayerTag, &tBar);

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