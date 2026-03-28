#pragma once
#include "GameObject.h"
#include "Editor_Defines.h"

/* ------------------------------------------------------------ */
// CEditInstance : 에디터 기능 간 오케스트레이션 및 총괄 관리
/* ------------------------------------------------------------ */

NS_BEGIN(Editor)

class CEditInstance final : public CBase
{
	DECLARE_SINGLETON(CEditInstance)

private:
	CEditInstance();
	virtual ~CEditInstance() = default;

public:
#pragma region EDITOR
	HRESULT Initialize_Editor(const ENGINE_DESC& EngineDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext);
	void Update_Editor(_float fTimeDelta);
	HRESULT Draw();
	void Release_Editor();
#pragma endregion

#pragma region IMGUI_MANAGER

#pragma endregion

#pragma region SELECT_MANAGER
	void Select(CGameObject* pObj, bool bMultiSelect = false);
	void Deselect(CGameObject* pObj);
	void Clear();

	const vector<CGameObject*>& Get_Selected() const;
	CGameObject* Get_Primary() const;
	bool Is_Selected(CGameObject* pObj) const;

	void Register_Callback(const _string& strKey, SelectionChangedCB cb);
	void Unregister_Callback(const _string& strKey);
#pragma endregion

#pragma region OBJECT_REGISTRY
	const vector<CGameObject*>& Get_EditorObjects() const;
	void Register_Object(_uint iProtoLevel, WNameID strProtoTag, _uint iLayerLevel, WNameID strLayerTag, void* pArg);
	void Unregister_Object(CGameObject* pObj);
	void Clone_Object(CGameObject* pObj);
#pragma endregion

#pragma region 4

#pragma endregion

#pragma region 5

#pragma endregion

#pragma region 6

#pragma endregion

#pragma region 7

#pragma endregion

#pragma region 8

#pragma endregion

private:
	class CImGui_Manager* m_pImGui_Manager = { nullptr };
	class CSelect_Manager* m_pSelect_Manager = { nullptr };
	class CObject_Registry* m_pObject_Registry = { nullptr };

private:
	virtual void Free() override;
};

NS_END