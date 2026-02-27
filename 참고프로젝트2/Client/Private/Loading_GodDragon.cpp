#include "stdafx.h"
#include "..\Public\Loading_GodDragon.h"

#include "RenderInstance.h"
#include "GameInstance.h"

CLoading_GodDragon::CLoading_GodDragon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CLoading_GodDragon::CLoading_GodDragon(const CLoading_GodDragon& Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CLoading_GodDragon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLoading_GodDragon::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC Desc{};
	Desc.fRotationPerSec = 1.f;
	Desc.fSpeedPerSec = 10.f;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State_Position(_float3(0.f, 0.f, 1.f));
	//m_pTransformCom->Rotation({ 0.f , 1.f, 0.f }, XMConvertToRadians(100.f));

	Add_Light(_float4(-0.f, -0.f, 1.f, 0.f), _float4(0.9f, 0.9f, 1.0f, 1.0f), _float4(0.5f, 0.5f, 0.5f, 1.f), _float4(0.f, 0.f, 0.f, 1.f), "SELECT_GodDragon_Light");
	
	m_pModelCom->SetUp_Animation(5, true, 0.1f);

	m_strName = "Dragon_1";
	m_RendererDesc.strName = m_strName;
	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;

	LightDesc.vDirection = _float4(-0.15f, -0.7f, 0.5f, 0.f);
	LightDesc.vDiffuse = _float4(0.9f, 0.9f, 1.0f, 1.0f);
	LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
	LightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 1.f);
	LightDesc.pPlayerDirection = &m_iDir;
	LightDesc.strName = m_strName;

	if (FAILED(m_pRenderInstance->Add_Player_Light(m_strName, LightDesc)))
		return E_FAIL;


	return S_OK;
}

void CLoading_GodDragon::Camera_Update(_float fTimeDelta)
{

}

void CLoading_GodDragon::Update(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);

	m_fTexcoordValue -= fTimeDelta * 0.25f;

	if (m_fTexcoordValue <= 0.f)
		m_fTexcoordValue = 0.f;
}

void CLoading_GodDragon::Late_Update(_float fTimeDelta)
{
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_PLAYER, this, &m_RendererDesc);
}

HRESULT CLoading_GodDragon::Render(_float fTimeDelta)
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

		//m_pModelCom->Bind_MaterialSRV(m_pShaderCom, aiTextureType_NORMALS, "g_NormalTexture", i);

		/* 모델이 가지고 있는 뼈들 중에서 현재 렌더링할려고 했던 i번째ㅑ 메시가 사용하는 뼈들을 배열로 만들어서 쉐이더로 던져준다.  */
		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;


		if (FAILED(m_pShaderCom->Begin(6)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoading_GodDragon::Add_Light(_float4 vDirection, _float4 vDiffuse, _float4 vAmbient, _float4 vSpecular, string strName)
{
	m_RendererDesc.strName = strName;
	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;

	LightDesc.vDirection = vDirection;
	LightDesc.vDiffuse = vDiffuse;
	LightDesc.vAmbient = vAmbient;
	LightDesc.vSpecular = vSpecular;
	LightDesc.pPlayerDirection = &m_iLookDirection;
	LightDesc.strName = m_RendererDesc.strName;

	if (FAILED(m_pRenderInstance->Add_Player_Light(strName, LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoading_GodDragon::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Loading_GodDragon"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_PLN_ilm"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;
	
	

	/* Com_LimTexture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Character_OutLine_GodDragon"),
		TEXT("Com_LimTexture"), reinterpret_cast<CComponent**>(&m_pLimTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoading_GodDragon::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pLimTextureCom->Bind_ShaderResource(m_pShaderCom, "g_OutLineTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTexcoordValue", &m_fTexcoordValue, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CLoading_GodDragon* CLoading_GodDragon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLoading_GodDragon* pInstance = new CLoading_GodDragon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLoading_GodDragon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLoading_GodDragon::Clone(void* pArg)
{
	CLoading_GodDragon* pInstance = new CLoading_GodDragon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CLoading_GodDragon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoading_GodDragon::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pLimTextureCom);

	__super::Free();
}
