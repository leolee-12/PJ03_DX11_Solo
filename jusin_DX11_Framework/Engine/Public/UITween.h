#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CUITween : public CBase
{
public:
	struct UITWEEN_DESC
	{
		UI_TWEEN_TARGET eTarget{ UI_TWEEN_TARGET::END };
		UI_EASE eEase{ UI_EASE::LINEAR };
		UI_TWEEN_LOOP eLoop{ UI_TWEEN_LOOP::NONE };
		_float fStart{ 0.f };
		_float fEnd{ 0.f };
		_float fDuration{ 1.f };
		_float fDelay{ 0.f };
	};

private:
	CUITween() = default;
	~CUITween() = default;
	
public:	
	UI_TWEEN_TARGET Get_Target() const { return m_tDesc.eTarget; }

	HRESULT Initialize(class CUIObject* pOwner, const UITWEEN_DESC& tDesc);
	void Tick(_float fTimeDelta, class CUIObject* pOwner);
	_bool Is_Finished() const;
	void Stop();

private:
	CUIObject* m_pOwner = { nullptr };  // base ops용
	CUIProgressBar* m_pOwnerAsBar = { nullptr };  // FILL_AMOUNT 전용, 해당 시에만 set

	UITWEEN_DESC m_tDesc;
	_float m_fElapsed = { 0.f };
	_bool m_bFinished = { false };
	_bool m_bForward = { true };   // PINGPONG 방향

private:
	_float Evaluate_Ease(_float t) const;
	void Apply_To_Owner(_float fValue);

public:
	static CUITween* Create(class CUIObject* pOwner, const UITWEEN_DESC& tDesc);

private:
	virtual void Free() override;
};

NS_END