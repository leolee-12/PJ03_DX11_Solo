#pragma once
#include "Actor.h"
#include "Body.h"
#include "Spawn_Defines.h"

NS_BEGIN(Engine)
class CCollider;
class CNavigation;
NS_END

NS_BEGIN(Game_PKM)
class CInteraction_Encounter;

class CActor_WildPokemon final : public CActor
{
public:
	struct ACTOR_WILD_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		WNameID				strBodyProtoTag = { 0 };
		CBody::BODY_DESC*	pBodyDesc = { nullptr };
		_uint				iBodyProtoLevel = ETOUI(LEVEL::STATIC);
		_uint				iComponentLevel = ETOUI(LEVEL::GAMEPLAY);

		// Encounter 페이로드 - Interaction_Encounter 가 Push_Level(Capture) 시 그대로 전달
		_uint              iSpeciesID = { 0 };
		_uint              iLevel = { 1 };

		// S2 추가 - SpawnManager 가 생성 시 채워서 전달
		_uint              iSpawnRectID = { 0 };
		_float3            vSpawnAnchor = {};
		_float             fLeashRadius = { 10.f };
		_uint              iCurrentCellIndex = { INVALID_NAV_CELL };
		SPAWN_RECT_DESC    tSpawnRectDesc = {};

		WNameID            strBodyModelProtoTag = { 0 };
	};

private:
	CActor_WildPokemon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CActor_WildPokemon(const CActor_WildPokemon& Prototype);
	virtual ~CActor_WildPokemon() = default;

public:
	virtual _string Get_TypeName() const override { return "WildPokemonActor"; }
	_uint Get_SpawnRectID() const { return m_iSpawnRectID; }

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	enum class WANDER_STATE : _ubyte { IDLE, MOVING, END };

	CInteraction_Encounter* m_pEncounter = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

	// S3 추가 - Navigation 컴포넌트 (배회 이동 + 셀 추적)
	CNavigation* m_pNavigationCom = { nullptr };

	// S2 추가 - Initialize 에서 desc 로부터 복사 (사용처는 S3 의 배회 로직)
	_uint           m_iSpawnRectID = { 0 };
	_float3         m_vSpawnAnchor = {};
	_float          m_fLeashRadius = { 10.f };
	_uint           m_iCurrentCellIndex = { INVALID_NAV_CELL };
	SPAWN_RECT_DESC m_tSpawnRectDesc = {};

	// S3 추가 - 배회 상태 머신
	WANDER_STATE m_eWanderState = { WANDER_STATE::IDLE };
	_float       m_fWanderTimer = { 0.f };
	_float       m_fMoveDuration = { 0.f };          // MOVING 상태 누적 시간 (루트모션 delta=0 케이스 타임아웃 용)
	_float3      m_vMoveTarget = {};
	_uint        m_iTargetCellIndex = { INVALID_NAV_CELL };
	_bool        m_bUseRectWander = { true };
	WNameID      m_strBodyModelProtoTag = { 0 };

private:
	HRESULT Ready_Components(const ACTOR_WILD_DESC* pDesc);
	HRESULT Ready_PartObjects(const ACTOR_WILD_DESC* pDesc);
	void    Cache_Members();

	virtual void Tick_Movement(_float fTimeDelta) override;
	_bool   Choose_WanderTarget();


public:
	static CActor_WildPokemon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

private:
	virtual void Free() override;
};

NS_END