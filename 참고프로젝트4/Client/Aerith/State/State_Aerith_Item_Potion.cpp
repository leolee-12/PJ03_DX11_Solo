#include "stdafx.h"
#include "Aerith/State/State_list_Aerith.h"
#include "StatusComp.h"

#include "Sound_Manager.h"

CState_Aerith_Item_Potion::CState_Aerith_Item_Potion(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	: BASE(pActor, pStatemachine)
{
}

HRESULT CState_Aerith_Item_Potion::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_ItemCure01", 1.3f, false, true, 2.f);

	//유진 - 회복 폰트 추가
	_float HP_plus = m_pStatusCom.lock()->Get_MaxHP() - m_pStatusCom.lock()->Get_CurHP(); //회복량
	_float3 MakeFontPos = m_pActor.lock()->Get_TransformCom().lock()->Get_State(CTransform::STATE_POSITION);
	_float3 MakeFontOffset = m_pActor.lock()->Get_PhysXColliderCom().lock()->Get_LocalOffset();
			MakeFontPos.y += MakeFontOffset.y;
	GET_SINGLE(CUI_Manager)->Make_DamageFont(1, &MakeFontPos, static_cast<_int>(HP_plus));

	m_pStatusCom.lock()->Set_HP_Full();

	GET_SINGLE(CEffect_Manager)->Create_Effect<CParticle>(TEXT("Player_Heal"), m_pActor.lock());

	m_pGameInstance->Play_Sound(TEXT("FF7_Resident_UI"), L"032_SE_UI (UI032_PartyJoin).wav", ESoundGroup::UI, 1.f);

	return S_OK;
}

void CState_Aerith_Item_Potion::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
	
	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		m_pStateMachineCom.lock()->Enter_State<CState_Aerith_Idle>();
}

void CState_Aerith_Item_Potion::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Aerith_Item_Potion::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Aerith_Item_Potion::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_Aerith_Item_Potion::isValid_NextState(CState* state)
{
	return true;
}

void CState_Aerith_Item_Potion::Free()
{
}
