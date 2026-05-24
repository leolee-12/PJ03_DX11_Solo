#include "Effect_Mesh.h"

#include "GameInstance.h"

CEffect_Mesh::CEffect_Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CEffect_Mesh::CEffect_Mesh(const CEffect_Mesh& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CEffect_Mesh::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Mesh::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	m_tDesc = *static_cast<MESH_EFFECT_DESC*>(pArg);

	if (FAILED(__super::Initialize(&m_tDesc)))
		return E_FAIL;

	/* model / shader는 필수. CEffect::Initialize 단에서 fallback 채움. */
	if (m_tDesc.strModelProtoTag == INVALID_TAG ||
		m_tDesc.strShaderProtoTag == INVALID_TAG)
		return E_FAIL;

	m_pParentTransform = m_tDesc.pParentTransform;
	m_fAge = 0.f;

	/* alpha curve 빈 경우 자연 fadeout default (CParticleEmitter::Initialize와 동일 패턴) */
	if (m_tDesc.curveAlpha.IsEmpty())
	{
		m_tDesc.curveAlpha.Add_Key(0.0f, 1.f);
		m_tDesc.curveAlpha.Add_Key(0.9f, 1.f);
		m_tDesc.curveAlpha.Add_Key(1.0f, 0.f);
	}

	if (FAILED(Ready_Components()))
		return E_FAIL;

	/* 자체 transform은 origin 유지 — 셰이더 g_WorldMatrix는 parent (effect root) 사용 */
	m_pTransformCom->Set_State(STATE::POSITION,
		XMVectorSet(m_tDesc.vSpawnPos.x, m_tDesc.vSpawnPos.y, m_tDesc.vSpawnPos.z, 1.f));

	/* 모션 / debris 초기화 — 인스턴스마다 독립 랜덤 (분산) */
	m_vLocalPos       = m_tDesc.vStartOffset;
	m_vVelocity       = Make_RandomVelocity();
	m_fDelayRemaining = m_tDesc.fStartDelay;

	if (m_tDesc.fSpinSpeedMax > 0.0001f)
	{
		_float3 vAxis(m_pGameInstance->Random(-1.f, 1.f),
			m_pGameInstance->Random(-1.f, 1.f),
			m_pGameInstance->Random(-1.f, 1.f));
		_vector v = XMLoadFloat3(&vAxis);
		if (XMVectorGetX(XMVector3LengthSq(v)) <= 1e-6f)
			v = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		XMStoreFloat3(&m_vSpinAxis, XMVector3Normalize(v));
		m_fSpinSpeed = m_pGameInstance->Random(-m_tDesc.fSpinSpeedMax, m_tDesc.fSpinSpeedMax);
	}

	return S_OK;
}

void CEffect_Mesh::Priority_Update(_float fTimeDelta)
{
}

void CEffect_Mesh::Update(_float fTimeDelta)
{
	if (Is_Dead())
		return;

	/* startDelay 동안은 등장 전 — age/모션 정지 */
	if (m_fDelayRemaining > 0.f)
	{
		m_fDelayRemaining -= fTimeDelta;
		return;
	}

	m_fAge += fTimeDelta;

	if (m_fAge >= m_tDesc.fLifeTime)
	{
		Set_Dead();
		return;
	}

	/* semi-implicit Euler (입자와 동일 규약) */
	_vector vVel = XMLoadFloat3(&m_vVelocity) + XMLoadFloat3(&m_tDesc.vGravity) * fTimeDelta;
	XMStoreFloat3(&m_vVelocity, vVel);
	XMStoreFloat3(&m_vLocalPos, XMLoadFloat3(&m_vLocalPos) + vVel * fTimeDelta);

	m_fSpinAngle += m_fSpinSpeed * fTimeDelta;
}

void CEffect_Mesh::Late_Update(_float fTimeDelta)
{
	(void)fTimeDelta;

	if (Is_Dead())
		return;

	if (m_fDelayRemaining > 0.f)   // delay 중엔 렌더 안 함
		return;

	const _float fLife = max(m_tDesc.fLifeTime, 0.0001f);
	const _float t01 = min(m_fAge / fLife, 1.f);

	_float fScale = m_tDesc.curveScale.IsEmpty() ? 1.f : m_tDesc.curveScale.Sample(t01);

	if (MESH_EFFECT_DEFINITION::SCALE_AXIS::UNIFORM == m_tDesc.eScaleAxis)
		m_vCurrentScale = _float3(fScale, fScale, fScale);
	else
		m_vCurrentScale = _float3(1.f, 1.f, fScale);

	_float4 vColor = m_tDesc.curveColor.IsEmpty()
		? _float4(1.f, 1.f, 1.f, 1.f)
		: m_tDesc.curveColor.Sample(t01);

	if (!m_tDesc.curveAlpha.IsEmpty())
		vColor.w *= m_tDesc.curveAlpha.Sample(t01);

	m_vCurrentColor = vColor;

	m_pGameInstance->Add_RenderGroup(RENDERID::BLEND, this);
}

HRESULT CEffect_Mesh::Render()
{
	if (FAILED(Bind_ShaderGlobals()))
		return E_FAIL;

	/* pass 인덱스 규약: Alpha=0, Add=1, Alpha_DepthOff=2, Add_DepthOff=3 (Shader_Particle3D와 동일) */
	_uint iPass = (BLEND_MODE::ALPHA == m_tDesc.eBlend) ? 0u : 1u;
	if (m_tDesc.bIgnoreDepth)
		iPass += 2u;

	const _uint iNumMeshes = static_cast<_uint>(m_pModelCom->Get_NumMeshes());

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pShaderCom->Begin(iPass)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Mesh::Ready_Components()
{
	const WNameID strTextureProtoTag =
		(m_tDesc.strTextureProtoTag != INVALID_TAG)
		? m_tDesc.strTextureProtoTag
		: PROTO_COM_TEX_DUMMY_WHITE;

	if (FAILED(__super::Add_Component(m_tDesc.iTextureProtoLevel, strTextureProtoTag,
		COM_TEXTURE, reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), m_tDesc.strShaderProtoTag,
		COM_SHADER, reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), m_tDesc.strModelProtoTag,
		COM_MODEL, reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Mesh::Bind_ShaderGlobals()
{
	CTransform* pSrcTransform = (nullptr != m_pParentTransform)
		? m_pParentTransform
		: m_pTransformCom;

	/* local 모션(스핀+이동)을 root world에 합성. 스케일은 g_vScale로 셰이더에서 별도 적용. */
	_matrix mLocal = XMMatrixRotationAxis(XMLoadFloat3(&m_vSpinAxis), m_fSpinAngle)
		* XMMatrixTranslation(m_vLocalPos.x, m_vLocalPos.y, m_vLocalPos.z);
	_float4x4 mWorld;
	XMStoreFloat4x4(&mWorld, mLocal * XMLoadFloat4x4(pSrcTransform->Get_WorldMatrixPtr()));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &mWorld)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix",
		m_pGameInstance->Get_Transform(D3DTS::VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix",
		m_pGameInstance->Get_Transform(D3DTS::PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vScale",
		&m_vCurrentScale, sizeof(_float3))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor",
		&m_vCurrentColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	return S_OK;
}

_float3 CEffect_Mesh::Make_RandomVelocity() const
{
	const _float fSpeed = m_pGameInstance->Random(m_tDesc.vSpeedRange.x, m_tDesc.vSpeedRange.y);

	_vector vBase = XMLoadFloat3(&m_tDesc.vEmitDirection);
	if (XMVectorGetX(XMVector3LengthSq(vBase)) <= 1e-6f)
		vBase = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	vBase = XMVector3Normalize(vBase);

	_float3 vOut{};
	if (m_tDesc.fEmitConeHalfAngle <= 0.0001f)
	{
		XMStoreFloat3(&vOut, vBase * fSpeed);
		return vOut;
	}

	_vector vTmp = (fabsf(XMVectorGetX(XMVector3Dot(vBase, XMVectorSet(0.f, 1.f, 0.f, 0.f)))) > 0.95f)
		? XMVectorSet(1.f, 0.f, 0.f, 0.f)
		: XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector vRight = XMVector3Normalize(XMVector3Cross(vTmp, vBase));
	_vector vUp = XMVector3Normalize(XMVector3Cross(vBase, vRight));

	const _float t = m_pGameInstance->Random(0.f, m_tDesc.fEmitConeHalfAngle);
	const _float p = m_pGameInstance->Random(0.f, XM_2PI);
	_vector vDir = vBase * cosf(t) + (vRight * cosf(p) + vUp * sinf(p)) * sinf(t);
	XMStoreFloat3(&vOut, XMVector3Normalize(vDir) * fSpeed);
	return vOut;
}

CEffect_Mesh* CEffect_Mesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_Mesh* pInstance = new CEffect_Mesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEffect_Mesh");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEffect_Mesh::Clone(void* pArg)
{
	CEffect_Mesh* pInstance = new CEffect_Mesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEffect_Mesh");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Mesh::Free()
{
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	__super::Free();
}