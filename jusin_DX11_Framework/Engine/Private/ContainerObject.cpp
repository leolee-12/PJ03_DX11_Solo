#include "ContainerObject.h"
#include "GameInstance.h"

#include "PartObject.h"

CContainerObject::CContainerObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CContainerObject::CContainerObject(const CContainerObject& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CContainerObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CContainerObject::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CContainerObject::Priority_Update(_float fTimeDelta)
{
}

void CContainerObject::Update(_float fTimeDelta)
{
}

void CContainerObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CContainerObject::Render()
{
	return S_OK;
}

HRESULT CContainerObject::Add_PartObject(_uint iPrototypeLevelIndex, const WNameID strPrototypeTag, const WNameID strPartTag, void* pArg)
{
	auto pPartObject = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iPrototypeLevelIndex, strPrototypeTag, pArg));
	if (nullptr == pPartObject)
		return E_FAIL;

	m_PartObjects.emplace(strPartTag, pPartObject);

	return S_OK;
}

void XM_CALLCONV CContainerObject::Tick_RootMotionMovement(_fvector vMoveDir, _bool bHasInput,
	const _float3& vRawRootMotionDelta, CNavigation* pNavigation, _float fTimeDelta)
{
	if (!bHasInput)
	{
		m_MoveState.Pivoting = false;
		return;
	}

	_vector vCurLook = XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK));
	_float fDot = XMVectorGetX(XMVector3Dot(vCurLook, vMoveDir));

	if (!m_MoveState.Pivoting)
	{
		if (fDot < m_Tuning.fPivotEnterDot)     // 큰 각도로 회전 시작 → 피벗
			m_MoveState.Pivoting = true;
	}
	else
	{
		if (fDot > m_Tuning.fPivotExitDot)      // 거의 정렬 완료 → 해제
			m_MoveState.Pivoting = false;
	}

	m_pTransformCom->Face_Direction(vCurLook, vMoveDir, fTimeDelta);

	_vector vRaw = m_Tuning.fRootMotionScale * XMLoadFloat3(&vRawRootMotionDelta);
	_vector vSmoothed = XMLoadFloat3(&m_MoveState.vSmoothedLocalDelta);
	_float  fAlpha = 1.f - expf(-m_Tuning.fDeltaSmoothingPerSec * fTimeDelta); // 프레임레이트 독립
	vSmoothed = XMVectorLerp(vSmoothed, vRaw, fAlpha);
	XMStoreFloat3(&m_MoveState.vSmoothedLocalDelta, vSmoothed);

	_vector vLocalDelta = bHasInput && !m_MoveState.Pivoting ? vSmoothed : XMVectorZero();
	m_pTransformCom->Move_Delta(vLocalDelta, pNavigation);
}

void CContainerObject::Free()
{
	__super::Free();

	m_PartObjects.for_each([](auto& pair) { Safe_Release(pair.second); });
	m_PartObjects.clear();
}
