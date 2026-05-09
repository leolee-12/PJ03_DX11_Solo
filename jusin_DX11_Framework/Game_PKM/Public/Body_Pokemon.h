#pragma once
#include "Body.h"

NS_BEGIN(Game_PKM)

class CBody_Pokemon final : public CBody
{
public:
	using BODY_POKEMON_DESC = CBody::BODY_DESC;

private:
	CBody_Pokemon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Pokemon(const CBody_Pokemon& Prototype);
	virtual ~CBody_Pokemon() = default;

public:
	virtual HRESULT Render() override;

public:
	static CBody_Pokemon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

private:
	virtual void Free() override;
};

NS_END