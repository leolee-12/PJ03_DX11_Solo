#pragma once
#include "Game_PKM_Defines.h"
#include "Capture_Session.h"
#include "Game_LevelEntry.h"
#include "BattleMsg.h"
#include "Effect_Manager.h"

#include "Level.h"

NS_BEGIN(Engine)
class CUISequence;
NS_END

NS_BEGIN(Game_PKM)
class CCapture_Manager;
class CCapture_Menu;
class CMonsterBall;
class CCamera_Free;
class CActor_CaptureTarget;
class CBattleMsg;

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
	CAPTURE_ENV m_tEnv = {};
	CCamera_Free* m_pCaptureCamera = { nullptr };  // weak - layer owns
	CCapture_Manager* m_pCaptureManager = { nullptr };
	CCapture_Menu* m_pCaptureMenu = { nullptr };   // weak - UI Hub 가 owner
	CUISequence* m_pCursorSeq = { nullptr };      // weak - Add_GameObject_Ex 가 owner
	CMonsterBall* m_pMonsterBall = { nullptr };   // weak - 레이어가 owner
	CActor_CaptureTarget* m_pCaptureTarget = { nullptr };  // weak - layer owns

	_float3 m_vStageCameraStartEye = {};
	_float3 m_vStageCameraStartAt = {};
	_bool   m_bStageCameraActive = false;

	_float3 m_vStageBallAirCenter = {};
	_float3 m_vStageBallGroundCenter = {};
	_float3 m_vStageCameraTargetEye = {};
	_float3 m_vStageCameraTargetAt = {};

	_int    m_iAppliedShakeIndex = { -1 };
	_bool   m_bPrevDidHit = { false };   // capture_hit SFX 를 충돌 순간 1회만 재생하기 위한 엣지 추적

	CBattleMsg* m_pCaptureMsg = { nullptr };      // weak - UI Hub owns
	_bool   m_bCaptureIntroMessageActive = { false };
	_bool   m_bCaptureIntroMessageFinished = { false };
	_float  m_fCaptureIntroMessageDoneElapsed = { 0.f };

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera(WNameID strLayerTag);
	HRESULT Ready_Layer_BackGround(WNameID strLayerTag);
	HRESULT Ready_Layer_Battler(WNameID strLayerTag);
	HRESULT Ready_Layer_Ball(WNameID strLayerTag);
	HRESULT Ready_Layer_UI(WNameID strLayerTag);

	void    Update_AimPose();
	void    Update_IntroBallPose();
	void    Set_AimingCameraControl(_bool bEnabled);
	void	Reset_CaptureCameraPose();
	void    Begin_StageDrop();
	void    Begin_StageCamera();
	void    Apply_StageCameraPose();

	void    Begin_CaptureIntroView();
	void    Tick_CaptureIntroView(_float fTimeDelta);
	_wstring Build_CaptureIntroMessage() const;

	void Tick_CaptureSuccessView();
	void    Begin_CaptureSuccessView();
	_wstring Build_CaptureSuccessMessage() const;

public:
	static CLevel_Capture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LEVEL_ENTRY_DESC* pEntryDesc);

protected:
	virtual void Free() override;
};

NS_END
