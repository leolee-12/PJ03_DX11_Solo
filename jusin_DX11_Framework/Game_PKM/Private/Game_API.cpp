#include "Game_API.h"
#include "Level_Loading.h"
#include "SharedTexture_Manager.h"
#include "UIButton_Glow.h"
#include "UIButton_Layered.h"
#include "UIController.h"
#include "UIController_Hub.h"

#include "GameInstance.h"
#include "UISequence.h"
#include "UIImage.h"
#include "UIText.h"
#include "UIProgressBar.h"

NS_BEGIN(Game_PKM)

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

HRESULT Start_Level(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eStartLevelID)
{
	CLevel* pPreLevel = CLevel_Loading::Create(pDevice, pContext, eStartLevelID);

	if (nullptr == pPreLevel)
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Change_Level(ETOI(LEVEL::LOADING), pPreLevel)))
		return E_FAIL;

	return S_OK;
}

#pragma region UI 컨트롤러 Hub 래퍼
HRESULT UI_Register(CUIController* pCtrl)
{
	auto* pHub = Get_UIHub();
	if (nullptr == pHub)
		return E_FAIL;
	return pHub->Register(pCtrl);
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

void UI_Cleanup()
{
	Safe_Release(g_pUIHub);
	g_pUIHub = nullptr;
}
#pragma endregion

NS_END
