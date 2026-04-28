#include "Pokemon.h"
#include "GameInstance.h"

CPokemon::CPokemon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
	m_strName = { L"Pokemon_Default" };
}

CPokemon::CPokemon(const CPokemon& Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CPokemon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPokemon::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//m_pModelCom->Set_AnimationIndex(rand() % 20, true);
	m_pModelCom->Set_AnimationIndex(m_iDummy, true);

	m_pTransformCom->Set_State(STATE::POSITION,
		XMVectorSet(
			m_pGameInstance->Random(10.f, 30.f),
			0.f,
			m_pGameInstance->Random(-20.f, 0.f),
			1.f
		));

	return S_OK;
}

void CPokemon::Priority_Update(_float fTimeDelta)
{

}

void CPokemon::Update(_float fTimeDelta)
{
	if (true == m_pModelCom->Play_Animation(fTimeDelta))
		int a = 10; // 중단점용 임시 코드
}

void CPokemon::Late_Update(_float fTimeDelta)
{

	m_pGameInstance->Add_RenderGroup(RENDERID::NONBLEND, this);
}

HRESULT CPokemon::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	size_t iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_TexDiff", i, MATERIAL_TYPE::DIFFUSE, 0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CPokemon::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_SHADER_VTXANIMMESH,
		COM_SHADER, reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	WNameID strProtoModelTag{};
	_uint iRand = rand() % 4;
	if (iRand == 0)			strProtoModelTag = PROTO_COM_MODEL_PM0001_00;
	else if (iRand == 1)	strProtoModelTag = PROTO_COM_MODEL_PM0004_00;
	else if (iRand == 2)	strProtoModelTag = PROTO_COM_MODEL_PM0007_00;
	else if (iRand == 3)	strProtoModelTag = PROTO_COM_MODEL_PM0025_00;

	//strProtoModelTag = PROTO_COM_MODEL_PM0001_00;

	if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), strProtoModelTag,
		COM_MODEL, reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CPokemon::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pTransformCom->Bind_ShaderResourceWIT(m_pShaderCom, "g_WITMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(D3DTS::PROJ))))
		return E_FAIL;

	return S_OK;
}


CPokemon* CPokemon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPokemon* pInstance = new CPokemon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPokemon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPokemon::Clone(void* pArg)
{
	CPokemon* pInstance = new CPokemon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPokemon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPokemon::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
