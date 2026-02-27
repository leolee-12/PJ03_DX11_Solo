#pragma once
#include "IMGUI_Tab.h"
#include "Camera.h"
#include "Virtual_Camera.h"

BEGIN(Client)

class CIMGUI_Camera_Tab : public CIMGUI_Tab
{
	struct pair_hash {
		template <class T1, class T2>
		std::size_t operator()(const std::pair<T1, T2>& pair) const {
			return std::hash<T1>()(pair.first) ^ (std::hash<T2>()(pair.second) << 1);
		}
	};

	//모델은 정해져있음
	enum CAMERA_MODELID { CAMERA_MODELID_NOT = -1, CAMERA_MODELID_NORMAL, CAMERA_MODELID_FREE, CAMERA_MODELID_SON, CAMERA_MODELID_21, CAMERA_MODELID_HIT, CAMERA_MODELID_FRIEZA, CAMERA_MODELID_END };
	//스킬은 각 모델마다 enum이 달라서 map
protected:
	CIMGUI_Camera_Tab(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CIMGUI_Camera_Tab() = default;

public:
	HRESULT Initialize() override;
	void Render(_float fTimeDelta) override;
	virtual const _char* GetTabName() const { return "Camera"; };

private:
	void IMGUI_Camera_Select_Model(_float fTimeDelta);
	void IMGUI_Camera_Select_Skill(_float fTimeDelta);
	void IMGUI_Camera_Select_Animation(_float fTimeDelta);

	void IMGUI_Save_Button();
	void UpdateCameraSelection();
	void IMGUI_Show_Points(_float fTimeDelta);	   // 현재 가상 카메라의 포인트를 목록으로 보여주기
	void IMGUI_Button();        // Add_Point 버튼 처리 함수
	void IMGUI_Delete_Points();
	void IMGUI_Play_Button();	   // Play 버튼
	void IMGUI_Pause_Button();	   // Pause 버튼
	void IMGUI_Stop_Button();	   // Stop 버튼
	void IMGUI_Delete_Point(_int index); //Delete 버튼
	void IMGUI_Modify_Point(_int index); //Modify 버튼
	void IMGUI_Modify_Point_UI(_int index);
	void IMGUI_Point_Modify_Save();

	const _float4x4* Get_Model_Float4x4();
	void VisualizeCameraPoints(const vector<CameraPoint>& points, _float fTimeDelta);
	void DrawDebugText(const _float3& position, const std::wstring& text, const _float4& color);

private:
	CAMERA_MODELID m_iSelected_Model = CAMERA_MODELID_NORMAL;  // 모델 선택 상태를 저장
	_int  m_iSelected_Skill = -1;
	_int m_iSelected_Animation = -1; // 애니메이션 선택 상태를 저장 (인덱스 기반)

	// 모델별 스킬 목록
	unordered_map<CAMERA_MODELID, vector<string>> m_ModelSkills;
	// 모델 이름 배열 선언
	const char* MODEL_NAMES[CAMERA_MODELID_END];

	// 스킬과 애니메이션 인덱스 쌍을 키로 받고 애니메이션 목록을 값으로 갖는 맵
	unordered_map<pair<CAMERA_MODELID, int>, vector<string>, pair_hash> m_SkillAnimations;

	_int m_selectedPoint = -1;
	_bool m_isEditing = { false };     // 수정 모드 여부
	_bool m_isCompleteCameraSelect = { true };
	//모델과 스킬 pair쌍을 키로 받고 가상카메라 인덱스를 값으로 갖는
	unordered_map<pair<CAMERA_MODELID, _int>, _uint, pair_hash> m_CameraIndexMap;


	// 임시 데이터 구조체
	struct TempPointData {
		_float duration;
		CVirtual_Camera::InterpolationType interpType;
		_float damping = { 1.f };
	};

	TempPointData m_tempPointData;

	//메인카메라
	class CMain_Camera* m_pMainCamera = { nullptr };
	class CLine_Draw* m_pLineDraw = { nullptr };
public:
	static CIMGUI_Camera_Tab* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END
