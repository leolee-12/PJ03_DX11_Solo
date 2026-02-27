#include "Light.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"

#include "GameInstance.h"
#ifdef _DEBUG
#include "DebugDraw.h"
#endif

CLight::CLight()
{

}

HRESULT CLight::Initialize(const LIGHT_DESC& LightDesc)
{
	Set_LightDesc(LightDesc);

	if (m_LightDesc.strLightName == "")
	{
		MSG_BOX("CLight : Initialize Failed - Name이 없습니다.");
		return E_FAIL;
	}

#if LIGHT_DEBUG
#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(GI()->Get_D3D11Context().Get());
	m_pEffect = new BasicEffect(GI()->Get_D3D11Device().Get());
	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCode = { nullptr };
	size_t	iShaderCodeLength = { 0 };

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderCodeLength);

	if (FAILED(GI()->Get_D3D11Device()
		->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iShaderCodeLength, m_pInputLayout.GetAddressOf())))
		return E_FAIL;
#endif
#endif

	return S_OK;
}

void CLight::Tick(_cref_time fTimeDelta)
{
	Dead_Ready(fTimeDelta);
	Update_LightOwner();

}

HRESULT CLight::Render(shared_ptr<CShader> pShader, shared_ptr<CVIBuffer_Rect> pVIBuffer)
{
	_uint		iPassIndex = { 0 };

	if (LIGHT_DESC::TYPE_DIRECTIONAL == m_LightDesc.eType)
	{
		pShader->Bind_RawValue("g_vLightDir", &m_LightDesc.vDirection, sizeof(_float4));

		iPassIndex = 1;
	}
	else if ((LIGHT_DESC::TYPE_POINT == m_LightDesc.eType)
		|| (LIGHT_DESC::TYPE_SHADOW == m_LightDesc.eType))
	{
		pShader->Bind_RawValue("g_vLightPos", &m_LightDesc.vPosition, sizeof(_float4));
		pShader->Bind_RawValue("g_fLightRange", &m_LightDesc.fRange, sizeof(_float));
		pShader->Bind_RawValue("g_fSpotPower", &m_LightDesc.fSpotPower, sizeof(_float));

		iPassIndex = 2;
	}
	else if ((LIGHT_DESC::TYPE_SPOT == m_LightDesc.eType)
		|| (LIGHT_DESC::TYPE_SYMMETRY == m_LightDesc.eType))
	{
		pShader->Bind_RawValue("g_fLightRange", &m_LightDesc.fRange, sizeof(_float));
		pShader->Bind_RawValue("g_vLightDir", &m_LightDesc.vDirection, sizeof(_float4));
		pShader->Bind_RawValue("g_vLightPos", &m_LightDesc.vPosition, sizeof(_float4));
		pShader->Bind_RawValue("g_fSpotPower", &m_LightDesc.fSpotPower, sizeof(_float));

		_float2 ThetaPi_Radian = { XMConvertToRadians(m_LightDesc.fThetaPi.x), XMConvertToRadians(m_LightDesc.fThetaPi.y) };
		pShader->Bind_RawValue("g_fLightThetaPi", &ThetaPi_Radian, sizeof(_float2));

		if (LIGHT_DESC::TYPE_SPOT == m_LightDesc.eType)
			iPassIndex = 12;
		else if (LIGHT_DESC::TYPE_SYMMETRY == m_LightDesc.eType)
			iPassIndex = 13;
	}

	if (FAILED(pShader->Bind_RawValue("g_fLightConstDamping", &m_LightDesc.fConstDamping, sizeof(_float))))
		RETURN_EFAIL;
	if (FAILED(pShader->Bind_RawValue("g_fLightLinearDamping", &m_LightDesc.fLinearDamping, sizeof(_float))))
		RETURN_EFAIL;
	if (FAILED(pShader->Bind_RawValue("g_fLightQuadDamping", &m_LightDesc.fQuadDamping, sizeof(_float))))
		RETURN_EFAIL;

	if (FAILED(pShader->Bind_RawValue("g_vLightDiffuse", &m_LightDesc.vDiffuse, sizeof(_float4))))
		RETURN_EFAIL;
	if (FAILED(pShader->Bind_RawValue("g_vLightAmbient", &m_LightDesc.vAmbient, sizeof(_float4))))
		RETURN_EFAIL;
	if (FAILED(pShader->Bind_RawValue("g_vLightSpecular", &m_LightDesc.vSpecular, sizeof(_float4))))
		RETURN_EFAIL;
	if (FAILED(pShader->Bind_RawValue("g_vLightEmissive", &m_LightDesc.vEmissive, sizeof(_float4))))
		RETURN_EFAIL;

	if (FAILED(pShader->Bind_RawValue("g_bUseVolumetric", &m_LightDesc.bUseVolumetric, sizeof(_bool))))
		RETURN_EFAIL;
	if (FAILED(pShader->Bind_RawValue("g_fVolumeQuadDamping", &m_LightDesc.fVolumeQuadDamping, sizeof(_float))))
		RETURN_EFAIL;

	pShader->Begin(iPassIndex);

	pVIBuffer->Bind_VIBuffers();

#if LIGHT_DEBUG
#ifdef _DEBUG
	GI()->Add_DebugEvent(MakeDelegate(this, &CLight::Render_Debug));
#endif
#endif

	return pVIBuffer->Render();
}

#ifdef _DEBUG
HRESULT CLight::Render_Debug()
{
	if (nullptr == m_pBatch || nullptr == m_pEffect || nullptr == m_pInputLayout)
		return E_FAIL;

	m_pBatch->Begin();

	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView(GI()->Get_TransformMatrix(CPipeLine::TS_VIEW));
	m_pEffect->SetProjection(GI()->Get_TransformMatrix(CPipeLine::TS_PROJ));

	GI()->Get_D3D11Context()->IASetInputLayout(m_pInputLayout.Get());

	m_pEffect->Apply(GI()->Get_D3D11Context().Get());

	DX::Draw(m_pBatch, m_AABB, Colors::Yellow);

	m_pBatch->End();

	return S_OK;
}
#endif // _DEBUG


void CLight::Invalidate_Light()
{
	// 빛에 변동이 일어났다면 그에 대한 영역 초기화를 해준다.
	if (m_bNeedInvalidate)
	{
		switch (m_LightDesc.eType)
		{
		case LIGHT_DESC::TYPE_SHADOW:
		case LIGHT_DESC::TYPE_POINT:
		{
			BoundingSphere Sphere(
				_float3(m_LightDesc.vPosition.x, m_LightDesc.vPosition.y, m_LightDesc.vPosition.z),
				m_LightDesc.fRange);
			BoundingBox::CreateFromSphere(m_AABB, Sphere);
			break;
		}
		case LIGHT_DESC::TYPE_SPOT:
		{
			_vector vPos = XMLoadFloat4(&m_LightDesc.vPosition) + XMLoadFloat4(&m_LightDesc.vDirection) * m_LightDesc.fRange * 2.f;
			BoundingSphere Sphere(_float3(vPos), m_LightDesc.fRange * 2.f);
			BoundingBox::CreateFromSphere(m_AABB, Sphere);
			/*BoundingBox::CreateFromPoints(m_AABB, XMLoadFloat4(&m_LightDesc.vPosition),
				XMLoadFloat4(&m_LightDesc.vPosition)
				+ XMLoadFloat4(&m_LightDesc.vDirection) * m_LightDesc.fRange);*/
			break;
		}
		case LIGHT_DESC::TYPE_SYMMETRY:
		{
			BoundingSphere Sphere(
				_float3(m_LightDesc.vPosition.x, m_LightDesc.vPosition.y, m_LightDesc.vPosition.z),
				m_LightDesc.fRange);
			BoundingBox::CreateFromSphere(m_AABB, Sphere);
			break;
		}
		case LIGHT_DESC::TYPE_DIRECTIONAL:
		default:
			m_AABB.Center = _float3(m_LightDesc.vPosition.x, m_LightDesc.vPosition.y, m_LightDesc.vPosition.z);
			m_AABB.Extents = _float3(FLT_MAX, FLT_MAX, FLT_MAX);
			m_LightDesc.fRange = FLT_MAX * 0.5f;
			break;
		}

		// 초기화 후 변경
		m_bNeedInvalidate = false;
	}
}

_bool CLight::Intersects(const BoundingFrustum& Frustum)
{
	// 업데이트 된적 없으면 업데이트
	Invalidate_Light();

	return m_bIsEnabled = m_AABB.Intersects(Frustum);
}


void CLight::Set_LightPosition(_float4 vPos)
{
	m_LightDesc.vPosition = vPos;
	// 빛의 변동을 알리기 위해 설정하는 변수
	m_bNeedInvalidate = true;
}

const _float4 CLight::Get_LightPosition()
{
	if (m_bNeedInvalidate)
		Invalidate_Light();

	return m_LightDesc.vPosition;
}

void CLight::Set_LightRange(_float fRange)
{
	m_LightDesc.fRange = max(fRange, FLT_MIN);
	// 빛의 변동을 알리기 위해 설정하는 변수
	m_bNeedInvalidate = true;
}

const _float CLight::Get_LightRange()
{
	if (m_bNeedInvalidate)
		Invalidate_Light();

	return m_LightDesc.fRange;
}

void CLight::Set_LightDamping(_float fQuad, _float fLinear, _float fConst)
{
	m_LightDesc.fConstDamping = fConst;
	m_LightDesc.fLinearDamping = fLinear;
	m_LightDesc.fQuadDamping = fQuad;
}

void CLight::Set_LightVolumeQuadDamping(_float fVolumeQuadDamping)
{
	m_LightDesc.fVolumeQuadDamping = fVolumeQuadDamping;
}

void CLight::Set_LightDesc(LIGHT_DESC Desc)
{
	XMStoreFloat4(&Desc.vDirection, XMVector3Normalize(XMLoadFloat4(&Desc.vDirection)));
	m_LightDesc = Desc;
	// 빛의 변동을 알리기 위해 설정하는 변수
	m_bNeedInvalidate = true;
}

const LIGHT_DESC& CLight::Get_LightDesc()
{
	if (m_bNeedInvalidate)
		Invalidate_Light();

	return m_LightDesc;
}

void CLight::Dead_Ready(_cref_time fTimeDelta)
{
	if (m_bDeadReady == true)
	{
		m_fRangeQuadDecrease += m_fRangeQuadDecrease * m_fRangeQuadDecrease * fTimeDelta;
		Set_LightRange(m_LightDesc.fRange
			- (m_fRangeLinearDecrease * fTimeDelta)
			- (m_fRangeQuadDecrease * fTimeDelta));
		m_LightDesc.fSpotPower -= 0.1f * fTimeDelta;
		m_LightDesc.vDiffuse.w -= 0.1f * fTimeDelta;

		if (m_LightDesc.fRange < 0)
		{
			m_bDead = true;
			m_bDeadReady = false;
		}
	}
}

void CLight::Update_LightOwner()
{
	if (m_bIsGetOwner == true)
	{
		if (m_pOwner.lock() != nullptr)
		{
			_bool IsActiveState = m_pOwner.lock()->IsState(OBJSTATE::Active);
			
			if (true == IsActiveState) { m_bTurnOn = true; }
			else if (false == IsActiveState) { m_bTurnOn = false;  return; }
						
			float3 TargetPos = m_pOwner.lock()->Get_TransformCom().lock()->Get_State(CTransform::STATE_POSITION);
			float3 LocalOffset = m_pOwner.lock()->Get_PhysXColliderCom().lock()->Get_LocalOffset();
			TargetPos += LocalOffset;

			m_LightDesc.vPosition = { TargetPos.x, TargetPos.y, TargetPos.z, 1.f };
		}
		else if (m_pOwner.lock() == nullptr)			
		{
			Set_Dead();
		}
	}
}

void CLight::Set_Owner(weak_ptr<CGameObject> _pOwner)
{
	if (_pOwner.lock() == nullptr)
		return;

	m_pOwner = _pOwner;
	m_bIsGetOwner = true;
}

weak_ptr<CGameObject> CLight::Get_Owner()
{
	return m_pOwner;
}


shared_ptr<CLight> CLight::Create(const LIGHT_DESC& LightDesc)
{
	shared_ptr<CLight> pInstance(new CLight(), DELETER(CLight));

	if (FAILED(pInstance->Initialize(LightDesc)))
	{
		MSG_BOX("Failed to Created : CLight");
	}
	return pInstance;
}


void CLight::Free()
{
	__super::Free();

#ifdef _DEBUG
	Safe_Delete(m_pBatch);
	Safe_Delete(m_pEffect);
#endif
}