#pragma once

#include "GameObject.h"
#include "Client_Defines.h"
#include "UI_Define.h"

#include "AnimationEvent_Defines.h"
#include "AnimationNameMap.h"


BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)


class CBattleInterface_Manager final : public CBase
{
	DECLARE_SINGLETON(CBattleInterface_Manager)

public:
	typedef struct
	{
		_ushort iTeam = {};
		CUI_Define::PLAYER_SLOT eSlot = { CUI_Define::SLOT_END };
		wstring	PrototypeTag = {};
		CUI_Define::PLAYER_ID ePlayerID = { CUI_Define::PAWN_END };
	}CHARACREATE_DESC;
private:
	CBattleInterface_Manager();
	virtual ~CBattleInterface_Manager() = default;

public:
	void Gain_HitCount(_ushort iHitCount, _ushort iTeam);
	void Reset_HitCount(_ushort iTeam);
	_ushort Get_HitCount(_ushort iTeam);


	void Gain_HitAttackStep(_ushort iHitCount, _ushort iTeam);
	void Reset_HitAttackStep(_ushort iTeam);
	_ushort Get_HitAttackStep(_ushort iTeam);


	_bool Get_bSparkingEnable(_ushort iTeam);
	void Set_bSparkingEnable(_bool bSparkingEnable, _ushort iTeam);

	void Gain_KiGuage(_ushort iKi, _ushort iTeam);
	_bool Use_KiGuage(_ushort  irequirementKi, _ushort iTeam);
	_bool Use_KiRealGuage(_ushort  irequirementKi, _ushort iTeam);
	_ushort Get_KiGuage(_ushort iTeam);

	_ushort Get_KiNumber(_ushort iTeam);

	void Stop_CharacterWithoutMe(_ushort iTeam, _ubyte iSlot, _float fTime);
	void Stop_AllCharacter(_float fStopTime);


	_ushort Get_iAliveMemberCount(_ushort iTeam);

	void Stop_AllAttackObject(_float fStopTime);

	void SpecialKiGain(_ushort iKiNumber, _ushort iTeam);


	//이거 하나만 호출하게
	//	void Tag_CharacterAIO(_ubyte iTeam, _ubyte NewCharacterslot);
	_bool Tag_CharacterAIO(_ubyte iTeam, _ubyte NewCharacterslot, _vector vPos);


	//Initalize같은것.  character에 enemy등록과는 다름
	void Regist_Character(_ubyte iTeam, class CCharacter* pCharacter, _ubyte iSlot);
	

	void Set_Character_pEnemy(_ubyte iNewTeam, _ubyte iNewSlot);


	//캐릭터 변경시 호출해서 상대팀에게 enemy를 바꿔줘야됨
	class CCharacter* EnemyInitalize(_ubyte iTeam);

	//사망시 그냥 이거나 호출하고 말자
	_bool Check_NextRoundFromDeathCharacter(_ubyte iTeam, _ubyte NewCharacterslot);
	
	void Set_InvisibleWithoutMe(_ubyte iTeam, _ubyte iCharacterslot);

	void Character_Opening_AIO();
	void Character_Opening_EndForCharacter(_ubyte iTeam);

	void Set_b1VS1(_bool b1vs1);
	bool Check_survivor(_ubyte iTeam, _ubyte NewCharacterslot);

private:
	
public:
	void Set_CharaDesc(_uint iIndex, _ushort iTeam, CUI_Define::PLAYER_SLOT eSlot, wstring PrototypeTag, CUI_Define::PLAYER_ID ePlayerID);
	CHARACREATE_DESC Get_CharaDesc(_uint iIndex){ return m_tCharaCreateDesc[iIndex]; }

private:
	class CCharacter* m_p1TeamCharacter[3] = { nullptr };
	class CCharacter* m_p2TeamCharacter[3] = { nullptr };


	_bool m_bSparkingEnable[2] = { true ,true};
	_uint m_iHitCount[2] = {}; //화면에 뜰 히트수
	
	_uint m_iKiGuage[2] = {};
	_uint m_iKiNumber[2] = {3,3};

	_uint m_iHitAttackStep[2] = {};

	_ubyte m_i1TeamPlayingCharacterIndex = {};
	_ubyte m_i2TeamPlayingCharacterIndex = {};

	CHARACREATE_DESC m_tCharaCreateDesc[4] = {};

	_bool m_bCinematicOpening = false;


	_bool m_b1vs1 = false;


private:
	CGameInstance* m_pGameInstance = { nullptr };

public:

	virtual void Free() override;
};

END