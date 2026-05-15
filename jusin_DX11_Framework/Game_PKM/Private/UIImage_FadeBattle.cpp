#include "UIImage_FadeBattle.h"

#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"

CUIImage_FadeBattle::CUIImage_FadeBattle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIImage{ pDevice, pContext }
{
}

CUIImage_FadeBattle::CUIImage_FadeBattle(const CUIImage_FadeBattle& Prototype)
	: CUIImage{ Prototype }
	, m_fFadeTime{ 0.f }
{
}

HRESULT CUIImage_FadeBattle::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIImage_FadeBattle::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_FadeBattleComponents()))
		return E_FAIL;

	return S_OK;
}

void CUIImage_FadeBattle::Update(_float fTimeDelta)
{
	CUIObject::Update(fTimeDelta);

	if (!m_bVisible)
		return;

	m_fFadeTime += fTimeDelta;

	if (!m_bSpriteAnimEnabled || !m_bSpriteTickAllowed)
		return;

	if (m_iSpriteFrameCount <= 1 || m_fSpriteFrameDuration <= 0.f)
		return;

	if (INVALID_INDEX == m_iTextureIndex || m_iTextureIndex >= m_iSpriteFrameCount)
		m_iTextureIndex = 0;

	if (m_iTextureIndex >= m_iSpriteFrameCount - 1)
	{
		m_iTextureIndex = m_iSpriteFrameCount - 1;
		m_fSpriteAccumTime = 0.f;
		return;
	}

	m_fSpriteAccumTime += fTimeDelta;

	while (m_fSpriteAccumTime >= m_fSpriteFrameDuration)
	{
		m_fSpriteAccumTime -= m_fSpriteFrameDuration;

		if (m_iTextureIndex + 1 >= m_iSpriteFrameCount)
		{
			m_iTextureIndex = m_iSpriteFrameCount - 1;
			m_fSpriteAccumTime = 0.f;
			break;
		}

		++m_iTextureIndex;
	}
}

HRESULT CUIImage_FadeBattle::Render()
{
	if (!Has_ValidData())
		return S_OK;

	if (FAILED(Bind_FadeBattleResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(m_iShaderPass)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIImage_FadeBattle::Ready_FadeBattleComponents()
{
	if (FAILED(Add_Component(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_BTL_FADE_COL,
		COM_TEXTURE_DIFF, reinterpret_cast<CComponent**>(&m_pColorTextureCom))))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CUIImage_FadeBattle::Bind_FadeBattleResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	if (nullptr != m_pColorTextureCom)
	{
		if (FAILED(m_pColorTextureCom->Bind_ShaderResource(m_pShaderCom, "g_TexDiff", 0)))
			return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFadeTime", &m_fFadeTime, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CUIImage_FadeBattle* CUIImage_FadeBattle::Create(ID3D11Device* pDevice, ID3D11DeviceContext*
	pContext)
{
	CUIImage_FadeBattle* pInstance = new CUIImage_FadeBattle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUIImage_FadeBattle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIImage_FadeBattle::Clone(void* pArg)
{
	CUIImage_FadeBattle* pInstance = new CUIImage_FadeBattle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIImage_FadeBattle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIImage_FadeBattle::Free()
{
	Safe_Release(m_pColorTextureCom);

	__super::Free();
}