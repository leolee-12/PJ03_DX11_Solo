#include "Level_Logo.h"
#include "Level_Loading.h"

#include "UIImage.h"
#include "Effect_Star.h"

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
	if (m_pGameInstance->Key_Down(DIK_TAB))
	{
		if (SUCCEEDED(m_pGameInstance->Change_Level(ETOI(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::GAMEPLAY))))
			return;
	}

	if (m_pGameInstance->Key_Down(DIK_F5) && m_pTestStarEffect && m_pTestTargetUI)
	{
		m_pTestStarEffect->Play(m_pTestTargetUI, _float2(0.f, 0.f));
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
	CUIImage::UIIMAGE_DESC WhiteDesc{};
	WhiteDesc.fCenterX = 960.f;
	WhiteDesc.fCenterY = 540.f;
	WhiteDesc.fSizeX = 1920.f;
	WhiteDesc.fSizeY = 1080.f;
	WhiteDesc.iZOrder = 0;
	WhiteDesc.bVisible = true;
	WhiteDesc.strTextureTag = PROTO_COM_TEXTURE_DUMMY_WHITE;
	WhiteDesc.iTextureLevel = ETOUI(LEVEL::STATIC);
	WhiteDesc.iTextureIndex = 0;
	WhiteDesc.strShaderTag = PROTO_COM_SHADER_UI;
	WhiteDesc.iShaderLevel = ETOUI(LEVEL::STATIC);
	WhiteDesc.strVIBufferTag = PROTO_COM_VIBUFFER_RECT;
	WhiteDesc.iVIBufferLevel = ETOUI(LEVEL::STATIC);
	WhiteDesc.iShaderPass = 0;
	WhiteDesc.vColor = _float4(1.f, 1.f, 1.f, 1.f);

	CUIImage* pWhite = static_cast<CUIImage*>(m_pGameInstance->Clone_Prototype(
		PROTOTYPE::GAMEOBJECT, ETOUI(LEVEL::STATIC), PROTO_UI_IMAGE, &WhiteDesc));

	if (nullptr == pWhite)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_Ex(CURRENT_LEVEL, strLayerTag, pWhite)))
	{
		Safe_Release(pWhite);
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_Monster(WNameID strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_UI(WNameID strLayerTag)
{
	CUIImage::UIIMAGE_DESC TargetDesc{};
	TargetDesc.fCenterX = 960.f;
	TargetDesc.fCenterY = 540.f;
	TargetDesc.fSizeX = 220.f;
	TargetDesc.fSizeY = 120.f;
	TargetDesc.iZOrder = 10;
	TargetDesc.bVisible = true;
	TargetDesc.strTextureTag = PROTO_COM_TEXTURE_TITLE_LOGO_DIFF;
	TargetDesc.iTextureLevel = ETOUI(LEVEL::GAMEPLAY);
	TargetDesc.iTextureIndex = 0;
	TargetDesc.strShaderTag = PROTO_COM_SHADER_UI;
	TargetDesc.iShaderLevel = ETOUI(LEVEL::STATIC);
	TargetDesc.strVIBufferTag = PROTO_COM_VIBUFFER_RECT;
	TargetDesc.iVIBufferLevel = ETOUI(LEVEL::STATIC);
	TargetDesc.iShaderPass = 0;
	TargetDesc.vColor = _float4(1.f, 1.f, 1.f, 1.f);

	CUIImage* pTarget = static_cast<CUIImage*>(m_pGameInstance->Clone_Prototype(
		PROTOTYPE::GAMEOBJECT, ETOUI(LEVEL::STATIC), PROTO_UI_IMAGE, &TargetDesc));

	if (nullptr == pTarget)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_Ex(CURRENT_LEVEL, strLayerTag, pTarget)))
	{
		Safe_Release(pTarget);
		return E_FAIL;
	}

	m_pTestTargetUI = pTarget;

	CEffect_Star::EFFECT_STAR_DESC StarDesc{};
	StarDesc.fCenterX = 960.f;
	StarDesc.fCenterY = 540.f;
	StarDesc.fSizeX = 1.f;
	StarDesc.fSizeY = 1.f;
	StarDesc.iZOrder = 10;
	StarDesc.bVisible = false;
	StarDesc.pFollowTarget = m_pTestTargetUI;
	StarDesc.vFollowOffset = _float2(0.f, 0.f);
	StarDesc.iNumParticles = 32;
	StarDesc.vSpawnRange = _float2(160.f, 90.f);
	StarDesc.vSizeRange = _float2(64.f, 128.f);
	StarDesc.vSpeedRange = _float2(20.f, 80.f);
	StarDesc.vLifeRange = _float2(0.35f, 0.8f);
	StarDesc.vRotationSpeedRange = _float2(-1.f, 1.f);
	StarDesc.vMaskRotationSpeedRange = _float2(-0.5f, 0.5f);
	StarDesc.fMaskStrength = 0.5f;
	StarDesc.vColor = _float4(0.835f, 0.741f, 0.310f, 1.f);
	StarDesc.bStartActive = false;
	StarDesc.bLoop = false;
	StarDesc.iStarTextureIndex = 0;
	StarDesc.iMaskTextureIndex = 1;
	StarDesc.iDiamondTextureIndex = 2;
	StarDesc.eMaskSampleMode = TEXTURE_SAMPLE_MODE::BI_VERTICAL;
	StarDesc.eSubSampleMode = TEXTURE_SAMPLE_MODE::QUAD;

	CEffect_Star* pStar = static_cast<CEffect_Star*>(m_pGameInstance->Clone_Prototype(
		PROTOTYPE::GAMEOBJECT, ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_EFT_STAR, &StarDesc));

	if (nullptr == pStar)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_Ex(CURRENT_LEVEL, strLayerTag, pStar)))
	{
		Safe_Release(pStar);
		return E_FAIL;
	}

	m_pTestStarEffect = pStar;

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