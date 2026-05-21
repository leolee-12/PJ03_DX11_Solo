#pragma once
#include "UIContainer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CUISequence : public CUIContainer
{
public:
	enum class UISEQ_SLOT_CATEGORY { EFFECT, BGM, SFX, SIGNAL, END };

	struct UISEQ_EVENT_CONTEXT
	{
		CUIObject* pTarget = nullptr;    // 비소유
		_string strSlotId{};
		_string strTargetId{};
		_float fSequenceTime = 0.f;
	};

	using UISEQ_SLOT_FUNC = function<void(const UISEQ_EVENT_CONTEXT&)>;

	struct UISEQ_SLOT_BINDING
	{
		UISEQ_SLOT_CATEGORY eCategory{ UISEQ_SLOT_CATEGORY::END };
		UISEQ_SLOT_FUNC fnFire{};
		UISEQ_SLOT_FUNC fnRelease{};
	};

	struct UISEQ_STEP
	{
		UI_SEQ_STEP_KIND eKind{ UI_SEQ_STEP_KIND::WAIT };
		class CUIObject* pTarget{ nullptr };     // 비소유

		_string strTargetId{};                   // 슬롯/디버그용 원본 targetId
		_string strSlotId{};                     // 외부 슬롯 ID

		_wstring strAnimName{};
		_float fWaitSec{ 0.f };
		_bool bVisible{ true };
		function<void()> fnCallback{};
		_bool bJoinPrev{ false };                // true면 이전 step과 동일 프레임 내 연쇄

		_bool bRequired{ false };                // 미바인딩/타겟 누락 시 강한 경고용
	};

	struct UISEQUENCE_DESC : public CUIContainer::UICONTAINER_DESC
	{
		_string strPath{};
		_uint iProtoLevel{ INVALID_INDEX };
	};

protected:
	CUISequence(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUISequence(const CUISequence& Prototype);
	virtual ~CUISequence() = default;

public:
	virtual _string Get_TypeName() const override { return "UISequence"; }
	CUIObject* Find_Widget(const _string& strId) const;

	// -- 조회 --
	const vector<UISEQ_STEP>& Get_Steps() const { return m_Steps; }
	_int   Get_Cursor() const { return m_iCursor; }
	_float Get_Timer()  const { return m_fTimer; }

	// -- 편집(재생 중에는 모두 false 반환) --
	_bool  Insert_Step(_int iIndex, const UISEQ_STEP& step);
	_bool  Remove_Step(_int iIndex);
	_bool  Move_Step(_int iFrom, _int iTo);
	_bool  Update_Step(_int iIndex, const UISEQ_STEP& step);

	// -- 스크럽(Play 중에만 의미) --
	void   Seek_ToStep(_int iIndex);
	void   Set_Timer(_float fTimer);

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Update(_float fTimeDelta) override;

	_bool Append(const UISEQ_STEP& step);	// bJoinPrev=false 강제
	_bool Join(const UISEQ_STEP& step);		// bJoinPrev=true 강제
	void Play();
	void Stop();
	_bool Is_Playing() const { return m_bPlaying; }
	_bool Clear_Timeline();					// Play 중이 아닐 때만 호출

	void Bind_Slot(
		const _string& strSlotId,
		UISEQ_SLOT_CATEGORY eCategory,
		UISEQ_SLOT_FUNC fnFire,
		UISEQ_SLOT_FUNC fnRelease = nullptr);

	void Bind_Effect(
		const _string& strSlotId,
		UISEQ_SLOT_FUNC fnFire,
		UISEQ_SLOT_FUNC fnRelease = nullptr);

	void Bind_BGM(
		const _string& strSlotId,
		UISEQ_SLOT_FUNC fnFire,
		UISEQ_SLOT_FUNC fnRelease = nullptr);

	void Bind_SFX(
		const _string& strSlotId,
		UISEQ_SLOT_FUNC fnFire);

	void Bind_Signal(
		const _string& strSlotId,
		UISEQ_SLOT_FUNC fnFire);

	void Unbind_Slot(const _string& strSlotId);
	void Clear_Bindings();

private:
	vector<UISEQ_STEP> m_Steps;
	_int   m_iCursor = { -1 };
	_float m_fTimer = { 0.f };
	_float m_fSequenceTime = { 0.f };
	_bool  m_bPlaying = { false };
	_bool  m_bStepStarted = { false };

	unordered_map<_string, class CUIObject*> m_mapById;   // weak (child가 owner)
	unordered_map<_string, UISEQ_SLOT_BINDING> m_SlotBindings;
	unordered_set<_string> m_ActiveReleaseSlots;

private:
	HRESULT Build_FromFile(const _string& strPath, _uint iProtoLevel);
	void Fire_Slot(const UISEQ_STEP& s);
	void Release_Slot(const UISEQ_STEP& s);
	void Release_All_ActiveSlots();

	_bool Is_SlotPlayKind(UI_SEQ_STEP_KIND eKind) const;
	_bool Is_SlotStopKind(UI_SEQ_STEP_KIND eKind) const;

public:
	static CUISequence* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

protected:
	virtual void Free() override;
};

NS_END