#include "stdafx.h"
#include "..\Public\Lobby_Ranking.h"

#include "RenderInstance.h"
#include "GameInstance.h"

CLobby_Ranking::CLobby_Ranking(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CLobby_Ranking::CLobby_Ranking(const CLobby_Ranking& Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CLobby_Ranking::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLobby_Ranking::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State_Position(_float3(16.230f, 0.046f, 36.560f));
	m_pTransformCom->Rotation({ 0.f, 1.f,0.f }, XMConvertToRadians(201.f));
	m_pTransformCom->Set_Scaled( 2.5f, 2.5f,2.5f);

	return S_OK;
}

void CLobby_Ranking::Camera_Update(_float fTimeDelta)
{

}

void CLobby_Ranking::Update(_float fTimeDelta)
{
}

void CLobby_Ranking::Late_Update(_float fTimeDelta)
{
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CLobby_Ranking::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pDiffTexture[i]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
			return E_FAIL;

		/* 모델이 가지고 있는 머테리얼 중 i번째 메시가 사용해야하는 머테리얼구조체의 aiTextureType_DIFFUSE번째 텍스쳐를 */
		/* m_pShaderCom에 있는 g_DiffuseTexture변수에 던져. */
		if (FAILED(m_pModelCom->Bind_MaterialSRV(m_pShaderCom, aiTextureType_DIFFUSE, "g_DiffuseTexture", i)))
			return E_FAIL;
		// m_pModelCom->Bind_MaterialSRV(m_pShaderCom, aiTextureType_NORMALS, "g_NormalTexture", i);

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLobby_Ranking::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_Ranking"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_DiffTexture */
	if (FAILED(__super::Add_Component(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Lobby_lob_singboard"),
		TEXT("Com_DiffTexture0"), reinterpret_cast<CComponent**>(&m_pDiffTexture[0]))))
		return E_FAIL;

	/* Com_DiffTexture */
	if (FAILED(__super::Add_Component(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Lobby_lob_ranking"),
		TEXT("Com_DiffTexture1"), reinterpret_cast<CComponent**>(&m_pDiffTexture[1]))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLobby_Ranking::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CLobby_Ranking* CLobby_Ranking::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLobby_Ranking* pInstance = new CLobby_Ranking(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLobby_Ranking"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLobby_Ranking::Clone(void* pArg)
{
	CLobby_Ranking* pInstance = new CLobby_Ranking(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CLobby_Ranking"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLobby_Ranking::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	for(int i = 0 ; i < 2; i++)
		Safe_Release(m_pDiffTexture[i]);

	__super::Free();
}
