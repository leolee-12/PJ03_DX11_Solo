#include "stdafx.h"
#include "..\Public\Model_Preview.h"

#include "RenderInstance.h"
#include "GameInstance.h"

#include "FrameEvent_Manager.h"

CModel_Preview::CModel_Preview(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CModel_Preview::CModel_Preview(const CModel_Preview& Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CModel_Preview::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CModel_Preview::Initialize(void* pArg)
{

	//MODEL_PREVIEW_DESC* pDesc = static_cast<MODEL_PREVIEW_DESC*>(pArg);
	//m_strModelName = pDesc->strModelName;


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	m_pTransformCom->Set_Scaled(-1.f, 1.f, 1.f);
	//m_pModelCom->SetUp_Animation(16, true);
	m_pModelCom->SetUp_Animation(0, false);
	m_pModelCom->Play_Animation(0.f);

	CFrameEvent_Manager::Get_Instance()->Initalize_NameMap();
	//CFrameEvent_Manager::Get_Instance()->LoadFile2("../Bin/FrameEventData/Split.txt");
	//CFrameEvent_Manager::Get_Instance()->LoadFile2("../Bin/FrameEventData/EventData_21.txt");

	//FlipDirection(-1);

	//FlipDirection(-1);

	return S_OK;
}

void CModel_Preview::Camera_Update(_float fTimeDelta)
{

}

void CModel_Preview::Update(_float fTimeDelta)
{
	//Imgui AnimationTab에서만 처리할것
	//m_pModelCom->Play_Animation(fTimeDelta);
}

void CModel_Preview::Late_Update(_float fTimeDelta)
{
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CModel_Preview::Render(_float fTimeDelta)
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

		/* 모델이 가지고 있는 뼈들 중에서 현재 렌더링할려고 했던 i번째ㅑ 메시가 사용하는 뼈들을 배열로 만들어서 쉐이더로 던져준다.  */
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

void CModel_Preview::FlipDirection(_int iDirection)
{
	if (iDirection == 0)
	{
		m_iLookDirection = -m_iLookDirection;
	}
	else
	{
		m_iLookDirection = iDirection;
	}

	//if (m_iLookDirection == -1)
	//{
	//	m_pTransformCom->Set_Scaled(-1, 1, 1);
	//}
	//else
	//{
	//	m_pTransformCom->Set_Scaled(1, 1, 1);
	//}

	//m_pTransformCom->Set_Scaled(-1, 1, 1);

}

HRESULT CModel_Preview::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_untitled"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Model */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_strModelName,
	//	TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CModel_Preview::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CModel_Preview* CModel_Preview::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CModel_Preview* pInstance = new CModel_Preview(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CModel_Preview"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CModel_Preview::Clone(void* pArg)
{
	CModel_Preview* pInstance = new CModel_Preview(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CModel_Preview"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel_Preview::Free()
{
	__super::Free();


	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
