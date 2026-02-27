#include "stdafx.h"

#include "UI_InputDirEffect.h"
#include "RenderInstance.h"

CUI_InputDirEffect::CUI_InputDirEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_Input{ pDevice ,pContext }
{
}

CUI_InputDirEffect::CUI_InputDirEffect(const CUI_InputDirEffect& Prototype)
	:CUI_Input{ Prototype }
{
}

HRESULT CUI_InputDirEffect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_InputDirEffect::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	UI_DIREFFECT* pDesc = static_cast<UI_DIREFFECT*>(pArg);
	_vector vCreatePos = pDesc->vCreatePos;
	_float CreatePosX = XMVectorGetX(vCreatePos);
	_float CreatePosY = XMVectorGetY(vCreatePos);

	_float fAngle = pDesc->fAngle;
	_float fScaled = pDesc->fScaled;

	m_fSizeX = 60 * fScaled;
	m_fSizeY = 12;
	m_fPosX = CreatePosX;
	m_fPosY = CreatePosY;

	
	__super::Set_UI_Setting(m_fSizeX , m_fSizeY, m_fPosX, m_fPosY, 0.8f);

	fAngle += 90.f;

	if (fAngle >= 360.f)
	{
		fAngle -= 360.f;
	}

	m_pTransformCom->Rotation({ 0,0,1 }, XMConvertToRadians(fAngle));

	return S_OK;
}

void CUI_InputDirEffect::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);

	m_fDestroyTimer += fTimeDelta;
}

void CUI_InputDirEffect::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (m_fDestroyTimer >= 0.25f)
	{
		m_fDestroyTimer = 0.f;
		Destory();
	}
}

void CUI_InputDirEffect::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_InputDirEffect::Render(_float fTimeDelta)
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 1)))
		return E_FAIL;

	_vector vColor = { 1 - m_pUI_Manager->m_fColorValue  , 0.f , m_pUI_Manager->m_fColorValue  , 1.f };

	_float fColorTimer = m_pUI_Manager->m_fColorValue + 0.125f; 

	if (fColorTimer >= 1.f)
		fColorTimer = 1.f;
	

	_vector vEndColor = { 1 - fColorTimer, 0.f , fColorTimer, 1.f };


	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_vector))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vEndColor", &vEndColor, sizeof(_vector))))
		return E_FAIL;

	_float m_fAlpha = 1.f;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaTimer", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(14)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_InputDirEffect::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_DirKeyInputEffect"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

CUI_InputDirEffect* CUI_InputDirEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_InputDirEffect* pInstatnce = new CUI_InputDirEffect(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_InputDirEffect"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_InputDirEffect::Clone(void* pArg)
{
	CUI_InputDirEffect* pInstatnce = new CUI_InputDirEffect(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_InputDirEffect"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_InputDirEffect::Free()
{
	__super::Free();
}
