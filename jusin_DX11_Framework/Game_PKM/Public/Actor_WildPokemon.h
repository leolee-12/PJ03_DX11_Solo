#pragma once
#include "Actor.h"
#include "Body.h"

NS_BEGIN(Engine)
class CCollider;
NS_END

NS_BEGIN(Game_PKM)
class CInteraction_Encounter;

class CActor_WildPokemon final : public CActor
{
public:
	struct ACTOR_WILD_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		WNameID            strBodyProtoTag = { 0 };
		CBody::BODY_DESC* pBodyDesc = { nullptr };
		_uint              iBodyProtoLevel = ETOUI(LEVEL::STATIC);
		_uint              iComponentLevel = ETOUI(LEVEL::GAMEPLAY);

		// Encounter 페이로드 — Interaction_Encounter 가 Push_Level(Capture) 시 그대로 전달
		_uint              iSpeciesID = { 0 };
		_uint              iLevel = { 1 };
	};

private:
	CActor_WildPokemon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CActor_WildPokemon(const CActor_WildPokemon& Prototype);
	virtual ~CActor_WildPokemon() = default;

public:
	virtual _string Get_TypeName() const override { return "WildPokemonActor"; }

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CInteraction_Encounter* m_pEncounter = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

private:
	HRESULT Ready_Components(const ACTOR_WILD_DESC* pDesc);
	HRESULT Ready_PartObjects(const ACTOR_WILD_DESC* pDesc);
	void    Cache_Members();

public:
	static CActor_WildPokemon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

private:
	virtual void Free() override;
};

NS_END