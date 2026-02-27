#include "stdafx.h"
#include "..\Public\Lobby_Frieza.h"

#include "RenderInstance.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Level_Lobby.h"

#include "UI_Lobby_Text.h"

CLobby_Frieza::CLobby_Frieza(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CLobby_Frieza::CLobby_Frieza(const CLobby_Frieza& Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CLobby_Frieza::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLobby_Frieza::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC Desc{};
	Desc.fRotationPerSec = 1.f;
	Desc.fSpeedPerSec = 10.f;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//if (FAILED(Ready_Text()))
	//	return E_FAIL;

	m_pTransformCom->Set_State_Position(_float3(-51.700f, 0.f, 3.900f));
	m_pTransformCom->Rotation({ 0.f , 1.f, 0.f }, XMConvertToRadians(180.f));
	
	//아이들
	m_pModelCom->SetUp_Animation(0, true, 0.1f);

	return S_OK;
}

void CLobby_Frieza::Camera_Update(_float fTimeDelta)
{

}

void CLobby_Frieza::Update(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);

}

void CLobby_Frieza::Late_Update(_float fTimeDelta)
{
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CLobby_Frieza::Render(_float fTimeDelta)
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

HRESULT CLobby_Frieza::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_Frieza"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLobby_Frieza::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	//if (false == m_isShadow)
	//{
	//
	//	m_isShadow = true;
	//	m_iPassIndex = 3;
	//}
	//else
	//{
	//	_float4x4			LightViewMatrix, LightProjMatrix;
	//
	//	XMStoreFloat4x4(&LightViewMatrix, XMMatrixLookAtLH(XMVectorSet(0.f, 10.f, 0.f, 1.f), XMVectorSet(1.f, -1.f, 1.f, 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	//	XMStoreFloat4x4(&LightProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(120.0f), (_float)g_iWinSizeX / g_iWinSizeY, 0.1f, 1000.f));
	//
	//	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &LightViewMatrix)))
	//		return E_FAIL;
	//
	//	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &LightProjMatrix)))
	//		return E_FAIL;
	//
	//	m_isShadow = false;
	//	m_iPassIndex = 2;
	//}

	return S_OK;
}

HRESULT CLobby_Frieza::Ready_Text()
{
	CUI_Lobby_Text::UI_TEXT Desc = {};
	queue<CUI_Lobby_Text::UI_TEXT> Text;

	Text.push({ TEXT("프리저! 네가 여기 나타난 순간부터 싸울 각오 했어! 이제 물러설 곳은 없어!"), CUI_Define::NPC_KRILLIN });
	Text.push({ TEXT("하하하! 네가 감히 나에게 덤비다니, 웃음을 주는군. 하지만 오래가진 못할 거야."), CUI_Define::NPC_FRIEZA });
	Text.push({ TEXT("내 힘을 무시하지 마라! 모두를 지키기 위해 난 싸운다!"), CUI_Define::NPC_KRILLIN });
	Text.push({ TEXT("이것이 네 최선인가? 내 손끝 하나로 너를 끝내주마."), CUI_Define::NPC_FRIEZA });
	Text.push({ TEXT("프리저, 네 상대는 나야! 크리링, 쉬고 있어. 이제 내가 나선다."), CUI_Define::NPC_GOKU });
	Text.push({ TEXT("오공... 또 네가 끼어드는군. 이번엔 널 완전히 없애주겠다!"), CUI_Define::NPC_FRIEZA });

	//Text.push({ TEXT("저기에서 무슨 이상한 소리가 들리지 않나,,?"), CUI_Define::ID_END});


	dynamic_cast<CUI_Lobby_Text*>(m_pGameInstance->Get_GameObject(LEVEL_LOBBY, TEXT("Layer_Lobby_TextBox")))->Add_Text(Text);

	return S_OK;
}

CLobby_Frieza* CLobby_Frieza::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLobby_Frieza* pInstance = new CLobby_Frieza(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLobby_Frieza"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLobby_Frieza::Clone(void* pArg)
{
	CLobby_Frieza* pInstance = new CLobby_Frieza(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CLobby_Frieza"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLobby_Frieza::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	__super::Free();
}
