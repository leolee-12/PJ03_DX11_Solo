#include "stdafx.h"
#include "..\Public\Frieza_Metal.h"
#include "RenderInstance.h"
#include "GameInstance.h"
#include "Play_Frieza.h"

CFrieza_Metal::CFrieza_Metal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CFrieza_Metal::CFrieza_Metal(const CFrieza_Metal& Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CFrieza_Metal::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFrieza_Metal::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	Frieza_Metal_DESC* pDesc = static_cast<Frieza_Metal_DESC*>(pArg);

	m_pParentMatrix = pDesc->pParentMatrix;
	m_pSocketMatrix = pDesc->pSocketBoneMatrix;
	m_pLookDirection = pDesc->pLookDirection;
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	CTransform::TRANSFORM_DESC tDesc{};
	tDesc.fSpeedPerSec = 1.f;
	tDesc.fRotationPerSec = 1.f;
	m_pTransformCom->SetUp_TransformDesc(&tDesc);
	m_pLookAtTransformCom->SetUp_TransformDesc(&tDesc);


	m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pDesc->vPosition));
	

	return S_OK;
}

void CFrieza_Metal::Player_Update(_float fTimeDelta)
{
	/* Test Position */
	
	//if (m_pGameInstance->Key_Pressing(DIK_W))
	//{
	//	m_pTransformCom->Go_Straight(fTimeDelta * 0.1f);
	//}
	//if (m_pGameInstance->Key_Pressing(DIK_S))
	//{
	//	m_pTransformCom->Go_Backward(fTimeDelta * 0.1f);
	//}
	//if (m_pGameInstance->Key_Pressing(DIK_A))
	//{
	//	m_pTransformCom->Go_Left(fTimeDelta * 0.1f);
	//}
	//if (m_pGameInstance->Key_Pressing(DIK_D))
	//{
	//	m_pTransformCom->Go_Right(fTimeDelta * 0.1f);
	//}
	//if (m_pGameInstance->Key_Pressing(DIK_Q))
	//{
	//	m_pTransformCom->Go_Up(fTimeDelta * 0.1f);
	//}
	//if (m_pGameInstance->Key_Pressing(DIK_E))
	//{
	//	m_pTransformCom->Go_Down(fTimeDelta * 0.1f);
	//}

	//if (m_pGameInstance->Key_Pressing(DIK_UP))
	//{
	//	_float3 vScale = m_pTransformCom->Get_Scaled();
	//	vScale.y += fTimeDelta * 0.1f;
	//	m_pTransformCom->Set_Scaled(vScale.x, vScale.y, vScale.z);
	//}
	//if (m_pGameInstance->Key_Pressing(DIK_DOWN))
	//{
	//	_float3 vScale = m_pTransformCom->Get_Scaled();
	//	vScale.y -= fTimeDelta * 0.1f;
	//	m_pTransformCom->Set_Scaled(vScale.x, vScale.y, vScale.z);
	//}

	//if (m_pGameInstance->Key_Pressing(DIK_LEFT))
	//{
	//	_float3 vScale = m_pTransformCom->Get_Scaled();
	//	vScale.x -= fTimeDelta * 0.1f;
	//	m_pTransformCom->Set_Scaled(vScale.x, vScale.y, vScale.z);
	//}
	//if (m_pGameInstance->Key_Pressing(DIK_RIGHT))
	//{
	//	_float3 vScale = m_pTransformCom->Get_Scaled();
	//	vScale.x += fTimeDelta * 0.1f;
	//	m_pTransformCom->Set_Scaled(vScale.x, vScale.y, vScale.z);
	//}



	//_vector LocalPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//_float4 ChangeTaetPos;
	//XMStoreFloat4(&ChangeTaetPos, LocalPos);

	//_float3 vScale = m_pTransformCom->Get_Scaled();

	//int a = 10;
	
}

void CFrieza_Metal::Camera_Update(_float fTimeDelta)
{

}

void CFrieza_Metal::Update(_float fTimeDelta)
{
	//__super::Update(fTimeDelta);

	if (m_pGameInstance->Key_Down(DIK_F6))
	{
		if(m_iTestRGIndex == CRenderer::RG_UI)
			m_iTestRGIndex = CRenderer::RG_PLAYER_METALLIC;
		else
			m_iTestRGIndex = CRenderer::RG_UI;
	}
	
}

void CFrieza_Metal::Late_Update(_float fTimeDelta)
{
	//m_pRenderInstance->Add_RenderObject(CRenderer::RG_PLAYER_METALLIC, this);
	m_pRenderInstance->Add_RenderObject(static_cast<CRenderer::RENDERGROUP>(m_iTestRGIndex), this);
}

HRESULT CFrieza_Metal::Render(_float fTimeDelta)
{
	_matrix		SocketBoneMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; i++)
		SocketBoneMatrix.r[i] = XMVector3Normalize(SocketBoneMatrix.r[i]);

	if (*m_pLookDirection == 1)
		m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(0.f));
	else
		m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketBoneMatrix * XMLoadFloat4x4(m_pParentMatrix));
	m_pLookAtTransformCom->Set_WorldMatrix(m_WorldMatrix);

	_vector vCamPos = m_pGameInstance->Get_CamPosition_Vector();
	if (*m_pLookDirection != 1)
	{
		_vector vPos = m_pLookAtTransformCom->Get_State(CTransform::STATE_POSITION);

		_vector vDir = vPos - vCamPos;
		vCamPos += vDir * 2.f;
	}
	m_pLookAtTransformCom->LookAt(vCamPos);

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFrieza_Metal::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxShaderRect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FRN_metal"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	m_pLookAtTransformCom = CTransform::Create(m_pDevice, m_pContext);
	
	return S_OK;
}

HRESULT CFrieza_Metal::Bind_ShaderResources()
{
	if (FAILED(m_pLookAtTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CFrieza_Metal* CFrieza_Metal::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFrieza_Metal* pInstance = new CFrieza_Metal(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CFrieza_Metal"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFrieza_Metal::Clone(void* pArg)
{
	CFrieza_Metal* pInstance = new CFrieza_Metal(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CFrieza_Metal"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFrieza_Metal::Free()
{
	__super::Free();

	Safe_Release(m_pLookAtTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
}
