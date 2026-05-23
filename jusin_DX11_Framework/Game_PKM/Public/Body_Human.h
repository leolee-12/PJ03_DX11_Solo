#pragma once
#include "Body.h"
#include "RenderProfile.h"

NS_BEGIN(Game_PKM)

class CBody_Human final : public CBody
{
public:
	struct BODY_HUMAN_DESC : public CBody::BODY_DESC
	{
		const CRenderRule* pRenderRule = { nullptr }; // weak
	};

private:
	CBody_Human(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Human(const CBody_Human& Prototype);
	virtual ~CBody_Human() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderProfile m_RenderProfile;

	_uint m_iCurrAnim = {};

private:
	virtual _matrix Resolve_ShaderWorldMatrix() const override;

public:
	static CBody_Human* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

private:
	virtual void Free() override;
};

NS_END