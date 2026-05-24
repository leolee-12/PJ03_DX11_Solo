#include "Level_Loading.h"
#include "Loader.h"
#include "Level_Logo.h"
#include "Level_GamePlay.h"

#include "GameInstance.h"
#include "UISequence.h"

NS_BEGIN(Game_PKM)
static constexpr _uint CURRENT_LEVEL = ETOUI(LEVEL::LOADING);
NS_END

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID, const LEVEL_ENTRY_DESC* pEntryDesc)
	: CLevel{ pDevice, pContext }
{
	if (nullptr != pEntryDesc)
		m_tEntryDesc = *pEntryDesc;
	else
		m_tEntryDesc.Clear();

	m_tEntryDesc.eNextLevelID = eNextLevelID;
}

HRESULT CLevel_Loading::Initialize()
{
	if (FAILED(Ready_Layer_UI(LAYER_UI)))
		return E_FAIL;

	m_pLoader = CLoader::Create(m_pDevice, m_pContext, m_tEntryDesc.eNextLevelID);
	if (nullptr == m_pLoader)
		return E_FAIL;

	return S_OK;
}

void CLevel_Loading::Update(_float fTimeDelta)
{
	const _bool bAdvance =
		m_tEntryDesc.bAutoAdvance || m_pGameInstance->Key_Down(DIK_SPACE);

	if (bAdvance && m_pLoader->Is_Finished())
	{
		if (m_pLoader->Has_Error())
		{
			MSG_BOX("Loading failed");
			return;
		}

		CLevel* pNextLevel = { nullptr };

		switch (m_tEntryDesc.eNextLevelID)
		{
		case LEVEL::LOGO:
			pNextLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
			break;
		case LEVEL::GAMEPLAY:
			pNextLevel = CLevel_GamePlay::Create(m_pDevice, m_pContext);
			break;
		}

		if (nullptr == pNextLevel)
		{
			MSG_BOX("Failed to Changed");
			return;
		}

		if (SUCCEEDED(m_pGameInstance->Change_Level(ETOI(m_tEntryDesc.eNextLevelID), pNextLevel)))
			return;
	}
}

HRESULT CLevel_Loading::Render()
{
#ifdef _DEBUG
	m_pLoader->Show();
#endif

	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_UI(WNameID strLayerTag)
{
	CUISequence::UISEQUENCE_DESC tDesc{};
	tDesc.strPath = "../../DataFiles/UI/UI_Loading.uiseq";
	tDesc.iProtoLevel = ETOUI(LEVEL::STATIC);

	CUISequence* pSeq = static_cast<CUISequence*>(m_pGameInstance->Clone_Prototype(
		PROTOTYPE::GAMEOBJECT,
		ETOUI(LEVEL::STATIC),
		PROTO_UI_SEQUENCE,
		&tDesc));

	if (nullptr == pSeq)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_Ex(CURRENT_LEVEL, strLayerTag, pSeq)))
	{
		Safe_Release(pSeq);
		return E_FAIL;
	}

	pSeq->Play();
	m_pLoadingUI = pSeq;

	return S_OK;
}

CLevel_Loading* CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
	LEVEL eNextLevelID, const LEVEL_ENTRY_DESC* pEntryDesc)
{
	CLevel_Loading* pInstance = new CLevel_Loading(pDevice, pContext, eNextLevelID, pEntryDesc);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Loading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loading::Free()
{
	m_pLoadingUI = nullptr;
	Safe_Release(m_pLoader);
	
	__super::Free();
}