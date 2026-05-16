#pragma once
#include "Interaction.h"

NS_BEGIN(Game_PKM)

class CInteraction_Encounter final : public CInteraction
{
public:
	struct INTERACTION_ENCOUNTER_DESC
	{
		_uint iSpeciesID = { 0 };
		_uint iLevel = { 1 };
	};

private:
	CInteraction_Encounter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInteraction_Encounter(const CInteraction_Encounter& Prototype);
	virtual ~CInteraction_Encounter() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual _bool Supports(INTERACTION_EVENT eEvent) const override;
	virtual _bool CanInteract(const INTERACTION_CONTEXT& ctx) const override;
	virtual void Execute(const INTERACTION_CONTEXT& ctx) override;

private:
	_uint m_iSpeciesID = { 0 };
	_uint m_iLevel = { 1 };

public:
	static CInteraction_Encounter* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;

private:
	virtual void Free() override;
};

NS_END