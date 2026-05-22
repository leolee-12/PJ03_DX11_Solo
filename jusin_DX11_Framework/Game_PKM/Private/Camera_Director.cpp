#include "Camera_Director.h"
#include "Camera_Free.h"
#include "Camera_Sequence.h"
#include "Battle_Manager.h"
#include "Battle_ActionSequencer.h"

#include "Transform.h"

IMPLEMENT_SINGLETON(CCamera_Director)

namespace
{
	CAMERA_POSE Lerp_Pose(const CAMERA_POSE& a, const CAMERA_POSE& b, _float t)
	{
		CAMERA_POSE r;
		r.vPosition = _float3(
			a.vPosition.x + (b.vPosition.x - a.vPosition.x) * t,
			a.vPosition.y + (b.vPosition.y - a.vPosition.y) * t,
			a.vPosition.z + (b.vPosition.z - a.vPosition.z) * t);

		r.vLookAt = _float3(
			a.vLookAt.x + (b.vLookAt.x - a.vLookAt.x) * t,
			a.vLookAt.y + (b.vLookAt.y - a.vLookAt.y) * t,
			a.vLookAt.z + (b.vLookAt.z - a.vLookAt.z) * t);

		r.vUp = b.vUp;
		r.fFovY = b.fFovY;
		return r;
	}

	CCamera_Sequence* Build_Tackle_Physical_Sequence()
	{
		CCamera_Sequence* pSeq = CCamera_Sequence::Create();
		if (nullptr == pSeq)
			return nullptr;

		CAMERA_SHOT_DESC shot = {};

		shot.eType = CAMERA_SHOT_TYPE::CUT;
		shot.fDuration = 0.35f;
		shot.fBlendTime = 0.f;
		shot.eFollowTarget = CAMERA_TARGET_TYPE::ATTACKER;
		shot.eLookAtTarget = CAMERA_TARGET_TYPE::ATTACKER;
		shot.vPositionOffset = _float3(1.4f, 1.4f, -1.4f);
		shot.vLookAtOffset = _float3(0.0f, 0.5f, 0.0f);
		pSeq->Push_Shot(shot);

		shot.eType = CAMERA_SHOT_TYPE::BLEND_TO;
		shot.fDuration = 0.45f;
		shot.fBlendTime = 0.25f;
		shot.eFollowTarget = CAMERA_TARGET_TYPE::BATTLE_CENTER;
		shot.eLookAtTarget = CAMERA_TARGET_TYPE::DEFENDER;
		shot.vPositionOffset = _float3(-0.7f, 1.4f, -2.8f);
		shot.vLookAtOffset = _float3(0.0f, 0.5f, 0.0f);
		pSeq->Push_Shot(shot);

		shot.eType = CAMERA_SHOT_TYPE::BLEND_TO;
		shot.fDuration = 0.35f;
		shot.fBlendTime = 0.18f;
		shot.eFollowTarget = CAMERA_TARGET_TYPE::DEFENDER;
		shot.eLookAtTarget = CAMERA_TARGET_TYPE::DEFENDER;
		shot.vPositionOffset = _float3(0.35f, 1.05f, -1.75f);
		shot.vLookAtOffset = _float3(0.0f, 0.4f, 0.0f);
		pSeq->Push_Shot(shot);

		return pSeq;
	}

	CCamera_Sequence* Build_Ranged_Energy_Sequence()
	{
		CCamera_Sequence* pSeq = CCamera_Sequence::Create();
		if (nullptr == pSeq)
			return nullptr;

		CAMERA_SHOT_DESC shot = {};

		shot.eType = CAMERA_SHOT_TYPE::CUT;
		shot.fDuration = 0.25f;
		shot.fBlendTime = 0.f;
		shot.eFollowTarget = CAMERA_TARGET_TYPE::ATTACKER;
		shot.eLookAtTarget = CAMERA_TARGET_TYPE::DEFENDER;
		shot.vPositionOffset = _float3(-1.75f, 1.54f, -2.45f);
		shot.vLookAtOffset = _float3(0.0f, 0.5f, 0.0f);
		pSeq->Push_Shot(shot);

		shot = {};
		shot.eType = CAMERA_SHOT_TYPE::FOLLOW_LOOKAT;
		shot.fDuration = 0.65f;
		shot.fBlendTime = 0.25f;
		shot.eFollowTarget = CAMERA_TARGET_TYPE::BATTLE_CENTER;
		shot.eLookAtTarget = CAMERA_TARGET_TYPE::DEFENDER;
		shot.vPositionOffset = _float3(0.0f, 1.4f, -3.5f);
		shot.vLookAtOffset = _float3(0.0f, 0.5f, 0.0f);
		pSeq->Push_Shot(shot);

		shot = {};
		shot.eType = CAMERA_SHOT_TYPE::BLEND_TO;
		shot.fDuration = 0.35f;
		shot.fBlendTime = 0.18f;
		shot.eFollowTarget = CAMERA_TARGET_TYPE::DEFENDER;
		shot.eLookAtTarget = CAMERA_TARGET_TYPE::DEFENDER;
		shot.vPositionOffset = _float3(-0.84f, 1.12f, -1.54f);
		shot.vLookAtOffset = _float3(0.0f, 0.4f, 0.0f);
		pSeq->Push_Shot(shot);

		return pSeq;
	}

	CCamera_Sequence* Build_Area_Wide_Sequence()
	{
		CCamera_Sequence* pSeq = CCamera_Sequence::Create();
		if (nullptr == pSeq)
			return nullptr;

		CAMERA_SHOT_DESC shot = {};

		shot.eType = CAMERA_SHOT_TYPE::CUT;
		shot.fDuration = 0.3f;
		shot.fBlendTime = 0.f;
		shot.eFollowTarget = CAMERA_TARGET_TYPE::BATTLE_CENTER;
		shot.eLookAtTarget = CAMERA_TARGET_TYPE::BATTLE_CENTER;
		shot.vPositionOffset = _float3(0.0f, 3.36f, -4.34f);
		shot.vLookAtOffset = _float3(0.0f, 0.4f, 0.0f);
		pSeq->Push_Shot(shot);

		shot = {};
		shot.eType = CAMERA_SHOT_TYPE::FOLLOW_LOOKAT;
		shot.fDuration = 0.75f;
		shot.fBlendTime = 0.35f;
		shot.eFollowTarget = CAMERA_TARGET_TYPE::BATTLE_CENTER;
		shot.eLookAtTarget = CAMERA_TARGET_TYPE::BATTLE_CENTER;
		shot.vPositionOffset = _float3(0.0f, 3.92f, -4.9f);
		shot.vLookAtOffset = _float3(0.0f, 0.3f, 0.0f);
		pSeq->Push_Shot(shot);

		shot = {};
		shot.eType = CAMERA_SHOT_TYPE::BLEND_TO;
		shot.fDuration = 0.4f;
		shot.fBlendTime = 0.25f;
		shot.eFollowTarget = CAMERA_TARGET_TYPE::BATTLE_CENTER;
		shot.eLookAtTarget = CAMERA_TARGET_TYPE::BATTLE_CENTER;
		shot.vPositionOffset = _float3(-2.24f, 2.1f, -4.2f);
		shot.vLookAtOffset = _float3(0.0f, 0.4f, 0.0f);
		pSeq->Push_Shot(shot);

		return pSeq;
	}

	CCamera_Sequence* Build_Camera_Sequence(CAMERA_SEQUENCE_ID eID)
	{
		switch (eID)
		{
		case CAMERA_SEQUENCE_ID::TACKLE_PHYSICAL:
			return Build_Tackle_Physical_Sequence();

		case CAMERA_SEQUENCE_ID::RANGED_ENERGY:
			return Build_Ranged_Energy_Sequence();

		case CAMERA_SEQUENCE_ID::AREA_WIDE:
			return Build_Area_Wide_Sequence();

		case CAMERA_SEQUENCE_ID::NONE:
		case CAMERA_SEQUENCE_ID::HIT_ONLY:
		case CAMERA_SEQUENCE_ID::BUFF_SELF:
		default:
			return nullptr;
		}
	}
}

HRESULT CCamera_Director::Initialize()
{
	/* M1 시점엔 별도 초기화가 없다. JSON 프리셋 로드(M9) 도입 시 본 함수에서 처리. */
	return S_OK;
}

void CCamera_Director::Bind(CCamera_Free* pCamera, CBattle_Manager* pManager)
{
	m_pCamera = pCamera;          // weak
	m_pBattleManager = pManager;         // weak

	/* Bind 자체는 mode 를 변경하지 않는다. caller 가 Set_Mode 로 명시. */
}

void CCamera_Director::Unbind()
{
	Stop_Sequence();
	m_Shake.Stop();

	if (nullptr != m_pCamera)
		m_pCamera->Set_ControlEnabled(true);

	m_pCamera = nullptr;
	m_pBattleManager = nullptr;
	m_eMode = CAMERA_MODE::FIELD;
}

void CCamera_Director::Try_Unbind()
{
	if (nullptr != m_pInstance)
		m_pInstance->Unbind();
}

void CCamera_Director::Set_Mode(CAMERA_MODE eMode)
{
	m_eMode = eMode;

	if (nullptr == m_pCamera)
		return;

	switch (eMode)
	{
	case CAMERA_MODE::BATTLE_DEFAULT:
	case CAMERA_MODE::CINEMATIC:
		/* Director 가 단일 owner: 자유 입력 차단 + follow 해제. */
		m_pCamera->Set_ControlEnabled(false);
		m_pCamera->Set_Following(false);
		break;

	case CAMERA_MODE::FIELD:
	case CAMERA_MODE::DEBUG_FREE:
		m_pCamera->Set_ControlEnabled(true);
		/* follow 는 caller 의 직전 설정을 보존하기 위해 건드리지 않음. */
		break;

	default:
		break;
	}
}

void CCamera_Director::Set_Default_Battle_Pose(const CAMERA_POSE& pose)
{
	/* M2: Cut_To 로 위임. 결과적으로 m_DefaultBattlePose / m_CurrentPose / m_PreviousPose
	   모두 일관 갱신되고, BATTLE_DEFAULT 모드면 즉시 Apply 까지 처리된다. */
	Cut_To(pose);
}

void CCamera_Director::Cut_To(const CAMERA_POSE& target)
{
	m_DefaultBattlePose = target;
	Begin_Blend(target, 0.f);
}

void CCamera_Director::Blend_To(const CAMERA_POSE& target, _float fBlendTime)
{
	m_DefaultBattlePose = target;
	Begin_Blend(target, fBlendTime);
}

void CCamera_Director::Play_Sequence(CCamera_Sequence* pSeq)
{
	if (nullptr == pSeq)
		return;

	Stop_Sequence();

	m_pCurrentSequence = pSeq;
	Safe_AddRef(m_pCurrentSequence);
	m_eCurrentSequenceID = CAMERA_SEQUENCE_ID::NONE;

	Set_Mode(CAMERA_MODE::CINEMATIC);
}

_bool CCamera_Director::Play_Sequence(CAMERA_SEQUENCE_ID eID)
{
	CCamera_Sequence* pSeq = Build_Camera_Sequence(eID);
	if (nullptr == pSeq)
		return false;

	Play_Sequence(pSeq);
	m_eCurrentSequenceID = eID;
	Safe_Release(pSeq);

	return true;
}

_float CCamera_Director::Get_Sequence_Duration(CAMERA_SEQUENCE_ID eID) const
{
	CCamera_Sequence* pSeq = Build_Camera_Sequence(eID);
	if (nullptr == pSeq)
		return 0.f;

	const _float fDuration = pSeq->Get_Total_Duration();
	Safe_Release(pSeq);

	return fDuration;
}

void CCamera_Director::Return_To_BattleDefault(_float fBlendTime)
{
	Stop_Sequence();
	Set_Mode(CAMERA_MODE::BATTLE_DEFAULT);

	if (fBlendTime <= 0.f)
		Cut_To(m_DefaultBattlePose);
	else
		Blend_To(m_DefaultBattlePose, fBlendTime);
}

void CCamera_Director::Stop_Sequence()
{
	if (nullptr != m_pCurrentSequence)
	{
		Safe_Release(m_pCurrentSequence);
		m_pCurrentSequence = nullptr;
	}

	if (CAMERA_MODE::CINEMATIC == m_eMode)
		Set_Mode(CAMERA_MODE::BATTLE_DEFAULT);

	m_ActiveShotPose = {};
	m_eCurrentSequenceID = CAMERA_SEQUENCE_ID::NONE;
}

CAMERA_SHOT_TYPE CCamera_Director::Get_CurrentShotType() const
{
	if (nullptr == m_pCurrentSequence)
		return CAMERA_SHOT_TYPE::END;

	const CAMERA_SHOT_DESC* pShot = m_pCurrentSequence->Get_Current_Shot();
	return (nullptr != pShot) ? pShot->eType : CAMERA_SHOT_TYPE::END;
}

_float CCamera_Director::Get_CurrentShotElapsed() const
{
	if (nullptr == m_pCurrentSequence)
		return 0.f;

	return m_pCurrentSequence->Get_Elapsed_In_Shot();
}

_float CCamera_Director::Get_CurrentShotDuration() const
{
	if (nullptr == m_pCurrentSequence)
		return 0.f;

	const CAMERA_SHOT_DESC* pShot = m_pCurrentSequence->Get_Current_Shot();
	return (nullptr != pShot) ? pShot->fDuration : 0.f;
}

void CCamera_Director::Start_Shake(_float fPower, _float fFrequency, _float fDuration)
{
	m_Shake.Start(fPower, fFrequency, fDuration);
}

void CCamera_Director::Stop_Shake()
{
	m_Shake.Stop();
}

void CCamera_Director::Tick(_float fTimeDelta)
{
	if (nullptr == m_pCamera)
		return;

	CAMERA_POSE basePose = {};

	if (CAMERA_MODE::CINEMATIC == m_eMode && nullptr != m_pCurrentSequence)
	{
		m_pCurrentSequence->Tick(fTimeDelta);

		const CAMERA_SHOT_DESC* pShot = m_pCurrentSequence->Get_Current_Shot();
		if (nullptr == pShot)
		{
			Stop_Sequence();
			basePose = m_DefaultBattlePose;
		}
		else
		{
			const _bool bJustEntered =
				(m_pCurrentSequence->Get_Elapsed_In_Shot() <= fTimeDelta + FLT_EPSILON);

			if (bJustEntered)
			{
				m_ActiveShotPose =
					(CAMERA_SHOT_TYPE::RETURN_DEFAULT == pShot->eType)
					? m_DefaultBattlePose
					: Evaluate_Shot(*pShot);

				const _float fBlend =
					(CAMERA_SHOT_TYPE::CUT == pShot->eType) ? 0.f : pShot->fBlendTime;
				Begin_Blend(m_ActiveShotPose, fBlend);
			}

			if (CAMERA_SHOT_TYPE::FOLLOW_LOOKAT == pShot->eType)
				basePose = Evaluate_Shot(*pShot);
			else
				basePose = m_ActiveShotPose;
		}
	}
	else if (CAMERA_MODE::BATTLE_DEFAULT == m_eMode)
	{
		basePose = m_DefaultBattlePose;
	}
	else
	{
		return;
	}

	const CAMERA_POSE blendedPose = ApplyBlend(basePose, fTimeDelta);
	m_CurrentPose = blendedPose;   // §함정 1: Shake 적용 전 상태 보존

	CAMERA_POSE finalPose = blendedPose;
	const _float3 vShakeOffset = m_Shake.Evaluate(fTimeDelta);
	finalPose.vPosition = _float3(
		finalPose.vPosition.x + vShakeOffset.x,
		finalPose.vPosition.y + vShakeOffset.y,
		finalPose.vPosition.z + vShakeOffset.z);

	Apply_Pose_To_Camera(finalPose);

	if (nullptr != m_pCamera)
		m_pCamera->Flush_PipeLine();
}

void CCamera_Director::Begin_Blend(const CAMERA_POSE& target, _float fBlendTime)
{
	m_PreviousPose = m_CurrentPose;

	if (fBlendTime <= 0.f)
	{
		m_bBlending = false;
		m_fBlendElapsed = 0.f;
		m_fBlendDuration = 0.f;
		m_CurrentPose = target;

		if (nullptr != m_pCamera &&
			(CAMERA_MODE::BATTLE_DEFAULT == m_eMode ||
				CAMERA_MODE::CINEMATIC == m_eMode))
		{
			Apply_Pose_To_Camera(target);
		}
	}
	else
	{
		m_bBlending = true;
		m_fBlendElapsed = 0.f;
		m_fBlendDuration = fBlendTime;
	}
}

CAMERA_POSE CCamera_Director::ApplyBlend(const CAMERA_POSE& basePose, _float fTimeDelta)
{
	if (!m_bBlending)
		return basePose;

	m_fBlendElapsed += fTimeDelta;

	if (m_fBlendElapsed >= m_fBlendDuration || m_fBlendDuration <= 0.f)
	{
		/* 보간 종료. 다음 프레임부터는 basePose 그대로. */
		m_bBlending = false;
		m_fBlendElapsed = 0.f;
		m_fBlendDuration = 0.f;
		return basePose;
	}

	_float t = m_fBlendElapsed / m_fBlendDuration;
	t = t * t * (3.f - 2.f * t);   // smoothstep, 시작/끝 미분 0

	return Lerp_Pose(m_PreviousPose, basePose, t);
}

_float3 CCamera_Director::Resolve_Target_Pos(CAMERA_TARGET_TYPE eType) const
{
	if (nullptr == m_pBattleManager)
		return _float3();

	const CBattle_ActionSequencer* pSeq = m_pBattleManager->Get_Sequencer();
	const BATTLE_ACTION_DATA* pData = (nullptr != pSeq) ? &pSeq->Get_ActionData() : nullptr;

	switch (eType)
	{
	case CAMERA_TARGET_TYPE::ATTACKER:
		return (nullptr != pData)
			? m_pBattleManager->Get_PokemonPos(pData->iActorSide)
			: Resolve_Target_Pos(CAMERA_TARGET_TYPE::BATTLE_CENTER);

	case CAMERA_TARGET_TYPE::DEFENDER:
		return (nullptr != pData)
			? m_pBattleManager->Get_PokemonPos(pData->iTargetSide)
			: Resolve_Target_Pos(CAMERA_TARGET_TYPE::BATTLE_CENTER);

	case CAMERA_TARGET_TYPE::BATTLE_CENTER:
	{
		const _float3 p0 = m_pBattleManager->Get_PokemonPos(g_kBattleSide_Player);
		const _float3 p1 = m_pBattleManager->Get_PokemonPos(g_kBattleSide_Opponent);
		return _float3(
			(p0.x + p1.x) * 0.5f,
			(p0.y + p1.y) * 0.5f,
			(p0.z + p1.z) * 0.5f);
	}

	case CAMERA_TARGET_TYPE::PLAYER_POKEMON:
		return m_pBattleManager->Get_PokemonPos(g_kBattleSide_Player);

	case CAMERA_TARGET_TYPE::OPPONENT_POKEMON:
		return m_pBattleManager->Get_PokemonPos(g_kBattleSide_Opponent);

	case CAMERA_TARGET_TYPE::PLAYER_TRAINER:
		return m_pBattleManager->Get_TrainerPos(g_kBattleSide_Player);

	case CAMERA_TARGET_TYPE::OPPONENT_TRAINER:
		return m_pBattleManager->Get_TrainerPos(g_kBattleSide_Opponent);

	case CAMERA_TARGET_TYPE::ABSOLUTE_CAMERA:
	case CAMERA_TARGET_TYPE::NONE:
	default:
		return _float3();
	}
}

CAMERA_POSE CCamera_Director::Evaluate_Shot(const CAMERA_SHOT_DESC& shot) const
{
	CAMERA_POSE pose = {};
	pose.vUp = _float3(0.f, 1.f, 0.f);
	pose.fFovY = (shot.fStartFov > 0.f) ? shot.fStartFov : 0.f;

	const _float3 followBase = Resolve_Target_Pos(shot.eFollowTarget);
	const _float3 lookAtBase = Resolve_Target_Pos(shot.eLookAtTarget);

	pose.vPosition = _float3(
		followBase.x + shot.vPositionOffset.x,
		followBase.y + shot.vPositionOffset.y,
		followBase.z + shot.vPositionOffset.z);

	pose.vLookAt = _float3(
		lookAtBase.x + shot.vLookAtOffset.x,
		lookAtBase.y + shot.vLookAtOffset.y,
		lookAtBase.z + shot.vLookAtOffset.z);

	return pose;
}

void CCamera_Director::Apply_Pose_To_Camera(const CAMERA_POSE& pose)
{
	CTransform* pTr = m_pCamera->Get_Transform();
	if (nullptr == pTr)
		return;

	XMVECTOR vPos = XMVectorSet(pose.vPosition.x, pose.vPosition.y, pose.vPosition.z, 1.f);
	XMVECTOR vLookAtPt = XMVectorSet(pose.vLookAt.x, pose.vLookAt.y, pose.vLookAt.z, 1.f);
	XMVECTOR vDir = XMVectorSubtract(vLookAtPt, vPos);
	_float   fDirLen = XMVectorGetX(XMVector3Length(vDir));
	if (fDirLen > 1e-4f)
	{
		XMVECTOR vDirN = XMVector3Normalize(vDir);
		_float   fDot = fabsf(XMVectorGetX(
			XMVector3Dot(vDirN, XMVectorSet(0.f, 1.f, 0.f, 0.f))));
		if (fDot > 0.999f)
			vLookAtPt = XMVectorAdd(vLookAtPt, XMVectorSet(0.001f, 0.f, 0.f, 0.f));
	}

	pTr->Set_State(STATE::POSITION, vPos);
	pTr->LookAt(vLookAtPt);
}

void CCamera_Director::Free()
{
	Stop_Sequence();
	m_pCamera = nullptr;
	m_pBattleManager = nullptr;
}