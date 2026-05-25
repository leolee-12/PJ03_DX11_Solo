#include "Player_LGPE.h"
#include "Body_Hero.h"
#include "Actor.h"
#include "Interaction.h"
#include "Level_GamePlay.h"
#include "Battle_AnimDef.h"
#include "Game_API.h"
#include "Effect_Manager.h"

#include "GameInstance.h"

namespace
{
	constexpr _float INTERACT_MAX_DIST = 3.0f;
	constexpr _float INTERACT_COS_ANGLE = 0.5f;   // cos(60°)

	_bool Is_GamePlayDialogueActive(CGameInstance* pGameInstance)
	{
		if (nullptr == pGameInstance)
			return false;

		CLevel* pCurrent = pGameInstance->Get_CurrentLevelPtr();
		CLevel_GamePlay* pGamePlay = dynamic_cast<CLevel_GamePlay*>(pCurrent);
		if (nullptr == pGamePlay)
			return false;

		return pGamePlay->Is_Dialogue_Playing();
	}
}

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
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
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

	if (true == Is_GamePlayDialogueActive(m_pGameInstance))	// 대화 중엔 입력 차단
	{
		if (nullptr != m_pColliderCom)
			m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

		Update_AnimState(false);
		m_pCurrentInteractTarget = nullptr;
		return;
	}
	
	// 2) 입력 -> 이동 방향
	_vector vMoveDir = Read_MoveInput();
	_bool bHasInput = (XMVectorGetX(XMVector3LengthSq(vMoveDir)) > 1e-6f);
	CBody_Hero* pBody = static_cast<CBody_Hero*>(m_PartObjects[PART_BODY]);

	Tick_RootMotionMovement(vMoveDir, bHasInput, pBody->Get_RootMotionDelta(), m_pNavigationCom, fTimeDelta);

	// 2.5) 콜라이더 world 갱신 - 위치 확정 직후
	if (nullptr != m_pColliderCom)
		m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

	// 3) 다음 프레임 애니메이션 상태
	Update_AnimState(bHasInput);

	// 4) 상호작용 후보 탐색 + 입력 처리
	Update_Interaction(fTimeDelta);
}

void CPlayer_LGPE::Late_Update(_float fTimeDelta)
{
	m_PartObjects.for_each([&fTimeDelta](auto& Pair)
		{
			if (nullptr != Pair.second)
				Pair.second->Late_Update(fTimeDelta);
		});

	Update_TouchTriggers();

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
	m_pGameInstance->Add_DebugComponent(m_pNavigationCom);
#endif
}

HRESULT CPlayer_LGPE::Render()
{
	return S_OK;
}

void CPlayer_LGPE::Clear_TouchSet()
{
	m_PrevTouchSet.clear();
}


HRESULT CPlayer_LGPE::Ready_Components()
{
	/* For.Com_Navigation */
	CNavigation::NAVIGATION_DESC NaviDesc{ 67 };	// Pallet Town
	//CNavigation::NAVIGATION_DESC NaviDesc{ 669 };	// Gym

	if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), PROTO_COM_NAVIGATION_MAP,
		COM_NAVIGATION, reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
		return E_FAIL;

	/* For.Com_Collider_Sphere - TOUCH 트리거용 */
	CBounding_Sphere::BOUNDING_SPHERE_DESC SphereDesc{};
	SphereDesc.vCenter = _float3(0.f, 0.5f, 0.f);
	SphereDesc.fRadius = 0.5f;

	if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_COLLIDER_SPHERE,
		COM_COLLIDER_SPHERE, reinterpret_cast<CComponent**>(&m_pColliderCom), &SphereDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_LGPE::Ready_PartObjects()
{
	m_strBodyModelProtoTag = PROTO_COM_MODEL_HERO;

	CBody_Hero::BODY_HERO_DESC BodyDesc{};
	BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	BodyDesc.strModelProtoTag = m_strBodyModelProtoTag;
	BodyDesc.strShaderProtoTag = PROTO_COM_SHADER_PLAYER_LGPE;
	BodyDesc.iDefaultAnim = BattleAnim::Find_AnimIndex(m_strBodyModelProtoTag, ANIM_KIND::IDLE);
	BodyDesc.bLoop = true;
	BodyDesc.fScale = 1.f;
	BodyDesc.bEnableRootMotion = true;
	BodyDesc.iRootMotionBoneIndex = 3;
	BodyDesc.pParentState = &m_iState;

	if (FAILED(__super::Add_PartObject(ETOUI(LEVEL::STATIC), PROTO_OBJ_BODY_HERO, PART_BODY, &BodyDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_LGPE::Bind_ShaderResources()
{



	return S_OK;
}

_vector CPlayer_LGPE::Read_MoveInput() const
{
	if (UI_Is_AnyOpen())
		return XMVectorZero();

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
	if (nullptr == pBody)
		return;

	const ANIM_KIND eAnimKind =
		(bHasInput && !m_MoveState.Pivoting)
		? ANIM_KIND::WALK
		: ANIM_KIND::IDLE;

	pBody->Set_Anim(
		BattleAnim::Find_AnimIndex(m_strBodyModelProtoTag, eAnimKind),
		true);
}

void CPlayer_LGPE::Update_Interaction(_float fTimeDelta)
{
	if (true == Is_GamePlayDialogueActive(m_pGameInstance))
	{
		m_pCurrentInteractTarget = nullptr;
		return;
	}

	m_pCurrentInteractTarget = Find_InteractionCandidate();

	if (m_pGameInstance->Key_Down(DIK_F))
	{
		OutputDebugStringW(L"[Player] F pressed\n");
		if (nullptr == m_pCurrentInteractTarget)
		{
			OutputDebugStringW(L"[Player] No target (Find_InteractionCandidate returned nullptr\n");
				return;
		}

		OutputDebugStringW(L"[Player] Target found, calling Try_Talk\n");
		Try_Talk();
	}
}

CActor* CPlayer_LGPE::Find_InteractionCandidate() const
{
	const list<CGameObject*>* pNpcList =
		m_pGameInstance->Get_ObjectList(ETOUI(LEVEL::GAMEPLAY), LAYER_NPC);

	if (nullptr == pNpcList || pNpcList->empty())
		return nullptr;

	const _vector vPlayerPos = m_pTransformCom->Get_State(STATE::POSITION);
	const _vector vPlayerLook = XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK));

	CActor* pBest = nullptr;
	_float fBestDistSq = INTERACT_MAX_DIST * INTERACT_MAX_DIST;

	for (CGameObject* pObject : *pNpcList)
	{
		CActor* pActor = dynamic_cast<CActor*>(pObject);
		if (nullptr == pActor)
			continue;

		const _vector vNpcPos = pActor->Get_Transform()->Get_State(STATE::POSITION);
		const _vector vDelta = vNpcPos - vPlayerPos;
		const _float fDistSq = XMVectorGetX(XMVector3LengthSq(vDelta));

		if (fDistSq > fBestDistSq)
			continue;

		const _vector vToNpc = XMVector3Normalize(vDelta);
		const _float fDot = XMVectorGetX(XMVector3Dot(vPlayerLook, vToNpc));

		if (fDot < INTERACT_COS_ANGLE)
			continue;

		pBest = pActor;
		fBestDistSq = fDistSq;
	}

	return pBest;
}

void CPlayer_LGPE::Try_Talk()
{
	if (nullptr == m_pCurrentInteractTarget)
		return;

	INTERACTION_CONTEXT ctx;
	ctx.pCaller = this;
	ctx.pTarget = m_pCurrentInteractTarget;
	ctx.eEvent = INTERACTION_EVENT::TALK;

	const _vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
	XMStoreFloat4(&ctx.vCallerPosition, vPos);

	const _vector vLook = m_pTransformCom->Get_State(STATE::LOOK);
	XMStoreFloat4(&ctx.vCallerLook, vLook);

	const _bool bResult = m_pCurrentInteractTarget->TryInteract(ctx);
	OutputDebugStringW(bResult
		? L"[Player] TryInteract = true\n"
		: L"[Player] TryInteract = false\n");

	/* F 로 상호작용이 수락됐을 때, 메시지 진행(Enter/Space)과 동일한 확인음을 재생. */
	if (true == bResult)
		m_pGameInstance->Play(L"SFX/MsgBox_Enter.wav", CHANNELID::UI, 0.7f);
}

void CPlayer_LGPE::Update_TouchTriggers()
{
	const list<CGameObject*>* pList =
		m_pGameInstance->Get_ObjectList(ETOUI(LEVEL::GAMEPLAY), LAYER_INTERACTABLE);

	unordered_set<CActor*> CurrentTouchSet;

	if (nullptr == m_pColliderCom || nullptr == pList || pList->empty())
	{
		m_PrevTouchSet.clear();
		return;
	}

	CActor* pNewlyEnteredFirst = nullptr;

	for (CGameObject* pObject : *pList)
	{
		CActor* pActor = dynamic_cast<CActor*>(pObject);
		if (nullptr == pActor)
			continue;

		CCollider* pTargetCollider = pActor->Get_Component<CCollider>(COM_COLLIDER_SPHERE);
		if (nullptr == pTargetCollider)
			continue;

		if (!m_pColliderCom->Intersect(pTargetCollider))
			continue;

		CurrentTouchSet.insert(pActor);

		// 직전 프레임에도 overlap 이었으면 Enter 가 아님 - 발화 제외
		if (m_PrevTouchSet.find(pActor) != m_PrevTouchSet.end())
			continue;

		// 첫 Enter 액터 1개만 발화 (다중 진입 시 중복 Push_Level 방지)
		if (nullptr == pNewlyEnteredFirst)
			pNewlyEnteredFirst = pActor;
	}

	if (nullptr != pNewlyEnteredFirst)
		Fire_Touch(pNewlyEnteredFirst);

	// weak 비교만 수행 - dangling 안전. 레벨 Push/Pop 경계는 GAMEPLAY::OnResume 이 Clear_TouchSet() 호출로 정리.
	m_PrevTouchSet = std::move(CurrentTouchSet);
}

void CPlayer_LGPE::Fire_Touch(CActor* pActor)
{
	if (nullptr == pActor)
		return;

	INTERACTION_CONTEXT ctx;
	ctx.pCaller = this;
	ctx.pTarget = pActor;
	ctx.eEvent = INTERACTION_EVENT::TOUCH;

	const _vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
	XMStoreFloat4(&ctx.vCallerPosition, vPos);

	const _vector vLook = m_pTransformCom->Get_State(STATE::LOOK);
	XMStoreFloat4(&ctx.vCallerLook, vLook);

	const _bool bResult = pActor->TryInteract(ctx);
	OutputDebugStringW(bResult
		? L"[Player] TOUCH fired = true\n"
		: L"[Player] TOUCH fired = false\n");
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
	m_PrevTouchSet.clear();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pNavigationCom);
	
	__super::Free();
}
