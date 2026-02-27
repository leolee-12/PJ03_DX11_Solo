#include "stdafx.h"
#ifdef _DEBUG
#include "Imgui_Tab_Object.h"

#include "ImGuiFileDialog.h"
#include "Imgui_Manager.h"
#include "Imgui_Window_MapEdit.h"
#include "Imgui_Tab_Instance.h"
#include "Imgui_GroupObject.h"
#include "Imgui_Tab_Map_AnimObject.h"

#include "Camera_Dynamic.h"
#include "Camera_ThirdPerson.h"

#include "CommonModelComp.h"
#include "ModelBinary.h"

#include "Cell.h"
#include "MapObject.h"

#include "Camera_Manager.h"
#include "Data_Manager.h"

#include <iostream>
#include <filesystem>

#include "ModelBinary.h"
#include "Utility_File.h"

namespace fs = std::filesystem;

CImgui_Tab_Object::CImgui_Tab_Object(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CImgui_MapToolBase(pGameObjectList, pDevice, pContext)
{
	m_pTab_Instance = CImgui_Tab_Instance::Create(pGameObjectList, m_pDevice, m_pContext);
	m_pGroupObject = CImgui_GroupObject::Create(pGameObjectList, m_pDevice, m_pContext);
	m_pMap_AnimObject = CImgui_Tab_Map_AnimObject::Create(pGameObjectList, m_pDevice, m_pContext);
	m_GameObjectList = pGameObjectList;
}

HRESULT CImgui_Tab_Object::Initialize()
{

	return S_OK;
}

void CImgui_Tab_Object::Tick(_cref_time fTimeDelta)
{
}

HRESULT CImgui_Tab_Object::Render()
{
	if (m_bTestModelCreate)
	{
		LoadFolder();

		const char* str = "TestModel";

		Create_Object(wstring(str, &str[strlen(str)]), _float3(0.f, 0.f, 0.f), nullptr);

		m_pTestObject = m_pGameObjectList[m_iCreateLayerType]->front();
		//m_pGameObjectList[L_OBJECT]->erase(m_pGameObjectList[L_OBJECT]->begin());

		m_pTestObject->Get_TransformCom().lock()->Set_Scaling(0.3f, 0.3f, 0.3f);

		for (auto iter : *(m_pGameInstance->Get_ObjectList(LEVEL_TOOL, L_CAMERA)))
		{
			if (typeid(*iter) == typeid(CCamera_Dynamic))
				m_pCamera = iter;
			if (typeid(*iter) == typeid(CCamera_ThirdPerson))
				m_pThirdCamera = iter;
		}

		m_bTestModelCreate = false;

		m_pTab_Instance->Set_TestModel(m_pTestObject);

		if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/Map_Data/EffectTestMap.json", LEVEL_TOOL, L_TOOLMAP)))
			RETURN_EFAIL;

		if (FAILED(CData_Manager::GetInstance()->Load_ObjectData("../Bin/Data/Collider_Data/TestColl.json", LEVEL_TOOL, L_TOOLMAP)))
			RETURN_EFAIL;

	}

	ImGui::SeparatorText(m_szModeName.c_str());

	ImGui::RadioButton("Object", &m_iCurrentMode, 0);
	ImGui::SameLine();
	ImGui::RadioButton("Instance", &m_iCurrentMode, 1);
	ImGui::SameLine();
	ImGui::RadioButton("Group", &m_iCurrentMode, 2);
	ImGui::SameLine();
	ImGui::RadioButton("AnimObject", &m_iCurrentMode, 3);

	if (ImGui::Button("TestModel_On"))
	{
		m_pTestObject->Get_ModelCom().lock()->Active_AllMeshes();
		m_pTestObject->TurnOn_State(OBJSTATE::Active);
	}
	ImGui::SameLine();
	if (ImGui::Button("TestModel_Off"))
	{
		m_pTestObject->Get_ModelCom().lock()->Deactive_AllMeshes();
		m_pTestObject->TurnOff_State(OBJSTATE::Active);
	}
	m_szModeName = "Object_Mode";

	static _float fSpeed = 0.f;

	fSpeed = CCamera_Manager::GetInstance()->Get_CamSpeed();

	ImGui::DragFloat("Camera_Speed", &fSpeed, 0.1f, 0.f, 80.f);

	CCamera_Manager::GetInstance()->Set_CamSpeed(fSpeed);

	OffsetTestModel();

	/*========== Render ================*/


	switch (m_iCurrentMode)
	{
	case 0:
	{

		if (m_pGameInstance->Key_Down(DIK_T))
		{
			if (m_bCreateMode)
				m_bCreateMode = false;
			else
				m_bCreateMode = true;
		}

		if (m_pGameInstance->Key_Down(DIK_Y))
		{
			if (m_bButton)
			{
				m_bButton = false;
				m_szButtonMode = " Current_Tag : Model_Tag";
			}
			else
			{
				m_bButton = true;
				m_szButtonMode = " Current_Tag : Object_Tag";
			}
		}

		if (m_pGameInstance->Key_Down(DIK_O))
		{
			fSpeed = CCamera_Manager::GetInstance()->Get_CamSpeed();

			if (fSpeed < 1.f)
			{
				fSpeed = 5.f;
			}
			else
			{
				fSpeed = 0.f;
			}

			CCamera_Manager::GetInstance()->Set_CamSpeed(fSpeed);
		}
		if (m_pGameInstance->Key_Down(DIK_DELETE))
		{
			ImGui::OpenPopup("Delete_Msg");
		}

		ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.2f, 0.2f, 0.2f, 0.9f));
		if (ImGui::BeginPopupModal("Delete_Msg"))
		{
			string strMsg;
			strMsg = WstrToStr(dynamic_pointer_cast<CMapObject>(m_pPickObject)->Get_ModelTag() + L"_" + m_pPickObject->Get_ObjectTag());
			strMsg += " \nDelete The Model?";
			ImGui::Text(strMsg.c_str());

			ImGui::NextColumn();

			if (ImGui::Button(" YES "))
			{
				Delete_Object();
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button(" NO "))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		ImGui::PopStyleColor();
		ImGui::SeparatorText("ListBox");

		RenderListBox();

		ImGui::Text(m_szButtonMode.c_str());

		ImGui::SeparatorText("Model_Value_Change");
		/*==== Object Create ====*/
		static char str[128] = "";
		static _float3 vCreatePos = { 0.f,0.f,0.f };

		if (m_pPickObject != nullptr)
		{
			wstring str = TEXT(" Choice Object_Tag : ");

			ImGui::Text(Engine::WstrToStr(str + m_pPickObject->Get_ObjectTag()).c_str());

			wstring str2 = TEXT(" Model_Tag : ");

			ImGui::Text(Engine::WstrToStr(str2 + m_szModelName).c_str());
		}

		ImGui::InputText(" Object_Tag", str, IM_ARRAYSIZE(str));
		ImGui::SameLine();
		if (ImGui::Button("Change_Tag"))
		{
			if (CImgui_Manager::GetInstance()->Find_ObjectTag(L_OBJECT, wstring(str, &str[strlen(str)])))
				MSG_BOX("TAG IS ALEADY IN USE");
			else if (m_pPickObject != nullptr)
				m_pPickObject->Set_ObjectTag(wstring(str, &str[strlen(str)]));
		}

		ImGui::RadioButton("BOX", &m_iModelType, 0);
		ImGui::SameLine();
		ImGui::RadioButton("CYLINDER", &m_iModelType, 1);
		ImGui::SameLine();
		ImGui::RadioButton("CONE", &m_iModelType, 2);
		ImGui::SameLine();
		ImGui::RadioButton("NONE", &m_iModelType, 3);

		if (m_pPickObject != nullptr)
		{
			dynamic_pointer_cast<CMapObject>(m_pPickObject)->Set_ModelType((CMapObject::MODEL_TYPE)m_iModelType);
		}

		if (ImGui::TreeNode("Create_Mode"))
		{

			//ImGui::DragFloat3("Create_Position", &vCreatePos.x);

			if (ImGui::Button("Create_Object"))
			{
				Create_Object(wstring(str, &str[strlen(str)]), vCreatePos, nullptr);
			}
			ImGui::SameLine();
			if (ImGui::Button("Delete_Object"))
			{
				Delete_Object();
			}

			ImGui::SameLine();
			if (ImGui::Button("All_Delete"))
			{
				for (int i = 0; i < m_ObjectList.size(); i++)
				{
					if (m_ObjectList[i].second->IsState(OBJSTATE::WillRemoved))
					{
						m_ObjectList.erase(m_ObjectList.begin() + i);
					}
				}
			}

			ImGui::Checkbox(" PickingToCreate ", &m_bCreateMode);
			ImGui::SameLine();
			ImGui::Checkbox(" PickingToChoice ", &m_bChoiceMode);

			//유진 추가
			if (ImGui::TreeNode("LightOBJ Type"))
			{
				if (m_pPickObject != nullptr)
				{
					m_iLightObjectType = (_int)dynamic_pointer_cast<CMapObject>(m_pPickObject)->Get_LightObjectType();
				}

				ImGui::RadioButton("LightOBJ_NONE", &m_iLightObjectType, 0);
				ImGui::RadioButton("LightOBJ_WHITE", &m_iLightObjectType, 1);
				ImGui::RadioButton("LightOBJ_YELLOW", &m_iLightObjectType, 2);
				ImGui::RadioButton("LightOBJ_RED", &m_iLightObjectType, 3);

				if (m_pPickObject != nullptr)
				{
					dynamic_pointer_cast<CMapObject>(m_pPickObject)->Set_LightObjectType((CMapObject::LIGHT_OBJECT_TYPE)m_iLightObjectType);
				}
				ImGui::TreePop();
			}
			///////////

			if (m_bCreateMode)
			{
				m_bChoiceMode = false;
			}

			if (m_bChoiceMode)
			{
				int a = 0;
				m_bCreateMode = false;
			}
			ImGui::TreePop();
		}

		ImGui::SeparatorText("Guizmo");

		if (ImGui::TreeNode("TestModel"))
		{
			Use_ImGuizmo(m_pTestObject, false);

			ImGui::TreePop();
		}

		auto strw = m_pPickObject.get()->Get_ObjectTag();

		if (m_pPickObject != nullptr && m_pPickObject.get()->Get_ObjectTag() != L"TestModel")
		{
			if (ImGui::TreeNode("Object"))
			{
				Use_ImGuizmo(m_pPickObject, true);

				ImGui::TreePop();
			}
		}

		FileSaveLoad();

		Shake_Camera();

		/*==== Picking ====*/

		_float3 vPickMousePos;

		if (m_pGameInstance->Mouse_Down(DIM_LB))
		{
			if (!ImGuizmo::IsOver())
				if (!ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered())
				{
					if (m_bCreateMode)
					{
						_int TempIndex = 0;
						if (CImgui_Manager::GetInstance()->Picking_Object(SP_WORLD, L_OBJECT, &vPickMousePos, &TempIndex, nullptr))
						{
							Create_Object(wstring(str, &str[strlen(str)]), _float3(vPickMousePos.x, vPickMousePos.y, vPickMousePos.z), nullptr);
						}
					}
					else if (m_bChoiceMode)
					{
						_int TempIndex = 0;
						if (CImgui_Manager::GetInstance()->Picking_Object(SP_WORLD, L_OBJECT, &vPickMousePos, &TempIndex, nullptr))
						{
							m_iPickIndex = TempIndex;
							m_pPickObject = m_ObjectList[m_iPickIndex].second;
						}
					}
				}
		}
	}
	break;
	case 1:
		m_szModeName = "Instance_Mode";
		m_pTab_Instance->Render();
		break;
	case 2:
		m_szModeName = "Group_Mode";
		m_pGroupObject->Render();
		break;
	case 3:
		m_szModeName = "AnimObject_Mode";
		m_pMap_AnimObject->Render();
		break;
	}

	return S_OK;
}

void CImgui_Tab_Object::RenderListBox()
{
	ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.2f, 0.2f, 0.2f, 0.9f));

	ImGui::SetNextItemWidth(100.0f);

	if (ImGui::BeginCombo("Folder_Name", WstrToStr(m_FolderNames[m_iModelFolder]).c_str()))
	{
		for (size_t n = 0; n < m_FolderNames.size(); n++)
		{
			bool bSelected = (m_iModelFolder == n);
			if (ImGui::Selectable(WstrToStr(m_FolderNames[n]).c_str(), bSelected))
			{
				m_iModelFolder = n;

				m_ModelList.clear();
				auto selectedFolder = m_FolderNames[m_iModelFolder];
				if (m_mapFolderList.find(selectedFolder) != m_mapFolderList.end())
				{
					for (auto& item : m_mapFolderList[selectedFolder])
					{
						m_ModelList.push_back(item);
					}
				}

				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	ImGui::PopStyleColor();
	ImGui::SameLine();
	if (ImGui::Button("ReNew"))
	{
		LoadFolder();

		m_ObjectList = m_pTab_Instance->Get_ObjectList();
		m_GameObjectList->clear();

		for (auto& iter : m_ObjectList)
		{
			m_GameObjectList->push_back(iter.second);
		}
	}

	ImVec2 vSize = ImVec2(280, 100);
	if (ImGui::TreeNode("Model_Tag"))
	{
		if (ImGui::BeginListBox("1", vSize))
		{
			if (!m_bButton && !m_ModelList.empty())
			{
				if (m_pGameInstance->Key_Down(DIK_DOWN) && m_iPickIndex_ProtoTag < m_ModelList.size() - 1)
				{
					m_iPickIndex_ProtoTag++;
					m_szModelName = m_ModelList[m_iPickIndex_ProtoTag];
					dynamic_pointer_cast<CMapObject>(m_pTestObject)->Change_ModelTag(m_szModelName);
				}
				if (m_pGameInstance->Key_Down(DIK_UP) && m_iPickIndex_ProtoTag > 0 && m_iPickIndex_ProtoTag <= m_ModelList.size() - 1)
				{
					m_iPickIndex_ProtoTag--;
					m_szModelName = m_ModelList[m_iPickIndex_ProtoTag];
					dynamic_pointer_cast<CMapObject>(m_pTestObject)->Change_ModelTag(m_szModelName);
				}
			}

			for (int i = 0; i < m_ModelList.size(); i++)
			{
				wstring Temp = m_ModelList[i];

				Temp = L"[" + Temp + L"] ";

				if (ImGui::Selectable(WstrToStr(Temp).c_str(), m_iPickIndex_ProtoTag == i))
				{

					m_iPickIndex_ProtoTag = i;
					m_szModelName = m_ModelList[i];
					dynamic_pointer_cast<CMapObject>(m_pTestObject)->Change_ModelTag(m_szModelName);
				}

			}
			ImGui::EndListBox();
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Object_Tag"))
	{
		if (ImGui::BeginListBox("2", vSize))
		{
			if (m_bButton && !m_ObjectList.empty())
			{
				if (m_pGameInstance->Key_Down(DIK_DOWN) && m_iPickIndex < m_ObjectList.size() - 1)
				{
					m_iPickIndex++;
					m_pPickObject = m_ObjectList[m_iPickIndex].second;
					m_szModelName = dynamic_pointer_cast<CMapObject>(m_pPickObject)->Get_ModelTag();
				}
				if (m_pGameInstance->Key_Down(DIK_UP) && m_iPickIndex > 0)
				{
					m_iPickIndex--;
					m_pPickObject = m_ObjectList[m_iPickIndex].second;
					m_szModelName = dynamic_pointer_cast<CMapObject>(m_pPickObject)->Get_ModelTag();
				}
			}

			for (int i = 0; i < m_ObjectList.size(); i++)
			{
				shared_ptr<CGameObject> pGameObject = m_ObjectList[i].second;
				wstring wstrTag = L"[" + dynamic_pointer_cast<CMapObject>(pGameObject)->Get_ModelTag() + L"] " + pGameObject->Get_ObjectTag();

				if (i == 0)
				{
					wstrTag = L"Test_Model";
				}

				if (ImGui::Selectable(WstrToStr(wstrTag).c_str(), m_iPickIndex == i))
				{
					m_iPickIndex = i;
					m_pPickObject = pGameObject;
				}
			}
			ImGui::EndListBox();
		}
		ImGui::TreePop();
	}
}

void CImgui_Tab_Object::Delete_Object()
{
	if (m_pPickObject != nullptr)
	{
		m_ObjectList.erase(m_ObjectList.begin() + m_iPickIndex);
		m_pPickObject->TurnOn_State(OBJSTATE::WillRemoved);

		m_iPickIndex -= 1;
		if (m_iPickIndex >= 0)
			m_pPickObject = m_ObjectList[m_iPickIndex].second;
		else
			m_pPickObject = nullptr;

		m_pGameObjectList[L_OBJECT]->erase(m_pGameObjectList[L_OBJECT]->begin() + m_iPickIndex);
		m_pGroupObject->Set_ObjectList(m_ObjectList);
		m_pGroupObject->Set_SaveObjects(m_ObjectList);
		m_pTab_Instance->Set_ObjectList(m_ObjectList);
	}
}

void CImgui_Tab_Object::FileSaveLoad()
{
	ImGui::SeparatorText("File");

	ImVec2 vSize2 = ImVec2(80, 30);

	if (ImGui::Button(" Map Save ", vSize2))
	{
		ImGuiFileDialog::Instance()->OpenDialog("SaveMapData", "Save Map File", ".json", "../Bin/Data/Map_Data/");
	}
	ImGui::SameLine();
	if (ImGui::Button(" Map Load ", vSize2))
	{
		ImGuiFileDialog::Instance()->OpenDialog("LoadMapData", "Load Map File", ".json", "../Bin/Data/Map_Data/");
	}

	if (ImGuiFileDialog::Instance()->Display("SaveMapData"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			vector<shared_ptr<CGameObject>> ObjectList;
			for (auto& pObject : m_ObjectList)
			{
				ObjectList.push_back(pObject.second);
			}
			CData_Manager::GetInstance()->Save_ObjectData(filePathName, &ObjectList, L_OBJECT);
		}
		ImGuiFileDialog::Instance()->Close();
	}

	if (ImGuiFileDialog::Instance()->Display("LoadMapData"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();

			auto pObjectList = m_pGameInstance->Get_ObjectList(LEVEL_TOOL, L_OBJECT);

			auto pTestList = *(m_pGameInstance->Get_ObjectList(LEVEL_TOOL, L_TOOLMAP));

			if (!pTestList.empty())
			{
				for (auto& pObject : pTestList)
				{
					pObject->TurnOn_State(OBJSTATE::WillRemoved);
				}
				pTestList.clear();
			}
			_int i = 0;

			for (auto& pObject : m_ObjectList)
			{
				pObject.second.get()->TurnOn_State(OBJSTATE::WillRemoved);
			}
			m_ObjectList.clear();
			m_pGameObjectList[m_iCreateLayerType]->clear();
			pObjectList->clear();
			m_iCreateIndex = 0;
			CData_Manager::GetInstance()->Load_ObjectData(filePathName, LEVEL_TOOL, L_OBJECT);

			if (!pObjectList->empty())
			{
				for (auto& pGameObject : *pObjectList)
				{
					m_ObjectList.push_back({ (LAYERTYPE)m_iCreateLayerType,pGameObject });
					m_pGameObjectList[m_iCreateLayerType]->push_back(pGameObject);
					m_iPickIndex = m_ObjectList.size() - 1;
					if (pGameObject->Get_ObjectTag() != L"TestModel")
					{
						m_iCreateIndex++;
						wstring strObjectTag = to_wstring(m_iCreateIndex);
						pGameObject->Set_ObjectTag(strObjectTag);
					}
					if (i == 0)
						m_pTestObject = pGameObject;
					else
						m_pPickObject = pGameObject;

					i++;
				}
			}
			m_pGameObjectList[L_OBJECT]->erase(m_pGameObjectList[L_OBJECT]->begin());

			m_pGroupObject->Set_ObjectList(m_ObjectList);
			m_pGroupObject->Set_SaveObjects(m_ObjectList);
			m_pTab_Instance->Set_ObjectList(m_ObjectList);
		}

		ImGuiFileDialog::Instance()->Close();
	}

	if (ImGui::Button(" Laser Save ", vSize2))
	{
		ImGuiFileDialog::Instance()->OpenDialog("SaveLaserata", "Save Laser File", ".json", "../Bin/Data/Laser_Data/");
	}

	if (ImGuiFileDialog::Instance()->Display("SaveLaserata"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			vector<shared_ptr<CGameObject>> ObjectList;
			for (auto& pObject : m_ObjectList)
			{
				if (dynamic_pointer_cast<CMapObject>(pObject.second)->Get_ModelTag() == L"FacilityLaser_01_1stAvenue")
					ObjectList.push_back(pObject.second);
			}
			CData_Manager::GetInstance()->Save_ObjectData(filePathName, &ObjectList, L_OBJECT);
		}
		ImGuiFileDialog::Instance()->Close();
	}
	//ImGui::SameLine();
	//if (ImGui::Button(" Laser Load ", vSize2))
	//{
	//	ImGuiFileDialog::Instance()->OpenDialog("LoadLaserata", "Save Load File", ".json", "../Bin/Data/Laser_Data/");
	//}

	//if (ImGuiFileDialog::Instance()->Display("LoadLaserata"))
	//{
	//	if (ImGuiFileDialog::Instance()->IsOk())
	//	{
	//		string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
	//		vector<_float4> vecPos;
	//		CData_Manager::GetInstance()->Load_LaserData(filePathName, vecPos);
	//	}
	//	ImGuiFileDialog::Instance()->Close();
	//}
}

void CImgui_Tab_Object::Shake_Camera()
{
	auto pCloudCam = GET_SINGLE(CCamera_Manager)->Get_ThirdPersonCamera(Client::CLOUD);

	if (pCloudCam.lock() == nullptr)
		return;

	static char str2[128] = "";
	ImGui::SeparatorText("Shake_Value");
	ImGui::InputText("Shake_Name", str2, IM_ARRAYSIZE(str2));
	ImGui::DragFloat("Origin_Amp", &m_fOrigin_Amp);
	ImGui::DragFloat("Amp", &m_fAmp);
	ImGui::DragFloat("Duration", &m_fDuration);
	ImGui::DragFloat("Speed", &m_fSpeed);

	if (ImGui::Button("Create"))
	{
		

		CAMERASHAKE_DESC Desc = {};
		Desc.fAmplitude = m_fAmp;
		Desc.fDuration = m_fDuration;
		Desc.fOrigin_Amplitude = m_fOrigin_Amp;
		Desc.fSpeed = m_fSpeed;
		m_strName = str2;
		dynamic_pointer_cast<CCamera_ThirdPerson>(pCloudCam.lock())->Change_Shake_Value(m_strName,Desc);
	}
	ImGui::SameLine();
	if (ImGui::Button("Play_Shake"))
	{
		/*auto pCloudCam = GET_SINGLE(CCamera_Manager)->Get_ThirdPersonCamera(Client::CLOUD);

		if (pCloudCam.lock() == nullptr)
			return;*/

		dynamic_pointer_cast<CCamera_ThirdPerson>(pCloudCam.lock())->Set_CameraShake(m_strName, true);
	}
	ImGui::SameLine();
	if (ImGui::Button("Reset_Pos"))
	{
		//m_pThirdCamera->Get_TransformCom().lock()->Set_State(CTransform::STATE_POSITION, _float3(0.f, 0.f, 0.f));
		m_pThirdCamera->Get_TransformCom().lock()->Set_Position(1.f, _float3(0.f, 0.f, 0.f));
	}
	if (m_pGameInstance->Key_DownEx(DIK_SPACE,DIK_MD_LSHIFT))
	{
		/*auto pCloudCam = GET_SINGLE(CCamera_Manager)->Get_ThirdPersonCamera(Client::CLOUD);

		if (pCloudCam.lock() == nullptr)
			return;*/

		dynamic_pointer_cast<CCamera_ThirdPerson>(pCloudCam.lock())->Set_CameraShake(m_strName, true);
	}
}

HRESULT CImgui_Tab_Object::Create_Object(wstring& strObjectTag, _float3 vPosition, void* pArg)
{
	CMapObject::MODEL_DESC Desc;

	if (pArg != nullptr)
	{
		Desc = *(CMapObject::MODEL_DESC*)pArg;
	}

	Desc.vInitialPosition = vPosition;
	Desc.strModelTag = m_szModelName;

	shared_ptr<CGameObject> pGameObject = nullptr;
	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_TOOL, (LAYERTYPE)m_iCreateLayerType,
		TEXT("Prototype_GameObject_MapObject"), &Desc, &pGameObject)))
		RETURN_EFAIL;

	if (nullptr != pGameObject)
	{
		if (strObjectTag[0] == NULL)
		{
			strObjectTag = to_wstring(m_iCreateIndex);
			++m_iCreateIndex;
		}
		else
		{

			if (CImgui_Manager::GetInstance()->Find_ObjectTag((LAYERTYPE)m_iCreateLayerType, strObjectTag))
			{
				MSG_BOX("TAG IS ALEADY IN USE");
				pGameObject->TurnOn_State(OBJSTATE::WillRemoved);
				return S_OK;
			}

		}
		m_ObjectList.push_back({ (LAYERTYPE)m_iCreateLayerType,pGameObject });
		m_pGameObjectList[m_iCreateLayerType]->push_back(pGameObject);
		pGameObject->Set_ObjectTag(strObjectTag);
		m_iPickIndex = m_ObjectList.size() - 1;
		m_pPickObject = pGameObject;
		m_pGroupObject->Set_ObjectList(m_ObjectList);
		m_pGroupObject->Set_SaveObjects(m_ObjectList);
		m_pTab_Instance->Set_ObjectList(m_ObjectList);
	}
	return S_OK;
}

CImgui_Tab_Object* CImgui_Tab_Object::Create(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	CImgui_Tab_Object* pInstance = new CImgui_Tab_Object(pGameObjectList, pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CImgui_Tab_Object");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CImgui_Tab_Object::Free()
{
	m_ObjectList.clear();

	Safe_Release(m_pTab_Instance);
	Safe_Release(m_pGroupObject);
	Safe_Release(m_pMap_AnimObject);
	__super::Free();
}

#endif // DEBUG
