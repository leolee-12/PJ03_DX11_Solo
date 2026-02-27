#include "stdafx.h"
#include "SubTitle_Manager.h"
#include "GameInstance.h"
#include "SubTitle.h"
IMPLEMENT_SINGLETON(CSubTitle_Manager)

CSubTitle_Manager::CSubTitle_Manager()
	: m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CSubTitle_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	// 자막 텍스트 초기화
	m_mapSubTitleText[GOKU_VS_FRIEZA_SUBTITLE_0] = L"그만둬 프리저---!!!!";
	m_mapSubTitleText[GOKU_VS_FRIEZA_SUBTITLE_1] = L"오공----!!!!";
	m_mapSubTitleText[GOKU_VS_FRIEZA_SUBTITLE_2] = L"요..용서 못해.. 잘도.. 잘도..";
	m_mapSubTitleText[GOKU_VS_FRIEZA_SUBTITLE_3] = L"뭐..뭐냐..!?";
	m_mapSubTitleText[GOKU_VS_FRIEZA_SUBTITLE_4] = L"나 화났다----!!! 프리저---!!!!!";
	m_mapSubTitleText[HUMAN_21_SUBTITLE_0] = L"자, 즐겁고 즐거운 사냥 시간이야";
	m_mapSubTitleText[FRIEZA_SUBTITLE_0] = L"워밍업은 이쯤 해두고 슬슬 진심으로 해볼까...";
	m_mapSubTitleText[GOKU_SUBTITLE_0] = L"나는 초사이어인, 손오공이다";
	m_mapSubTitleText[HIT_WIN_SUBTITLE_0] = L"너라는 녀석은 끝이 안 보여서 이해할 수가 없군";

	m_pSubTitle = static_cast<CSubTitle*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_SubTitle")));

	return S_OK;
}

void CSubTitle_Manager::Camera_Update(_float fTimeDelta)
{
}


void CSubTitle_Manager::Update(_float fTimeDelta)
{
	m_pSubTitle->Update(fTimeDelta);
}

void CSubTitle_Manager::Late_Update(_float fTimeDelta)
{
	m_pSubTitle->Late_Update(fTimeDelta);
}

HRESULT CSubTitle_Manager::Render(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CSubTitle_Manager::Play(SUBTITLE_ID eID, _float duration)
{
	// 자막 ID가 유효한지 확인
	auto iter = m_mapSubTitleText.find(eID);
	if (iter == m_mapSubTitleText.end())
		return E_FAIL;

	m_pSubTitle->Initialize_SubTitle(iter->second, duration);
	m_pSubTitle->SetActive(true);

	return S_OK;
}

void CSubTitle_Manager::Stop(SUBTITLE_ID eID)
{
	m_pSubTitle->SetActive(false);
}



void CSubTitle_Manager::Free()
{
	Safe_Release(m_pSubTitle);
	Safe_Release(m_pGameInstance);

	__super::Free();
}
