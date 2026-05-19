#pragma once
#include "Base.h"
#include "Editor_Defines.h"
#include "Effect_Defines.h"

NS_BEGIN(Game_PKM)
class CEffect;
NS_END

NS_BEGIN(Editor)

class CEffectEditorSession final : public CBase
{
private:
	CEffectEditorSession();
	virtual ~CEffectEditorSession() = default;

public:
	HRESULT Initialize();
	void Update(_float fTimeDelta);

	const EFFECT_DEFINITION& Get_Doc() const { return m_Doc; }
	EFFECT_DEFINITION& Get_DocMutable() { return m_Doc; }
	void Set_DocID(const _string& strID);
	HRESULT Save(const _string& strPath);
	HRESULT Load(const _string& strPath);
	void Clear_Dirty();

	const _string& Get_DocPath() const { return m_strDocPath; }
	void Set_DocPath(const _string& strPath);
	const _string& Get_Status() const { return m_strStatus; }
	_bool Is_Dirty() const { return m_bDirty; }

	_int Get_SelectedEmitter() const { return m_iSelectedEmitter; }
	void Set_SelectedEmitter(_int iIndex);

	void New_Doc();
	void Add_Emitter();
	void Erase_SelectedEmitter();
	EMITTER_DEFINITION* Get_SelectedEmitterMutable();

	void Mark_Dirty(const char* pReason);

	HRESULT Spawn_Preview();
	void Stop_Preview();
	void Destroy_Preview();
	_bool Is_PreviewAlive() const;
	const _float3& Get_PreviewPosition() const { return m_vPreviewPosition; }
	void Set_PreviewPosition(const _float3& vPosition);
	void Reset_PreviewPosition();

private:
	EFFECT_DEFINITION m_Doc = {};
	_int m_iSelectedEmitter = { -1 };
	_bool m_bDirty = { false };
	_string m_strDocPath = { "../../Resources/Effects/new_effect.effect.json" };
	_string m_strStatus = {};

	Game_PKM::CEffect* m_pPreviewEffect = { nullptr }; // borrowed
	_bool m_bPreviewRefreshPending = { false };
	_float3 m_vPreviewPosition = { 0.f, 1.f, 0.f };

private:
	void Normalize_Selection();

public:
	static CEffectEditorSession* Create();

private:
	virtual void Free() override;
};

NS_END