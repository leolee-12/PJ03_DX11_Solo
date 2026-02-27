#include "stdafx.h"

#include "UI_Cursor.h"
#include "RenderInstance.h"

CUI_Cursor::CUI_Cursor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{ pDevice ,pContext }
{
}

CUI_Cursor::CUI_Cursor(const CUI_Cursor& Prototype)
	:CUIObject{ Prototype }
{
}

HRESULT CUI_Cursor::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Cursor::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

void CUI_Cursor::Camera_Update(_float fTimeDelta)
{
	POINT MousePos;
	GetCursorPos(&MousePos);
	ScreenToClient(g_hWnd, &MousePos);

	/*if (m_pGameInstance->Key_Down('O'))
	{
		_int iX = MousePos.x;
		_int iY = MousePos.y;

		_uint iTmp = 0;
	}*/

	__super::Set_UI_Setting(50.f, 50.f, MousePos.x, MousePos.y);
}

void CUI_Cursor::Update(_float fTimeDelta)
{
	
}

void CUI_Cursor::Late_Update(_float fTimeDelta)
{
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_Cursor::Render(_float fTimeDelta)
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Cursor::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Cursor"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;
}

CUI_Cursor* CUI_Cursor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Cursor* pInstatnce = new CUI_Cursor(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_Cursor"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_Cursor::Clone(void* pArg)
{
	CUI_Cursor* pInstatnce = new CUI_Cursor(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_Cursor"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_Cursor::Free()
{
	__super::Free();
}
