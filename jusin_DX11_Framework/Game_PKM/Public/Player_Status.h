#pragma once
#include "GameObject.h"
#include "Player_Data.h"
#include "Battle_Data.h"

NS_BEGIN(Game_PKM)

class CPlayer_Status final : public CGameObject
{
protected:
	CPlayer_Status(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_Status(const CPlayer_Status& Prototype);
	virtual ~CPlayer_Status() = default;

public:
	virtual _string Get_TypeName() const override { return "PlayerState"; }

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	PARTY& Get_Party() { return m_tParty; }
	const PARTY& Get_Party() const { return m_tParty; }

	POKEDEX& Get_Pokedex() { return m_tPokedex; }
	const POKEDEX& Get_Pokedex() const { return m_tPokedex; }

	BOX& Get_Box() { return m_tBox; }
	const BOX& Get_Box() const { return m_tBox; }

	POKEDEX_STATE Get_DexState(_uint iDexNo) const;
	_bool Mark_DexSeen(_uint iDexNo);
	_bool Mark_DexCaught(_uint iDexNo);
	_bool Acquire_Pokemon(_uint iSpeciesID, _ubyte iLevel, _uint iCapturedAtZoneID);

	_uint Get_TrainerID() const { return m_iTrainerID; }
	void Set_TrainerID(_uint iTrainerID) { m_iTrainerID = iTrainerID; }

	_uint Get_Money() const { return m_iMoney; }
	void Set_Money(_uint iMoney) { m_iMoney = iMoney; }

	_uint Get_BadgeFlags() const { return m_iBadgeFlags; }
	void Set_BadgeFlags(_uint iBadgeFlags) { m_iBadgeFlags = iBadgeFlags; }

	WNameID Get_TrainerBodyProtoTag() const { return m_strTrainerBodyProtoTag; }
	WNameID Get_TrainerModelProtoTag() const { return m_strTrainerModelProtoTag; }
	WNameID Get_TrainerShaderProtoTag() const { return m_strTrainerShaderProtoTag; }

private:
	PARTY m_tParty = {};
	BOX m_tBox = {};
	POKEDEX m_tPokedex = {};
	_uint m_iTrainerID = { 1 };
	_uint m_iMoney = {};
	_uint m_iBadgeFlags = {};

	WNameID m_strTrainerBodyProtoTag = {};
	WNameID m_strTrainerModelProtoTag = {};
	WNameID m_strTrainerShaderProtoTag = {};

public:
	static CPlayer_Status* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

private:
	virtual void Free() override;
};

NS_END