#ifdef _DEBUG
#pragma once
#include "Imgui_Tab.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"
#include "ImGuizmo.h"
#include "SoundPoint.h"

BEGIN(Engine)
class CSoundPoint;
END

BEGIN(Client)

class CImgui_Tab_Sound : public CImgui_Tab
{
private:
	CImgui_Tab_Sound(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual	~CImgui_Tab_Sound() = default;

public:
	virtual HRESULT Initialize();
	virtual void	Tick(_cref_time fTimeDelta);
	virtual HRESULT Render();


public:
	// [검증단계]
	void		Layout_Set_SoundData();
	// [검증단계]
	void		Layout_Update_SoundData();
	// [검증단계]
	void		Layout_SaveLoad_SoundData();


private:
	// [검증단계]
	HRESULT		Create_SoundPoint(const string& strFindName, const TSoundPointDesc SoundPointDesc, shared_ptr<CSoundPoint>* ppOut = nullptr);
	

#pragma region ImGuizmo Variable
private:
	// [검증단계]
	void		Use_ImGuizmo();

private:
	ImGuizmo::OPERATION m_CurrentGizmoOperation = ImGuizmo::TRANSLATE;
	bool useSnap = false;
	float snap[3] = { 1.f, 1.f, 1.f };
#pragma endregion

private:
	// [완료]
	void Invalidate_SoundPointData(CSoundPoint* pSoundPoint);

private:
	vector<shared_ptr<CSoundPoint>>		m_SoundPointsList;

	// 피킹 객체
	shared_ptr<CSoundPoint>		m_pPickedSound = { nullptr };
	_int						m_iPickedIndex = -1;			// 피킹된 사운드 인덱스

	// 피킹 객체 데이터
	string						m_strSoundPointName = { "" };	// 사운드 객체의 이름
	TSoundPointDesc				m_SoundPointDesc = {};

	string						m_strCreateSoundPointName = { "" };	// 생성 사운드 객체의 이름
	TSoundPointDesc				m_CreateSoundPointDesc = {};


private: // 모달용
	_int m_iSelectedSoundGroup_Index = -1;
	_int m_iSelectedSound_Index = -1;

	string m_strCurrentSound_Name;
	string m_strAdd_SoundGroup;
	string m_strAdd_SoundName;

public:
	static CImgui_Tab_Sound* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual void	Free() override;
};

END

#endif // DEBUG
