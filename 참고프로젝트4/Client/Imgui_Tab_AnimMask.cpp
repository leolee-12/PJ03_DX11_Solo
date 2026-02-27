#include "stdafx.h"
#ifdef _DEBUG
#include "Imgui_Tab_AnimMask.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"

#include "Imgui_Manager.h"
#include "ImGuiFileDialog.h"

#include "AnimObject.h"
#include "CommonModelComp.h"
#include "AnimationComponent.h"
#include "BoneAnimContainer.h"
#include "GameInstance.h"
#include "CommonModelComp.h"
#include "BoneContainer.h"
#include "Model_Manager.h"

#include "Imgui_Window_AnimMainEdit.h"
#include "Imgui_Tab_AnimObject.h"


CImgui_Tab_AnimMask::CImgui_Tab_AnimMask(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: BASE(pGameObjectList, pDevice, pContext)
{
}

HRESULT CImgui_Tab_AnimMask::Initialize()
{

	return S_OK;
}

void CImgui_Tab_AnimMask::Tick(_cref_time fTimeDelta)  //X
{

}

HRESULT CImgui_Tab_AnimMask::Render()
{
	auto pAnimEdit = m_pImgui_Manger->Find_Window<CImgui_Window_AnimEdit>(CImgui_Manager::TOOL_ANIM);
	auto pAnimObjectTab = pAnimEdit->Tab_AnimObject();

	ImGui::SeparatorText(u8"애니메이션 Mask");

	if (m_pAnimObject.expired())
	{
		m_pAnimObject = pAnimObjectTab->Get_AnimObject();
		//auto pAnimComp = m_pAnimObject.lock()->Get_ModelCom().lock()->Get_AnimationComponent();
		//pAnimComp->Create_MaskAnim();
		m_EndBone_Names.clear();
		m_iNumEndBones = 0;
	}

	if (m_pAnimObject.expired())
		return S_OK;

	// 애니메이션 이름 받아오기
	vector<string> AnimNames;
	vector<const _char*> AnimNamesConst;
	if (m_pAnimObject.lock() != nullptr)
	{
		CAnimationComponent* pAnimComp = m_pAnimObject.lock()->Get_ModelCom().lock()->AnimationComp().get();
		auto& AnimDatas = pAnimComp->Get_AnimGroup()->Get_BoneAnimDatasVector();
		for (size_t i = 0; i < AnimDatas.size(); i++)
		{
			AnimNames.push_back(ConvertToString(AnimDatas[i]->strName));
		}
		sort(AnimNames.begin(), AnimNames.end());
		for (size_t i = 0; i < AnimNames.size(); i++)
		{
			AnimNamesConst.push_back(AnimNames[i].c_str());
		}
	}

	_bool bIsChanged = { false };
	//auto& AnimGroup = *m_pGameInstance->Find_AnimGroup()
	ImGui::SeparatorText(u8"애니메이션 리스트");
	ImGui::SetNextItemWidth(280);
	if (ImGui::ListBox(u8"##애니메이션 마스크 리스트", &m_iAnimSelect_Index.Cur, AnimNamesConst.data(), (_int)AnimNamesConst.size(), 8))
	{
		// 애니메이션, 더블클릭시 적용
		if (m_iAnimSelect_Index.Cur == m_iAnimSelect_Index.Prev)
		{
			m_AnimSelect_Name = ConvertToWstring(AnimNames[m_iAnimSelect_Index.Cur]);
			bIsChanged = true;
		}
	}
	m_iAnimSelect_Index.Prev = m_iAnimSelect_Index.Cur;
	if (ImGui::Button(u8"애니메이션 마스크 선택"))
	{
		bIsChanged = true;
	}

	ImGui::Checkbox(u8"애니메이션 루프", &m_bLoop);

	// 뼈
	ImGui::SeparatorText(u8"뼈");
	const auto& pBoneGroup = m_pAnimObject.lock()->Get_ModelCom().lock()->Get_BoneGroup();

	ImGui::SetNextItemWidth(280.f);

	// 뼈 정보 로드
	_uint iNumBoneCount = pBoneGroup->CRef_BoneDatas_Count();
	vector<FBoneData*>& BoneDatas = pBoneGroup->Get_BoneDatasVector();



	vector<string> BoneNames;
	BoneNames.reserve(iNumBoneCount);
	for (auto iter = BoneDatas.begin(); iter != BoneDatas.end(); ++iter)
	{
		BoneNames.push_back(ConvertToString((*iter)->strName));
	}

#pragma region BasisBones 콤보박스
	// 표시 문자열
	{
		auto pBone = pBoneGroup->Find_BoneData(m_BasisBone_Name);
		_uint iBoneIndex = -1;
		string strDisplay;
		if (pBone == nullptr)
			strDisplay = BoneNames[0];
		else
		{
			iBoneIndex = pBone->iID;
			strDisplay = BoneNames[iBoneIndex];
		}

		// EndBone 선택
		if (ImGui::BeginCombo(u8"Basis 선택하기##Mask", strDisplay.c_str()))
		{
			for (_uint i = 0; i < BoneNames.size(); i++)
			{
				_bool bIsSelected = (iBoneIndex == i);
				if (ImGui::Selectable((BoneNames[i]).c_str(), bIsSelected))
				{
					m_BasisBone_Name = ConvertToWstring(BoneNames[i]);
					bIsChanged = true;
				}
				if (bIsSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
	}
#pragma endregion


#pragma region EndBones 콤보박스

	if (ImGui::Button(u8"EndBone 추가##Mask"))
	{
		auto pBone = pBoneGroup->Find_BoneData(0);
		m_iNumEndBones++;
		m_EndBone_Names.push_back(pBone->strName);
	}

	vector<_uint> vecDelete;
	vector<wstring> vecEndBoneNames;
	for (_uint i = 0; i < m_iNumEndBones; ++i)
	{
		auto pBone = pBoneGroup->Find_BoneData(m_EndBone_Names[i]);
		_uint iBoneIndex = -1;
		string strDisplay;
		if (pBone == nullptr)
			strDisplay = BoneNames[0];
		else
		{
			iBoneIndex = pBone->iID;
			strDisplay = BoneNames[iBoneIndex];
		}

		if (ImGui::Button(u8"X"))
			vecDelete.push_back(iBoneIndex);

		string strInt = u8"EndBone 선택하기" + to_string(i);

		// EndBone 선택
		if (ImGui::BeginCombo(strInt.c_str(), strDisplay.c_str()))
		{
			for (_uint j = 0; j < (_uint)BoneNames.size(); j++)
			{
				_bool bIsSelected = (iBoneIndex == j);
				if (ImGui::Selectable((BoneNames[j]).c_str(), bIsSelected))
				{
					vecEndBoneNames.push_back(ConvertToWstring(BoneNames[j]));
					bIsChanged = true;
				}
				if (bIsSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
	}
	if (m_EndBone_Names.size() > vecEndBoneNames.size())
		vecEndBoneNames = m_EndBone_Names;

	m_EndBone_Names = vecEndBoneNames;
	// EndBone 삭제
	for (_uint i = 0; i < m_EndBone_Names.size(); ++i)
	{
		auto pBone = pBoneGroup->Find_BoneData(m_EndBone_Names[i]);
		_uint iBoneIndex = -1;
		if (pBone != nullptr)
			iBoneIndex = pBone->iID;

		_bool bDeleted = false;
		for (_uint j = 0; j < (_uint)vecDelete.size(); j++)
			bDeleted = (vecDelete[j] == iBoneIndex);

		// 삭제
		if (bDeleted)
		{
			auto iter = m_EndBone_Names.begin() + iBoneIndex;
			m_EndBone_Names.erase(iter);
			m_iNumEndBones--;
			i++;
		}
	}
	
	_float fRecursive = m_iRecursive;
	ImGui::DragFloat(u8"재귀", &fRecursive, 1.f, 0.f, (_float)UINT_MAX, "%.0f");
	m_iRecursive = (_uint)fRecursive;
#pragma endregion

	if (bIsChanged)
		Change_MaskAnimation();

	return S_OK;
}

CImgui_Tab_AnimMask* CImgui_Tab_AnimMask::Create(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	CImgui_Tab_AnimMask* pInstance = new CImgui_Tab_AnimMask(pGameObjectList, pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CImgui_Tab_AnimMask");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CImgui_Tab_AnimMask::Free()
{
	__super::Free();

}

void CImgui_Tab_AnimMask::Change_MaskAnimation()
{
	auto pAnimationComp = m_pAnimObject.lock()->Get_ModelCom().lock()->AnimationComp();
	pAnimationComp->Set_MaskAnim(0, m_AnimSelect_Name, m_BasisBone_Name, m_EndBone_Names, m_iRecursive, 1.f, m_bLoop);
}

#endif // DEBUG
