#include "Level_Logo.h"
#include "Level_Loading.h"
#include "BackGround.h"
#include "Effect_Star.h"
#include "Battle_Data.h"
#include "Monster.h"
#include "Camera_Free.h"

#include "UITween.h"
#include "UISequence.h"
#include "GameInstance.h"

NS_BEGIN(Game_PKM)
static constexpr _uint CURRENT_LEVEL = ETOUI(LEVEL::LOGO);
NS_END

CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Logo::Initialize()
{
	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(LAYER_CAMERA)))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(LAYER_BACKGROUND)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(LAYER_MONSTER)))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(LAYER_UI)))
		return E_FAIL;

	return S_OK;
}

void CLevel_Logo::Update(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_F2))
		m_pGameInstance->Toggle_CameraFollow();

	if (m_pGameInstance->Key_Down(DIK_F3))
		m_pGameInstance->Toggle_Debug();

	if (m_pGameInstance->Key_Down(DIK_F8))
	{
		if (SUCCEEDED(m_pGameInstance->Change_Level(ETOI(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::GAMEPLAY))))
			return;
	}
}

HRESULT CLevel_Logo::Render()
{
#ifdef _DEBUG
	SetWindowText(m_pGameInstance->Get_HWND(), TEXT("로고레벨입니다."));
#endif

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(WNameID strLayerTag)
{
	CBackGround::BACKGROUND_DESC tDesc{};
	tDesc.m_strTextureTag = PROTO_COM_TEX_TITLE_BG;

	if (FAILED(m_pGameInstance->Add_GameObject(CURRENT_LEVEL, PROTO_OBJ_TITLE_BG, CURRENT_LEVEL, strLayerTag, &tDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Lights()
{
	m_pGameInstance->Clear_Lights();

	LIGHT_DESC LightDesc{};

	LightDesc.eType = LIGHT::DIRECTIONAL;
	LightDesc.vDiffuse = _float4(0.9f, 0.9f, 0.9f, 1.f);   // GamePlay와 동일 중성 화이트
	LightDesc.vAmbient = _float4(0.70f, 0.70f, 0.70f, 1.f);   // GamePlay와 동일 중성 강화
	LightDesc.vSpecular = _float4(0.3f, 0.3f, 0.3f, 1.f);     // 변화 없음
	LightDesc.vDirection = _float4(0.2f, -0.7f, 0.7f, 0.f);   // Logo 고유 방향 유지

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	LightDesc.eType = LIGHT::POINT;
	LightDesc.vDiffuse = _float4(0.6f, 0.6f, 0.6f, 1.f);
	LightDesc.vAmbient = _float4(0.0f, 0.0f, 0.0f, 1.f);
	LightDesc.vSpecular = _float4(0.0f, 0.0f, 0.0f, 1.f);
	LightDesc.vPosition = _float4(1.f, 0.2f, -3.f, 1.f);
	LightDesc.fRange = 10.f;
	
	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	LightDesc.eType = LIGHT::POINT;
	LightDesc.vDiffuse = _float4(0.6f, 0.6f, 0.6f, 1.f);
	LightDesc.vAmbient = _float4(0.0f, 0.0f, 0.0f, 1.f);
	LightDesc.vSpecular = _float4(0.0f, 0.0f, 0.0f, 1.f);
	LightDesc.vPosition = _float4(0.f, 0.2f, -3.f, 1.f);
	LightDesc.fRange = 10.f;

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	LightDesc.eType = LIGHT::POINT;
	LightDesc.vDiffuse = _float4(0.6f, 0.6f, 0.6f, 1.f);
	LightDesc.vAmbient = _float4(0.0f, 0.0f, 0.0f, 1.f);
	LightDesc.vSpecular = _float4(0.0f, 0.0f, 0.0f, 1.f);
	LightDesc.vPosition = _float4(0.25f, 0.2f, -3.f, 1.f);
	LightDesc.fRange = 10.f;

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	LightDesc.eType = LIGHT::POINT;
	LightDesc.vDiffuse = _float4(0.6f, 0.6f, 0.5f, 1.f);
	LightDesc.vAmbient = _float4(0.0f, 0.0f, 0.0f, 1.f);
	LightDesc.vSpecular = _float4(0.0f, 0.0f, 0.0f, 1.f);
	LightDesc.vPosition = _float4(0.75f, 0.2f, -3.f, 1.f);
	LightDesc.fRange = 10.f;

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_Camera(WNameID strLayerTag)
{
	CCamera_Free::CAMERA_FREE_DESC CameraDesc = {};

	CameraDesc.vEye = _float3(0.f, 0.f, -5.f);
	CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
	CameraDesc.fFovy = XMConvertToRadians(35.f);
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 500.f;
	CameraDesc.fSpeedPerSec = 20.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(180.f);
	CameraDesc.fMouseSensor = 0.03f;

	if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::STATIC), PROTO_OBJ_CAMERA_FREE,
		CURRENT_LEVEL, strLayerTag, &CameraDesc)))
		return E_FAIL;

	const list<CGameObject*>* pList =
		m_pGameInstance->Get_ObjectList(CURRENT_LEVEL, strLayerTag);

	if (nullptr == pList || pList->empty())
		return E_FAIL;

	CCamera* pCamera = dynamic_cast<CCamera*>(pList->back());
	if (nullptr == pCamera)
		return E_FAIL;

	m_pGameInstance->Set_MainCamera(pCamera);

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_Monster(WNameID strLayerTag)
{
	CMonster::MONSTER_DESC MonsterDesc{};
	MonsterDesc.iComponentLevel = ETOUI(LEVEL::STATIC);
	MonsterDesc.strShaderProtoTag = PROTO_COM_SHADER_POKEMON;
	MonsterDesc.strModelProtoTag = PROTO_COM_MODEL_PM0025_00;
	MonsterDesc.pRenderMappingPath = "../../Resources/Models/pkm/PM0025_00/pm0025_00_mapping.json";
	MonsterDesc.fScale = 1.f;

	if (FAILED(m_pGameInstance->Add_GameObject(
		CURRENT_LEVEL, PROTO_OBJ_LOGO_MONSTER,
		CURRENT_LEVEL, strLayerTag,
		&MonsterDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_UI(WNameID strLayerTag)
{
	CEffect_Star::EFFECT_STAR_DESC StarDesc{};
	StarDesc.fCenterX = -100.f;
	StarDesc.fCenterY = 580.f;
	StarDesc.fSizeX = 1.5f;
	StarDesc.fSizeY = 1.5f;
	StarDesc.iZOrder = 9;
	StarDesc.bVisible = false;

	StarDesc.iNumParticles = 16;
	StarDesc.vSpawnRange = _float2(5.f, 10.f);
	StarDesc.vSizeRange = _float2(64.f, 128.f);
	StarDesc.vSpeedRange = _float2(180.f, 300.f);
	StarDesc.vEmitDir = _float2(-1.f, 0.f);
	StarDesc.fEmitSpreadAngle = XMConvertToRadians(120.f);
	StarDesc.vLifeRange = _float2(0.5f, 2.5f);
	StarDesc.vRotationSpeedRange = _float2(-1.f, 1.f);
	StarDesc.vMaskRotationSpeedRange = _float2(-0.5f, 0.5f);
	StarDesc.fMaskStrength = 0.5f;
	StarDesc.vColor = _float4(0.835f, 0.741f, 0.310f, 1.f);
	StarDesc.bStartActive = false;
	StarDesc.bLoop = true;
	StarDesc.iStarTextureIndex = 0;
	StarDesc.iMaskTextureIndex = 1;
	StarDesc.iDiamondTextureIndex = 2;
	StarDesc.eMaskSampleMode = TEXTURE_SAMPLE_MODE::BI_VERTICAL;
	StarDesc.eSubSampleMode = TEXTURE_SAMPLE_MODE::QUAD;

	CUISequence* pTitleUI = Create_TitleSequence();

	CEffect_Star* pStarEffect1 = Create_StarEffect(&StarDesc);

	StarDesc.fCenterX = -150.f;
	StarDesc.fCenterY = 590.f;
	StarDesc.iZOrder = 11;
	CEffect_Star* pStarEffect2 = Create_StarEffect(&StarDesc);

	if (nullptr == pTitleUI || nullptr == pStarEffect1 || nullptr == pStarEffect2)
	{
		Safe_Release(pTitleUI);
		Safe_Release(pStarEffect1);
		Safe_Release(pStarEffect2);
		return E_FAIL;
	}

	vector<CEffect_Star*> Effects{ pStarEffect1, pStarEffect2 };
	Bind_TitleSlots(pTitleUI, Effects);

	if (FAILED(m_pGameInstance->Add_GameObject_Ex(CURRENT_LEVEL, strLayerTag, pTitleUI)))
	{
		Safe_Release(pTitleUI);
		Safe_Release(pStarEffect1);
		Safe_Release(pStarEffect2);
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_GameObject_Ex(CURRENT_LEVEL, strLayerTag, pStarEffect1)))
	{
		Safe_Release(pStarEffect1);
		Safe_Release(pStarEffect2);
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_GameObject_Ex(CURRENT_LEVEL, strLayerTag, pStarEffect2)))
	{
		Safe_Release(pStarEffect2);
		return E_FAIL;
	}

	pTitleUI->Play();

	return S_OK;
}

CUISequence* CLevel_Logo::Create_TitleSequence()
{
	CUISequence::UISEQUENCE_DESC SeqDesc{};
	SeqDesc.strPath = "../../DataFiles/UI/UI_Title.uiseq";
	SeqDesc.iProtoLevel = ETOUI(LEVEL::STATIC);

	return static_cast<CUISequence*>(m_pGameInstance->Clone_Prototype(
		PROTOTYPE::GAMEOBJECT, ETOUI(LEVEL::STATIC), PROTO_UI_SEQUENCE, &SeqDesc));
}

CEffect_Star* CLevel_Logo::Create_StarEffect(void* pStarDesc)
{
	return static_cast<CEffect_Star*>(m_pGameInstance->Clone_Prototype(
		PROTOTYPE::GAMEOBJECT, CURRENT_LEVEL, PROTO_OBJ_EFT_STAR, pStarDesc));
}

void CLevel_Logo::Bind_TitleSlots(CUISequence* pTitleUI, vector<CEffect_Star*>& Effects)
{
	pTitleUI->Bind_Effect("eff_star",
		[Effects](const CUISequence::UISEQ_EVENT_CONTEXT& ctx)
		{
			CUITween::UITWEEN_DESC tween{};
			tween.eTarget = UI_TWEEN_TARGET::POSITION_X;
			tween.fStart = -100.f;
			tween.fEnd = 2600.f;
			tween.fDuration = 4.25f;
			tween.fDelay = 0.75f;
			tween.eEase = UI_EASE::EASE_OUT_SINE;
			tween.eLoop = UI_TWEEN_LOOP::NONE;

			for (size_t i = 0; i < Effects.size(); ++i)
			{
				Effects[i]->Play();
				tween.fStart -= 100.f * i;
				tween.fEnd -= 50.f * i;

				if (auto* pAnim = Effects[i]->Get_Animator())
					pAnim->Play_Tween(tween);
			}
		},
		[Effects](const CUISequence::UISEQ_EVENT_CONTEXT&)
		{
			for (auto* pEffect : Effects)
				pEffect->Stop();
		});

	pTitleUI->Bind_BGM(
		"bgm_title",
		[pGI = m_pGameInstance](const CUISequence::UISEQ_EVENT_CONTEXT&)
		{
			pGI->Play_BGM(L"BGM/1-03. Title Screen.mp3", 0.3f);
		},
		[pGI = m_pGameInstance](const CUISequence::UISEQ_EVENT_CONTEXT&)
		{
			pGI->Stop_Sound(CHANNELID::BGM);
		});

	pTitleUI->Bind_SFX(
		"voice_pika",
		[pGI = m_pGameInstance](const CUISequence::UISEQ_EVENT_CONTEXT&)
		{
			pGI->Play(
				L"SFX/025 - Pikachu (01).wav",
				CHANNELID::SFX,
				0.6f);
		});
}

CLevel_Logo* CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Logo* pInstance = new CLevel_Logo(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Logo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();
}