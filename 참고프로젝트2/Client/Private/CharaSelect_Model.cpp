#include "stdafx.h"
#include "..\Public\CharaSelect_Model.h"

#include "RenderInstance.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Level_Lobby.h"
#include "UI_Manager.h"

CCharaSelect_Model::CCharaSelect_Model(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CCharaSelect_Model::CCharaSelect_Model(const CCharaSelect_Model& Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CCharaSelect_Model::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCharaSelect_Model::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC Desc{};
	Desc.fRotationPerSec = 1.f;
	Desc.fSpeedPerSec = 10.f;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	SELECT_MODEL* pDesc = static_cast<SELECT_MODEL*>(pArg);
	m_ePlayerID = pDesc->ePlayerID;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	Default_Position(pDesc->iNumModel);

	m_pModelCom[m_ePlayerID]->SetUp_Animation(1, false, 0.1f);

	if(m_ePlayerID == CUI_Define::HIT)
		m_pModelCom[m_ePlayerID]->SetUp_Animation(0, false, 0.1f);

	Default_PlayerSetting();

	return S_OK;
}

void CCharaSelect_Model::Camera_Update(_float fTimeDelta)
{

}

void CCharaSelect_Model::Update(_float fTimeDelta)
{
	m_pModelCom[m_ePlayerID]->Play_Animation(fTimeDelta);
}

void CCharaSelect_Model::Late_Update(_float fTimeDelta)
{
	m_fRenderTimer += fTimeDelta;
	
	if (m_ePlayerID == CUI_Define::HIT)
	{
		if (m_fRenderTimer >= 0.15f)
			m_pRenderInstance->Add_RenderObject(CRenderer::RG_PLAYER, this, &m_RendererDesc);
	}
	else
		m_pRenderInstance->Add_RenderObject(CRenderer::RG_PLAYER, this, &m_RendererDesc);
}

HRESULT CCharaSelect_Model::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom[m_ePlayerID]->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (m_ePlayerID == CUI_Define::FRIEZA)
		{
			if(i == 1 || i == 2 || i == 5)
				continue;
		}
		else if(m_ePlayerID == CUI_Define::GOKU)
		{
			if(i >= 1)
				continue;
		}
		else
		{
			if (i > 1)
				continue;
		}



		/* 모델이 가지고 있는 머테리얼 중 i번째 메시가 사용해야하는 머테리얼구조체의 aiTextureType_DIFFUSE번째 텍스쳐를 */
		/* m_pShaderCom에 있는 g_DiffuseTexture변수에 던져. */
		if (FAILED(m_pModelCom[m_ePlayerID]->Bind_MaterialSRV(m_pShaderCom, aiTextureType_DIFFUSE, "g_DiffuseTexture", i)))
			return E_FAIL;

		//m_pModelCom->Bind_MaterialSRV(m_pShaderCom, aiTextureType_NORMALS, "g_NormalTexture", i);

		/* 모델이 가지고 있는 뼈들 중에서 현재 렌더링할려고 했던 i번째ㅑ 메시가 사용하는 뼈들을 배열로 만들어서 쉐이더로 던져준다.  */
		if (FAILED(m_pModelCom[m_ePlayerID]->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;


		if (FAILED(m_pShaderCom->Begin(m_iShaderIndex)))
			return E_FAIL;

		if (FAILED(m_pModelCom[m_ePlayerID]->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CCharaSelect_Model::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	//손오공
	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_CHARACTER, TEXT("Prototype_Component_Model_CharaSelectMddel_Goku"),
		TEXT("Com_Model_Goku"), reinterpret_cast<CComponent**>(&m_pModelCom[0]))))
		return E_FAIL;

	/* Com_DiffTexture */
	if (FAILED(__super::Add_Component(LEVEL_CHARACTER, TEXT("Prototype_Component_Texture_Character_Goku_Base"),
		TEXT("Com_DiffTexture_Goku"), reinterpret_cast<CComponent**>(&m_pDiffTexture[0]))))
		return E_FAIL;

	/* Com_LimTexture */
	if (FAILED(__super::Add_Component(LEVEL_CHARACTER, TEXT("Prototype_Component_Texture_Character_OutLine_Goku"),
		TEXT("Com_LimTexture_Goku"), reinterpret_cast<CComponent**>(&m_pLimTexture[0]))))
		return E_FAIL;

	//21호
	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_CHARACTER, TEXT("Prototype_Component_Model_CharaSelectMddel_21"),
		TEXT("Com_Model_21"), reinterpret_cast<CComponent**>(&m_pModelCom[1]))))
		return E_FAIL;

	/* Com_DiffTexture */
	if (FAILED(__super::Add_Component(LEVEL_CHARACTER, TEXT("Prototype_Component_Texture_Character_21_Base"),
		TEXT("Com_DiffTexture_21"), reinterpret_cast<CComponent**>(&m_pDiffTexture[1]))))
		return E_FAIL;

	/* Com_LimTexture */
	if (FAILED(__super::Add_Component(LEVEL_CHARACTER, TEXT("Prototype_Component_Texture_Character_OutLine_21"),
		TEXT("Com_LimTexture_21"), reinterpret_cast<CComponent**>(&m_pLimTexture[1]))))
		return E_FAIL;

	//프리저
	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_CHARACTER, TEXT("Prototype_Component_Model_CharaSelectMddel_Frieza"),
		TEXT("Com_Model_Frieza"), reinterpret_cast<CComponent**>(&m_pModelCom[2]))))
		return E_FAIL;

	/* Com_DiffTexture */
	if (FAILED(__super::Add_Component(LEVEL_CHARACTER, TEXT("Prototype_Component_Texture_Character_Frieza_Base"),
		TEXT("Com_DiffTexture_Frieza"), reinterpret_cast<CComponent**>(&m_pDiffTexture[2]))))
		return E_FAIL;

	/* Com_LimTexture */
	if (FAILED(__super::Add_Component(LEVEL_CHARACTER, TEXT("Prototype_Component_Texture_Character_OutLine_Frieza"),
		TEXT("Com_LimTexture_Frieza"), reinterpret_cast<CComponent**>(&m_pLimTexture[2]))))
		return E_FAIL;

	//히트
	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_CHARACTER, TEXT("Prototype_Component_Model_CharaSelectMddel_Hit"),
		TEXT("Com_Model_Hit"), reinterpret_cast<CComponent**>(&m_pModelCom[3]))))
		return E_FAIL;

	/* Com_DiffTexture */
	if (FAILED(__super::Add_Component(LEVEL_CHARACTER, TEXT("Prototype_Component_Texture_Character_Hit_Base"),
		TEXT("Com_DiffTexture_Hit"), reinterpret_cast<CComponent**>(&m_pDiffTexture[3]))))
		return E_FAIL;

	/* Com_LimTexture */
	if (FAILED(__super::Add_Component(LEVEL_CHARACTER, TEXT("Prototype_Component_Texture_Character_OutLine_Hit"),
		TEXT("Com_LimTexture_Hit"), reinterpret_cast<CComponent**>(&m_pLimTexture[3]))))
		return E_FAIL;

	return S_OK;
}

HRESULT CCharaSelect_Model::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pDiffTexture[m_ePlayerID]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pLimTexture[m_ePlayerID]->Bind_ShaderResource(m_pShaderCom, "g_OutLineTexture", 0)))
		return E_FAIL;

	return S_OK;
}

void CCharaSelect_Model::Default_Position(_uint iNumModel)
{
	switch (iNumModel)
	{
	case 0:
		m_pTransformCom->Set_State_Position(_float3(-2.5f, 0.1f, 6.5f));
		break;

	case 1:
		m_pTransformCom->Set_State_Position(_float3(-4.f, 0.1f, 6.5f));
		break;

	case 2:
		m_pTransformCom->Set_State_Position(_float3(2.5f, 0.1f, 6.5f));
		m_pTransformCom->Rotation({0.f, 1.f, 0.f}, XMConvertToRadians(75.f));
		break;

	case 3:
		m_pTransformCom->Set_State_Position(_float3(4.f, 0.1f, 6.5f));
		m_pTransformCom->Rotation({ 0.f, 1.f, 0.f }, XMConvertToRadians(90.f));
		break;
	}
}

void CCharaSelect_Model::Default_PlayerSetting()
{
	switch (m_ePlayerID)
	{
	case CUI_Define::GOKU:
		m_iShaderIndex = 0;
		Add_Light(_float4(-0.15f, -0.7f, 0.5f, 0.f), _float4(0.9f, 0.9f, 1.0f, 1.0f), _float4(0.5f, 0.5f, 0.5f, 1.f), _float4(0.f, 0.f, 0.f, 1.f), "SELECT_Goku_Light");
		break;

	case CUI_Define::ANDROID21:
		m_iShaderIndex = 1;
		Add_Light(_float4(-0.06f, -0.07f, 0.1f, 0.f), _float4(1.f, 1.f, 1.0f, 1.0f), _float4(0.5f, 0.5f, 0.5f, 1.f), _float4(0.f, 0.f, 0.f, 1.f), "SELECT_21_Light");
		break;

	case CUI_Define::FRIEZA:
		m_iShaderIndex = 5;
		Add_Light(_float4(-0.06f, -0.07f, 0.1f, 0.f), _float4(1.f, 1.f, 1.0f, 1.0f), _float4(0.5f, 0.5f, 0.5f, 1.f), _float4(0.f, 0.f, 0.f, 1.f), "SELECT_Frieza_Light");
		break;

	case CUI_Define::HIT:
		Add_Light(_float4(-1.f, -0.2f, 1.f, 0.f), _float4(0.9f, 0.9f, 1.0f, 1.0f), _float4(0.5f, 0.5f, 0.5f, 1.f), _float4(0.f, 0.f, 0.f, 1.f), "SELECT_Hit_Light");
		m_iShaderIndex = 1;
		break;
	}
}

HRESULT CCharaSelect_Model::Add_Light(_float4 vDirection, _float4 vDiffuse, _float4 vAmbient, _float4 vSpecular, string strName)
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
	

	if (FAILED(m_pRenderInstance->Add_Player_Light(strName, LightDesc, {1.f,1.f,1.f,0.f} ,&m_bLight)))
		return E_FAIL;

	return S_OK;
}

CCharaSelect_Model* CCharaSelect_Model::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCharaSelect_Model* pInstance = new CCharaSelect_Model(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CCharaSelect_Model"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCharaSelect_Model::Clone(void* pArg)
{
	CCharaSelect_Model* pInstance = new CCharaSelect_Model(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CCharaSelect_Model"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCharaSelect_Model::Free()
{
	Safe_Release(m_pShaderCom);

	for (int i = 0; i < 4; i++)
	{
		Safe_Release(m_pModelCom[i]);
		Safe_Release(m_pDiffTexture[i]);
		Safe_Release(m_pLimTexture[i]);
	}
	__super::Free();
}
