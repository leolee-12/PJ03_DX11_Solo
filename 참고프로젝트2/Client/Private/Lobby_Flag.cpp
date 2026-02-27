#include "stdafx.h"
#include "..\Public\Lobby_Flag.h"

#include "RenderInstance.h"
#include "GameInstance.h"

CLobby_Flag::CLobby_Flag(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CLobby_Flag::CLobby_Flag(const CLobby_Flag& Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CLobby_Flag::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLobby_Flag::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	FLAG_DESC* pDesc = static_cast<FLAG_DESC*>(pArg);
	Default_Setting(pDesc->iNumObject);
	m_pTransformCom->Set_Scaled(2.5f, 2.5f, 2.5f);
	return S_OK;
}

void CLobby_Flag::Camera_Update(_float fTimeDelta)
{

}

void CLobby_Flag::Update(_float fTimeDelta)
{
	m_fAnimFrame += fTimeDelta * 5.f;
	m_fTexcoordValue += fTimeDelta * 0.5f;

	//if (m_fTexcoordValue >= 1.f)
		//m_fTexcoordValue = 0.f;

	if (m_fAnimFrame >= 1.f)
	{
		m_iSpriteIndex++;
		m_fAnimFrame = 0.f;
	}

	if (m_iSpriteIndex >= 8)
		m_iSpriteIndex = 0;


}

void CLobby_Flag::Late_Update(_float fTimeDelta)
{
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CLobby_Flag::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pDiffTexture[i]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
			return E_FAIL;

		(i == 0) ? m_iShaderIndex = 3 : m_iShaderIndex = 0;

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

void CLobby_Flag::Default_Setting(_uint iNum)
{
	switch (iNum)
	{
	case 0:
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, { 7.801f, 0.046f, -31.624f , 1.f });
		m_pTransformCom->Rotation({ 0.f, 1.f, 0.f }, XMConvertToRadians(144.000));
		break;

	case 1:
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, { 16.301f, 0.046f, -27.624f , 1.f });
		m_pTransformCom->Rotation({ 0.f, 1.f, 0.f }, XMConvertToRadians(156.000));
		break;

	case 2:
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, { -17.299f, 0.046f, -27.424f , 1.f });
		m_pTransformCom->Rotation({ 0.f, 1.f, 0.f }, XMConvertToRadians(180.000));
		break;
	case 3:
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, { -8.299f, 0.046f, -31.224f , 1.f });
		m_pTransformCom->Rotation({ 0.f, 1.f, 0.f }, XMConvertToRadians(180.000));
		break;

	}
}

HRESULT CLobby_Flag::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_FlagB"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_DiffTexture1 */
	if (FAILED(__super::Add_Component(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Lobby_lob_flagY_anim"),
		TEXT("Com_DiffTexture0"), reinterpret_cast<CComponent**>(&m_pDiffTexture[0]))))
		return E_FAIL;

	/* Com_DiffTexture0 */
	if (FAILED(__super::Add_Component(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Lobby_lob_flag_rod"),
		TEXT("Com_DiffTexture1"), reinterpret_cast<CComponent**>(&m_pDiffTexture[1]))))
		return E_FAIL;


	return S_OK;
}

HRESULT CLobby_Flag::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_uint iNumSprite = 8;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iNumSprite", &iNumSprite, sizeof(_uint))))
		return E_FAIL;

	_bool bSwitch = TRUE;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_bXYSwitch", &bSwitch, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_iSpriteIndex", &m_iSpriteIndex, sizeof(_uint))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fTexcoordValue, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CLobby_Flag* CLobby_Flag::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLobby_Flag* pInstance = new CLobby_Flag(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLobby_Flag"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLobby_Flag::Clone(void* pArg)
{
	CLobby_Flag* pInstance = new CLobby_Flag(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CLobby_Flag"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLobby_Flag::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	for(int i = 0 ; i <  2; i++)
		Safe_Release(m_pDiffTexture[i]);

	__super::Free();
}
