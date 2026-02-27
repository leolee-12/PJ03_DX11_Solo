#include "stdafx.h"
#include "IMGUI_Object_Tab.h"
#include "GameInstance.h"
#include "RenderInstance.h"
#include <codecvt>
#include "Transform.h"
#include "GameObject.h"
#include "ContainerObject.h"
#include "PartObject.h"
#include "Main_Camera.h"
CIMGUI_Object_Tab::CIMGUI_Object_Tab(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CIMGUI_Tab{ pDevice,pContext }
{
}

HRESULT CIMGUI_Object_Tab::Initialize()
{
	return S_OK;
}

void CIMGUI_Object_Tab::Render(_float fTimeDelta)
{
	static bool bShowPrototypeObjects = true;
	static bool bShowLayerObjects = true;
	static bool bShowSettings = true;
	static bool bShowSaveSuccessMessage = false;
	static bool bShowSaveFailMessage = false;

	if (ImGui::BeginTabBar("Object_InnerTabs")) { // 하위 탭 바 시작
		if (ImGui::BeginTabItem("Layer_ObjectList", &bShowLayerObjects)) { // 하위 Layer 탭
			if (bShowLayerObjects) {
				IMGUI_Show_LayerObjects(); // Layer 오브젝트 표시
			}
			ImGui::EndTabItem(); // 하위 Layer 탭 종료
		}

		if (ImGui::BeginTabItem("Prototype_ObjectList", &bShowPrototypeObjects)) { // 하위 Prototype 탭
			if (bShowPrototypeObjects) {
				IMGUI_Show_PrototypeObjects(); // Prototype 오브젝트 표시
			}
			ImGui::EndTabItem(); // 하위 Prototype 탭 종료
		}

		if (ImGui::BeginTabItem("Save", &bShowSettings)) {
			if (ImGui::Button("Save")) { // "Save" 버튼
				IMGUI_Save_Button_Pressed(&bShowSaveSuccessMessage, &bShowSaveFailMessage);
			}

			if (bShowSaveSuccessMessage) {
				ImGui::Text("File saved successfully!");
			}

			if (bShowSaveFailMessage) {
				ImGui::Text("Failed to save file!");
			}

			ImGui::EndTabItem();
		}
		ImGui::EndTabBar(); // 하위 탭 바 종료
	}
}


HRESULT CIMGUI_Object_Tab::IMGUI_Show_PrototypeObjects()
{
	vector<string> objectPrototypes;
	m_pGameInstance->Get_Prototype_Names(&objectPrototypes);
	int selectedItem = -1; // 초기 선택 항목 없음

	static bool showTerrainInput = false;
	static string selectedTerrainType;

	// 리스트 박스 시작
	if (ImGui::BeginListBox("##PrototypesListBox", ImVec2(-FLT_MIN, 30 * ImGui::GetTextLineHeightWithSpacing())))
	{
		for (int i = 0; i < objectPrototypes.size(); i++) {
			if (ImGui::Selectable(objectPrototypes[i].c_str(), selectedItem == i)) {
				selectedItem = i;
				// 오브젝트 생성 로직
				IMGUI_SpawnObjectAtZero(objectPrototypes[i]);

				// 상태 업데이트
				m_bIsClickedPrototype = true;
				m_SelectedPrototype = objectPrototypes[i];
				m_pCurrentMovingObject = nullptr; // 현재 이동 객체 초기화
			}
		}
		ImGui::EndListBox();
	}

	return S_OK;
}

HRESULT CIMGUI_Object_Tab::IMGUI_SpawnObjectAtZero(const std::string& type)
{
	_uint currentLevel = m_pGameInstance->Get_CurrentLevel_Index();

	// 문자열에서 "Prototype_GameObject_" 접두어 제거
	std::string prefix = "Prototype_GameObject_";
	std::string suffix;

	if (type.substr(0, prefix.size()) == prefix) {
		suffix = type.substr(prefix.size());
	}
	else {
		suffix = type; // 접두어가 없다면 전체 타입을 사용
	}

	// Layer 이름 생성
	std::string layerName = "Layer_" + suffix;

	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	std::wstring converted = converter.from_bytes(type);
	std::wstring wLayerName = converter.from_bytes(layerName);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(currentLevel, converted.c_str(), wLayerName)))
		return E_FAIL;

	return S_OK;
}

HRESULT CIMGUI_Object_Tab::IMGUI_Show_LayerObjects()
{
	_uint currentLevel = m_pGameInstance->Get_CurrentLevel_Index();
	ImGui::Columns(2, NULL, true); // 두 개의 컬럼 생성

	//몇번째 레이어를 클릭했는지 알기 위함
	//ShowLayerList(왼쪽 패널)에서 갱신 함.
	static _int iSelectedItemIndex = -1; // 초기 선택 항목 없음

	//현재 레벨에 있는 레이어들의 이름과, 레이어에 속해있는 GameObject list로 각각 가져오기.
	vector<pair<string, list<CGameObject*>>> objectLayersVector;
	m_pGameInstance->Add_Object_Layers_Vector(currentLevel, &objectLayersVector);

	//레이어의 정보들을 왼쪽 Left_Panel에 이동
	ShowLayerList(objectLayersVector, iSelectedItemIndex);
	ImGui::NextColumn(); // 다음 컬럼으로 이동

	//각 레이어들이 갖고있는 개별 게임오브젝트를 보여주는 함수
	ShowObjectListAndControls(objectLayersVector, iSelectedItemIndex);

	ImGui::Columns(1); // 열 병합

	return S_OK;
}

HRESULT CIMGUI_Object_Tab::IMGUI_Save_Button_Pressed(_bool* bShowSaveSuccessMessage, _bool* bShowSaveFailMessage)
{
	// 오브젝트마다의 정보
	vector<FILEDATA> vecFileData;

	// 레벨 정보 
	_uint currentLevel = m_pGameInstance->Get_CurrentLevel_Index();

	vector<pair<wstring, list<CGameObject*>>> objectLayersVector;
	m_pGameInstance->Add_Object_Layers_Vector(currentLevel, &objectLayersVector);

	// 여기에 스킵할 레이어 이름을 정의
	unordered_set<wstring> skipLayers =
	{ L"Layer_Camera",L"Layer_IMGUI_Manager",L"Layer_Terrain",L"Layer_Particle_Manager",L"Layer_Item"
	,L"Layer_Inventory", };

	// "Layer_UI"로 시작하는 모든 레이어를 스킵할 레이어에 추가
	for (const auto& object : objectLayersVector)
	{
		if (object.first.find(L"Layer_UI") == 0 ||
			object.first.find(L"Layer_Particle") == 0)
		{
			skipLayers.insert(object.first);
		}
	}

	for (auto& object : objectLayersVector)
	{
		// 현재 레이어가 스킵할 리스트에 있는지 검사
		if (skipLayers.find(object.first) != skipLayers.end()) {
			continue;  // 스킵할 레이어면 다음 반복으로 넘어감
		}

		for (auto& iter : object.second)
		{
			CTransform* transform = dynamic_cast<CTransform*>(iter->Get_Component(TEXT("Com_Transform")));

			// Layer_ 뒤의 문자열 추출
			wstring layerName = object.first;
			size_t pos = layerName.find(L"Layer_");
			if (pos != wstring::npos) {
				// "Layer_" 다음 문자열 시작 위치는 pos + 6
				wstring suffix = layerName.substr(pos + 6);

				// 새로운 prefix 생성
				wstring newPrefix = L"Prototype_GameObject_" + suffix;

				// 임시 변수를 사용하여 XMVECTOR를 _float3로 변환
				_float3 position;
				_float3 rightRotation;
				_float3 upRotation;
				_float3 lookRotation;

				XMStoreFloat3(&position, transform->Get_State(CTransform::STATE_POSITION));
				XMStoreFloat3(&rightRotation, transform->Get_State(CTransform::STATE_RIGHT));
				XMStoreFloat3(&upRotation, transform->Get_State(CTransform::STATE_UP));
				XMStoreFloat3(&lookRotation, transform->Get_State(CTransform::STATE_LOOK));

				// 이 새로운 문자열을 vecFileData에 추가
				vecFileData.emplace_back(FILEDATA({ true, newPrefix, object.first, currentLevel,
					position, rightRotation, upRotation,lookRotation,transform->Get_Scaled() }));
			}
		}
	}

	// currentLevel에 따라 파일 이름 지정
	wstring filePath;
	switch (currentLevel)
	{
	case LEVEL_GAMEPLAY:
		filePath = L"../Bin/Level_GamePlay_Objects.txt";
		break;
	default:
		*bShowSaveFailMessage = true;
		*bShowSaveSuccessMessage = false;
		return E_FAIL;
	}

	// 이걸 레벨당 사본 객체 리스트를 넘겨줘야 한다.
	HRESULT result = m_pGameInstance->SaveObjects(filePath.c_str(), &vecFileData);
	if (result == S_OK) {
		*bShowSaveSuccessMessage = true;
		*bShowSaveFailMessage = false;
		return S_OK;
	}
	else {
		*bShowSaveFailMessage = true;
		*bShowSaveSuccessMessage = false;
		return E_FAIL;
	}
}

HRESULT CIMGUI_Object_Tab::IMGUI_Click_Button_Release(unordered_set<CGameObject*>& container, CGameObject* gameObject)
{
	for (auto it = container.begin(); it != container.end(); /* 빈 상태 */) {

		if (*it == gameObject)
		{
			gameObject->m_bDead = true;
			it = container.erase(it); // 컨테이너에서 오브젝트 제거
			return S_OK;
		}
		else
		{
			++it; // 이터레이터 증가
		}
	}
	return S_OK;
}

HRESULT CIMGUI_Object_Tab::IMGUI_CopySelectedObject(const wstring& layerName, CGameObject* sourceObject)
{
	// Get the current level index
	_uint currentLevel = m_pGameInstance->Get_CurrentLevel_Index();

	// Extract the prototype suffix from the layer name
	std::wstring prototypeSuffix = layerName.substr(6); // "Layer_" 이후의 문자열 추출
	std::wstring prototypeTag = L"Prototype_GameObject_" + prototypeSuffix;

	// Copy the position and scale from the source object
	CComponent* sourceComponent = sourceObject->Get_Component(TEXT("Com_Transform"));
	CTransform* sourceTransform = static_cast<CTransform*>(sourceComponent);

	FILEDATA copy_DESC{};
	copy_DESC.prototypeTag = prototypeTag;
	copy_DESC.layerName = layerName;
	copy_DESC.rightRotation = { 1.0f, 0.f, 0.f };
	copy_DESC.upRotation = { 0.f, 1.f, 0.f };
	copy_DESC.lookRotation = { 0.f, 0.f, 1.f };

	_vector positionVector = sourceTransform->Get_State(CTransform::STATE_POSITION);
	_vector rightRotationVector = sourceTransform->Get_State(CTransform::STATE_RIGHT);
	_vector upRotationVector = sourceTransform->Get_State(CTransform::STATE_UP);
	_vector lookRotationVector = sourceTransform->Get_State(CTransform::STATE_LOOK);

	XMStoreFloat3(&copy_DESC.position, positionVector);
	XMStoreFloat3(&copy_DESC.rightRotation, rightRotationVector);
	XMStoreFloat3(&copy_DESC.upRotation, upRotationVector);
	XMStoreFloat3(&copy_DESC.lookRotation, lookRotationVector);

	copy_DESC.scale = sourceTransform->Get_Scaled();
	copy_DESC.isParsing = true;

	// Create a new game object using the prototype
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(currentLevel, prototypeTag.c_str(), layerName.c_str(), &copy_DESC)))
		return E_FAIL;

	return S_OK;
}

void CIMGUI_Object_Tab::ShowLayerList(const vector<pair<string, list<CGameObject*>>>& objectLayersVector, _int& _iSelectedItemIndex)
{
	ImGui::BeginChild("LeftPane", ImVec2(150, 0), true); // 왼쪽 창 생성
	//레이어의 개수만큼 for문 돌리기
	for (int i = 0; i < objectLayersVector.size(); i++) {
		//내가 어떤 레이어를 클릭했다면 몇번째 레이어를 클릭했는지 Index 반환
		if (ImGui::Selectable(objectLayersVector[i].first.c_str(), _iSelectedItemIndex == i)) {
			_iSelectedItemIndex = i;
		}
	}

	ImGui::EndChild();
}

void CIMGUI_Object_Tab::ShowObjectListAndControls(const vector<pair<string, list<CGameObject*>>>& objectLayersVector, _int selectedItem)
{
	// 오른쪽 창 생성
	ImGui::BeginChild("RightPane", ImVec2(0, 0), true);

	// 선택된 게임오브젝트가 있다면?
	static CGameObject* pSelectGameObject = nullptr;
	static bool showContainerDetails = false;  // Container Object Details 창의 열림 상태를 추적하기 위한 변수
	static bool showColliderDetails = false;  // Container Object Details 창의 열림 상태를 추적하기 위한 변수

	// 내가 왼쪽 패널에서 특정 레이어를 클릭했을 경우
	if (selectedItem != -1)
	{
		// 레이어의 이름을 가져오기
		string layerName = objectLayersVector[selectedItem].first;
		// 해당 레이어들의 게임오브젝트 list를 가져오기
		const list<CGameObject*>& gameObjects = objectLayersVector[selectedItem].second; // const를 사용하여 오류 해결
		// 레이어들의 게임오브젝트의 번호를 표시하기 위한 인덱스
		_uint index = 0;

		// 레이어의 게임오브젝트를 돌린다.
		for (auto it = gameObjects.begin(); it != gameObjects.end();) {
			CGameObject* gameObject = *it;

			// Selectable 함수는 자체가 매개변수의 문자열을 출력해준다.
			bool isSelected = m_hashSet_selectedGameObjects.find(gameObject) != m_hashSet_selectedGameObjects.end(); // 선택된 상태를 확인
			if (ImGui::Selectable((layerName + " " + std::to_string(index)).c_str(), isSelected)) {
				if (ImGui::GetIO().KeyShift) {
					// Shift 키가 눌린 상태에서 클릭된 경우
					if (isSelected) {
						m_hashSet_selectedGameObjects.erase(gameObject); // 이미 선택된 상태라면 선택 해제
						gameObject->SetImguiSelect(false);
					}
					else {
						m_hashSet_selectedGameObjects.insert(gameObject); // 선택된 상태가 아니라면 선택 추가
						gameObject->SetImguiSelect(true);
					}
				}
				else {
					for (auto& obj : m_hashSet_selectedGameObjects) {
						obj->SetImguiSelect(false); // 기존 선택을 모두 해제
					}

					// Shift 키가 눌리지 않은 상태에서 클릭된 경우
					m_hashSet_selectedGameObjects.clear(); // 기존 선택을 모두 해제
					m_hashSet_selectedGameObjects.insert(gameObject); // 현재 선택을 추가
					gameObject->SetImguiSelect(true); // 현재 선택을 true로 설정
					pSelectGameObject = gameObject;

					if (CContainerObject* containerObject = dynamic_cast<CContainerObject*>(pSelectGameObject)) {
						// Show a new window for CContainerObject
						showContainerDetails = true;
					}

					if (pSelectGameObject->Get_Component(TEXT("Com_Collider")) != nullptr) {
						// Show a new window for CContainerObject
						showColliderDetails = true;
					}
				}
			}

			++it; // 삭제하지 않으면 다음 오브젝트로 이동
			index++;  // 다음 오브젝트에 대해 인덱스 증가
		}
	}

	// 이전 프레임에서 중복 선택했던 객체가 변했을 때 초기화해야함
	if (m_hashSet_selectedGameObjects != m_hashSet_previousSelectedGameObjects) {
		m_uiDeltaPosition = { 0.0f, 0.0f, 0.0f };
		m_uiDeltaRotation = { 0.0f, 0.0f, 0.0f };
		m_uiDeltaScale = { 0.0f, 0.0f, 0.0f };
		m_hashSet_previousSelectedGameObjects = m_hashSet_selectedGameObjects; // 이전 선택된 객체 업데이트
	}

	// 단일 선택이라면
	if (m_hashSet_selectedGameObjects.size() == 1) {
		pSelectGameObject = *m_hashSet_selectedGameObjects.begin();
		if (pSelectGameObject != nullptr) {
			// Handle normal CGameObject transformations
			HandleObjectPosition(pSelectGameObject);
			HandleObjectRotation(pSelectGameObject);
			HandleObjectScale(pSelectGameObject);
			HandleObjectManagement(m_hashSet_selectedGameObjects, pSelectGameObject, objectLayersVector[selectedItem].first);

			// 활성화 상태를 토글하는 버튼 추가
			if (ImGui::Button(pSelectGameObject->IsActive() ? "Deactivate" : "Activate")) {
				pSelectGameObject->SetActive(!pSelectGameObject->IsActive());
			}
		}
	}
	// 중복 선택이라면
	else if (m_hashSet_selectedGameObjects.size() > 1) {
		HandleMultipleObjectControl(m_hashSet_selectedGameObjects);
	}

	ImGui::EndChild();

	if (pSelectGameObject != nullptr && pSelectGameObject->m_bDead)
		return;

	// Show Container Object Details window if required
	if (showContainerDetails && pSelectGameObject) {
		if (ImGui::Begin("Container Object Details", &showContainerDetails)) {
			HandleContainerObject(dynamic_cast<CContainerObject*>(pSelectGameObject));
		}
		ImGui::End();
	}

	if (showColliderDetails && pSelectGameObject) {
		if (ImGui::Begin("Collider Object Details", &showColliderDetails)) {
			HandleAABBColliderObject(pSelectGameObject);
		}
		ImGui::End();
	}
}

void CIMGUI_Object_Tab::HandleContainerObject(CContainerObject* containerObject)
{
	// CContainerObject의 m_Parts 목록을 표시하고 각각의 객체에 대해 조정 가능하게 함
	const vector<CPartObject*>& parts = containerObject->GetParts();

	// 선택된 파츠 오브젝트를 추적하기 위한 정적 변수
	static CPartObject* selectedPart = nullptr;

	// Container Object의 파트 목록을 보여주는 창을 띄움

	for (size_t i = 0; i < parts.size(); ++i) {
		CPartObject* part = parts[i];
		if (part == nullptr)
			continue;

		// RTTI를 사용하여 클래스 이름을 가져오기
		std::string partName = typeid(*part).name();

		bool isSelected = (selectedPart == part); // 선택된 상태를 확인
		if (ImGui::Selectable(partName.c_str(), isSelected)) {
			if (ImGui::GetIO().KeyShift) {
				// Shift 키가 눌린 상태에서 클릭된 경우
				if (isSelected) {
					selectedPart = nullptr; // 이미 선택된 상태라면 선택 해제
					part->SetImguiSelect(false);
				}
				else {
					selectedPart = part; // 선택된 상태가 아니라면 선택 추가
					part->SetImguiSelect(true);
				}
			}
			else {
				if (isSelected) {
					selectedPart = nullptr; // 이미 선택된 상태라면 선택 해제
					part->SetImguiSelect(false);
				}
				else {
					selectedPart = part; // 선택된 상태가 아니라면 선택 추가
					part->SetImguiSelect(true);
				}
			}
		}

		// 선택된 파트 객체의 상세 정보를 같은 창에서 직접 표시
		if (selectedPart == part) {
			ImGui::Separator();
			ImGui::Text("Selected Part Details:");

			// Handle position, rotation, and scale for the selected part
			HandleObjectPosition(part);
			HandleObjectRotation(part);
			HandleObjectScale(part);

			// 활성화 상태를 토글하는 버튼 추가
			if (ImGui::Button(part->IsActive() ? "Deactivate" : "Activate")) {
				part->SetActive(!part->IsActive());
			}

			// 우측 하단에 닫기 버튼 추가
			if (ImGui::Button("Close Details")) {
				selectedPart = nullptr;
			}
		}
	}
}

void CIMGUI_Object_Tab::HandleAABBColliderObject(CGameObject* colliderObject)
{
	CCollider* collider = dynamic_cast<CCollider*>(colliderObject->Get_Component(TEXT("Com_Collider")));
	if (collider == nullptr) return;

	ImGui::Separator();

	HandleColliderTransform(collider);
}

void CIMGUI_Object_Tab::HandleObjectPosition(CGameObject* _pSelectedGameObject)
{
	if (_pSelectedGameObject == nullptr || _pSelectedGameObject->m_bDead)
		return;

	// 객체의 현재 위치를 가져오기
	_float3 currentPos = { 0.0f, 0.0f, 0.0f };

	//객체의 Transform 컴포넌트 가져오기
	CComponent* component = _pSelectedGameObject->Get_Component(TEXT("Com_Transform"));
	CTransform* transform = nullptr;

	if (component != nullptr) {
		transform = static_cast<CTransform*>(component);
		_vector positionVector = transform->Get_State(CTransform::STATE_POSITION);
		//가져온 위치정보를 저장용 _float3으로 변경
		XMStoreFloat3(&currentPos, positionVector);
	}
	else
		return;

	// 키 입력을 처리하여 포지션 이동
	if (bMovementEnabled)
	{
		_float3 deltaPosition = { 0.0f, 0.0f, 0.0f };

		_float offset = 0.1f;

		if (ImGui::IsKeyDown(ImGuiKey_W)) {
			deltaPosition.z += offset;
		}
		if (ImGui::IsKeyDown(ImGuiKey_S)) {
			deltaPosition.z -= offset;
		}
		if (ImGui::IsKeyDown(ImGuiKey_A)) {
			deltaPosition.x -= offset;
		}
		if (ImGui::IsKeyDown(ImGuiKey_D)) {
			deltaPosition.x += offset;
		}
		if (ImGui::IsKeyDown(ImGuiKey_Q)) {
			deltaPosition.y -= offset;
		}
		if (ImGui::IsKeyDown(ImGuiKey_E)) {
			deltaPosition.y += offset;
		}

		// deltaPosition을 _vector로 변환
		_vector deltaVector = XMLoadFloat3(&deltaPosition);

		// currentPosition에 deltaVector를 더함
		_vector currentPosition = XMLoadFloat3(&currentPos);
		currentPosition += deltaVector;

		// 위치 벡터의 w 요소를 1로 설정
		currentPosition = XMVectorSetW(currentPosition, 1.0f);

		// Set_State에 currentPosition을 다시 넘김
		transform->Set_State(CTransform::STATE_POSITION, currentPosition);
		// currentPos를 업데이트된 위치로 설정
		XMStoreFloat3(&currentPos, currentPosition);
	}

	float sliderWidth = 150.0f; // 슬라이더의 너비
	float inputWidth = 50.0f;   // 입력 필드의 너비
	float sliderPower = 0.01f;  // 슬라이더 조정 민감도
	float rangeX = 50.0f; // 슬라이더 범위 설정
	float rangeY = 50.0f;
	float rangeZ = 50.0f;

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 4)); // 요소 간 간격 조정
	ImGui::Separator(); // 구분선 추가
	ImGui::Text("Position: "); // 라벨 추가
	ImGui::PushButtonRepeat(true); // 버튼 반복을 활성화
	ImGui::PushItemWidth(sliderWidth);

	// X 축 조정
	ImGui::PushItemWidth(sliderWidth);
	if (ImGui::Button("-X")) currentPos.x -= 0.1f;
	ImGui::SameLine();
	ImGui::SliderFloat("##PosX", &currentPos.x, -rangeX, rangeX, "%.3f", sliderPower);
	ImGui::SameLine();
	if (ImGui::Button("+X")) currentPos.x += 0.1f;
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	ImGui::InputFloat("##PosXInput", &currentPos.x);
	ImGui::PopItemWidth();
	ImGui::PopItemWidth();

	// Y 축 조정
	ImGui::PushItemWidth(sliderWidth);
	if (ImGui::Button("-Y")) currentPos.y -= 0.1f;
	ImGui::SameLine();
	ImGui::SliderFloat("##PosY", &currentPos.y, -rangeY, rangeY, "%.3f", sliderPower);
	ImGui::SameLine();
	if (ImGui::Button("+Y")) currentPos.y += 0.1f;
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	ImGui::InputFloat("##PosYInput", &currentPos.y);
	ImGui::PopItemWidth();
	ImGui::PopItemWidth();

	// Z 축 조정
	ImGui::PushItemWidth(sliderWidth);
	if (ImGui::Button("-Z")) currentPos.z -= 0.1f;
	ImGui::SameLine();
	ImGui::SliderFloat("##PosZ", &currentPos.z, -rangeZ, rangeZ, "%.3f", sliderPower);
	ImGui::SameLine();
	if (ImGui::Button("+Z")) currentPos.z += 0.1f;
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	ImGui::InputFloat("##PosZInput", &currentPos.z);
	ImGui::PopItemWidth();
	ImGui::PopItemWidth();

	ImGui::PopButtonRepeat(); // 버튼 반복을 비활성화
	ImGui::PopStyleVar();

	_vector newPosition = XMLoadFloat3(&currentPos);
	newPosition = XMVectorSetW(newPosition, 1.0f);

	if (CMain_Camera* camera = dynamic_cast<CMain_Camera*>(_pSelectedGameObject))
	{
		camera->SetPosition(newPosition);
	}
	else
	{
		transform->Set_State(CTransform::STATE_POSITION, newPosition);
	}
}

void CIMGUI_Object_Tab::HandleColliderTransform(CCollider* _pCollider)
{
	// 현재 콜라이더의 원래 위치와 크기를 가져오기
	BoundingBox boundingBox = _pCollider->AABB_GetDesc();
	static _float3 currentPos = { 0.0f, 0.0f, 0.0f }; // 위치를 0으로 초기화
	static _float3 extents = boundingBox.Extents;

	float sliderWidth = 150.0f; // 슬라이더의 너비
	float inputWidth = 50.0f;   // 입력 필드의 너비
	float sliderPower = 0.01f;  // 슬라이더 조정 민감도
	float rangeX = 50.0f; // 슬라이더 범위 설정
	float rangeY = 50.0f;
	float rangeZ = 50.0f;

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 4)); // 요소 간 간격 조정
	ImGui::Separator(); // 구분선 추가
	ImGui::Text("Collider Transform: "); // 라벨 추가
	ImGui::PushButtonRepeat(true); // 버튼 반복을 활성화
	ImGui::PushItemWidth(sliderWidth);

	// 위치 조정
	ImGui::Text("Position:");
	// X 축 조정
	if (ImGui::Button("-X##Pos")) currentPos.x -= 0.1f;
	ImGui::SameLine();
	ImGui::SliderFloat("##ColliderPosX", &currentPos.x, -rangeX, rangeX, "%.3f", sliderPower);
	ImGui::SameLine();
	if (ImGui::Button("+X##Pos")) currentPos.x += 0.1f;
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	ImGui::InputFloat("##ColliderPosXInput", &currentPos.x);
	ImGui::PopItemWidth();

	// Y 축 조정
	if (ImGui::Button("-Y##Pos")) currentPos.y -= 0.1f;
	ImGui::SameLine();
	ImGui::SliderFloat("##ColliderPosY", &currentPos.y, -rangeY, rangeY, "%.3f", sliderPower);
	ImGui::SameLine();
	if (ImGui::Button("+Y##Pos")) currentPos.y += 0.1f;
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	ImGui::InputFloat("##ColliderPosYInput", &currentPos.y);
	ImGui::PopItemWidth();

	// Z 축 조정
	if (ImGui::Button("-Z##Pos")) currentPos.z -= 0.1f;
	ImGui::SameLine();
	ImGui::SliderFloat("##ColliderPosZ", &currentPos.z, -rangeZ, rangeZ, "%.3f", sliderPower);
	ImGui::SameLine();
	if (ImGui::Button("+Z##Pos")) currentPos.z += 0.1f;
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	ImGui::InputFloat("##ColliderPosZInput", &currentPos.z);
	ImGui::PopItemWidth();

	ImGui::Separator();

	// 스케일 조정
	ImGui::Text("Scale:");
	// 스케일 X 축 조정
	if (ImGui::Button("-X##Scale")) extents.x -= 0.01f;
	ImGui::SameLine();
	ImGui::SliderFloat("##ColliderScaleX", &extents.x, 0.1f, 10.0f, "%.3f", sliderPower);
	ImGui::SameLine();
	if (ImGui::Button("+X##Scale")) extents.x += 0.01f;
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	ImGui::InputFloat("##ColliderScaleXInput", &extents.x);
	ImGui::PopItemWidth();

	// 스케일 Y 축 조정
	if (ImGui::Button("-Y##Scale")) extents.y -= 0.01f;
	ImGui::SameLine();
	ImGui::SliderFloat("##ColliderScaleY", &extents.y, 0.1f, 10.0f, "%.3f", sliderPower);
	ImGui::SameLine();
	if (ImGui::Button("+Y##Scale")) extents.y += 0.01f;
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	ImGui::InputFloat("##ColliderScaleYInput", &extents.y);
	ImGui::PopItemWidth();

	// 스케일 Z 축 조정
	if (ImGui::Button("-Z##Scale")) extents.z -= 0.01f;
	ImGui::SameLine();
	ImGui::SliderFloat("##ColliderScaleZ", &extents.z, 0.1f, 10.0f, "%.3f", sliderPower);
	ImGui::SameLine();
	if (ImGui::Button("+Z##Scale")) extents.z += 0.01f;
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	ImGui::InputFloat("##ColliderScaleZInput", &extents.z);
	ImGui::PopItemWidth();

	ImGui::PopButtonRepeat(); // 버튼 반복을 비활성화
	ImGui::PopStyleVar();

	// 현재 위치와 스케일 값을 업데이트된 값으로 설정
	boundingBox.Center = currentPos;
	boundingBox.Extents = extents;
	_pCollider->AABB_SetDesc(boundingBox);
}

void CIMGUI_Object_Tab::HandleObjectRotation(CGameObject* _pSelectedGameObject)
{
	if (_pSelectedGameObject == nullptr || _pSelectedGameObject->m_bDead)
		return;

	// Transform 컴포넌트 가져오기
	CComponent* component = _pSelectedGameObject->Get_Component(TEXT("Com_Transform"));
	if (component == nullptr) return;

	CTransform* transform = static_cast<CTransform*>(component);

	// 월드 행렬에서 회전 행렬 추출
	_matrix worldMatrix = transform->Get_WorldMatrix();
	_matrix rotationMatrix = worldMatrix;
	rotationMatrix.r[3] = XMVectorSet(0, 0, 0, 1); // 위치 벡터를 제거

	// 스케일 값을 노멀라이즈하여 분리
	rotationMatrix.r[0] = XMVector3Normalize(rotationMatrix.r[0]);
	rotationMatrix.r[1] = XMVector3Normalize(rotationMatrix.r[1]);
	rotationMatrix.r[2] = XMVector3Normalize(rotationMatrix.r[2]);

	// 회전 행렬을 오일러 각도로 변환
	_float3 currentRotation = transform->GetEulerAnglesFromRotationMatrix(rotationMatrix);

	// 객체에 대한 회전 상태가 없으면 초기화
	if (m_hashTable_Rotation.find(_pSelectedGameObject) == m_hashTable_Rotation.end()) {
		m_hashTable_Rotation[_pSelectedGameObject] = _float3{
			XMConvertToDegrees(currentRotation.x),
			XMConvertToDegrees(currentRotation.y),
			XMConvertToDegrees(currentRotation.z)
		};
	}

	// 현재 객체의 회전 상태 불러오기
	_float3& displayRotation = m_hashTable_Rotation[_pSelectedGameObject];


	float sliderWidth = 150.0f; // 슬라이더의 너비
	float inputWidth = 50.0f;   // 입력 필드의 너비
	float sliderPower = 1.0f;   // 슬라이더 조정 민감도

	bool rotationChanged = false;

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 4)); // 요소 간 간격 조정
	ImGui::Separator(); // 구분선 추가
	ImGui::Text("Rotation: "); // 라벨 추가
	ImGui::PushButtonRepeat(true); // 버튼 반복을 활성화
	ImGui::PushItemWidth(sliderWidth);

	// X 축 (Pitch) 회전 조정
	if (ImGui::Button("-X")) { displayRotation.x -= 1.0f; rotationChanged = true; }
	ImGui::SameLine();
	if (ImGui::SliderFloat("##RotX", &displayRotation.x, -180.0f, 180.0f, "%.3f", sliderPower)) {
		rotationChanged = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("+X")) { displayRotation.x += 1.0f; rotationChanged = true; }
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	if (ImGui::InputFloat("##RotXInput", &displayRotation.x)) {
		rotationChanged = true;
	}
	ImGui::PopItemWidth();

	// Y 축 (Yaw) 회전 조정
	if (ImGui::Button("-Y")) { displayRotation.y -= 1.0f; rotationChanged = true; }
	ImGui::SameLine();
	if (ImGui::SliderFloat("##RotY", &displayRotation.y, -180.0f, 180.0f, "%.3f", sliderPower)) {
		rotationChanged = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("+Y")) { displayRotation.y += 1.0f; rotationChanged = true; }
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	if (ImGui::InputFloat("##RotYInput", &displayRotation.y)) {
		rotationChanged = true;
	}
	ImGui::PopItemWidth();

	// Z 축 (Roll) 회전 조정
	if (ImGui::Button("-Z")) { displayRotation.z -= 1.0f; rotationChanged = true; }
	ImGui::SameLine();
	if (ImGui::SliderFloat("##RotZ", &displayRotation.z, -180.0f, 180.0f, "%.3f", sliderPower)) {
		rotationChanged = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("+Z")) { displayRotation.z += 1.0f; rotationChanged = true; }
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	if (ImGui::InputFloat("##RotZInput", &displayRotation.z)) {
		rotationChanged = true;
	}
	ImGui::PopItemWidth();

	ImGui::PopButtonRepeat(); // 버튼 반복을 비활성화
	ImGui::PopStyleVar();

	// 회전 값이 변경되었을 때만 업데이트
	if (rotationChanged) {
		_float3 newRotation = {
			XMConvertToRadians(displayRotation.x),
			XMConvertToRadians(displayRotation.y),
			XMConvertToRadians(displayRotation.z)
		};
		transform->Rotation(newRotation);
	}
}

void CIMGUI_Object_Tab::HandleObjectScale(CGameObject* _pSelectedGameObject)
{
	if (_pSelectedGameObject == nullptr || _pSelectedGameObject->m_bDead)
		return;

	// 객체의 Transform 컴포넌트 가져오기
	CComponent* component = _pSelectedGameObject->Get_Component(TEXT("Com_Transform"));
	CTransform* transform = nullptr;
	_float3 currentScale = { 1.0f, 1.0f, 1.0f };

	if (component != nullptr) {
		transform = static_cast<CTransform*>(component);
		currentScale = transform->Get_Scaled();
	}
	else
		return; // Transform 컴포넌트가 없으면 함수를 종료


	float sliderWidth = 150.0f; // 슬라이더의 너비
	float inputWidth = 50.0f;   // 입력 필드의 너비
	float sliderPower = 0.01f;  // 슬라이더 조정 민감도

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 4)); // 요소 간 간격 조정
	ImGui::Separator(); // 구분선 추가
	ImGui::Text("Scale: "); // 라벨 추가
	ImGui::PushButtonRepeat(true); // 버튼 반복을 활성화
	ImGui::PushItemWidth(sliderWidth);

	// 스케일 X 축 조정
	if (ImGui::Button("-SX")) currentScale.x -= 0.01f;
	ImGui::SameLine();
	ImGui::SliderFloat("##ScaleX", &currentScale.x, 0.1f, 10.0f, "%.3f", sliderPower);
	ImGui::SameLine();
	if (ImGui::Button("+SX")) currentScale.x += 0.01f;
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	ImGui::InputFloat("##ScaleXInput", &currentScale.x);
	ImGui::PopItemWidth();

	// 스케일 Y 축 조정
	if (ImGui::Button("-SY")) currentScale.y -= 0.01f;
	ImGui::SameLine();
	ImGui::SliderFloat("##ScaleY", &currentScale.y, 0.1f, 10.0f, "%.3f", sliderPower);
	ImGui::SameLine();
	if (ImGui::Button("+SY")) currentScale.y += 0.01f;
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	ImGui::InputFloat("##ScaleYInput", &currentScale.y);
	ImGui::PopItemWidth();

	// 스케일 Z 축 조정
	if (ImGui::Button("-SZ")) currentScale.z -= 0.01f;
	ImGui::SameLine();
	ImGui::SliderFloat("##ScaleZ", &currentScale.z, 0.1f, 10.0f, "%.3f", sliderPower);
	ImGui::SameLine();
	if (ImGui::Button("+SZ")) currentScale.z += 0.01f;
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	ImGui::InputFloat("##ScaleZInput", &currentScale.z);
	ImGui::PopItemWidth();

	ImGui::PopItemWidth();
	ImGui::PopButtonRepeat(); // 버튼 반복을 비활성화
	ImGui::PopStyleVar();

	// 현재 스케일 값을 업데이트된 스케일로 설정
	if (transform != nullptr) {
		transform->Set_Scaled(currentScale.x, currentScale.y, currentScale.z); // 스케일 적용
	}
}

void CIMGUI_Object_Tab::HandleObjectManagement(unordered_set<CGameObject*>& container, CGameObject* _pSelectedGameObject, const string& layerName)
{
	// 삭제 버튼 추가
	ImGui::Separator(); // 구분선 추가
	if (ImGui::Button("Delete Selected Object")) {
		IMGUI_Click_Button_Release(container, _pSelectedGameObject);
	}

	// 복사 버튼 추가
	ImGui::Separator(); // 구분선 추가
	if (ImGui::Button("Copy Selected Object")) {
		// 레이어 이름과 함께 객체를 복사합니다.
		wstring_convert<std::codecvt_utf8<wchar_t>> converter;
		std::wstring wLayerName = converter.from_bytes(layerName);
		IMGUI_CopySelectedObject(wLayerName, _pSelectedGameObject);
	}
}

void CIMGUI_Object_Tab::HandleMultipleObjectControl(const unordered_set<CGameObject*>& selectedGameObjects)
{
	float sliderWidth = 150.0f; // 슬라이더의 너비
	float inputWidth = 50.0f;   // 입력 필드의 너비
	float sliderPower = 0.01f;  // 슬라이더 조정 민감도

	_float3 actualDeltaPosition = { 0.0f, 0.0f, 0.0f };
	_float3 actualDeltaRotation = { 0.0f, 0.0f, 0.0f };
	_float3 actualDeltaScale = { 0.0f, 0.0f, 0.0f };

	// 위치 조정 UI
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 4)); // 요소 간 간격 조정
	ImGui::Separator(); // 구분선 추가
	ImGui::Text("Position: "); // 라벨 추가
	ImGui::PushButtonRepeat(true); // 버튼 반복을 활성화
	ImGui::PushItemWidth(sliderWidth);

	// X 축 조정
	if (ImGui::Button("-X")) {
		m_uiDeltaPosition.x -= 0.1f;
		actualDeltaPosition.x -= 0.1f;
	}
	ImGui::SameLine();
	if (ImGui::SliderFloat("##PosX", &m_uiDeltaPosition.x, -50.0f, 50.0f, "%.3f", sliderPower)) {
		actualDeltaPosition.x = m_uiDeltaPosition.x;
	}
	ImGui::SameLine();
	if (ImGui::Button("+X")) {
		m_uiDeltaPosition.x += 0.1f;
		actualDeltaPosition.x += 0.1f;
	}
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	if (ImGui::InputFloat("##PosXInput", &m_uiDeltaPosition.x)) {
		actualDeltaPosition.x = m_uiDeltaPosition.x;
	}
	ImGui::PopItemWidth();

	// Y 축 조정
	if (ImGui::Button("-Y")) {
		m_uiDeltaPosition.y -= 0.1f;
		actualDeltaPosition.y -= 0.1f;
	}
	ImGui::SameLine();
	if (ImGui::SliderFloat("##PosY", &m_uiDeltaPosition.y, -50.0f, 50.0f, "%.3f", sliderPower)) {
		actualDeltaPosition.y = m_uiDeltaPosition.y;
	}
	ImGui::SameLine();
	if (ImGui::Button("+Y")) {
		m_uiDeltaPosition.y += 0.1f;
		actualDeltaPosition.y += 0.1f;
	}
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	if (ImGui::InputFloat("##PosYInput", &m_uiDeltaPosition.y)) {
		actualDeltaPosition.y = m_uiDeltaPosition.y;
	}
	ImGui::PopItemWidth();

	// Z 축 조정
	if (ImGui::Button("-Z")) {
		m_uiDeltaPosition.z -= 0.1f;
		actualDeltaPosition.z -= 0.1f;
	}
	ImGui::SameLine();
	if (ImGui::SliderFloat("##PosZ", &m_uiDeltaPosition.z, -50.0f, 50.0f, "%.3f", sliderPower)) {
		actualDeltaPosition.z = m_uiDeltaPosition.z;
	}
	ImGui::SameLine();
	if (ImGui::Button("+Z")) {
		m_uiDeltaPosition.z += 0.1f;
		actualDeltaPosition.z += 0.1f;
	}
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	if (ImGui::InputFloat("##PosZInput", &m_uiDeltaPosition.z)) {
		actualDeltaPosition.z = m_uiDeltaPosition.z;
	}
	ImGui::PopItemWidth();

	ImGui::PopButtonRepeat();
	ImGui::PopStyleVar();

	// 회전 조정 UI
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 4));
	ImGui::Separator();
	ImGui::Text("Rotation: ");
	ImGui::PushButtonRepeat(true);
	ImGui::PushItemWidth(sliderWidth);

	// X 축 (Pitch) 회전 조정
	if (ImGui::Button("-X")) {
		m_uiDeltaRotation.x -= 1.0f;
		actualDeltaRotation.x -= 1.0f;
	}
	ImGui::SameLine();
	if (ImGui::SliderFloat("##RotX", &m_uiDeltaRotation.x, -180.0f, 180.0f, "%.3f", 1.0f)) {
		actualDeltaRotation.x = m_uiDeltaRotation.x;
	}
	ImGui::SameLine();
	if (ImGui::Button("+X")) {
		m_uiDeltaRotation.x += 1.0f;
		actualDeltaRotation.x += 1.0f;
	}
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	if (ImGui::InputFloat("##RotXInput", &m_uiDeltaRotation.x)) {
		actualDeltaRotation.x = m_uiDeltaRotation.x;
	}
	ImGui::PopItemWidth();

	// Y 축 (Yaw) 회전 조정
	if (ImGui::Button("-Y")) {
		m_uiDeltaRotation.y -= 1.0f;
		actualDeltaRotation.y -= 1.0f;
	}
	ImGui::SameLine();
	if (ImGui::SliderFloat("##RotY", &m_uiDeltaRotation.y, -180.0f, 180.0f, "%.3f", 1.0f)) {
		actualDeltaRotation.y = m_uiDeltaRotation.y;
	}
	ImGui::SameLine();
	if (ImGui::Button("+Y")) {
		m_uiDeltaRotation.y += 1.0f;
		actualDeltaRotation.y += 1.0f;
	}
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	if (ImGui::InputFloat("##RotYInput", &m_uiDeltaRotation.y)) {
		actualDeltaRotation.y = m_uiDeltaRotation.y;
	}
	ImGui::PopItemWidth();

	// Z 축 (Roll) 회전 조정
	if (ImGui::Button("-Z")) {
		m_uiDeltaRotation.z -= 1.0f;
		actualDeltaRotation.z -= 1.0f;
	}
	ImGui::SameLine();
	if (ImGui::SliderFloat("##RotZ", &m_uiDeltaRotation.z, -180.0f, 180.0f, "%.3f", 1.0f)) {
		actualDeltaRotation.z = m_uiDeltaRotation.z;
	}
	ImGui::SameLine();
	if (ImGui::Button("+Z")) {
		m_uiDeltaRotation.z += 1.0f;
		actualDeltaRotation.z += 1.0f;
	}
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	if (ImGui::InputFloat("##RotZInput", &m_uiDeltaRotation.z)) {
		actualDeltaRotation.z = m_uiDeltaRotation.z;
	}
	ImGui::PopItemWidth();

	ImGui::PopButtonRepeat();
	ImGui::PopStyleVar();

	// 스케일 조정 UI
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 4));
	ImGui::Separator();
	ImGui::Text("Scale: ");
	ImGui::PushButtonRepeat(true);
	ImGui::PushItemWidth(sliderWidth);

	// X 축 스케일 조정
	if (ImGui::Button("-SX")) {
		m_uiDeltaScale.x -= 0.01f;
		actualDeltaScale.x -= 0.01f;
	}
	ImGui::SameLine();
	if (ImGui::SliderFloat("##ScaleX", &m_uiDeltaScale.x, 0.1f, 10.0f, "%.3f", sliderPower)) {
		actualDeltaScale.x = m_uiDeltaScale.x;
	}
	ImGui::SameLine();
	if (ImGui::Button("+SX")) {
		m_uiDeltaScale.x += 0.01f;
		actualDeltaScale.x += 0.01f;
	}
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	if (ImGui::InputFloat("##ScaleXInput", &m_uiDeltaScale.x)) {
		actualDeltaScale.x = m_uiDeltaScale.x;
	}
	ImGui::PopItemWidth();

	// Y 축 스케일 조정
	if (ImGui::Button("-SY")) {
		m_uiDeltaScale.y -= 0.01f;
		actualDeltaScale.y -= 0.01f;
	}
	ImGui::SameLine();
	if (ImGui::SliderFloat("##ScaleY", &m_uiDeltaScale.y, 0.1f, 10.0f, "%.3f", sliderPower)) {
		actualDeltaScale.y = m_uiDeltaScale.y;
	}
	ImGui::SameLine();
	if (ImGui::Button("+SY")) {
		m_uiDeltaScale.y += 0.01f;
		actualDeltaScale.y += 0.01f;
	}
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	if (ImGui::InputFloat("##ScaleYInput", &m_uiDeltaScale.y)) {
		actualDeltaScale.y = m_uiDeltaScale.y;
	}
	ImGui::PopItemWidth();

	// Z 축 스케일 조정
	if (ImGui::Button("-SZ")) {
		m_uiDeltaScale.z -= 0.01f;
		actualDeltaScale.z -= 0.01f;
	}
	ImGui::SameLine();
	if (ImGui::SliderFloat("##ScaleZ", &m_uiDeltaScale.z, 0.1f, 10.0f, "%.3f", sliderPower)) {
		actualDeltaScale.z = m_uiDeltaScale.z;
	}
	ImGui::SameLine();
	if (ImGui::Button("+SZ")) {
		m_uiDeltaScale.z += 0.01f;
		actualDeltaScale.z += 0.01f;
	}
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	if (ImGui::InputFloat("##ScaleZInput", &m_uiDeltaScale.z)) {
		actualDeltaScale.z = m_uiDeltaScale.z;
	}
	ImGui::PopItemWidth();

	ImGui::PopButtonRepeat();
	ImGui::PopStyleVar();

	// 선택된 모든 객체에 대해 delta 값 적용
	for (CGameObject* gameObject : selectedGameObjects)
	{
		// Transform 컴포넌트 가져오기
		CComponent* component = gameObject->Get_Component(TEXT("Com_Transform"));
		if (component == nullptr) continue;
		CTransform* transform = static_cast<CTransform*>(component);

		// 객체에 대한 회전 상태가 없으면 초기화
		if (m_hashTable_Rotation.find(gameObject) == m_hashTable_Rotation.end()) {
			_matrix worldMatrix = transform->Get_WorldMatrix();
			_matrix rotationMatrix = worldMatrix;
			rotationMatrix.r[3] = XMVectorSet(0, 0, 0, 1); // 위치 벡터를 제거

			// 스케일 값을 노멀라이즈하여 분리
			rotationMatrix.r[0] = XMVector3Normalize(rotationMatrix.r[0]);
			rotationMatrix.r[1] = XMVector3Normalize(rotationMatrix.r[1]);
			rotationMatrix.r[2] = XMVector3Normalize(rotationMatrix.r[2]);

			// 회전 행렬을 오일러 각도로 변환
			_float3 currentRotation = transform->GetEulerAnglesFromRotationMatrix(rotationMatrix);

			m_hashTable_Rotation[gameObject] = _float3{
				XMConvertToDegrees(currentRotation.x),
				XMConvertToDegrees(currentRotation.y),
				XMConvertToDegrees(currentRotation.z)
			};
		}

		// 현재 객체의 회전 상태 불러오기
		_float3& displayRotation = m_hashTable_Rotation[gameObject];

		// 위치 업데이트
		_vector currentPosition = transform->Get_State(CTransform::STATE_POSITION);
		_float3 currentPos;
		XMStoreFloat3(&currentPos, currentPosition);
		currentPos.x += actualDeltaPosition.x;
		currentPos.y += actualDeltaPosition.y;
		currentPos.z += actualDeltaPosition.z;
		_vector newPosition = XMLoadFloat3(&currentPos);
		newPosition = XMVectorSetW(newPosition, 1.0f);
		transform->Set_State(CTransform::STATE_POSITION, newPosition);

		// 회전 업데이트
		displayRotation.x += actualDeltaRotation.x;
		displayRotation.y += actualDeltaRotation.y;
		displayRotation.z += actualDeltaRotation.z;

		_float3 newRotation = {
			XMConvertToRadians(displayRotation.x),
			XMConvertToRadians(displayRotation.y),
			XMConvertToRadians(displayRotation.z)
		};
		transform->Rotation(newRotation);

		// 스케일 업데이트
		_float3 currentScale = transform->Get_Scaled();
		currentScale.x += actualDeltaScale.x;
		currentScale.y += actualDeltaScale.y;
		currentScale.z += actualDeltaScale.z;
		transform->Set_Scaled(currentScale.x, currentScale.y, currentScale.z);
	}
}

CIMGUI_Object_Tab* CIMGUI_Object_Tab::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CIMGUI_Object_Tab* pInstance = new CIMGUI_Object_Tab(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CIMGUI_Object_Tab"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIMGUI_Object_Tab::Free()
{
	__super::Free();
}
