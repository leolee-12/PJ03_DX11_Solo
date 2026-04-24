#include "Level_GamePlay.h"
#include "Camera_Free.h"
#include "Player_LGPE.h"

#include "UIImage.h"
#include "UIButton.h"
#include "UIProgressBar.h"
#include "UIText.h"
#include "UITween.h"
#include "UIAnimator.h"
#include "UISequence.h"

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

	auto pList = m_pGameInstance->Get_ObjectList(CURRENT_LEVEL, LAYER_UI);
	if (pList && pList->size() >= 4)
	{
		auto it = pList->begin();
		CUIImage* pImg = static_cast<CUIImage*>(*it);
		++it;
		CUIButton* pBtn = static_cast<CUIButton*>(*it);
		++it;
		CUIProgressBar* pBar = static_cast<CUIProgressBar*>(*it);
		++it;
		CUIText* pText = static_cast<CUIText*>(*it);

		m_pTestImage = pImg;
		m_pTestButton = pBtn;
		m_pTestProgressBar = pBar;
		m_pTestText = pText;

		{
			CUITween::UITWEEN_DESC tFade{};
			tFade.eTarget = UI_TWEEN_TARGET::COLOR_A;
			tFade.fStart = 0.f;
			tFade.fEnd = 1.f;
			tFade.fDuration = 0.5f;
			tFade.eEase = UI_EASE::EASE_OUT_CUBIC;

			CUITween::UITWEEN_DESC tRotate{};
			tRotate.eTarget = UI_TWEEN_TARGET::ROTATION;
			tRotate.fStart = 0.f;
			tRotate.fEnd = XM_PIDIV4;
			tRotate.fDuration = 1.f;
			tRotate.eEase = UI_EASE::EASE_IN_OUT_CUBIC;

			pImg->Get_Animator()->Register_Animation(L"fade_in", { tFade, tRotate });
		}

		{
			CUITween::UITWEEN_DESC tAlpha{};
			tAlpha.eTarget = UI_TWEEN_TARGET::COLOR_A;
			tAlpha.fStart = 0.f;
			tAlpha.fEnd = 1.f;
			tAlpha.fDuration = 0.5f;
			tAlpha.eEase = UI_EASE::EASE_OUT_CUBIC;

			CUITween::UITWEEN_DESC tSizeX{};
			tSizeX.eTarget = UI_TWEEN_TARGET::SIZE_X;
			tSizeX.fStart = 100.f;
			tSizeX.fEnd = 200.f;
			tSizeX.fDuration = 0.5f;
			tSizeX.eEase = UI_EASE::EASE_OUT_CUBIC;

			pBtn->Get_Animator()->Register_Animation(L"appear", { tAlpha, tSizeX });
		}

		{
			CUITween::UITWEEN_DESC tFill{};
			tFill.eTarget = UI_TWEEN_TARGET::FILL_AMOUNT;
			tFill.fStart = 0.f;
			tFill.fEnd = 1.f;
			tFill.fDuration = 1.f;
			tFill.eEase = UI_EASE::EASE_OUT_CUBIC;

			CUITween::UITWEEN_DESC tBackR{};
			tBackR.eTarget = UI_TWEEN_TARGET::BACK_COLOR_R;
			tBackR.fStart = 0.2f;
			tBackR.fEnd = 0.1f;
			tBackR.fDuration = 1.f;

			CUITween::UITWEEN_DESC tBackG{};
			tBackG.eTarget = UI_TWEEN_TARGET::BACK_COLOR_G;
			tBackG.fStart = 0.2f;
			tBackG.fEnd = 0.45f;
			tBackG.fDuration = 1.f;

			CUITween::UITWEEN_DESC tBackB{};
			tBackB.eTarget = UI_TWEEN_TARGET::BACK_COLOR_B;
			tBackB.fStart = 0.2f;
			tBackB.fEnd = 0.7f;
			tBackB.fDuration = 1.f;

			pBar->Get_Animator()->Register_Animation(L"fill", { tFill, tBackR, tBackG, tBackB });
		}

		{
			CUITween::UITWEEN_DESC tAlpha{};
			tAlpha.eTarget = UI_TWEEN_TARGET::COLOR_A;
			tAlpha.fStart = 0.f;  tAlpha.fEnd = 1.f;
			tAlpha.fDuration = 0.5f;
			tAlpha.eEase = UI_EASE::EASE_OUT_CUBIC;

			CUITween::UITWEEN_DESC tRot{};
			tRot.eTarget = UI_TWEEN_TARGET::ROTATION;
			tRot.fStart = -XM_PIDIV4 * 0.25f;   // -11.25° 에서
			tRot.fEnd = 0.f;                  // 수평까지 복귀
			tRot.fDuration = 0.6f;
			tRot.eEase = UI_EASE::EASE_OUT_CUBIC;
			tRot.eLoop = UI_TWEEN_LOOP::PINGPONG;

			pText->Get_Animator()->Register_Animation(L"fade_in", { tAlpha, tRot });
		}

		m_pTestSequence = CUISequence::Create(m_pDevice, m_pContext);
		if (m_pTestSequence)
		{
			m_pTestSequence->Initialize(nullptr);

			CUISequence::UISEQ_STEP step{};

			step = {};
			step.eKind = UI_SEQ_STEP_KIND::PLAY_ANIM;
			step.pTarget = pImg;
			step.strAnimName = L"fade_in";
			m_pTestSequence->Append(step);

			step = {};
			step.eKind = UI_SEQ_STEP_KIND::WAIT;
			step.fWaitSec = 0.3f;
			m_pTestSequence->Append(step);

			step = {};
			step.eKind = UI_SEQ_STEP_KIND::PLAY_ANIM;
			step.pTarget = pBtn;
			step.strAnimName = L"appear";
			m_pTestSequence->Append(step);

			step = {};
			step.eKind = UI_SEQ_STEP_KIND::PLAY_ANIM;
			step.pTarget = pBar;
			step.strAnimName = L"fill";
			m_pTestSequence->Join(step);

			step = {};
			step.eKind = UI_SEQ_STEP_KIND::PLAY_ANIM;
			step.pTarget = pText;
			step.strAnimName = L"fade_in";
			m_pTestSequence->Join(step);

			step = {};
			step.eKind = UI_SEQ_STEP_KIND::WAIT;
			step.fWaitSec = 1.0f;
			m_pTestSequence->Append(step);

			step = {};
			step.eKind = UI_SEQ_STEP_KIND::SET_VISIBLE;
			step.pTarget = pImg;
			step.bVisible = false;
			m_pTestSequence->Append(step);

			Reset_TestUIState();
		}
	}

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

	if (m_pTestSequence)
	{
		if (m_pGameInstance->Key_Down(DIK_F2))
		{
			Reset_TestUIState();
			m_pTestSequence->Play();
		}

		m_pTestSequence->Update(fTimeDelta);
	}
}

HRESULT CLevel_GamePlay::Render()
{
#ifdef _DEBUG
	SetWindowText(m_pGameInstance->Get_HWND(), TEXT("Gameplay Level"));
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
	{
		return E_FAIL;
	}

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
	{
		return E_FAIL;
	}

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
	{
		return E_FAIL;
	}

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
	tDesc.vColor = _float4(0.f, 0.f, 0.f, 0.f);

	if (FAILED(m_pGameInstance->Add_GameObject(CURRENT_LEVEL, PROTO_UI_IMAGE, CURRENT_LEVEL, strLayerTag, &tDesc)))
		return E_FAIL;

	CUIButton::UIBUTTON_DESC tBtn{};
	tBtn.fCenterX = 200.f;
	tBtn.fCenterY = 200.f;
	tBtn.fSizeX = 100.f;
	tBtn.fSizeY = 80.f;
	tBtn.iZOrder = 10;
	tBtn.bVisible = true;
	tBtn.strTextureTag = PROTO_COM_TEXTURE_TITLE_LOGO_DIFF;
	tBtn.strShaderTag = PROTO_COM_SHADER_UI;
	tBtn.strVIBufferTag = PROTO_COM_VIBUFFER_RECT;
	tBtn.iTextureLevel = ETOUI(LEVEL::GAMEPLAY);
	tBtn.iShaderLevel = ETOUI(LEVEL::GAMEPLAY);
	tBtn.iVIBufferLevel = ETOUI(LEVEL::STATIC);
	tBtn.iNormalTextureIndex = 0;
	tBtn.iHoverTextureIndex = 1;
	tBtn.iPressedTextureIndex = 2;
	tBtn.iDisabledTextureIndex = INVALID_INDEX;
	tBtn.bInteractable = true;
	tBtn.vColor = _float4(1.f, 1.f, 1.f, 0.f);

	if (FAILED(m_pGameInstance->Add_GameObject(CURRENT_LEVEL, PROTO_UI_BUTTON,
		CURRENT_LEVEL, strLayerTag, &tBtn)))
	{
		return E_FAIL;
	}

	CUIProgressBar::UIPROGRESSBAR_DESC tBar{};
	tBar.fCenterX = 640.f;
	tBar.fCenterY = 400.f;
	tBar.fSizeX = 400.f;
	tBar.fSizeY = 40.f;
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
	tBar.vBackColor = _float4(0.2f, 0.2f, 0.2f, 1.f);
	tBar.vFillColor = _float4(1.0f, 0.3f, 0.3f, 1.f);
	tBar.fFillAmount = 0.f;
	tBar.eDirection = CUIProgressBar::UI_PROGRESS_DIR::LEFT_TO_RIGHT;

	if (FAILED(m_pGameInstance->Add_GameObject(CURRENT_LEVEL, PROTO_UI_PROGRESSBAR,
		CURRENT_LEVEL, strLayerTag, &tBar)))
	{
		return E_FAIL;
	}

	CUIText::UITEXT_DESC tText{};
	tText.fCenterX = fViewWidth * 0.5f;
	tText.fCenterY = 100.f;
	tText.fSizeX = 400.f;
	tText.fSizeY = 60.f;
	tText.iZOrder = 20;
	tText.bVisible = true;
	tText.strText = L"Gameplay Test";
	tText.strFontTag = FONT_MALGUN;
	tText.vColor = _float4(1.f, 1.f, 1.f, 0.f);
	tText.eAlign = UI_TEXT_ALIGN::CENTER;

	if (FAILED(m_pGameInstance->Add_GameObject(CURRENT_LEVEL, PROTO_UI_TEXT,
		CURRENT_LEVEL, strLayerTag, &tText)))
		return E_FAIL;

	return S_OK;
}

void CLevel_GamePlay::Reset_TestUIState()
{
	if (m_pTestImage)
	{
		m_pTestImage->Set_Visible(true);
		m_pTestImage->Set_Color(_float4(0.f, 0.f, 0.f, 0.f));
		m_pTestImage->Apply_Tween_Target(UI_TWEEN_TARGET::ROTATION, 0.f);
	}

	if (m_pTestButton)
	{
		m_pTestButton->Set_Visible(true);
		m_pTestButton->Apply_Tween_Target(UI_TWEEN_TARGET::COLOR_A, 0.f);
		m_pTestButton->Apply_Tween_Target(UI_TWEEN_TARGET::SIZE_X, 100.f);
	}

	if (m_pTestProgressBar)
	{
		m_pTestProgressBar->Set_Visible(true);
		m_pTestProgressBar->Set_FillAmount(0.f);
		m_pTestProgressBar->Apply_Tween_Target(UI_TWEEN_TARGET::BACK_COLOR_R, 0.2f);
		m_pTestProgressBar->Apply_Tween_Target(UI_TWEEN_TARGET::BACK_COLOR_G, 0.2f);
		m_pTestProgressBar->Apply_Tween_Target(UI_TWEEN_TARGET::BACK_COLOR_B, 0.2f);
		m_pTestProgressBar->Apply_Tween_Target(UI_TWEEN_TARGET::BACK_COLOR_A, 1.f);
	}

	if (m_pTestText)
	{
		m_pTestText->Set_Visible(true);
		m_pTestText->Set_Color(_float4(1.f, 1.f, 1.f, 0.f));
		m_pTestText->Apply_Tween_Target(UI_TWEEN_TARGET::ROTATION, -XM_PIDIV4 * 0.25f);
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

	Safe_Release(m_pTestSequence);
}
