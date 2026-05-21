#pragma once
#include "Actor.h"
#include "Body.h"
#include "Spawn_Defines.h"
#include "Battle_AnimDef.h"

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
		WNameID		strBodyProtoTag = { 0 };
		WNameID		strBodyModelProtoTag = { 0 };
		_uint		iBodyProtoLevel = ETOUI(LEVEL::STATIC);
		_uint		iComponentLevel = ETOUI(LEVEL::GAMEPLAY);
		CBody::BODY_DESC*	pBodyDesc = { nullptr };

		SPAWN_RECT_DESC	tSpawnRectDesc = {};
		
		_uint		iSpeciesID = { 0 };
		_uint		iLevel = { 1 };

		_uint		iSpawnRectID = { 0 };
		_float3		vSpawnAnchor = {};
		_float		fLeashRadius = { 10.f };
		_uint		iCurrentCellIndex = { INVALID_NAV_CELL };

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

	// Navigation 컴포넌트 (배회 이동 + 셀 추적)
	CNavigation* m_pNavigationCom = { nullptr };

	// 배회 상태 머신
	_uint           m_iSpawnRectID = { 0 };
	_float3         m_vSpawnAnchor = {};
	_float          m_fLeashRadius = { 10.f };
	_uint           m_iCurrentCellIndex = { INVALID_NAV_CELL };
	SPAWN_RECT_DESC m_tSpawnRectDesc = {};

	WANDER_STATE m_eWanderState = { WANDER_STATE::IDLE };
	_float       m_fWanderTimer = { 0.f };
	_float       m_fMoveDuration = { 0.f };          // MOVING 상태 누적 시간 (루트모션 delta=0 케이스 타임아웃 용)
	_float3      m_vMoveTarget = {};
	_uint        m_iTargetCellIndex = { INVALID_NAV_CELL };

	_bool        m_bUseRectWander = { true };
	WNameID      m_strBodyModelProtoTag = { 0 };

	_bool        m_bIdleVariantPlaying = { false };
	ANIM_KIND    m_eIdleVariantKind = { ANIM_KIND::IDLE };
	_float       m_fIdleVariantElapsed = { 0.f };
	_float       m_fNextIdleVariantTime = { 0.f };

private:
	HRESULT Ready_Components(const ACTOR_WILD_DESC* pDesc);
	HRESULT Ready_PartObjects(const ACTOR_WILD_DESC* pDesc);
	void    Cache_Members();

	virtual void Tick_Movement(_float fTimeDelta) override;
	_bool   Choose_WanderTarget();

	void    Schedule_NextIdleVariant();
	_bool   Try_StartIdleVariant();
	void    Finish_IdleVariant();


public:
	static CActor_WildPokemon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

private:
	virtual void Free() override;
};

NS_END