#include "Level_Manager.h"

#include "Level.h"

#include "GameInstance.h"

CLevel_Manager::CLevel_Manager()
    : m_pGameInstance { CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

void CLevel_Manager::Update(_float fTimeDelta)
{
    if (nullptr == m_pCurrentLevel)
        return;

    m_pCurrentLevel->Update(fTimeDelta);
}

HRESULT CLevel_Manager::Render()
{
    if (nullptr == m_pCurrentLevel)
        return E_FAIL;

   return m_pCurrentLevel->Render();
}

HRESULT CLevel_Manager::Change_Level(_uint iLevelID, CLevel* pNewLevel)
{
    /* 기존 레벨용 자원을 삭제한다. */
    if(nullptr != m_pCurrentLevel)
        m_pGameInstance->Clear_Resources(m_iLevelID);

    Safe_Release(m_pCurrentLevel);

    m_pCurrentLevel = pNewLevel;

    m_iLevelID = iLevelID;

    return S_OK;
}

CLevel_Manager* CLevel_Manager::Create()
{
    return new CLevel_Manager();
}

void CLevel_Manager::Free()
{
    __super::Free();

    m_pGameInstance->DestroyInstance();

    Safe_Release(m_pCurrentLevel);
}
