#include "MonsterBall.h"
#include "GameInstance.h"

#include "Collider.h"
#include "Bounding_Sphere.h"

namespace
{
	const _float4x4 g_IdentityMatrix =
	{
			1.f, 0.f, 0.f, 0.f,
			0.f, 1.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			0.f, 0.f, 0.f, 1.f
	};

	constexpr _float IMPACT_BOUNCE_DISTANCE = 0.65f;
	constexpr _float IMPACT_BOUNCE_DURATION = 0.32f;
	constexpr _float IMPACT_BOUNCE_UP_BIAS = 0.25f;
	constexpr _float IMPACT_OPEN_CLOSE_WAIT = 0.50f;
	constexpr _float MISS_BOUNCE_FORWARD = 0.65f;
	constexpr _float MISS_BOUNCE_HEIGHT = 0.45f;
	constexpr _float MISS_BOUNCE_DURATION = 0.42f;
	constexpr _float MISS_FALL_DURATION = 0.18f;
}

CMonsterBall::CMonsterBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
	m_strName = L"MonsterBall";
}

CMonsterBall::CMonsterBall(const CMonsterBall& Prototype)
	: CPartObject{ Prototype }
{
}

HRESULT CMonsterBall::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonsterBall::Initialize(void* pArg)
{
	MONSTER_BALL_DESC Desc{};

	if (nullptr != pArg)
		Desc = *static_cast<const MONSTER_BALL_DESC*>(pArg);

	if (nullptr == Desc.pParentMatrix)
		Desc.pParentMatrix = &g_IdentityMatrix;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	m_vStartPos = Desc.vSpawnPos;
	m_vTargetPos = Desc.vTargetPos;
	m_fFlightDuration = Desc.fFlightDuration;
	m_fArcHeight = Desc.fArcHeight;
	m_fImpactDuration = Desc.fImpactDuration;
	

	if (FAILED(Ready_Components()))
		return E_FAIL;

	/* 시각 보정 - Scaling 후 Rotation(X축 -60°). CTransform::Rotation 은 현재 스케일을 보존하므로
	   Scaling -> Rotation 순서로 호출 안전. Set_State(POSITION) 은 RIGHT/UP/LOOK 보존 -> 회전/스케일 유지. */
	_float fScale = 1.f;
	m_pTransformCom->Scaling(fScale, fScale, fScale);
	//m_pTransformCom->Rotation(XMConvertToRadians(-60.f), 0.f, 0.f);

	Set_CenterPosition(m_vStartPos);

	Update_Collider();

	m_pModelCom->Set_AnimationIndex(ETOUI(ANIM::CAPTURE_IDLE), true);
	m_pModelCom->Play_Animation(0.f);
	m_eState = BALL_STATE::READY;
	m_fElapsed = 0.f;

	return S_OK;
}

void CMonsterBall::Priority_Update(_float fTimeDelta)
{
}

void CMonsterBall::Update(_float fTimeDelta)
{
#ifdef _DEBUG
	if (m_pGameInstance->Key_Down(DIK_Z))
	{
		m_iDummy++;

		if (m_iDummy >= m_pModelCom->Get_NumAnimations())
			m_iDummy = 0;

		m_pModelCom->Set_AnimationIndex(m_iDummy);
	}
#endif

	switch (m_eState)
	{
	case BALL_STATE::READY:
		Update_Ready(fTimeDelta);
		break;

	case BALL_STATE::FLYING:
		Update_Flying(fTimeDelta);
		break;

	case BALL_STATE::IMPACT:
		Update_Impact(fTimeDelta);
		break;

	case BALL_STATE::STAGE_DROP:
		Update_StageDrop(fTimeDelta);
		break;

	case BALL_STATE::STAGE_SHAKE:
		Update_StageShake(fTimeDelta);
		break;

	case BALL_STATE::BATTLE_SENDOUT:
		Update_BattleSendOut(fTimeDelta);
		break;

	case BALL_STATE::DONE:
		Update_Done(fTimeDelta);
		break;

	default:
		break;
	}

	Update_Collider();
}

void CMonsterBall::Late_Update(_float fTimeDelta)
{
	__super::Compute_CombinedWorldMatrix(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

	if (!m_bVisible) return;
	
	m_pGameInstance->Add_RenderGroup(RENDERID::NONBLEND, this);
	m_pGameInstance->Add_RenderGroup(RENDERID::SHADOW, this);

#ifdef _DEBUG
	if (nullptr != m_pColliderCom)
		m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CMonsterBall::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	size_t iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_TexDiff", i, MATERIAL_TYPE::DIFFUSE, 0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_TexNorm", i, MATERIAL_TYPE::NORMALS, 0)))
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

HRESULT CMonsterBall::Render_Shadow()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pTransformCom->Bind_ShaderResourceWIT(m_pShaderCom, "g_WITMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Shadow_Transform(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Shadow_Transform(D3DTS::PROJ))))
		return E_FAIL;

	size_t iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_TexDiff", i, MATERIAL_TYPE::DIFFUSE, 0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_TexNorm", i, MATERIAL_TYPE::NORMALS, 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(1)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

void CMonsterBall::Set_AimPose(const _float3& vStartPos, const _float3& vTargetPos)
{
	if (BALL_STATE::FLYING == m_eState
		|| BALL_STATE::IMPACT == m_eState
		|| BALL_STATE::STAGE_DROP == m_eState
		|| BALL_STATE::STAGE_SHAKE == m_eState
		|| BALL_STATE::BATTLE_SENDOUT == m_eState)
		return;

	m_vStartPos = vStartPos;
	m_vTargetPos = vTargetPos;

	Face_CenterTo(m_vStartPos, m_vTargetPos);
	Set_CenterPosition(m_vStartPos);
	Update_Collider();
}

void CMonsterBall::Play_BattleOpen(const _float3& vCenterPos, const _float3& vFaceTarget)
{
	m_eState = BALL_STATE::BATTLE_SENDOUT;
	m_fElapsed = 0.f;
	m_bVisible = true;
	m_bWaitCloseAfterOpen = false;
	m_bOpenFinished = false;

	m_eBounceMode = BOUNCE_MODE::NONE;
	m_fBounceTime = 0.f;
	m_fBounceDuration = 0.f;
	m_fBounceHeight = 0.f;

	m_bStageDropFinished = false;
	m_fStageDropTime = 0.f;
	m_fStageDropDuration = 0.f;

	m_vShakeCenter = {};
	m_vShakePivotPos = {};
	m_fShakeTime = 0.f;
	m_fShakeDuration = 0.f;
	m_fShakeAngleRad = 0.f;
	m_bShakeFinished = false;

	Face_CenterToYaw(vCenterPos, vFaceTarget);
	Set_CenterPosition(vCenterPos);

	if (nullptr != m_pModelCom)
	{
		m_pModelCom->Set_AnimationIndex(ETOUI(ANIM::CAPTURE_IDLE), true, 0.f);
		m_pModelCom->Play_Animation(0.f);
		m_pModelCom->Set_AnimationIndex(ETOUI(ANIM::BATTLE_OPEN), false, 0.f);
		m_pModelCom->Play_Animation(0.f);
	}

	Update_Collider();
	OutputDebugStringW(L"[MonsterBall] BATTLE_SENDOUT -> BATTLE_OPEN\n");
}

void CMonsterBall::Launch()
{
	/* READY 외 상태에서 들어온 Launch 는 무시. */
	if (BALL_STATE::READY != m_eState)
		return;

	m_eState = BALL_STATE::FLYING;
	m_fElapsed = 0.f;
	m_bVisible = true;   // 발사 시 무조건 가시화 (이전 Hide 가 있어도 복귀)
	m_bWaitCloseAfterOpen = false;
	m_bOpenFinished = false;

	m_eBounceMode = BOUNCE_MODE::NONE;
	m_fBounceTime = 0.f;
	m_fBounceDuration = 0.f;
	m_fBounceHeight = 0.f;

	m_bStageDropFinished = false;
	m_fStageDropTime = 0.f;
	m_fStageDropDuration = 0.f;

	m_vShakeCenter = {};
	m_vShakePivotPos = {};
	m_fShakeTime = 0.f;
	m_fShakeDuration = 0.f;
	m_fShakeAngleRad = 0.f;
	m_bShakeFinished = false;

	OutputDebugStringW(L"[MonsterBall] READY -> FLYING (Show)\n");
}

void CMonsterBall::Reset()
{
	m_eState = BALL_STATE::READY;
	m_fElapsed = 0.f;
	m_bWaitCloseAfterOpen = false;
	m_bOpenFinished = false;

	m_eBounceMode = BOUNCE_MODE::NONE;
	m_fBounceTime = 0.f;
	m_fBounceDuration = 0.f;
	m_fBounceHeight = 0.f;

	m_bStageDropFinished = false;
	m_fStageDropTime = 0.f;
	m_fStageDropDuration = 0.f;

	m_vShakeCenter = {};
	m_vShakePivotPos = {};
	m_fShakeTime = 0.f;
	m_fShakeDuration = 0.f;
	m_fShakeAngleRad = 0.f;
	m_bShakeFinished = false;

	Set_CenterPosition(m_vStartPos);

	if (nullptr != m_pModelCom)
	{
		m_pModelCom->Set_AnimationIndex(ETOUI(ANIM::CAPTURE_IDLE), true);
		m_pModelCom->Play_Animation(0.f);
	}

	Update_Collider();

	OutputDebugStringW(L"[MonsterBall] Reset -> READY\n");
}

void CMonsterBall::Hide()
{
	m_bVisible = false;
	OutputDebugStringW(L"[MonsterBall] Hide\n");
}

void CMonsterBall::Show()
{
	m_bVisible = true;
	OutputDebugStringW(L"[MonsterBall] Show\n");
}

void CMonsterBall::Trigger_Impact(const _float3& vTargetCenter)
{
	if (BALL_STATE::FLYING != m_eState)
		return;

	const _vector vTargetCenterV = XMLoadFloat3(&vTargetCenter);
	const _vector vBallCenterV = XMLoadFloat3(&m_vCenterPos);

	_vector vNormal = vBallCenterV - vTargetCenterV;

	if (XMVectorGetX(XMVector3LengthSq(vNormal)) <= 0.000001f)
		vNormal = XMLoadFloat3(&m_vStartPos) - vTargetCenterV;

	if (XMVectorGetX(XMVector3LengthSq(vNormal)) <= 0.000001f)
		vNormal = XMVectorSet(0.f, 0.f, -1.f, 0.f);

	vNormal = XMVector3Normalize(vNormal);

	/* 반사 방향에 위쪽 성분을 강제로 가산해 항상 위로 튕기도록 보정. */
	vNormal = vNormal + XMVectorSet(0.f, IMPACT_BOUNCE_UP_BIAS, 0.f, 0.f);
	vNormal = XMVector3Normalize(vNormal);

	_float3 vStartCenter{};
	_float3 vEndCenter{};
	XMStoreFloat3(&vStartCenter, vBallCenterV);
	XMStoreFloat3(&vEndCenter, vBallCenterV + vNormal * IMPACT_BOUNCE_DISTANCE);

	Face_CenterTo(vStartCenter, vTargetCenter);
	Begin_Bounce(BOUNCE_MODE::IMPACT_RECOIL, vStartCenter, vEndCenter, IMPACT_BOUNCE_DURATION, 0.f);
}

void CMonsterBall::Begin_StageDrop(
	const _float3& vAirCenter,
	const _float3& vGroundCenter,
	const _float3& vFaceTarget,
	_float fDuration)
{
	m_eState = BALL_STATE::STAGE_DROP;
	m_fElapsed = 0.f;

	m_eBounceMode = BOUNCE_MODE::NONE;
	m_bWaitCloseAfterOpen = false;
	m_bOpenFinished = false;

	m_vStageDropStartCenter = vAirCenter;
	m_vStageDropEndCenter = vGroundCenter;
	m_fStageDropTime = 0.f;
	m_fStageDropDuration = max(fDuration, 0.001f);
	m_bStageDropFinished = false;

	m_vShakeCenter = {};
	m_vShakePivotPos = {};
	m_fShakeTime = 0.f;
	m_fShakeDuration = 0.f;
	m_fShakeAngleRad = 0.f;
	m_bShakeFinished = false;

	m_bVisible = true;

	Face_CenterToYaw(m_vStageDropStartCenter, vFaceTarget);
	Set_CenterPosition(m_vStageDropStartCenter);
	Update_Collider();

	OutputDebugStringW(L"[MonsterBall] Begin_StageDrop\n");
}

HRESULT CMonsterBall::Ready_Components()
{
	/* Shader - 애니메이션 메시용 VTXANIMMESH 사용. */
	if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), PROTO_COM_SHADER_VTXANIMMESH,
		COM_SHADER, reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Model - ball.wmodel (Loader 등록). */
	if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), PROTO_COM_MODEL_MONSTER_BALL,
		COM_MODEL, reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	CBounding_Sphere::BOUNDING_SPHERE_DESC SphereDesc{};
	SphereDesc.vCenter = _float3(0.f, 0.2f, 0.f);
	SphereDesc.fRadius = 0.15f;

	if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_COLLIDER_SPHERE,
		COM_COLLIDER_SPHERE, reinterpret_cast<CComponent**>(&m_pColliderCom), &SphereDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonsterBall::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pTransformCom->Bind_ShaderResourceCombinedWIT(m_pShaderCom, "g_WITMatrix", XMLoadFloat4x4(&m_CombinedWorldMatrix))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix",
		m_pGameInstance->Get_Transform(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix",
		m_pGameInstance->Get_Transform(D3DTS::PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFarZ", m_pGameInstance->Get_FarZPtr(), sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

void CMonsterBall::Update_Ready(_float fTimeDelta)
{
	if (nullptr != m_pModelCom)
		m_pModelCom->Play_Animation(fTimeDelta);
}

void CMonsterBall::Update_BattleSendOut(_float fTimeDelta)
{
	m_fElapsed += fTimeDelta;

	if (nullptr == m_pModelCom)
	{
		m_bOpenFinished = true;
		m_eState = BALL_STATE::DONE;
		return;
	}

	if (m_pModelCom->Play_Animation(fTimeDelta))
	{
		m_bOpenFinished = true;
		m_eState = BALL_STATE::DONE;
		m_fElapsed = 0.f;
		OutputDebugStringW(L"[MonsterBall] BATTLE_OPEN finished -> DONE\n");
	}
}

void CMonsterBall::Update_Flying(_float fTimeDelta)
{
	m_fElapsed += fTimeDelta;

	m_pTransformCom->Turn(
		XMVector3Normalize(m_pTransformCom->Get_State(STATE::RIGHT)),
		fTimeDelta);

	const _float3 vPrevCenter = m_vCenterPos;

	Update_Position();

	const _float fGroundCenterY = m_vLocalCenter.y;

	if (vPrevCenter.y > fGroundCenterY && m_vCenterPos.y <= fGroundCenterY)
	{
		const _float fDenom = vPrevCenter.y - m_vCenterPos.y;
		const _float fRatio = (fDenom > 1e-6f)
			? (vPrevCenter.y - fGroundCenterY) / fDenom
			: 1.f;

		const _vector vPrev = XMLoadFloat3(&vPrevCenter);
		const _vector vCurr = XMLoadFloat3(&m_vCenterPos);

		_float3 vGroundCenter{};
		XMStoreFloat3(&vGroundCenter, XMVectorLerp(vPrev, vCurr, fRatio));
		vGroundCenter.y = fGroundCenterY;

		Set_CenterPosition(vGroundCenter);
		Begin_MissBounce();

		OutputDebugStringW(L"[MonsterBall] FLYING -> MISS_BOUNCE (ground)\n");
		return;
	}

	if (m_fElapsed > 0.05f && m_vCenterPos.y <= fGroundCenterY)
	{
		_float3 vGroundCenter = m_vCenterPos;
		vGroundCenter.y = fGroundCenterY;

		Set_CenterPosition(vGroundCenter);
		Begin_MissBounce();

		OutputDebugStringW(L"[MonsterBall] FLYING -> MISS_BOUNCE (ground fallback)\n");
		return;
	}
}

void CMonsterBall::Update_Impact(_float fTimeDelta)
{
	m_fElapsed += fTimeDelta;

	if (BOUNCE_MODE::NONE != m_eBounceMode)
	{
		Update_Bounce(fTimeDelta);
		return;
	}

	if (m_bWaitCloseAfterOpen)
	{
		if (m_fElapsed < IMPACT_OPEN_CLOSE_WAIT)
			return;

		m_bWaitCloseAfterOpen = false;
		m_fElapsed = 0.f;

		if (nullptr != m_pModelCom)
		{
			m_pModelCom->Set_AnimationIndex(ETOUI(ANIM::CLOSE), false);
			m_pModelCom->Play_Animation(0.f);
		}

		OutputDebugStringW(L"[MonsterBall] IMPACT wait -> CLOSE\n");
		return;
	}

	if (nullptr != m_pModelCom
		&& ETOUI(ANIM::OPEN) == m_pModelCom->Get_CurrAnimIndex())
	{
		if (m_pModelCom->Play_Animation(fTimeDelta))
		{
			m_bOpenFinished = true;
			m_bWaitCloseAfterOpen = true;
			m_fElapsed = 0.f;

			OutputDebugStringW(L"[MonsterBall] IMPACT OPEN finished -> wait\n");
		}

		return;
	}

	if (nullptr != m_pModelCom
		&& ETOUI(ANIM::CLOSE) == m_pModelCom->Get_CurrAnimIndex())
	{
		if (m_pModelCom->Play_Animation(fTimeDelta))
		{
			m_eState = BALL_STATE::DONE;
			m_fElapsed = 0.f;

			OutputDebugStringW(L"[MonsterBall] IMPACT -> DONE (close anim finished)\n");
		}

		return;
	}

	if (m_fElapsed >= m_fImpactDuration)
	{
		m_eState = BALL_STATE::DONE;
		m_fElapsed = 0.f;

		OutputDebugStringW(L"[MonsterBall] IMPACT -> DONE\n");
	}
}

void CMonsterBall::Update_StageDrop(_float fTimeDelta)
{
	m_fStageDropTime += fTimeDelta;

	const _float fT = (m_fStageDropDuration > 1e-6f)
		? min(m_fStageDropTime / m_fStageDropDuration, 1.f)
		: 1.f;

	const _float fDropT = fT * fT;

	const _vector vStart = XMLoadFloat3(&m_vStageDropStartCenter);
	const _vector vEnd = XMLoadFloat3(&m_vStageDropEndCenter);

	_vector vCenter = XMVectorLerp(vStart, vEnd, fT);
	vCenter = XMVectorSetY(
		vCenter,
		m_vStageDropStartCenter.y + (m_vStageDropEndCenter.y - m_vStageDropStartCenter.y) * fDropT);

	_float3 vCenterF{};
	XMStoreFloat3(&vCenterF, vCenter);
	Set_CenterPosition(vCenterF);

	if (fT < 1.f)
		return;

	Set_CenterPosition(m_vStageDropEndCenter);

	m_eState = BALL_STATE::DONE;
	m_fElapsed = 0.f;
	m_bStageDropFinished = true;

	OutputDebugStringW(L"[MonsterBall] STAGE_DROP -> DONE\n");
}

void CMonsterBall::Update_StageShake(_float fTimeDelta)
{
	m_fShakeTime += fTimeDelta;

	const _float fT = (m_fShakeDuration > 1e-6f)
		? min(m_fShakeTime / m_fShakeDuration, 1.f)
		: 1.f;

	_float fRoll = 0.f;

	if (fT < 0.375f)
	{
		const _float fSegT = fT / 0.375f;
		fRoll = -m_fShakeAngleRad * fSegT;
	}
	else if (fT < 0.75f)
	{
		const _float fSegT = (fT - 0.375f) / 0.375f;
		fRoll = -m_fShakeAngleRad + (2.f * m_fShakeAngleRad * fSegT);
	}
	else
	{
		const _float fSegT = (fT - 0.75f) / 0.25f;
		fRoll = m_fShakeAngleRad * (1.f - fSegT);
	}

	Apply_ShakeRoll(fRoll);

	if (fT < 1.f)
		return;

	Apply_ShakeRoll(0.f);

	m_eState = BALL_STATE::DONE;
	m_fElapsed = 0.f;
	m_bShakeFinished = true;

	OutputDebugStringW(L"[MonsterBall] STAGE_SHAKE -> DONE\n");
}

void CMonsterBall::Begin_OneShake(_float fDuration, _float fAngleDeg)
{
	m_eState = BALL_STATE::STAGE_SHAKE;
	m_fElapsed = 0.f;

	m_eBounceMode = BOUNCE_MODE::NONE;
	m_bWaitCloseAfterOpen = false;
	m_bOpenFinished = false;

	m_vShakeCenter = m_vCenterPos;

	const _vector vPivotPos = m_pTransformCom->Get_State(STATE::POSITION);
	XMStoreFloat3(&m_vShakePivotPos, vPivotPos);

	m_fShakeTime = 0.f;
	m_fShakeDuration = max(fDuration, 0.001f);
	m_fShakeAngleRad = XMConvertToRadians(fAngleDeg);
	m_bShakeFinished = false;

	Reset_UprightBasisFromCurrentYaw();
	m_pTransformCom->Set_State(
		STATE::POSITION,
		XMVectorSetW(XMLoadFloat3(&m_vShakePivotPos), 1.f));

	const _vector vLocalCenter = XMLoadFloat3(&m_vLocalCenter);
	const _vector vCenter =
		vPivotPos + XMVectorSet(0.f, XMVectorGetY(vLocalCenter), XMVectorGetZ(vLocalCenter), 0.f);
	XMStoreFloat3(&m_vCenterPos, vCenter);

	Update_Collider();

	OutputDebugStringW(L"[MonsterBall] Begin_OneShake\n");
}

void CMonsterBall::Update_Done(_float fTimeDelta)
{
}

void CMonsterBall::Update_Position()
{
	/* t는 1 이후도 허용한다. 미충돌 시 포물선을 계속 외삽해 바닥 y 판정으로 bounce 진입. */
	const _float t = (m_fFlightDuration > 1e-6f)
		? max(m_fElapsed / m_fFlightDuration, 0.f)
		: 1.f;

	/* XZ 선형 보간, Y 선형 + 표준 포물선 가산.
	   arc 항 4·H·t·(1-t) 는 t=0/1 에서 0, t=0.5 에서 H - 정점 높이 = H. */
	const _float x = m_vStartPos.x + (m_vTargetPos.x - m_vStartPos.x) * t;
	const _float z = m_vStartPos.z + (m_vTargetPos.z - m_vStartPos.z) * t;
	const _float yLin = m_vStartPos.y + (m_vTargetPos.y - m_vStartPos.y) * t;
	const _float yArc = 4.f * m_fArcHeight * t * (1.f - t);
	const _float y = yLin + yArc;

	Set_CenterPosition(_float3(x, y, z));
}

void CMonsterBall::Update_Collider()
{
	if (nullptr != m_pColliderCom)
		m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
}

void CMonsterBall::Set_CenterPosition(const _float3& vCenterPos)
{
	m_vCenterPos = vCenterPos;

	const _vector vLocalCenter = XMLoadFloat3(&m_vLocalCenter);

	const _vector vRight = m_pTransformCom->Get_State(STATE::RIGHT);
	const _vector vUp = m_pTransformCom->Get_State(STATE::UP);
	const _vector vLook = m_pTransformCom->Get_State(STATE::LOOK);

	const _vector vOffset =
		vRight * XMVectorGetX(vLocalCenter) +
		vUp * XMVectorGetY(vLocalCenter) +
		vLook * XMVectorGetZ(vLocalCenter);

	const _vector vCenter = XMLoadFloat3(&vCenterPos);
	const _vector vPosition = XMVectorSetW(vCenter - vOffset, 1.f);

	m_pTransformCom->Set_State(STATE::POSITION, vPosition);
}

void CMonsterBall::Face_CenterTo(const _float3& vCenterPos, const _float3& vTargetCenter)
{
	_vector vLook = XMLoadFloat3(&vTargetCenter) - XMLoadFloat3(&vCenterPos);

	if (XMVectorGetX(XMVector3LengthSq(vLook)) < 1e-6f)
		return;

	vLook = XMVector3Normalize(vLook);

	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	if (XMVectorGetX(XMVector3LengthSq(vRight)) < 1e-6f)
		vRight = XMVector3Cross(XMVectorSet(0.f, 0.f, 1.f, 0.f), vLook);

	vRight = XMVector3Normalize(vRight);
	_vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

	const _float3 vScale = m_pTransformCom->Get_Scaled();

	m_pTransformCom->Set_State(STATE::RIGHT, vRight * vScale.x);
	m_pTransformCom->Set_State(STATE::UP, vUp * vScale.y);
	m_pTransformCom->Set_State(STATE::LOOK, vLook * vScale.z);
}

void CMonsterBall::Face_CenterToYaw(const _float3& vCenterPos, const _float3& vTargetCenter)
{
	_vector vLook = XMLoadFloat3(&vTargetCenter) - XMLoadFloat3(&vCenterPos);
	vLook = XMVectorSetY(vLook, 0.f);

	if (XMVectorGetX(XMVector3LengthSq(vLook)) <= 0.000001f)
		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);

	vLook = XMVector3Normalize(vLook);

	const _float3 vScale = m_pTransformCom->Get_Scaled();

	const _vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector vRight = XMVector3Cross(vUp, vLook);

	if (XMVectorGetX(XMVector3LengthSq(vRight)) <= 0.000001f)
		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f);

	vRight = XMVector3Normalize(vRight);

	m_pTransformCom->Set_State(STATE::RIGHT, vRight * vScale.x);
	m_pTransformCom->Set_State(STATE::UP, vUp * vScale.y);
	m_pTransformCom->Set_State(STATE::LOOK, vLook * vScale.z);
}

void CMonsterBall::Reset_UprightBasisFromCurrentYaw()
{
	_vector vLook = m_pTransformCom->Get_State(STATE::LOOK);
	vLook = XMVectorSetY(vLook, 0.f);

	if (XMVectorGetX(XMVector3LengthSq(vLook)) <= 0.000001f)
		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);

	vLook = XMVector3Normalize(vLook);

	const _float3 vScale = m_pTransformCom->Get_Scaled();

	const _vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector vRight = XMVector3Cross(vUp, vLook);

	if (XMVectorGetX(XMVector3LengthSq(vRight)) <= 0.000001f)
		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f);

	vRight = XMVector3Normalize(vRight);

	m_pTransformCom->Set_State(STATE::RIGHT, vRight * vScale.x);
	m_pTransformCom->Set_State(STATE::UP, vUp * vScale.y);
	m_pTransformCom->Set_State(STATE::LOOK, vLook * vScale.z);
}

void CMonsterBall::Reset_UprightBasis()
{
	const _float3 vScale = m_pTransformCom->Get_Scaled();

	m_pTransformCom->Set_State(STATE::RIGHT, XMVectorSet(vScale.x, 0.f, 0.f, 0.f));
	m_pTransformCom->Set_State(STATE::UP, XMVectorSet(0.f, vScale.y, 0.f, 0.f));
	m_pTransformCom->Set_State(STATE::LOOK, XMVectorSet(0.f, 0.f, vScale.z, 0.f));
}

void CMonsterBall::Apply_ShakeRoll(_float fRollRad)
{
	const _float3 vScale = m_pTransformCom->Get_Scaled();
	const _float fCos = cosf(fRollRad);
	const _float fSin = sinf(fRollRad);

	_vector vLook = m_pTransformCom->Get_State(STATE::LOOK);
	vLook = XMVectorSetY(vLook, 0.f);

	if (XMVectorGetX(XMVector3LengthSq(vLook)) <= 0.000001f)
		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);

	vLook = XMVector3Normalize(vLook);

	const _vector vUpBase = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector vRightBase = XMVector3Cross(vUpBase, vLook);

	if (XMVectorGetX(XMVector3LengthSq(vRightBase)) <= 0.000001f)
		vRightBase = XMVectorSet(1.f, 0.f, 0.f, 0.f);

	vRightBase = XMVector3Normalize(vRightBase);

	const _vector vRight = vRightBase * fCos + vUpBase * fSin;
	const _vector vUp = -vRightBase * fSin + vUpBase * fCos;

	m_pTransformCom->Set_State(STATE::RIGHT, vRight * vScale.x);
	m_pTransformCom->Set_State(STATE::UP, vUp * vScale.y);
	m_pTransformCom->Set_State(STATE::LOOK, vLook * vScale.z);

	const _vector vPivotPos = XMVectorSetW(XMLoadFloat3(&m_vShakePivotPos), 1.f);
	m_pTransformCom->Set_State(STATE::POSITION, vPivotPos);

	const _vector vLocalCenter = XMLoadFloat3(&m_vLocalCenter);
	const _vector vCenter =
		vPivotPos +
		vRight * XMVectorGetX(vLocalCenter) +
		vUp * XMVectorGetY(vLocalCenter) +
		vLook * XMVectorGetZ(vLocalCenter);

	XMStoreFloat3(&m_vCenterPos, vCenter);
	Update_Collider();
}

void CMonsterBall::Begin_Bounce(BOUNCE_MODE eMode, const _float3& vStartCenter, const _float3& vEndCenter, _float fDuration, _float fHeight)
{
	m_eState = BALL_STATE::IMPACT;
	m_fElapsed = 0.f;
	m_bWaitCloseAfterOpen = false;
	m_bOpenFinished = false;

	m_vBounceStartCenter = vStartCenter;
	m_vBounceEndCenter = vEndCenter;
	m_fBounceTime = 0.f;
	m_fBounceDuration = fDuration;
	m_fBounceHeight = fHeight;
	m_eBounceMode = eMode;

	m_bStageDropFinished = false;
	m_fStageDropTime = 0.f;
	m_fStageDropDuration = 0.f;

	m_vShakeCenter = {};
	m_vShakePivotPos = {};
	m_fShakeTime = 0.f;
	m_fShakeDuration = 0.f;
	m_fShakeAngleRad = 0.f;
	m_bShakeFinished = false;

	Set_CenterPosition(m_vBounceStartCenter);
	Update_Collider();
}

void CMonsterBall::Begin_MissBounce()
{
	_vector vMoveDir = XMLoadFloat3(&m_vTargetPos) - XMLoadFloat3(&m_vStartPos);
	vMoveDir = XMVectorSetY(vMoveDir, 0.f);

	if (XMVectorGetX(XMVector3LengthSq(vMoveDir)) <= 0.000001f)
		vMoveDir = XMVectorSet(0.f, 0.f, 1.f, 0.f);

	vMoveDir = XMVector3Normalize(vMoveDir);

	_float3 vStartCenter = m_vCenterPos;
	if (vStartCenter.y < m_vLocalCenter.y)
		vStartCenter.y = m_vLocalCenter.y;

	_float3 vEndCenter{};
	XMStoreFloat3(&vEndCenter, XMLoadFloat3(&vStartCenter) + vMoveDir * MISS_BOUNCE_FORWARD);

	Begin_Bounce(BOUNCE_MODE::MISS_GROUND, vStartCenter, vEndCenter, MISS_BOUNCE_DURATION, MISS_BOUNCE_HEIGHT);
}

void CMonsterBall::Update_Bounce(_float fTimeDelta)
{
	const BOUNCE_MODE eMode = m_eBounceMode;

	m_fBounceTime += fTimeDelta;

	const _float fT = (m_fBounceDuration > 1e-6f)
		? min(m_fBounceTime / m_fBounceDuration, 1.f)
		: 1.f;

	const _vector vStart = XMLoadFloat3(&m_vBounceStartCenter);
	const _vector vEnd = XMLoadFloat3(&m_vBounceEndCenter);

	_vector vCenter = XMVectorLerp(vStart, vEnd, fT);

	if (m_fBounceHeight > 0.f)
	{
		const _float fBounceY = 4.f * m_fBounceHeight * fT * (1.f - fT);
		vCenter = XMVectorSetY(vCenter, XMVectorGetY(vCenter) + fBounceY);
	}

	_float3 vCenterF{};
	XMStoreFloat3(&vCenterF, vCenter);
	Set_CenterPosition(vCenterF);

	if (fT < 1.f)
		return;

	m_eBounceMode = BOUNCE_MODE::NONE;
	m_fElapsed = 0.f;

	if (BOUNCE_MODE::IMPACT_RECOIL == eMode)
	{
		m_pModelCom->Set_AnimationIndex(ETOUI(ANIM::OPEN), false);
		m_pModelCom->Play_Animation(0.f);
		return;
	}

	m_eState = BALL_STATE::DONE;
	OutputDebugStringW(L"[MonsterBall] MISS_BOUNCE -> DONE\n");
}

CMonsterBall* CMonsterBall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonsterBall* pInstance = new CMonsterBall(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMonsterBall");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMonsterBall::Clone(void* pArg)
{
	CMonsterBall* pInstance = new CMonsterBall(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMonsterBall");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonsterBall::Free()
{
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);

	__super::Free();
}
