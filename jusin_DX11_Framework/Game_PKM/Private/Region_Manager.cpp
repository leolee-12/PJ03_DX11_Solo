#include "Region_Manager.h"
#include "GameInstance.h"

CRegion_Manager::CRegion_Manager()
{
}

HRESULT CRegion_Manager::Initialize()
{
	Clear();   // 재진입 안전성

	m_pGameInstance = CGameInstance::GetInstance();   // weak
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	return S_OK;
}

HRESULT CRegion_Manager::Register_Region(const REGION_RECT_DESC& tDesc)
{
	if (INVALID_REGION_ID == tDesc.iRegionID)
		return E_FAIL;

	for (const auto& tRegion : m_Regions)
	{
		if (tRegion.iRegionID == tDesc.iRegionID)
			return E_FAIL;
	}

	m_Regions.push_back(tDesc);
	return S_OK;
}

void CRegion_Manager::Clear()
{
	m_Regions.clear();
	m_iCurrentRegionID = INVALID_REGION_ID;
}

void CRegion_Manager::Update()
{
	_float3 vPos{};
	if (false == Get_PlayerPosition(vPos))
		return;

	const _uint iResolved = Resolve_Region(vPos);

	if (INVALID_REGION_ID == iResolved)   // 어느 지역 사각형에도 없음 → 직전 지역 유지
		return;
	if (iResolved == m_iCurrentRegionID)
		return;

	// 첫 진입(직전 INVALID)은 BGM만 깔고 배너는 띄우지 않는다.
	const _bool bInitial = (INVALID_REGION_ID == m_iCurrentRegionID);
	Enter_Region(iResolved, false == bInitial);
}

void CRegion_Manager::Resolve_Initial()
{
	_float3 vPos{};
	if (false == Get_PlayerPosition(vPos))
		return;

	Enter_Region(Resolve_Region(vPos), false);
}

void CRegion_Manager::Play_Current_BGM() const
{
	const REGION_RECT_DESC* pRegion = Find_Region(m_iCurrentRegionID);
	if (nullptr == pRegion || nullptr == m_pGameInstance)
		return;

	if (0 != pRegion->szBGM[0])
		m_pGameInstance->Play_BGM(pRegion->szBGM, pRegion->fBGMVolume);
}

const REGION_RECT_DESC* CRegion_Manager::Find_Region(_uint iRegionID) const
{
	if (INVALID_REGION_ID == iRegionID)
		return nullptr;

	for (const auto& tDesc : m_Regions)
		if (tDesc.iRegionID == iRegionID)
			return &tDesc;

	return nullptr;
}

_bool CRegion_Manager::Get_PlayerPosition(_float3& vOut) const
{
	if (nullptr == m_pGameInstance)
		return false;

	const list<CGameObject*>* pPlayerList =
		m_pGameInstance->Get_ObjectList(ETOUI(LEVEL::GAMEPLAY), LAYER_PLAYER);
	if (nullptr == pPlayerList || pPlayerList->empty())
		return false;

	CGameObject* pPlayer = pPlayerList->front();
	if (nullptr == pPlayer)
		return false;

	XMStoreFloat3(&vOut, pPlayer->Get_Transform()->Get_State(STATE::POSITION));
	return true;
}

_uint CRegion_Manager::Resolve_Region(const _float3& vPlayerPos) const
{
	_uint iBestID = INVALID_REGION_ID;
	_uint iBestPriority = 0;
	_bool bFound = false;

	for (const auto& tDesc : m_Regions)
	{
		if (false == Is_PointInsideRegionXZ(vPlayerPos, tDesc))
			continue;

		if (false == bFound || tDesc.iPriority > iBestPriority)
		{
			iBestID = tDesc.iRegionID;
			iBestPriority = tDesc.iPriority;
			bFound = true;
		}
	}

	return iBestID;
}

void CRegion_Manager::Enter_Region(_uint iNewRegionID, _bool bNotify)
{
	const REGION_RECT_DESC* pPrevRegion = Find_Region(m_iCurrentRegionID);
	const REGION_RECT_DESC* pNewRegion = Find_Region(iNewRegionID);
	if (nullptr == pNewRegion)
		return;

	m_iCurrentRegionID = iNewRegionID;

	if (nullptr != m_pGameInstance && 0 != pNewRegion->szBGM[0])
		m_pGameInstance->Play_BGM(pNewRegion->szBGM, pNewRegion->fBGMVolume);

	if (true == bNotify && nullptr != m_fnOnRegionChanged)
		m_fnOnRegionChanged(pPrevRegion, *pNewRegion);
}

CRegion_Manager* CRegion_Manager::Create()
{
	CRegion_Manager* pInstance = new CRegion_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CRegion_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRegion_Manager::Free()
{
	__super::Free();

	m_fnOnRegionChanged = nullptr;
	m_Regions.clear();
	m_pGameInstance = nullptr;   // weak - Release 하지 않음
}