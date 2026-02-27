#pragma once

#include "Client_Defines.h"
#include "AnimationEvent_Defines.h"
#include "FrameEvent_Manager.h"
#include "Character.h"

#include "Frieza_MeleeAttack.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CPlay_Frieza final : public CCharacter
{
public:
	typedef struct : CGameObject::GAMEOBJECT_DESC
	{
		_wstring strModelName;
	}Play_Frieza_DESC;
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

		ANIME_ATTACK_AIR1 = 51, ANIME_ATTACK_AIR2 = 52, ANIME_ATTACK_AIR3 = 53, ANIME_ATTACK_SPECIAL_AIR = 54, ANIME_ATTACK_SPECIAL_AIR2 = 55,

		ANIME_ATTACK_CROUCH_LIGHT = 47, ANIME_ATTACK_CROUCH_MEDUIM = 48, ANIME_ATTACK_CROUCH_HEAVY = 49, ANIME_ATTACK_CROUCH_SPECIAL = 50,
		ANIME_ATTACK_CROUCH_CROUCH_SPECIAL = 56,


		ANIME_SPARKING = 58,
		ANIME_REFLECT = 57,


		//시네마틱
		ANIME_GOKU_CINEMATIC_01 = 86, //FRN801cs_01  //오프닝 무비
		ANIME_GOKU_CINEMATIC_02 = 87, //FRN801cs_02 //시작 직후 
		ANIME_GOKU_CINEMATIC_03 = 88,  //승리 후


		ANIME_GRAB = 59, //304 60 드래곤러시    duration 40~130은 공격  23에서 275로 이동함  5~20쯤에서 공격판정하고 성공시 40으로 이동하기


		ANIME_ATTACK_236 = 62,
		ANIME_ATTACK_236_SPECIAL = 63,
		ANIME_ATTACK_214 = 64,  //214 Special을 이걸로 쓰자. 기원참임
		ANIME_ULTIMATE = 65, //1필


		ANIME_214_FINAL = 66,
		ANIME_214_FINAL_2 = 67,


		ANIME_WAKEUP_FINAL = 68,
		ANIME_TRANSFORM_FINAL = 69,
		ANIME_TRANSFORM_FINAL_CUTSCENE = 70,






		ANIME_IDLE = 0,
		ANIME_JUMP_UP = 5, ANIME_JUMP_DOWN = 6,
		ANIME_FORWARD_WALK = 8, ANIME_BACK_WALK = 9,
		ANIME_FORWARD_DASH = 10, ANIME_BACK_DASH = 11,
		ANIME_FORWARD_DASH_END = 13, ANIME_BACK_DASH_END = 14,
		ANIME_CROUCH_START = 2, ANIME_CROUCHING = 3,
		ANIME_CHASE = 12,


		//가드
		ANIME_GUARD_GROUND = 17,	//040
		ANIME_GUARD_CROUCH = 18,	//041
		ANIME_GUARD_AIR = 19,		//042



		//땅
		ANIME_HIT_LIGHT = 20, ANIME_HIT_LIGHT_FRONT = 21, ANIME_HIT_CROUCH = 22,			//050   051 052




		//공중
		ANIE_HIT_AIR = 23, ANIME_HIT_FALL = 25,									//060 061


		//땅에 닿는 순간
		ANIME_HIT_BOUND_DOWN = 24,   // WakeUp필살기로 연계모션 중간단계


		ANIME_LAYUP = 26,
		ANIME_DIE_STAND = 27,


		//피격
		ANIME_HIT_HEAVY_AWAY_SPIN_UP = 28,
		ANIME_HIT_HEAVY_AWAY_SPIN_LEFTUP = 29,
		ANIME_HIT_HEAVY_AWAY_SPIN_LEFT = 30,

		ANIME_HIT_HEAVY_AWAY_LEFT = 31,
		ANIME_HIT_HEAVY_AWAY_UP = 33, //077      보통 061(26번) 으로 연계됨 
		ANIME_HIT_HEAVY_AWAY_LEFTDOWN = 37, //081


		ANIME_HIT_WALLBOUNCE = 32,				//076.  이 다음 062(26번 ANIME_HIT_FALL_UPPER)로 연계됨

		ANIME_BREAK_FALL_GROUND = 39, // 100
		ANIME_BREAK_FALL_AIR = 40, // 101

		ANIME_GRAB_READY = 16,  //038 기본잡기 준비자세.  (공중)   추적에선 사용하지 않고 잡기 준비로만 사용,  NextPosition 5



		//시작 600cs
		ANIME_START_DEFAULT = 71,

		//승리 610cs
		ANIME_WIN_DEFAULT = 74,

		//라운드변경
		ANIME_NEWROUND_RIGHTHAND_APEEAR_CUTSCENE = 76, //620 cs.   ->630로 연계
		ANIME_NEWROUND_LEFTHAND_CUTSCENE = 77, //621 cs ->631으로 연계 Durtaion 122

		ANIME_NEWROUND_RIGHTHAND = 78,  //630.   
		ANIME_NEWROUND_LEFTHAND = 79,  //631.    Durtaion 24


	};
	enum AnimationCount
	{
		COUNT_ATTACK_MEDIUM = 0,
		COUNT_ATTACK_CROUCH_MEDUIM,
		COUNT_ATTACK_SPECIAL,
		COUNT_ATTACK_GRAB,
		COUNT_ATTACK_BENISHING,
		COUNT_ATTACK_CROUCH_SPECIAL,
		COUNT_END
	};

private:
	CPlay_Frieza(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlay_Frieza(const CPlay_Frieza& Prototype);
	virtual ~CPlay_Frieza() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Player_Update(_float fTimeDelta) override;
	virtual void Camera_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;
	virtual HRESULT Shadow_Render(_float fTimeDelta) override;
	
	//virtual void NextMoveCheck() override;
	//virtual void AttackNextMoveCheck() override;
	//virtual void AnimeEndNextMoveCheck() override;

	//virtual void Test_InputCommand();
	//virtual void Set_Animation(_uint iAnimationIndex) override;
	//virtual void Set_Animation(_uint iAnimationIndex, _bool bloof = false);

	void KeyTest();


	virtual _bool Check_bCurAnimationisGroundMove(_uint iAnimation = 1000) override;
	virtual _bool Check_bCurAnimationisAttack(_uint iAnimation = 1000) override;
	virtual _bool Check_bCurAnimationisAirAttack(_uint iAnimation = 1000)override;
	virtual _bool Check_bCurAnimationisHalfGravityStop(_uint iAnimation = 1000) override;


	virtual _short Check_bCurAnimationisCanChase() override;


	_bool* Get_pbAttackCount() { return m_bAttackCount; };
	_ushort* Get_piSpecialCount() { return &m_iCountGroundSpecial; };
	_ushort* Get_piSAirpecialCount() { return &m_iCountAirSpecial; };

	virtual void Reset_AttackCount() override;

	virtual void Gravity(_float fTimeDelta) override;

	virtual void AttackEvent(_int iAttackEvent, _int AddEvent = 0)override;
	virtual void Play_Sound(_uint SoundName, _bool bisLoof, _float fvolume)override;
	virtual void Play_Group_Sound(_uint groupKey, _bool loop, _float volume)override;
	virtual void Play_Sound_Stop(_uint SoundName)override;


	virtual _float Get_DamageScale(_bool bUltimate = false) override;
	virtual void Set_BreakFall_Ground() override;

	void Add_YellowLight();
	void Add_BlueLight();


	void Update214ReturnEvent(_float fTimeDelta);

	_bool Get_bGoldenFrieza();

	virtual void Character_CinematicEnd() override;

private:
	//CModel* m_pModelCom_Opening = { nullptr };
	CModel* m_pModelCom_Skill = { nullptr };
	CTexture* m_pGold_BaseTexture = { nullptr };
	CTexture* m_pGold_OutLineTexture = { nullptr };
	CTexture* m_pMetalTexture = { nullptr };

	CFrieza_MeleeAttack m_tAttackMap;

	_bool m_bAttackCount[COUNT_END] = { true };
	_ushort m_iCountGroundSpecial = 0;
	_ushort m_iCountAirSpecial = 0;


	_float m_fAcc214AttackTime = {};
	_bool m_bAcc214Attack = false;
	_short m_i214AttackPreviousDirection = { 0 };


	_bool m_bGoldFrieza = false;

	list<CEffect_Layer*>m_DeleteLayer;

	CEffect_Layer* m_p236SpecialEffect = { nullptr };
	CEffect_Layer* m_p236UltimateEffect = { nullptr };

	CEffect_Layer* m_p214FinalEffect = { nullptr };
	CEffect_Layer* m_pAttackHeavyEffect = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


public:
	static CPlay_Frieza* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END