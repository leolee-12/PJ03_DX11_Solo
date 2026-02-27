#include "stdafx.h"

#include "UI_VS_BoltEff.h"
#include "RenderInstance.h"

CUI_VS_BoltEff::CUI_VS_BoltEff(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{ pDevice ,pContext }
{
}

CUI_VS_BoltEff::CUI_VS_BoltEff(const CUI_VS_BoltEff& Prototype)
	:CUIObject{ Prototype }
{
}

HRESULT CUI_VS_BoltEff::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_VS_BoltEff::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fPosX = m_vPrevWinSize.x * 0.5f, m_fPosY = m_vPrevWinSize.y * 0.5f;
	m_fSizeX = 400.f, m_fSizeY = 200.f;

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.f);

	return S_OK;
}

void CUI_VS_BoltEff::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_VS_BoltEff::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	m_fAnimDuration += fTimeDelta * 12.f;
	m_fAlphaValue -= fTimeDelta * 1.5f;

	if (m_fAnimDuration > 6.f)
	{

		_float fRandomScaled = Get_Random(0.5f, 1.5f);
		_float fRandomRadian =  rand() % 360;

		_float fPosX = (m_vPrevWinSize.x * 0.5f) + 300.f * cos(XMConvertToRadians(fRandomRadian));
		_float fPosY = (m_vPrevWinSize.y * 0.5f) - 300.f * sin(XMConvertToRadians(fRandomRadian));

		_float fRotValue = Get_RotationValue({ fPosX , fPosY});
		m_pTransformCom->Rotation({ 0.f, 0.f , 1.f }, XMConvertToRadians(fRotValue));

		m_fAlphaValue = Get_Alpha();

		__super::Set_UI_Setting(m_fSizeX * fRandomScaled, m_fSizeY * fRandomScaled, fPosX, fPosY, 0.f);
		m_fAnimDuration = 0.f;
	}

}

void CUI_VS_BoltEff::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_VS_BoltEff::Render(_float fTimeDelta)
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_fAnimDuration)))
		return E_FAIL;

	if (m_fAlphaValue <= 0.f)
		m_fAlphaValue = 0.f;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaTimer", &m_fAlphaValue, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(15)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_VS_BoltEff::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_VS, TEXT("Prototype_Component_Texture_UI_VS_BG_Bolt"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

_float CUI_VS_BoltEff::Get_RandomNormalize()
{
	return (_float)rand() / RAND_MAX;
}

_float CUI_VS_BoltEff::Get_Random(_float fMin, _float fMax)
{
	return fMin + (fMax - fMin) * Get_RandomNormalize();
}

_float CUI_VS_BoltEff::Get_RotationValue(_vector CurrPos)
{
	_vector vCenter = { m_vPrevWinSize.x * 0.5f , m_vPrevWinSize.y * 0.5f };
	_vector vDir = (vCenter - CurrPos);
	_float fAngleRadians = atan2( XMVectorGetX(vDir), XMVectorGetY(vDir));

	_float fDegree = XMConvertToDegrees(fAngleRadians);

	fDegree += 90.f;

	if (fDegree > 360.f)
		fDegree -= 360.f;

	return fDegree;
}

_float CUI_VS_BoltEff::Get_Alpha()
{
	_uint iRandomAlpha = rand() % 3;

	if (iRandomAlpha >= 1)
		return 0.f;
	else if(iRandomAlpha == 0)
		return 1.f;
}

CUI_VS_BoltEff* CUI_VS_BoltEff::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_VS_BoltEff* pInstatnce = new CUI_VS_BoltEff(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_VS_BoltEff"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_VS_BoltEff::Clone(void* pArg)
{
	CUI_VS_BoltEff* pInstatnce = new CUI_VS_BoltEff(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_VS_BoltEff"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_VS_BoltEff::Free()
{
	__super::Free();
}
