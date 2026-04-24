#pragma once
#include "UIContainer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CUISequence : public CUIContainer
{
public:
	struct UISEQ_STEP
	{
		UI_SEQ_STEP_KIND eKind{ UI_SEQ_STEP_KIND::WAIT };
		class CUIObject* pTarget{ nullptr };     // 비소유
		_wstring strAnimName{};
		_float fWaitSec{ 0.f };
		_bool bVisible{ true };
		function<void()> fnCallback{};
		_bool bJoinPrev{ false };       // true면 이전 step과 동일 프레임 내 연쇄
	};

protected:
	CUISequence(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUISequence(const CUISequence& Prototype);
	virtual ~CUISequence() = default;

public:
	virtual _string Get_TypeName() const override { return "UISequence"; }

	// ── 조회 ──
	const vector<UISEQ_STEP>& Get_Steps() const { return m_Steps; }
	_int   Get_Cursor() const { return m_iCursor; }
	_float Get_Timer()  const { return m_fTimer; }

	// ── 편집(재생 중에는 모두 false 반환) ──
	_bool  Insert_Step(_int iIndex, const UISEQ_STEP& step);
	_bool  Remove_Step(_int iIndex);
	_bool  Move_Step(_int iFrom, _int iTo);
	_bool  Update_Step(_int iIndex, const UISEQ_STEP& step);

	// ── 스크럽(Play 중에만 의미) ──
	void   Seek_ToStep(_int iIndex);
	void   Set_Timer(_float fTimer);

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Update(_float fTimeDelta) override;

	void  Append(const UISEQ_STEP& step);	// bJoinPrev=false 강제
	void  Join(const UISEQ_STEP& step);		// bJoinPrev=true 강제
	void  Play();
	void  Stop();
	_bool Is_Playing() const { return m_bPlaying; }
	void  Clear_Timeline();					// Play 중이 아닐 때만 호출

private:
	vector<UISEQ_STEP> m_Steps;
	_int   m_iCursor = { -1 };
	_float m_fTimer = { 0.f };
	_bool  m_bPlaying = { false };
	_bool  m_bStepStarted = { false };

public:
	static CUISequence* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

protected:
	virtual void Free() override;
};

NS_END