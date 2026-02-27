//Sky.cpp
#include "stdafx.h"
#include "SkyBox.h"

#include "GameInstance.h"

IMPLEMENT_CLONE(CSkyBox, CGameObject)
IMPLEMENT_CREATE_EX1(CSkyBox, const wstring&, strTextureTag)

CSkyBox::CSkyBox(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CGameObject(pDevice, pContext)
{

}

CSkyBox::CSkyBox(const CSkyBox& rhs)
	: CGameObject(rhs), m_TextureProtoTag(rhs.m_TextureProtoTag)
{
}

HRESULT CSkyBox::Initialize_Prototype(const wstring& strTextureTag)
{
	m_TextureProtoTag = strTextureTag;
	return S_OK;
}

HRESULT CSkyBox::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	if (FAILED(Ready_Components()))
		RETURN_EFAIL;

	return S_OK;
}

void CSkyBox::Priority_Tick(_cref_time fTimeDelta)
{
}	

void CSkyBox::Tick(_cref_time fTimeDelta)
{

}

void CSkyBox::Late_Tick(_cref_time fTimeDelta)
{	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_pGameInstance->Get_CamPosition()));
	
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_PRIORITY, shared_from_this())))
		return;
}

HRESULT CSkyBox::Render()
{
	if (FAILED(Bind_ShaderResources()))
		RETURN_EFAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Bind_VIBuffers();

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CSkyBox::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxCube"),
		TEXT("Com_Shader"), &(m_pShaderCom))))
		RETURN_EFAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"),
		TEXT("Com_VIBuffer"), & (m_pVIBufferCom))))
		RETURN_EFAIL;

	/* For.Com_Material */	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Material"),
		TEXT("Com_Material"), &(m_pMaterialCom))))
		RETURN_EFAIL;

	m_pMaterialCom->Ready_CustomSingleMaterial(MATERIALTYPE::MATERIATYPE_DIFFUSE, m_TextureProtoTag, 1);

	return S_OK;
}

HRESULT CSkyBox::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom.get(), "g_WorldMatrix")))
		RETURN_EFAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_VIEW))))
		RETURN_EFAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_PROJ))))
		RETURN_EFAIL;

	if (FAILED(m_pMaterialCom->Bind_SRVToShader(m_pShaderCom.get(), "g_Texture", MATERIALTYPE::MATERIATYPE_DIFFUSE, 0))) RETURN_EFAIL;

	return S_OK;
}



void CSkyBox::Free()
{
	__super::Free();

}