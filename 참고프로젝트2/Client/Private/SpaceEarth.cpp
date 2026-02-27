#include "stdafx.h"
#include "..\Public\SpaceEarth.h"

#include "RenderInstance.h"
#include "GameInstance.h"

CSpaceEarth::CSpaceEarth(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject { pDevice, pContext }
{

}

CSpaceEarth::CSpaceEarth(const CSpaceEarth & Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CSpaceEarth::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSpaceEarth::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	//m_pTransformCom->Set_Scaled(1.1f, 1.1f, 1.f);
	m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(-90.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(100.f, -100.f, -200.f, 1.f));

	CMap_Manager::Get_Instance()->Push_MapObject(CMap_Manager::MAP_SPACE,
		static_cast<CMap_Manager::Map_Object_Key*>(pArg)->m_PrototypeKey, this);

	return S_OK;
}

void CSpaceEarth::Camera_Update(_float fTimeDelta)
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

void CSpaceEarth::Update(_float fTimeDelta)
{
}

void CSpaceEarth::Late_Update(_float fTimeDelta)
{
	//_vector vPos = m_pGameInstance->Get_CamPosition_Vector();
	//vPos = XMVectorSetZ(vPos, XMVectorGetZ(vPos) + 100);

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_PRIORITY, this);
}

HRESULT CSpaceEarth::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_fTimeEarth = fTimeDelta;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_TimeEarth", &m_fTimeEarth, sizeof(float))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pShaderCom->Begin(2)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CSpaceEarth::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxSpace"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Space_Earth_Diffuse"),
		TEXT("Com_Texture_Diffuse"), reinterpret_cast<CComponent**>(&m_pTextureCom_Diffuse))))
		return E_FAIL;
	
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Space_Earth_Light"),
		TEXT("Com_Texture_Light"), reinterpret_cast<CComponent**>(&m_pTextureCom_Light))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Space_Earth_Shadow"),
		TEXT("Com_Texture_Shadow"), reinterpret_cast<CComponent**>(&m_pTextureCom_Shadow))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Space_Earth_Cloud"),
		TEXT("Com_Texture_Cloud"), reinterpret_cast<CComponent**>(&m_pTextureCom_Cloud))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SpaceEarth"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CSpaceEarth::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	if (FAILED(m_pTextureCom_Diffuse->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pTextureCom_Light->Bind_ShaderResource(m_pShaderCom, "g_EarthLight", 0)))
		return E_FAIL;
	if (FAILED(m_pTextureCom_Shadow->Bind_ShaderResource(m_pShaderCom, "g_EarthShadow", 0)))
		return E_FAIL;

	if (FAILED(m_pTextureCom_Cloud->Bind_ShaderResource(m_pShaderCom, "g_EarthCloud0", 0)))
		return E_FAIL;
	if (FAILED(m_pTextureCom_Cloud->Bind_ShaderResource(m_pShaderCom, "g_EarthCloud1", 1)))
		return E_FAIL;
	if (FAILED(m_pTextureCom_Cloud->Bind_ShaderResource(m_pShaderCom, "g_EarthCloud2", 2)))
		return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_MaskStar_Value_1", &m_fMaskStar_Value_1, sizeof(float))))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_MaskStar_Value_2", &m_fMaskStar_Value_2, sizeof(float))))
	//	return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_TimeEarth", &m_fAccTime, sizeof(float))))
		return E_FAIL;
	
	return S_OK;
}

CSpaceEarth * CSpaceEarth::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSpaceEarth*		pInstance = new CSpaceEarth(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSpaceEarth"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSpaceEarth::Clone(void * pArg)
{
	CSpaceEarth*		pInstance = new CSpaceEarth(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSpaceEarth"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSpaceEarth::Free()
{

	Safe_Release(m_pTextureCom_Diffuse);
	Safe_Release(m_pTextureCom_Light);
	Safe_Release(m_pTextureCom_Shadow);
	Safe_Release(m_pTextureCom_Cloud);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	__super::Free();
}
