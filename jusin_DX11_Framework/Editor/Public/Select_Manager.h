#pragma once
#include "Base.h"
#include "Editor_Defines.h"

NS_BEGIN(Engine)
class CGameObject;
NS_END

NS_BEGIN(Editor)

class CSelect_Manager final : public CBase
{
private:
	CSelect_Manager() = default;
	virtual ~CSelect_Manager() = default;

public:
	HRESULT Initialize();

	// 선택 조작
	void Select(CGameObject* pObj, bool bMultiSelect = false);
	void Deselect(CGameObject* pObj);
	void Clear();

	// 조회
	const vector<CGameObject*>& Get_Selected() const { return m_Selected; }
	CGameObject* Get_Primary() const;   // m_Selected[0], 없으면 nullptr
	bool Is_Selected(CGameObject* pObj) const;

	// 변경 통지 콜백 (패널 등록용)
	void Register_Callback(const _string& strKey, SelectionChangedCB cb);
	void Unregister_Callback(const _string& strKey);

private:
	vector<CGameObject*> m_Selected;
	unordered_map<_string, SelectionChangedCB> m_Callbacks;

	void Notify();  // 등록된 콜백 전부 호출

public:
	static CSelect_Manager* Create();

private:
	virtual void Free() override;
};

NS_END