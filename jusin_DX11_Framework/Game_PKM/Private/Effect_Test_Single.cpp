#include "Effect_Test_Single.h"
#include "GameInstance.h"

CEffect_Test_Single::CEffect_Test_Single(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CEffect_Test_Single::CEffect_Test_Single(const CEffect_Test_Single& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CEffect_Test_Single::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Test_Single::Initialize(void* pArg)
{
	if (nullptr != pArg)
		m_tDesc = *static_cast<DESC*>(pArg);

	if (FAILED(__super::Initialize(&m_tDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Reset_Particle();
	Sync_Transform_ToParticle();

	return S_OK;
}

void CEffect_Test_Single::Priority_Update(_float fTimeDelta)
{
}

void CEffect_Test_Single::Update(_float fTimeDelta)
{
	if (Is_Dead())
		return;

	Update_Particle(fTimeDelta);
	Sync_Transform_ToParticle();

	if (m_Particle.fAge >= m_Particle.fLifeTime)
		Set_Dead();
}

void CEffect_Test_Single::Late_Update(_float fTimeDelta)
{
	if (Is_Dead())
		return;

	m_pGameInstance->Add_RenderGroup(RENDERID::BLEND, this);
}

HRESULT CEffect_Test_Single::Render()
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

HRESULT CEffect_Test_Single::Ready_Components()
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

HRESULT CEffect_Test_Single::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(D3DTS::VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(D3DTS::PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_Particle.vColor, sizeof(_float4))))
		return E_FAIL;

	_float fAlpha = 1.f;
	if (m_Particle.fLifeTime > 0.f)
	{
		const _float fRatio = min(m_Particle.fAge / m_Particle.fLifeTime, 1.f);
		constexpr _float fFadeStartRatio = 0.8f;

		if (fRatio > fFadeStartRatio)
			fAlpha = 1.f - min((fRatio - fFadeStartRatio) / (1.f - fFadeStartRatio), 1.f);
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &fAlpha, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

void CEffect_Test_Single::Reset_Particle()
{
	m_Particle.vPosition = _float3(0.f, 0.f, 0.f);
	m_Particle.vVelocity = m_tDesc.vInitVelocity;
	m_Particle.vAcceleration = m_tDesc.vAcceleration;
	m_Particle.fSize = m_tDesc.fSize;
	m_Particle.fAge = 0.f;
	m_Particle.fLifeTime = max(m_tDesc.fLifeTime, 0.001f);
	m_Particle.vColor = m_tDesc.vColor;
}

void CEffect_Test_Single::Update_Particle(_float fTimeDelta)
{
	m_Particle.vVelocity.x += m_Particle.vAcceleration.x * fTimeDelta;
	m_Particle.vVelocity.y += m_Particle.vAcceleration.y * fTimeDelta;
	m_Particle.vVelocity.z += m_Particle.vAcceleration.z * fTimeDelta;

	m_Particle.vPosition.x += m_Particle.vVelocity.x * fTimeDelta;
	m_Particle.vPosition.y += m_Particle.vVelocity.y * fTimeDelta;
	m_Particle.vPosition.z += m_Particle.vVelocity.z * fTimeDelta;

	m_Particle.fAge += fTimeDelta;
}

void CEffect_Test_Single::Sync_Transform_ToParticle()
{
	m_pTransformCom->ScaleTo(m_Particle.fSize, m_Particle.fSize, 1.f);

	m_pTransformCom->Set_State(STATE::POSITION,
		XMVectorSet(
			m_tDesc.vSpawnPos.x + m_Particle.vPosition.x,
			m_tDesc.vSpawnPos.y + m_Particle.vPosition.y,
			m_tDesc.vSpawnPos.z + m_Particle.vPosition.z,
			1.f));
}

CEffect_Test_Single* CEffect_Test_Single::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_Test_Single* pInstance = new CEffect_Test_Single(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEffect_Test_Single");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Test_Single::Clone(void* pArg)
{
	CEffect_Test_Single* pInstance = new CEffect_Test_Single(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEffect_Test_Single");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Test_Single::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}