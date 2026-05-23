#include "Actor.h"

CActor::CActor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CContainerObject(pDevice, pContext)
{
}

CActor::CActor(const CActor& Prototype)
	: CContainerObject(Prototype)
{
}

void CActor::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	m_PartObjects.for_each([&fTimeDelta](auto& Pair)
		{
			if (nullptr != Pair.second)
				Pair.second->Priority_Update(fTimeDelta);
		});
}

void CActor::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	m_PartObjects.for_each([&fTimeDelta](auto& Pair)
		{
			if (nullptr != Pair.second)
				Pair.second->Update(fTimeDelta);
		});

	Tick_Movement(fTimeDelta);

	for (auto* pInteraction : m_Interactions)
	{
		if (nullptr != pInteraction)
			pInteraction->Tick(fTimeDelta);
	}
}

void CActor::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_PartObjects.for_each([&fTimeDelta](auto& Pair)
		{
			if (nullptr != Pair.second)
				Pair.second->Late_Update(fTimeDelta);
		});
}

void CActor::Tick_Movement(_float fTimeDelta)
{
	Tick_RootMotionMovement(XMVectorZero(), false, _float3{},
		/*pNavigation=*/nullptr, fTimeDelta);
}

_bool CActor::CanInteract(const INTERACTION_CONTEXT& ctx) const
{
	for (auto* pInteraction : m_Interactions)
	{
		if (nullptr == pInteraction)
			continue;

		if (pInteraction->CanInteract(ctx))
			return true;
	}

	return false;
}

_bool CActor::TryInteract(const INTERACTION_CONTEXT& ctx)
{
	OutputDebugStringW((L"[Actor] TryInteract: m_Interactions size = "
		+ std::to_wstring(m_Interactions.size()) + L"\n").c_str());

	CInteraction* pBestInteraction = { nullptr };
	_int iBestPriority = { 0 };

	for (auto* pInteraction : m_Interactions)
	{
		if (nullptr == pInteraction)
			continue;

		if (!pInteraction->CanInteract(ctx))
			continue;

		const _int iPriority = pInteraction->Get_Priority(ctx);

		if (nullptr == pBestInteraction || iPriority > iBestPriority)
		{
			pBestInteraction = pInteraction;
			iBestPriority = iPriority;
		}
	}

	if (nullptr == pBestInteraction)
		return false;

	pBestInteraction->Execute(ctx);
	return true;
}

void CActor::Rebuild_InteractionCache()
{
	m_Interactions.clear();

	m_Components.for_each([this](auto& pair)
		{
			if (CInteraction* pInteraction = dynamic_cast<CInteraction*>(pair.second))
			{
				m_Interactions.push_back(pInteraction);
			}
		});
}

void CActor::Free()
{
	m_pBody = nullptr;
	// m_Interactions / m_pBody는 캐시이므로 별도 release 없음 (소유는 m_Components / m_PartObject)
	m_Interactions.clear();

	__super::Free();
}