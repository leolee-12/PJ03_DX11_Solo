#pragma once
#include "Game_PKM_Defines.h"
#include "Capture_Session.h"
#include "Game_LevelEntry.h"

#include "Level.h"

NS_BEGIN(Game_PKM)
class CCapture_Manager;

class CLevel_Capture final : public CLevel
{
private:
	CLevel_Capture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CAPTURE_ENV& tEnv);
	virtual ~CLevel_Capture() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer_Camera(WNameID strLayerTag);

private:
	CAPTURE_ENV m_tEnv = {};
	CCapture_Manager* m_pCaptureManager = { nullptr };

public:
	static CLevel_Capture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LEVEL_ENTRY_DESC* pEntryDesc);

protected:
	virtual void Free() override;
};

NS_END