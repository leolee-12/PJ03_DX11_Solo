#include "Trail.h"
#include "VIBuffer_Trail.h"
#include "GameInstance.h"

CTrail::CTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CTrail::CTrail(const CTrail& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CTrail::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTrail::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	m_tDesc = *static_cast<TRAIL_DESC*>(pArg);

	if (FAILED(__super::Initialize(&m_tDesc)))
		return E_FAIL;

	m_pParentTransform = m_tDesc.pParentTransform;
	if (m_tDesc.iMaxSegments < 2)   m_tDesc.iMaxSegments = 2;
	if (m_tDesc.iMaxSegments > 128) m_tDesc.iMaxSegments = 128;  // CVIBuffer_Trail::iMaxVertices/2

	if (m_tDesc.curveColor.IsEmpty())
	{
		m_tDesc.curveColor.Add_Key(0.f, _float4(1.f, 1.f, 1.f, 1.f));
		m_tDesc.curveColor.Add_Key(1.f, _float4(1.f, 1.f, 1.f, 0.f));
	}

	m_Ribbon.reserve(m_tDesc.iMaxSegments * 2);
	if (FAILED(Ready_Components())) return E_FAIL;

	m_pTransformCom->Set_State(STATE::POSITION,
		XMVectorSet(m_tDesc.vSpawnPos.x, m_tDesc.vSpawnPos.y, m_tDesc.vSpawnPos.z, 1.f));
	return S_OK;
}

void CTrail::Priority_Update(_float) {}
void CTrail::Update(_float) {}

void CTrail::Late_Update(_float fTimeDelta)
{
	if (Is_Dead()) return;

	if (!m_bStopped && nullptr != m_pParentTransform)
	{
		_float4 vPos;
		XMStoreFloat4(&vPos, m_pParentTransform->Get_State(STATE::POSITION));
		Push_Tip(_float3(vPos.x, vPos.y, vPos.z));
	}

	for (auto& seg : m_Segments) seg.fAge += fTimeDelta;
	const _float fLife = max(m_tDesc.fLifeTimePerSegment, 0.0001f);
	while (!m_Segments.empty() && m_Segments.front().fAge >= fLife)
		m_Segments.pop_front();

	if (m_bStopped && m_Segments.empty()) { Set_Dead(); return; }
	if (m_Segments.size() < 2) return;

	Build_Ribbon();
	m_pVIBufferCom->Update_Vertices(m_Ribbon.data(), static_cast<_uint>(m_Ribbon.size()));
	m_pGameInstance->Add_RenderGroup(RENDERID::BLEND, this);
}

void CTrail::Push_Tip(const _float3& vWorldPos)
{
	if (m_bHasLastTip)
	{
		const _float dx = vWorldPos.x - m_vLastTip.x;
		const _float dy = vWorldPos.y - m_vLastTip.y;
		const _float dz = vWorldPos.z - m_vLastTip.z;
		if (dx * dx + dy * dy + dz * dz < m_tDesc.fSegmentSpacing * m_tDesc.fSegmentSpacing)
			return;
	}
	if (m_Segments.size() >= m_tDesc.iMaxSegments)
		m_Segments.pop_front();          // DROP_OLDEST
	m_Segments.push_back({ vWorldPos, 0.f });
	m_vLastTip = vWorldPos;
	m_bHasLastTip = true;
}

void CTrail::Build_Ribbon()
{
	m_Ribbon.clear();
	const _float fLife = max(m_tDesc.fLifeTimePerSegment, 0.0001f);
	const _uint  iCount = static_cast<_uint>(m_Segments.size());
	const _vector vUp = XMVector3Normalize(XMLoadFloat3(&m_tDesc.vUpAxis));

	for (_uint i = 0; i < iCount; ++i)
	{
		const _vector vCur = XMLoadFloat3(&m_Segments[i].vPos);
		_vector vDir = (i + 1 < iCount)
			? XMVectorSubtract(XMLoadFloat3(&m_Segments[i + 1].vPos), vCur)
			: XMVectorSubtract(vCur, XMLoadFloat3(&m_Segments[i - 1].vPos));
		vDir = XMVector3Normalize(vDir);
		const _vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vDir));

		const _float t = m_Segments[i].fAge / fLife;       // 0=신규 .. 1=소멸직전
		const _float fWidth = m_tDesc.fWidthStart + (m_tDesc.fWidthEnd - m_tDesc.fWidthStart) * t;
		const _float fU = (iCount > 1) ? static_cast<_float>(i) / (iCount - 1) : 0.f;
		const _float4 vColor = m_tDesc.curveColor.Sample(t);

		const _vector vHalf = XMVectorScale(vRight, fWidth * 0.5f);
		_float3 vL, vR;
		XMStoreFloat3(&vL, XMVectorAdd(vCur, vHalf));
		XMStoreFloat3(&vR, XMVectorSubtract(vCur, vHalf));

		m_Ribbon.push_back({ vL, _float2(fU, 0.f), vColor });
		m_Ribbon.push_back({ vR, _float2(fU, 1.f), vColor });
	}
}

HRESULT CTrail::Render()
{
	_float4x4 mIdentity;
	XMStoreFloat4x4(&mIdentity, XMMatrixIdentity());

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &mIdentity))) return E_FAIL;  // ribbon = world space
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(D3DTS::VIEW)))) return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(D3DTS::PROJ)))) return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0))) return E_FAIL;

	_uint iPass = (BLEND_MODE::ALPHA == m_tDesc.eBlend) ? 0u : 1u;
	if (m_tDesc.bIgnoreDepth) iPass += 2u;

	if (FAILED(m_pShaderCom->Begin(iPass)))      return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_Resources())) return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))         return E_FAIL;
	return S_OK;
}

HRESULT CTrail::Ready_Components()
{
	const WNameID strTex = (m_tDesc.strTextureProtoTag != INVALID_TAG)
		? m_tDesc.strTextureProtoTag : PROTO_COM_TEX_DUMMY_WHITE;
	if (FAILED(__super::Add_Component(m_tDesc.iTextureProtoLevel, strTex,
		COM_TEXTURE, reinterpret_cast<CComponent**>(&m_pTextureCom)))) return E_FAIL;
	if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), PROTO_COM_SHADER_TRAIL,
		COM_SHADER, reinterpret_cast<CComponent**>(&m_pShaderCom)))) return E_FAIL;
	if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), PROTO_COM_VIBUFFER_TRAIL,
		COM_VIBUFFER, reinterpret_cast<CComponent**>(&m_pVIBufferCom)))) return E_FAIL;
	return S_OK;
}

CTrail* CTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTrail* pInstance = new CTrail(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTrail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CTrail::Clone(void* pArg)
{
	CTrail* pInstance = new CTrail(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTrail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTrail::Free()
{
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	__super::Free();
}