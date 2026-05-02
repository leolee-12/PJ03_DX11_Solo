#pragma once
#include "Base.h"
#include "Editor_Defines.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Editor)

class CUIEditorSession : public CBase
{
public:
	enum class VPMODE { SCENE, UI_LAYOUT, UI_ANIM, END };

private:
	CUIEditorSession(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CUIEditorSession() = default;

public:
	// document accessors
	const UISEQ_DOC& Get_Doc() const { return m_Doc; }
	UISEQ_DOC& Get_DocMutable() { return m_Doc; }   // 패널이 직접 편집할 때
	const _string& Get_DocPath() const { return m_strDocPath; }
	const _string& Get_Status()  const { return m_strStatus; }
	void  Set_DocPath(const _string& s) { m_strDocPath = s; }
	_bool Is_Dirty() const { return m_bDirty; }
	void Set_DocCanvas(_float fWidth, _float fHeight, UI_SCALE_POLICY eScalePolicy);

	// selection accessors
	_int Get_SelectedWidget()    const { return m_iSelectedWidget; }
	_int Get_SelectedAnimation() const { return m_iSelectedAnimation; }
	_int Get_SelectedTrack()     const { return m_iSelectedTrack; }
	_int Get_SelectedStep()      const { return m_iSelectedStep; }
	void Set_SelectedWidget(_int i) { m_iSelectedWidget = i; Normalize_Selection(); }
	void Set_SelectedAnimation(_int i) { m_iSelectedAnimation = i; Normalize_Selection(); }
	void Set_SelectedTrack(_int i) { m_iSelectedTrack = i; Normalize_Selection(); }
	void Set_SelectedStep(_int i) { m_iSelectedStep = i; Normalize_Selection(); }

	VPMODE Get_VPMode() const { return m_eVPMode; }
	void   Set_VPMode(VPMODE e) { m_eVPMode = e; }

	HRESULT Initialize();
	void Update(_float fTimeDelta);

	_int Find_WidgetIndexById(const _string& strId) const;
	_wstring Make_UniqueAnimationName(const UISEQ_WIDGET_NODE& tWidget, const _wstring& strBase, _int iSkipIndex = -1) const;
	void Rename_Animation(UISEQ_WIDGET_NODE& w, _int iAnimIdx, const _wstring& strOldName, const _wstring& strNewName);
	CUIObject* Resolve_Sentinel(UI_TYPE eType) const;
	_bool Sanitize_DocReferences();
	UISEQ_WIDGET_NODE Make_DefaultWidget(UI_TYPE eType) const;
	CUITween::UITWEEN_DESC Make_DefaultTrack(const UISEQ_WIDGET_NODE& tWidget) const;
	UISEQ_STEP_NODE Make_DefaultStep(UI_SEQ_STEP_KIND eKind, _bool bJoinPrev) const;
	void Duplicate_Widget(_int iWidgetIndex);
	void Erase_Widget(_int iWidgetIndex);
	void Mark_Dirty(const char* pszReason);
	void Mark_Dirty_Property(const char* pszReason);	// Dirty만, rebuild 회피
	const UISEQ_WIDGET_NODE* Find_WidgetById(const _string& strId) const;
	_bool Apply_StepTargetFallback(UISEQ_STEP_NODE& tStep) const;

	HRESULT Save(const _string& strPath);   // EditInstance::Save_UISequence 호출
	HRESULT Load(const _string& strPath);   // 호출 후 Sanitize + Normalize
	HRESULT New_Doc();                      // Reset_Doc 래퍼

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext ={ nullptr };

	CGameInstance* m_pGameInstance = { nullptr };
	class CEditInstance* m_pEditInstance = { nullptr };
	class CUIObject* m_pSentinels[5] = { nullptr };

	UISEQ_DOC m_Doc = {};
	_int m_iSelectedWidget = { -1 };
	_int m_iSelectedAnimation = { -1 };
	_int m_iSelectedTrack = { -1 };
	_int m_iSelectedStep = { -1 };
	_bool m_bDirty = { false };
	_string m_strDocPath = { "../../DataFiles/UI/HUD_Layout.uiseq" };
	_string m_strStatus = {};

	VPMODE m_eVPMode = { VPMODE::SCENE };

private:
	HRESULT Initialize_Sentinels();
	void Reset_Doc();
	_string Make_NextWidgetId() const;
	_string Make_NextCallbackId() const;
	_string Make_NextSlotId(const char* pszPrefix) const;
	void Normalize_Selection();
	void Clear_Dirty();
	void Propagate_DocCanvasToWidgets();

public:
	static CUIEditorSession* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

private:
	virtual void Free() override;
};

NS_END