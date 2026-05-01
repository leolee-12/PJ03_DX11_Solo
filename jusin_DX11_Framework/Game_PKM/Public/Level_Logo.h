#pragma once
#include "Game_PKM_Defines.h"
#include "Level.h"

NS_BEGIN(Engine)
class CUIImage;
NS_END

NS_BEGIN(Game_PKM)
class CEffect_Star;

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
	CUIImage* m_pTestTargetUI = { nullptr };
	CEffect_Star* m_pTestStarEffect = { nullptr };

private:
	HRESULT Ready_Layer_BackGround(WNameID strLayerTag);
	HRESULT Ready_Layer_Monster(WNameID strLayerTag);
	HRESULT Ready_Layer_UI(WNameID strLayerTag);

public:
	static CLevel_Logo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

protected:
	virtual void Free() override;
};

NS_END