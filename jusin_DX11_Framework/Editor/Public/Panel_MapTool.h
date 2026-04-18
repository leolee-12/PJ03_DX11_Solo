#pragma once
#include "Panel_Base.h"
#include "EditInstance.h"

NS_BEGIN(Editor)

class CPanel_MapTool final : public CPanel_Base
{
private:
	enum NAV_TOOL_MODE { POINT, SELECT, MOVE, REMOVE, END };

protected:
	CPanel_MapTool();
	virtual ~CPanel_MapTool() = default;

public:
	_bool Is_NavEditMode() const { return m_bNavEditMode; }
	_bool Is_NavPointMode() const { return m_bNavEditMode && m_eToolMode == NAV_TOOL_MODE::POINT; }
	_bool Is_NavSelectMode() const { return m_bNavEditMode && m_eToolMode == NAV_TOOL_MODE::SELECT; }
	_bool Is_NavMoveMode() const { return m_bNavEditMode && m_eToolMode == NAV_TOOL_MODE::MOVE; }
	_bool Is_NavDeleteMode() const { return m_bNavEditMode && m_eToolMode == NAV_TOOL_MODE::REMOVE; }
	NAV_TOOL_MODE Get_NavToolMode() const { return m_eToolMode; }
	void Set_DragHitPos(const _float3& v) { m_vDragHitPos = v; m_bHasDragHit = true; }

	HRESULT Initialize() override;
	void Update(_float fTimeDelta) override;
	HRESULT Render() override;
	void Add_NavPoint(const _float3& vWorldPos);
	void Clear_PendingPoints();
	void Handle_NavClick(const _float3& vWorldPos);

private:
	_bool m_bNavEditMode = { false };
	NAV_TOOL_MODE m_eToolMode = { NAV_TOOL_MODE::POINT };
	_int m_iSelectedCell = { -1 };	// 선택된 셀 인덱스 (-1 = 없음)
	_int m_iDragVertex = { -1 };	// 드래그 중인 꼭짓점 (0~2, -1 = 없음)
	_bool m_bDragging = { false };
	_float3 m_vDragHitPos = {};
	_bool m_bHasDragHit = { false };
	_float m_fSnapRadius = { 0.3f };

	// 편집 데이터
	vector<_float3> m_PendingPoints;		// 최대 2개 누적
	vector<array<_float3, 3>> m_NavCells;	// 확정된 셀들

	// 파일 경로 (char: ImGui InputText용)
	_char m_szNavSavePath[260] = {};

private:
	HRESULT Ready_EditableTexture(const _tchar* pFileDir);
	_float3 Snap_Point(const _float3& vInput) const;
	void Render_NavOverlay();
	ImVec2 Project_To_Screen(const _float3& vWorldPos) const;
	void Save_NavMesh();
	void Load_NavMesh();
	_int HitTest_Cell(const _float3& vWorldPos) const;							// 클릭 위치가 어느 셀 안인지
	_int HitTest_Vertex(const _float3& vWorldPos, _int* pOutCellIdx) const;	// 근접 꼭짓점 찾기
	void Update_SharedVertex(const _float3& vOld, const _float3& vNew);


private:

public:
	static CPanel_MapTool* Create();

private:
	virtual void Free() override;
};

NS_END