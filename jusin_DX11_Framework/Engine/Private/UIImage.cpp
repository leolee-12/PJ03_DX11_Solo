#include "UIImage.h"

#include "GameInstance.h"
#include "UIButton.h"

CUIImage::CUIImage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUIImage::CUIImage(const CUIImage& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUIImage::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIImage::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		auto pDesc = static_cast<UIIMAGE_DESC*>(pArg);
		m_strTextureTag = pDesc->strTextureTag;
		m_strShaderTag = pDesc->strShaderTag;
		m_strVIBufferTag = pDesc->strVIBufferTag;
		m_iTextureLevel = pDesc->iTextureLevel;
		m_iShaderLevel = pDesc->iShaderLevel;
		m_iVIBufferLevel = pDesc->iVIBufferLevel;
		m_iTextureIndex = pDesc->iTextureIndex;
		m_vColor = pDesc->vColor;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUIImage::Priority_Update(_float fTimeDelta)
{
}

void CUIImage::Update(_float fTimeDelta)
{
}

void CUIImage::Late_Update(_float fTimeDelta)
{
	if (!m_bVisible)
		return;

	m_pGameInstance->Add_RenderGroup(RENDERID::UI, this);
}

HRESULT CUIImage::Render()
{
	if (!Has_ValidData())
		return S_OK;

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

HRESULT CUIImage::Ready_Components()
{
	if (INVALID_TAG == m_strShaderTag || INVALID_TAG == m_strVIBufferTag)
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iShaderLevel, m_strShaderTag, COM_SHADER, reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iVIBufferLevel, m_strVIBufferTag, COM_VIBUFFER, reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if (INVALID_TAG != m_strTextureTag)
	{
		if (FAILED(__super::Add_Component(m_iTextureLevel, m_strTextureTag, COM_TEXTURE, reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CUIImage::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
		return E_FAIL;

	if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
		return E_FAIL;

	if (m_pTextureCom && INVALID_INDEX != m_iTextureIndex)
	{
		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureIndex)))
			return E_FAIL;
	}

	// 틴트 색상 — UI 전용 셰이더 도입 시 자동 반영
	m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4));

	return S_OK;
}

_bool CUIImage::Has_ValidData() const
{
	return (nullptr != m_pShaderCom)
		&& (nullptr != m_pVIBufferCom)
		&& (nullptr != m_pTextureCom)
		&& (INVALID_INDEX != m_iTextureIndex);
}

CUIImage* CUIImage::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIImage* pInstance = new CUIImage(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUIImage");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIImage::Clone(void* pArg)
{
	CUIImage* pInstance = new CUIImage(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIImage");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIImage::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
}