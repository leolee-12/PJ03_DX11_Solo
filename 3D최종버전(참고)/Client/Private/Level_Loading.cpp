#include "Level_Loading.h"

#include "Loader.h"
#include "GameInstance.h"

#include "Level_Logo.h"
#include "Level_GamePlay.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}


HRESULT CLevel_Loading::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	if (FAILED(Ready_Layer_BackGround()))
		return E_FAIL;
	if (FAILED(Ready_Layer_UI()))
		return E_FAIL;

	/* 다음 레벨에대한 자원준비를 할 수 있도록 한다. */
	m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevelID);
	if (nullptr == m_pLoader)
		return E_FAIL;

	return S_OK;
}

void CLevel_Loading::Update(_float fTimeDelta)
{
	


		
//	10000000 01100011
//&	10000000 00000000
//	10000000 00000000
	/*if (GetKeyState(VK_SPACE) & 0b1000000000000000)
	if (GetKeyState(VK_SPACE) & 0x8000)
	if (GetKeyState(VK_SPACE) < 0)
	{


	}*/

	if (true == m_pLoader->isFinished() &&
		GetKeyState(VK_SPACE) & 0x8000)
	{
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


		if (FAILED(m_pGameInstance->Change_Level(ENUM_CLASS(m_eNextLevelID), pNextLevel)))
			return;

		return;
	}	
}

HRESULT CLevel_Loading::Render()
{
	m_pLoader->Print_Text();

	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_BackGround()
{
	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_UI()
{
	return S_OK;
}

CLevel_Loading* CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLevel_Loading* pInstance = new CLevel_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
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
