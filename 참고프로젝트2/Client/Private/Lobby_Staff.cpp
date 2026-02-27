#include "stdafx.h"
#include "..\Public\Lobby_Staff.h"

#include "RenderInstance.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Level_Lobby.h"

#include "UI_Staff_Text.h"

CLobby_Staff::CLobby_Staff(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CLobby_Staff::CLobby_Staff(const CLobby_Staff& Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CLobby_Staff::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLobby_Staff::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC Desc{};
	Desc.fRotationPerSec = 1.f;
	Desc.fSpeedPerSec = 10.f;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	STAFF_DESC* pDesc = static_cast<STAFF_DESC*>(pArg);
	Default_Setting(pDesc->iNumObject);

	//아이들
	m_pModelCom->SetUp_Animation(0, true, 0.1f);

	return S_OK;
}

void CLobby_Staff::Camera_Update(_float fTimeDelta)
{

}

void CLobby_Staff::Update(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);

}

void CLobby_Staff::Late_Update(_float fTimeDelta)
{
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CLobby_Staff::Render(_float fTimeDelta)
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


		if (FAILED(m_pShaderCom->Begin(3)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

void CLobby_Staff::State()
{
	switch (m_eCurrState)
	{
	case IDLE:
		
		break;

	case RUN:
		break;
	}
}

void CLobby_Staff::AnimState()
{
	if (m_bAnimChange)
	{
		m_ePrevState = m_eCurrState;
		switch (m_eCurrState)
		{
		case IDLE:

			break;

		case RUN:
			break;
		}

		m_bAnimChange = FALSE;
	}
}

_bool CLobby_Staff::Idle(_float fTimeDelta)
{
	return _bool();
}

_bool CLobby_Staff::Run(_vector vTargPos, _float fTimeDelta)
{
	return _bool();
}

void CLobby_Staff::Default_Setting(_uint iNumObj)
{
	switch (iNumObj)
	{
	case 0:
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, { -12.675f , 0.000f,34.633f , 1.f });
		m_pTransformCom->Rotation({0.f,1.f, 0.f} , XMConvertToRadians(155.f));
		break;

	case 1:
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, { 18.658f, 0.000f,32.596f , 1.f });
		m_pTransformCom->Rotation({ 0.f,1.f, 0.f }, XMConvertToRadians(213.f));
		break;
	}
	//CUI_Staff_Text::STAFF_TEXT_DESC TextDesc = {};
	//
	//TextDesc.pStaffTransform = m_pTransformCom;
	//m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOBBY, TEXT("Prototype_GameObject_Lobby_Lobby_Staff"), TEXT("Layer_Lobby_Staff"), &TextDesc);
}

HRESULT CLobby_Staff::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_NPC_Staff"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLobby_Staff::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CLobby_Staff* CLobby_Staff::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLobby_Staff* pInstance = new CLobby_Staff(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLobby_Staff"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLobby_Staff::Clone(void* pArg)
{
	CLobby_Staff* pInstance = new CLobby_Staff(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CLobby_Staff"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLobby_Staff::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	__super::Free();
}
