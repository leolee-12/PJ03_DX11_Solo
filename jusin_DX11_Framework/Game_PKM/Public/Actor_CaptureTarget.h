#pragma once
#include "Battle_AnimDef.h"
#include "Actor.h"
#include "Body.h"

NS_BEGIN(Engine)
class CCollider;
NS_END

NS_BEGIN(Game_PKM)
class CInteraction_BallHit;

/* 캡처 타깃 종별 이동 패턴. 콜라이더가 타깃 위치를 따라가므로 조준 난이도를 만든다. */
enum class CAPTURE_MOVE_TYPE { STAY, RUN_TURN, BAT, END };

class CActor_CaptureTarget final : public CActor
{
public:
	struct ACTOR_CAPTURE_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		WNameID				strBodyProtoTag = { 0 };
		CBody::BODY_DESC*	pBodyDesc = { nullptr };
		_uint				iBodyProtoLevel = ETOUI(LEVEL::STATIC);
		_uint				iComponentLevel = ETOUI(LEVEL::GAMEPLAY);

		_uint				iSpeciesID = { 0 };   // ǥ�á������
		_uint				iLevel = { 1 };
		_uint				iInitialBallItemID = { 0 };
		_bool				bCaughtBefore = { false };
	};

private:
	CActor_CaptureTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CActor_CaptureTarget(const CActor_CaptureTarget& Prototype);
	virtual ~CActor_CaptureTarget() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual _string Get_TypeName() const override { return "CaptureTargetActor"; }
	_uint Get_SpeciesID() const { return m_iSpeciesID; }
	_uint Get_Level() const { return m_iLevel; }
	_bool Is_CaughtBefore() const { return m_bCaughtBefore; }
	CCollider* Get_Collider() const { return m_pColliderCom; }
	_float  Get_CaptureRadius() const { return m_fCaptureRadius; }
	_float3 Get_CaptureCenter() const;
	_float3 Get_EffectPivot() const;

	void    Begin_Absorb();
	void    Begin_Appear();
	_bool   Is_Absorbing() const { return m_bAbsorbing; }

	void    Set_MoveActive(_bool bActive) { m_bMoveActive = bActive; }   // 미니게임 이동 on/off
	void    Reset_Move();   // 이동 누적시간 초기화 + 홈(스폰) 위치로 스냅

	void    Play_IdleAnim();
	void    Play_AppearAnim();

private:
	CInteraction_BallHit* m_pBallHit = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

	_uint m_iSpeciesID = { 0 };
	_uint m_iLevel = { 1 };
	_bool m_bCaughtBefore = { false };

	WNameID   m_strModelTag = {};
	ANIM_KIND m_eAnimKind = { ANIM_KIND::IDLE };
	_float    m_fAnimTimer = { 0.f };
	_float    m_fAnimDuration = { 0.f };

	_float  m_fCaptureRadius = { 0.6f };
	_float3 m_vCaptureCenter = { 0.f, 0.5f, 0.f };

	_bool   m_bAbsorbing = { false };
	_bool   m_bAbsorbReverse = { false };
	_float  m_fAbsorbElapsed = { 0.f };
	_float  m_fAbsorbDuration = { 0.28f };

	_bool   m_bBasisCached = { false };
	_float3 m_vRightUnit = { 1.f, 0.f, 0.f };
	_float3 m_vUpUnit = { 0.f, 1.f, 0.f };
	_float3 m_vLookUnit = { 0.f, 0.f, 1.f };

	CAPTURE_MOVE_TYPE m_eMoveType = { CAPTURE_MOVE_TYPE::STAY };
	_bool   m_bMoveActive = { true };
	_float  m_fMoveElapsed = { 0.f };
	_float3 m_vHomePos = {};
	_int    m_iRunDir = { 1 };   // RUN_TURN: +1 우측, -1 좌측

private:
	HRESULT Ready_Components(const ACTOR_CAPTURE_DESC* pDesc);
	HRESULT Ready_PartObjects(const ACTOR_CAPTURE_DESC* pDesc);
	void    Cache_Members();
	void    Cache_BasisIfNeeded();

	virtual void Tick_Movement(_float fTimeDelta) override;   // CActor::Update 가 파트 애님 갱신 후 호출(루트모션 델타 준비됨)
	void    Tick_RunTurn(_float fTimeDelta);                  // RUN_TURN: 달리기(루트모션) + 끝점 턴
	_float3 Compute_MoveOffset(_float fElapsed) const;        // BAT: 위치 오프셋

public:
	static CActor_CaptureTarget* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

private:
	virtual void Free() override;
};

NS_END
