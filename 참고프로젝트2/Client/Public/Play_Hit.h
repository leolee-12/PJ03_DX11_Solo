#pragma once

#include "Client_Defines.h"
#include "AnimationEvent_Defines.h"
#include "FrameEvent_Manager.h"
#include "Character.h"

#include "Hit_MeleeAttack.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CPlay_Hit final : public CCharacter
{
public:
	typedef struct : CGameObject::GAMEOBJECT_DESC
	{
		_wstring strModelName;
	}Play_Hit_DESC;
	enum AnimationIndex
	{
		//Light - 약공격
		//Meaduim - 중공격
		//Heavy - 강공격
		//Special - 기탄 (원거리 공격)
		//Air - 1,2,3 공중공격
		//Crouch - 앉은 상태 공격
		//236 숫자 써있는건 텐키의 방향키를 묘사한 숫자
		//Final - 최종필살기
		ANIME_ATTACK_LIGHT1 = 41, ANIME_ATTACK_LIGHT2 = 42, ANIME_ATTACK_LIGHT3 = 43,
		ANIME_ATTACK_MEDIUM = 44, ANIME_ATTACK_HEAVY = 45,
		ANIME_ATTACK_SPECIAL = 46,


		ANIME_ATTACK_CROUCH_LIGHT = 47, ANIME_ATTACK_CROUCH_MEDUIM = 48, ANIME_ATTACK_CROUCH_HEAVY = 49,
		ANIME_ATTACK_AIR1 = 50, ANIME_ATTACK_AIR2 = 51, ANIME_ATTACK_AIR3 = 52, ANIME_ATTACK_UPPER_AIR = 53, ANIME_ATTACK_SPECIAL_AIR = 54,

		ANIME_GRAB = 57, //304 
		//ANIME_GRAB_RUSH = 61 // 306cs
		//306cs 61  지상 드래곤러시 성공 후 추적

		ANIME_SPARKING = 56,
		ANIME_REFLECT = 55,




		ANIME_CROUCH_START = 2, ANIME_CROUCHING = 3,
		//ANIME_CROUCH_SPIN = 4,

		//가드
		ANIME_GUARD_GROUND = 17,	//040      //1번 애니메이션은 뒤돌기임
		ANIME_GUARD_CROUCH = 18,	//041
		ANIME_GUARD_AIR = 19,		//026

		ANIME_JUMP_UP = 5, ANIME_JUMP_DOWN = 6,



		//이동
		ANIME_IDLE = 0,
		ANIME_FORWARD_WALK = 8, ANIME_BACK_WALK = 9,
		ANIME_FORWARD_DASH = 10, ANIME_BACK_DASH = 11,
		ANIME_FORWARD_DASH_END = 13, ANIME_BACK_DASH_END = 14,



		ANIME_CHASE = 12,

		ANIME_GRAB_READY = 16,  //036 기본잡기 준비자세.  (공중)   추적에선 사용하지 않고 잡기 준비로만 사용,  NextPosition 5



		//땅
		ANIME_HIT_LIGHT = 20, ANIME_HIT_LIGHT_FRONT = 21, ANIME_HIT_CROUCH = 22,			//050   051 052


		//공중
		ANIE_HIT_AIR = 23, ANIME_HIT_FALL = 25,									//060 061


		//땅에 닿는 순간
		ANIME_HIT_BOUND_DOWN = 24,

		//피격
		ANIME_HIT_HEAVY_AWAY_LEFT = 31,
		ANIME_HIT_HEAVY_AWAY_UP = 33, //077      보통 061(26번) 으로 연계됨 
		ANIME_HIT_HEAVY_AWAY_LEFTDOWN = 37, //081
		ANIME_HIT_HEAVY_AWAY_SPIN_UP = 28,
		ANIME_HIT_HEAVY_AWAY_SPIN_LEFTUP = 29,
		ANIME_HIT_HEAVY_AWAY_SPIN_LEFT = 30,

		ANIME_HIT_WALLBOUNCE = 32,				//076.  이 다음 062(26번 ANIME_HIT_FALL_UPPER)로 연계됨


		ANIME_DIE_STAND = 27,


		ANIME_LAYUP = 26,
		ANIME_BREAK_FALL_GROUND = 39, // 100
		ANIME_BREAK_FALL_AIR = 40, // 101

		//////////////////////////기술들, 필살기들 처리하기



		ANIME_236_POSE = 60,
		ANIME_236_POSE_LIGHT = 61,
		ANIME_236_POSE_MEDIUM = 62,
		ANIME_236_POSE_HEAVY = 63,
		//405(=64)는 어퍼 직전까지만 하는데 뭐냐 이거

		//각각 236약, 어퍼, 앉아약  인데 이거 진짜 구현하나?
		ANIME_236_SPECIAL_RIGHT =61,
		ANIME_236_SPECIAL_UP = 49,
		ANIME_236_SPECIAL_DOWN = 47,


		ANIME_214_POSE = 65,


		ANIME_236_SPECIAL = 66,
		ANIME_236_ULTIMATE = 67,  //공중버전도 하나로 묶여있음
		//68 R버전 필살기
		ANIME_214_FINAL_START = 69, // 
		ANIME_214_FINAL_SUCESS = 70, // 


		//28 ??	 MEDIUM 같은데 중공격 맞아도 이거 안나옴
		//078 : HIT_AWAY_LEFTUP

		//310(=59) 기모으기


		//뭔지모름
		
		//037 (=15) 이상하게 앉는모션
		//038 이상하게 점프
		//040 이상하게 서서가드
		 

		//시작 600cs
		ANIME_START_DEFAULT = 71,

		//승리 610cs
		ANIME_WIN_DEFAULT = 74,

		//라운드변경
		ANIME_NEWROUND_RIGHTHAND_APEEAR_CUTSCENE = 75, //620 cs.   ->630로 연계
		ANIME_NEWROUND_LEFTHAND_CUTSCENE = 76, //621 cs ->631으로 연계 Durtaion 122

		ANIME_NEWROUND_RIGHTHAND = 77,  //630.   
		ANIME_NEWROUND_LEFTHAND = 78,  //631.    Durtaion 24


		
	};
	enum AnimationCount
	{
		COUNT_ATTACK_MEDIUM = 0,
		//COUNT_ATTACK_CROUCH_MEDUIM,
		COUNT_ATTACK_AUTO_MEDIUMTOHEAVY,
		COUNT_ATTACK_SPECIAL,
		COUNT_ATTACK_236SPECIAL_UP,
		COUNT_ATTACK_236SPECIAL_RIGHT,
		COUNT_ATTACK_236SPECIAL_DOWN,
		COUNT_ATTACK_236_LIGHT,
		COUNT_ATTACK_236_MEDIUM,
		COUNT_ATTACK_236_HEAVY,
		COUNT_ATTACK_GRAB,
		COUNT_ATTACK_BENISHING,
		COUNT_END
	};

	struct LaserData {
		float fLifeTime;
		_float3 Rotation;
		//_float3 Scale;
		_float2 Scale;  //z값은 무조건 1이라 이게 맞긴 한데 

	};
private:
	CPlay_Hit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlay_Hit(const CPlay_Hit& Prototype);
	virtual ~CPlay_Hit() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Player_Update(_float fTimeDelta) override;
	virtual void Camera_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;


	_bool Update_CounterPose(_float fTimeDelta);
	_bool Update_214Pose(_float fTimeDelta);


	_bool Get_b236Posing();
	void Set_b236Posing(_bool b236Posing);

	_bool Get_b236Sepcial();
	void Set_b236Special(_bool b236Special);


	virtual _bool Check_bCurAnimationisGroundMove(_uint iAnimation = 1000) override;
	virtual _bool Check_bCurAnimationisAttack(_uint iAnimation = 1000) override;
	virtual _bool Check_bCurAnimationisAirAttack(_uint iAnimation = 1000)override;
	virtual _bool Check_bCurAnimationisHalfGravityStop(_uint iAnimation = 1000) override;


	virtual _short Check_bCurAnimationisCanChase() override;


	_bool* Get_pbAttackCount() { return m_bAttackCount; };
	_bool* Get_pbCounterPose() { return &m_bCounterPose; };

	virtual void Reset_AttackCount() override;

	virtual void Gravity(_float fTimeDelta) override;

	virtual void AttackEvent(_int iAttackEvent, _int AddEvent = 0)override;
	virtual void Play_Sound(_uint SoundName, _bool bisLoof, _float fvolume)override;
	virtual void Play_Group_Sound(_uint groupKey, _bool loop, _float volume)override;
	virtual void Play_Sound_Stop(_uint SoundName)override;


	void Reset_PoseTime();

	void Add_YellowLight();
	void Add_BlueLight();


	void Update_214FinalInvisible();
	void Update_214FinalInvisible(_float fTimeDelta);

	_bool isNearlyEqual(_float CurValue, _float TargetValue);

	//virtual AttackColliderResult Set_Hit4(_uint eAnimation, AttackGrade eAttackGrade, AttackType eAttackType, _float fStunTime, _uint iDamage, _float fStopTime, _short iDirection, _float2 Impus = { 0,0 }) override;
	virtual AttackColliderResult Set_Hit4(_uint eAnimation, AttackGrade eAttackGrade, AttackType eAttackType, _float fStunTime, _uint iDamage, _float fStopTime, _short iDirection, _float2 Impus = { 0,0 },_bool bParitle=true) override;



	void MoveToEnemy_Ground(_float fMaxDistance, _float fOffset = 0.4f);


	void LaserListUpdate(_float fTimeDelta);

	//방향,시간
	//list<_float3, _float> m_LaserList;

	//시간,xyz회전각도
	list<pair<_float, _float3>> m_LaserList;
	
	vector<LaserData> m_LaserListRS;

	list<CEffect_Layer*> m_214GlassList;

	//_ubyte iFinalDustCount = 0;
	_bool m_bFinalInvisibleToggle = false;
	_float m_fFinalInvisiblePositionSave = {};
	_ubyte m_iFinalInvisibleCount = {};
private:
	
	CHit_MeleeAttack m_tAttackMap;

	_bool m_bAttackCount[COUNT_END] = { true };


	_bool m_bFinalSkillAdd = { false };

	_float m_fAccPoseTime = {};
	_float m_fMaxPoseTime = { 1.5f };

	_bool m_b236Posing = { false };
	_bool m_b236Special = { false };

	_bool m_b214Posing = { false };
	//_bool m_b214Medium = { 0 };

	_bool m_bCounterPose = { false };
	_bool m_bCounterSucces = { false };

	//_bool m_bInvisible = { false };

	_ushort m_iAttackLightLoofCount = { 2 };

	//_bool m_bUltimate_Air

	_float m_fAccFinalTime = {};


	CEffect_Layer* m_pUltimateAuraEffect = { nullptr };
	class CEffect_Layer* m_pAttackFinalGlassEffect_Layer = { nullptr };
	CEffect_Layer* m_p236LightEffect = { nullptr };
	CEffect_Layer* m_pOpeningGlass = { nullptr };
	CEffect_Layer* m_p236PoseEffect = { nullptr };
	_bool m_b236LightSuccess = { false };

	_short m_i214GlassCount = 0;
	//_bool m_bLight3Effect = true;
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CPlay_Hit* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END