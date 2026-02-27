#include "stdafx.h"
//
//#include "imgui.h"
//#include "imgui_impl_win32.h"
//#include "imgui_impl_dx11.h"
//#include "imgui_internal.h"
//
//#include "Imgui_Manager.h"
//#include "Imgui_Tab_BakeBinary.h"
//#include "ImGuiFileDialog.h"
//
//CImgui_Tab_BakeBinary::CImgui_Tab_BakeBinary(vector<CGameObject*>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
//    :CImgui_Tab(pGameObjectList,pDevice,pContext)
//{
//}
//
//HRESULT CImgui_Tab_BakeBinary::Initialize()
//{
//    return S_OK;
//}
//
//void CImgui_Tab_BakeBinary::Tick(_cref_time fTimeDelta)
//{
//
//}
//
//HRESULT CImgui_Tab_BakeBinary::Render()
//{
//	ImGui::RadioButton("ANIM", &m_eModelType, MODELTYPE::ANIM);
//	ImGui::SameLine();
//	ImGui::RadioButton("NON_ANIM", &m_eModelType, MODELTYPE::NONANIM);
//
//	if (ImGui::Button("Bake_Binary"))
//	{
//		ImGuiFileDialog::Instance()->OpenDialog("BakeBinary", "Bake Binary File", ".fbx", "../Bin/Resources/Models/");
//	}
//
//	if (ImGuiFileDialog::Instance()->Display("BakeBinary"))
//	{
//		if (ImGuiFileDialog::Instance()->IsOk())
//		{
//			BakeBinary_FromAssimp();
//		}
//		ImGuiFileDialog::Instance()->Close();
//	}
//
//	return S_OK;
//}
//
//HRESULT CImgui_Tab_BakeBinary::Bake_Binary(string filePathName, MODEL_DATA& ModelData)
//{
//	_char		szDrive[MAX_PATH] = "";
//	_char		szDirectory[MAX_PATH] = "";
//	_char		szFileName[MAX_PATH] = "";
//	_char		szEXT[MAX_PATH] = ".bin";
//
//	_splitpath_s(filePathName.c_str(), szDrive, MAX_PATH, szDirectory, MAX_PATH, szFileName, MAX_PATH, nullptr, 0);
//	
//	_char		szFullPath[MAX_PATH] = "";
//	strcpy_s(szFullPath, szDrive);
//	strcat_s(szFullPath, szDirectory);
//	strcat_s(szFullPath, szFileName);
//	strcat_s(szFullPath, szEXT);
//
//	string strPath = string(szFullPath);
//
//	ofstream fout;
//
//	fout.open(strPath,std::ofstream::binary);
//
//	if (fout.is_open())
//	{
//		ModelData.Bake_Binary(fout);
//	}
//	else
//		RETURN_EFAIL;
//
//	fout.close();
//
//	return S_OK;
//}
//
//HRESULT CImgui_Tab_BakeBinary::BakeBinary_FromAssimp()
//{
//	string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
//	filePathName = string("..") + filePathName.substr(filePathName.find("\\Bin"));
//	
//	MODEL_DATA ModelData = {};
//	ModelData.eModelType = (MODELTYPE)m_eModelType;
//
//	_fmatrix PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
//
//	_uint	iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;
//
//	if (MODELTYPE::NONANIM == ModelData.eModelType)
//		iFlag |= aiProcess_PreTransformVertices;
//
//	m_pAIScene = m_Importer.ReadFile(filePathName, iFlag);
//	if (nullptr == m_pAIScene)
//		RETURN_EFAIL;
//
//	if (FAILED(ModelData.Bake_Bones(m_pAIScene->mRootNode, -1)))
//		RETURN_EFAIL;
//
//	if (FAILED(ModelData.Bake_Meshes(m_pAIScene,PivotMatrix)))
//		RETURN_EFAIL;
//
//	if (FAILED(ModelData.Bake_Materials(m_pAIScene, filePathName)))
//		RETURN_EFAIL;
//
//	if (FAILED(ModelData.Bake_Animations(m_pAIScene)))
//		RETURN_EFAIL;
//
//	Bake_Binary(filePathName, ModelData);
//
//	return S_OK;
//}
//
//CImgui_Tab_BakeBinary* CImgui_Tab_BakeBinary::Create(vector<CGameObject*>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
//{
//	CImgui_Tab_BakeBinary* pInstance = new CImgui_Tab_BakeBinary(pGameObjectList,pDevice,pContext);
//
//	if (FAILED(pInstance->Initialize()))
//	{
//		MSG_BOX("Failed to Created : CImgui_Tab_BakeBinary");
//		Safe_Release(pInstance);
//	}
//	return pInstance;
//}
//
//void CImgui_Tab_BakeBinary::Free()
//{
//	__super::Free();
//}
