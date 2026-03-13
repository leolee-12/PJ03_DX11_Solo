#include "Level_EditPlay.h"

static constexpr _uint CURRENT_LEVEL = ETOUI(LEVEL::EDITPLAY);

CLevel_EditPlay::CLevel_EditPlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_EditPlay::Initialize()
{
	return S_OK;
}

void CLevel_EditPlay::Update(_float fTimeDelta)
{
}

HRESULT CLevel_EditPlay::Render()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));
#endif

	return S_OK;
}

CLevel_EditPlay* CLevel_EditPlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_EditPlay* pInstance = new CLevel_EditPlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_EditPlay");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_EditPlay::Free()
{
	__super::Free();
}