#include "stdafx.h"

#include "UI_SelectArrow.h"
#include "RenderInstance.h"

CUI_SelectArrow::CUI_SelectArrow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{ pDevice ,pContext }
{
}

CUI_SelectArrow::CUI_SelectArrow(const CUI_SelectArrow& Prototype)
	:CUIObject{ Prototype }
{
}

HRESULT CUI_SelectArrow::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_SelectArrow::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fPosX = m_vPrevWinSize.x * 0.5f, m_fPosY = 620.f;
	m_fSizeX = 50.f, m_fSizeY = 50.f;

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.f);

	return S_OK;
}

void CUI_SelectArrow::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_SelectArrow::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	Move(fTimeDelta);

}

void CUI_SelectArrow::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	if (m_iNumChoice >= 4)
		Destory();

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_SelectArrow::Render(_float fTimeDelta)
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	if (m_iTextureIndex == 2)
	{
		m_iTextureIndex += 1;
	}

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureIndex)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_SelectArrow::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_CHARACTER, TEXT("Prototype_Component_Texture_CharacterSelectArrow"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

void CUI_SelectArrow::PositionUpdate(_float fTimeDelta)
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float fPosX = XMVectorGetX(vPos) + g_iWinSizeX * 0.5f;
	_float fPosY = g_iWinSizeY * 0.5f - XMVectorGetY(vPos);
	
	if (fPosX <= 50)
		m_pTransformCom->Go_Right(fTimeDelta * 5.f);
	else if(fPosX >= 1800)
		m_pTransformCom->Go_Left(fTimeDelta * 5.f);
	else if(fPosY <= 820)
		m_pTransformCom->Go_Down(fTimeDelta * 5.f);
	else if(fPosY >= 1000)
		m_pTransformCom->Go_Up(fTimeDelta * 5.f);
}

void CUI_SelectArrow::Move(_float fTimeDelta)
{
	PositionUpdate(fTimeDelta);
	_bool bInput[4] = { FALSE , FALSE ,FALSE ,FALSE };

		if (m_pGameInstance->Key_Pressing(DIK_W))
		{
			m_pTransformCom->Go_Up(fTimeDelta * m_fSpeedOffset);
			bInput[0] = TRUE;
		}

		if (m_pGameInstance->Key_Pressing(DIK_S))
		{
			m_pTransformCom->Go_Down(fTimeDelta * m_fSpeedOffset);
			bInput[1] = TRUE;
		}

		if (m_pGameInstance->Key_Pressing(DIK_A))
		{
			m_pTransformCom->Go_Left(fTimeDelta * m_fSpeedOffset);
			bInput[2] = TRUE;
		}

		if (m_pGameInstance->Key_Pressing(DIK_D))
		{
			m_pTransformCom->Go_Right(fTimeDelta * m_fSpeedOffset);
			bInput[3] = TRUE;
		}

		_bool bResultInput = bInput[0] || bInput[1] || bInput[2] || bInput[3];
		bResultInput ? m_fSpeedOffset += fTimeDelta : m_fSpeedOffset = 1.f;
		
		if (m_fSpeedOffset >= 2.5f)
			m_fSpeedOffset = 2.5f;
}

CUI_SelectArrow* CUI_SelectArrow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_SelectArrow* pInstatnce = new CUI_SelectArrow(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_SelectArrow"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_SelectArrow::Clone(void* pArg)
{
	CUI_SelectArrow* pInstatnce = new CUI_SelectArrow(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_SelectArrow"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_SelectArrow::Free()
{
	__super::Free();
}
