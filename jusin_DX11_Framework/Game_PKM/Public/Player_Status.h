#pragma once
#include "GameObject.h"
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

	_uint Get_TrainerID() const { return m_iTrainerID; }
	void Set_TrainerID(_uint iTrainerID) { m_iTrainerID = iTrainerID; }

	_uint Get_Money() const { return m_iMoney; }
	void Set_Money(_uint iMoney) { m_iMoney = iMoney; }

	_uint Get_BadgeFlags() const { return m_iBadgeFlags; }
	void Set_BadgeFlags(_uint iBadgeFlags) { m_iBadgeFlags = iBadgeFlags; }

private:
	PARTY m_tParty = {};
	_uint m_iTrainerID = { 1 };
	_uint m_iMoney = {};
	_uint m_iBadgeFlags = {};

public:
	static CPlayer_Status* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

private:
	virtual void Free() override;
};

NS_END