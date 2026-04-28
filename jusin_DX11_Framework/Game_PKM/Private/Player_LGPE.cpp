#include "Player_LGPE.h"
#include "GameInstance.h"

#include "Body_Hero.h"

CPlayer_LGPE::CPlayer_LGPE(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CContainerObject{ pDevice, pContext }
{
	m_strName = { L"Player_LGPE" };
}

CPlayer_LGPE::CPlayer_LGPE(const CPlayer_LGPE& Prototype)
	: CContainerObject{ Prototype }
{

}

HRESULT CPlayer_LGPE::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer_LGPE::Initialize(void* pArg)
{
	GAMEOBJECT_DESC Desc{};

	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(1800.f);

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	m_pTransformCom->Set_State(STATE::POSITION, m_pNavigationCom->Get_CellPos());

	return S_OK;
}

void CPlayer_LGPE::Priority_Update(_float fTimeDelta)
{
	m_PartObjects.for_each([&fTimeDelta](auto& Pair)
		{
			if (nullptr != Pair.second)
				Pair.second->Priority_Update(fTimeDelta);
		});
}

void CPlayer_LGPE::Update(_float fTimeDelta)
{
	// 1) 파츠 애니메이션 진행 : 이전 프레임에 애님 결정 -> RootMotionDelta 생성
	m_PartObjects.for_each([&fTimeDelta](auto& Pair)
		{
			if (nullptr != Pair.second)
				Pair.second->Update(fTimeDelta);
		});
	
	// 2) 입력 → 이동 방향
	_vector vMoveDir = Read_MoveInput();
	_bool bHasInput = (XMVectorGetX(XMVector3LengthSq(vMoveDir)) > 1e-6f);
	CBody_Hero* pBody = static_cast<CBody_Hero*>(m_PartObjects[PART_BODY]);

	Tick_RootMotionMovement(vMoveDir, bHasInput, pBody->Get_RootMotionDelta(), m_pNavigationCom, fTimeDelta);

	// 3) 다음 프레임 애니메이션 상태
	Update_AnimState(bHasInput);
}

void CPlayer_LGPE::Late_Update(_float fTimeDelta)
{
	m_PartObjects.for_each([&fTimeDelta](auto& Pair)
		{
			if (nullptr != Pair.second)
				Pair.second->Late_Update(fTimeDelta);
		});

	m_pGameInstance->Add_RenderGroup(RENDERID::NONBLEND, this);
#ifdef _DEBUG
	//m_pGameInstance->Add_DebugComponent(m_pColliderCom);
	m_pGameInstance->Add_DebugComponent(m_pNavigationCom);
#endif
}

HRESULT CPlayer_LGPE::Render()
{
	return S_OK;
}

HRESULT CPlayer_LGPE::Ready_Components()
{
	/* For.Com_Navigation */
	CNavigation::NAVIGATION_DESC NaviDesc{ 5 };

	if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_NAVIGATION_MAP,
		COM_NAVIGATION, reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_LGPE::Ready_PartObjects()
{
	CBody_Hero::BODY_HERO_DESC BodyDesc{};
	BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	BodyDesc.pParentState = &m_iState;

	if (FAILED(__super::Add_PartObject(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_BODY_HERO, PART_BODY, &BodyDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_LGPE::Bind_ShaderResources()
{



	return S_OK;
}

_vector CPlayer_LGPE::Read_MoveInput() const
{
	_vector vMoveDir = XMVectorZero();

	_float x = 0.f;
	_float z = 0.f;

	if (m_pGameInstance->Key_Pressing(DIK_LEFT))  x -= 1.f;
	if (m_pGameInstance->Key_Pressing(DIK_RIGHT)) x += 1.f;
	if (m_pGameInstance->Key_Pressing(DIK_UP))    z += 1.f;
	if (m_pGameInstance->Key_Pressing(DIK_DOWN))  z -= 1.f;

	if (x == 0.f && z == 0.f)
		return XMVectorZero();

	return XMVector3Normalize(XMVectorSet(x, 0.f, z, 0.f));
}

void CPlayer_LGPE::Update_AnimState(_bool bHasInput)
{
	CBody_Hero* pBody = static_cast<CBody_Hero*>(m_PartObjects[PART_BODY]);
	pBody->Set_Anim(bHasInput && !m_MoveState.Pivoting ? RUN : IDLE, true);
}

CPlayer_LGPE* CPlayer_LGPE::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer_LGPE* pInstance = new CPlayer_LGPE(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer_LGPE");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer_LGPE::Clone(void* pArg)
{
	CPlayer_LGPE* pInstance = new CPlayer_LGPE(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer_LGPE");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_LGPE::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
}