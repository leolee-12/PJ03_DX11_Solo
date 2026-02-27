#include "stdafx.h"
#ifdef _DEBUG

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"

#include "Imgui_MapToolBase.h"
#include "ImGuiFileDialog.h"

#include "Imgui_Manager.h"
#include "Imgui_Window_MapEdit.h"
#include "Imgui_Tab_Instance.h"
#include "Imgui_GroupObject.h"


#include "CommonModelComp.h"
#include "ModelBinary.h"

#include "Cell.h"
#include "MapObject.h"

#include "Data_Manager.h"

#include <iostream>
#include <filesystem>

#include "ModelBinary.h"
#include "Utility_File.h"

namespace fs = std::filesystem;

CImgui_MapToolBase::CImgui_MapToolBase(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CImgui_Tab(pGameObjectList, pDevice, pContext)
{
}
void ::CImgui_MapToolBase::OffsetTestModel()
{
	if (m_pCamera != nullptr)
	{
		_vector vCameraPos = m_pCamera->Get_TransformCom().lock()->Get_State(CTransform::STATE_POSITION);
		_vector vOffsetPos = XMLoadFloat3(&m_vTesOffset);
		vCameraPos += vOffsetPos;
		m_pTestObject->Get_TransformCom().lock()->Set_State(CTransform::STATE_POSITION, vCameraPos);
	}
}

void CImgui_MapToolBase::LoadFolder()
{
	m_mapFolderList.clear();
	for (auto& iter : *(m_pGameInstance->Get_ModelDatas()))
	{
		wstring path = CUtility_File::Get_FilePath(CPath_Mgr::FILE_TYPE::MODEL_FILE, iter.first);

		vector<wstring> vecTemp0 = SplitWstr(path, L'/');
		vector<wstring> vecTemp;

		if (vecTemp0.size() < 3)
			continue;

		for (auto& iter : vecTemp0)
		{
			vector<wstring> strTemp2 = SplitWstr(iter, L'\\');
			for (auto& iter2 : strTemp2)
			{
				vecTemp.push_back(iter2);
			}
		}

		wstring strTemp = vecTemp[4];

		if (!Compare_Wstr(strTemp, TEXT("Stage1")))
			continue;

		fs::path filePath(path);

		wstring lastFolderName;
		wstring FileName;
		if (filePath.has_parent_path())
		{
			FileName = filePath.stem().wstring();
			lastFolderName = filePath.parent_path().filename().wstring();

			m_szFolder = lastFolderName;

			m_szModelName = FileName;

			m_mapFolderList[m_szFolder].push_back(m_szModelName);

			if (find(m_FolderNames.begin(), m_FolderNames.end(), m_szFolder) == m_FolderNames.end())
			{
				m_FolderNames.push_back(m_szFolder);
			}
		}

	}

	for (auto& folder : m_mapFolderList)
	{
		sort(folder.second.begin(), folder.second.end());
	}

	sort(m_FolderNames.begin(), m_FolderNames.end());
}

CImgui_MapToolBase* CImgui_MapToolBase::Create(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	CImgui_MapToolBase* pInstance = new CImgui_MapToolBase(pGameObjectList, pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CImgui_MapToolBase");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CImgui_MapToolBase::Free()
{
	m_pPickObject = nullptr;

	m_pTestObject = nullptr;

	m_pCamera = nullptr;
	m_ObjectList.clear();

	__super::Free();
}

#endif // DEBUG
