#include "stdafx.h"
#include "..\Public\Level_Logo.h"
#include "GameInstance.h"

#include "Level_Loading.h"
#include "UI_StartMenu.h"
#include "UI_StartMenuBack.h"

#include "ObjPool_Manager.h"
#include "Client_Manager.h"

CLevel_Logo::CLevel_Logo(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Logo::Initialize()
{
	__super::Initialize();

#ifndef _DEBUG
	::ShowCursor(FALSE);
#endif

	return S_OK;
}

void CLevel_Logo::Tick(_cref_time fTimeDelta)
{
#ifdef _DEBUG
	m_pGameInstance->Render_FPS(fTimeDelta, g_hWnd, TEXT("[LOGO_LEVEL] "));
#endif
	if ((m_pGameInstance->Key_Pressing(DIK_RBRACKET) || (GET_SINGLE(CClient_Manager)->Get_LogoEnd())))
	{
		if (m_pGameInstance->Key_Pressing(DIK_RBRACKET))
		{
			dynamic_pointer_cast<CUI_StartMenu>(m_pLogoUI)->ALL_UI_NoneRender();
			m_pLogoUI.reset();
			m_pLogoUI = nullptr;			
		}

		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_TOOL))))
			return;
	}
}

HRESULT CLevel_Logo::Render()
{

	return S_OK;
}


HRESULT CLevel_Logo::Ready_Layer_Environment(const LAYERTYPE& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_Object(const LAYERTYPE& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_Character(const LAYERTYPE& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_Effect(const LAYERTYPE& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_Camera(const LAYERTYPE& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_UI(const LAYERTYPE& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_UI,
		TEXT("Prototype_GameObject_UI_StartMenu"), nullptr, &m_pLogoUI)))
		RETURN_EFAIL;

	return S_OK;
}



CLevel_Logo * CLevel_Logo::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	CLevel_Logo*		pInstance = new CLevel_Logo(pDevice, pContext);

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

	m_pGameInstance->FadeOut_BGM(1.f);
	
	if (m_pLogoUI != nullptr) {
		m_pLogoUI.reset();
		m_pLogoUI = nullptr;
	}
	
}
