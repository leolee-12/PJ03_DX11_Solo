#include "UIButton_Layered.h"

#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"
#include "Transform.h"

CUIButton_Layered::CUIButton_Layered(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIButton{ pDevice, pContext }
{
}

CUIButton_Layered::CUIButton_Layered(const CUIButton_Layered& Prototype)
	: CUIButton{ Prototype }
{
}

HRESULT CUIButton_Layered::Initialize_Prototype()
{
	return __super::Initialize_Prototype();
}

HRESULT CUIButton_Layered::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		auto pDesc = static_cast<LAYEREDBUTTON_DESC*>(pArg);

		m_strTexLineTag = pDesc->strLineTextureTag;
		m_strTexGlowTag = pDesc->strGlowTextureTag;
		m_iLineTextureLevel = pDesc->iLineTextureLevel;
		m_iGlowTextureLevel = pDesc->iGlowTextureLevel;
		m_iLineTextureIndex = pDesc->iLineTextureIndex;
		m_iGlowTextureIndex = pDesc->iGlowTextureIndex;

		m_vColDiff_Normal = pDesc->vColorBG_Normal;
		m_vColLine_Normal = pDesc->vColorLine_Normal;
		m_vColDiff_Hover = pDesc->vColorBG_Hover;
		m_vColLine_Hover = pDesc->vColorLine_Hover;

		m_bUseGlow = pDesc->bUseGlow;
		m_bUseMirrorUV = pDesc->bUseMirrorUV;
		m_fGlowPulseSpeed = pDesc->fGlowPulseSpeed;
		m_fGlowFadeSpeed = pDesc->fGlowFadeSpeed;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components_Layered()))
		return E_FAIL;

	Apply_StateColors(UI_BUTTON_STATE::NORMAL);
	return S_OK;
}

void CUIButton_Layered::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	const UI_BUTTON_STATE eCur = Get_State();

	if (eCur != m_eLastAppliedState)
		Apply_StateColors(eCur);

	if (m_bUseGlow)
	{
		const _bool bHoverLike =
			(UI_BUTTON_STATE::HOVER == eCur ||
				UI_BUTTON_STATE::PRESSED == eCur);

		const _float fTarget = bHoverLike ? 1.f : 0.f;

		m_fGlowAmount += (fTarget - m_fGlowAmount) * min(1.f, m_fGlowFadeSpeed * fTimeDelta);

		if (m_fGlowAmount > 0.f)
		{
			m_fGlowPhase += m_fGlowPulseSpeed * fTimeDelta;
			if (m_fGlowPhase > XM_2PI)
				m_fGlowPhase -= XM_2PI;
		}
	}
	else
	{
		m_fGlowAmount = 0.f;
	}
}

HRESULT CUIButton_Layered::Render()
{
	const _uint iBGIdx = Get_CurrentTextureIndex();

	if (false == Has_ValidData(iBGIdx))
		return S_OK;

	if (nullptr == m_pLineTextureCom)
		return S_OK;

	if (INVALID_INDEX == m_iLineTextureIndex)
		return S_OK;

	if (FAILED(Bind_LayeredResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIButton_Layered::Ready_Components_Layered()
{
	if (INVALID_TAG != m_strTexLineTag)
	{
		if (FAILED(Add_Component(m_iLineTextureLevel, m_strTexLineTag,
			COM_TEXTURE_LINE, reinterpret_cast<CComponent**>(&m_pLineTextureCom))))
			return E_FAIL;
	}

	if (m_bUseGlow && INVALID_TAG != m_strTexGlowTag)
	{
		if (FAILED(Add_Component(m_iGlowTextureLevel, m_strTexGlowTag,
			COM_TEXTURE_GLOW, reinterpret_cast<CComponent**>(&m_pGlowTextureCom))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CUIButton_Layered::Bind_LayeredResources()
{
	const _uint iBGIdx = Get_CurrentTextureIndex();

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(Bind_ShaderResource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
		return E_FAIL;

	if (FAILED(Bind_ShaderResource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_TexDiff", iBGIdx)))
		return E_FAIL;

	if (FAILED(m_pLineTextureCom->Bind_ShaderResource(m_pShaderCom, "g_TexLine", m_iLineTextureIndex)))
		return E_FAIL;

	if (m_bUseGlow && nullptr != m_pGlowTextureCom && INVALID_INDEX != m_iGlowTextureIndex)
	{
		if (FAILED(m_pGlowTextureCom->Bind_ShaderResource(m_pShaderCom, "g_TexGlow", m_iGlowTextureIndex)))
			return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColDiff", &m_vColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColLine", &m_vColorLine, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fGlowPhase", &m_fGlowPhase, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fGlowAmount", &m_fGlowAmount, sizeof(_float))))
		return E_FAIL;

	const _float fMirror = m_bUseMirrorUV ? 1.f : 0.f;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bMirrorUV", &fMirror, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

void CUIButton_Layered::Apply_StateColors(UI_BUTTON_STATE eState)
{
	const _bool bHoverLike =
		(UI_BUTTON_STATE::HOVER == eState ||
			UI_BUTTON_STATE::PRESSED == eState);

	m_vColor = bHoverLike ? m_vColDiff_Hover : m_vColDiff_Normal;
	m_vColorLine = bHoverLike ? m_vColLine_Hover : m_vColLine_Normal;
	m_eLastAppliedState = eState;
}

CUIButton_Layered* CUIButton_Layered::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIButton_Layered* pInstance = new CUIButton_Layered(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUIButton_Layered");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIButton_Layered::Clone(void* pArg)
{
	CUIButton_Layered* pInstance = new CUIButton_Layered(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIButton_Layered");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIButton_Layered::Free()
{
	__super::Free();

	Safe_Release(m_pLineTextureCom);
	Safe_Release(m_pGlowTextureCom);
}