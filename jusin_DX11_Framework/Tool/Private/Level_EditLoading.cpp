#include "Level_EditLoading.h"
#include "GameInstance.h"
#include "EditLoader.h"

#include "Level_EditLogo.h"
#include "Level_EditPlay.h"

static constexpr _uint CURRENT_LEVEL = ETOUI(LEVEL::LOADING);

CLevel_EditLoading::CLevel_EditLoading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_EditLoading::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	m_pLoader = CEditLoader::Create(m_pDevice, m_pContext, eNextLevelID);
	if (nullptr == m_pLoader)
		return E_FAIL;

	return S_OK;
}

void CLevel_EditLoading::Update(_float fTimeDelta)
{
    if (GetKeyState(VK_SPACE) & 0x8000 &&
        true == m_pLoader->isFinished())
    {
        CLevel* pNextLevel = { nullptr };

        switch (m_eNextLevelID)
        {
        case LEVEL::EDITLOGO:
            pNextLevel = CLevel_EditLogo::Create(m_pDevice, m_pContext);
            break;
        case LEVEL::EDITPLAY:
            pNextLevel = CLevel_EditPlay::Create(m_pDevice, m_pContext);
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

HRESULT CLevel_EditLoading::Render()
{
#ifdef _DEBUG
    m_pLoader->Show();
#endif

	return S_OK;
}

CLevel_EditLoading* CLevel_EditLoading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLevel_EditLoading* pInstance = new CLevel_EditLoading(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Created : CLevel_EditLoading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_EditLoading::Free()
{
	__super::Free();

    Safe_Release(m_pLoader);
}