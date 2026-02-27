#include "stdafx.h"

#include "UI_Loading_EnergyEff.h"
#include "RenderInstance.h"

CUI_Loading_EnergyEff::CUI_Loading_EnergyEff(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{ pDevice ,pContext }
{
}

CUI_Loading_EnergyEff::CUI_Loading_EnergyEff(const CUI_Loading_EnergyEff& Prototype)
	:CUIObject{ Prototype }
{
}

HRESULT CUI_Loading_EnergyEff::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Loading_EnergyEff::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fSizeX = 50.f, m_fSizeY = 50.f;
	m_fPosX = m_vPrevWinSize.x * 0.5f, m_fPosY = m_vPrevWinSize.y * 0.5f;

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.f);

	return S_OK;
}

void CUI_Loading_EnergyEff::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_Loading_EnergyEff::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	/*m_fAngle += 180.f * fTimeDelta;

	if (m_fAngle >= 360)
		m_fAngle = 0;*/
	
	/*_float fRotationX = (m_vPrevWinSize.x * 0.5f) + 100.f * cos(XMConvertToRadians(m_fAngle));
	_float fRotationY = (m_vPrevWinSize.y * 0.5f) - 100.f * sin(XMConvertToRadians(m_fAngle));
	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,{ fRotationX ,fRotationY , 0.f,1.f });*/

	if (m_pGameInstance->Key_Pressing(DIK_W))
	{
		m_pTransformCom->Go_Up(fTimeDelta);
	}
	
	if (m_pGameInstance->Key_Pressing(DIK_S))
	{
		m_pTransformCom->Go_Down(fTimeDelta);
	}
	
	if (m_pGameInstance->Key_Pressing(DIK_A))
	{
		m_pTransformCom->Go_Left(fTimeDelta);
	}
	
	if (m_pGameInstance->Key_Pressing(DIK_D))
	{
		m_pTransformCom->Go_Right(fTimeDelta);
	}

	//CTransform* pTargetTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_CHARACTER, TEXT("Layer_MarkArrow"), TEXT("Com_Transform")));
	//_vector vOriginPos = pTargetTransform->Get_State(CTransform::STATE_POSITION);
	
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, vOriginPos);

	//_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//_float3 OffsetPos = { XMVectorGetX(vPos) , XMVectorGetY(vPos) , 0.f };
	//
	//m_pTrailVIBuffer->Line(OffsetPos);
}

void CUI_Loading_EnergyEff::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	RENDER_OBJECT tDesc{};
	tDesc.tGlowDesc.iPassIndex = 2;
	tDesc.tGlowDesc.fGlowFactor = 2.7f;

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI_GLOW, this,&tDesc);
}

HRESULT CUI_Loading_EnergyEff::Render(_float fTimeDelta)
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	_vector vColor = { 1.f, 0.f, 0.f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_vector))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(32)))
		return E_FAIL;

	if (FAILED(m_pTrailVIBuffer->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pTrailVIBuffer->Render()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CUI_Loading_EnergyEff::Ready_Components()
{
	if(FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_CHARACTER, TEXT("Prototype_Component_Texture_UI_LoadingEnergyEff"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_TrailBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Trail"),
		TEXT("Com_TrailBuffer"), reinterpret_cast<CComponent**>(&m_pTrailVIBuffer))))
		return E_FAIL;

	return S_OK;
}

CUI_Loading_EnergyEff* CUI_Loading_EnergyEff::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Loading_EnergyEff* pInstatnce = new CUI_Loading_EnergyEff(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_Loading_EnergyEff"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_Loading_EnergyEff::Clone(void* pArg)
{
	CUI_Loading_EnergyEff* pInstatnce = new CUI_Loading_EnergyEff(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_Loading_EnergyEff"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_Loading_EnergyEff::Free()
{
	Safe_Release(m_pTrailVIBuffer);

	__super::Free();
}
