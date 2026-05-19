#include "Game_API.h"
#include "Level_Loading.h"
#include "SharedTexture_Manager.h"
#include "UIButton_Glow.h"
#include "UIButton_Layered.h"
#include "UIController.h"
#include "UIController_Hub.h"
#include "PokemonData_Manager.h"
#include "TrainerData_Manager.h"
#include "Player_Status.h"
#include "Game_PresetTable.h"
#include "RenderRule_Manager.h"
#include "Spawn_Manager.h"
#include "Effect_Manager.h"

#include "GameInstance.h"
#include "UISequence.h"
#include "UIImage.h"
#include "UIText.h"
#include "UIProgressBar.h"

NS_BEGIN(Game_PKM)

#pragma region 공용 로직
HRESULT Start_Level(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eStartLevelID)
{
	CLevel* pPreLevel = CLevel_Loading::Create(pDevice, pContext, eStartLevelID);

	if (nullptr == pPreLevel)
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Change_Level(ETOI(LEVEL::LOADING), pPreLevel)))
		return E_FAIL;

	return S_OK;
}
HRESULT Ready_PersistentObjects(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	auto* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(pGameInstance->Add_Prototype(ETOUI(LEVEL::STATIC), PROTO_OBJ_PLAYER_STATE,
		CPlayer_Status::Create(pDevice, pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(ETOUI(LEVEL::STATIC), PROTO_OBJ_PLAYER_STATE,
		ETOUI(LEVEL::STATIC), LAYER_PERSISTENT)))
		return E_FAIL;

	return S_OK;
}

HRESULT Ready_StaticTables()
{
	auto* pPokemonDataManager = CPokemonData_Manager::GetInstance();
	if (nullptr == pPokemonDataManager)
		return E_FAIL;

	if (FAILED(pPokemonDataManager->Initialize()))
		return E_FAIL;

	auto* pTrainerDataManager = CTrainerData_Manager::GetInstance();
	if (nullptr == pTrainerDataManager)
		return E_FAIL;

	if (FAILED(pTrainerDataManager->Initialize()))
		return E_FAIL;

	auto* pRenderRuleManager = CRenderRule_Manager::GetInstance();
	if (nullptr == pRenderRuleManager)
		return E_FAIL;

	if (FAILED(pRenderRuleManager->Initialize()))
		return E_FAIL;

	return S_OK;
}

void Cleanup_StaticTables()
{
	CEffect_Manager::DestroyInstance();
	CSpawn_Manager::DestroyInstance();
	CRenderRule_Manager::DestroyInstance();
	CTrainerData_Manager::DestroyInstance();
	CPokemonData_Manager::DestroyInstance();
}
#pragma endregion

#pragma region Resources
HRESULT Ready_Prototypes_For_Static(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGameInstance* m_pGameInstance = CGameInstance::GetInstance();

	/* Prototype_Component_Texture_Dummy_Black */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_TEX_DUMMY_BLACK,
		CTexture::Create(pDevice, pContext, TEXT("../../Resources/dummy/dummy_black.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Dummy_Magenta */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_TEX_DUMMY_MAGENTA,
		CTexture::Create(pDevice, pContext, TEXT("../../Resources/dummy/dummy_magenta.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Dummy_White */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_TEX_DUMMY_WHITE,
		CTexture::Create(pDevice, pContext, TEXT("../../Resources/dummy/dummy_white.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_VIBUFFER_RECT,
		CVIBuffer_Rect::Create(pDevice, pContext))))
		return E_FAIL;

	/* Prototype_Component_Shader_VtxTex */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_SHADER_VTXTEX,
		CShader::Create(pDevice, pContext, TEXT("../../ShaderFiles/Shader_VtxTex.hlsl"), VTXTEX::Elements, VTXTEX::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_Shader_UI */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_SHADER_UI,
		CShader::Create(pDevice, pContext, TEXT("../../ShaderFiles/Shader_UI.hlsl"), VTXTEX::Elements, VTXTEX::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_Shader_UI_Button_Glow */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_SHADER_UIBUTTON_GLOW,
		CShader::Create(pDevice, pContext, TEXT("../../ShaderFiles/Shader_UIButton_Glow.hlsl"),
			VTXTEX::Elements, VTXTEX::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_Shader_UI_Button_Layered */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_SHADER_UIBUTTON_LAYERED,
		CShader::Create(pDevice, pContext, TEXT("../../ShaderFiles/Shader_UIButton_Layered.hlsl"),
			VTXTEX::Elements, VTXTEX::iNumElements))))
		return E_FAIL;

	/* UI Objects */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::STATIC), PROTO_UI_CONTAINER,
		CUIContainer::Create(pDevice, pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::STATIC), PROTO_UI_SEQUENCE,
		CUISequence::Create(pDevice, pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::STATIC), PROTO_UI_IMAGE,
		CUIImage::Create(pDevice, pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::STATIC), PROTO_UI_TEXT,
		CUIText::Create(pDevice, pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::STATIC), PROTO_UI_BUTTON,
		CUIButton::Create(pDevice, pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::STATIC), PROTO_UI_PROGRESSBAR,
		CUIProgressBar::Create(pDevice, pContext))))
		return E_FAIL;

	/* Prototype_Button_Glow_Menu_Partner */
	auto tGlowDesc = Get_GlowButtonPreset(GLOW_BUTTON_PRESET::MENU_PARTNER);
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::STATIC), PROTO_BTN_GLOW_MENU_PARTNER,
		CUIButton_Glow::Create(pDevice, pContext, tGlowDesc))))
		return E_FAIL;

	/* Prototype_Button_Glow_Menu_Square */
	tGlowDesc = Get_GlowButtonPreset(GLOW_BUTTON_PRESET::MENU_SQUARE);
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::STATIC), PROTO_BTN_GLOW_MENU_SQUARE,
		CUIButton_Glow::Create(pDevice, pContext, tGlowDesc))))
		return E_FAIL;

	/* Prototype_Button_Layered_Get */
	auto tLayeredDesc = Get_LayeredButtonPreset(LAYERED_BUTTON_PRESET::GET_COMMAND);
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::STATIC), PROTO_BTN_LAYERED_GET,
		CUIButton_Layered::Create(pDevice, pContext, tLayeredDesc))))
		return E_FAIL;

	/* Prototype_Button_Layered_Battle_Command */
	tLayeredDesc = Get_LayeredButtonPreset(LAYERED_BUTTON_PRESET::BATTLE_COMMAND);
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::STATIC), PROTO_BTN_LAYERED_BTL_COMMAND,
		CUIButton_Layered::Create(pDevice, pContext, tLayeredDesc))))
		return E_FAIL;

	return S_OK;
}

HRESULT Ready_SharedTextures(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	auto* pManager = CSharedTexture_Manager::GetInstance();
	if (FAILED(pManager->Initialize(pDevice, pContext))) return E_FAIL;

	//pManager->Register_TextureGroup(SHARED_TEXTURE_TYPE::MASK,
	//     TEXT("../../Resources/Shared/mask/mask_%02d.png"), 1);
	 pManager->Register_TextureGroup(SHARED_TEXTURE_TYPE::NOISE,
		 TEXT("../../Resources/Shared/noise/noise_%02d.png"), 1);
	 pManager->Register_TextureGroup(SHARED_TEXTURE_TYPE::GRADIENT,
		 TEXT("../../Resources/Shared/gradient/gradient_%02d.png"), 1);
	 //pManager->Register_TextureGroup(SHARED_TEXTURE_TYPE::HIGHLIGHT,
		// TEXT("../../Resources/Shared/highlight/highlight_%02d.png"), 1);

	CGameInstance::GetInstance()->Set_SharedTextureBinder(pManager);
	return S_OK;
}

HRESULT Ready_Fonts()
{
	CGameInstance* m_pGameInstance = CGameInstance::GetInstance();

	if (FAILED(m_pGameInstance->Add_Font(FONT_MALGUN, TEXT("../../Resources/Fonts/malgun.spritefont"))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Font(FONT_NANUMBARUNGOTHIC, TEXT("../../Resources/Fonts/NanumBarunGothic.spritefont"))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Font(FONT_NOTOSANSKR, TEXT("../../Resources/Fonts/NotoSansKRThin.spritefont"))))
		return E_FAIL;

	return S_OK;
}
#pragma endregion

#pragma region UI 컨트롤러 Hub 래퍼

namespace
{
	CUIController_Hub* g_pUIHub = { nullptr };

	CUIController_Hub* Get_UIHub()
	{
		if (nullptr == g_pUIHub)
			g_pUIHub = CUIController_Hub::Create();
		return g_pUIHub;
	}
}

HRESULT UI_Register(CUIController* pCtrl, _uint iOwnerLevel)
{
	auto* pHub = Get_UIHub();
	if (nullptr == pHub)
		return E_FAIL;
	return pHub->Register(pCtrl, iOwnerLevel);
}

void UI_Unregister(CUIController* pCtrl)
{
	if (nullptr == g_pUIHub)
		return;
	g_pUIHub->Unregister(pCtrl);
}

void UI_Update_All(_float fTimeDelta)
{
	if (nullptr == g_pUIHub)
		return;
	g_pUIHub->Update_All(fTimeDelta);
}

void UI_Close_All()
{
	if (nullptr == g_pUIHub)
		return;
	g_pUIHub->Close_All();
}

void UI_Cleanup_Level(_uint iOwnerLevel)
{
	if (nullptr == g_pUIHub)
		return;
	g_pUIHub->Cleanup_Level(iOwnerLevel);
}

_bool UI_Is_AnyOpen()
{
	if (nullptr == g_pUIHub)
		return false;
	return g_pUIHub->Is_AnyOpen();
}

void UI_Set_Cursor_Sequence(CUISequence* pSeq)
{
	/* 해제(nullptr) 시에는 Hub 를 새로 만들면 안 됨 - 다른 래퍼들과 동일한 g_pUIHub 가드 사용.
	   주입(non-nullptr) 시에는 Get_UIHub 로 Hub 생성 보장. */
	if (nullptr == pSeq)
	{
		if (nullptr == g_pUIHub)
			return;
		g_pUIHub->Set_Cursor_Sequence(nullptr);
		return;
	}

	auto* pHub = Get_UIHub();
	if (nullptr == pHub)
		return;
	pHub->Set_Cursor_Sequence(pSeq);
}

void UI_Cleanup()
{
	Safe_Release(g_pUIHub);
	g_pUIHub = nullptr;
}
#pragma endregion

NS_END
