#pragma once
#include "UIImage.h"
#include "UIText.h"
#include "UIButton.h"
#include "UIProgressBar.h"
#include "Editor_Defines.h"

NS_BEGIN(Engine)
class CGameInstance;
class CUIObject;
class CUISequence;
NS_END

NS_BEGIN(Editor)

enum class UI_PREVIEW_MODE { LAYOUT, SELECTED_ANIM, SEQUENCE };
enum class UI_PREVIEW_STATE { IDLE, PLAYING, PAUSED };

class CUIPreviewHost : public CBase
{
private:
	CUIPreviewHost(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CUIPreviewHost() = default;

public:
	void Set_Mode(UI_PREVIEW_MODE eMode) { m_eMode = eMode; };
	UI_PREVIEW_MODE Get_Mode() const { return m_eMode; }
	UI_PREVIEW_STATE Get_State() const { return m_eState; }

	const ImVec2& Get_LastViewportSize() const { return m_vLastViewportSize; }
	const vector<_int>& Get_ZOrderIdx() const { return m_vZOrderIdx; }
	const vector<CUIObject*>& Get_Widgets() const;

	HRESULT Initialize();
	void Tick(_float fTimeDelta);	// Update_Editor 안에서 호출
	void Render_Queue_Submit();		// Late_Update 호출 (RT active 후)
	HRESULT Rebuild();				// 현재 doc로 재구성
	void Mark_Rebuild_Pending() { m_bRebuildPending = true; }
	_bool Has_Rebuild_Pending() const { return m_bRebuildPending; }
	void Process_Rebuild_If_Pending();	// RT active 후 호출

	void Play();
	void Pause();
	void Resume();
	void Stop();
	void Restart();

	CUIObject* Find_Runtime(const _string& strId) const;
	_string Hit_Test_TopMost(const ImVec2& vDocXY) const;	// z-order 우선, hit widget의 ID 반환, 없으면 빈 문자열

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };
	class CEditInstance* m_pEditInstance = { nullptr };
	class CUIEditorSession* m_pSession = { nullptr };	// weak

	unordered_map<_string, CUIObject*> m_id2Widget;	// weak refs into sequence children
	CUISequence* m_pSequence = { nullptr };			// owned

	UI_PREVIEW_MODE m_eMode = UI_PREVIEW_MODE::LAYOUT;
	UI_PREVIEW_STATE m_eState = UI_PREVIEW_STATE::IDLE;

	_bool m_bRebuildPending = { true };	// 첫 프레임에 build
	ImVec2 m_vLastViewportSize = { 0.f, 0.f };

	vector<_int> m_vZOrderIdx;	// tick 시 사용

	_int m_iLastSelWidget = { -2 };
	_int m_iLastSelAnim = { -2 };

private:
	void Release_All();
	void Apply_Fallback_Image(CUIImage::UIIMAGE_DESC& d) const;
	void Apply_Fallback_Text(CUIText::UITEXT_DESC& d) const;
	void Apply_Fallback_Button(CUIButton::UIBUTTON_DESC& d) const;
	void Apply_Fallback_ProgressBar(CUIProgressBar::UIPROGRESSBAR_DESC& d) const;
	void Sort_ZOrder();

public:
	static CUIPreviewHost* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

private:
	virtual void Free() override;
};

NS_END