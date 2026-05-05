#pragma once
#include "Game_PKM_Defines.h"
#include "Game_BattleSession.h"
#include "Game_LevelEntry.h"

#include "Level.h"

NS_BEGIN(Game_PKM)
class CBattle_Manager;

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

	HRESULT Ready_Debug_WildOpponent();

private:
	BATTLE_ENV m_tEnv = {};
	CBattle_Manager* m_pBattleManager = { nullptr };
	POKEMON_INSTANCE m_tDebugWildOpponent = {};

public:
	static CLevel_Battle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LEVEL_ENTRY_DESC* pEntryDesc);

protected:
	virtual void Free() override;
};

NS_END