#include "stdafx.h"
#ifdef _DEBUG
#include "Imgui_GroupObject.h"

#include "ImGuiFileDialog.h"
#include "Imgui_Manager.h"  

#include "CommonModelComp.h"
#include "ModelBinary.h"

#include "MapObject.h"

#include "Model_Manager.h"
#include "Data_Manager.h"

#include <iostream>
#include <filesystem>

#include "Utility_File.h"

namespace fs = std::filesystem;

CImgui_GroupObject::CImgui_GroupObject(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CImgui_MapToolBase(pGameObjectList, pDevice, pContext)
{
}

HRESULT CImgui_GroupObject::Initialize()
{
	return S_OK;
}

void CImgui_GroupObject::Tick(_cref_time fTimeDelta)
{
}

HRESULT CImgui_GroupObject::Render()
{
	//리스트박스
	RenderList();

	//추가 삭제
	RenderAddOut();


	ImGui::SeparatorText("Guizmo");

	if (m_pPickTabObject != nullptr)
	{
		Use_ImGuizmo(m_pPickTabObject, true);
	}

	//저장 다이얼로그
	RenderSave();

	return S_OK;
}

void CImgui_GroupObject::RenderList()
{
	ImVec2 vSize = ImVec2(280, 100);

	ImGui::SeparatorText("Tab_ObjectList");
	if (ImGui::BeginListBox("1 ", vSize))
	{
		for (int i = 0; i < m_ObjectList.size(); i++)
		{
			wstring strModelTag = dynamic_pointer_cast<CMapObject>(m_ObjectList[i].second)->Get_ModelTag();
			wstring strObjTag = dynamic_pointer_cast<CMapObject>(m_ObjectList[i].second)->Get_ObjectTag();
			wstring wstrTag = L"[" + strModelTag + L"] - " + L"[" + strObjTag + L"]";

			if (ImGui::Selectable(WstrToStr(wstrTag).c_str(), m_iPickIndex == i))
			{
				m_iPickIndex = i;
				m_pPickTabObject = m_ObjectList[i].second;
			}
		}
		ImGui::EndListBox();
	}

	ImGui::SeparatorText("Group_Object_List");
	if (ImGui::BeginListBox("2 ", vSize))
	{
		for (int i = 0; i < m_GroupObjects.size(); i++)
		{
			wstring strModelTag = dynamic_pointer_cast<CMapObject>(m_GroupObjects[i])->Get_ModelTag();
			wstring strObjTag = dynamic_pointer_cast<CMapObject>(m_GroupObjects[i])->Get_ObjectTag();
			wstring wstrTag = L"[" + strModelTag + L"] - " + L"[" + strObjTag + L"]";

			if (ImGui::Selectable(WstrToStr(wstrTag).c_str(), m_iPickIndex == i))
			{
				m_iPickIndex = i;
				m_pPickObject = m_GroupObjects[i];
			}
		}
		ImGui::EndListBox();
	}
}

void CImgui_GroupObject::RenderAddOut()
{
	if (ImGui::Button("Add_Object"))
	{
		if (m_pPickTabObject != nullptr)
		{
			m_GroupObjects.push_back(m_pPickTabObject);
			m_ObjectList.erase(m_ObjectList.begin() + m_iPickIndex);

			m_iPickIndex -= 1;
			if (m_iPickIndex >= 0)
				m_pPickTabObject = m_ObjectList[m_iPickIndex].second;
			else
				m_pPickTabObject = nullptr;
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Out_Object"))
	{
		if (m_pPickObject != nullptr)
		{
			m_ObjectList.push_back({ L_OBJECT,m_pPickObject });
			m_GroupObjects.erase(m_GroupObjects.begin() + m_iPickIndex);

			m_iPickIndex -= 1;
			if (m_iPickIndex >= 0)
				m_pPickObject = m_GroupObjects[m_iPickIndex];
			else
				m_pPickObject = nullptr;
		}
	}
}

void CImgui_GroupObject::RenderSave()
{
	ImVec2 vSize2 = ImVec2(80, 30);

	ImGui::SeparatorText("File");

	if (ImGui::Button(" Group Save ", vSize2))
	{
		ImGuiFileDialog::Instance()->OpenDialog("SaveGroupData", "Save Mpa File", ".groupmodel", "../Bin/Resources/Model/Stage1/Group/");
	}

	if (ImGuiFileDialog::Instance()->Display("SaveGroupData"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();

			GroupSave(StrToWstr(filePathName));

			m_pGameInstance->Load_DirectorySub_Models(L"Stage1/Group/");

		}
		ImGuiFileDialog::Instance()->Close();
	}
	ImGui::SameLine();
	if (ImGui::Button(" Group AllSave ", vSize2))
	{
		ImGuiFileDialog::Instance()->OpenDialog("AllSaveGroupData", "Save Mpa File", ".groupmodel", "../Bin/Resources/Model/Stage1/Group/");
	}

	if (ImGuiFileDialog::Instance()->Display("AllSaveGroupData"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();

			AllGroupSave(StrToWstr(filePathName));

			m_pGameInstance->Load_DirectorySub_Models(L"Stage1/Group/");

		}
		ImGuiFileDialog::Instance()->Close();
	}
}

HRESULT CImgui_GroupObject::GroupSave(const wstring& filePath)
{
	vector<CModelBinary*> vecModel;

	for (_uint i = 0; i < m_GroupObjects.size(); ++i)
	{
		CModelBinary* pModel = CModelBinary::Create();

		wstring ModelTag = dynamic_pointer_cast<CMapObject>(m_GroupObjects[i])->Get_ModelTag();

		pModel->Load_Binary(CUtility_File::Get_FilePath(CPath_Mgr::FILE_TYPE::MODEL_FILE, ModelTag));

		_float4x4 ObjectMat = dynamic_pointer_cast<CMapObject>(m_GroupObjects[i])->Get_TransformCom().lock()->Get_WorldFloat4x4();
		pModel->m_GroupMatrix = ObjectMat;

		vecModel.push_back(pModel);

	}

	vecModel[0]->Save_GroupBinary(filePath, vecModel);

	fs::path CutPath(filePath);
	if (CutPath.has_parent_path())
	{
		wstring FileName = CutPath.stem().wstring();
		vector<wstring> vecTemp = SplitWstr(filePath, L'\\');
		wstring szFilePath = L"..\\";
		for (_int i = 7; i < vecTemp.size(); i++)
		{
			if (i < 12)
				szFilePath += vecTemp[i] + L"/";
			else
				szFilePath += vecTemp[i];
		}
		CUtility_File::Add_FilePath(CPath_Mgr::FILE_TYPE::MODEL_FILE, FileName, szFilePath);

		m_pGameInstance->Load_Model(FileName);
	}

	m_GroupObjects.clear();

	for (auto& iter : vecModel)
		Safe_Release(iter);

	m_ObjectList = m_SaveObjects;
	return S_OK;
}

HRESULT CImgui_GroupObject::AllGroupSave(const wstring& filePath)
{
	vector<CModelBinary*> vecModel;

	for (_int i = 0; i < m_ObjectList.size(); ++i)
	{
		if (i != 0)
			m_GroupObjects.push_back(m_ObjectList[i].second);
	}

	for (_uint i = 0; i < m_GroupObjects.size(); ++i)
	{
		CModelBinary* pModel = CModelBinary::Create();

		wstring ModelTag = dynamic_pointer_cast<CMapObject>(m_GroupObjects[i])->Get_ModelTag();

		pModel->Load_Binary(CUtility_File::Get_FilePath(CPath_Mgr::FILE_TYPE::MODEL_FILE, ModelTag));

		_float4x4 ObjectMat = dynamic_pointer_cast<CMapObject>(m_GroupObjects[i])->Get_TransformCom().lock()->Get_WorldFloat4x4();
		pModel->m_GroupMatrix = ObjectMat;

		vecModel.push_back(pModel);

	}

	vecModel[0]->Save_GroupBinary(filePath, vecModel);

	fs::path CutPath(filePath);
	if (CutPath.has_parent_path())
	{
		wstring FileName = CutPath.stem().wstring();
		vector<wstring> vecTemp = SplitWstr(filePath, L'\\');
		wstring szFilePath = L"..\\";
		for (_int i = 7; i < vecTemp.size(); i++)
		{
			if (i < 12)
				szFilePath += vecTemp[i] + L"/";
			else
				szFilePath += vecTemp[i];
		}
		CUtility_File::Add_FilePath(CPath_Mgr::FILE_TYPE::MODEL_FILE, FileName, szFilePath);

		m_pGameInstance->Load_Model(FileName);
	}

	m_GroupObjects.clear();

	for (auto& iter : vecModel)
		Safe_Release(iter);

	m_ObjectList = m_SaveObjects;

	return S_OK;
}

CImgui_GroupObject* CImgui_GroupObject::Create(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	CImgui_GroupObject* pInstance = new CImgui_GroupObject(pGameObjectList, pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CImgui_GroupObject");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CImgui_GroupObject::Free()
{
	m_pPickTabObject = nullptr;
	m_pPickObject = nullptr;

	m_ObjectList.clear();
	m_GroupObjects.clear();
}

#endif // DEBUG
