#pragma once
#include "Body.h"

NS_BEGIN(Game_PKM)

class CBody_BattleBasicAnim final : public CBody
{
private:
	CBody_BattleBasicAnim(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_BattleBasicAnim(const CBody_BattleBasicAnim& Prototype);
	virtual ~CBody_BattleBasicAnim() = default;

public:
	virtual HRESULT Render() override;

public:
	static CBody_BattleBasicAnim* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

private:
	virtual void Free() override;
};

NS_END