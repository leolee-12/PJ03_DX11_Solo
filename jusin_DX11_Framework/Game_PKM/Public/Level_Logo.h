#pragma once
#include "Game_PKM_Defines.h"
#include "Level.h"

NS_BEGIN(Engine)
class CUISequence;
NS_END

NS_BEGIN(Game_PKM)
class CEffect_Star;
class CMonster;

class CLevel_Logo : public CLevel
{
private:
	CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Logo() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CMonster* m_pLogoMonster = { nullptr }; // weak

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera(WNameID strLayerTag);
	HRESULT Ready_Layer_BackGround(WNameID strLayerTag);
	HRESULT Ready_Layer_Monster(WNameID strLayerTag);
	HRESULT Ready_Layer_UI(WNameID strLayerTag);

	CUISequence* Create_TitleSequence();
	CEffect_Star* Create_StarEffect(void* pStarDesc);
	void Bind_TitleSlots(CUISequence* pTitleUI, vector<CEffect_Star*>& Effects);

public:
	static CLevel_Logo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

protected:
	virtual void Free() override;
};

NS_END