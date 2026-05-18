#pragma once
#include "Base.h"
#include "Battle_Data.h"

NS_BEGIN(Game_PKM)

class CTrainerData_Manager : public CBase
{
	DECLARE_SINGLETON(CTrainerData_Manager)

private:
	CTrainerData_Manager();
	virtual ~CTrainerData_Manager() = default;

public:
	HRESULT Initialize();
	const TRAINER_DATA* Find_Trainer(_uint iTrainerID) const;

private:
	HRESULT Load_BuiltinSeed();
	HRESULT Validate_Seed() const;

private:
	std::unordered_map<_uint, TRAINER_DATA> m_TrainerTable;
	_bool m_bInitialized = { false };

private:
	virtual void Free() override;
};

NS_END