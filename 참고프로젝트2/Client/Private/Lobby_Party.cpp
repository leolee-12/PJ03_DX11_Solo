#include "stdafx.h"
#include "..\Public\Lobby_Party.h"

#include "RenderInstance.h"
#include "GameInstance.h"

CLobby_Party::CLobby_Party(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CLobby_Party::CLobby_Party(const CLobby_Party& Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CLobby_Party::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLobby_Party::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State_Position(_float3(-41.535f, 2.046f, 41.200f));
	m_pTransformCom->Rotation({ 0.f, 1.f, 0.f }, XMConvertToRadians(-180.191f));
	m_pTransformCom->Set_Scaled(1.5f, 1.5f, 1.5f);

	return S_OK;
}

void CLobby_Party::Camera_Update(_float fTimeDelta)
{

}

void CLobby_Party::Update(_float fTimeDelta)
{
	m_fTexcoordValue += fTimeDelta;
	if (m_fTexcoordValue >= 1.f)
		m_fTexcoordValue = 0.f;
}

void CLobby_Party::Late_Update(_float fTimeDelta)
{
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CLobby_Party::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pDiffTexture[i]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
			return E_FAIL;

		m_iShaderIndex = 0;

		if (i == 6)
			m_iShaderIndex = 4;
		else if(i == 7)
			m_iShaderIndex = 5;

		/* 모델이 가지고 있는 머테리얼 중 i번째 메시가 사용해야하는 머테리얼구조체의 aiTextureType_DIFFUSE번째 텍스쳐를 */
		/* m_pShaderCom에 있는 g_DiffuseTexture변수에 던져. */
		if (FAILED(m_pModelCom->Bind_MaterialSRV(m_pShaderCom, aiTextureType_DIFFUSE, "g_DiffuseTexture", i)))
			return E_FAIL;
		// m_pModelCom->Bind_MaterialSRV(m_pShaderCom, aiTextureType_NORMALS, "g_NormalTexture", i);

		if (FAILED(m_pShaderCom->Begin(m_iShaderIndex)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLobby_Party::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_Party"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_DiffTexture */
	if (FAILED(__super::Add_Component(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Lobby_lob_PTree"),
		TEXT("Com_DiffTexture0"), reinterpret_cast<CComponent**>(&m_pDiffTexture[0]))))
		return E_FAIL;

	/* Com_DiffTexture */
	if (FAILED(__super::Add_Component(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Lobby_lob_PTree"),
		TEXT("Com_DiffTexture1"), reinterpret_cast<CComponent**>(&m_pDiffTexture[1]))))
		return E_FAIL;

	/* Com_DiffTexture */
	if (FAILED(__super::Add_Component(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Lobby_lob_PIlandSide"),
		TEXT("Com_DiffTexture2"), reinterpret_cast<CComponent**>(&m_pDiffTexture[2]))))
		return E_FAIL;

	/* Com_DiffTexture */
	if (FAILED(__super::Add_Component(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Lobby_lob_KameH"),
		TEXT("Com_DiffTexture3"), reinterpret_cast<CComponent**>(&m_pDiffTexture[3]))))
		return E_FAIL;

	/* Com_DiffTexture */
	if (FAILED(__super::Add_Component(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Lobby_lob_PTree"),
		TEXT("Com_DiffTexture4"), reinterpret_cast<CComponent**>(&m_pDiffTexture[4]))))
		return E_FAIL;

	/* Com_DiffTexture */
	if (FAILED(__super::Add_Component(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Lobby_lob_PGround"),
		TEXT("Com_DiffTexture5"), reinterpret_cast<CComponent**>(&m_pDiffTexture[5]))))
		return E_FAIL;

	/* Com_DiffTexture */
	if (FAILED(__super::Add_Component(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Lobby_lob_PWaterFall"),
		TEXT("Com_DiffTexture6"), reinterpret_cast<CComponent**>(&m_pDiffTexture[6]))))
		return E_FAIL;

	/* Com_DiffTexture */
	if (FAILED(__super::Add_Component(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Lobby_lob_PSea"),
		TEXT("Com_DiffTexture7"), reinterpret_cast<CComponent**>(&m_pDiffTexture[7]))))
		return E_FAIL;

	/* Com_DiffTexture */
	if (FAILED(__super::Add_Component(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Lobby_lob_PWaterSurface"),
		TEXT("Com_WaterSurTex"), reinterpret_cast<CComponent**>(&m_pWaterSur))))
		return E_FAIL;

	/* Com_WaterLightTex */
	if (FAILED(__super::Add_Component(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Lobby_lob_PWaterLight"),
		TEXT("Com_WaterLightTex"), reinterpret_cast<CComponent**>(&m_pWaterLight))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLobby_Party::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fTexcoordValue, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pWaterSur->Bind_ShaderResource(m_pShaderCom, "g_SurfaceTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pWaterLight->Bind_ShaderResource(m_pShaderCom, "g_LightTexture", 0)))
		return E_FAIL;

	//Prototype_Component_Texture_Lobby_lob_PWaterLight

	return S_OK;
}

CLobby_Party* CLobby_Party::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLobby_Party* pInstance = new CLobby_Party(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLobby_Party"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLobby_Party::Clone(void* pArg)
{
	CLobby_Party* pInstance = new CLobby_Party(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CLobby_Party"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLobby_Party::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	for(int i = 0 ; i < 8 ;++i)
		Safe_Release(m_pDiffTexture[i]);

	Safe_Release(m_pWaterSur);
	Safe_Release(m_pWaterLight);

	__super::Free();
}
