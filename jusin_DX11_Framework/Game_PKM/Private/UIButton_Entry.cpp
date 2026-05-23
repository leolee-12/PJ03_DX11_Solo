#include "UIButton_Entry.h"

#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"

namespace
{
	// 셰이더 식 fPulse = 0.5 + 0.5*sin(phase) 가 1.0 이 되도록 고정. (Selected 표현을 정적 토글로만 사용)
	constexpr _float kStaticPhase = 1.5707963f;
	constexpr _float kGlowPulseSpeed = 6.f;
	constexpr _float kGlowFadeSpeed = 8.f;
	constexpr _float kGlowSelectedAmount = 0.35f;
}

CUIButton_Entry::CUIButton_Entry(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const
	ENTRYBUTTON_DESC& tDesc)
	: CUIButton{ pDevice, pContext }
	, m_iBaseTextureIndex{ tDesc.iBaseTextureIndex }
	, m_iLineTextureIndex{ tDesc.iLineTextureIndex }
	, m_iGlowTextureIndex{ tDesc.iGlowTextureIndex }
	, m_iDiffuseTextureIndex{ tDesc.iDiffuseTextureIndex }
	, m_vColDiff{ tDesc.vColDiff }
	, m_vColLine{ tDesc.vColLine }
	, m_iShaderPass{ tDesc.iShaderPass }
{
}

CUIButton_Entry::CUIButton_Entry(const CUIButton_Entry& Prototype)
	: CUIButton{ Prototype }
	, m_iBaseTextureIndex{ Prototype.m_iBaseTextureIndex }
	, m_iLineTextureIndex{ Prototype.m_iLineTextureIndex }
	, m_iGlowTextureIndex{ Prototype.m_iGlowTextureIndex }
	, m_iDiffuseTextureIndex{ Prototype.m_iDiffuseTextureIndex }
	, m_vColDiff{ Prototype.m_vColDiff }
	, m_vColLine{ Prototype.m_vColLine }
	, m_iShaderPass{ Prototype.m_iShaderPass }
{
}

HRESULT CUIButton_Entry::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIButton_Entry::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CUIButton_Entry::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	const _float fTarget = m_bSelected ? kGlowSelectedAmount : 0.f;
	m_fGlowAmount += (fTarget - m_fGlowAmount) * min(1.f, kGlowFadeSpeed * fTimeDelta);

	if (m_fGlowAmount > 0.f)
	{
		m_fGlowPhase += kGlowPulseSpeed * fTimeDelta;
		if (m_fGlowPhase > XM_2PI)
			m_fGlowPhase -= XM_2PI;
	}
}

HRESULT CUIButton_Entry::Render()
{
	if (nullptr == m_pShaderCom || nullptr == m_pVIBufferCom || nullptr == m_pTextureCom)
		return S_OK;

	if (FAILED(Bind_EntryResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(m_iShaderPass)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CUIButton_Entry::Reset_SelectedVisual()
{
	m_bSelected = false;
	m_fGlowAmount = 0.f;
	m_fGlowPhase = 0.f;
}

HRESULT CUIButton_Entry::Ready_Components()
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

HRESULT CUIButton_Entry::Bind_EntryResources()
{
	if (FAILED(__super::Bind_BaseMatrices(m_pShaderCom)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_TexMask", m_iBaseTextureIndex)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_TexLine", m_iLineTextureIndex)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_TexGlow", m_iGlowTextureIndex)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_TexDiff", m_iDiffuseTextureIndex)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColDiff", &m_vColDiff, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColLine", &m_vColLine, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fGlowPhase", &m_fGlowPhase, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fGlowAmount", &m_fGlowAmount, sizeof(_float))))
		return E_FAIL;

	const _float fMirror = 1.f;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_bMirrorUV", &fMirror, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CUIButton_Entry* CUIButton_Entry::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const
	ENTRYBUTTON_DESC& tDesc)
{
	CUIButton_Entry* pInstance = new CUIButton_Entry(pDevice, pContext, tDesc);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUIButton_Entry");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIButton_Entry::Clone(void* pArg)
{
	CUIButton_Entry* pInstance = new CUIButton_Entry(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIButton_Entry");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIButton_Entry::Free()
{
	__super::Free();
}