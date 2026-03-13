#include "Level_EditLogo.h"
#include "Level_EditLoading.h"
#include "GameInstance.h"

static constexpr _uint CURRENT_LEVEL = ETOUI(LEVEL::EDITLOGO);

CLevel_EditLogo::CLevel_EditLogo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_EditLogo::Initialize()
{
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_EditLogo::Update(_float fTimeDelta)
{
	if (GetKeyState(VK_RETURN) & 0x8000)
	{
		if (SUCCEEDED(m_pGameInstance->Change_Level(ETOI(LEVEL::LOADING), CLevel_EditLoading::Create(m_pDevice, m_pContext, LEVEL::EDITPLAY))))
			return;
	}
}

HRESULT CLevel_EditLogo::Render()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("로고레벨입니다."));
#endif

	return S_OK;
}

HRESULT CLevel_EditLogo::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject(CURRENT_LEVEL, TEXT("Prototype_GameObject_BackGround"), CURRENT_LEVEL, strLayerTag)))
		return E_FAIL;

	return S_OK;
}

CLevel_EditLogo* CLevel_EditLogo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_EditLogo* pInstance = new CLevel_EditLogo(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_EditLogo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_EditLogo::Free()
{
	__super::Free();
}