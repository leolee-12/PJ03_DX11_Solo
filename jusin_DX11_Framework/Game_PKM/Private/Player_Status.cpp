#include "Player_Status.h"
#include "PokemonData_Manager.h"

CPlayer_Status::CPlayer_Status(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
	m_strName = L"PlayerState";
}

CPlayer_Status::CPlayer_Status(const CPlayer_Status& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CPlayer_Status::Initialize_Prototype()
{
	PartyOps::Clear(m_tParty);
	BoxOps::Clear(m_tBox);
	PokedexOps::Clear(m_tPokedex);
	return S_OK;
}

HRESULT CPlayer_Status::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_strTrainerBodyProtoTag = PROTO_OBJ_BODY_HERO;
	m_strTrainerModelProtoTag = PROTO_COM_MODEL_HERO;
	m_strTrainerShaderProtoTag = PROTO_COM_SHADER_PLAYER_LGPE;

	if (0 == m_tParty.iCount)
		Acquire_Pokemon(25, 5, 0);

	return S_OK;
}

POKEDEX_STATE CPlayer_Status::Get_DexState(_uint iDexNo) const
{
	return PokedexOps::Get(m_tPokedex, iDexNo);
}

_bool CPlayer_Status::Mark_DexSeen(_uint iDexNo)
{
	return PokedexOps::Mark_Seen(m_tPokedex, iDexNo);
}

_bool CPlayer_Status::Mark_DexCaught(_uint iDexNo)
{
	return PokedexOps::Mark_Caught(m_tPokedex, iDexNo);
}

_bool CPlayer_Status::Acquire_Pokemon(_uint iSpeciesID, _ubyte iLevel, _uint iCapturedAtZoneID)
{
	auto* pDataMgr = CPokemonData_Manager::GetInstance();
	if (nullptr == pDataMgr)
		return false;

	const SPECIES_DATA* pSpecies = pDataMgr->Find_Species(iSpeciesID);
	if (nullptr == pSpecies)
		return false;

	POKEMON_INSTANCE tInstance = Build_PokemonInstance(
		*pSpecies,
		iLevel,
		m_iTrainerID,
		iCapturedAtZoneID,
		pDataMgr);

	_bool bAdded = false;

	if (PartyOps::Has_Empty_Slot(m_tParty))
		bAdded = PartyOps::Add(m_tParty, tInstance);
	else
		bAdded = BoxOps::Add(m_tBox, tInstance);

	if (false == bAdded)
		return false;

	Mark_DexCaught(iSpeciesID);
	return true;
}

CPlayer_Status* CPlayer_Status::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer_Status* pInstance = new CPlayer_Status(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer_Status");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer_Status::Clone(void* pArg)
{
	CPlayer_Status* pInstance = new CPlayer_Status(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer_Status");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Status::Free()
{
	__super::Free();
}