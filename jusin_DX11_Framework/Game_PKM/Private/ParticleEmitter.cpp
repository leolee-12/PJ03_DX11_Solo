#include "ParticleEmitter.h"
#include "VIBuffer_Particle3D_Instance.h"

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
	
	m_pParentTransform = m_tDesc.pParentTransform;

	if (FAILED(__super::Initialize(&m_tDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_tDesc.iCapacity = max(1u, m_tDesc.iCapacity);
	m_Particles.resize(m_tDesc.iCapacity);
	m_InstanceScratch.reserve(m_tDesc.iCapacity);
	m_iAliveCount = 0;
	m_fSpawnAccumulator = 0.f;
	m_bEmitting = true;
	m_bBurstSpawned = false;

	if (m_tDesc.curveAlpha.IsEmpty())
	{
		m_tDesc.curveAlpha.Add_Key(0.0f, 1.f);
		m_tDesc.curveAlpha.Add_Key(0.9f, 1.f);
		m_tDesc.curveAlpha.Add_Key(1.0f, 0.f);
	}

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

	Build_Instances();

	m_pGameInstance->Add_RenderGroup(RENDERID::BLEND, this);
}

HRESULT CParticleEmitter::Render()
{
	if (0 == m_iAliveCount)
		return S_OK;

	if (FAILED(Bind_ShaderGlobals()))
		return E_FAIL;

	const _uint iPass = (EMITTER_DESC::BLEND_MODE::ALPHA == m_tDesc.eBlend) ? 0u : 1u;
	if (FAILED(m_pShaderCom->Begin(iPass)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticleEmitter::Ready_Components()
{
	const WNameID strTextureProtoTag =
		(m_tDesc.strTextureProtoTag != INVALID_TAG)
		? m_tDesc.strTextureProtoTag
		: PROTO_COM_TEX_DUMMY_WHITE;

	if (FAILED(__super::Add_Component(m_tDesc.iTextureProtoLevel, strTextureProtoTag,
		COM_TEXTURE, reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), PROTO_COM_SHADER_PARTICLE3D,
		COM_SHADER, reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), PROTO_COM_VIBUFFER_INST_PARTICLE3D,
		COM_VIBUFFER, reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticleEmitter::Bind_ShaderGlobals()
{
	/* M8: parent (effect root) transform이 있으면 그쪽을 g_WorldMatrix로.
	   없으면 자체 transform (단독 emitter 사용 시 회귀 없음). */
	CTransform* pSrcTransform = (nullptr != m_pParentTransform)
		? m_pParentTransform
		: m_pTransformCom;

	if (FAILED(pSrcTransform->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix",
		m_pGameInstance->Get_Transform(D3DTS::VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix",
		m_pGameInstance->Get_Transform(D3DTS::PROJ))))
		return E_FAIL;

	/* M4: 카메라 inverse view (월드 공간 basis 추출용) */
	const _float4x4* pViewInv = m_pGameInstance->Get_Transform_Inverse(D3DTS::VIEW);
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewInvMatrix", pViewInv)))
		return E_FAIL;

	/* M4: 빌보드 모드 + 고정축 */
	const _uint iBillboardMode = static_cast<_uint>(m_tDesc.eBillboard);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iBillboardMode", &iBillboardMode, sizeof(_uint))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vFixedAxis",
		&m_tDesc.vBillboardFixedAxis, sizeof(_float3))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	return S_OK;
}

void CParticleEmitter::Build_Instances()
{
	m_InstanceScratch.resize(m_iAliveCount);

	for (_uint i = 0; i < m_iAliveCount; ++i)
	{
		const CParticle& p = m_Particles[i];
		VTXPARTICLE3D_INSTANCE& v = m_InstanceScratch[i];

		v.vCenter = p.vPosition;       // emitter local
		v.fSize = p.fSize;
		v.fRotation = p.fRotation;
		v._pad0 = _float3(0.f, 0.f, 0.f);

		v.vColor = p.vColor;
		v.vAgeLife = _float2(p.fAge, p.fLifeTime);
		v._pad1 = _float2(0.f, 0.f);

		v.vAtlasUV = _float4(0.f, 0.f, 1.f, 1.f);   // M6+ 에서 의미 부여
	}

	m_pVIBufferCom->Update_Particle3D_Instances(m_InstanceScratch.data(), m_iAliveCount);
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

		/* M6: 커브 sample (t01 = age/life). 빈 커브는 skip — 입자 초기값 유지. */
		const _float fLife = max(Particle.fLifeTime, 0.0001f);
		const _float t01 = min(Particle.fAge / fLife, 1.f);

		if (!m_tDesc.curveSize.IsEmpty())
			Particle.fSize = m_tDesc.curveSize.Sample(t01);

		if (!m_tDesc.curveColor.IsEmpty())
			Particle.vColor = m_tDesc.curveColor.Sample(t01);

		if (!m_tDesc.curveAlpha.IsEmpty())
			Particle.vColor.w *= m_tDesc.curveAlpha.Sample(t01);

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