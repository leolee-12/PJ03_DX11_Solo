#pragma once
#include "Interaction.h"
#include "Battle_Session.h"

NS_BEGIN(Game_PKM)

class CInteraction_DialogueBattle final : public CInteraction
{
public:
	struct INTERACTION_DIALOGUE_BATTLE_DESC
	{
		_wstring         strDialogueKey;
		_uint            iTrainerID = { 0 };
		ENVIRONMENT_TYPE eEnvironment = { ENVIRONMENT_TYPE::GRASS };
		BATTLE_RULE      eRule = { BATTLE_RULE::TRAINER_SINGLE };
		_uint            iBGResourceID = { 0 };
		_uint            iZoneID = { 0 };
		_bool            bCanRun = { false };
		_bool            bCanCapture = { false };
		_bool            bExpGain = { true };
		_bool            bOneShot = { true };
	};

private:
	CInteraction_DialogueBattle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInteraction_DialogueBattle(const CInteraction_DialogueBattle& Prototype);
	virtual ~CInteraction_DialogueBattle() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual _bool Supports(INTERACTION_EVENT eEvent) const override;
	virtual _bool CanInteract(const INTERACTION_CONTEXT& ctx) const override;
	virtual void  Execute(const INTERACTION_CONTEXT& ctx) override;
	virtual _int  Get_Priority(const INTERACTION_CONTEXT& ctx) const override;
	virtual void  Tick(_float fTimeDelta) override;

private:
	enum class RUN_STATE : _ubyte { IDLE, DIALOGUE, DONE };

	_wstring   m_strDialogueKey;
	_uint      m_iTrainerID = { 0 };
	BATTLE_ENV m_tBattleEnv = {};
	_bool      m_bOneShot = { true };
	_bool      m_bAlreadyBattled = { false };
	RUN_STATE  m_eRunState = { RUN_STATE::IDLE };

public:
	static CInteraction_DialogueBattle* Create(ID3D11Device* pDevice, ID3D11DeviceContext*
		pContext);
	virtual CComponent* Clone(void* pArg) override;

private:
	virtual void Free() override;
};

NS_END