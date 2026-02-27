#include "stdafx.h"
#ifdef _DEBUG
#include "Imgui_Tab_AnimObject.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"

#include "Imgui_Manager.h"
#include "ImGuiFileDialog.h"

#include <winsock2.h>
#include "AnimObject.h"
#include "CommonModelComp.h"
#include "AnimationComponent.h"
#include "BoneAnimContainer.h"
#include "GameInstance.h"
#include "CommonModelComp.h"
#include "BoneContainer.h"
#include "Model_Manager.h"


CImgui_Tab_AnimObject::CImgui_Tab_AnimObject(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: BASE(pGameObjectList, pDevice, pContext)
{
}

HRESULT CImgui_Tab_AnimObject::Initialize()
{

	return S_OK;
}

void CImgui_Tab_AnimObject::Tick(_cref_time fTimeDelta)  //X
{

}

HRESULT CImgui_Tab_AnimObject::Render()
{
	ImGui::SeparatorText(u8"애니메이션 오브젝트");

	// 모델 이름 받아오기
	auto& ModelMap = *m_pGameInstance->Get_ModelDatas();
	vector<string> ModelNames;
	vector<const _char*> ModelNamesConst;
	for (auto iter = ModelMap.begin(); iter != ModelMap.end(); ++iter)
	{
		// 애니메이션이 있는지 체크
		if ((*iter).second->pAnimGroup->Get_NumAnims() > 0)
			ModelNames.push_back(ConvertToString((*iter).first));
	}
	sort(ModelNames.begin(), ModelNames.end());
	for (auto iter = ModelNames.begin(); iter != ModelNames.end(); ++iter)
	{
		ModelNamesConst.push_back((*iter).c_str());
	}

	if (ImGui::Button(u8"애니메이션 객체 목록 갱신"))
	{
		GI()->Load_DirectorySub_Models(TEXT("Character/"));
	}
	ImGui::SameLine();
	if (ImGui::Button(u8"AnimObject 객체 목록 갱신"))
	{
		GI()->Load_DirectorySub_Models(TEXT("AnimObject/"));
	}

	ImGui::SeparatorText(u8"모델 리스트");
	ImGui::SetNextItemWidth(280);
	if (ImGui::ListBox(u8"##모델 리스트", &m_iModelSelect_Index.Cur, ModelNamesConst.data(), (_int)ModelNamesConst.size(), 8))
	{
		m_ModelSelect_Name = ConvertToWstring(ModelNames[m_iModelSelect_Index.Cur]);
		if (m_iAnimSelect_Index.Cur == m_iAnimSelect_Index.Prev)
		{
			Load_ModelObjectToEditor();
			
		}
	}
	m_iModelSelect_Index.Prev = m_iModelSelect_Index.Cur;

	if (ImGui::Button(u8"모델객체 로드"))
	{
		Load_ModelObjectToEditor();
	}
	ImGui::SameLine();
	if (ImGui::Button(u8"모델객체 삭제"))
	{
		Delete_ModelObjectFromEditor();
	}

	vector<string> AnimNames;
	vector<const _char*> AnimNamesConst;
	if (m_pAnimObject.get() != nullptr)
	{
		CAnimationComponent* pAnimComp = m_pAnimObject->Get_ModelCom().lock()->AnimationComp().get();
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

	//auto& AnimGroup = *m_pGameInstance->Find_AnimGroup()
	ImGui::SeparatorText(u8"애니메이션 리스트");
	ImGui::SetNextItemWidth(280);
	if (ImGui::ListBox(u8"##애니메이션 리스트", &m_iAnimSelect_Index.Cur, AnimNamesConst.data(), (_int)AnimNamesConst.size(), 20))
	{
		// 애니메이션, 더블클릭시 적용
		if (m_iAnimSelect_Index.Cur == m_iAnimSelect_Index.Prev)
		{
			m_AnimSelect_Name = ConvertToWstring(AnimNames[m_iAnimSelect_Index.Cur]);
			Change_Animation();
		}
	}
	m_iAnimSelect_Index.Prev = m_iAnimSelect_Index.Cur;
	if (ImGui::Button(u8"애니메이션 선택"))
	{
		Change_Animation();
	}
	/*ImGui::SameLine();
	if (ImGui::Button(u8"애니메이션 "))
	{

	}*/

	if (nullptr != m_pAnimObject)
	{
		ImGui::SeparatorText(u8"뼈");
		const auto& pBoneGroup = m_pAnimObject->Get_ModelCom().lock()->Get_BoneGroup();

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



#pragma region 루트 본 표시
		{
			_uint iRootBoneIndex = pBoneGroup->CRef_RootTransBoneInd();
			string strPreviewBone = u8"없음";
			if (iNumBoneCount != 0)
			{
				strPreviewBone = BoneNames[iRootBoneIndex];
			}

			ImGui::Text(u8"루트 트랜스 본");
			ImGui::SameLine();
			if (ImGui::Button(strPreviewBone.c_str()))
			{

			}
		}

		{
			_uint iRootBoneIndex = pBoneGroup->CRef_RootRotateBoneInd();
			string strPreviewBone = u8"없음";
			if (iNumBoneCount != 0)
			{
				strPreviewBone = BoneNames[iRootBoneIndex];
			}

			ImGui::SameLine();
			ImGui::Text(u8"루트 로테이트 본");
			ImGui::SameLine();
			if (ImGui::Button(strPreviewBone.c_str()))
			{

			}
		}

		{
			_uint iRootBoneIndex = pBoneGroup->CRef_RootScaleBoneInd();
			string strPreviewBone = u8"없음";
			if (iNumBoneCount != 0)
			{
				strPreviewBone = BoneNames[iRootBoneIndex];
			}

			ImGui::SameLine();
			ImGui::Text(u8"루트 스케일 본");
			ImGui::SameLine();
			if (ImGui::Button(strPreviewBone.c_str()))
			{

			}
		}
#pragma endregion



#pragma region 뼈 선택
		{
			string strPreviewBone;
			if (iNumBoneCount == 0)
			{
				strPreviewBone = "없음";
				m_iBoneSelect_index = -1;
			}
			else
			{
				if (m_iBoneSelect_index == -1)
					m_iBoneSelect_index = 0;
				strPreviewBone = BoneNames[m_iBoneSelect_index];
			}

			if (ImGui::BeginCombo(u8"뼈 선택하기", strPreviewBone.c_str()))
			{
				for (_uint i = 0; i < iNumBoneCount; i++)
				{
					_bool bIsSelected = (m_iBoneSelect_index == i);
					if (ImGui::Selectable((BoneNames[i]).c_str(), bIsSelected))
					{
						m_iBoneSelect_index = i;
					}
					if (bIsSelected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
		}
#pragma endregion

		ImGui::SeparatorText(u8"오브젝트 기즈모");
		Use_ImGuizmo(m_pAnimObject, true);
	}

	return S_OK;
}

CImgui_Tab_AnimObject* CImgui_Tab_AnimObject::Create(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	CImgui_Tab_AnimObject* pInstance = new CImgui_Tab_AnimObject(pGameObjectList, pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CImgui_Tab_AnimObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CImgui_Tab_AnimObject::Free()
{
	__super::Free();


}

void CImgui_Tab_AnimObject::Load_ModelObjectToEditor()
{
	if (m_iModelSelect_Index.Cur == -1)
		return;

	// 기존 오브젝트 삭제하기
	Delete_ModelObjectFromEditor();

	// 게임오브젝트 불러오기
	CAnimObject::TMODEL_DESC Desc = {};
	Desc.strModelTag = m_ModelSelect_Name;

	shared_ptr<CAnimObject> pAnimObject = { nullptr };
	if (FAILED(GI()->Add_CloneObject(LEVEL_TOOL, L_PLAYER,
		TEXT("Prototype_GameObject_AnimObject"), &Desc, ReCast<shared_ptr<CAnimObject>*>(&pAnimObject))))
		return;

	m_pAnimObject = pAnimObject;
	m_pAnimObject->Get_ModelCom().lock()->AnimationComp()->Create_MaskAnim();
}

void CImgui_Tab_AnimObject::Delete_ModelObjectFromEditor()
{
	// 기존 오브젝트 삭제하기
	if (m_pAnimObject.get() != nullptr)
	{
		m_pAnimObject->Set_Dead();
		m_pAnimObject.reset();
		m_iBoneSelect_index = -1;
	}
}

void CImgui_Tab_AnimObject::TogglePlay_AnimObject()
{
	if (m_pAnimObject.get() != nullptr)
	{
		if (m_pAnimObject->Get_IsAnimPlaying())
			m_pAnimObject->Set_AnimPlay(false);
		else
			m_pAnimObject->Set_AnimPlay(true);
	}
}

void CImgui_Tab_AnimObject::Play_AnimObject()
{
	if (m_pAnimObject.get() != nullptr)
	{
		m_pAnimObject->Set_AnimPlay(true);
	}
}

void CImgui_Tab_AnimObject::Pause_AnimObject()
{
	if (m_pAnimObject.get() != nullptr)
	{
		m_pAnimObject->Set_AnimPlay(false);
	}
}

void CImgui_Tab_AnimObject::Reset_AnimObject()
{
	if (m_pAnimObject.get() != nullptr)
	{
		m_pAnimObject->Set_AnimPlay(false);
	}
}

void CImgui_Tab_AnimObject::Set_TrackPos_AnimObject(_int iIndex)
{
	if (m_pAnimObject.get() != nullptr)
	{
		m_pAnimObject->Get_ModelCom().lock()->AnimationComp()->Set_AnimTrackPos(iIndex);
	}
}

_int CImgui_Tab_AnimObject::Get_TrackPos_AnimObject()
{
	if (m_pAnimObject.get() == nullptr)
		return 0;

	return Cast<_int>(m_pAnimObject->Get_ModelCom().lock()->AnimationComp()->Get_AnimTrackPos());
}

_int CImgui_Tab_AnimObject::Get_Duration_AnimObject()
{
	if (m_pAnimObject.get() == nullptr)
		return 0;

	return Cast<_int>(m_pAnimObject->Get_ModelCom().lock()->AnimationComp()->Get_AnimDuration());
}

void CImgui_Tab_AnimObject::Change_Animation()
{
	if (m_iAnimSelect_Index.Cur == -1)
		return;

	if (m_pAnimObject.get() != nullptr)
	{
		m_pAnimObject->Get_ModelCom().lock()->AnimationComp()->Set_Animation(m_AnimSelect_Name, 1.f, 1.f);
	}
}

#endif // DEBUG
