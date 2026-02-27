#include "stdafx.h"
#ifdef _DEBUG
#include "Imgui_Tab_Instance.h"


#include "ImGuiFileDialog.h"
#include "Imgui_Manager.h"  

#include "CommonModelComp.h"
#include "ModelBinary.h"

#include "Cell.h"
#include "MapObject.h"
#include "Instance_Object.h"

#include "Data_Manager.h"

#include <iostream>
#include <filesystem>

#include "Utility_File.h"

namespace fs = std::filesystem;

const char* strLayerTypes[] = { "L_ENV","L_OBJECT","L_INSTANCE","L_PLAYER","L_EFFECT","L_CAMERA","L_UI" };


CImgui_Tab_Instance::CImgui_Tab_Instance(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CImgui_MapToolBase(pGameObjectList, pDevice, pContext)
{
}

HRESULT CImgui_Tab_Instance::Initialize()
{

	return S_OK;
}

void CImgui_Tab_Instance::Tick(_cref_time fTimeDelta)
{
}

HRESULT CImgui_Tab_Instance::Render()
{
	if (ImGui::TreeNode("Object"))
	{
		//오브젝트 리스트박스
		RenderListBox();

		ImGui::TreePop();
	}

	//오브젝트 추가 삭제
	RenderObjectCreate();

	if (ImGui::TreeNode("Instance"))
	{
		//인스턴스 리스트박스
		RenderInstanceList();

		ImGui::TreePop();
	}

	//인스턴스 추가 삭제
	RenderInstanceCreate();

	//에러 메세지 창 내용
	RenderErrorMsg();

	ImGui::SeparatorText("Guizmo");

	if (m_pPickInstance != nullptr)
	{
		Use_ImGuizmo(m_pPickInstance, true);
	}

	//저장 불러오기 다이얼로그
	SaveLoadData();

	return S_OK;
}

void CImgui_Tab_Instance::RenderListBox()
{
	ImVec2 vSize = ImVec2(280, 100);
	ImGui::SeparatorText("Tab_ObjectList");

	if (ImGui::BeginListBox("1", vSize))
	{
		for (int i = 0; i < m_ObjectList.size(); i++)
		{
			wstring strModelTag = dynamic_pointer_cast<CMapObject>(m_ObjectList[i].second)->Get_ModelTag();
			wstring strObjTag = dynamic_pointer_cast<CMapObject>(m_ObjectList[i].second)->Get_ObjectTag();
			wstring wstrTag = L"[" + strModelTag + L"] - " + L"[" + strObjTag + L"]";

			if (ImGui::Selectable(WstrToStr(wstrTag).c_str(), m_iPickIndex == i))
			{
				m_iPickIndex = i;
				m_pPickObject = m_ObjectList[i].second;
			}
		}
		ImGui::EndListBox();
	}

	ImGui::SeparatorText("Instancing_List");

	if (ImGui::BeginListBox("2", vSize))
	{
		for (int i = 0; i < m_InstancingList.size(); i++)
		{
			wstring strModelTag = dynamic_pointer_cast<CMapObject>(m_InstancingList[i])->Get_ModelTag();
			wstring strObjTag = dynamic_pointer_cast<CMapObject>(m_InstancingList[i])->Get_ObjectTag();
			wstring wstrTag = L"[" + strModelTag + L"] - " + L"[" + strObjTag + L"]";

			if (ImGui::Selectable(WstrToStr(wstrTag).c_str(), m_iPickIndex == i))
			{
				m_iPickIndex = i;
				m_pPickInstancing = m_InstancingList[i];
			}
		}
		ImGui::EndListBox();
	}
}

void CImgui_Tab_Instance::RenderObjectCreate()
{
	if (ImGui::Button("Add_Instancing"))
	{
		if (!m_InstancingList.empty())
		{
			wstring strInstTag = dynamic_pointer_cast<CMapObject>(m_InstancingList[0])->Get_ModelTag();
			wstring strCurrentTag = dynamic_pointer_cast<CMapObject>(m_pPickObject)->Get_ModelTag();
			if (strInstTag == strCurrentTag)
			{
				m_InstancingList.push_back(m_pPickObject);
				m_ObjectList.erase(m_ObjectList.begin() + m_iPickIndex);

				m_iPickIndex -= 1;
				if (m_iPickIndex >= 0)
					m_pPickObject = m_ObjectList[m_iPickIndex].second;
				else
					m_pPickObject = nullptr;
			}
			else
			{
				ImGui::OpenPopup("Error");
			}
		}
		else
		{
			m_InstancingList.push_back(m_pPickObject);
			m_ObjectList.erase(m_ObjectList.begin() + m_iPickIndex);

			m_iPickIndex -= 1;
			if (m_iPickIndex >= 0)
				m_pPickObject = m_ObjectList[m_iPickIndex].second;
			else
				m_pPickObject = nullptr;
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Out_Instancing"))
	{
		if (m_pPickInstancing != nullptr)
		{
			m_ObjectList.push_back({ L_OBJECT,m_pPickInstancing });
			m_InstancingList.erase(m_InstancingList.begin() + m_iPickIndex);

			m_iPickIndex -= 1;
			if (m_iPickIndex >= 0)
				m_pPickInstancing = m_InstancingList[m_iPickIndex];
			else
				m_pPickInstancing = nullptr;
		}
	}
}

void CImgui_Tab_Instance::RenderInstanceList()
{
	ImVec2 vSize = ImVec2(280, 100);

	ImGui::SeparatorText("Instance_List");
	if (ImGui::BeginListBox("3", vSize))
	{
		for (int i = 0; i < m_pGameObjectList[L_INSTANCE]->size(); i++)
		{
			shared_ptr<CGameObject> pGameObject = (*m_pGameObjectList[L_INSTANCE])[i];
			wstring wstrTag = L"[" + m_vecInstanceTag[i] + dynamic_pointer_cast<CInstance_Object>(pGameObject)->Get_ModelTag() + L"] ";

			if (ImGui::Selectable(WstrToStr(wstrTag).c_str(), m_iPickInstanceIdx == i))
			{
				m_iPickInstanceIdx = i;
				m_pPickInstance = pGameObject;
			}
		}
		ImGui::EndListBox();
	}
}

void CImgui_Tab_Instance::RenderInstanceCreate()
{
	if (ImGui::Button("Create_Instance"))
	{
		if (1 >= m_InstancingList.size())
		{
			ImGui::OpenPopup("Error_Msg");
		}
		else
		{
			vector<shared_ptr<CGameObject>> vecModelList;
			for (int i = 0; i < m_InstancingList.size(); i++)
			{
				m_vecMatrix.push_back(m_InstancingList[i]->Get_TransformCom().lock()->Get_WorldFloat4x4());
				vecModelList.push_back(m_InstancingList[i]);
			}
			Create_Instance(m_vecMatrix, vecModelList);
			for (auto& iter : m_InstancingList)
			{
				iter->TurnOn_State(OBJSTATE::WillRemoved);
			}
			m_InstancingList.clear();
			m_vecMatrix.clear();
		}
	}

	ImGui::SameLine();
	if (ImGui::Button("Delete_Instance"))
	{
		if (m_pPickInstance != nullptr)
		{
			m_pGameObjectList[L_INSTANCE]->erase(m_pGameObjectList[L_INSTANCE]->begin() + m_iPickInstanceIdx);
			m_pPickInstance->TurnOn_State(OBJSTATE::WillRemoved);
			m_iPickInstanceIdx -= 1;
			if (m_iPickInstanceIdx >= 0)
				m_pPickInstance = (*m_pGameObjectList[L_INSTANCE])[m_iPickInstanceIdx];
			else
				m_pPickInstance = nullptr;
		}
	}

	if (ImGui::Button("Auto Instancing"))
	{
		AutoInstancing();
	}
	ImGui::SameLine();
	if (ImGui::Button("All Delete"))
	{
		auto pInstanceList = *(m_pGameObjectList[L_INSTANCE]);

		if (!pInstanceList.empty())
		{
			for (auto& pObject : pInstanceList)
			{
				pObject->TurnOn_State(OBJSTATE::WillRemoved);
			}
			pInstanceList.clear();
			m_pGameObjectList[L_INSTANCE]->clear();
		}
	}
}

void CImgui_Tab_Instance::RenderErrorMsg()
{
	ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.2f, 0.2f, 0.2f, 0.9f));
	if (ImGui::BeginPopupModal("Error_Msg"))
	{
		string strMsg = "Model Create First";
		ImGui::Text(strMsg.c_str());

		ImGui::NextColumn();

		if (ImGui::Button(" OK "))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	if (ImGui::BeginPopupModal("Error"))
	{
		string strMsg = "ModelTag Not Same ";
		ImGui::Text(strMsg.c_str());

		ImGui::NextColumn();

		if (ImGui::Button(" OK "))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	ImGui::PopStyleColor();
}

HRESULT CImgui_Tab_Instance::SaveLoadData()
{
	ImVec2 vSize2 = ImVec2(120, 30);

	ImGui::SeparatorText("File");

	if (ImGui::Button(" Instance Save ", vSize2))
	{
		ImGuiFileDialog::Instance()->OpenDialog("SaveInstanceData", "Save Mpa File", ".InstBin", "../Bin/Data/Instance_Data/");
	}
	ImGui::SameLine();
	if (ImGui::Button(" Instance Load ", vSize2))
	{
		ImGuiFileDialog::Instance()->OpenDialog("LoadInstanceData", "Load Map File", ".InstBin", "../Bin/Data/Instance_Data/");
	}

	if (ImGuiFileDialog::Instance()->Display("SaveInstanceData"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();

			if (FAILED(CData_Manager::GetInstance()->Save_InstanceData(filePathName, (*m_pGameObjectList[L_INSTANCE]))))
				RETURN_EFAIL;
		}
		ImGuiFileDialog::Instance()->Close();
	}

	if (ImGuiFileDialog::Instance()->Display("LoadInstanceData"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();

			//filePathName = "C:\\Users\\hs534\\Documents\\Team\\Framework\\Client\\Bin\\Data\\Instance_Data\\Stage1\\Stage1_1.InstBin"
			CData_Manager::GetInstance()->Load_InstanceData(filePathName, LEVEL_TOOL);
			auto iter = m_pGameInstance->Get_ObjectList(LEVEL_TOOL, L_INSTANCE);
			if (!iter->empty())
			{
				for (auto pInst = iter->begin(); pInst != iter->end(); ++pInst)
				{
					m_szInstanceName += (*pInst)->Get_ObjectTag();
					m_vecInstanceTag.push_back(m_szInstanceName);
					m_pGameObjectList[L_INSTANCE]->push_back(*pInst);
					m_iPickIndex = m_ObjectList.size() - 1;
					m_pPickInstance = *pInst;
				}
			}

		}
		ImGuiFileDialog::Instance()->Close();
	}

	return S_OK;
}

HRESULT CImgui_Tab_Instance::Create_Instance(vector<float4x4> vecInst, vector <shared_ptr<CGameObject>> pObject)
{
	CInstance_Object::INSTANCE_OBJECT_DESC Desc;

	wstring strObjectTag;

	Desc.strName = WstrToStr(dynamic_pointer_cast<CMapObject>(pObject[0])->Get_ModelTag());
	Desc.vecInstanceVertex = vecInst;

	shared_ptr<CGameObject> pGameObject = nullptr;
	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_TOOL, (LAYERTYPE)m_iCreateLayerType,
		TEXT("Prototype_GameObject_Instance_Object"), &Desc, reinterpret_cast<shared_ptr<CGameObject>*>(&pGameObject))))
		RETURN_EFAIL;

	if (nullptr != pGameObject)
	{
		//if (strObjectTag[0] == NULL)
		//{
		//	strObjectTag = to_wstring(m_iCreateIndex);
		//	++m_iCreateIndex;
		//}
		//else
		//{
		if (CImgui_Manager::GetInstance()->Find_ObjectTag((LAYERTYPE)m_iCreateLayerType, strObjectTag))
		{
			MSG_BOX("TAG IS ALEADY IN USE");
			return S_OK;
		}
		//}
		//pGameObject->Set_ObjectTag(strObjectTag);
		m_szInstanceName += pGameObject->Get_ObjectTag();
		m_vecInstanceTag.push_back(m_szInstanceName);
		m_pGameObjectList[L_INSTANCE]->push_back(pGameObject);
		m_iPickIndex = m_ObjectList.size() - 1;
		m_pPickInstance = pGameObject;
	}
	return S_OK;
}

void CImgui_Tab_Instance::AutoInstancing()
{
	if (!m_ObjectList.empty())
	{
		vector<shared_ptr<CGameObject>> GroupModelList;
		_int iSize = m_ObjectList.size();
		for (_int j = 0; j < iSize; j++)
		{
			for (_int i = m_ObjectList.size() - 1; i >= 0; --i)
			{
				wstring strModelTag = dynamic_pointer_cast<CMapObject>(m_ObjectList[i].second)->Get_ModelTag();

				if (strModelTag == L"Frame_fabirc" || strModelTag == L"Frame_fabric2" || strModelTag == L"Stage1OutWall"|| strModelTag ==L"Fence7th_Slum")
				{
					continue;
				}

				if (m_InstancingList.empty() || (strModelTag == dynamic_pointer_cast<CMapObject>(m_InstancingList[0])->Get_ModelTag()))
				{
					m_InstancingList.push_back(m_ObjectList[i].second);
					m_ObjectList.erase(m_ObjectList.begin() + i);
				}
			}
			if (m_InstancingList.empty())
			{
				continue;
			}
			else if (m_InstancingList.size() >= 2)
			{
				vector<shared_ptr<CGameObject>> vecModelList;
				for (int i = 0; i < m_InstancingList.size(); i++)
				{
					m_vecMatrix.push_back(m_InstancingList[i]->Get_TransformCom().lock()->Get_WorldFloat4x4());
					vecModelList.push_back(m_InstancingList[i]);
				}

				Create_Instance(m_vecMatrix, vecModelList);

				for (auto& iter : m_InstancingList)
				{
					iter->TurnOn_State(OBJSTATE::WillRemoved);
				}
				m_InstancingList.clear();
				m_vecMatrix.clear();
			}
			else
			{
				GroupModelList.push_back(m_InstancingList[0]);
				m_InstancingList.clear();
			}
		}
		for (_int i = 0; i < GroupModelList.size(); ++i)
		{
			m_ObjectList.push_back({ L_OBJECT,GroupModelList[i] });
		}

		sort(m_vecInstanceTag.begin(), m_vecInstanceTag.end());

		MSG_BOX("Auto Instancing Complete");
	}
}

CImgui_Tab_Instance* CImgui_Tab_Instance::Create(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	CImgui_Tab_Instance* pInstance = new CImgui_Tab_Instance(pGameObjectList, pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CImgui_Tab_Instance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CImgui_Tab_Instance::Free()
{
	m_pPickInstancing = nullptr;
	m_pPickInstance = nullptr;

	m_InstancingList.clear();
}

#endif // DEBUG
