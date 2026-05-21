#pragma once
#include "Interaction.h"

NS_BEGIN(Game_PKM)

class CInteraction_EventSequence final : public CInteraction
{
public:
	struct INTERACTION_EVENT_SEQUENCE_DESC
	{
		_wstring strSequenceID;
		INTERACTION_EVENT eTrigger = { INTERACTION_EVENT::TALK };
		_int iPriority = { 150 };
	};

private:
	CInteraction_EventSequence(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInteraction_EventSequence(const CInteraction_EventSequence& Prototype);
	virtual ~CInteraction_EventSequence() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual _bool Supports(INTERACTION_EVENT eEvent) const override;
	virtual _bool CanInteract(const INTERACTION_CONTEXT& ctx) const override;
	virtual void Execute(const INTERACTION_CONTEXT& ctx) override;
	virtual _int Get_Priority(const INTERACTION_CONTEXT& ctx) const override;

private:
	_wstring m_strSequenceID;
	INTERACTION_EVENT m_eTrigger = { INTERACTION_EVENT::TALK };
	_int m_iPriority = { 150 };

public:
	static CInteraction_EventSequence* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;

private:
	virtual void Free() override;
};

NS_END