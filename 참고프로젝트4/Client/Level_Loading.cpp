#include "stdafx.h"
#include "..\Public\Level_Loading.h"
#include "Loader.h"

#include "GameInstance.h"
#include "Level_Logo.h"
#include "Level_Stage1.h"
#include "Level_Stage2.h"
#include "Level_Stage3.h"
#include "Level_Village1.h"
#include "Level_Village2.h"
#include "Level_BossStage.h"
#include "Level_Tool.h"

#include "UI_LoadingLogo.h"

CLevel_Loading::CLevel_Loading(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CLevel(pDevice, pContext)
{

}

HRESULT CLevel_Loading::Initialize(LEVEL eNextLevelID)
{

	/* 추후 로딩 레벨이 끝나면 원래 목적으로 뒀던 레벨로 넘어가기위해서. */
 	m_eNextLevelID = eNextLevelID;

	m_pGameInstance->Set_CreateLevelIndex(m_eNextLevelID);
	/* 메인스레드로 대충 로드한다. */
	/* 로딩용 자원을 로드한다. */
	/* 로딩레벨에서 보여줘야할 객체들을 생성한다.(배경, 일러스트, 로딩바) */

	m_pGameInstance->Reset_Light();
	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.strLightName = "Loading_Light_Directional";
	LightDesc.vDirection = _float4(0.f, -1.f, 1.f, 0.f);
	LightDesc.vPosition = _float4(0.f, 0.f, 0.f, 1.f);
	LightDesc.fRange = 4.f;
	LightDesc.vDiffuse = _float4(0.8f, 0.8f, 0.85f, 0.5f);
	LightDesc.vAmbient = _float4(0.8f, 0.8f, 0.85f, 0.5f);
	LightDesc.vSpecular = _float4(0.1f, 0.1f, 0.1f, 0.5f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc, &m_pLight)))
		RETURN_EFAIL;

	if (m_pLoadingUI == nullptr) {
		if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_UI,
			TEXT("Prototype_GameObject_UI_LoadingLogo"), nullptr, &m_pLoadingUI)))
			RETURN_EFAIL;
	}

	/* 추가적인 스레드를 생성하여 eNextLevelID에 필요한 자원들을 로드한다. */
	m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevelID);
	if (nullptr == m_pLoader)
		RETURN_EFAIL;

	m_pGameInstance->Reset_CollisionManager();
	//m_pGameInstance->Reset_LightManager();
	m_pGameInstance->Clear_TickEvents();

	return S_OK;
}

void CLevel_Loading::Tick(_cref_time fTimeDelta)
{
	if (nullptr == m_pGameInstance)
		return;

	if (true == m_pLoader->isFinished())
	{
		//if (m_pGameInstance->Key_Down(DIK_SPACE))
		{
			CLevel*		pNewLevel = { nullptr };

			switch (m_eNextLevelID)
			{
			case LEVEL_LOGO:
				pNewLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_STAGE1:
				pNewLevel = CLevel_Stage1::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_STAGE2:
				pNewLevel = CLevel_Stage2::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_STAGE3:
				pNewLevel = CLevel_Stage3::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_VILLAGE1:
				pNewLevel = CLevel_Village1::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_VILLAGE2:
				pNewLevel = CLevel_Village2::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_BOSS1:
				pNewLevel = CLevel_BossStage::Create(m_pDevice, m_pContext);
				break;

			case LEVEL_TOOL:
				pNewLevel = CLevel_Tool::Create(m_pDevice, m_pContext);
				break;
			}

			if (nullptr == pNewLevel)
				return;

			if (FAILED(m_pGameInstance->Open_Level(m_eNextLevelID, pNewLevel)))
				return;
		}
	}
}

HRESULT CLevel_Loading::Render()
{
	m_pLoader->Print_LoadingText();

	return S_OK;
}


CLevel_Loading * CLevel_Loading::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, LEVEL eNextLevelID)
{
	CLevel_Loading*		pInstance = new CLevel_Loading(pDevice, pContext);

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
	
	m_pGameInstance->Release_Light(m_pLight);

	m_pLoadingUI->Set_Dead();
	m_pLoadingUI = nullptr;
}
