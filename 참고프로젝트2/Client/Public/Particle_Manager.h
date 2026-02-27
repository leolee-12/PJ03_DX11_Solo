#pragma once

#include "Base.h"
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CGameInstance;
class CGameObject;
END

BEGIN(Client)

class CParticle_Manager : public CBase
{
	DECLARE_SINGLETON(CParticle_Manager)

public:
	enum PARTICLE_ID
	{
		FREIZA_ULTIMATE_3_PARTICLE,
		FREIZA_ULTIMATE_1_PARTICLE,
		FREIZA_ULTIMATE_1_HIT_PARTICLE,
		FREIZA_ULTIMATE_3_HIT_PARTICLE,
		ULTIMATE_3_21_PARTICLE,
		COMMON_HIT_PARTICLE,
		PARTICLE_END
	};

private:
	CParticle_Manager();
	virtual ~CParticle_Manager() = default;

public:
	virtual HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Camera_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render(_float fTimeDelta);

	HRESULT Play(PARTICLE_ID eID, const _float3& vPosition);
	HRESULT Play(PARTICLE_ID eID, const _vector& vPosition);
	void Stop(PARTICLE_ID eID);

	CGameInstance* m_pGameInstance = { nullptr };

	unordered_map<PARTICLE_ID, vector<class CParticle*>> m_ParticlePools;
public:
	virtual void Free() override;
};

END