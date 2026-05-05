#include "Battle_Pokemon.h"
#include "PokemonData_Manager.h"

#include "GameInstance.h"

CBattle_Pokemon::CBattle_Pokemon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
	m_strName = { L"Pokemon_Default" };
}

CBattle_Pokemon::CBattle_Pokemon(const CBattle_Pokemon& Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CBattle_Pokemon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBattle_Pokemon::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	const POKEMON_DESC* pDesc = static_cast<const POKEMON_DESC*>(pArg);

	if (nullptr == pDesc->pInstance)
		return E_FAIL;

	m_pInstance = pDesc->pInstance;
	m_iSide = pDesc->iSide;

	if (m_iSide >= g_kBattleSideCount)
		return E_FAIL;

	const CPokemonData_Manager* pDataMgr = CPokemonData_Manager::GetInstance();
	if (nullptr == pDataMgr)
		return E_FAIL;

	const SPECIES_DATA* pSpecies = pDataMgr->Find_Species(m_pInstance->iSpeciesID);
	if (nullptr == pSpecies || 0 == pSpecies->strModelTag)
		return E_FAIL;

	m_strSpeciesModelTag = pSpecies->strModelTag;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pModelCom->Set_AnimationIndex(0, true);

	const _float3& vPos = BattleSlotPose::vPokemonPos[m_iSide];
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(vPos.x, vPos.y, vPos.z, 1.f));
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), BattleSlotPose::fYawPokemon[m_iSide]);

	return S_OK;
}

void CBattle_Pokemon::Priority_Update(_float fTimeDelta)
{

}

void CBattle_Pokemon::Update(_float fTimeDelta)
{
	if (true == m_pModelCom->Play_Animation(fTimeDelta))
		int a = 10; // 중단점용 임시 코드
}

void CBattle_Pokemon::Late_Update(_float fTimeDelta)
{

	m_pGameInstance->Add_RenderGroup(RENDERID::NONBLEND, this);
}

HRESULT CBattle_Pokemon::Render()
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

HRESULT CBattle_Pokemon::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), PROTO_COM_SHADER_VTXANIMMESH,
		COM_SHADER, reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), m_strSpeciesModelTag,
		COM_MODEL, reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CBattle_Pokemon::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pTransformCom->Bind_ShaderResourceWIT(m_pShaderCom, "g_WITMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(D3DTS::PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFarZ", m_pGameInstance->Get_FarZPtr(), sizeof(_float))))
		return E_FAIL;

	return S_OK;
}


CBattle_Pokemon* CBattle_Pokemon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBattle_Pokemon* pInstance = new CBattle_Pokemon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBattle_Pokemon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBattle_Pokemon::Clone(void* pArg)
{
	CBattle_Pokemon* pInstance = new CBattle_Pokemon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBattle_Pokemon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBattle_Pokemon::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
