#pragma once

#include "Client_Defines.h"
#include "AnimationEvent_Defines.h"
#include "FrameEvent_Manager.h"
#include "Character.h"

#include "S21_MeleeAttack.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CPlay_21 final : public CCharacter
{
public:
	typedef struct: CGameObject::GAMEOBJECT_DESC
	{
		_wstring strModelName;
	}Play_21_DESC;
	enum AnimationIndex 
	{
		ANIME_ATTACK_LIGHT1 = 43, ANIME_ATTACK_LIGHT2 = 44, ANIME_ATTACK_LIGHT3 = 45,
		ANIME_ATTACK_MEDIUM = 46, ANIME_ATTACK_HEAVY = 47,
		ANIME_ATTACK_SPECIAL =48,

		ANIME_ATTACK_AIR1 = 53, ANIME_ATTACK_AIR2 = 54, ANIME_ATTACK_AIR3 = 55, //,ANIME_ATTACK_SPECIAL_AIR = 52, 52애니메이션을 16position부터

		ANIME_ATTACK_CROUCH_LIGHT = 49, ANIME_ATTACK_CROUCH_MEDUIM = 50, ANIME_ATTACK_CROUCH_HEAVY = 51, ANIME_ATTACK_UPPER_AIR = 51,  //공중어퍼랑 지상어퍼 모션이 같고 구분이 안되어있는데 어쩌지
		ANIME_ATTACK_CROUCH_SPECIAL = 52,

		ANIME_ATTACK_236 = 64,  //ANIME_ATTACK_236_AIR = 67,
		ANIME_ATTACK_214 = 61,

		ANIME_ATTACK_236_SPECIAL =63, // ANIME_ATTACK_236_SPECIAL_UPSIDE = 63,
		ANIME_ATTACK_236_SPECIAL_SUCCES = 62,
		//ANIME_ATTACK_214_SPECIAL = 62, // ANIME_ATTACK_236_SPECIAL_UPSIDE = 63,

		ANIME_ATTACK_236_ULTIMATE = 71,  //ANIME_ATTACK_236_SPECIAL_AIR_DOWNSIDE = 65,

		ANIME_FINAL_START = 75,
		ANIME_FINAL_END = 76,

		ANIME_IDLE = 2, 
		ANIME_FORWARD_WALK = 10, ANIME_BACK_WALK = 11,  
		ANIME_FORWARD_DASH = 12, ANIME_BACK_DASH = 13,
		ANIME_FORWARD_DASH_END = 15, ANIME_BACK_DASH_END = 16,


		ANIME_CROUCH_START = 4, ANIME_CROUCHING = 5,
		ANIME_JUMP_UP = 7, ANIME_JUMP_DOWN = 8,


		///////////////////
	

		ANIME_CHASE = 14,


		//땅
		ANIME_HIT_LIGHT = 22, ANIME_HIT_LIGHT_FRONT = 23, ANIME_HIT_CROUCH = 24,			//050   051 052


		//공중
		ANIE_HIT_AIR = 25, ANIME_HIT_FALL = 27,									//060 061


		//땅에 닿는 순간
		ANIME_HIT_BOUND_DOWN = 26,

		//피격
		ANIME_HIT_HEAVY_AWAY_LEFT = 33,
		ANIME_HIT_HEAVY_AWAY_UP = 35, //077      보통 061(26번) 으로 연계됨 
		ANIME_HIT_HEAVY_AWAY_LEFTDOWN = 39, //081
		ANIME_HIT_HEAVY_AWAY_SPIN_UP = 30, //072
		ANIME_HIT_HEAVY_AWAY_SPIN_LEFTUP = 31,
		ANIME_HIT_HEAVY_AWAY_SPIN_LEFT = 32,

		ANIME_HIT_WALLBOUNCE = 34,				//076.  이 다음 062(26번 ANIME_HIT_FALL_UPPER)로 연계됨


		ANIME_LAYUP = 28,
		ANIME_BREAK_FALL_GROUND = 41, // 100
		ANIME_BREAK_FALL_AIR = 42, // 101


		//28 ??	 MEDIUM 같은데 중공격 맞아도 이거 안나옴
		//078 : HIT_AWAY_LEFTUP

		ANIME_DIE_STAND = 29,


		//가드
		ANIME_GUARD_GROUND = 19,	//040
		ANIME_GUARD_CROUCH = 20,	//041
		ANIME_GUARD_AIR = 21,		//042



		ANIME_GRAB_READY = 17,  //038 기본잡기 준비자세.  (공중)   추적에선 사용하지 않고 잡기 준비로만 사용,  NextPosition 5
		ANIME_GRAB = 58, //304 60 드래곤러시    duration 40~130은 공격  23에서 275로 이동함  5~20쯤에서 공격판정하고 성공시 40으로 이동하기
		//ANIME_GRAB_RUSH = 61 // 306cs

		ANIME_SPARKING = 57,
		ANIME_REFLECT=56,//301 56 패링
		//306cs 61  지상 드래곤러시 성공 후 추적


		//시작 600cs
		ANIME_START_DEFAULT = 77,

		//승리 610cs
		ANIME_WIN_DEFAULT = 80,

		//라운드변경
		ANIME_NEWROUND_RIGHTHAND_APEEAR_CUTSCENE = 81, //620 cs.   ->630로 연계
		ANIME_NEWROUND_LEFTHAND_CUTSCENE = 82, //621 cs ->631으로 연계 Durtaion 122

		ANIME_NEWROUND_RIGHTHAND = 83,  //630.   
		ANIME_NEWROUND_LEFTHAND = 84,  //631.    Durtaion 24

	};
	enum AnimationCount
	{
		COUNT_ATTACK_MEDIUM =0,
		COUNT_ATTACK_CROUCH_MEDUIM,
		COUNT_ATTACK_GRAB, 
		COUNT_ATTACK_BENISHING,
		COUNT_END
	};

private:
	CPlay_21(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlay_21(const CPlay_21& Prototype);
	virtual ~CPlay_21() = default;

public:

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Player_Update(_float fTimeDelta) override;
	virtual void Camera_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;


	//virtual void Test_InputCommand();
	//virtual void Set_Animation(_uint iAnimationIndex) override;
	//virtual void Set_Animation(_uint iAnimationIndex, _bool bloof = false);

	void KeyTest();


	virtual _bool Check_bCurAnimationisGroundMove(_uint iAnimation = 1000) override;
	virtual _bool Check_bCurAnimationisAttack(_uint iAnimation = 1000) override;
	virtual _bool Check_bCurAnimationisAirAttack(_uint iAnimation = 1000)override;


	virtual _short Check_bCurAnimationisCanChase() override;


	_bool* Get_pbAttackCount() { return m_bAttackCount; };
	virtual void Reset_AttackCount() override;

	virtual void Gravity(_float fTimeDelta) override;
	virtual void AttackEvent(_int iAttackEvent, _int AddEvent = 0)override;
	virtual void Play_Sound(_uint SoundName, _bool bisLoof, _float fvolume)override;
	virtual void Play_Group_Sound(_uint groupKey, _bool loop, _float volume)override;
	virtual void Play_Sound_Stop(_uint SoundName)override;

	virtual void Test_InputCommand();


	void Update_FinalSkill(_float fTimeDelta);


	

private:
	CModel* m_pModelCom_Opening = { nullptr };

	CS21_MeleeAttack m_tAttackMap;

	_bool m_bAttackCount[COUNT_END] = { true };
	//_ushort m_iCountGroundSpecial = 0;

	_float m_fAccFinalSkillTime = {};
	_bool m_bFinalSkillSucess = { false };


	_ushort m_iFinalLoofCount = { 10 };
	_bool m_bFinalSkillAdd = { false };

	class CEffect_Layer* m_pAttack236ChargeEffect_Layer = { nullptr };
	class CEffect_Layer* m_pAttack214AssultEffect_Layer = { nullptr };

	class CEffect_Layer* m_pAttack214GroundEffect_Layer = { nullptr };

	class CEffect_Layer* m_pAttackUltimateLaserEffect_Layer = { nullptr };

	class CEffect_Layer* m_pAttackFinalChaseEffect_Layer = { nullptr };
	

	_bool m_bFinalSoundEnable = { true };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


	void Check_Ground();
	
public:
	static CPlay_21* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END