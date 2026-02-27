#include "stdafx.h"
#include "IMGUI_Camera_Tab.h"
#include "GameInstance.h"
#include "RenderInstance.h"
#include "Main_Camera.h"
#include "Virtual_Camera.h"
#include "Line_Draw.h"
#include "Character.h"
_bool bShowCameraWindow = true;  // IMGUI 창 표시 여부를 제어하는 전역 변수
_wstring filename = L"../Bin/CameraPoints.txt"; //데이터 저장되는 txt


CIMGUI_Camera_Tab::CIMGUI_Camera_Tab(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CIMGUI_Tab{ pDevice,pContext }
{
}

HRESULT CIMGUI_Camera_Tab::Initialize()
{
	// 모델 이름 배열 초기화
	MODEL_NAMES[CAMERA_MODELID_NORMAL] = "Normal";
	MODEL_NAMES[CAMERA_MODELID_FREE] = "Free";
	MODEL_NAMES[CAMERA_MODELID_SON] = "Son";
	MODEL_NAMES[CAMERA_MODELID_21] = "21";
	MODEL_NAMES[CAMERA_MODELID_HIT] = "Hit";
	MODEL_NAMES[CAMERA_MODELID_FRIEZA] = "Frieza";

	_int index = 0;

	// 모델별 스킬 목록 초기화
	m_ModelSkills[CAMERA_MODELID_SON] = { "Camera_Son_Heavy", "Camera_Son_Knock_Away_Up","Camera_Son_Air_Smash","Camera_Son_Grab","Camera_Son_Same_Grab","Camera_Son_Energy","Camera_Son_Ultimate" };
	m_ModelSkills[CAMERA_MODELID_21] = { "Camera_21_Heavy", "Camera_21_Knock_Away_Up", "Camera_21_Air_Smash","Camera_21_Grab","Camera_21_Same_Grab","Camera_21_Grab_Special","Camera_21_Energy","Camera_21_Ultimate" };
	m_ModelSkills[CAMERA_MODELID_HIT] = { "Camera_Hit_Heavy", "Camera_Hit_Knock_Away_Up","Camera_Hit_Grab","Camera_Hit_214_Middle","Camera_Hit_236_Middle","Camera_Hit_236_Heavy","Camera_Hit_1_Ultimate","Camera_Hit_3_Ultimate" };
	m_ModelSkills[CAMERA_MODELID_FRIEZA] = { "Camera_Frieza_Light_Final", "Camera_Frieza_Heavy", "Camera_Frieza_Knock_Away_Up","Camera_Frieza_Grab","Camera_Frieza_Ultimate_1","Camera_Frieza_Ultimate_3","Camera_Frieza_Golden", "Camera_Frieza_Down_Forward_J","Camera_Frieza_Fall_Reflect","Camera_Common_Appear","Camera_Goku_Entry","Camera_21_Entry","Camera_Hit_Entry","Camera_Frieza_Entry","Camera_Goku_vs_Frieza_Entry","Camera_Hit_Win"};

	// 모델과 스킬 인덱스에 따른 카메라 인덱스 매핑 초기화
	// 기본 카메라 매핑
	m_CameraIndexMap[{CAMERA_MODELID_NORMAL, -1}] = index++;
	m_CameraIndexMap[{CAMERA_MODELID_FREE, -1}] = index++;

	// 각 모델과 그에 해당하는 스킬을 순회하며 매핑 설정
	for (const auto& modelSkillPair : m_ModelSkills)
	{
		CAMERA_MODELID model = modelSkillPair.first;
		const std::vector<std::string>& skills = modelSkillPair.second;

		for (int skillIdx = 0; skillIdx < static_cast<int>(skills.size()); ++skillIdx)
		{
			m_CameraIndexMap[{model, skillIdx}] = index++;

			// 스킬별 애니메이션 목록 초기화 (예시 데이터)
			if (model == CAMERA_MODELID_SON)
			{
				if (skillIdx == 0)
				{
					m_SkillAnimations[{model, skillIdx}] = { "Son_Heavy_Anim1" };
				}
				else if (skillIdx == 1)
				{
					m_SkillAnimations[{model, skillIdx}] = { "Son_Knock_Away_Up_Anim1" };
				}
				else if (skillIdx == 2)
				{
					m_SkillAnimations[{model, skillIdx}] = { "Son_Air_Smash_Anim1" };
				}
				else if (skillIdx == 3)
				{
					m_SkillAnimations[{model, skillIdx}] = { "Son_Grab_Anim1" };
				}
				else if (skillIdx == 4)
				{
					m_SkillAnimations[{model, skillIdx}] = { "Son_Same_Grab_Anim1" };
				}
				else if (skillIdx == 5)
				{
					m_SkillAnimations[{model, skillIdx}] = { "Son_Energy_Anim1" };
				}
				else if (skillIdx == 6)
				{
					m_SkillAnimations[{model, skillIdx}] = { "Son_Ultimate_Anim1", "Son_Ultimate_Anim2", "Son_Ultimate_Anim3", "Son_Ultimate_Anim4", "Son_Ultimate_Anim5" };
				}
			}
			else if (model == CAMERA_MODELID_21)
			{
				if (skillIdx == 0) // 21_Skill1
				{
					m_SkillAnimations[{model, skillIdx}] = { "21_Heavy_Anim1" };
				}
				else if (skillIdx == 1) // 21_Skill2
				{
					m_SkillAnimations[{model, skillIdx}] = { "21_Knock_Away_Up_Anim1" };
				}
				else if (skillIdx == 2) // 21_Skill3
				{
					m_SkillAnimations[{model, skillIdx}] = { "21_Air_Smash_Anim1" };
				}
				else if (skillIdx == 3)
				{
					m_SkillAnimations[{model, skillIdx}] = { "21_Grab_Anim1" };
				}
				else if (skillIdx == 4)
				{
					m_SkillAnimations[{model, skillIdx}] = { "21_Same_Grab_Anim1" };
				}
				else if (skillIdx == 5)
				{
					m_SkillAnimations[{model, skillIdx}] = { "21_Grab_Special_Anim1" };
				}
				else if (skillIdx == 6)
				{
					m_SkillAnimations[{model, skillIdx}] = { "21_Energy_Anim1" };
				}
				else if (skillIdx == 7)
				{
					m_SkillAnimations[{model, skillIdx}] = { "21_Ultimate_Anim1", "21_Ultimate_Anim2", "21_Ultimate_Anim3_Success", "21_Ultimate_Anim3_Fail", "21_Ultimate_Anim4", "21_Ultimate_Anim4_Flip" };
				}
			}
			else if (model == CAMERA_MODELID_HIT)
			{
				if (skillIdx == 0) // 21_강공격
				{
					m_SkillAnimations[{model, skillIdx}] = { "Hit_Heavy_Anim1" };
				}
				else if (skillIdx == 1) // 어퍼
				{
					m_SkillAnimations[{model, skillIdx}] = { "Hit_Knock_Away_Up_Anim1" };
				}
				else if (skillIdx == 2) // 잡기
				{
					m_SkillAnimations[{model, skillIdx}] = { "Hit_Grab_Anim1" };
				}
				else if (skillIdx == 3) // 히트 214 중
				{
					m_SkillAnimations[{model, skillIdx}] = { "Hit_214_Middle_Anim1" };
				}
				else if (skillIdx == 4) // 히트 236 중
				{
					m_SkillAnimations[{model, skillIdx}] = { "Hit_236_Middle_Anim1" };
				}
				else if (skillIdx == 5) // 히트 236 강
				{
					m_SkillAnimations[{model, skillIdx}] = { "Hit_236_Heavy_Anim1" };
				}
				else if (skillIdx == 6) // 히트 1필
				{
					m_SkillAnimations[{model, skillIdx}] = { "Hit_1_Ultimate_Anim1","Hit_1_Ultimate_Anim2" };
				}
				else if (skillIdx == 7) // 히트 3필
				{
					m_SkillAnimations[{model, skillIdx}] = { "Hit_3_Ultimate_Anim1","Hit_3_Ultimate_Anim2","Hit_3_Ultimate_Anim3","Hit_3_Ultimate_Anim4" };
				}
			}
			// 다른 모델과 스킬에 대한 애니메이션도 유사하게 초기화
			else if (model == CAMERA_MODELID_FRIEZA)
			{
				if (skillIdx == 0) // 약공격 마지막
				{
					m_SkillAnimations[{model, skillIdx}] = { "Frieza_Light_Final_Anim1","Frieza_Light_Final_Anim1_Flip","Frieza_Light_Final_Anim2","Frieza_Light_Final_Anim2_Flip" };
				}
				else if (skillIdx == 1)  //강공격
				{
					m_SkillAnimations[{model, skillIdx}] = { "Frieza_Heavy_Anim1" };
				}
				else if (skillIdx == 2) //어퍼컷
				{
					m_SkillAnimations[{model, skillIdx}] = { "Frieza_Knock_Away_Up_Anim1" };
				}
				else if (skillIdx == 3)
				{
					m_SkillAnimations[{model, skillIdx}] = { "Frieza_Grab_Anim1" };
				}
				else if (skillIdx == 4)
				{
					m_SkillAnimations[{model, skillIdx}] = { "Frieza_Ultimate_1_Anim1","Frieza_Ultimate_1_Anim1_Flip" };
				}
				else if (skillIdx == 5)
				{
					m_SkillAnimations[{model, skillIdx}] = { "Frieza_Ultimate_3_Anim1","Frieza_Ultimate_3_Anim1_Flip" };
				}
				else if (skillIdx == 6)
				{
					m_SkillAnimations[{model, skillIdx}] = { "Frieza_Golden_Anim1" };
				}
				else if (skillIdx == 7)
				{
					m_SkillAnimations[{model, skillIdx}] = { "Frieza_Down_Forward_J","Frieza_Down_Forward_J_Flip" };
				}
				else if (skillIdx == 8)
				{
					m_SkillAnimations[{model, skillIdx}] = { "Frieza_Fall_Reflect" };
				}
				else if (skillIdx == 9)
				{
					m_SkillAnimations[{model, skillIdx}] = { "Common_Appear_Anim_1","Common_Appear_Anim_1_Flip" };
				}
				else if (skillIdx == 10)
				{
					m_SkillAnimations[{model, skillIdx}] = { "Goku_Entry_Anim_1","Goku_Entry_Anim_1_Flip" };
				}
				else if (skillIdx == 11)
				{
					m_SkillAnimations[{model, skillIdx}] = { "21_Entry_Anim_1","21_Entry_Anim_1_Flip" };
				}
				else if (skillIdx == 12)
				{
					m_SkillAnimations[{model, skillIdx}] = { "Hit_Entry_Anim_1","Hit_Entry_Anim_1_Flip" };
				}
				else if (skillIdx == 13)
				{
					m_SkillAnimations[{model, skillIdx}] = { "Frieza_Entry_Anim_1","Frieza_Entry_Anim_1_Flip" };
				}
				else if (skillIdx == 14)
				{
					m_SkillAnimations[{model, skillIdx}] = { "Goku_VS_Frieza_Entry_Anim_1","Goku_VS_Frieza_Entry_Anim_1_Flip" };
				}
				else if (skillIdx == 15)
				{
					m_SkillAnimations[{model, skillIdx}] = { "Hit_Win_Anim_1"};
				}
			}
		}
	}

	return S_OK;
}

void CIMGUI_Camera_Tab::Render(_float fTimeDelta)
{
	//초기 메인카메라 셋팅
	if (m_pMainCamera == nullptr)
	{
		CGameObject* camera = m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera"));
		m_pMainCamera = static_cast<CMain_Camera*>(camera);

		Safe_AddRef(m_pMainCamera);
		return;
	}

	// 모델 선택 UI 호출
	IMGUI_Camera_Select_Model(fTimeDelta);

	// 모델이 선택된 경우에만 스킬 선택 UI를 표시
	if (m_iSelected_Model >= CAMERA_MODELID_SON) {
		ImGui::Spacing();  // 한 줄 띄우기
		ImGui::Separator();  // 경계선 그리기

		// 스킬 선택 UI 호출
		IMGUI_Camera_Select_Skill(fTimeDelta);
	}


	//선택안되면 -1임.
	if (m_iSelected_Skill >= 0)
	{
		ImGui::Spacing();  // 한 줄 띄우기
		ImGui::Separator();  // 경계선 그리기

		// 애니메이션 선택 UI 호출
		IMGUI_Camera_Select_Animation(fTimeDelta);
	}

	if (m_iSelected_Animation >= 0)
	{
		// 포인트 보여주기
		IMGUI_Show_Points(fTimeDelta);

		// Point 버튼 호출
		IMGUI_Button();
	}

	////각 모델과 각 스킬에 연결된 카메라가 가지고 있는 Point를 메모장으로 저장하는 방식
	IMGUI_Save_Button();
}

void CIMGUI_Camera_Tab::IMGUI_Camera_Select_Model(_float fTimeDelta)
{
	// Model selection dropdown
	_int iSelected_Model = static_cast<_int>(m_iSelected_Model);

	ImGui::Text("Select Model");
	if (ImGui::Combo("Model", &iSelected_Model, MODEL_NAMES, IM_ARRAYSIZE(MODEL_NAMES))) {
		// 모델이 변경된 경우에만 스킬 선택을 초기화
		if (m_iSelected_Model != static_cast<CAMERA_MODELID>(iSelected_Model)) {
			m_iSelected_Model = static_cast<CAMERA_MODELID>(iSelected_Model); // 새로운 모델로 업데이트
			m_iSelected_Skill = -1;  // 스킬 선택 초기화
			m_iSelected_Animation = -1;

		}
	}

	//카메라 모델이 디폴트나 FREE모드라면 모델 선택 후 바로 교체
	if (m_iSelected_Model == CAMERA_MODELID_NORMAL || m_iSelected_Model == CAMERA_MODELID_FREE)
		UpdateCameraSelection();
}

void CIMGUI_Camera_Tab::IMGUI_Camera_Select_Skill(_float fTimeDelta)
{
	// 현재 선택된 모델에 대한 스킬 목록 가져오기
	auto it = m_ModelSkills.find(m_iSelected_Model);
	if (it == m_ModelSkills.end()) {
		ImGui::Text("No skills available for this model.");
		return;
	}

	//모델이 가지고 있는 string 배열 가져오기
	const vector<string>& skills = it->second;

	// 스킬 목록을 const char* 배열로 변환
	vector<const char*> skill_options;
	for (const auto& skill : skills) {
		skill_options.push_back(skill.c_str());
	}

	// 현재 선택된 스킬 인덱스 가져오기
	int iSelected_Skill = m_iSelected_Skill;

	ImGui::Text("Select Skill");
	if (ImGui::Combo("Skill", &iSelected_Skill, skill_options.data(), static_cast<int>(skill_options.size()))) {
		// 스킬 선택이 변경된 경우
		if (m_iSelected_Skill != iSelected_Skill) {
			m_iSelected_Skill = iSelected_Skill; // 새로운 스킬으로 업데이트 (인덱스 기반)
			m_iSelected_Animation = -1;
			//메인카메라의 가상카메라를 스킬에 맞게 세팅
			UpdateCameraSelection();
		}
	}
}

void CIMGUI_Camera_Tab::IMGUI_Camera_Select_Animation(_float fTimeDelta)
{
	// 현재 선택된 모델과 스킬에 대한 애니메이션 목록 가져오기
	auto key = make_pair(m_iSelected_Model, m_iSelected_Skill);
	auto it = m_SkillAnimations.find(key);
	if (it == m_SkillAnimations.end()) {
		ImGui::Text("No animations available for this skill.");
		return;
	}

	const vector<string>& animations = it->second;

	// 애니메이션 목록을 const char* 배열로 변환
	vector<const char*> animation_options;
	for (const auto& anim : animations) {
		animation_options.push_back(anim.c_str());
	}

	// 현재 선택된 애니메이션 인덱스 가져오기
	int iSelected_Animation = m_iSelected_Animation;

	ImGui::Text("Select Animation");
	if (ImGui::Combo("Animation", &iSelected_Animation, animation_options.data(), static_cast<int>(animation_options.size()))) {
		// 애니메이션 선택이 변경된 경우
		if (m_iSelected_Animation != iSelected_Animation) {
			m_iSelected_Animation = iSelected_Animation; // 새로운 애니메이션으로 업데이트

			if (m_iSelected_Animation >= 0) {
				// 애니메이션 선택에 따른 추가 로직이 필요하면 여기서 구현
			}
		}
	}
}

//스킬을 바꾸면, MainCamera에서 가상카메라를 갈아끼워줘야 함
void CIMGUI_Camera_Tab::UpdateCameraSelection()
{
	auto key = make_pair(m_iSelected_Model, m_iSelected_Skill);
	auto it = m_CameraIndexMap.find(key);

	if (it != m_CameraIndexMap.end())
	{
		_int cameraIndex = it->second;
		m_pMainCamera->Set_Virtual_Camera((CMain_Camera::VIRTUAL_CAMERA)cameraIndex);
	}
	else
	{
		// 매핑된 카메라가 없을 경우 처리 (예: 기본 카메라 선택 또는 오류 메시지 표시)
		ImGui::Text("No camera mapped for this model and skill combination.");
	}
}


void CIMGUI_Camera_Tab::IMGUI_Show_Points(_float fTimeDelta) {

	//스킬에 따른 애니메이션이 선택되지 않았다면
	if (m_iSelected_Animation == -1)
		return;

	//각 스킬마다 가상카메라가 있는데
	//각 스킬의 애니메이션 인덱스에 따른 Points를 보여준다.
	vector<CameraPoint>& points = m_pMainCamera->Get_VectorPoint(m_iSelected_Animation);  // 포인트 벡터 가져오기

	if (points.empty()) {
		ImGui::Text("No camera points available.");
		return;
	}

	ImGui::Separator();
	ImGui::Text("Camera Points:");

	for (size_t i = 0; i < points.size(); ++i) {
		ImGui::PushID(static_cast<int>(i));  // 각 포인트에 고유 ID 부여

		bool isSelected = (m_selectedPoint == static_cast<int>(i));
		if (isSelected) {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));  // 노란색 텍스트
		}

		ImGui::BulletText("Point %d:", static_cast<int>(i) + 1);

		if (isSelected) {
			ImGui::PopStyleColor();  // 텍스트 색상 복원
		}

		ImGui::SameLine();
		ImGui::Dummy(ImVec2(50, 0)); // 50 픽셀의 가로 간격 (필요에 따라 조정)
		ImGui::SameLine();

		// Delete 버튼
		if (ImGui::Button("Delete")) {
			// 삭제 확인 팝업을 띄우려면 추가 구현 필요
			IMGUI_Delete_Point(static_cast<int>(i));
			ImGui::PopID();
			break;  // 삭제 후 루프 종료
		}

		// Modify 버튼
		ImGui::SameLine();
		if (ImGui::Button("Modify")) {
			IMGUI_Modify_Point(static_cast<int>(i));
		}

		// 포인트 정보 표시 (선택 상태에 따라 텍스트 색상 변경)
		if (isSelected) {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));  // 노란색 텍스트
		}

		CameraPoint& point = points[i];
		ImGui::Text("  Position: (%.2f, %.2f, %.2f)", point.position.x, point.position.y, point.position.z);
		ImGui::Text("  Quaternion: (%.2f, %.2f, %.2f)", point.rotation.x, point.rotation.y, point.rotation.z);
		ImGui::Text("  Duration: %.2f", point.duration);
		ImGui::Text("  Interpolation: %s",
			(point.interpolationType == CVirtual_Camera::InterpolationType::INTERPOLATION_LINEAR_MODE)
			? "Linear"
			: (point.interpolationType == CVirtual_Camera::InterpolationType::INTERPOLATION_DAMPING_MODE)
			? "Damping"
			: "Skip");

		// **Damping 값 표시 (Damping Mode인 경우)**
		if (point.interpolationType == CVirtual_Camera::InterpolationType::INTERPOLATION_DAMPING_MODE) {
			ImGui::Text("  Damping: %.2f", point.damping);
		}

		// 선택된 포인트에 대한 수정 UI 표시
		if (isSelected)
		{
			ImGui::PopStyleColor();  // 텍스트 색상 복원
			IMGUI_Modify_Point_UI(static_cast<int>(i));
		}

		ImGui::Separator();
		ImGui::PopID();
	}

	_bool isPlay = m_pMainCamera->Get_IsPlay();
	if (!isPlay)
		// 포인트 시각화 함수 호출
		VisualizeCameraPoints(points, fTimeDelta);
}


void CIMGUI_Camera_Tab::IMGUI_Delete_Point(_int index)
{
	// 포인트 삭제
	m_pMainCamera->Remove_Point(index, m_iSelected_Animation);

	// 선택된 포인트가 삭제된 경우 선택 해제
	if (m_selectedPoint == index) {
		m_selectedPoint = -1;
	}
	else if (m_selectedPoint > index) {
		// 삭제된 포인트 이전의 포인트가 삭제되면, 선택된 포인트의 인덱스가 감소합니다.
		m_selectedPoint -= 1;
	}
}

void CIMGUI_Camera_Tab::IMGUI_Modify_Point(_int index)
{
	if (m_selectedPoint == index) {
		// 이미 선택된 상태라면 선택 해제
		m_selectedPoint = -1;
		m_isEditing = false;
	}
	else {
		// 새로운 포인트 선택
		m_selectedPoint = index;
		m_isEditing = true;

		// 선택된 포인트의 데이터를 임시 변수에 복사
		vector<CameraPoint>& points = m_pMainCamera->Get_VectorPoint(m_iSelected_Animation);
		if (index >= 0 && index < static_cast<int>(points.size())) {
			CameraPoint& point = points[index];
			m_tempPointData.duration = point.duration;
			m_tempPointData.interpType = (CVirtual_Camera::InterpolationType)point.interpolationType;
			m_tempPointData.damping = point.damping;
		}

		// 선택된 포인트의 위치와 회전으로 카메라 업데이트
		m_pMainCamera->Move_Point(index, m_iSelected_Animation);
	}
}

void CIMGUI_Camera_Tab::IMGUI_Modify_Point_UI(_int index)
{
	// 해당 포인트에 대한 참조 가져오기
	std::vector<CameraPoint>& points = m_pMainCamera->Get_VectorPoint(m_iSelected_Animation);

	// 선택된 포인트와 수정 모드인지 확인
	if (!m_isEditing || m_selectedPoint != index) {
		return;
	}

	ImGui::Indent(); // 들여쓰기 시작

	// Duration 수정 (임시 변수에 바인딩)
	ImGui::InputFloat("Duration", &m_tempPointData.duration, 0.1f, 1.0f, "%.2f");

	// Interpolation Type 수정 (임시 변수에 바인딩)
	const char* interp_options[] = { "Linear", "Damping", "Skip" };
	int interpIndex = static_cast<int>(m_tempPointData.interpType);
	if (ImGui::BeginCombo("Interpolation Type", interp_options[interpIndex])) {
		for (int n = 0; n < IM_ARRAYSIZE(interp_options); n++) {
			bool is_selected = (interpIndex == n);
			if (ImGui::Selectable(interp_options[n], is_selected)) {
				m_tempPointData.interpType = static_cast<CVirtual_Camera::InterpolationType>(n);
			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	// Damping 계수 수정 (Damping Mode일 때만 표시)
	if (m_tempPointData.interpType == CVirtual_Camera::InterpolationType::INTERPOLATION_DAMPING_MODE)
	{
		ImGui::SliderFloat("Damping Coefficient", &m_tempPointData.damping, 0.0f, 2.0f, "%.2f");
	}

	// "Save" 버튼 추가
	if (ImGui::Button("Save")) {
		// 사용자에게 저장됨을 알림
		IMGUI_Point_Modify_Save();
	}

	ImGui::Unindent(); // 들여쓰기 종료
}

void CIMGUI_Camera_Tab::IMGUI_Point_Modify_Save()
{
	// 선택된 포인트 인덱스 확인
	if (m_selectedPoint < 0) {
		return;
	}

	vector<CameraPoint>& points = m_pMainCamera->Get_VectorPoint(m_iSelected_Animation);
	if (m_selectedPoint >= static_cast<int>(points.size())) {
		return;
	}

	// 선택된 포인트에 임시 데이터 적용
	CameraPoint& point = points[m_selectedPoint];
	point.duration = m_tempPointData.duration;
	point.interpolationType = m_tempPointData.interpType;
	point.damping = m_tempPointData.damping;

	m_pMainCamera->Modify_Transform(m_selectedPoint, m_iSelected_Animation);
}

const _float4x4* CIMGUI_Camera_Tab::Get_Model_Float4x4()
{
	CGameObject* model = nullptr;
	CTransform* modelTransform = nullptr;

	switch (m_iSelected_Model)
	{
	case CAMERA_MODELID_NORMAL:
		return nullptr;
	case CAMERA_MODELID_SON:
		model = m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Character"), 0);
		break;
	case CAMERA_MODELID_21:
		model = m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Character"));
		break;
	case CAMERA_MODELID_HIT:
		model = m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Character"), 0);
		break;
	case CAMERA_MODELID_FRIEZA:
		model = m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Character"));
		break;
	}
	// 해당모델의 Transform에서 월드매트리스 Ptr이 있어야 한다.
	// 각 카메라에 매핑된 모델의 Transform을 가져오는것도 만들긴해야함

	modelTransform = static_cast<CTransform*>(model->Get_Component(TEXT("Com_Transform")));

	return  modelTransform->Get_WorldMatrixPtr();
}

void CIMGUI_Camera_Tab::VisualizeCameraPoints(const vector<CameraPoint>& points, _float fTimeDelta)
{
	if (points.size() < 2)
		return;

	if (m_pLineDraw == nullptr)
	{
		m_pLineDraw = static_cast<CLine_Draw*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Line_Draw")));
		return;
	}

	// **플레이어의 방향 가져오기 (1이면 그대로, -1이면 반전)**
	CCharacter* character = static_cast<CCharacter*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Character"), 0));
	_int direction = character->Get_iDirection();


	for (size_t i = 0; i < points.size() - 1; ++i)
	{
		const CameraPoint& point = points[i];
		const CameraPoint& nextPoint = points[i + 1];

		_float3 startPos = point.position;
		_float3 endPos = nextPoint.position;

		// **direction에 따른 포지션 조정**
		if (direction == -1)
		{
			// x축 부호 반전
			startPos.x = -startPos.x;
			endPos.x = -endPos.x;
		}

		if (point.hasWorldFloat4x4 && point.pWorldFloat4x4)
		{
			_vector localPos = XMLoadFloat3(&startPos);
			_matrix worldMatrix = XMLoadFloat4x4(point.pWorldFloat4x4);

			// **스케일링 제거를 위한 행렬 분해**
			_vector modelScale;
			_vector modelRotationQuat;
			_vector modelTranslation;
			XMMatrixDecompose(&modelScale, &modelRotationQuat, &modelTranslation, worldMatrix);

			// **스케일링이 제거된 월드 행렬 재구성**
			_matrix rotationMatrix = XMMatrixRotationQuaternion(modelRotationQuat);
			_matrix translationMatrix = XMMatrixTranslationFromVector(modelTranslation);
			_matrix worldMatrixNoScale = rotationMatrix * translationMatrix;

			// 로컬 포지션을 스케일링이 제거된 월드 행렬로 변환
			_vector worldPosVec = XMVector3TransformCoord(localPos, worldMatrixNoScale);

			XMStoreFloat3(&startPos, worldPosVec);
		}

		if (nextPoint.hasWorldFloat4x4 && nextPoint.pWorldFloat4x4)
		{
			_vector localPos = XMLoadFloat3(&endPos);
			_matrix worldMatrix = XMLoadFloat4x4(nextPoint.pWorldFloat4x4);

			// **스케일링 제거를 위한 행렬 분해**
			_vector modelScale;
			_vector modelRotationQuat;
			_vector modelTranslation;
			XMMatrixDecompose(&modelScale, &modelRotationQuat, &modelTranslation, worldMatrix);

			// **스케일링이 제거된 월드 행렬 재구성**
			_matrix rotationMatrix = XMMatrixRotationQuaternion(modelRotationQuat);
			_matrix translationMatrix = XMMatrixTranslationFromVector(modelTranslation);
			_matrix worldMatrixNoScale = rotationMatrix * translationMatrix;

			// 로컬 포지션을 스케일링이 제거된 월드 행렬로 변환
			_vector worldPosVec = XMVector3TransformCoord(localPos, worldMatrixNoScale);

			XMStoreFloat3(&endPos, worldPosVec);
		}

		m_pLineDraw->Set_LinePoints(points, startPos, endPos);
		m_pLineDraw->Render(fTimeDelta);
	}

	m_pLineDraw->TextRender(points, fTimeDelta);
}

void CIMGUI_Camera_Tab::DrawDebugText(const _float3& position, const std::wstring& text, const _float4& color)
{
	// 월드 좌표를 스크린 좌표로 변환
	_vector worldPos = XMLoadFloat3(&position);
	_matrix viewMatrix = m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW);    // 뷰 행렬 가져오기
	_matrix projMatrix = m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ);   // 프로젝션 행렬 가져오기

	_vector screenPos = XMVector3TransformCoord(worldPos, viewMatrix * projMatrix);

	// 스크린 좌표를 가져옵니다.
	_float screenX = (XMVectorGetX(screenPos) + 1.0f) * 0.5f * g_iWinSizeX;
	_float screenY = (1.0f - XMVectorGetY(screenPos)) * 0.5f * g_iWinSizeY;

	// 텍스트를 화면에 렌더링
	//AddText(ImVec2(screenX, screenY), ImGui::ColorConvertFloat4ToU32(ImVec4(color.x, color.y, color.z, color.w)), text.c_str());
}


void CIMGUI_Camera_Tab::IMGUI_Button()
{
	// 현재 활성화된 가상 카메라 가져오기
	int cameraIndex = static_cast<int>(m_pMainCamera->Get_Virtual_Camera());

	// 사용자 입력을 저장할 변수
	static float duration = 1.0f;
	static int selected_interp = 0;
	static float damping = 1.0f; // 기본 Damping 계수
	const char* interp_options[] = { "Linear", "Damping", "Skip" }; // 새로운 모드 추가

	// Add_Point 버튼과 입력 필드 배치
	ImGui::Separator();
	ImGui::Text("Add Camera Point:");
	ImGui::InputFloat("Duration", &duration, 0.1f, 1.0f, "%.2f");

	if (ImGui::BeginCombo("Interpolation Type", interp_options[selected_interp])) {
		for (int n = 0; n < IM_ARRAYSIZE(interp_options); n++) {
			bool is_selected = (selected_interp == n);
			if (ImGui::Selectable(interp_options[n], is_selected))
				selected_interp = n;
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	// Damping 계수 수정 (Damping Mode일 때만 표시)
	if (selected_interp == 1) // "Damping" 선택 시
	{
		ImGui::SliderFloat("Damping Coefficient", &damping, 0.0f, 2.0f, "%.2f");
	}

	// Add Point와 Delete 버튼을 같은 라인에 배치
	if (ImGui::Button("Add Point")) {
		// Interpolation Type 설정
		CVirtual_Camera::InterpolationType interpType = CVirtual_Camera::InterpolationType::INTERPOLATION_LINEAR_MODE;
		switch (selected_interp) {
		case 0:
			interpType = CVirtual_Camera::InterpolationType::INTERPOLATION_LINEAR_MODE;
			break;
		case 1:
			interpType = CVirtual_Camera::InterpolationType::INTERPOLATION_DAMPING_MODE;
			break;
		case 2:
			interpType = CVirtual_Camera::InterpolationType::INTERPOLATION_SKIP_MODE;
			break;
		}

		//설정된 모델의 월드행렬 포인트 주소 저장하기
		const _float4x4* worldMatrixPtr = Get_Model_Float4x4();
		_bool hasWorldFloat4x4 = worldMatrixPtr == nullptr ? false : true;

		// 메인 카메라의 Add_Point 함수를 호출하여 포인트 추가
		m_pMainCamera->Add_Point(duration, interpType, worldMatrixPtr, (interpType == CVirtual_Camera::InterpolationType::INTERPOLATION_DAMPING_MODE) ? damping : 1.0f, hasWorldFloat4x4, m_iSelected_Animation);

		// 사용자에게 추가됨을 알림
		ImGui::TextColored(ImVec4(0, 1, 0, 1), "Added new camera point.");

		// 입력 필드 초기화
		duration = 1.0f;
		selected_interp = 0;
		damping = 1.0f;
	}

	if (ImGui::Button("Add Default_Point")) {
		// Interpolation Type 설정
		CVirtual_Camera::InterpolationType interpType = CVirtual_Camera::InterpolationType::INTERPOLATION_LINEAR_MODE;
		switch (selected_interp) {
		case 0:
			interpType = CVirtual_Camera::InterpolationType::INTERPOLATION_LINEAR_MODE;
			break;
		case 1:
			interpType = CVirtual_Camera::InterpolationType::INTERPOLATION_DAMPING_MODE;
			break;
		case 2:
			interpType = CVirtual_Camera::InterpolationType::INTERPOLATION_SKIP_MODE;
			break;
		}

		//설정된 모델의 월드행렬 포인트 주소 저장하기
		const _float4x4* worldMatrixPtr = Get_Model_Float4x4();
		_bool hasWorldFloat4x4 = worldMatrixPtr == nullptr ? false : true;

		// 메인 카메라의 Add_Point 함수를 호출하여 포인트 추가
		m_pMainCamera->Add_NormalPoint(duration, interpType, worldMatrixPtr, (interpType == CVirtual_Camera::InterpolationType::INTERPOLATION_DAMPING_MODE) ? damping : 1.0f, hasWorldFloat4x4, m_iSelected_Animation);

		// 사용자에게 추가됨을 알림
		ImGui::TextColored(ImVec4(0, 1, 0, 1), "Added new camera point.");

		// 입력 필드 초기화
		duration = 1.0f;
		selected_interp = 0;
		damping = 1.0f;
	}

	// Add a small space and place Delete button on the same line
	ImGui::SameLine();
	if (ImGui::Button("Delete")) {
		m_pMainCamera->Delete_Points(m_iSelected_Animation);
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "All camera points have been deleted.");
	}


	// 플레이 버튼
	IMGUI_Play_Button();
	// 일시정지 버튼
	IMGUI_Pause_Button();
	// 정지 버튼
	IMGUI_Stop_Button();
}

void CIMGUI_Camera_Tab::IMGUI_Delete_Points()
{
	//현재 가상카메라의 포인트들을 전부 지운다.
	if (ImGui::Button("Delete")) {
		m_pMainCamera->Delete_Points(m_iSelected_Animation);
	}
}

void CIMGUI_Camera_Tab::IMGUI_Play_Button()
{
	ImVec2 button_size = ImVec2(20, 20);
	if (ImGui::InvisibleButton("play_button", button_size))
	{
		CGameObject* gameObject_1P = m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Character"));
		m_pMainCamera->IMGUI_Play(m_iSelected_Animation, gameObject_1P);
	}

	ImVec2 p = ImGui::GetItemRectMin();
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	float size = 20.0f;
	ImVec2 triangle_pos[3] = {
		ImVec2(p.x + (button_size.x - size) * 0.5f, p.y),
		ImVec2(p.x + (button_size.x + size) * 0.5f, p.y + size / 2),
		ImVec2(p.x + (button_size.x - size) * 0.5f, p.y + size)
	};

	ImU32 triangle_color;

	if (ImGui::IsItemHovered())
		triangle_color = IM_COL32(155, 255, 155, 255);
	else
		triangle_color = IM_COL32(85, 255, 85, 255);

	draw_list->AddTriangleFilled(triangle_pos[0], triangle_pos[1], triangle_pos[2], triangle_color);
}

void CIMGUI_Camera_Tab::IMGUI_Pause_Button()
{
	ImVec2 button_size = ImVec2(20, 20);
	ImGui::SameLine();
	if (ImGui::InvisibleButton("pause_button", button_size))
		m_pMainCamera->Pause();

	ImVec2 p = ImGui::GetItemRectMin();
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	float size = 20.0f;

	ImVec2 pause_rect1_pos[2] = {
		ImVec2(p.x + 4, p.y),
		ImVec2(p.x + 8, p.y + size)
	};
	ImVec2 pause_rect2_pos[2] = {
		ImVec2(p.x + 12, p.y),
		ImVec2(p.x + 16, p.y + size)
	};
	ImU32 pause_color = IM_COL32(255, 255, 65, 255);
	if (ImGui::IsItemHovered())
		pause_color = IM_COL32(255, 255, 185, 255);

	draw_list->AddRectFilled(pause_rect1_pos[0], pause_rect1_pos[1], pause_color);
	draw_list->AddRectFilled(pause_rect2_pos[0], pause_rect2_pos[1], pause_color);

}

void CIMGUI_Camera_Tab::IMGUI_Stop_Button()
{
	ImVec2 button_size = ImVec2(20, 20);
	ImGui::SameLine();
	if (ImGui::InvisibleButton("reset_button", button_size))
		m_pMainCamera->Button_Stop();

	ImVec2 p = ImGui::GetItemRectMin();
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	float size = 20.0f;

	ImVec2 reset_rect_pos[2] = {
		ImVec2(p.x, p.y),
		ImVec2(p.x + size, p.y + size)
	};
	ImU32 reset_color = IM_COL32(255, 85, 85, 255);
	if (ImGui::IsItemHovered())
		reset_color = IM_COL32(255, 135, 135, 255);

	draw_list->AddRectFilled(reset_rect_pos[0], reset_rect_pos[1], reset_color);
}


void CIMGUI_Camera_Tab::IMGUI_Save_Button()
{
	if (ImGui::Button("Save"))
	{
		// CameraSaveData 구조체 초기화
		CameraSaveData saveData;

		// 모델별 데이터 순회
		for (const auto& modelPair : m_ModelSkills)
		{
			//모델 Enum
			CAMERA_MODELID modelID = modelPair.first;
			//모델 string
			const vector<string>& skills = modelPair.second;

			CameraSaveData::ModelData modelData;
			//모델 ID
			modelData.modelID = (_int)modelID;

			//모델들 순회
			for (size_t skillIdx = 0; skillIdx < skills.size(); ++skillIdx)
			{
				//스킬 이름
				const string& skillName = skills[skillIdx];
				CameraSaveData::ModelData::SkillData skillData;
				skillData.skillName = skillName;

				// 해당 모델과 스킬에 매핑된 카메라 인덱스 찾기
				auto cameraKey = make_pair(modelID, static_cast<int>(skillIdx));
				auto cameraIt = m_CameraIndexMap.find(cameraKey);

				_int cameraIndex = cameraIt->second;

				//각 모델과 스킬에 따른 버츄얼 카메라
				CVirtual_Camera* virtualCamera = m_pMainCamera->m_vecVirtualCamera[cameraIndex];

				// 해당 스킬에 대한 애니메이션 목록 가져오기
				auto animIt = m_SkillAnimations.find(cameraKey);

				//애니메이션들의 이름
				const vector<string>& animations = animIt->second;

				for (size_t animIdx = 0; animIdx < animations.size(); ++animIdx)
				{
					const string& animationName = animations[animIdx];
					CameraSaveData::ModelData::SkillData::AnimationData animData;
					animData.animationName = animationName;

					// 해당 애니메이션의 포인트 가져오기
					vector<CameraPoint> points = virtualCamera->m_mapPoints[static_cast<int>(animIdx)];
					animData.points = points;

					skillData.animations.push_back(animData);
				}

				modelData.skills.push_back(skillData);
			}

			saveData.models.push_back(modelData);
		}

		// 파일 매니저를 통해 저장
		HRESULT hr = m_pGameInstance->Save_All_CameraPoints(filename, &saveData);
	}
}


CIMGUI_Camera_Tab* CIMGUI_Camera_Tab::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CIMGUI_Camera_Tab* pInstance = new CIMGUI_Camera_Tab(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CIMGUI_Camera_Tab"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIMGUI_Camera_Tab::Free()
{
	Safe_Release(m_pMainCamera);
	Safe_Release(m_pLineDraw);

	__super::Free();
}
