#include "Level_Logo.h"
#include "Level_Loading.h"
#include "BackGround.h"
#include "Effect_Star.h"

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
	tDesc.m_strTextureTag = PROTO_COM_TEXTURE_TITLE_BG;

	if (FAILED(m_pGameInstance->Add_GameObject(CURRENT_LEVEL, PROTO_OBJ_TITLE_BG, CURRENT_LEVEL, strLayerTag, &tDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_Monster(WNameID strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_UI(WNameID strLayerTag)
{
	CUISequence::UISEQUENCE_DESC SeqDesc{};
	SeqDesc.strPath = "../../DataFiles/UI/UI_Title.uiseq";
	SeqDesc.iProtoLevel = ETOUI(LEVEL::STATIC);

	CUISequence* pTitleUI = static_cast<CUISequence*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ETOUI(LEVEL::STATIC), PROTO_UI_SEQUENCE, &SeqDesc));
	if (nullptr == pTitleUI)
		return E_FAIL;

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
	StarDesc.vSpeedRange = _float2(100.f, 180.f);
	StarDesc.vEmitDir = _float2(-1.f, 0.f);
	StarDesc.fEmitSpreadAngle = XMConvertToRadians(90.f);
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

	CEffect_Star* pStarEffect1 = static_cast<CEffect_Star*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, CURRENT_LEVEL, PROTO_OBJ_EFT_STAR, &StarDesc));
	if (nullptr == pStarEffect1)
	{
		Safe_Release(pTitleUI);
		return E_FAIL;
	}

	StarDesc.fCenterX = -150.f;
	StarDesc.fCenterY = 590.f;
	StarDesc.iZOrder = 11;
	CEffect_Star* pStarEffect2 = static_cast<CEffect_Star*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, CURRENT_LEVEL, PROTO_OBJ_EFT_STAR, &StarDesc));
	if (nullptr == pStarEffect2)
	{
		Safe_Release(pTitleUI);
		Safe_Release(pStarEffect1);
		return E_FAIL;
	}

	pTitleUI->Bind_Effect("eff_star",
		[pStarEffect1, pStarEffect2](const CUISequence::UISEQ_EVENT_CONTEXT& ctx)
		{
			pStarEffect1->Play();
			pStarEffect2->Play();

			CUITween::UITWEEN_DESC tween{};
			tween.eTarget = UI_TWEEN_TARGET::POSITION_X;
			tween.fStart = -100.f;
			tween.fEnd = 2500.f;
			tween.fDuration = 4.25f;
			tween.fDelay = 0.75f;
			tween.eEase = UI_EASE::EASE_OUT_SINE;
			tween.eLoop = UI_TWEEN_LOOP::NONE;

			if (auto* pAnim = pStarEffect1->Get_Animator())
				pAnim->Play_Tween(tween);
			
			tween.fStart = -200.f;
			tween.fEnd = 2400.f;
			if (auto* pAnim = pStarEffect2->Get_Animator())
				pAnim->Play_Tween(tween);
		},
		[pStarEffect1, pStarEffect2](const CUISequence::UISEQ_EVENT_CONTEXT&)
		{
			pStarEffect1->Stop();
			pStarEffect2->Stop();
		});

	pTitleUI->Bind_BGM(
		"bgm_title",
		[pGI = m_pGameInstance](const CUISequence::UISEQ_EVENT_CONTEXT&)
		{
			pGI->Play_BGM(L"BGM/1-03. Title Screen.mp3", 0.5f);
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
				0.7f);
		});

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