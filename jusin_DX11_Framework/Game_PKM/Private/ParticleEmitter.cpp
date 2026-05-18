#include "ParticleEmitter.h"
#include "GameInstance.h"

CParticleEmitter::CParticleEmitter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CParticleEmitter::CParticleEmitter(const CParticleEmitter& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CParticleEmitter::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParticleEmitter::Initialize(void* pArg)
{
	if (nullptr != pArg)
		m_tDesc = *static_cast<EMITTER_DESC*>(pArg);

	if (FAILED(__super::Initialize(&m_tDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_tDesc.iCapacity = max(1u, m_tDesc.iCapacity);
	m_Particles.resize(m_tDesc.iCapacity);
	m_iAliveCount = 0;
	m_fSpawnAccumulator = 0.f;
	m_bEmitting = true;
	m_bBurstSpawned = false;

	m_pTransformCom->Set_State(STATE::POSITION,
		XMVectorSet(m_tDesc.vSpawnPos.x, m_tDesc.vSpawnPos.y, m_tDesc.vSpawnPos.z, 1.f));

	return S_OK;
}

void CParticleEmitter::Priority_Update(_float fTimeDelta)
{
}

void CParticleEmitter::Update(_float fTimeDelta)
{
	if (Is_Dead())
		return;

	if (m_bEmitting)
	{
		Spawn_Burst_Once();
		Spawn_FromAccumulator(fTimeDelta);
	}

	Update_Particles(fTimeDelta);
}

void CParticleEmitter::Late_Update(_float fTimeDelta)
{
	if (Is_Dead())
		return;

	if (0 == m_iAliveCount)
		return;

	m_pGameInstance->Add_RenderGroup(RENDERID::BLEND, this);
}

HRESULT CParticleEmitter::Render()
{
	if (FAILED(Bind_ShaderGlobals()))
		return E_FAIL;

	for (_uint i = 0; i < m_iAliveCount; ++i)
	{
		if (FAILED(Render_Particle(m_Particles[i])))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CParticleEmitter::Ready_Components()
{
	if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), PROTO_COM_TEX_DUMMY_WHITE,
		COM_TEXTURE, reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), PROTO_COM_SHADER_EFFECT_M1,
		COM_SHADER, reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), PROTO_COM_VIBUFFER_RECT,
		COM_VIBUFFER, reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticleEmitter::Bind_ShaderGlobals()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(D3DTS::VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(D3DTS::PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticleEmitter::Render_Particle(const CParticle& Particle)
{
	_vector vRootPos = m_pTransformCom->Get_State(STATE::POSITION);
	_vector vLocalPos = XMLoadFloat3(&Particle.vPosition);
	_vector vWorldPos = vRootPos + XMVectorSetW(vLocalPos, 0.f);

	_matrix matWorld =
		XMMatrixScaling(Particle.fSize, Particle.fSize, 1.f) *
		XMMatrixTranslationFromVector(vWorldPos);

	_float4x4 WorldMatrix{};
	XMStoreFloat4x4(&WorldMatrix, matWorld);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &Particle.vColor, sizeof(_float4))))
		return E_FAIL;

	_float fAlpha = 1.f;
	if (Particle.fLifeTime > 0.f)
	{
		const _float fRatio = min(Particle.fAge / Particle.fLifeTime, 1.f);
		constexpr _float fFadeStartRatio = 0.8f;

		if (fRatio > fFadeStartRatio)
			fAlpha = 1.f - min((fRatio - fFadeStartRatio) / (1.f - fFadeStartRatio), 1.f);
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CParticleEmitter::Spawn_Burst_Once()
{
	if (m_bBurstSpawned)
		return;

	m_bBurstSpawned = true;

	for (_uint i = 0; i < m_tDesc.iBurstCount; ++i)
		Spawn_One();
}

void CParticleEmitter::Spawn_FromAccumulator(_float fTimeDelta)
{
	if (m_tDesc.fSpawnRate <= 0.f)
		return;

	m_fSpawnAccumulator += m_tDesc.fSpawnRate * fTimeDelta;

	const _uint iSpawnCount = static_cast<_uint>(m_fSpawnAccumulator);
	if (0 == iSpawnCount)
		return;

	m_fSpawnAccumulator -= static_cast<_float>(iSpawnCount);

	for (_uint i = 0; i < iSpawnCount; ++i)
		Spawn_One();
}

void CParticleEmitter::Spawn_One()
{
	if (m_iAliveCount >= m_tDesc.iCapacity)
	{
		if (EMITTER_DESC::SPAWN_OVERFLOW_POLICY::DROP_NEW == m_tDesc.eOverflow)
			return;

		_uint iOldest = 0;
		for (_uint i = 1; i < m_iAliveCount; ++i)
		{
			if (m_Particles[i].fAge > m_Particles[iOldest].fAge)
				iOldest = i;
		}

		Kill_AtIndex(iOldest);
	}

	CParticle& Particle = m_Particles[m_iAliveCount];

	Particle.vPosition = _float3(0.f, 0.f, 0.f);
	Particle.vVelocity = Make_RandomVelocity();
	Particle.vAcceleration = _float3(0.f, 0.f, 0.f);

	Particle.fSize = m_pGameInstance->Random(m_tDesc.vSizeRange.x, m_tDesc.vSizeRange.y);
	Particle.fAge = 0.f;
	Particle.fLifeTime = max(m_pGameInstance->Random(m_tDesc.vLifeTimeRange.x, m_tDesc.vLifeTimeRange.y), 0.001f);
	Particle.fRotation = 0.f;
	Particle.fRotationSpeed = 0.f;
	Particle.fRandomSeed = m_pGameInstance->Random(0.f, 1.f);
	Particle.vColor = _float4(1.f, 1.f, 0.f, 1.f);

	++m_iAliveCount;
}

void CParticleEmitter::Kill_AtIndex(_uint iIndex)
{
	if (iIndex >= m_iAliveCount)
		return;

	if (iIndex != m_iAliveCount - 1)
		std::swap(m_Particles[iIndex], m_Particles[m_iAliveCount - 1]);

	--m_iAliveCount;
}

void CParticleEmitter::Update_Particles(_float fTimeDelta)
{
	for (_uint i = 0; i < m_iAliveCount;)
	{
		CParticle& Particle = m_Particles[i];

		Particle.vVelocity.x += Particle.vAcceleration.x * fTimeDelta;
		Particle.vVelocity.y += Particle.vAcceleration.y * fTimeDelta;
		Particle.vVelocity.z += Particle.vAcceleration.z * fTimeDelta;

		Particle.vPosition.x += Particle.vVelocity.x * fTimeDelta;
		Particle.vPosition.y += Particle.vVelocity.y * fTimeDelta;
		Particle.vPosition.z += Particle.vVelocity.z * fTimeDelta;

		Particle.fAge += fTimeDelta;

		if (Particle.fAge >= Particle.fLifeTime)
		{
			Kill_AtIndex(i);
			continue;
		}

		++i;
	}
}

_float3 CParticleEmitter::Make_RandomVelocity() const
{
	_float fSpeed = m_pGameInstance->Random(m_tDesc.vSpeedRange.x, m_tDesc.vSpeedRange.y);

	_vector vBaseDir = XMLoadFloat3(&m_tDesc.vEmitDirection);
	if (XMVectorGetX(XMVector3LengthSq(vBaseDir)) <= 0.000001f)
		vBaseDir = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	vBaseDir = XMVector3Normalize(vBaseDir);

	if (m_tDesc.fEmitConeHalfAngle <= 0.0001f)
	{
		_float3 vVelocity{};
		XMStoreFloat3(&vVelocity, vBaseDir * fSpeed);
		return vVelocity;
	}

	_vector vTempUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	if (fabsf(XMVectorGetX(XMVector3Dot(vBaseDir, vTempUp))) > 0.95f)
		vTempUp = XMVectorSet(1.f, 0.f, 0.f, 0.f);

	_vector vRight = XMVector3Normalize(XMVector3Cross(vTempUp, vBaseDir));
	_vector vUp = XMVector3Normalize(XMVector3Cross(vBaseDir, vRight));

	const _float fTheta = m_pGameInstance->Random(0.f, m_tDesc.fEmitConeHalfAngle);
	const _float fPhi = m_pGameInstance->Random(0.f, XM_2PI);

	const _float fSinTheta = sinf(fTheta);
	const _float fCosTheta = cosf(fTheta);
	const _float fCosPhi = cosf(fPhi);
	const _float fSinPhi = sinf(fPhi);

	_vector vDir =
		vBaseDir * fCosTheta +
		vRight * (fSinTheta * fCosPhi) +
		vUp * (fSinTheta * fSinPhi);

	_float3 vVelocity{};
	XMStoreFloat3(&vVelocity, XMVector3Normalize(vDir) * fSpeed);
	return vVelocity;
}

CParticleEmitter* CParticleEmitter::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CParticleEmitter* pInstance = new CParticleEmitter(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CParticleEmitter");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CParticleEmitter::Clone(void* pArg)
{
	CParticleEmitter* pInstance = new CParticleEmitter(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CParticleEmitter");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticleEmitter::Free()
{
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);

	__super::Free();
}