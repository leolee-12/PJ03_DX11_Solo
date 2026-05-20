#pragma once
#include "Body.h"
#include "RenderProfile.h"

NS_BEGIN(Game_PKM)

class CBody_Pokemon final : public CBody
{
public:
	struct BODY_POKEMON_DESC : public CBody::BODY_DESC
	{
		const CRenderRule* pRenderRule = { nullptr }; // weak
	};

private:
	CBody_Pokemon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Pokemon(const CBody_Pokemon& Prototype);
	virtual ~CBody_Pokemon() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderProfile m_RenderProfile;
	_uint m_iCurrAnim{};

public:
	static CBody_Pokemon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

private:
	virtual void Free() override;
};

NS_END