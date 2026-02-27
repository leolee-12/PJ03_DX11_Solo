#include "stdafx.h"
#include "..\Public\QTE_Hit.h"

#include "RenderInstance.h"
#include "GameInstance.h"
#include "QTE_Hit_Situation.h"
#include "Character.h"

CQTE_Hit::CQTE_Hit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CQTE_Hit::CQTE_Hit(const CQTE_Hit& Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CQTE_Hit::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CQTE_Hit::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_vecHit_Situation.resize(Hit_Situation_ID_END);

	CQTE_Hit_Situation::QTE_HIT_SITUATION_DESC Desc{};

#pragma region Goku_Hit

	Desc.lifeTime = 15.f;
	Desc.create_Num = 3;
	Desc.ID = Hit_Situation_ID_Goku;

	CQTE_Hit_Situation* situation_Goku = static_cast<CQTE_Hit_Situation*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_QTE_Hit_Situation"), &Desc));
	m_vecHit_Situation[Hit_Situation_ID_Goku] = situation_Goku;

#pragma endregion

#pragma region 21_Hit

	Desc.lifeTime = 15.f;
	Desc.create_Num = 3;
	Desc.ID = Hit_Situation_ID_21;

	CQTE_Hit_Situation* situation_21 = static_cast<CQTE_Hit_Situation*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_QTE_Hit_Situation"), &Desc));
	m_vecHit_Situation[Hit_Situation_ID_21] = situation_21;

#pragma endregion

#pragma region Frieza_Hit

	Desc.lifeTime = 15.f;
	Desc.create_Num = 5;
	Desc.ID = Hit_Situation_ID_Frieza;

	CQTE_Hit_Situation* situation_Frieza = static_cast<CQTE_Hit_Situation*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_QTE_Hit_Situation"), &Desc));
	m_vecHit_Situation[Hit_Situation_ID_Frieza] = situation_Frieza;

#pragma endregion

#pragma region Hit_Hit

	Desc.lifeTime = 10.f;
	Desc.create_Num = 3;
	Desc.ID = Hit_Situation_ID_Hit;

	CQTE_Hit_Situation* situation_Hit = static_cast<CQTE_Hit_Situation*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_QTE_Hit_Situation"), &Desc));
	m_vecHit_Situation[Hit_Situation_ID_Hit] = situation_Hit;

#pragma endregion


	return S_OK;
}

void CQTE_Hit::Camera_Update(_float fTimeDelta)
{

}

void CQTE_Hit::Update(_float fTimeDelta)
{
	if (m_current_Situation_ID == Hit_Situation_ID_END)
		return;

	//선택된 시뮬레이션만 Update
	m_vecHit_Situation[m_current_Situation_ID]->Update(fTimeDelta);
}

void CQTE_Hit::Late_Update(_float fTimeDelta)
{
	if (m_current_Situation_ID == Hit_Situation_ID_END)
		return;

	//선택된 시뮬레이션만 Late_Update
	m_vecHit_Situation[m_current_Situation_ID]->Late_Update(fTimeDelta);
}

HRESULT CQTE_Hit::Render(_float fTimeDelta)
{
	return S_OK;
}

void CQTE_Hit::Start_Hit(CGameObject* pCall_Object)
{
	CUI_Define::PLAYER_ID ID = static_cast<CCharacter*>(pCall_Object)->Get_CharacterID();

	m_current_Situation_ID = (Hit_Situation_ID)ID;
	m_vecHit_Situation[m_current_Situation_ID]->Start(pCall_Object);
}

CQTE_Hit* CQTE_Hit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CQTE_Hit* pInstance = new CQTE_Hit(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CQTE_Hit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CQTE_Hit::Clone(void* pArg)
{
	CQTE_Hit* pInstance = new CQTE_Hit(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CQTE_Hit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CQTE_Hit::Free()
{
	for (auto& iter : m_vecHit_Situation)
		Safe_Release(iter);

	__super::Free();
}
