#include "stdafx.h"
#include "..\Public\SpaceSky.h"

#include "RenderInstance.h"
#include "GameInstance.h"

CSpaceSky::CSpaceSky(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject { pDevice, pContext }
{

}

CSpaceSky::CSpaceSky(const CSpaceSky & Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CSpaceSky::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSpaceSky::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	CMap_Manager::Get_Instance()->Push_MapObject(CMap_Manager::MAP_SPACE,
		static_cast<CMap_Manager::Map_Object_Key*>(pArg)->m_PrototypeKey, this);

	return S_OK;
}

void CSpaceSky::Camera_Update(_float fTimeDelta)
{
	
}

void CSpaceSky::Update(_float fTimeDelta)
{
	m_fAccTime += fTimeDelta;

	if (m_isStarSwitch == false)
	{
		m_fMaskStar_Value_1 += fTimeDelta * 0.2f;
		m_fMaskStar_Value_2 -= fTimeDelta * 0.2f;

		if (m_fMaskStar_Value_1 >= 0.3f)
		{
			m_isStarSwitch = true;
			m_fMaskStar_Value_1 = 0.3f;
			m_fMaskStar_Value_2 = 0.f;
		}
	}
	else
	{
		m_fMaskStar_Value_1 -= fTimeDelta * 0.2f;
		m_fMaskStar_Value_2 += fTimeDelta * 0.2f;

		if (m_fMaskStar_Value_1 <= 0)
		{
			m_isStarSwitch = false;
			m_fMaskStar_Value_1 = 0.f;
			m_fMaskStar_Value_2 = 0.3f;
		}
	}
}

void CSpaceSky::Late_Update(_float fTimeDelta)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pGameInstance->Get_CamPosition_Vector());
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_PRIORITY, this);
}

HRESULT CSpaceSky::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CSpaceSky::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxSpace"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_SpaceSky_Diffuse"),
		TEXT("Com_Texture_Diffuse"), reinterpret_cast<CComponent**>(&m_pTextureCom_Diffuse))))
		return E_FAIL;
	
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_SpaceSky_Star1"),
		TEXT("Com_Texture_Star1"), reinterpret_cast<CComponent**>(&m_pTextureCom_Star1))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_SpaceSky_Star2"),
		TEXT("Com_Texture_Star2"), reinterpret_cast<CComponent**>(&m_pTextureCom_Star2))))
		return E_FAIL;


	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SpaceSky"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CSpaceSky::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	if (FAILED(m_pTextureCom_Diffuse->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pTextureCom_Star1->Bind_ShaderResource(m_pShaderCom, "g_MaskStar1", 0)))
		return E_FAIL;
	
	if (FAILED(m_pTextureCom_Star2->Bind_ShaderResource(m_pShaderCom, "g_MaskStar2", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_MaskStar_Value_1", &m_fMaskStar_Value_1, sizeof(float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_MaskStar_Value_2", &m_fMaskStar_Value_2, sizeof(float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Time", &m_fAccTime, sizeof(float))))
		return E_FAIL;
	
	return S_OK;
}

CSpaceSky * CSpaceSky::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSpaceSky*		pInstance = new CSpaceSky(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSpaceSky"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSpaceSky::Clone(void * pArg)
{
	CSpaceSky*		pInstance = new CSpaceSky(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSpaceSky"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSpaceSky::Free()
{


	/*for (size_t i = 0; i < SPACE_END; i++)
	{		
		Safe_Release(m_pTextureCom[i]);
	}*/

	Safe_Release(m_pTextureCom_Diffuse);
	Safe_Release(m_pTextureCom_Star1);
	Safe_Release(m_pTextureCom_Star2);
	
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	__super::Free();
}
