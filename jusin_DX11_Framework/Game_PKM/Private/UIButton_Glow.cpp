#include "UIButton_Glow.h"

#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"
#include "Transform.h"

CUIButton_Glow::CUIButton_Glow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIButton{ pDevice, pContext }
{
}

CUIButton_Glow::CUIButton_Glow(const CUIButton_Glow& Prototype)
	: CUIButton{ Prototype }
{
}

HRESULT CUIButton_Glow::Initialize_Prototype()
{
	return __super::Initialize_Prototype();
}

HRESULT CUIButton_Glow::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		auto pDesc = static_cast<GLOWBUTTON_DESC*>(pArg);

		m_strTexGlowTag = pDesc->strGlowTextureTag;
		m_iGlowTextureLevel = pDesc->iGlowTextureLevel;
		m_iGlowTextureIndex = pDesc->iGlowTextureIndex;
		m_fGlowPulseSpeed = pDesc->fGlowPulseSpeed;
		m_fGlowFadeSpeed = pDesc->fGlowFadeSpeed;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components_Glow()))
		return E_FAIL;

	return S_OK;
}

void CUIButton_Glow::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	const _bool bHoverLike =
		(UI_BUTTON_STATE::HOVER == Get_State() ||
			UI_BUTTON_STATE::PRESSED == Get_State());

	const _float fTarget = bHoverLike ? 1.f : 0.f;

	m_fGlowAmount += (fTarget - m_fGlowAmount) * min(1.f, m_fGlowFadeSpeed * fTimeDelta);

	if (m_fGlowAmount > 0.f)
	{
		m_fGlowPhase += m_fGlowPulseSpeed * fTimeDelta;
		if (m_fGlowPhase > XM_2PI)
			m_fGlowPhase -= XM_2PI;
	}
}

HRESULT CUIButton_Glow::Render()
{
	const _uint iBaseIdx = Get_CurrentTextureIndex();

	if (false == Has_ValidData(iBaseIdx))
		return S_OK;

	if (nullptr == m_pGlowTextureCom)
		return S_OK;

	if (INVALID_INDEX == m_iGlowTextureIndex)
		return S_OK;

	if (FAILED(Bind_GlowResources(iBaseIdx)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIButton_Glow::Ready_Components_Glow()
{
	if (INVALID_TAG == m_strTexGlowTag)
		return S_OK;

	if (FAILED(Add_Component(m_iGlowTextureLevel, m_strTexGlowTag,
		COM_TEXTURE_GLOW, reinterpret_cast<CComponent**>(&m_pGlowTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIButton_Glow::Bind_GlowResources(_uint iBaseTextureIndex)
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(Bind_ShaderResource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
		return E_FAIL;

	if (FAILED(Bind_ShaderResource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_TexDiff", iBaseTextureIndex)))
		return E_FAIL;

	if (FAILED(m_pGlowTextureCom->Bind_ShaderResource(m_pShaderCom, "g_TexGlow", m_iGlowTextureIndex)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fGlowPhase", &m_fGlowPhase, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fGlowAmount", &m_fGlowAmount, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CUIButton_Glow* CUIButton_Glow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIButton_Glow* pInstance = new CUIButton_Glow(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUIButton_Glow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIButton_Glow::Clone(void* pArg)
{
	CUIButton_Glow* pInstance = new CUIButton_Glow(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIButton_Glow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIButton_Glow::Free()
{
	__super::Free();

	Safe_Release(m_pGlowTextureCom);
}