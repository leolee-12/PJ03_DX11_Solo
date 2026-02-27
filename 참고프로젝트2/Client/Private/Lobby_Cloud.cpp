#include "stdafx.h"
#include "..\Public\Lobby_Cloud.h"

#include "RenderInstance.h"
#include "GameInstance.h"

CLobby_Cloud::CLobby_Cloud(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CLobby_Cloud::CLobby_Cloud(const CLobby_Cloud& Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CLobby_Cloud::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLobby_Cloud::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//CLOUD_DESC* pDesc = static_cast<CLOUD_DESC*>(pArg);
	//_uint iNumObj = pDesc->iNumObject;

	_wstring strPrototypeTag = {};
	//Default_Setting(iNumObj, strPrototypeTag);

	if (FAILED(Ready_Components(strPrototypeTag)))
		return E_FAIL;


	m_pTransformCom->Set_State_Position(_float3(0.f, 0.f, 0.f));

	return S_OK;
}

void CLobby_Cloud::Camera_Update(_float fTimeDelta)
{

}

void CLobby_Cloud::Update(_float fTimeDelta)
{
}

void CLobby_Cloud::Late_Update(_float fTimeDelta)
{
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CLobby_Cloud::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
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

void CLobby_Cloud::Default_Setting(_uint iNumObj,_wstring& strPrototypeTag)
{
	switch (iNumObj)
	{
	case 0:
		strPrototypeTag = TEXT("Prototype_Component_Model_Lobby_Cloud0");
		break;

	case 1:
		strPrototypeTag = TEXT("Prototype_Component_Model_Lobby_Cloud1");
		break;

	case 2:
		strPrototypeTag = TEXT("Prototype_Component_Model_Lobby_Cloud2");
		break;

	case 3:
		strPrototypeTag = TEXT("Prototype_Component_Model_Lobby_Cloud3");
		break;

	case 4:
		strPrototypeTag = TEXT("Prototype_Component_Model_Lobby_Cloud4");
		break;
	}
}

HRESULT CLobby_Cloud::Ready_Components(_wstring strPrototypeTag)
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_Cloud2"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLobby_Cloud::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CLobby_Cloud* CLobby_Cloud::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLobby_Cloud* pInstance = new CLobby_Cloud(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLobby_Cloud"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLobby_Cloud::Clone(void* pArg)
{
	CLobby_Cloud* pInstance = new CLobby_Cloud(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CLobby_Cloud"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLobby_Cloud::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	__super::Free();
}
