#include "Game_API.h"
#include "Level_Loading.h"
#include "SharedTexture_Manager.h"

#include "GameInstance.h"
#include "UIContainer.h"
#include "UIImage.h"
#include "UIText.h"
#include "UIButton.h"
#include "UIProgressBar.h"

NS_BEGIN(Game_PKM)

HRESULT Ready_Prototypes_For_Static(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGameInstance* m_pGameInstance = CGameInstance::GetInstance();

	/* Prototype_Component_Texture_Dummy_Black */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_TEXTURE_DUMMY_BLACK,
		CTexture::Create(pDevice, pContext, TEXT("../../Resources/dummy/dummy_black.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Dummy_Magenta */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_TEXTURE_DUMMY_MAGENTA,
		CTexture::Create(pDevice, pContext, TEXT("../../Resources/dummy/dummy_magenta.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Dummy_White */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_TEXTURE_DUMMY_WHITE,
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

	return S_OK;
}

HRESULT Ready_Prototypes_For_Editor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGameInstance* m_pGameInstance = CGameInstance::GetInstance();

	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::STATIC), PROTO_UI_CONTAINER,
		CUIContainer::Create(pDevice, pContext))))
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

HRESULT Ready_SharedTextures()
{
	CSharedTexture_Manager* pManager = CSharedTexture_Manager::GetInstance();

	if (FAILED(pManager->Initialize()))
		return E_FAIL;

	if (FAILED(pManager->Register_TextureGroup(SHARED_TEXTURE::NOISE, ETOUI(LEVEL::STATIC),
		PROTO_COM_TEXTURE_SHARED_NOISE)))
		return E_FAIL;

	if (FAILED(pManager->Register_TextureGroup(SHARED_TEXTURE::GRADIENT, ETOUI(LEVEL::STATIC),
		PROTO_COM_TEXTURE_SHARED_GRADIENT)))
		return E_FAIL;

	if (FAILED(pManager->Register_TextureGroup(SHARED_TEXTURE::MASK, ETOUI(LEVEL::STATIC),
		PROTO_COM_TEXTURE_SHARED_MASK)))
		return E_FAIL;

	// 등록 후 Engine UI leaf의 binder hook이 호출할 binder를 매니저로 지정
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

NS_END
