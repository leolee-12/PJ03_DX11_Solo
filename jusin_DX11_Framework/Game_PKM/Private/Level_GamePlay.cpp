#include "Level_GamePlay.h"
#include "Camera_Free.h"
#include "Player_LGPE.h"
#include "Effect_Star.h"
#include "UIButton_Glow.h"
#include "UIButton_Layered.h"
#include "UIButton_Group.h"

#include "GameInstance.h"
#include "UISequence.h"

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

	if (FAILED(Ready_Test_UIButtons(LAYER_UI)))
		return E_FAIL;

	CCamera* pCamera = static_cast<CCamera*>(*(m_pGameInstance->Get_ObjectList(ETOUI(LEVEL::GAMEPLAY), LAYER_CAMERA)->begin()));
	CPlayer_LGPE* pPlayer = static_cast<CPlayer_LGPE*>(*(m_pGameInstance->Get_ObjectList(ETOUI(LEVEL::GAMEPLAY), LAYER_PLAYER)->begin()));
	pCamera->Set_FollowTarget(pPlayer->Get_Transform());
	pCamera->Set_FollowOffset({ 0.f, 6.5f, -7.5f });
	m_pGameInstance->Set_MainCamera(pCamera);

	m_pGameInstance->Play_BGM(L"BGM/1-04. Pallet Town Theme.mp3", 0.5f);

	return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_F2))
		m_pGameInstance->Toggle_CameraFollow();

	if (m_pGameInstance->Key_Down(DIK_F3))
		m_pGameInstance->Toggle_Debug();

	if (m_pGameInstance->Key_Down(DIK_F4) && m_pRuntimeUI)
		m_pRuntimeUI->Play();

	Update_Test_UIButtons(fTimeDelta);
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
	LIGHT_DESC      LightDesc{};

	LightDesc.eType = LIGHT::DIRECTIONAL;
	LightDesc.vDiffuse = _float4(0.9f, 0.9f, 0.9f, 1.f);
	LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.2f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	LightDesc.eType = LIGHT::POINT;
	LightDesc.vDiffuse = _float4(1.f, 0.f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(0.05f, 0.f, 0.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 0.1f, 0.1f, 1.f);
	LightDesc.vPosition = _float4(10.f, 5.f, 10.f, 1.f);
	LightDesc.fRange = 15.f;

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	LightDesc.eType = LIGHT::POINT;
	LightDesc.vDiffuse = _float4(0.f, 1.f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(0.f, 0.05f, 0.f, 1.f);
	LightDesc.vSpecular = _float4(0.1f, 1.f, 0.1f, 1.f);
	LightDesc.vPosition = _float4(25.f, 5.f, 10.f, 1.f);
	LightDesc.fRange = 15.f;

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
	CameraDesc.fMouseSensor = 0.03f;

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

	//if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_SNOW,
	//	ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
	//	return E_FAIL;

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
	CUISequence::UISEQUENCE_DESC tDesc{};
	tDesc.strPath = "../../DataFiles/UI/UI_Get.uiseq";
	tDesc.iProtoLevel = ETOUI(LEVEL::STATIC);
	
	CUISequence* pSeq = static_cast<CUISequence*>(m_pGameInstance->Clone_Prototype(
		PROTOTYPE::GAMEOBJECT, ETOUI(LEVEL::STATIC), PROTO_UI_SEQUENCE, &tDesc));
	if (nullptr == pSeq)
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_GameObject_Ex(CURRENT_LEVEL, strLayerTag, pSeq)))
	{
		Safe_Release(pSeq);
		return E_FAIL;
	}
	
	m_pRuntimeUI = pSeq;  // weak

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Test_UIButtons(WNameID strLayerTag)
{
	m_pTestButtonGroup = CUIButton_Group::Create();
	if (nullptr == m_pTestButtonGroup)
		return E_FAIL;

	if (FAILED(m_pTestButtonGroup->Initialize_Linear(true)))
		return E_FAIL;

	CUIButton_Glow::GLOWBUTTON_DESC GlowDesc{};
	GlowDesc.fCenterX = 360.f;
	GlowDesc.fCenterY = 160.f;
	GlowDesc.fSizeX = 260.f;
	GlowDesc.fSizeY = 90.f;
	GlowDesc.iZOrder = 50;
	GlowDesc.bVisible = true;

	GlowDesc.strShaderTag = PROTO_COM_SHADER_UIBUTTON_GLOW;
	GlowDesc.strVIBufferTag = PROTO_COM_VIBUFFER_RECT;
	GlowDesc.strTextureTag = PROTO_COM_TEX_GET_BUTTON;
	GlowDesc.strGlowTextureTag = PROTO_COM_TEX_GET_BUTTON;

	GlowDesc.iShaderLevel = ETOUI(LEVEL::STATIC);
	GlowDesc.iVIBufferLevel = ETOUI(LEVEL::STATIC);
	GlowDesc.iTextureLevel = ETOUI(LEVEL::GAMEPLAY);
	GlowDesc.iGlowTextureLevel = ETOUI(LEVEL::GAMEPLAY);

	GlowDesc.iNormalTextureIndex = 0;
	GlowDesc.iHoverTextureIndex = 1;
	GlowDesc.iPressedTextureIndex = 2;
	GlowDesc.iDisabledTextureIndex = 0;
	GlowDesc.iGlowTextureIndex = 2;

	GlowDesc.vColor = _float4(1.f, 1.f, 1.f, 1.f);
	GlowDesc.fGlowPulseSpeed = 6.f;
	GlowDesc.fGlowFadeSpeed = 8.f;

	auto* pGlow = static_cast<CUIButton_Glow*>(m_pGameInstance->Clone_Prototype(
		PROTOTYPE::GAMEOBJECT, ETOUI(LEVEL::STATIC), PROTO_UIBUTTON_GLOW, &GlowDesc));

	if (nullptr == pGlow)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_Ex(CURRENT_LEVEL, strLayerTag, pGlow)))
		return E_FAIL;

	m_TestButtons.push_back(pGlow);
	m_pTestButtonGroup->Add_Button(pGlow);

	CUIButton_Layered::LAYEREDBUTTON_DESC LayeredDesc{};
	LayeredDesc.fCenterX = 360.f;
	LayeredDesc.fCenterY = 280.f;
	LayeredDesc.fSizeX = 300.f;
	LayeredDesc.fSizeY = 90.f;
	LayeredDesc.iZOrder = 51;
	LayeredDesc.bVisible = true;

	LayeredDesc.strShaderTag = PROTO_COM_SHADER_UIBUTTON_LAYERED;
	LayeredDesc.strVIBufferTag = PROTO_COM_VIBUFFER_RECT;
	LayeredDesc.strTextureTag = PROTO_COM_TEX_GET_LINE_BACK;
	LayeredDesc.strLineTextureTag = PROTO_COM_TEX_GET_LINE_FILL;
	LayeredDesc.strGlowTextureTag = PROTO_COM_TEX_GET_BUTTON;

	LayeredDesc.iShaderLevel = ETOUI(LEVEL::STATIC);
	LayeredDesc.iVIBufferLevel = ETOUI(LEVEL::STATIC);
	LayeredDesc.iTextureLevel = ETOUI(LEVEL::GAMEPLAY);
	LayeredDesc.iLineTextureLevel = ETOUI(LEVEL::GAMEPLAY);
	LayeredDesc.iGlowTextureLevel = ETOUI(LEVEL::GAMEPLAY);

	LayeredDesc.iNormalTextureIndex = 0;
	LayeredDesc.iHoverTextureIndex = 0;
	LayeredDesc.iPressedTextureIndex = 0;
	LayeredDesc.iDisabledTextureIndex = 0;
	LayeredDesc.iLineTextureIndex = 0;
	LayeredDesc.iGlowTextureIndex = 2;

	LayeredDesc.vColorBG_Normal = _float4(0.15f, 0.25f, 0.95f, 1.f);
	LayeredDesc.vColorLine_Normal = _float4(1.f, 1.f, 1.f, 1.f);
	LayeredDesc.vColorBG_Hover = _float4(1.f, 1.f, 1.f, 1.f);
	LayeredDesc.vColorLine_Hover = _float4(0.15f, 0.25f, 0.95f, 1.f);

	LayeredDesc.bUseGlow = true;
	LayeredDesc.bUseMirrorUV = false;
	LayeredDesc.fGlowPulseSpeed = 6.f;
	LayeredDesc.fGlowFadeSpeed = 8.f;

	auto* pLayered = static_cast<CUIButton_Layered*>(m_pGameInstance->Clone_Prototype(
		PROTOTYPE::GAMEOBJECT, ETOUI(LEVEL::STATIC), PROTO_UIBUTTON_LAYERED, &LayeredDesc));

	if (nullptr == pLayered)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_Ex(CURRENT_LEVEL, strLayerTag, pLayered)))
		return E_FAIL;

	m_TestButtons.push_back(pLayered);
	m_pTestButtonGroup->Add_Button(pLayered);

	return S_OK;
}

void CLevel_GamePlay::Update_Test_UIButtons(_float fTimeDelta)
{
	if (nullptr == m_pTestButtonGroup)
		return;

	m_pTestButtonGroup->Update(fTimeDelta);

	if (m_pGameInstance->Key_Down(DIK_F5))
	{
		++m_iTestStateStep;

		const CUIButton::UI_BUTTON_STATE eState =
			(0 == m_iTestStateStep % 3) ? CUIButton::UI_BUTTON_STATE::NORMAL :
			(1 == m_iTestStateStep % 3) ? CUIButton::UI_BUTTON_STATE::HOVER :
			CUIButton::UI_BUTTON_STATE::PRESSED;

		for (auto* pButton : m_TestButtons)
		{
			if (nullptr != pButton)
				pButton->Set_State(eState);
		}
	}

	if (m_pTestButtonGroup->Was_Activated_This_Frame())
	{
		const _int iIndex = m_pTestButtonGroup->Get_Activated_Index();
		wstring strMsg = L"UIButton test activated index: " + to_wstring(iIndex);
		OutputDebugStringW(strMsg.c_str());
		OutputDebugStringW(L"\n");
	}

	if (m_pTestButtonGroup->Was_Cancelled_This_Frame())
	{
		OutputDebugStringW(L"UIButton test cancelled\n");
	}
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

	m_pRuntimeUI = nullptr;
	Safe_Release(m_pTestButtonGroup);
	m_TestButtons.clear();
}
