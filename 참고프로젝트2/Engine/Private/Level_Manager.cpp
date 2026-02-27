#include "..\Public\Level_Manager.h"
#include "Level.h"

CLevel_Manager::CLevel_Manager()	
{
	
}

HRESULT CLevel_Manager::Initialize()
{
	return S_OK;
}

void CLevel_Manager::Update(_float fTimeDelta)
{
	if (nullptr == m_pCurrentLevel)
		return;

	m_pCurrentLevel->Update(fTimeDelta);
}

HRESULT CLevel_Manager::Render(_float fTimeDelta)
{
	if (nullptr == m_pCurrentLevel)
		return E_FAIL;

	return m_pCurrentLevel->Render(fTimeDelta);
}

HRESULT CLevel_Manager::Change_Level(CLevel * pNewLevel)
{
	if(nullptr != m_pCurrentLevel)
		m_pCurrentLevel->Clear_Resources();

	Safe_Release(m_pCurrentLevel);


	m_pCurrentLevel = pNewLevel;

	return S_OK;
}

_uint CLevel_Manager::Get_CurrentLevel_Index()
{
	return m_pCurrentLevel->GetLevelIndex();
}

_uint CLevel_Manager::Get_LoadingLevel_Index()
{
	return m_iloadingLevelIndex;
}

HRESULT CLevel_Manager::Set_LoadingLevel_Index(_uint _level)
{
	m_iloadingLevelIndex = _level;
	return S_OK;
}


CLevel_Manager * CLevel_Manager::Create()
{
	CLevel_Manager*		pInstance = new CLevel_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_Manager::Free()
{
	__super::Free();
	
	Safe_Release(m_pCurrentLevel);
}

