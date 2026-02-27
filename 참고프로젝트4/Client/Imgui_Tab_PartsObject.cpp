#include "stdafx.h"
#ifdef _DEBUG

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"

#include "Imgui_Manager.h"
#include "Imgui_Tab_PartsObject.h"
#include "ImGuiFileDialog.h"

#include "Data_Manager.h"
#include "Utility_File.h"

#include "CommonModelComp.h"
#include "AnimationComponent.h"
#include "BoneAnimContainer.h"
#include "GameInstance.h"
#include "CommonModelComp.h"
#include "BoneContainer.h"
#include "Model_Manager.h"

CImgui_Tab_PartsObject::CImgui_Tab_PartsObject(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CImgui_Tab(pGameObjectList, pDevice, pContext)
{
}

HRESULT CImgui_Tab_PartsObject::Initialize()
{
	const unordered_map<wstring, FModelData*>* pModels = m_pGameInstance->Get_ModelDatas();
	if (pModels == nullptr)
		RETURN_EFAIL;

	for (auto& iter : *pModels)
	{
		vector<wstring> vecTemp = SplitWstr(CUtility_File::PathFinder(TEXT("../Bin/Resources/Model/Character/Weapon/"), iter.first), L'/');

		if (vecTemp.size() < 3)
			continue;

		wstring strTemp = vecTemp[5];

		if (Compare_Wstr(strTemp, TEXT("Weapon")))
			m_PrototypeModelList.push_back(iter.first);
	}

	m_eCurLayerType = LAYERTYPE::L_PLAYER;

	return S_OK;
}

void CImgui_Tab_PartsObject::Tick(_cref_time fTimeDelta)
{

}

HRESULT CImgui_Tab_PartsObject::Render()
{
	/*if (ImGui::Button("Save_Effect"))
	{
		ImGuiFileDialog::Instance()->OpenDialog("SaveEffect", "Save Effect File", ".json", "../Bin/Data/Effect_Data/Trail/");
	}

	if (ImGuiFileDialog::Instance()->Display("SaveEffect"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			CData_Manager::GetInstance()->Save_ObjectData(filePathName, m_pTrailEffect);
		}
		ImGuiFileDialog::Instance()->Close();
	}

	if (!m_bModify)
	{
		ImGui::SameLine();
		if (ImGui::Button("Load_Effect"))
		{
			ImGuiFileDialog::Instance()->OpenDialog("LoadEffect", "Load Effect File", ".json", "../Bin/Data/Effect_Data/Trail/");
		}

		if (ImGuiFileDialog::Instance()->Display("LoadEffect"))
		{
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				string filePathName = m_strLoadFilePath = ImGuiFileDialog::Instance()->GetFilePathName();

				if (m_pTrailEffect != nullptr)
					m_pTrailEffect->TurnOn_State(OBJSTATE::WillRemoved);

				if (FAILED(m_pGameInstance->Add_Prototype(TEXT("ToolTrail_") + to_wstring(m_iNumLoad),
					CTrail_Effect::Create(m_pDevice, m_pContext, filePathName))))
					RETURN_EFAIL;

				if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(),
					L_EFFECT, TEXT("ToolTrail_") + to_wstring(m_iNumLoad), nullptr, reinterpret_cast<shared_ptr<CGameObject>*>(&m_pTrailEffect))))
					RETURN_EFAIL;

				m_pImgui_Manger->Update_List(L_EFFECT);

				m_TrailDesc = *(m_pTrailEffect->Get_TrailDesc());

				m_iDeadType = (_int)m_TrailDesc.eDeadType;

				m_iUseTypeRadio = m_TrailDesc.eUseType;

				m_bLoad = true;

				++m_iNumLoad;
			}
			ImGuiFileDialog::Instance()->Close();
		}
	}*/

	m_pImgui_Manger->Update_List(m_eCurLayerType);
	if (!m_pGameObjectList[m_eCurLayerType]->empty())
	{
		if (m_pAnimObject == nullptr)
		{
			m_pAnimObject = dynamic_pointer_cast<CAnimObject>(m_pGameObjectList[m_eCurLayerType]->back());
		}
	}

	if (m_pAnimObject)
	{
		ImGui::SeparatorText(u8"»À");
		const auto& pBoneGroup = m_pAnimObject->Get_ModelCom().lock()->Get_BoneGroup();

		ImGui::SetNextItemWidth(280.f);

		// »À Á¤º¸ ·Îµå
		_uint iNumBoneCount = pBoneGroup->CRef_BoneDatas_Count();
		vector<FBoneData*>& BoneDatas = pBoneGroup->Get_BoneDatasVector();
		vector<string> BoneNames;
		BoneNames.reserve(iNumBoneCount);
		for (auto iter = BoneDatas.begin(); iter != BoneDatas.end(); ++iter)
		{
			BoneNames.push_back(ConvertToString((*iter)->strName));
		}
		sort(BoneNames.begin(), BoneNames.end());

		string strPreviewBone;
		if (iNumBoneCount == 0)
		{
			strPreviewBone = "¾øÀ½";
			m_iBoneSelect_index = -1;
		}
		else
		{
			if (m_iBoneSelect_index == -1)
				m_iBoneSelect_index = 0;
			strPreviewBone = BoneNames[m_iBoneSelect_index];
		}

		ImVec2 vSize = ImVec2(280, 100);
		if (ImGui::TreeNode("BoneList"))
		{
			if (ImGui::BeginListBox(u8"1", vSize))
			{
				if (m_pGameInstance->Key_Down(DIK_DOWN) && m_iBoneSelect_index < Cast<_int>(iNumBoneCount - 1))
				{
					m_iBoneSelect_index++;
					m_tPartsDesc.strBoneName = StrToWstr(BoneNames[m_iBoneSelect_index]);
				}
				if (m_pGameInstance->Key_Down(DIK_UP) && m_iBoneSelect_index > 0 )
				{
					m_iBoneSelect_index--;
					m_tPartsDesc.strBoneName = StrToWstr(BoneNames[m_iBoneSelect_index]);
				}
				for (_uint i = 0; i < iNumBoneCount; i++)
				{
					_bool bIsSelected = (m_iBoneSelect_index == i);
					if (ImGui::Selectable((BoneNames[i]).c_str(), bIsSelected))
					{
						m_iBoneSelect_index = i;
						m_tPartsDesc.strBoneName = StrToWstr(BoneNames[m_iBoneSelect_index]);
						// »À ÀÌ¸§ ¾ò±â
					}
					if (bIsSelected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndListBox();
			}
			ImGui::TreePop();
		}

		ImGui::Separator();

		if (ImGui::TreeNode("Weapon_List"))
		{
			if (ImGui::BeginListBox("1", vSize))
			{
				for (int i = 0; i < m_PrototypeModelList.size(); i++)
				{
					wstring Temp = m_PrototypeModelList[i];
					vector<wstring> PrtTag = SplitWstr(Temp, L'/');
					wstring wstrTag = {};
					wstrTag = SplitWstr(PrtTag[PrtTag.size() - 1], L'.').front();
					wstrTag = L"[" + wstrTag + L"] ";

					string  str;
					str = WstrToStr(wstrTag);
					if (ImGui::Selectable(str.c_str(), m_iPickIndex_PrtTag == i))
					{
						m_tPartsDesc.strModelTag = m_PrototypeModelList[i];
					}
				}
				ImGui::EndListBox();
			}
			ImGui::TreePop();
		}

		if (ImGui::Button("Create_Parts"))
		{
			if (!m_pPartsObject || ! m_tPartsDesc.strBoneName.empty())
			{
				m_tPartsDesc.pParentTransform = m_pAnimObject->Get_TransformCom().lock();
				m_tPartsDesc.pBoneGroup = m_pAnimObject->Get_ModelCom().lock()->Get_BoneGroup();

				if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_NPC,
					TEXT("Prototype_GameObject_Demo_PartsObject"), &m_tPartsDesc, reinterpret_cast<shared_ptr<CGameObject>*>(&m_pPartsObject))))
					RETURN_EFAIL;

				m_pPartsObject->Set_Owner(m_pAnimObject);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Remove_Parts"))
		{
			if (m_pPartsObject)
			{
				m_pPartsObject->Set_Dead();
				m_pPartsObject = nullptr;
			}

			if (m_pAnimObject)
			{
				m_pAnimObject = nullptr;
				m_iBoneSelect_index = -1;
			}
			m_tPartsDesc.strBoneName = L"";

			return S_OK;
		}
		ImGui::SameLine();
		ImGui::Checkbox("Bone_Change", &m_bChnageBone);

		if (m_pPartsObject != nullptr && m_bChnageBone)
		{
			m_pPartsObject->Change_Bone(m_tPartsDesc.strBoneName);
		}

		ImGui::Separator();
		if (m_tPartsDesc.strModelTag.size() > 0)
		{
			ImGui::Text(("Model : " + WstrToStr(SplitWstr(m_tPartsDesc.strModelTag, L'/').back())).c_str());
			string str = "Current_Bone : ";
			str += WstrToStr(m_tPartsDesc.strBoneName);
			ImGui::Text(str.c_str());
		}

		if (m_pPartsObject)
		{
			ImGui::Separator();
			ImGui::Checkbox("Gizmo", &m_bGizmo);
			if (m_bGizmo)
			{
				Use_ImGuizmo(m_pPartsObject, true);
			}
		}
	}

	return S_OK;
}


CImgui_Tab_PartsObject* CImgui_Tab_PartsObject::Create(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	CImgui_Tab_PartsObject* pInstance = new CImgui_Tab_PartsObject(pGameObjectList, pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CImgui_Tab_PartsObject");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CImgui_Tab_PartsObject::Free()
{
	__super::Free();
}

#endif // DEBUG
