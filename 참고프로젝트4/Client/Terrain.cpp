#include "stdafx.h"
#include "Terrain.h"

#include "GameInstance.h"

IMPLEMENT_CLONE(CTerrain, CGameObject)
IMPLEMENT_CREATE(CTerrain)

CTerrain::CTerrain(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CGameObject(pDevice, pContext)
{

}

CTerrain::CTerrain(const CTerrain& rhs)
	: CGameObject(rhs)
{
}

HRESULT CTerrain::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CTerrain::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	if (FAILED(Ready_Components()))
		RETURN_EFAIL;

	return S_OK;
}

void CTerrain::Priority_Tick(_cref_time fTimeDelta)
{
}

void CTerrain::Tick(_cref_time fTimeDelta)
{
}

void CTerrain::Late_Tick(_cref_time fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this())))
		return;
}

HRESULT CTerrain::Render()
{
	if (FAILED(Bind_ShaderResources()))
		RETURN_EFAIL;

	/* 이 셰이더에 0번째 패스로 그릴꺼야. */
	m_pShaderCom->Begin(0);

	/* 내가 그릴려고하는 정점, 인덱스버퍼를 장치에 바인딩해. */
	m_pVIBufferCom->Bind_VIBuffers();

	/* 바인딩된 정점, 인덱스를 그려. */
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CTerrain::Ready_Components()
{	
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CreateLevelIndex(), TEXT("Prototype_Component_Shader_VtxNorTex"),
		TEXT("Com_Shader"), & (m_pShaderCom))))
		RETURN_EFAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CreateLevelIndex(), TEXT("Prototype_Component_VIBuffer_Terrain"),
		TEXT("Com_VIBuffer"), & (m_pVIBufferCom))))
		RETURN_EFAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CreateLevelIndex(), TEXT("Prototype_Component_Texture_Terrain"),
		TEXT("Com_Texture"), & (m_pTextureCom))))
		RETURN_EFAIL;

	///* For.Com_Brush */
	//if (FAILED(__super::Add_Component(m_pGameInstance->Get_CreateLevelIndex(), TEXT("Prototype_Component_Texture_Terrain_Brush"),
	//	TEXT("Com_Brush"), & (m_pTextureCom[TYPE_BRUSH]))))
	//	RETURN_EFAIL;

	return S_OK;
}

HRESULT CTerrain::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom.get(), "g_WorldMatrix")))
		RETURN_EFAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &(m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_VIEW)))))
		RETURN_EFAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &(m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_PROJ)))))
		RETURN_EFAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom.get(), "g_DiffuseTexture")))
		RETURN_EFAIL;
	/*if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_BrushTexture", 0)))
		RETURN_EFAIL;*/
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
	//	RETURN_EFAIL;

	return S_OK;
}




void CTerrain::Free()
{
	__super::Free();

	
}
