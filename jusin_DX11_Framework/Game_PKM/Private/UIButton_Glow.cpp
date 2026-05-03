#include "UIButton_Glow.h"

#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"

CUIButton_Glow::CUIButton_Glow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const GLOWBUTTON_DESC& tDesc)
	: CUIButton{ pDevice, pContext }
	, m_fGlowPulseSpeed{ tDesc.fGlowPulseSpeed }
	, m_fGlowFadeSpeed{ tDesc.fGlowFadeSpeed }
{
}

CUIButton_Glow::CUIButton_Glow(const CUIButton_Glow& Prototype)
	: CUIButton{ Prototype }
	, m_fGlowPulseSpeed{ Prototype.m_fGlowPulseSpeed }
	, m_fGlowFadeSpeed{ Prototype.m_fGlowFadeSpeed }
{
}

HRESULT CUIButton_Glow::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CUIButton_Glow::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	// __super::Initialize에서 오버라이드된 Ready_Components가 호출됨
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
	if (nullptr == m_pShaderCom || nullptr == m_pVIBufferCom || nullptr == m_pTextureCom)
		return S_OK;

	if (FAILED(Bind_GlowResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIButton_Glow::Ready_Components()
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

void CUIButton_Glow::On_State_Changed(UI_BUTTON_STATE /*eOld*/, UI_BUTTON_STATE /*eNew*/)
{
}

HRESULT CUIButton_Glow::Bind_GlowResources()
{
	if (FAILED(__super::Bind_BaseMatrices(m_pShaderCom)))
		return E_FAIL;

	_uint iDiffSlot = ETOUI(IMAGE_SLOT::BASE);

	switch (Get_State())
	{
	case UI_BUTTON_STATE::HOVER:
	case UI_BUTTON_STATE::PRESSED:
		iDiffSlot = ETOUI(IMAGE_SLOT::HOVERED);
		break;

	case UI_BUTTON_STATE::DISABLED:
		iDiffSlot = ETOUI(IMAGE_SLOT::DISABLED);
		break;

	default:
		iDiffSlot = ETOUI(IMAGE_SLOT::BASE);
		break;
	}

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_TexDiff", iDiffSlot)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_TexGlow",
		ETOUI(IMAGE_SLOT::GLOW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fGlowPhase", &m_fGlowPhase, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fGlowAmount", &m_fGlowAmount, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CUIButton_Glow* CUIButton_Glow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const GLOWBUTTON_DESC& tDesc)
{
	CUIButton_Glow* pInstance = new CUIButton_Glow(pDevice, pContext, tDesc);

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
}