#include "UIProgressBar.h"

#include "GameInstance.h"

CUIProgressBar::CUIProgressBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUIProgressBar::CUIProgressBar(const CUIProgressBar& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUIProgressBar::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIProgressBar::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		auto pDesc = static_cast<UIPROGRESSBAR_DESC*>(pArg);
		m_strBackTextureTag = pDesc->strBackTextureTag;
		m_iBackTextureIndex = pDesc->iBackTextureIndex;
		m_strFillTextureTag = pDesc->strFillTextureTag;
		m_iFillTextureIndex = pDesc->iFillTextureIndex;

		m_strShaderTag = pDesc->strShaderTag;
		m_strVIBufferTag = pDesc->strVIBufferTag;

		m_iShaderLevel = pDesc->iShaderLevel;
		m_iVIBufferLevel = pDesc->iVIBufferLevel;
		m_iBackTextureLevel = pDesc->iBackTextureLevel;
		m_iFillTextureLevel = pDesc->iFillTextureLevel;

		m_vBackColor = pDesc->vBackColor;
		m_vFillColor = pDesc->vFillColor;

		m_eDirection = pDesc->eDirection;
		Set_FillAmount(pDesc->fFillAmount);
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUIProgressBar::Priority_Update(_float fTimeDelta)
{
}

void CUIProgressBar::Update(_float fTimeDelta)
{
}

void CUIProgressBar::Late_Update(_float fTimeDelta)
{
	if (!m_bVisible) return;

	m_pGameInstance->Add_RenderGroup(RENDERID::UI, this);
}

HRESULT CUIProgressBar::Render()
{
	if (Has_ValidBack())
		Render_Rect(Get_ScreenRect(), m_pBackTextureCom, m_iBackTextureIndex, m_vBackColor);

	if (m_fFillAmount > 0.f && Has_ValidFill())
		Render_Rect(Get_FillRect(), m_pFillTextureCom, m_iFillTextureIndex, m_vFillColor);

	return S_OK;
}

HRESULT CUIProgressBar::Ready_Components()
{
	if (FAILED(__super::Add_Component(m_iShaderLevel, m_strShaderTag, COM_SHADER, reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iVIBufferLevel, m_strVIBufferTag, COM_VIBUFFER, reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iBackTextureLevel, m_strBackTextureTag, COM_TEXTURE_BACK, reinterpret_cast<CComponent**>(&m_pBackTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iFillTextureLevel, m_strFillTextureTag, COM_TEXTURE_FILL, reinterpret_cast<CComponent**>(&m_pFillTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIProgressBar::Render_Rect(const _float4& rc, CTexture* pTexture, _uint iTextureIndex, const _float4& vColor)
{
	const _float fCenterX = rc.x + rc.z * 0.5f;
	const _float fCenterY = rc.y + rc.w * 0.5f;

	_float4x4 matWorld;
	XMStoreFloat4x4(&matWorld,
		XMMatrixScaling(rc.z, rc.w, 1.f) *
		XMMatrixTranslation(
			fCenterX - m_fViewWidth * 0.5f,
			-fCenterY + m_fViewHeight * 0.5f,
			0.f));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &matWorld)))
		return E_FAIL;
	if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
		return E_FAIL;
	if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
		return E_FAIL;
	if (FAILED(pTexture->Bind_ShaderResource(m_pShaderCom, "g_Texture", iTextureIndex)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

_float4 CUIProgressBar::Get_FillRect() const
{
	const _float4 rc = Get_ScreenRect();   // (left, top, w, h)
	const _float fAmount = max(0.f, min(1.f, m_fFillAmount));

	_float fLeft = rc.x, fTop = rc.y, fWidth = rc.z, fHeight = rc.w;

	switch (m_eDirection)
	{
	case UI_PROGRESS_DIR::LEFT_TO_RIGHT:
		fWidth = rc.z * fAmount;
		break;
	case UI_PROGRESS_DIR::RIGHT_TO_LEFT:
		fWidth = rc.z * fAmount;
		fLeft = rc.x + (rc.z - fWidth);
		break;
	case UI_PROGRESS_DIR::TOP_TO_BOTTOM:
		fHeight = rc.w * fAmount;
		break;
	case UI_PROGRESS_DIR::BOTTOM_TO_TOP:
		fHeight = rc.w * fAmount;
		fTop = rc.y + (rc.w - fHeight);
		break;
	default: break;
	}

	return _float4(fLeft, fTop, fWidth, fHeight);
}

_bool CUIProgressBar::Has_ValidBack() const
{
	return (nullptr != m_pShaderCom)
		&& (nullptr != m_pVIBufferCom)
		&& (nullptr != m_pBackTextureCom)
		&& (INVALID_INDEX != m_iBackTextureIndex);
}

_bool CUIProgressBar::Has_ValidFill() const
{
	return (nullptr != m_pShaderCom)
		&& (nullptr != m_pVIBufferCom)
		&& (nullptr != m_pFillTextureCom)
		&& (INVALID_INDEX != m_iFillTextureIndex);
}

CUIProgressBar* CUIProgressBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIProgressBar* pInstance = new CUIProgressBar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUIProgressBar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIProgressBar::Clone(void* pArg)
{
	CUIProgressBar* pInstance = new CUIProgressBar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIProgressBar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIProgressBar::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pBackTextureCom);
	Safe_Release(m_pFillTextureCom);

}