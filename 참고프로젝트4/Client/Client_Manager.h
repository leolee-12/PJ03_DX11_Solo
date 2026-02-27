#pragma once

#include "Client_Defines.h"
#include "Base.h"

#include "Level_Test_Defines.h"

BEGIN(Engine)
class CGameObject;
class CGameInstance;
class CLight;
END


BEGIN(Client)

typedef struct tagDirKeyInfo
{
	_uint  iDirKey = { 0 };
	_float fRadianFromBaseDir = { 0.f };
	_bool  bDirKey_EventOn = { false };
}DIRKEYINFO;

typedef struct tagTargetInfo
{
	weak_ptr<CGameObject> pTarget;
	_float fDistance = { 0.f };
	_float3 vTargetPos = { 0.f,0.f,0.f };
	_float3 vDirToTarget = { 0.f,0.f,0.f };
}TARGETINFO;

class CClient_Manager final : public CBase
{
	DECLARE_SINGLETON(CClient_Manager)

private:
	CClient_Manager();
	virtual ~CClient_Manager() = default;

public:
	HRESULT Initialize_Client(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext>pContext);

public:
	void Tick_Client();
	void Clear(_uint ILevelIndex);

private:
	void Register_ClientKey();

/* 키 입력 관련*/
public:
	void Input_DirKey();
	void Input_Key();

private:
	DIRKEYINFO m_DirKeyInfo = {};
	_bool m_bDirKey_Input_Enable = { true };
	_bool m_bKey_Input_Enable = { true };


/*플레이어, 몬스터 관련*/
public:
	TARGETINFO Find_TargetMonster(PLAYER_TYPE ePlayerType, _uint iCloseOrder = 0);
	TARGETINFO Find_TargetPlayer(weak_ptr<CGameObject> pMonster, PLAYER_TYPE type = PLAYER_END);
	TARGETINFO Get_TargetMonster(PLAYER_TYPE ePlayerType);
	TARGETINFO Update_TargetInfo(weak_ptr<CGameObject>pActor, TARGETINFO& TargetInfo);
	void Register_Player(PLAYER_TYPE eType, weak_ptr<class CPlayer> pPlayer);

	void Switching_Player();
	void Switching_AttackMode();
	_bool IsPlayable(PLAYER_TYPE eType) { return m_ePlayerType == eType; };

	void Set_PlayerPos(_float3 vCloudPos,_float3 vAerithPos);

	_bool Get_AttackMode() { return m_bAttackMode; };

private:
#if TEST_PLAYER == 0
	PLAYER_TYPE m_ePlayerType = CLOUD;
#elif TEST_PLAYER == 1
	PLAYER_TYPE m_ePlayerType = AERITH;
#endif
	shared_ptr<class CPlayer> m_pPlayer[PLAYER_END];
	TARGETINFO m_TargetMonsterInfo;
	_uint	m_iTargetMonsterIndex = { 0 };
	_float  m_fMaxDistanceToTarget = { 15.f };
	_bool	m_bAttackMode = { false };
	_bool   m_bBattleMode_Natural = { true };

/* TimeDelta 조절*/
public:
	void Register_Event_AdjustTimeDelta(weak_ptr<CBase> _pSubscriber, _float fDuration, _float fAdjustTimeDelta);
	void Set_AdjustTimeDelta(_float fAdjustTimeDelta);

private:
	_float m_fAdjustTimeDelta_AccTime = 0.f;
	_float m_fAdjustTimeDelta_Duration = 0.f;
	_float m_fAdjustTimeDelta= 0.f;


/* 게임 정지/재생 조절*/
public:
	void Play_Game();
	void Pause_Game();
	void Switch_Game();
/* 모션 블러 설정*/
public:
	void Register_Event_MotionBlur(weak_ptr<CBase> _pSubscriber, _float fDuration, _bool bUseActorCenter = true, _float vBlurScale = 0.125f, _float2 vCenter = { 0.5f,0.5f });
	void TurnOn_MotionBlur(_bool bUseActorCenter = true, _float vBlurScale = 0.125f, _float2 vCenter = { 0.5f,0.5f },_float fBlurAmount = 64.f);
	void TurnOff_MotionBlur();

private:
	_float m_fMotionBlur_AccTime = 0.f;
	_float m_fMotionBlur_Duration = 0.f;

/* 레디얼 블러*/
public:
	void	TurnOn_RadialBlur(_float3 vCenter, _float Intensity = 0.5f);
	void	TurnOff_RadialBlur();


/* UI Command, 플레이어 상태 관련 */
private:
	_bool	m_bisBattleMode = { false };  // True > BattleMode, False > Non-BattleMode
	_bool	m_bisAssertMode = { true }; // True > AssertMode,  False > BraveMode
	_int	m_iCommandResult = { 0 }; // 사용 후 0 (None)으로 Set

	_bool   m_bisLockOnToMonster = { false };
	_bool   m_bisMainPopUpOpen = { false };
	_bool	m_bisLogoEnd = { false };

	_bool	m_bLockOnEnable = { true };
	
private:
	_int	m_iPlayerGIL = { 190 };
	_int	m_iItemCount[5] = { 0,4,0,0,0 }; //0포션 1하이포션 2에텔 3기념주화 4기사도교본

	/* 폭탄 부착 판단 여부 */
private:
	_bool m_bomb_attachment = { false };


public:
	void Update_TargetLockOn();
	_int Get_PlayerItemCount(_int ItemIdx = 1) { return m_iItemCount[ItemIdx]; }
	void Set_PlayerItemCount(_int NewCount, _int ItemIdx = 1) { m_iItemCount[ItemIdx] = NewCount; }
	void Add_PlayerGIL(_int AddGIL = 10) { m_iPlayerGIL += AddGIL; }
	void Add_PlayerItemCount(_int AddCount = 1, _int ItemIdx = 1) { m_iItemCount[ItemIdx] += AddCount; }

	/*플레이어 레이어 추가*/
public:
	void Add_Player_In_Layer(LEVEL eLevel, _float3 vCloudPos, _float3 vAerithPos);

public:
	GETSET_EX1(weak_ptr<class CPlayer>, m_pPlayer[m_ePlayerType], PlayerPtr, GET_C)
	weak_ptr<class CPlayer>		Get_PlayerPtr(PLAYER_TYPE ePlayerType) { return m_pPlayer[ePlayerType]; }

	GETSET_EX2(PLAYER_TYPE, m_ePlayerType, PlayerType, GET_C_REF, SET)
	GETSET_EX2(DIRKEYINFO, m_DirKeyInfo, DirKeyInfo, GET_C_REF, SET)
	GETSET_EX2(_bool, m_bDirKey_Input_Enable, DirKey_Input_Enable, GET_C_REF, SET)
	GETSET_EX2(_bool, m_bKey_Input_Enable, bKey_Input_Enable, GET_C_REF, SET)

	GETSET_EX2(_bool, m_bisBattleMode, BattleMode, GET_C_REF, SET)
	GETSET_EX2(_bool, m_bisAssertMode, AssertMode, GET_C_REF, SET)
	GETSET_EX2(_bool, m_bisMainPopUpOpen, MainPopUpOpen, GET_C_REF, SET)
	GETSET_EX2(_bool, m_bisLockOnToMonster, LockOnToMonster, GET_C_REF, SET)
	GETSET_EX2(_bool, m_bisLogoEnd, LogoEnd, GET_C_REF, SET)

	GETSET_EX2(_int, m_iCommandResult, Command_Result, GET_C_REF, SET)

	GETSET_EX2(_int, m_iPlayerGIL, PlayerGIL, GET_C_REF, SET)

	GETSET_EX2(_bool, m_bomb_attachment, attachment, GET_C_REF, SET)
	

	GETSET_EX2(_bool, m_bBattleMode_Natural, BattleMode_Natural, GET_C_REF, SET)

	void	Set_Player_Full_Limit();

public:

private:	
	LEVEL		m_eNowStageLevel = { LEVEL_STAGE1 };
	

	

private:
	CGameInstance* m_pGameInstance = nullptr;
	ComPtr<ID3D11Device> m_pDevice;
	ComPtr<ID3D11DeviceContext> m_pContext;
 
public:
	void Free();
};

END

