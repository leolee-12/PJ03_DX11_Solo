#pragma once
#include "IBattleAction_Step.h"
#include "Camera_Defines.h"

NS_BEGIN(Game_PKM)

class SCamera_PlaySequence final : public IBattleAction_Step
{
private:
	SCamera_PlaySequence();
	virtual ~SCamera_PlaySequence() = default;

public:
	HRESULT Initialize(CAMERA_SEQUENCE_ID eID);

	virtual void  OnEnter(const BATTLE_CONTEXT& ctx) override;
	virtual void  Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta) override;
	virtual _bool Is_Complete(const BATTLE_CONTEXT& ctx) const override;

private:
	CAMERA_SEQUENCE_ID m_eID = { CAMERA_SEQUENCE_ID::NONE };
	_float m_fDuration = { 0.f };
	_float m_fElapsed = { 0.f };
	_bool  m_bRequested = { false };

public:
	static SCamera_PlaySequence* Create(CAMERA_SEQUENCE_ID eID);

private:
	virtual void Free() override;
};

class SCamera_Shake final : public IBattleAction_Step
{
private:
	SCamera_Shake();
	virtual ~SCamera_Shake() = default;

public:
	HRESULT Initialize(_float fPower, _float fFrequency, _float fDuration);

	virtual void  OnEnter(const BATTLE_CONTEXT& ctx) override;
	virtual void  Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta) override;
	virtual _bool Is_Complete(const BATTLE_CONTEXT& ctx) const override;

private:
	_float m_fPower = { 0.f };
	_float m_fFrequency = { 0.f };
	_float m_fDuration = { 0.f };
	_float m_fElapsed = { 0.f };
	_bool  m_bStarted = { false };

public:
	static SCamera_Shake* Create(_float fPower, _float fFrequency, _float fDuration);

private:
	virtual void Free() override;
};

class SCamera_Return final : public IBattleAction_Step
{
private:
	SCamera_Return();
	virtual ~SCamera_Return() = default;

public:
	HRESULT Initialize(_float fBlendTime);

	virtual void  OnEnter(const BATTLE_CONTEXT& ctx) override;
	virtual void  Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta) override;
	virtual _bool Is_Complete(const BATTLE_CONTEXT& ctx) const override;

private:
	_float m_fBlendTime = { 0.f };
	_float m_fElapsed = { 0.f };

public:
	static SCamera_Return* Create(_float fBlendTime);

private:
	virtual void Free() override;
};

NS_END