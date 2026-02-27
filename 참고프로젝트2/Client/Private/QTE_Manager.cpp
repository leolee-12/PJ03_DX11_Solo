#include "stdafx.h"
#include "QTE_Manager.h"
#include "GameInstance.h"
#include "QTE_Same_Grab.h"
#include "QTE_Hit.h"
#include "QTE_Continuous_Attack.h"
#include "QTE_1P_Same_Grab.h"

IMPLEMENT_SINGLETON(CQTE_Manager)

CQTE_Manager::CQTE_Manager()
	: m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CQTE_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	//동시잡기
	CGameObject* SameGrab = m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_QTE_Same_Grab"));
	m_vecQTE.push_back(SameGrab);

	//박자맞추기
	CGameObject* Hit = m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_QTE_Hit"));
	m_vecQTE.push_back(Hit);

	//연타
	CGameObject* ConAttack = m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_QTE_Continuous_Attack"));
	m_vecQTE.push_back(ConAttack);

	//동시잡기 1p용
	CGameObject* SameGrab_1p = m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_QTE_1P_Same_Grab"));
	m_vecQTE.push_back(SameGrab_1p);

	return S_OK;
}

void CQTE_Manager::Camera_Update(_float fTimeDelta)
{
}


void CQTE_Manager::Update(_float fTimeDelta)
{
	for (auto& iter : m_vecQTE)
		iter->Update(fTimeDelta);

	//디버그용
	if (m_pGameInstance->Key_Down(DIK_F6))
	{
		CGameObject* gameObject = m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Character"), 0);

		Start_QTE(QTE_ID::QTE_ID_1P_SAME_GRAB, gameObject, 10.f);
	}
}

void CQTE_Manager::Late_Update(_float fTimeDelta)
{
	for (auto& iter : m_vecQTE)
		iter->Late_Update(fTimeDelta);
}

HRESULT CQTE_Manager::Render(_float fTimeDelta)
{
	return S_OK;
}

void CQTE_Manager::Start_QTE(QTE_ID ID, CGameObject* callObject, _float _lifeTime)
{
	switch (ID)
	{
	case Client::CQTE_Manager::QTE_ID_SAME_GRAB:
		static_cast<CQTE_Same_Grab*>(m_vecQTE[ID])->Start(callObject, _lifeTime);
		break;
	case Client::CQTE_Manager::QTE_ID_HIT:
		static_cast<CQTE_Hit*>(m_vecQTE[ID])->Start_Hit(callObject);
		break;
	case Client::CQTE_Manager::QTE_ID_CONTINUOUS_ATTACK:
		static_cast<CQTE_Continuous_Attack*>(m_vecQTE[ID])->Start(callObject);
		break;
	case Client::CQTE_Manager::QTE_ID_1P_SAME_GRAB:
		static_cast<CQTE_1P_Same_Grab*>(m_vecQTE[ID])->Start(callObject, _lifeTime);
		break;
	}
}

void CQTE_Manager::Free()
{
	Safe_Release(m_pGameInstance);

	for (auto& iter : m_vecQTE)
		Safe_Release(iter);

	__super::Free();
}
