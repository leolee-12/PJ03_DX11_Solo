#include "stdafx.h"
#include "TerrainD.h"

#include "GameInstance.h"

CTerrainD::CTerrainD(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{

}

CTerrainD::CTerrainD(const CTerrainD& rhs)
	: CGameObject(rhs)
{
}

HRESULT CTerrainD::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CTerrainD::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	return S_OK;
}

void CTerrainD::Priority_Tick(_float fTimeDelta)
{
}

void CTerrainD::Tick(_float fTimeDelta)
{

}

void CTerrainD::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;
}

HRESULT CTerrainD::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	/* 이 셰이더에 0번째 패스로 그릴꺼야. */
	
	m_pShaderCom->Begin(m_RenderState);

	/* 내가 그릴려고하는 정점, 인덱스버퍼를 장치에 바인딩해. */
	m_pVIBufferCom->Bind_VIBuffers();

	/* 바인딩된 정점, 인덱스를 그려. */
	m_pVIBufferCom->Render();

	return S_OK;
}



HRESULT CTerrainD::Ready_Components(void* pArg)
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CreateLevelIndex(), TEXT("Prototype_Component_Shader_VtxNorTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	/*CVIBuffer_TerrainD::VTXTERRAIND_DESC* Desc = (VTXTERRAIND_DESC*)pArg;
	CVIBuffer_TerrainD::VTXTERRAIND_DESC VTXDesc;
	VTXDesc.iVerticesXNum = Desc->iVerticesXNum;
	VTXDesc.iVerticesZNum = Desc->iVerticesZNum;*/

	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CreateLevelIndex(), TEXT("Prototype_Component_VIBuffer_TerrainD"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), pArg)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CreateLevelIndex(), TEXT("Prototype_Component_Texture_Terrain"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_Brush */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CreateLevelIndex(), TEXT("Prototype_Component_Texture_Terrain_Brush"),
		TEXT("Com_Brush"), reinterpret_cast<CComponent**>(&m_pTextureCom[TYPE_BRUSH]))))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrainD::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &(m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_VIEW)))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &(m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_PROJ)))))
		return E_FAIL;
	if (FAILED(m_pTextureCom[TYPE_DIFFUSE]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;
	if (FAILED(m_pTextureCom[TYPE_BRUSH]->Bind_ShaderResource(m_pShaderCom, "g_BrushTexture", 0)))
		return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
	//	return E_FAIL;

	return S_OK;
}


CTerrainD* CTerrainD::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTerrainD* pInstance = new CTerrainD(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTerrainD");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CTerrainD::Clone(void* pArg)
{
	CTerrainD* pInstance = new CTerrainD(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTerrainD");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTerrainD::Free()
{
	__super::Free();

	for (size_t i = 0; i < (size_t) TYPE_END; i++)
	{
		Safe_Release(m_pTextureCom[i]);
	}

}
