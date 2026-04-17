#pragma once
#include "Panel_Base.h"

NS_BEGIN(Editor)

class CPanel_MapTool final : public CPanel_Base
{
protected:
	CPanel_MapTool();
	virtual ~CPanel_MapTool() = default;

public:
	_bool Is_NavEditMode() const { return m_bNavEditMode; }
	_bool Is_NavPointMode() const { return m_bNavPointMode; }

	HRESULT Initialize() override;
	void Update(_float fTimeDelta) override;
	HRESULT Render() override;
	void Add_NavPoint(const _float3& vWorldPos);
	void Clear_PendingPoints();

private:
	_bool m_bNavEditMode = { false };
	_bool m_bNavPointMode = { false };
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

private:

public:
	static CPanel_MapTool* Create();

private:
	virtual void Free() override;
};

NS_END