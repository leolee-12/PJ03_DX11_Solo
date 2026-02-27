#include "stdafx.h"
#ifdef _DEBUG
#include "Imgui_Tab_AnimAdd.h"

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


CImgui_Tab_AnimAdd::CImgui_Tab_AnimAdd(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: BASE(pGameObjectList, pDevice, pContext)
{
}

HRESULT CImgui_Tab_AnimAdd::Initialize()
{

	return S_OK;
}

void CImgui_Tab_AnimAdd::Tick(_cref_time fTimeDelta)  //X
{

}

HRESULT CImgui_Tab_AnimAdd::Render()
{
	auto pAnimEdit = m_pImgui_Manger->Find_Window<CImgui_Window_AnimEdit>(CImgui_Manager::TOOL_ANIM);
	auto pAnimObjectTab = pAnimEdit->Tab_AnimObject();

	ImGui::SeparatorText(u8"애니메이션 ADD");

	if (m_pAnimObject.expired())
	{
		m_pAnimObject = pAnimObjectTab->Get_AnimObject();
		m_pAnimObject.lock()->Get_ModelCom().lock()->Get_AnimationComponent()
			->Create_Add_Animation();
		// 초기화 코드
		m_AnimSelect_Name = TEXT("");
		m_BasisBone_Name = TEXT("");
		m_EndBone_Name.clear();
		m_iNumEndBones = 0;
	}

	if (m_pAnimObject.expired())
		return S_OK;



	// 모델 이름 받아오기
	vector<string> AnimNames;
	vector<const _char*> AnimNamesConst;
	CAnimationComponent* pAnimComp = m_pAnimObject.lock()->Get_ModelCom().lock()->AnimationComp().get();
	auto& AnimDatas = pAnimComp->Get_AnimGroup()->Get_BoneAnimDatasVector();
	for (size_t i = 0; i < AnimDatas.size(); i++)
	{
		if (AnimDatas[i]->strName.find(TEXT("_Add")) != wstring::npos)
			AnimNames.push_back(ConvertToString(AnimDatas[i]->strName));
	}
	sort(AnimNames.begin(), AnimNames.end());
	for (size_t i = 0; i < AnimNames.size(); i++)
	{
		AnimNamesConst.push_back(AnimNames[i].c_str());
	}


	// 애니메이션 리스트
	_bool bIsChanged = { false };
	//auto& AnimGroup = *m_pGameInstance->Find_AnimGroup()
	ImGui::SeparatorText(u8"애니메이션 리스트");
	ImGui::SetNextItemWidth(280);
	if (ImGui::ListBox(u8"##애니메이션 리스트", &m_iAnimSelect_Index.Cur, AnimNamesConst.data(), (_int)AnimNamesConst.size(), 8))
	{
		// 애니메이션, 더블클릭시 적용
		if (m_iAnimSelect_Index.Cur == m_iAnimSelect_Index.Prev)
		{
			m_AnimSelect_Name = ConvertToWstring(AnimNames[m_iAnimSelect_Index.Cur]);
			bIsChanged = true;
		}
	}
	m_iAnimSelect_Index.Prev = m_iAnimSelect_Index.Cur;
	if (ImGui::Button(u8"애니메이션 선택"))
	{
		bIsChanged = true;
	}


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
		if (ImGui::BeginCombo(u8"Basis 선택하기", strDisplay.c_str()))
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
		m_EndBone_Name.push_back(pBone->strName);
	}

	vector<_uint> vecDelete;
	vector<wstring> vecEndBoneNames;
	for (_uint i = 0; i < m_iNumEndBones; ++i)
	{
		auto pBone = pBoneGroup->Find_BoneData(m_EndBone_Name[i]);
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
	if (m_EndBone_Name.size() > vecEndBoneNames.size())
		vecEndBoneNames = m_EndBone_Name;

	m_EndBone_Name = vecEndBoneNames;
	// EndBone 삭제
	for (_uint i = 0; i < m_EndBone_Name.size(); ++i)
	{
		auto pBone = pBoneGroup->Find_BoneData(m_EndBone_Name[i]);
		_uint iBoneIndex = -1;
		if (pBone != nullptr)
			iBoneIndex = pBone->iID;

		_bool bDeleted = false;
		for (_uint j = 0; j < (_uint)vecDelete.size(); j++)
			bDeleted = (vecDelete[j] == iBoneIndex);

		// 삭제
		if (bDeleted)
		{
			auto iter = m_EndBone_Name.begin() + iBoneIndex;
			m_EndBone_Name.erase(iter);
			m_iNumEndBones--;
			i++;
		}
	}
	
	m_pImgui_Manger->InputFloat(u8"속도", &m_fSpeed);
	m_pImgui_Manger->InputFloat(u8"가중치", &m_fWeight);
	_float fRecursive = m_iRecursive;
	m_pImgui_Manger->InputFloat(u8"재귀", &fRecursive);
	m_iRecursive = (_uint)fRecursive;
	ImGui::Checkbox(u8"루프", &m_bLoop);
#pragma endregion

	if (bIsChanged)
		Change_AddAnimation();

	return S_OK;
}

CImgui_Tab_AnimAdd* CImgui_Tab_AnimAdd::Create(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	CImgui_Tab_AnimAdd* pInstance = new CImgui_Tab_AnimAdd(pGameObjectList, pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CImgui_Tab_AnimAdd");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CImgui_Tab_AnimAdd::Free()
{
	__super::Free();

}

void CImgui_Tab_AnimAdd::Change_AddAnimation()
{
	auto pAnimationComp = m_pAnimObject.lock()->Get_ModelCom().lock()->AnimationComp();
	for (_uint i = 0; i < pAnimationComp->Get_NumAdd_Animations(); i++)
	{
		pAnimationComp->Set_ADD_Animation(i, m_AnimSelect_Name, m_BasisBone_Name, m_EndBone_Name
			, m_iRecursive, m_fSpeed, m_fWeight, false);
	}
}

#endif // DEBUG
