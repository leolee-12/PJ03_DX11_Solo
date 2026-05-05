#include "UIButton_Layered.h"

#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"

CUIButton_Layered::CUIButton_Layered(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LAYEREDBUTTON_DESC& tDesc)
	: CUIButton{ pDevice, pContext }
	, m_vColBase_Normal{ tDesc.vColBase_Normal }
	, m_vColLine_Normal{ tDesc.vColLine_Normal }
	, m_vColBase_Hover{ tDesc.vColBase_Hover }
	, m_vColLine_Hover{ tDesc.vColLine_Hover }
	, m_bUseGlow{ tDesc.bUseGlow }
	, m_bUseMirrorUV{ tDesc.bUseMirrorUV }
	, m_fGlowPulseSpeed{ tDesc.fGlowPulseSpeed }
	, m_fGlowFadeSpeed{ tDesc.fGlowFadeSpeed }
	, m_iShaderPass { tDesc.iShaderPass }
{
}

CUIButton_Layered::CUIButton_Layered(const CUIButton_Layered& Prototype)
	: CUIButton{ Prototype }
	, m_vColBase_Normal{ Prototype.m_vColBase_Normal }
	, m_vColLine_Normal{ Prototype.m_vColLine_Normal }
	, m_vColBase_Hover{ Prototype.m_vColBase_Hover }
	, m_vColLine_Hover{ Prototype.m_vColLine_Hover }
	, m_bUseGlow{ Prototype.m_bUseGlow }
	, m_bUseMirrorUV{ Prototype.m_bUseMirrorUV }
	, m_fGlowPulseSpeed{ Prototype.m_fGlowPulseSpeed }
	, m_fGlowFadeSpeed{ Prototype.m_fGlowFadeSpeed }
	, m_iShaderPass{ Prototype.m_iShaderPass }
{
}

HRESULT CUIButton_Layered::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIButton_Layered::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Apply_StateColors(UI_BUTTON_STATE::NORMAL);
	return S_OK;
}

void CUIButton_Layered::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (m_bUseGlow)
	{
		const UI_BUTTON_STATE eCur = Get_State();
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
	if (nullptr == m_pShaderCom || nullptr == m_pVIBufferCom || nullptr == m_pTextureCom)
		return S_OK;

	if (FAILED(Bind_LayeredResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(m_iShaderPass)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIButton_Layered::Ready_Components()
{
	if (FAILED(Add_Component(m_iShaderLevel, m_strShaderTag,
		COM_SHADER, reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(Add_Component(m_iVIBufferLevel, m_strVIBufferTag,
		COM_VIBUFFER, reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if (FAILED(Add_Component(m_iTextureLevel, m_strTextureTag,
		COM_TEXTURE, reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

void CUIButton_Layered::On_State_Changed(UI_BUTTON_STATE /*eOld*/, UI_BUTTON_STATE eNew)
{
	Apply_StateColors(eNew);
}

HRESULT CUIButton_Layered::Bind_LayeredResources()
{
	if (FAILED(__super::Bind_BaseMatrices(m_pShaderCom)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_TexDiff",
		ETOUI(IMAGE_SLOT::BASE))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_TexLine",
		ETOUI(IMAGE_SLOT::LINE))))
		return E_FAIL;

	if (m_bUseGlow)
	{
		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_TexGlow",
			ETOUI(IMAGE_SLOT::GLOW))))
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

	m_vColor = bHoverLike ? m_vColBase_Hover : m_vColBase_Normal;
	m_vColorLine = bHoverLike ? m_vColLine_Hover : m_vColLine_Normal;
}

CUIButton_Layered* CUIButton_Layered::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LAYEREDBUTTON_DESC& tDesc)
{
	CUIButton_Layered* pInstance = new CUIButton_Layered(pDevice, pContext, tDesc);

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
}