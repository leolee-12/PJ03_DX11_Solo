#pragma once
#include "Base.h"
#include "Game_PKM_Defines.h"
#include "Camera_Defines.h"
#include "Camera_Shake.h"

NS_BEGIN(Game_PKM)
class CCamera_Free;
class CBattle_Manager;
class CCamera_Sequence;

class CCamera_Director final : public CBase
{
	DECLARE_SINGLETON(CCamera_Director)

private:
	CCamera_Director() = default;
	virtual ~CCamera_Director() = default;

public:
	HRESULT Initialize();

	void Bind(CCamera_Free* pCamera, CBattle_Manager* pManager);
	void Unbind();
	static void Try_Unbind();

	void Set_Mode(CAMERA_MODE eMode);
	CAMERA_MODE Get_Mode() const { return m_eMode; }

	void Set_Default_Battle_Pose(const CAMERA_POSE& pose);
	const CAMERA_POSE& Get_Default_Battle_Pose() const { return m_DefaultBattlePose; }

	void Cut_To(const CAMERA_POSE& target);
	void Blend_To(const CAMERA_POSE& target, _float fBlendTime);

	void Play_Sequence(CCamera_Sequence* pSeq);
	_bool Play_Sequence(CAMERA_SEQUENCE_ID eID);

	_float Get_Sequence_Duration(CAMERA_SEQUENCE_ID eID) const;
	void Return_To_BattleDefault(_float fBlendTime);

	void Stop_Sequence();
	_bool Is_Sequence_Playing() const { return nullptr != m_pCurrentSequence; }
	CAMERA_SEQUENCE_ID Get_CurrentSequenceID() const { return m_eCurrentSequenceID; }
	CAMERA_SHOT_TYPE Get_CurrentShotType() const;
	_float Get_CurrentShotElapsed() const;
	_float Get_CurrentShotDuration() const;

	void Start_Shake(_float fPower, _float fFrequency, _float fDuration);
	void Stop_Shake();
	_bool Is_Shake_Active() const { return m_Shake.Is_Active(); }

	void Tick(_float fTimeDelta);

private:
	CCamera_Free* m_pCamera = { nullptr };  // weak
	CBattle_Manager* m_pBattleManager = { nullptr };  // weak

	CAMERA_MODE      m_eMode = { CAMERA_MODE::FIELD };

	CAMERA_POSE      m_CurrentPose = {};
	CAMERA_POSE      m_ActiveShotPose = {};
	CAMERA_POSE      m_DefaultBattlePose = {};
	CAMERA_POSE      m_PreviousPose = {};
	_float           m_fBlendElapsed = { 0.f };
	_float           m_fBlendDuration = { 0.f };
	_bool            m_bBlending = { false };

	CCamera_Sequence* m_pCurrentSequence = { nullptr };
	CAMERA_SEQUENCE_ID m_eCurrentSequenceID = { CAMERA_SEQUENCE_ID::NONE };
	CCamera_Shake m_Shake;

private:
	void Begin_Blend(const CAMERA_POSE& target, _float fBlendTime);
	CAMERA_POSE ApplyBlend(const CAMERA_POSE& basePose, _float fTimeDelta);

	_float3 Resolve_Target_Pos(CAMERA_TARGET_TYPE eType) const;
	CAMERA_POSE Evaluate_Shot(const CAMERA_SHOT_DESC& shot) const;

	void Apply_Pose_To_Camera(const CAMERA_POSE& pose);

private:
	virtual void Free() override;
};

NS_END