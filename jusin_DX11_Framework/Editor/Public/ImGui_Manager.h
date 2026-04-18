#pragma once
#include "GameObject.h"
#include "Editor_Defines.h"

NS_BEGIN(Editor)

class CImGui_Manager : public CBase
{
private:
	CImGui_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CImGui_Manager() = default;

public:
	HRESULT Initialize(HWND hWnd);
	void Update(_float fTimeDelta);
	HRESULT Render();

#pragma region Panel_MapTool
	_uint Get_NavToolMode() const;
	void Update_NavDragHit(const _float3& vWorldPos);
	//_bool Get_CurrentWorldHit(_float3* pOut) const;
	_bool Is_NavEditMode() const;
	_bool Is_NavPointMode() const;
	void Fire_NavClick(const _float3& vWorldPos);
#pragma endregion

#pragma region Panel_PlaceBrowser
	void Begin_PlaceMode(const CATALOG_ITEM& tItem);
	void End_PlaceMode();
	_bool Is_PlaceMode() const { return m_bPlaceMode; }
	const CATALOG_ITEM& Get_PlaceItem() const { return m_tPlaceItem; }
#pragma endregion

#pragma region Panel_Viewport
	ImVec2 Get_ViewportScreenPos() const;
	ImVec2 Get_ViewportScreenSize() const;
	class CPanel_Viewport* Get_ViewportPanel() const;
	_bool Is_ViewportActive() const;
	_bool Is_AnyNonViewportPanelActive() const;
#pragma endregion

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class CEditInstance* m_pEditInstance = { nullptr };
	array<class CPanel_Base*, g_kNumPanels> m_Panels{};

	_bool m_bPlaceMode = { false };
	CATALOG_ITEM m_tPlaceItem = {};

private:
	HRESULT Add_Panels();

public:
	static CImGui_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd);

private:
	virtual void Free() override;
};

NS_END