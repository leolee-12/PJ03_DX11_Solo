#pragma once
#include "Game_PKM_Defines.h"
#include "Capture_Session.h"
#include "Game_LevelEntry.h"

#include "Level.h"

NS_BEGIN(Engine)
class CUISequence;
NS_END

NS_BEGIN(Game_PKM)
class CCapture_Manager;
class CCapture_Menu;
class CMonsterBall;
class CActor_CaptureTarget;

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
	HRESULT Ready_Layer_Battler(WNameID strLayerTag);
	HRESULT Ready_Layer_Ball(WNameID strLayerTag);
	HRESULT Ready_Layer_UI(WNameID strLayerTag);

private:
	CAPTURE_ENV m_tEnv = {};
	CCapture_Manager* m_pCaptureManager = { nullptr };
	CCapture_Menu* m_pCaptureMenu = { nullptr };   // weak — UI Hub 가 owner
	CUISequence* m_pCursorSeq = { nullptr };      // weak — Add_GameObject_Ex 가 owner
	CMonsterBall* m_pMonsterBall = { nullptr };   // weak — 레이어가 owner
	CActor_CaptureTarget* m_pCaptureTarget = { nullptr };  // weak - layer owns

public:
	static CLevel_Capture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LEVEL_ENTRY_DESC* pEntryDesc);

protected:
	virtual void Free() override;
};

NS_END
