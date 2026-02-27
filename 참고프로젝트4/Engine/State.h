#pragma once

//#ifndef CSTATE_H
//#define CSTATE_H

#include "Base.h"
#include "BrainTree.h"
using namespace BrainTree;

BEGIN(Engine)

class ENGINE_DLL CState : public CBase
{
	INFO_CLASS(CState, CBase)

public:
	explicit CState(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine);
	virtual ~CState()= default;

public:
	virtual HRESULT			Initialize_State(CState* pPreviousState)	PURE;
	virtual void			Priority_Tick(_cref_time fTimeDelta)		PURE;
	virtual void			Tick(_cref_time fTimeDelta)					PURE;
	virtual void			Late_Tick(_cref_time fTimeDelta)			PURE;
	virtual void			Transition_State(CState* pNextState)		PURE;
	virtual bool			isValid_NextState(CState* state)			PURE;

public:
	template<typename T = class CGameObject>
	shared_ptr<T> Actor();

public:
	class CGameInstance* m_pGameInstance;
	weak_ptr<class CStateMachine>						m_pStateMachineCom;
	weak_ptr<class CGameObject>							m_pActor;
	weak_ptr<class CTransform>							m_pActor_TransformCom;
	weak_ptr<class CCommonModelComp>					m_pActor_ModelCom;
		
	Node::Ptr											m_pBehaviorTree = { nullptr };
	
protected:
	virtual void		Free();
};




END

template<typename T>
shared_ptr<T> CState::Actor()
{
	if (m_pActor.expired())
		return nullptr;

	return dynamic_pointer_cast<T>(m_pActor.lock());
}

//#endif


// TODO: Behavior Tree ¿¹½Ã
//
//void CreatingBehaviorTreeUsingBuilders()
//{
//    auto tree = BrainTree::Builder(m_pActor)
//        .composite<BrainTree::Sequence>()
//        .leaf<Action>()
//        .leaf<Action>()
//        .end()
//        .build();
//    tree->update();
//}
