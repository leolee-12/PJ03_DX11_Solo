#pragma once
#include "Game_PKM_Defines.h"
#include "Level.h"

NS_BEGIN(Engine)
class CUITween;
NS_END


NS_BEGIN(Game_PKM)

class CLevel_GamePlay : public CLevel
{
private:
	CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_GamePlay() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera(WNameID strLayerTag);
	HRESULT Ready_Layer_BackGround(WNameID strLayerTag);
	HRESULT Ready_Layer_Player(WNameID strLayerTag);
	HRESULT Ready_Layer_Monster(WNameID strLayerTag);
	HRESULT Ready_Layer_UI(WNameID strLayerTag);

private:
	CUITween* m_pTestTween{ nullptr };

public:
	static CLevel_GamePlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

protected:
	virtual void Free() override;
};

NS_END