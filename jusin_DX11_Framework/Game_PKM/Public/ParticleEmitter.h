#pragma once
#include "Game_PKM_Defines.h"
#include "GameObject.h"
#include "Particle.h"
#include "ParticleCurve.h"

NS_BEGIN(Engine)
class CTexture;
class CShader;
NS_END

NS_BEGIN(Game_PKM)
class CVIBuffer_Particle3D_Instance;

class CParticleEmitter final : public CGameObject
{
public:
	struct EMITTER_DESC final : public CGameObject::GAMEOBJECT_DESC
	{
		_uint   iCapacity = 256;
		_float  fSpawnRate = 50.f;
		_uint   iBurstCount = 0;

		_float2 vLifeTimeRange = { 1.f, 2.f };
		_float2 vSpeedRange = { 1.f, 3.f };
		_float2 vSizeRange = { 0.2f, 0.5f };

		_float3 vEmitDirection = { 0.f, 1.f, 0.f };
		_float  fEmitConeHalfAngle = 0.f;

		enum class BILLBOARD_MODE { VIEW_ALIGNED, AXIS_LOCKED, FIXED_NORMAL };
		BILLBOARD_MODE eBillboard = BILLBOARD_MODE::VIEW_ALIGNED;
		_float3        vBillboardFixedAxis = { 0.f, 1.f, 0.f };

		CCurveFloat  curveSize;
		CCurveColor  curveColor;
		CCurveFloat  curveAlpha;

		enum class BLEND_MODE { ALPHA, ADDITIVE };
		BLEND_MODE eBlend = BLEND_MODE::ADDITIVE;

		enum class SPAWN_OVERFLOW_POLICY { DROP_NEW, DROP_OLDEST };
		SPAWN_OVERFLOW_POLICY eOverflow = SPAWN_OVERFLOW_POLICY::DROP_NEW;

		_bool bSimulateInLocalAtSpawn = false;
		WNameID strTextureProtoTag = PROTO_COM_TEX_DUMMY_WHITE;
		_uint   iTextureProtoLevel = ETOUI(LEVEL::STATIC);
		class CTransform* pParentTransform = nullptr;  // M8: effect root transform °øÀ¯ (nullable)
	};

private:
	CParticleEmitter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticleEmitter(const CParticleEmitter& Prototype);
	virtual ~CParticleEmitter() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_Emitting(_bool bEmitting) { m_bEmitting = bEmitting; }
	_uint Get_AliveCount() const { return m_iAliveCount; }
	void Clear_All_Particles() { m_iAliveCount = 0; }

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderGlobals();
	void    Build_Instances();

	void Spawn_Burst_Once();
	void Spawn_FromAccumulator(_float fTimeDelta);
	void Spawn_One();
	void Kill_AtIndex(_uint iIndex);
	void Update_Particles(_float fTimeDelta);

	_float3 Make_RandomVelocity() const;

private:
	EMITTER_DESC m_tDesc = {};

	vector<CParticle> m_Particles;
	_uint m_iAliveCount = 0;
	_float m_fSpawnAccumulator = 0.f;

	_bool m_bEmitting = true;
	_bool m_bBurstSpawned = false;

	CTransform* m_pParentTransform = nullptr;
	CTexture* m_pTextureCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Particle3D_Instance* m_pVIBufferCom = { nullptr };
	vector<VTXPARTICLE3D_INSTANCE> m_InstanceScratch;

public:
	static CParticleEmitter* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

private:
	virtual void Free() override;
};

NS_END