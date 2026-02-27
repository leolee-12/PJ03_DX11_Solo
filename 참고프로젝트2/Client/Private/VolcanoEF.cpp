#include "stdafx.h"
#include "..\Public\VolcanoEF.h"

#include "RenderInstance.h"
#include "GameInstance.h"

CVolcanoEF::CVolcanoEF(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject { pDevice, pContext }
{

}

CVolcanoEF::CVolcanoEF(const CVolcanoEF & Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CVolcanoEF::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVolcanoEF::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//m_pTransformCom->Set_Scaled(-1.f, 1.f, 1.f);
	//m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(180.f));

	CMap_Manager::Get_Instance()->Push_MapObject(CMap_Manager::MAP_VOLCANO,
		static_cast<CMap_Manager::Map_Object_Key*>(pArg)->m_PrototypeKey, this);

	return S_OK;
}

void CVolcanoEF::Camera_Update(_float fTimeDelta)
{
	m_fAccTime += fTimeDelta;
}


void CVolcanoEF::Update(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_0))
	{
		//if (g_Pass == 4)
		//	g_Pass = 14;
		//else
		//	g_Pass = 4;

	}
}

_int iCount = 0;
void CVolcanoEF::Late_Update(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_0))
	{
		iCount++;
		_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
		if (iCount >= iNumMeshes)
			iCount = 0;
	}
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_SPACEMAP, this);
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_CUTSCENE_OBJECT, this);
}

HRESULT CVolcanoEF::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	//m_pTransformCom->Set_Scaled(-1.f, 1.f, 1.f);
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (m_isRenderSpaceMap == true)
	{
		if (FAILED(m_pModelCom->Bind_MaterialSRV(m_pShaderCom, aiTextureType_DIFFUSE, "g_DiffuseTexture", 2)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_EastColor", &m_vEastColor, sizeof(_float3))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(3)))
			return E_FAIL;
		if (FAILED(m_pModelCom->Render(2)))
			return E_FAIL;

		m_pTransformCom->Set_Scaled(15.f, 15.f, 1.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, -105.f, 0.f, 1.f));
		if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
			return E_FAIL;

		if (FAILED(m_pTextureCom_Diffuse->Bind_ShaderResource(m_pShaderCom, "g_EastGlowTexture", 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(21)))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render(0)))
			return E_FAIL;

		m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
		if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_MaterialSRV(m_pShaderCom, aiTextureType_DIFFUSE, "g_DiffuseTexture", 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(0)))
			return E_FAIL;

		m_isRenderSpaceMap = !m_isRenderSpaceMap;
	}
	else
	{

		_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

		for (size_t i = 0; i < iNumMeshes; i++)
		{
			if (i == 0 || i == 2)
				continue;
			//if (i != iCount)
			//   continue;

			if (FAILED(m_pModelCom->Bind_MaterialSRV(m_pShaderCom, aiTextureType_DIFFUSE, "g_DiffuseTexture", i)))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(22)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render(i)))
				return E_FAIL;
		}

		m_isRenderSpaceMap = !m_isRenderSpaceMap;
	}
	return S_OK;
}

HRESULT CVolcanoEF::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxVolcano"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_sphere01"),
		TEXT("Com_Texture_Diffuse"), reinterpret_cast<CComponent**>(&m_pTextureCom_Diffuse))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_VolcanoEF"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_acmn_povot_plane00"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CVolcanoEF::Bind_ShaderResources()
{
	
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;



	//if (FAILED(m_pShaderCom->Bind_RawValue("g_fSpriteSize", &m_fSpriteSize, sizeof(_float2))))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_fSpriteCurPos", &m_fSpriteCurPos, sizeof(_float2))))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_Time", &m_fAccTime, sizeof(float))))
	//	return E_FAIL;
	

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Bind_RawValue("g_Time", &m_fAccTime, sizeof(float))))
		return E_FAIL;

	LIGHT_DESC* pLightDesc = m_pRenderInstance->Get_LightDesc(CLight_Manager::LIGHT_BACKGROUND, 0);

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
	//	return E_FAIL;

	return S_OK;
}

CVolcanoEF * CVolcanoEF::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CVolcanoEF*		pInstance = new CVolcanoEF(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CVolcanoEF"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CVolcanoEF::Clone(void * pArg)
{
	CVolcanoEF*		pInstance = new CVolcanoEF(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CVolcanoEF"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVolcanoEF::Free()
{
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom_Diffuse);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	__super::Free();
}
