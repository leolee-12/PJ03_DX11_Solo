#include "stdafx.h"
#include "..\Public\Lobby_DisplayBoard.h"

#include "RenderInstance.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Level_Lobby.h"

CLobby_DisplayBoard::CLobby_DisplayBoard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CLobby_DisplayBoard::CLobby_DisplayBoard(const CLobby_DisplayBoard& Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CLobby_DisplayBoard::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLobby_DisplayBoard::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC Desc{};
	Desc.fRotationPerSec = 1.f;
	Desc.fSpeedPerSec = 10.f;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	DISPLAY_DESC* pDesc = static_cast<DISPLAY_DESC*>(pArg);
	m_eFontID = pDesc->eFontID;
	m_eInOutID = pDesc->eInOutID;

	Set_DefaultSetting();

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fAlphaValue = 0.359999925f;
	m_pTransformCom->Set_Scaled(1.15f, 1.15f, 1.15f);

	return S_OK;
}

void CLobby_DisplayBoard::Camera_Update(_float fTimeDelta)
{

}

void CLobby_DisplayBoard::Update(_float fTimeDelta)
{
	if (m_eInOutID == LOBBY_IN)
	{
		m_fTexcorrdValue += fTimeDelta * 0.1f;
		if (m_fTexcorrdValue >= 1.f)
			m_fTexcorrdValue = 0.f;
	}
	else if (m_eInOutID == LOBBY_OUT)
	{
		m_fTexcorrdValue -= fTimeDelta * 0.1f;
		if (m_fTexcorrdValue <= 0.f)
			m_fTexcorrdValue = 1.f;
	}

}

void CLobby_DisplayBoard::Late_Update(_float fTimeDelta)
{
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CLobby_DisplayBoard::Render(_float fTimeDelta)
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


		if (FAILED(m_pShaderCom->Begin(2)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLobby_DisplayBoard::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_LOBBY, strMeshTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	//Prototype_Component_Model_DIsplayBoardOut
	/* Com_Texutre */
	if (FAILED(__super::Add_Component(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Lobby_DisplayBoard"),
		TEXT("Com_Texutre"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_FontTexutre */
	if (FAILED(__super::Add_Component(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Lobby_DisplayFont"),
		TEXT("Com_FontTexutre "), reinterpret_cast<CComponent**>(&m_pFontTextureCom))))
		return E_FAIL;


	return S_OK;
}

HRESULT CLobby_DisplayBoard::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pFontTextureCom->Bind_ShaderResource(m_pShaderCom, "g_FontTexture", m_eFontID)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fTexcorrdValue, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaValue", &m_fAlphaValue, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vDiffColor", &vDiffColor, sizeof(_vector))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vOutLineColor", &vOutLineColor, sizeof(_vector))))
		return E_FAIL;

	return S_OK;
}

void CLobby_DisplayBoard::Set_DefaultSetting()
{
	switch (m_eFontID)
	{
	case PRACTICE:
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, { -60.6f, 0.4f , -0.2f , 1.f });
		vDiffColor = { 0.478f, 0.572f, 0.964, 1.f };
		vOutLineColor = { 0.f, 0.f, 0.f, 1.f };
		break;

	case STORY:
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, { 60.f, 0.4f , 0.2f , 1.f });
		vDiffColor = { 1.f, 0.572f, 0.464, 1.f };
		vOutLineColor = { 0.f, 0.f, 0.f, 1.f };
		break;
	}

	switch (m_eInOutID)
	{
	case LOBBY_IN:
		strMeshTag = TEXT("Prototype_Component_Model_DIsplayBoard_In");
		m_fTexcorrdValue = 0.f;
		break;

	case LOBBY_OUT:
		strMeshTag = TEXT("Prototype_Component_Model_DIsplayBoard_Out");
		m_fTexcorrdValue = 1.f;
		break;
	}
}

CLobby_DisplayBoard* CLobby_DisplayBoard::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLobby_DisplayBoard* pInstance = new CLobby_DisplayBoard(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLobby_DisplayBoard"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLobby_DisplayBoard::Clone(void* pArg)
{
	CLobby_DisplayBoard* pInstance = new CLobby_DisplayBoard(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CLobby_DisplayBoard"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLobby_DisplayBoard::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pFontTextureCom);

	__super::Free();
}
