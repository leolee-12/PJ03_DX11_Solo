#pragma once

#include "GameObject.h"
#include "Client_Defines.h"
#include "UI_Define.h"
#include "AnimationEvent_Defines.h"
#include "FrameEvent_Manager.h"

#include "Input.h"
#include <functional>

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)



class CCharacter : public CGameObject
{
public:

	static vector<CInput> Command_236Attack;
	static vector<CInput> Command_236Attack_Extra;

	static vector<CInput> Command_236Attack_Heavy;
	static vector<CInput> Command_236Attack_Heavy_Extra;

	static vector<CInput> Command_214Attack;
	static vector<CInput> Command_214Attack_Extra;
	static vector<CInput> Command_236Special;
	static vector<CInput> Command_236Special_Side;
	static vector<CInput> Command_236Special_Side_Extra;
	static vector<CInput> Command_214Special;
	static vector<CInput> Command_214Special_Extra;


	static vector<CInput> Command_214FinalAttack;
	static vector<CInput> Command_236UltimateAttack;
	static vector<CInput> Command_236UltimateAttack_Side;
	static vector<CInput> Command_236UltimateAttack_Side_Extra;


	static vector<CInput> Command_BackDash;
	static vector<CInput> Command_Forward;

	static vector<CInput> Command_LightAttack;
	static vector<CInput> Command_MediumAttack;
	static vector<CInput> Command_MediumAttack_Extra;

	static vector<CInput> Command_HeavyAttack;
	static vector<CInput> Command_SpecialAttack;
	static vector<CInput> Command_HeavyAttack_Extra;

	static vector<CInput> Command_Grab;


	static vector<CInput> Command_Crouch_LightAttack;
	static vector<CInput> Command_Crouch_MediumAttack;
	static vector<CInput> Command_Crouch_MediumAttack_Extra;
	static vector<CInput> Command_Crouch_HeavyAttack;
	static vector<CInput> Command_Crouch_HeavyAttack_Extra;
	static vector<CInput> Command_Crouch_SpecialAttack;
	static vector<CInput> Command_Crouch_Crouch_SpecialAttack;


	static vector<CInput> Command_Up_SpecialAttack;


	static vector<CInput> Command_Reflect;
	static vector<CInput> Command_BenishingAttack;
	static vector<CInput> Command_LowBenishingAttack;

	static vector<CInput> Command_Transform;

public:
	const int BUFFER_SIZE = 30;
	//enum AttackGrade {Attack_light =0, Attack_Medium, Attack_Heavy=2, Attack_Special=2, Attack_Command, Attack_Skill, Attack_FinalSkill};
	//enum HitMotion { HIT_LIGHT, HIT_MEDIUM, HIT_HEAVY, HIT_CROUCH_MEDIUM, HIT_KNOCK_AWAY_LEFT, HIT_KNOCK_AWAY_UP };
	static const _float fGroundHeight;  //0
	static const _float fJumpPower;
	//const enum LOOK{ LOOK_LEFT = -1,  LOOK_RIGHT = 1};

	typedef struct : CGameObject::GAMEOBJECT_DESC
	{
		//_wstring strModelName;
		_ushort iTeam = 1;
		CUI_Define::PLAYER_SLOT ePlayerSlot = {};

	}Character_DESC;

	struct CommandPattern {
		vector<CInput> pattern;  // 입력 패턴
		//std::function<void()> action;  // 해당 패턴이 성공했을 때 실행할 함수
		_uint AnimationIndex;
	};

	struct CommandPatternFunction {
		vector<CInput> pattern;  // 입력 패턴
		std::function<void()> action;  // 해당 패턴이 성공했을 때 실행할 함수
		//_uint AnimationIndex;
	};

protected:
	CCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCharacter(const CCharacter& Prototype);
	virtual ~CCharacter() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Player_Update(_float fTimeDelta) override;
	virtual void Camera_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;


	//커맨드 입력
	//virtual void InputCommand();
	virtual _bool InputCommand();
	virtual void InputedCommandUpdate(_float fTimeDelta);
	virtual void UpdateInputBuffer(CInput newInput)
	{
		if (inputBuffer.size() >= BUFFER_SIZE) {
			inputBuffer.erase(inputBuffer.begin());  // 오래된 입력 삭제
		}
		inputBuffer.push_back(newInput);
	}
	virtual bool CheckCommandSkippingExtras(const vector<CInput>& pattern, int timeWindow);
	bool CheckCommand_Exactly(const std::vector<CInput>& pattern, int timeWindow);
	_uint CheckAllCommands();


	//애니메이션
	virtual _bool Character_Play_Animation(_float fTimeDelta);
	virtual _bool Character_Play_Animation_NoXZ(_float fTimeDelta);

	virtual _bool Check_bCurAnimationisGroundMove(_uint iAnimation = 1000) { return false; };
	virtual _bool Check_bCurAnimationisAirMove(_uint iAnimation = 1000);
	virtual _bool Check_bCurAnimationisAttack(_uint iAnimation = 1000) { return false; };
	virtual _bool Check_bCurAnimationisAirAttack(_uint iAnimation = 1000) { return false; };
	virtual _bool Check_bCurAnimationisAirHit(_uint iAnimation = 1000);
	virtual _bool Check_bCurAnimationisHitAway(_uint iAnimation = 1000);
	virtual _bool Check_bCurAnimationisHitGround(_uint iAnimation = 1000);

	virtual _bool Check_bCurAnimationisChase(_uint iAnimation = 1000);  //특이한 경우라 애니메이션 뿐 만 아니라 m_bChase까지 끼워넣음
	virtual _bool Check_bCurAnimationisReflect(_uint iAnimation = 1000);

	virtual _bool Check_bCurAnimationisGuard(_uint iAnimation = 1000);
	virtual _bool Check_bCurAnimationisGrab(_uint iAnimation = 1000);

	virtual _bool Check_bCurAnimationisGroundSmash(_uint iAnimation = 1000);

	virtual _short Check_bCurAnimationisCanChase() { return 0; };  //현재 모션이 체이스로 연계 가능한지 여부를 체크, 0이면 불가능 그 외의 숫자는 시작속도*10니까 받아서 *0.01f할것


	//공중 기탄같이 중간에 중력이 다시 생기는 특수 처리해야하는것들 각자 override
	virtual _bool Check_bCurAnimationisHalfGravityStop(_uint iAnimation = 1000) { return false; };

	void Set_NextAnimation(_uint iAnimationIndex, _float fLifeTime, _float fAnimationPosition = 0);
	//void Set_NextAnimation(_uint iAnimationIndex, _float fLifeTime);
	virtual void AttackNextMoveCheck();
	virtual void AnimeEndNextMoveCheck();
	//virtual void Set_Animation(_uint iAnimationIndex) {};
	virtual void Set_Animation(_uint iAnimationIndex, _bool bloof = false);
	//_short Get_iIdleAnimation();
	void Set_IdleAnimation();

	_bool		CompareNextAnimation(_uint iAnimationIndex, _float fNextPosition = 0);

	virtual void Set_CurrentAnimationPositionJump(_float fAnimationPosition);

	//키프레임 이벤트
	virtual void ProcessEventsFramesZero(_uint characterIndex, _uint animationIndex);
	virtual void ProcessEventsBetweenFrames2(int characterIndex, int animationIndex, float prevFrame, float currentFrame);

	//키프레임 이벤트 파생
	void Create_Effect(_int iEffectIndex);



	void FlipDirection(_int iDirection = 0);
	_int Get_iDirection() { return m_iLookDirection; };
	_uint* Get_pAnimationIndex();
	_short Get_iAnimationIndex();


	_uint Get_iPlayerTeam() { return m_iPlayerTeam; };

	//virtual void AttackEvent(_int iAttackEventEnum, _int AddEvent = 0) {};
	virtual void AttackEvent(_int iAttackEvent, _int AddEvent = 0) {};
	virtual void Play_Sound(_uint SoundName, _bool bisLoof, _float fvolume) {};
	virtual void Play_Sound_Stop(_uint SoundName) {};
	virtual void Play_Group_Sound(_uint groupKey, _bool loop, _float volume) {};




	//디버그용 코드 
	virtual void ShowInputBuffer();
	virtual void DebugPositionReset(_short iDirection = 0);



	//중력관련
	//virtual void Gravity(_float fTimeDelta);
	virtual void Gravity(_float fTimeDelta);

	virtual void Set_fJumpPower(_float fJumpPower) { m_fJumpPower = fJumpPower; };
	virtual void Set_fGravityTime(_float fGravityTime) { m_fGravityTime = fGravityTime; };
	//virtual void Set_fImpulse(_float fImpulse) { m_fImpuse = fImpulse; };
	virtual void Set_fImpulse(_float2 fImpulse) { m_fImpuse = fImpulse; };
	virtual void Set_fImpulse(_float fImpulseX) { m_fImpuse.x = fImpulseX; };
	//virtual void Set_fImpulse(_float fImpulseX, _float fImpusY=0) { m_fImpuse.x = fImpulseX;   m_fImpuseImpus.y=fImpusY };  //반드시 0으로 초기화되는거니 패스


	_float Get_fHeight();
	_bool Get_bAirDashEnable() { return m_bAriDashEnable; };
	void Set_bAirDashEnable(_bool bAirDashEnable) { m_bAriDashEnable = bAirDashEnable; };

	CCharacter* Get_pEnemy();

	void Set_ForcedGravityDown();
	void Set_ForcedGravityTime_LittleUp();

	void Set_ForcveGravityTime(_float fGravityTime);

	void Set_bNextAnimationGravityEvent() { m_bNextAnimationGravityEvent = true; };

	void Set_bAttackGravity(_bool bAttackGravity);
	_float Get_fGravityTime() { return m_fGravityTime; };



	void AttckCancleJump();

	//void Chase(_float fTimeDelta);
	void Chase2(_float fTimeDelta);
	//void Chase_Ready(_float fTimeDelta);
	void Chase_Ready(_float fTimeDelta, _bool bNoReady = false);

	void Set_ChaseStoping();
	void Set_ChaseStop();


	void Chase_Grab(_float fTimeDelta);
	void Character_Attack_Grab(_float fTimeDelta);
	void Grab_LateDraw();


	void Move(_float fTimeDelta);
	void MoveKey1Team(_float fTimeDelta);
	void MoveKey2Team(_float fTimeDelta);
	virtual void Reset_AttackCount();


	void Set_bInivisible(_bool bInvisible);

	//피격 관련
	//AttackColliderResult Set_Hit3(_uint eAnimation, AttackGrade eAttackGrade, AttackType eAttackType, _float fStunTime, _uint iDamage, _float fStopTime, _float2 Impus = { 0,0 });
	//virtual AttackColliderResult Set_Hit4(_uint eAnimation, AttackGrade eAttackGrade, AttackType eAttackType, _float fStunTime, _uint iDamage, _float fStopTime, _short iDirection, _float2 Impus = { 0,0 });
	virtual AttackColliderResult Set_Hit4(_uint eAnimation, AttackGrade eAttackGrade, AttackType eAttackType, _float fStunTime, _uint iDamage, _float fStopTime, _short iDirection, _float2 Impus = { 0,0 }, _bool bParticle =true);


	//void Set_HitAnimation(_uint eAnimation, _float2 Impus = { 0,0 });
	void Set_HitAnimation(_uint eAnimation, _float2 Impus = { 0,0 },_bool bParticle=true);

	void Set_AnimationStop(_float fStopTime);
	void Set_UnlockAnimationStop();
	void Set_AnimationStopWithoutMe(_float fStopTime);

	void Check_StunEnd();
	void Stun_Shake();
	void Update_AnimationLock(_float fTimeDelta);
	void Update_StunImpus(_float fTimeDelta);
	virtual void Set_BreakFall_Ground();
	void BreakFall_Air();

	void Set_bNoGravity(_bool bNoGravity, _float MaxfNoGravitySafeTime = 0.2f);


	_uint Get_iHP();

	//공격 관련
	void Gain_AttackStep(_ushort iStep);// 
	void Gain_HitCount(_ushort iHit);// 

	//_float Get_DamageScale();
	virtual _float Get_DamageScale(_bool bUltimate = false);

	void Set_GrabLoofCount(_ushort iLoofCount);

	void Set_GroundSmash(_bool bSmash);
	//void Guard_Update();   //서브캐릭터용도로 써야하나?
	//_bool Guard_Check();
	//_bool Guard_Check2(AttackType eAttackType);
	AttackColliderResult Guard_Check3(AttackType eAttackType);
	_bool CompareGuardType(AttackType eAttackType);
	AttackColliderResult CompareGuardType3(AttackType eAttackType);
	AttackColliderResult CompareGrabType3(AttackType eAttackType);


	void Teleport_ToEnemy(_float OffsetX, _float OffsetY);

	//void Set_Grab(_bool bGrab, _bool bAir);
	void Set_Grab(_bool bAir);


	_ushort Get_BreakFall_AirAnimationIndex();
	_ushort Get_JumpAirAnimationIndex();


	_bool Get_bStun();
	void Update_PreviousXPosition();
	_float Get_fCalculatePreviousXPosition();
	_float Get_fAbsCalculatePreviousXPosition();
	_float Get_fPositionX();
	_vector Get_vPosition();

	void Set_bGrabbed(_bool bGrabbed);
	void Set_bGrabbedGravity(_bool bGrabbedGravity);

	_bool Get_bGrabbed();
	void Set_GrabAnimation();  //외부에서 호출해야하는데 각자 다르므로?


	void Set_bBenishingAttack(_bool bBenishing);
	_bool Get_bBenishingAttack();

	void Add_Move(_float2 fMovement);


	//BattleInterface
	void Sparking_ON(_float fTimeDelta);
	void Sparking_TimeCount(_float fTimeDelta);
	_bool Get_bSparking();

	void Gain_KiAmount(_ushort iKiAmount);

	_bool Get_bCharacterDead();

	void Tag_KeyCheck();
	void Tag_In(_ubyte iTagSlot);
	void RegisterEnemy(CCharacter* pEnemy);
	void pEnemyCheck();
	void Tag_Out(_vector vPosition);

	_bool Update_Tag_In(_float fTimeDelta);
	_uint Get_NewCharacterslot();

	void Set_AttackBackEvent(_bool bEvent);
	void Set_ReflectAttackBackEvent(_bool bEvent);

	_bool Get_bAttackBackEvent();

	void Set_bGrabDraw(_bool bGrabDraw);
	_bool Check_bWall();
	void Move_ForWall();

	void Set_bDynamicMove(_bool bDynamicMove);


	void Update_Dying(_float fTimeDelta);
	_bool Get_bDying();
	void Set_FinalSkillRoundEnd(_bool bSkillRoundEnd, _ushort iIndex);
	_bool m_bFinalSkillRoundEnd = false;
	_bool m_bFinalSkillRoundEndSolo = false;
	_float m_fMaxDyingTime = { 5.f };

	void Set_UnDying(_bool bNoneDying) {m_bUnDying = bNoneDying;};
	_bool m_bUnDying = false;

	void Play_WinAnimation();
	void Play_NewRound_Loser();
	void Play_NewRound_Winner();
	void Update_Collider();


	_bool Play_FirstOpening();

	void Update_Opening(_float fTimeDelta);
	_bool m_bOpening = false;
	_float m_fAccOpeningTime = {0.f};
	_float m_fMaxOpeningTime = {3.f};

	_float m_fAccfirstOpeningTime = { 0.f };

	void Set_bPlaying(_bool bPlaying);

	void Set_StopAllAttackObject(_float fStopTime);

	_bool Get_bReflect();

	void Set_bBeReflecting(_short iDirection);
	_bool Update_BeReflecting(_float fTimeDelta);

	void Set_bFinalSkillQTE(_bool bFinalSkillQTE);

	CHARACTER_INDEX Get_eCharacterIndex();



	//Model한테도 있음
	_bool m_bCinematic_NoMoveXZ = { false };

	void Set_AnimationMoveXZ(_bool bValue); // NOMoveXZ 잘못적어서 false가 움직임 true가 안움직임


	virtual void Character_CinematicEnd() {};


	void Set_bHeavySkill(_bool bHeavySkill);
	_bool m_bHeavySkill = { false };

	_float4 m_fAuraColor = {};
	_bool m_bAura = false;
	_bool m_bForcedAura = false;
	void Set_bForcedAura(_bool bForcedAura);

	void Set_bAura(_bool bAura);

	void Set_fAuraColor(_float4 fAuraColor);

	virtual void HitStopEffect() {};

	_bool m_bSoloFinalEndCount = false;
protected:

	void Reset_AttackStep();

	//애니메이션 끝에 대고 사용하지 말것
	void Update_NoEventAnimationLoof(_float fTimeDelta);
	void Update_ForcedEventAnimationLoof(_float fTimeDelta);

	void Update_NoEventTime(_float fTimeDelta);

	void Set_NoEventAnmationLoof(_float fMinPosition, _float fMaxPosition, _float fTime);
	void Set_EventAnmationLoof(_float fMinPosition, _float fMaxPosition, _float fTime);


	_float4x4 Make_BoneMatrix(char* BoneName);

	//기본적으로 좌우반전이 되지만 추가로 뒤집는경우 좌표가 좀 뒤틀릴 수 있음
	_float4x4 Make_BoneMatrix_Offset(char* BoneName, _float2 fOffset, _bool bFlipDirection = false);
	_float4x4 Character_Make_Matrix(_float2 fOffset = { 0,0 }, _bool bFlipDirection = false);
	//_float4x4 Character_Make_Matrix(_float2 fOffset = { 0,0 }, _bool bFlipDirection = false, _float fYRotation =1000.f);
	//_float4x4 Character_Make_Matrix(_float2 fOffset = { 0,0 }, _bool bFlipDirection = false, _float3 fScale ={1.f,1.f,1.f});



public:
	void		Character_Make_BoneEffect_Offset(char* BoneName, _wstring strEffectName, _float2 fOffset = { 0.f,0.f }, _bool bFlipDirection = false);
	class CEffect_Layer* Character_Make_BoneEffect(char* BoneName, _wstring strEffectName);
	class CEffect_Layer* Character_Make_Effect(_wstring strEffectName, _float2 fOffset = { 0,0 }, _bool bFlipDirection = false);
	//class CEffect_Layer* Character_Make_Effect(_wstring strEffectName, _float2 fOffset = { 0,0 }, _bool bFlipDirection = false, _float fYRotation =1000.f);

	//void Character_Make_Effect(_wstring strEffectName, _float2 fOffset = { 0,0 }, _bool bFlipDirection = false, _float3 fScale={1.f,1.f,1.f});

	void Character_Create_Distortion(_float3 vDir = { 0.f,1.f,0.f }, _float2 vOffSetPos = {0.f,0.f}, _float2 vOffSetScale = { 1.f,1.f }, _float fLifeTime = { 0.3f });
	void Character_Create_Distortion(_float3 vDir, _float3 vOffSetPos, _float2 vOffSetScale = { 1.f,1.f }, _float fLifeTime = { 0.3f });



	void Set_LoofAnimationCreate(_wstring strEffectName, _float fMaxTime, _float fPeriodTime, _float2 fOffset = { 0.f,0.f }, _bool bFlipDirection = false);
	void Update_LoofAnimationCreate(_float fTimeDelta);

	const _float4x4* Get_pTransformMatrix();

public:
	virtual void OnCollisionEnter(class CCollider* other, _float fTimeDelta) override;
	virtual void OnCollisionStay(class CCollider* other, _float fTimeDelta) override;
	virtual void OnCollisionExit(class CCollider* other) override;

	//UI
public:
	void Set_bRedHP(_bool bRedHP);
	void CharacterToUI_Info();

protected:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CTexture* m_pOutLineCom = { nullptr };
	CTexture* m_p2PTextureCom = { nullptr };
	CTexture* m_p1PTextureCom = { nullptr };
	CTexture* m_pDecalTextureCom = { nullptr };
	CTexture* m_pDetailTextureCom = { nullptr };
	
	_float					m_fRandom = {};
	_wstring				m_strModelName{};

	//_uint					m_iMoveGrade = {};
	//_uint					m_iCurMoveIndex{};
	//커맨드 만들면 스택형으로 넣어두고 0.3초안에 지우기

	//피격 스턴과 다름
	_float					m_fAccAnimationLock{};
	_float					m_fMaxAnimationLock{};
	_bool					m_bAnimationLock{};

	CHARACTER_INDEX			m_eCharacterIndex = { PLAY_GOKU };

	FrameEventMap* m_pFrameEvent = { nullptr };
	_bool					m_bMotionPlaying = false;

	vector<CInput> inputBuffer;

	_ushort m_iPlayerTeam = { 1 };
	_int	m_iLookDirection = { 1 };
	vector<CommandPattern> MoveCommandPatterns;
	vector<CommandPatternFunction> MoveCommandPatternsFunction;

	vector<CommandPatternFunction> MoveCommandPatternsFunction_Exactly;


	//_uint					m_iNextAnimationIndex = { 0 };

	//index,시간
	pair<_uint, _float>		m_iNextAnimation{ 0,0 };
	_float					m_fNextAnimationCurrentPosition = {};




	_ushort m_iJumpAnimationIndex = { 6 };
	_ushort m_iFallAnimationIndex = { 7 };

	_ushort m_iIdleAnimationIndex = { 0 };
	_ushort m_iCrouchAnimationIndex = { 4 };
	_ushort m_iBackWalkAnimationIndex = { 10 };
	_ushort m_iForwardWalkAnimationIndex = { 9 };
	_ushort m_iForwardDashAnimationIndex = { 11 };
	_ushort m_iForwardDashEndAnimationIndex = { 14 };

	_ushort m_iStandingMidAttackAnimationIndex = { 46 };
	_ushort m_iChaseAnimationIndex = { 13 };

	_ushort m_iReflectAnimationIndex = { 58 };

	_ushort m_iHit_Stand_LightAnimationIndex = { 21 };		//050
	//_ushort m_iHit_Stand_LightFrontAnimationIndex = { 21 };		//050

	_ushort m_iHit_Stand_MediumAnimationIndex = { 22 };		//051
	_ushort m_iHit_Crouch_AnimationIndex = { 23 };			//052

	_ushort m_iHit_Away_LeftAnimationIndex = { 33 };
	_ushort m_iHit_Away_UpAnimationIndex = { 35 };
	_ushort m_iHit_Away_LeftDownAnimationIndex = { 39 };

	_ushort m_iHit_WallBouce = { 34 };

	_ushort m_iHit_Air_Spin_LeftUp = { 31 };
	_ushort m_iHit_Air_Spin_Up = { 30 };  //072


	_ushort m_iHit_Air_LightAnimationIndex = { 24 };		//050
	_ushort m_iHit_Air_FallAnimationIndex = { 26 };


	//기상
	_ushort m_iBreakFall_Ground = { 41 };   //100
	_ushort m_iBreakFall_Air = { 42 };   //101
	_ushort m_iBound_Ground = { 25 };
	_ushort m_iLayUp = {27};


	_ushort m_iAttack_Air1 = { 52 };
	_ushort m_iAttack_Air2 = { 53 };
	_ushort m_iAttack_Air3 = { 54 };
	_ushort m_iAttack_AirUpper = { 55 };

	_ushort m_iAttack_Heavy = { 45 };
	_ushort m_iAttack_Crouch_Heavy = { 51 };


	_ushort m_iAttack_LightLast = { 47 };

	//가드
	_ushort m_iGuard_GroundAnimationIndex = { 18 };
	_ushort m_iGuard_CrouchAnimationIndex = { 19 };
	_ushort m_iGuard_AirAnimationIndex = { 20 };


	//잡기
	_ushort m_iGrabReadyAnimationIndex = { 17 };
	_ushort m_iGrabAnimationIndex = { 60 };

	_ushort m_iSparkingAnimationIndex = { 59 };  //303

	_ushort m_iDyingStandingAnimationIndex = { 29 };



	//시작
	_ushort m_iStartAnimatonIndex = { 75 };  //600cs

	//승리
	_ushort m_iWinAnimationIndex = {78 };  //610

	//라운드변경
	_ushort m_iNextRound_RightHandAppear_Cutscene_AnimationIndex = { 79 };  //620cs ->630로 연계
	_ushort m_iNextRound_RightHand_AnimationIndex = { 81 };  //621cs ->631으로 연계 Durtaion 122

	_ushort m_iNextRound_LeftHand_Cutscene_AnimationIndex = { 80 };  //630 Durtaion 24
	_ushort m_iNextRound_LeftHand_AnimationIndex = { 82 };  //631 Durtaion 24





	_float m_fGravityTime = { 0.f };
	_float m_fJumpPower = 3;// { 0.f };

	//가속도
	//_float m_fImpuse = { 0.f };


	//피격시 Y축 이동을 대비해 xy로 만듦.  각도별로 되어있으니 그냥 애니메이션으로 처리할까 생각도 듦
	_float2 m_fImpuse = { 0.f,0.f };


	_bool m_bDoubleJumpEnable = { true };
	_bool m_bAriDashEnable = { true };

	_bool m_bJumpLock = { false };
	_float m_fAccJumpLockTime = { 0.f };

	_bool m_bAttackGravity = { true };


	_bool m_bChase = { false };
	_float m_fAccChaseTime = { 0.f };
	_vector m_vChaseDir{ 0 };

	_wstring m_ChaseEffectName = { TEXT("BurstR-02_Rotated_Left") };

	


	//스턴 관련
	_bool m_bStun = { false };
	_float m_fAccStunTime = {};
	_float m_fMaxStunTime = {};

	_bool m_bStunShakeDirection = { false };

	//디버그용 임시 collider
	//CCollider_Test* m_pColliderCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

	CCharacter* m_pDebugEnemy = { nullptr };
	CCharacter* m_pEnemy = { nullptr };

	class CEffect_Layer* m_pChaseEffectLayer = { nullptr };

	_short		 m_iHP = 10000;   //맞는순간 음수가 될 수 있으니 ushort 대신 sohrt.  범위가   -32,768 ~ 32,767 니까 주의 


	_float		m_fAccSmokeTime = {};

	_ushort		m_iAttackStepCount = { 0 };  //콤보수 아님.

	_bool		m_bSparking = false;
	_float		m_fAccSparkingTime = {};
	_float		m_fMaxSparkingTime = {};
	_bool		m_bNextAnimationGravityEvent = false;




	//_bool m_bOnlyCutSceneNoMove = { false };

	_bool m_bChaseStoping = false;
	_bool m_bChaseEnable = true;
	//_bool m_bChaseAttackEnable = true;

	_bool m_bGrab = false;
	_bool m_bGrab_Air = false;
	_bool m_bGrabDraw = false;

	_float m_fAccGrabTime = {};


	//1020추가분량

	//스턴관련
	_bool m_bHitGroundSmashed = { false };
	_bool m_bWallBounce = { true };
	_bool m_bNoGravity = { false };
	_float m_fNoGravitySafeTime = { 0.f };
	_float m_fMaxfNoGravitySafeTime = { 0.2f };

	_bool m_bAwayUpGravity = { false };


	//멤버변수에 넣는 대신 함수로 체크?   이거 없으면  서브캐릭터들도 가드를 해버림.  교체할때 그냥 사라지게?
	//_bool m_bGuard = { false };


	_float m_fPreviousX = {};

	_bool	m_bGrabbed = { false };
	_bool	m_bGrabbedGravity = { false };
	_ushort m_iGrabLoof = 3;
	_float m_fAIrGrabEndAnimationPositon = { 29.99 }; //공중 잡기시 막타로 사용하는 애니메이션의 AttackEvent Posioton 


	_bool m_bCharacterDead = { false };

	_bool m_bTag_In = { false };	//교대하러 들어가는 캐릭터.
	_bool m_bPlaying = { false };  //이 캐릭터만 조작함

	_float m_fAccTag_InTime = { 0.f };

	_bool m_bNoEventLoofAnimation = false;
	_bool m_bForcedEventLoofAnimation = false;
	_float m_fNoEventLoofMinPosition = {};
	_float m_fNoEventLoofMaxPosition = {};
	_float m_fMaxNoEventLoofTime = {};
	_float m_fAccNoEventLoofTime = {};


	//정지상태에서 애니메이션 연속 생성용 변수
	_bool m_bEffectLoofCreate = false;
	_float m_fAccEffectLoofCreateTime = { 0.f };
	_float m_fMaxEffectLoofCreateTime = { 0.f };
	_wstring m_strEffectLoofCreateName = {};

	_float m_fAccEffectPeriodTime = {};
	_float m_fMaxEffectPeriodTime = {};

	_float2 m_fEffectLoofCreateOffset = {};
	_bool m_bEffectLoofCreateFlip = { false };


	_bool m_bDying = { false };
	_float m_fAccDyingTime = {};
	//_bool m_bKO = { false };
	_bool m_bDyingBlack = { true };

	_bool m_bDynamicMove = { false };
	_bool m_bDestructiveFinish = { false };
	_bool m_bAttackBackEvent = { false };

	_bool m_bReflect = { false };
	//_bool m_bReflectAttackBack 

	_bool m_bBeReflecting = { false };
	_float m_fAccBeReflectingTime = { 0.f };
	CGameObject* m_pReflectObject = { nullptr };


	//_bool m_bFinalSkillQTESucces = { false }; //m_iQTE 로 대체됨
	_bool m_bBenishingAttack = { false };
	_bool m_bInvisible = { false };



	//디버그용
	_uint m_iDebugComoboDamage = { 0 };
	_bool m_bDebugInputLock = { false };


	_short m_eChaseSoundIndex = {0};

public:
	void Set_InputActive(_bool isActive) { m_bDebugInputLock = isActive; }

	//class CAttackObject* m_pChaseAttackObejct = { nullptr };
	//Set_RemoteDestory()



public:
	typedef struct
	{
		_bool        bStun = { FALSE };
		_bool        bHit = { FALSE };
		_bool        bAttBuf = { FALSE };

		_int        iHp = { 0 };
		_uint        iComboCount = { 0 };

		_int        iSKillPoint = { 0 };
		_int        iSKillCount = { 0 };

		_uint		iTeam = { 0 };

		CUI_Define::PLAYER_SLOT		ePlayer_Slot = {};
		CUI_Define::PLAYER_ID        ePlayerID = {};

	}Character_INFO_DESC;

public:
	Character_INFO_DESC Get_PawnDesc() { return m_tCharacterDesc; }
	void GetUI_Input(DirectionInput eDirInput, ButtonInput eBtnInput,_uint iTeam);


	//UI에서 써야하는 정보 

protected:
	_bool					m_bHit = { FALSE };
private:
	_uint					m_iComboCount = { 0 };
	_int					m_iSKillPoint = { 0 };
	_int					m_iSKillCount = { 0 };

	_bool					m_bRedHp = { FALSE };
	_bool					m_bAttBuf = { FALSE };

	_uint					m_iNumAttBuf = { 1 };
	_uint   m_iPrevComboCount = { 0 };

	//UI에 보내야할 정보
	Character_INFO_DESC				 m_tCharacterDesc = {};

	_float					m_fAttBufTimer = { 0.f };

protected:
	CUI_Define::PLAYER_ID				m_eCharacterID = {};
	CUI_Define::PLAYER_SLOT				m_ePlayerSlot = { CUI_Define::SLOT_END };
	class CUI_Manager* m_pUI_Manager = { nullptr };
	class CEffect_Manager* m_pEffect_Manager = { nullptr };

	string m_strName = "";
	_float4		m_vDyingPosition = { 0.f,0.f,0.f,1.f };
	//_float4x4 m_IdentityMatrix;
private:
	HRESULT Ready_Components();
public:
	HRESULT Bind_ShaderResources();
	// Layer_MeteoBreak
	void Map_DestructiveFinish();

	// 1: 승, 
	// -1 : 패 
	// 0: 비김
	void Notify_QTE_Same_Grab(_int result);

	// 1: 승, 
	// -1 : 패 
	void Notify_QTE_1p_Grab(_int result);

	// 1: 승, 
	// -1 : 패 
	void Notify_QTE_Hit(_int result);

	// 1: 승, 
	// -1 : 패 
	void Notify_QTE_Continuous_Attack(_int result);

	CUI_Define::PLAYER_ID Get_CharacterID() { return m_eCharacterID; };


//public:
	

	void Character_Start_QTE(_uint iQTEID);
protected:
	_short m_iQTE = { -1 };
	_bool m_bCreateQTE = { false };


protected:
	void Add_ChaseLight();
	//m_eCharacterIndex;
	_float4 m_vChaseLight;
	_bool m_isLightChaseDone;

public:
	static CCharacter* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;


};



END