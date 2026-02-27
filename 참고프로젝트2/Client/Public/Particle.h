#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CParticle : public CGameObject
{
public:
	struct PARTICLE_DESC
	{
		_float4 vColor{};
		_uint iPassIndex{};
		_float fGlowFactor{};
		_float fXScale{};
		_float fYScale{};
	};

protected:
	CParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle(const CParticle& Prototype);
	virtual ~CParticle() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Camera_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;

	void Set_Position(_float3 position);
	void Set_Position(_vector position);

	virtual void Set_Particle_Active(_bool isActive);

protected:
	_float m_fParticle_XScale = {};
	_float m_fParticle_YScale = {};
	_float m_fGlow_Factor = {};
	_uint m_iPassIndex = {};
	_float4 m_vColor = {};

public:
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END