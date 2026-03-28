#pragma once
#include "GameObject.h"
#include "Editor_Defines.h"

NS_BEGIN(Editor)

class CObject_Registry final : public CBase
{
private:
	CObject_Registry() = default;
	virtual ~CObject_Registry() = default;

public:
	HRESULT Initialize();

	const vector<CGameObject*>& Get_EditorObjects() const { return m_EditorObjects; }
	void Register_Object(_uint iProtoLevel, WNameID strProtoTag, _uint iLayerLevel, WNameID strLayerTag, void* pArg);    // 배치 시 호출
	void Unregister_Object(CGameObject* pObj);  // 삭제 시 호출

private:
	class CGameInstance* m_pGameInstance = { nullptr };
	class CEditInstance* m_pEditInstance = { nullptr };
	vector<CGameObject*> m_Selected;
	vector<CGameObject*> m_EditorObjects{};

public:
	static CObject_Registry* Create();

private:
	virtual void Free() override;
};

NS_END