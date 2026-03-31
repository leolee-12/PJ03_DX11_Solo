#pragma once
#include "GameObject.h"
#include "Editor_Defines.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Editor)
class CEditInstance;

class CObject_Registry final : public CBase
{
private:
	CObject_Registry();
	virtual ~CObject_Registry() = default;

public:
	HRESULT Initialize();

	const vector<OBJ_RECORD>& Get_Records() const { return m_Records; }
	const vector<CGameObject*>& Get_EditorObjects() const { return m_EditorObjects; }
	void Register_Object(_uint iProtoLevel, WNameID strProtoTag, _uint iLayerLevel, WNameID strLayerTag, void* pArg);    // 배치 시 호출
	void Unregister_Object(CGameObject* pObj);  // 삭제 시 호출
	void Clone_Object(CGameObject* pObj); // 복제 시 호출

private:
	CGameInstance* m_pGameInstance = { nullptr };
	CEditInstance* m_pEditInstance = { nullptr };
	vector<OBJ_RECORD> m_Records{};
	vector<CGameObject*> m_EditorObjects{};

private:
	_wstring Make_UniqueName(const _wstring& wStrBaseName) const;

public:
	static CObject_Registry* Create();

private:
	virtual void Free() override;
};

NS_END