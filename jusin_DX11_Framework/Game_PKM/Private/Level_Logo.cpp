#include "Level_Logo.h"
#include "Level_Loading.h"
#include "Effect_Star.h"

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

	Ready_Event();

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

	if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::STATIC), PROTO_UI_SEQUENCE, CURRENT_LEVEL, strLayerTag, &SeqDesc)))
		return E_FAIL;

	CEffect_Star::EFFECT_STAR_DESC StarDesc{};
	StarDesc.fCenterX = 960.f;
	StarDesc.fCenterY = 540.f;
	StarDesc.fSizeX = 1.f;
	StarDesc.fSizeY = 1.f;
	StarDesc.iZOrder = 10;
	StarDesc.bVisible = false;

	StarDesc.iNumParticles = 16;
	StarDesc.vSpawnRange = _float2(160.f, 90.f);
	StarDesc.vSizeRange = _float2(64.f, 128.f);
	StarDesc.vSpeedRange = _float2(60.f, 80.f);
	StarDesc.vEmitDir = _float2(-1.f, 0.f);
	StarDesc.fEmitSpreadAngle = XMConvertToRadians(70.f);
	StarDesc.vLifeRange = _float2(0.35f, 0.8f);
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

	if (FAILED(m_pGameInstance->Add_GameObject(CURRENT_LEVEL, PROTO_OBJ_EFT_STAR, CURRENT_LEVEL, strLayerTag, &StarDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Event()
{
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