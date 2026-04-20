#include "Level_Loading.h"
#include "GameInstance.h"
#include "Loader.h"

#include "Level_Logo.h"
#include "Level_GamePlay.h"

NS_BEGIN(Game_PKM)
static constexpr _uint CURRENT_LEVEL = ETOUI(LEVEL::LOADING);
NS_END

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
	: CLevel{ pDevice, pContext }
	, m_eNextLevelID{ eNextLevelID }
{
}

HRESULT CLevel_Loading::Initialize()
{
	m_pLoader = CLoader::Create(m_pDevice, m_pContext, m_eNextLevelID);
	if (nullptr == m_pLoader)
		return E_FAIL;

	return S_OK;
}

void CLevel_Loading::Update(_float fTimeDelta)
{
    if (m_pGameInstance->Key_Down(DIK_SPACE) && m_pLoader->Is_Finished())
    {
        if (m_pLoader->Has_Error())
        {
            MSG_BOX("Loading failed");
            return;
        }

        CLevel* pNextLevel = { nullptr };

        switch (m_eNextLevelID)
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

        if (SUCCEEDED(m_pGameInstance->Change_Level(ETOI(m_eNextLevelID), pNextLevel)))
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

CLevel_Loading* CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLevel_Loading* pInstance = new CLevel_Loading(pDevice, pContext, eNextLevelID);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Loading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();

    Safe_Release(m_pLoader);
}