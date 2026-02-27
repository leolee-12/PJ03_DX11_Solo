#include "stdafx.h"

#include "Lobby_Goku_RunEff.h"
#include "RenderInstance.h"
#include "GameInstance.h"

CLobby_Goku_RunEff::CLobby_Goku_RunEff(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject{ pDevice ,pContext }
{
}

CLobby_Goku_RunEff::CLobby_Goku_RunEff(const CLobby_Goku_RunEff& Prototype)
	:CGameObject{ Prototype }
{
}

HRESULT CLobby_Goku_RunEff::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLobby_Goku_RunEff::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	CTransform* vTargetTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_LOBBY, TEXT("Layer_Lobby_Goku"), TEXT("Com_Transform")));
	_vector vTargetLook = -vTargetTransform->Get_State(CTransform::STATE_LOOK);
	vTargetLook = XMVectorSetY(vTargetLook, 1.f);

	m_iTextureIndex = rand() % 4;
	m_vDir = XMVector3Normalize(vTargetLook);
	Chase(-0.5f);

	return S_OK;
}

void CLobby_Goku_RunEff::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CLobby_Goku_RunEff::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	m_fDestroyTimer += fTimeDelta;

	if (m_fDestroyTimer >= 1.f)
	{
		m_fDestroyTimer = 0.f;
		Destory();
	}

	m_pVIBufferCom->MoveDir(m_vDir,fTimeDelta);
}

void CLobby_Goku_RunEff::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
}

HRESULT CLobby_Goku_RunEff::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;;

	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLobby_Goku_RunEff::Ready_Components()
{
	/* For.Com_SHader */
	if (FAILED(__super::Add_Component(LEVEL_LOBBY, TEXT("Prototype_Component_Shader_Particle_VtxPoint"),
		TEXT("Com_SHader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_UI_Lobby_Goku_RunEff"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_LOBBY, TEXT("Prototype_Component_VIBuffer_Particle_Lobby_Goku_RunEff"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CLobby_Goku_RunEff::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureIndex)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

void CLobby_Goku_RunEff::Chase(_float fOffsetZ)
{
	CTransform* vTargetTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_LOBBY, TEXT("Layer_Lobby_Goku"), TEXT("Com_Transform")));
	_vector vTargetPos = vTargetTransform->Get_State(CTransform::STATE_POSITION);
	_vector vTargetLook = vTargetTransform->Get_State(CTransform::STATE_LOOK);

	_vector vTargetOffsetLook = vTargetLook * fOffsetZ;

	vTargetPos += vTargetOffsetLook;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vTargetPos);
}

CLobby_Goku_RunEff* CLobby_Goku_RunEff::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLobby_Goku_RunEff* pInstatnce = new CLobby_Goku_RunEff(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLobby_Goku_RunEff"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CLobby_Goku_RunEff::Clone(void* pArg)
{
	CLobby_Goku_RunEff* pInstatnce = new CLobby_Goku_RunEff(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CLobby_Goku_RunEff"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CLobby_Goku_RunEff::Free()
{
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);

	__super::Free();
}
