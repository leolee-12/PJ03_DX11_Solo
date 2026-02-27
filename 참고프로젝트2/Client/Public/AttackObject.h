#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

#include "AnimationEvent_Defines.h"


BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CAttackObject  : public CGameObject
{
public:
	//enum HitMotion { HIT_LIGHT, HIT_MEDIUM, HIT_HEAVY, HIT_CROUCH_MEDIUM, HIT_KNOCK_AWAY_LEFT, HIT_KNOCK_AWAY_UP };

	typedef struct ATTACK_DESC
	{
		//CCollider::COLLIDER_DESC  ColliderDesc{};
		CBounding_AABB::BOUNDING_AABB_DESC ColliderDesc{};

		HitMotion ihitCharacter_Motion = { HIT_LIGHT };

		AttackGrade  eAttackGrade = { GRADE_NORMAL };
		AttackType   eAttackType = { ATTACKTYPE_HIGH};


		//잡기의 경우, 성공시 진행될 모션 
		_bool		bOwnerNextAnimation = { false };
		_uint		iOnwerNextAnimationIndex = {};


		_float2 fhitCharacter_Impus = {0,0};
		_float  fhitCharacter_StunTime = { 1.f };
		_float  fLifeTime = { 0.f };
		_ushort iTeam = { 1 };

		_uint	iDamage = { 1 };
		_float	fAnimationLockTime = { 0.1f };

		//_bool   bOwnerGravityTimeReset = { false };
		_bool		bGroundSmash = { false};
		//_bool		bGainAttackStep = { true };
		_ushort		iGainAttackStep = { 1 };
		_ushort		iGainHitCount = { 1 };


		_float		fForcedGravityTime = {100}; //무시할 기본 값. 0은 쓸 수도 있어서 100으로 함

		_bool		bGrabbedEnd = { false };
		_bool		bCameraZoom = { true };

		_ushort		iGainKiAmount = { 5 };

		//카메라용
		_uint iVirtualCameraindex = { 200 }; //200 : 미사용(기본값)
		_uint ianimationIndex = {};
		_float fCameraShakeDuration = { 0 };
		_float fCameraShakeMagnitude = { 0 };

		_bool bDrawNoneStop = { false };

		_bool bNoCreateMainCollider = { false }; //에너지파 전용

		_bool bReflect = { false };
		_bool bOnwerHitNoneStop = { false };
		_bool bHitNoGravity = { false };
		_float fMaxNoNoGravitySafeTime = { 0.2f };

		_short iOnwerDirection = { 231 };  //보통 보는 방향의 반대가 되도록 조정하지만  0으로 넣으면 뒤통수 때려도 뒤집히지 않음. 기본값(231)이면 자동처리하기


		_ushort isfxSoundIndex = { 60000 };
		_float fsfxVolume = { 1.f };

		
		_wstring strHitEffectName = {};
		_float2 fHitEffectOffset = {};
		_bool bHitEffectFlip = { false };

		_ushort iCallAttackBackIndex = { 60000 };
		_float3 fParticleOffset = {};

		_ushort iParticleIndex = { 60000 };


		class CCharacter* pOwner = { nullptr };
	};
protected:
	CAttackObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAttackObject(const CAttackObject& Prototype);
	virtual ~CAttackObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Camera_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;


	class CCharacter* Get_pOwner() { return m_pOwner; };
	AttackType		Get_AttackType() { return m_eAttackType; };
	
	void Set_AttackBackEvent();
	void Set_UpdateStop(_float fStopTime);
	_bool Get_bReflect();
public:
	void Set_RemoteDestory();


public:
	virtual void OnCollisionEnter(class CCollider* other, _float fTimeDelta) override;
	virtual void OnCollisionStay(class CCollider* other, _float fTimeDelta) override;
	virtual void OnCollisionExit(class CCollider* other) override;


protected:
	_bool Check_UpdateStop(_float fTimeDelta);

private:
	void CollisingAttack();
	void CollisingPlayer();

	//카메라
private:
	//강공격 카메라 함수 호출
	void Camera_Hit_Knock_Away_Left(class CCharacter* pOwner, class CCharacter* pHitOwner);
	void Camera_Hit_Knock_Away_Up(class CCharacter* pOwner, class CCharacter* pHitOwner);
	void Camera_GroundSmash(class CCharacter* pOwner, class CCharacter* pHitOwner);
protected:
	//CCollider_Test*			m_pColliderCom = { nullptr };

	HitMotion				m_ihitCharacter_Motion = { HIT_LIGHT };
	AttackGrade				m_eAttackGrade = { GRADE_NORMAL };
	AttackType				m_eAttackType = { ATTACKTYPE_HIGH };

	_float2					m_fhitCharacter_Impus{};
	_float					m_fhitCharacter_StunTime = { 1.f };
	_float					m_fLifeTime = { 0.f };
	_ushort					m_iTeam = { 1 };
	_ushort					m_iDamage = { 1 };

	_float					m_fAccLifeTime = { 0.f };

	_float3					m_fOffset{};

	_float					m_fAnimationLockTime = { 0.1f };

	//_bool					m_bOwnerGravityTimeReset = false;
	
	//1020추가
	_bool					m_bGroundSmash = { false };
	//_bool					m_bGain_AttackStep = { true };
	_ushort					m_iGainAttackStep = { 1 };
	_ushort					m_iGainHitCount = { 1 };

	class CCharacter*				m_pOwner = { nullptr };

	CTransform*				m_pOwnerTransform = { nullptr };

	CCollider* m_pColliderCom = { nullptr };
	_bool		m_bOwnerNextAnimation = { false };
	_uint		m_iOnwerNextAnimationIndex = {};

	_float		m_fForcedGravityTime = { 100 };  //무시할 기본 값. 0은 쓸 수도 있어서 100으로 함

	_bool		m_bEnableDestory = true;
	_bool		m_bGrabbedEnd = { false };
	_bool		m_bCameraZoom = { true };

	_float m_fCameraShakeDuration = {};
	_float m_fCameraShakeMagnitude = {};

	_ushort		m_iGainKiAmount = { 5 };


	_float		m_fAccUpdateStop = {};
	_float		m_fMaxUpdateStop = {};
	_bool		m_bUpdateStop = { false };

	_bool	m_bDrawNoneStop = { false };
	_bool	m_bReflect = { false };
	_bool	m_bOnwerHitNoneStop = { false };
	_bool	m_bHitNoGravity = { false };
	_float	m_fMaxNoNoGravitySafeTime = { 0.2f };

	_uint m_iVirtualCameraindex = { 200 };
	_uint m_ianimationIndex = {};


	_short m_iOnwerDirection = { 1 };


	_ushort m_isfxSoundIndex = { 1000 };
	_float  m_fsfxVolume = { 1.f };

	_wstring m_strHitEffectName = {};
	_float2 m_fHitEffectOffset = {};
	_bool	m_bHitEffectFlip = { false };

	_ushort m_iCallAttackBackIndex = { 60000 };

	_ushort m_iParticleIndex = { 60000 };
	_float3 m_fParticleOffset = {};

private:
	HRESULT Ready_Components(ATTACK_DESC* pDesc);

public:
	static CAttackObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END