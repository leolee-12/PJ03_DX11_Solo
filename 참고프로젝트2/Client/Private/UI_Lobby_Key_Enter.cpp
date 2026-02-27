#include "stdafx.h"

#include "UI_Lobby_Key_Enter.h"
#include "RenderInstance.h"
#include "GameInstance.h"

CUI_Lobby_Key_Enter::CUI_Lobby_Key_Enter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject{ pDevice ,pContext }
{
}

CUI_Lobby_Key_Enter::CUI_Lobby_Key_Enter(const CUI_Lobby_Key_Enter& Prototype)
	:CGameObject{ Prototype }
{
}

HRESULT CUI_Lobby_Key_Enter::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Lobby_Key_Enter::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	ENTER_DESC* pDesc =static_cast<ENTER_DESC*>(pArg);

	CTransform* pTargetTransform = pDesc->pTransform;
	_vector vTargetPos = pTargetTransform->Get_State(CTransform::STATE_POSITION);
	_vector vOffset = XMVectorSetY(vTargetPos, XMVectorGetY(vTargetPos) + 3.75f);

	m_pTransformCom->Set_Scaled(-2.f,2.f, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vOffset);

	m_vOriginPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	m_bIsActive = FALSE;
		
	return S_OK;
}

void CUI_Lobby_Key_Enter::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_Lobby_Key_Enter::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	CTransform* pTargetTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_LOBBY, TEXT("Layer_Main_Camera_Lobby"), TEXT("Com_Transform")));
	_vector vTargetPos = pTargetTransform->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->LookAt(vTargetPos);
	m_pTransformCom->Set_Scaled(-2.f, 2.f, 1.f);

	m_bPosSwitch ? m_fOffsetPos += fTimeDelta : m_fOffsetPos -= fTimeDelta;
	
	if (m_fOffsetPos >= 0.5f)
		m_bPosSwitch = FALSE;
	if (m_fOffsetPos <= 0.f)
		m_bPosSwitch = TRUE;
	
	_vector vOffsetPosY  =XMVectorSetY(m_vOriginPos, XMVectorGetY(m_vOriginPos) + m_fOffsetPos);
	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,vOffsetPosY);
}

void CUI_Lobby_Key_Enter::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_Lobby_Key_Enter::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Lobby_Key_Enter::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI_VtxRect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_UI_Lobby_Key_Enter"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Lobby_Key_Enter::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	return S_OK;
}

CUI_Lobby_Key_Enter* CUI_Lobby_Key_Enter::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Lobby_Key_Enter* pInstatnce = new CUI_Lobby_Key_Enter(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_Lobby_Key_Enter"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_Lobby_Key_Enter::Clone(void* pArg)
{
	CUI_Lobby_Key_Enter* pInstatnce = new CUI_Lobby_Key_Enter(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_Lobby_Key_Enter"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_Lobby_Key_Enter::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);

	__super::Free();
}
