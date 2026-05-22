#pragma once
#include "Game_PKM_Defines.h"
#include "ContainerObject.h"
#include "Interaction.h"

NS_BEGIN(Game_PKM)

class CBody;

class CActor abstract : public CContainerObject
{
protected:
	CActor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CActor(const CActor& Prototype);
	virtual ~CActor() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual void Tick_Movement(_float fTimeDelta);

	_bool CanInteract(const INTERACTION_CONTEXT& ctx) const;
	_bool TryInteract(const INTERACTION_CONTEXT& ctx);
	CBody* Get_Body() const { return m_pBody; }

protected:
	CBody* m_pBody = { nullptr };
	vector<CInteraction*> m_Interactions;

protected:
	void Rebuild_InteractionCache();

protected:
	virtual void Free() override;
};

NS_END