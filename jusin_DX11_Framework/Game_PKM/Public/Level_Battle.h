#pragma once
#include "Game_PKM_Defines.h"
#include "Battle_Session.h"
#include "Game_LevelEntry.h"

#include "Level.h"

NS_BEGIN(Engine)
class CUISequence;
NS_END

NS_BEGIN(Game_PKM)
class CBattle_Manager;
class CBattleMsg;
class CEntry;
class CBattlePlate;
class CBattle_MoveMenu;
class CBattle_MsgListener;
class CBattle_CommandMenu;
class CBattle_InputDirector;
class CBattle_PokemonListener;
class CBattle_ExpGainListener;
class CBattle_PlateListener;

class CLevel_Battle final : public CLevel
{
private:
	CLevel_Battle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const BATTLE_ENV& tEnv);
	virtual ~CLevel_Battle() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Bind_Battle_Sources();

	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera(WNameID strLayerTag);
	HRESULT Ready_Layer_BackGround(WNameID strLayerTag);
	HRESULT Ready_Layer_Battler(WNameID strLayerTag);
	HRESULT Ready_Layer_Effect(WNameID strLayerTag);
	HRESULT Ready_Layer_UI(WNameID strLayerTag);

private:
	BATTLE_ENV m_tEnv = {};
	
	CBattleMsg* m_pBattleMsg = { nullptr };
	CUISequence* m_pCursorSeq = { nullptr };
	CEntry* m_pEntry = { nullptr };
	CUISequence* m_pEntrySeq = { nullptr };
	CBattlePlate* m_pBattlePlate = { nullptr };
	CBattle_MoveMenu* m_pMoveMenu = { nullptr };

	CBattle_Manager* m_pBattleManager = { nullptr };
	CBattle_MsgListener* m_pBattleMsgListener = { nullptr };
	CBattle_CommandMenu* m_pCommandMenu = { nullptr };
	CBattle_InputDirector* m_pInputDirector = { nullptr };
	CBattle_PokemonListener* m_pPokemonListeners[g_kBattleSideCount] = { nullptr, nullptr };
	CBattle_ExpGainListener* m_pExpGainListener = { nullptr };
	CBattle_PlateListener* m_pBattlePlateListener = { nullptr };
	TRAINER_DATA m_tOpponentTrainer = {};

public:
	static CLevel_Battle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LEVEL_ENTRY_DESC* pEntryDesc);

protected:
	virtual void Free() override;
};

NS_END