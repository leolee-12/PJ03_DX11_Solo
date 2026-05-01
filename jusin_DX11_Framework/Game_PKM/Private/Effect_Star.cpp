#include "Effect_Star.h"
#include "VIBuffer_UI_Instance.h"

#include "GameInstance.h"

CEffect_Star::CEffect_Star(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{

}

CEffect_Star::CEffect_Star(const CEffect_Star& Prototype)
	: CUIObject{ Prototype }
{

}

HRESULT CEffect_Star::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Star::Initialize(void* pArg)
{
	if (nullptr != pArg)
		m_tDesc = *static_cast<EFFECT_STAR_DESC*>(pArg);

	m_tDesc.bVisible = m_tDesc.bStartActive;
	m_isActive = m_tDesc.bStartActive;

	if (FAILED(__super::Initialize(&m_tDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (m_tDesc.iNumParticles > m_pVIBufferCom->Get_MaxInstances())
		return E_FAIL;

	m_Particles.resize(m_tDesc.iNumParticles);
	m_RenderInstances.reserve(m_tDesc.iNumParticles);

	Reset_AllParticles();

	return Upload_RenderInstances();
}

void CEffect_Star::Priority_Update(_float fTimeDelta)
{

}

void CEffect_Star::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (false == m_isActive)
		return;

	Update_FollowTarget();
	Update_Particles(fTimeDelta);
	Build_RenderInstances();
	Upload_RenderInstances();
}

void CEffect_Star::Late_Update(_float fTimeDelta)
{
	if (false == m_isActive) return;

	m_pGameInstance->Add_RenderGroup(RENDERID::UI, this);
}

HRESULT CEffect_Star::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;

}

void CEffect_Star::Play(CUIObject* pFollowTarget, const _float2& vFollowOffset)
{
	m_tDesc.pFollowTarget = pFollowTarget;
	m_tDesc.vFollowOffset = vFollowOffset;
	m_isActive = true;
	m_bVisible = true;

	Reset_AllParticles();
	Build_RenderInstances();
	Upload_RenderInstances();
}

void CEffect_Star::Stop()
{
	m_isActive = false;
	m_bVisible = false;
	m_RenderInstances.clear();
	Upload_RenderInstances();
}

HRESULT CEffect_Star::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEXTURE_STAR,
		COM_TEXTURE, reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_SHADER_VTXUIINST,
		COM_SHADER, reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_VIBUFFER_INST_STAR,
		COM_VIBUFFER, reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Star::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))	// UI¿ë
		return E_FAIL;
	if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))	// UI¿ë
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_TexDiff", m_tDesc.iStarTextureIndex)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_TexMask", m_tDesc.iMaskTextureIndex)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_TexSub", m_tDesc.iDiamondTextureIndex)))
		return E_FAIL;

	return S_OK;
}

void CEffect_Star::Initialize_Particle(_uint iParticleIndex)
{
	PARTICLE_UI_STATE& Particle = m_Particles[iParticleIndex];

	const _float2 vHalfRange = { m_tDesc.vSpawnRange.x * 0.5f, m_tDesc.vSpawnRange.y * 0.5f };
	Particle.vOffset = _float2(m_pGameInstance->Random(-vHalfRange.x, vHalfRange.x), m_pGameInstance->Random(-vHalfRange.y, vHalfRange.y));

	const _float fSize = m_pGameInstance->Random(m_tDesc.vSizeRange.x, m_tDesc.vSizeRange.y);
	Particle.vSize = _float2(fSize, fSize);

	const _float fSpeed = m_pGameInstance->Random(m_tDesc.vSpeedRange.x, m_tDesc.vSpeedRange.y);
	const _float fAngle = m_pGameInstance->Random(0.f, XM_2PI);
	Particle.vVelocity = _float2(cosf(fAngle) * fSpeed, sinf(fAngle) * fSpeed);

	Particle.fAge = 0.f;
	Particle.fLifeTime = m_pGameInstance->Random(m_tDesc.vLifeRange.x, m_tDesc.vLifeRange.y);
	Particle.fRotation = m_pGameInstance->Random(0.f, XM_2PI);
	Particle.fRotationSpeed = m_pGameInstance->Random(m_tDesc.vRotationSpeedRange.x, m_tDesc.vRotationSpeedRange.y);
	Particle.fMaskRotation = m_pGameInstance->Random(0.f, XM_2PI);
	Particle.fMaskRotationSpeed = m_pGameInstance->Random(m_tDesc.vMaskRotationSpeedRange.x, m_tDesc.vMaskRotationSpeedRange.y);
	Particle.fAlpha = 1.f;

	Particle.iTextureIndex = m_tDesc.iStarTextureIndex;
	Particle.iFrameIndex = 0;
	Particle.eAtlasLayout = TEXTURE_SAMPLE_MODE::SINGLE;

	if (0 != m_tDesc.iDiamondTextureIndex && 0 == (iParticleIndex % 5))
	{
		Particle.iTextureIndex = m_tDesc.iDiamondTextureIndex;
		Particle.iFrameIndex = iParticleIndex % 4;
		Particle.eAtlasLayout = TEXTURE_SAMPLE_MODE::QUAD;
	}

	Particle.isAlive = true;
}

void CEffect_Star::Reset_AllParticles()
{
	_uint iCount = static_cast<_uint>(m_Particles.size());
	for (_uint i = 0; i < iCount; ++i)
		Initialize_Particle(i);

	Build_RenderInstances();
}

void CEffect_Star::Update_FollowTarget()
{
	if (nullptr == m_tDesc.pFollowTarget)
		return;

	const _float2 vTargetCenter = m_tDesc.pFollowTarget->Get_Center();

	Set_Center(
		vTargetCenter.x + m_tDesc.vFollowOffset.x,
		vTargetCenter.y + m_tDesc.vFollowOffset.y);
}

void CEffect_Star::Update_Particles(_float fTimeDelta)
{
	_bool hasAlive = false;

	for (_uint i = 0; i < static_cast<_uint>(m_Particles.size()); ++i)
	{
		PARTICLE_UI_STATE& Particle = m_Particles[i];

		if (false == Particle.isAlive)
			continue;

		Particle.vOffset.x += Particle.vVelocity.x * fTimeDelta;
		Particle.vOffset.y += Particle.vVelocity.y * fTimeDelta;
		Particle.fRotation += Particle.fRotationSpeed * fTimeDelta;
		Particle.fMaskRotation += Particle.fMaskRotationSpeed * fTimeDelta;
		Particle.fAge += fTimeDelta;

		const _float fRatio = (Particle.fLifeTime > 0.f) ? min(Particle.fAge / Particle.fLifeTime, 1.f) : 1.f;
		Particle.fAlpha = 1.f - fRatio;

		if (Particle.fAge >= Particle.fLifeTime)
		{
			if (true == m_tDesc.bLoop)
				Initialize_Particle(i);
			else
				Particle.isAlive = false;
		}

		if (true == Particle.isAlive)
			hasAlive = true;
	}

	if (false == hasAlive && false == m_tDesc.bLoop)
	{
		m_isActive = false;
		m_bVisible = false;
	}
}

void CEffect_Star::Build_RenderInstances()
{
	m_RenderInstances.clear();

	for (const PARTICLE_UI_STATE& Particle : m_Particles)
	{
		if (false == Particle.isAlive)
			continue;

		const _float c = cosf(Particle.fRotation);
		const _float s = sinf(Particle.fRotation);

		VTXUI_INSTANCE Instance{};
		Instance.vRight = _float4(c * Particle.vSize.x, s * Particle.vSize.x, 0.f, 0.f);
		Instance.vUp = _float4(-s * Particle.vSize.y, c * Particle.vSize.y, 0.f, 0.f);
		Instance.vTranslation = _float4(Particle.vOffset.x, -Particle.vOffset.y, 0.f, 1.f);
		Instance.vColor = _float4(m_tDesc.vColor.x, m_tDesc.vColor.y, m_tDesc.vColor.z, m_tDesc.vColor.w * Particle.fAlpha);

		const _float fMaskC = cosf(Particle.fMaskRotation);
		const _float fMaskS = sinf(Particle.fMaskRotation);
		Instance.vUVTransform = _float4(1.f, 1.f, 0.f, 0.f);
		Instance.vMaskUVTransform = _float4(fMaskC, fMaskS, -fMaskS, fMaskC);

		const _bool bUseSub = (TEXTURE_SAMPLE_MODE::QUAD == Particle.eAtlasLayout);
		const _bool bUseMask = true;

		const _float fBaseSampleMode = bUseSub ? 2.f : 0.f;
		const _float fMaskSampleMode = static_cast<_float>(ETOUI(m_tDesc.eMaskSampleMode));
		const _float fRenderMode = bUseSub ? 3.f : 1.f;

		Instance.vParams = _float4(
			m_tDesc.fMaskStrength,
			fBaseSampleMode,
			fMaskSampleMode,
			fRenderMode);

		m_RenderInstances.emplace_back(Instance);
	}
}

HRESULT CEffect_Star::Upload_RenderInstances()
{
	const _uint iNumRenderInstances = static_cast<_uint>(m_RenderInstances.size());
	const VTXUI_INSTANCE* pInstances = (0 == iNumRenderInstances ? nullptr : m_RenderInstances.data());
	return m_pVIBufferCom->Update_UIInstances(pInstances, iNumRenderInstances);
}


CEffect_Star* CEffect_Star::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_Star* pInstance = new CEffect_Star(pDevice, pContext);

 	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEffect_Star");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Star::Clone(void* pArg)
{
	CEffect_Star* pInstance = new CEffect_Star(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEffect_Star");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Star::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
