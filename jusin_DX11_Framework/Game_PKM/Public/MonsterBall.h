#pragma once
#include "PartObject.h"
#include "Game_PKM_Defines.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
NS_END

NS_BEGIN(Game_PKM)

/* -------------------------------------------------- */
// CMonsterBall : Capture 레벨에서 던지는 몬스터볼
//  - READY -> FLYING -> IMPACT -> DONE 상태 머신
//  - FLYING 시 포물선 운동 (XZ 선형, Y 선형 + arc)
//  - IMPACT 시 충돌 방향 반대로 짧게 이동한 뒤 OPEN -> CLOSE 애니메이션 재생
/* -------------------------------------------------- */

class CMonsterBall final : public CPartObject
{
public:
	enum class ANIM : _uint
	{
		BATTLE_OPEN = 1u,

		CAPTURE_IDLE = 30u,

		CLOSE = 28u,
		OPEN = 44u
	};

	enum class BALL_STATE : _ubyte { READY, FLYING, IMPACT, STAGE_DROP, STAGE_SHAKE, BATTLE_SENDOUT, DONE, END };
	enum class BOUNCE_MODE : _ubyte { NONE, IMPACT_RECOIL, MISS_GROUND };

	struct MONSTER_BALL_DESC : public CPartObject::PARTOBJECT_DESC
	{
		_float3 vTargetPos = { 0.f, 0.f, 0.f };       // 카메라 vAt = CaptureTarget 위치
		_float  fFlightDuration = 1.0f;                    // 비행 시간(초)
		_float  fArcHeight = 2.0f;                    // 포물선 최고점 추가 높이
		_float  fImpactDuration = 0.5f;                    // IMPACT 정지 시간(초)
	};

private:
	CMonsterBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonsterBall(const CMonsterBall& Prototype);
	virtual ~CMonsterBall() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Priority_Update(_float fTimeDelta) override;
	virtual void    Update(_float fTimeDelta) override;
	virtual void    Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

	virtual _string Get_TypeName() const override { return "MonsterBall"; }
	BALL_STATE Get_State() const { return m_eState; }
	_bool      Is_Done()   const { return BALL_STATE::DONE == m_eState; }
	_bool      Is_OpenFinished() const { return m_bOpenFinished; }
	CCollider* Get_Collider() const { return m_pColliderCom; }
	const _float3& Get_CenterPosition() const { return m_vCenterPos; }
	void       Set_AimPose(const _float3& vStartPos, const _float3& vTargetPos);
	void       Play_BattleOpen(const _float3& vCenterPos, const _float3& vFaceTarget);

	void       Launch();   // READY 상태에서만 동작 -> FLYING 전이 + 자동 Show
	void       Reset();    // 강제로 READY 로 복귀 - 재던지기 진입 시 호출 (가시성 건드리지 않음)
	void       Hide();     // 외부 가시성 false - Late_Update 의 RenderGroup 등록 차단
	void       Show();     // 외부 가시성 true

	void	Trigger_Impact(const _float3& vTargetCenter);   // FLYING 중 외부 충돌 신호로 IMPACT 전이
	void	Begin_StageDrop(const _float3& vAirCenter, const _float3& vGroundCenter, const _float3& vFaceTarget, _float fDuration);
	_bool	Is_DropDone() const { return m_bStageDropFinished; }
	void	Begin_OneShake(_float fDuration, _float fAngleDeg);
	_bool	Is_ShakeDone() const { return m_bShakeFinished; }


private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

	BALL_STATE m_eState = { BALL_STATE::READY };
	_float     m_fElapsed = { 0.f };

	_float3 m_vStartPos = { -1.3f, 1.5f, -7.3f };
	_float3 m_vTargetPos = { 0.f, 0.f, 0.f };
	_float3 m_vCenterPos = {};
	_float3 m_vLocalCenter = { 0.f, 0.2f, 0.f };
	_float  m_fFlightDuration = 1.0f;
	_float  m_fArcHeight = 2.0f;
	_float  m_fImpactDuration = 0.5f;
	_bool   m_bVisible = { true };
	_bool   m_bWaitCloseAfterOpen = { false };
	_bool   m_bOpenFinished = { false };

	_float3     m_vBounceStartCenter = {};
	_float3     m_vBounceEndCenter = {};
	_float      m_fBounceTime = 0.f;
	_float      m_fBounceDuration = 0.f;
	_float      m_fBounceHeight = 0.f;
	BOUNCE_MODE m_eBounceMode = BOUNCE_MODE::NONE;

	_float3     m_vStageDropStartCenter = {};
	_float3     m_vStageDropEndCenter = {};
	_float      m_fStageDropTime = 0.f;
	_float      m_fStageDropDuration = 0.f;
	_bool       m_bStageDropFinished = false;

	_float3     m_vShakeCenter = {};
	_float3     m_vShakePivotPos = {};
	_float      m_fShakeTime = 0.f;
	_float      m_fShakeDuration = 0.f;
	_float      m_fShakeAngleRad = 0.f;
	_bool       m_bShakeFinished = false;

	_uint m_iDummy = { 0u };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	void    Update_Ready(_float fTimeDelta);
	void    Update_Flying(_float fTimeDelta);
	void    Update_Impact(_float fTimeDelta);
	void    Update_StageDrop(_float fTimeDelta);
	void    Update_StageShake(_float fTimeDelta);
	void    Update_BattleSendOut(_float fTimeDelta);
	void    Update_Done(_float fTimeDelta);

	void    Update_Position();    // 현재 t 기반 포물선 좌표 -> Transform 반영
	void    Update_Collider();
	void    Set_CenterPosition(const _float3& vCenterPos);
	void    Face_CenterTo(const _float3& vCenterPos, const _float3& vTargetCenter);
	void Face_CenterToYaw(const _float3& vCenterPos, const _float3& vTargetCenter);
	void Reset_UprightBasisFromCurrentYaw();
	void    Reset_UprightBasis();
	void    Apply_ShakeRoll(_float fRollRad);

	void    Begin_Bounce(BOUNCE_MODE eMode, const _float3& vStartCenter, const _float3& vEndCenter, _float fDuration, _float fHeight);
	void    Begin_MissBounce();
	void    Update_Bounce(_float fTimeDelta);

public:
	static CMonsterBall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

private:
	virtual void Free() override;
};

NS_END
