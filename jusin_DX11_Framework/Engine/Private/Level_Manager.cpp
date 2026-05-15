#include "Level_Manager.h"
#include "GameInstance.h"
#include "Level.h"

CLevel_Manager::CLevel_Manager()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CLevel_Manager::Change_Level(_int iNewLevelIndex, CLevel* pNewLevel)
{
    if (nullptr == pNewLevel)
        return E_FAIL;

    /* 스택 전체를 위에서부터 정리 */
    while (false == m_LevelStack.empty())
    {
        LEVEL_ENTRY& top = m_LevelStack.back();
        m_pGameInstance->Clear_Resources(top.iLevelIndex);
        Safe_Release(top.pLevel);
        m_LevelStack.pop_back();
    }

    /* 새 레벨을 스택에 push */
    LEVEL_ENTRY entry{ iNewLevelIndex, pNewLevel };
    m_LevelStack.push_back(entry);

    m_iCurrentLevelIndex = iNewLevelIndex;

    return S_OK;
}

HRESULT CLevel_Manager::Push_Level(_int iLevelIndex, CLevel* pNewLevel)
{
    if (nullptr == pNewLevel)
        return E_FAIL;

    /* 직전 top이 있으면 OnPause 호출 */
    if (false == m_LevelStack.empty())
    {
        CLevel* pPrevTop = m_LevelStack.back().pLevel;
        if (nullptr != pPrevTop)
            pPrevTop->OnPause();
    }

    LEVEL_ENTRY entry{ iLevelIndex, pNewLevel };
    m_LevelStack.push_back(entry);

    m_iCurrentLevelIndex = iLevelIndex;

    return S_OK;
}

HRESULT CLevel_Manager::Pop_Level()
{
    /* 스택이 비었거나 1단이면 pop 불가 */
    if (m_LevelStack.size() < 2)
        return E_FAIL;

    /* 현재 top 정리 */
    LEVEL_ENTRY top = m_LevelStack.back();
    m_pGameInstance->Clear_Resources(top.iLevelIndex);
    Safe_Release(top.pLevel);
    m_LevelStack.pop_back();

    /* 새 top resume */
    LEVEL_ENTRY& newTop = m_LevelStack.back();
    if (nullptr != newTop.pLevel)
        newTop.pLevel->OnResume();

    m_iCurrentLevelIndex = newTop.iLevelIndex;

    return S_OK;
}

_bool CLevel_Manager::Is_Level_Active(_uint iLevel) const
{
    /* STATIC(0번 슬롯)은 항상 활성 (영속) */
    if (0u == iLevel)
        return true;

    /* 스택 top과 같은 슬롯이면 활성. 그 외(paused 중인 베이스)는 비활성. */
    if (false == m_LevelStack.empty() &&
        static_cast<_int>(iLevel) == m_LevelStack.back().iLevelIndex)
        return true;

    return false;
}

void CLevel_Manager::Update(_float fTimeDelta)
{
    if (m_LevelStack.empty())
        return;

    CLevel* pTop = m_LevelStack.back().pLevel;
    if (nullptr != pTop)
        pTop->Update(fTimeDelta);
}

HRESULT CLevel_Manager::Render()
{
    if (m_LevelStack.empty())
        return E_FAIL;

    CLevel* pTop = m_LevelStack.back().pLevel;
    if (nullptr == pTop)
        return E_FAIL;

    return pTop->Render();
}

CLevel_Manager* CLevel_Manager::Create()
{
    return new CLevel_Manager();
}

void CLevel_Manager::Free()
{
    __super::Free();

    /* 스택의 모든 레벨 Release.
       Clear_Resources는 Release_Engine 흐름에서 각 매니저의 Free가 자체 청소하므로
       여기서 중복 호출하지 않는다 (기존 정책과 동일). */
    for (auto it = m_LevelStack.rbegin(); it != m_LevelStack.rend(); ++it)
        Safe_Release(it->pLevel);
    m_LevelStack.clear();

    Safe_Release(m_pGameInstance);
}