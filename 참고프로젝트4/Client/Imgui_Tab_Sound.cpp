#include "stdafx.h"
#ifdef _DEBUG
#include "Imgui_Tab_Sound.h"

#include "ImGuiFileDialog.h"
#include "Imgui_Manager.h"

#include "VIBuffer_TerrainD.h"



CImgui_Tab_Sound::CImgui_Tab_Sound(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CImgui_Tab(pDevice, pContext)
{
}

HRESULT CImgui_Tab_Sound::Initialize()
{
	m_SoundPointDesc.fVolume = 0.5f;
	m_SoundPointDesc.eRollOffMode = FMOD_3D_LINEARSQUAREROLLOFF;
	m_SoundPointDesc.f3DMin = 0.5f;
	m_SoundPointDesc.f3DMax = 10.f;
	m_SoundPointDesc.eSoundGroup = ESoundGroup::Environment;

	m_CreateSoundPointDesc.fVolume = 0.5f;
	m_CreateSoundPointDesc.eRollOffMode = FMOD_3D_LINEARSQUAREROLLOFF;
	m_CreateSoundPointDesc.f3DMin = 0.5f;
	m_CreateSoundPointDesc.f3DMax = 10.f;
	m_CreateSoundPointDesc.eSoundGroup = ESoundGroup::Environment;

	return S_OK;
}

void CImgui_Tab_Sound::Tick(_cref_time fTimeDelta)
{
}

HRESULT CImgui_Tab_Sound::Render()
{
	Layout_Set_SoundData();
	Layout_Update_SoundData();

	if (nullptr != m_pPickedSound)
		Use_ImGuizmo();

	Layout_SaveLoad_SoundData();

	return S_OK;
}

void CImgui_Tab_Sound::Layout_Set_SoundData()
{
	

	ImVec2 vSize = ImVec2(180, 100);
	int DCnt = 0, PCnt = 0, SCnt = 0, ShCnt = 0;

#pragma region 사운드 리스트
	if (ImGui::BeginListBox(u8"사운드 리스트", vSize))
	{
		for (int i = 0; i < m_SoundPointsList.size(); i++)
		{
			shared_ptr<CSoundPoint> pSoundPoint = m_SoundPointsList[i];

			_char szNameTemp[MAX_PATH] = "";
			string NameTemp = pSoundPoint->Get_Name();
			strcpy_s(szNameTemp, NameTemp.c_str());

			if (ImGui::Selectable(szNameTemp, m_iPickedIndex == i))
			{
				m_iPickedIndex = i;
				m_pPickedSound = pSoundPoint;
			}
		}
		ImGui::EndListBox();
	}
	if (m_pPickedSound)
		Invalidate_SoundPointData(m_pPickedSound.get());
#pragma endregion





#pragma region 단축키
	ImGui::Text(u8"상하 화살표를 통해 리스트에서 선택가능");

	if (m_pGameInstance->Key_Down(DIK_DOWN))
	{
		if (m_iPickedIndex < (m_SoundPointsList.size() - 1))
		{
			m_iPickedIndex++;
			m_pPickedSound = m_SoundPointsList[m_iPickedIndex];

			Invalidate_SoundPointData(m_pPickedSound.get());
		}
	}
	else if (m_pGameInstance->Key_Down(DIK_UP))
	{
		if (m_iPickedIndex > 0)
		{
			m_iPickedIndex--;
			m_pPickedSound = m_SoundPointsList[m_iPickedIndex];

			Invalidate_SoundPointData(m_pPickedSound.get());
		}
	}
#pragma endregion




#pragma region 사운드 객체 수정
	ImGui::SeparatorText(u8"사운드 객체 수정");
	if (m_pPickedSound)
	{
		ImGui::PushItemWidth(300);

		{
			string strSoundName = m_pPickedSound->Get_Name();
			// 이름
			_char szName[MAX_PATH] = "";
			size_t iNameSize = sizeof(strSoundName.c_str());
			strcpy_s(szName, strSoundName.c_str());
			if (ImGui::InputText(u8"사운드 객체 이름", szName, MAX_PATH))
			{
				// 중복이름 검사
				if (GI()->Exists_SoundPointName(szName))
				{
					MSG_BOX("Exists SoundPointName");
				}
				else
					m_pPickedSound->Set_Name(szName);
			}
		}

		// 모달 생성 버튼
		if (ImGui::Button(u8"사운드 추가2##SoundTab"))
		{
			ImGui::OpenPopup(u8"사운드 추가2##SoundTab");
		}
		// 모달
		if (ImGui::BeginPopupModal(u8"사운드 추가2##SoundTab"))
		{
			ImGui::Text(u8"사운드 추가하기");

#pragma region 사운드 그룹 콤보박스
			{
				vector<string> SoundGroupNames;
				auto SoundMap = m_pGameInstance->CRef_SoundMap();
				for (auto iter = SoundMap->begin(); iter != SoundMap->end(); iter++)
				{
					SoundGroupNames.push_back(ConvertToString((*iter).first));
				}
				sort(SoundGroupNames.begin(), SoundGroupNames.end());

				string strDisplay;
				if (m_iSelectedSoundGroup_Index == -1)
					strDisplay = u8"선택해 주세요";
				else
					m_strCurrentSound_Name = strDisplay = SoundGroupNames[m_iSelectedSoundGroup_Index];

				if (ImGui::BeginCombo(u8"사운드 그룹 선택하기", strDisplay.c_str()))
				{
					for (_uint i = 0; i < SoundGroupNames.size(); i++)
					{
						_bool bIsSelected = (m_iSelectedSoundGroup_Index == i);
						if (ImGui::Selectable((SoundGroupNames[i]).c_str(), bIsSelected))
						{
							m_iSelectedSoundGroup_Index = i;
							m_iSelectedSound_Index = -1;
						}
						if (bIsSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
				m_strAdd_SoundGroup = strDisplay;
			}
#pragma endregion

#pragma region 사운드 이름 콤보박스
			{
				vector<string> SoundGroupNames;
				auto SoundNames = m_pGameInstance->Provide_GroupSoundNames(ConvertToWstring(m_strCurrentSound_Name));
				for (auto iter = SoundNames.begin(); iter != SoundNames.end(); iter++)
				{
					SoundGroupNames.push_back(ConvertToString(*iter));
				}
				sort(SoundGroupNames.begin(), SoundGroupNames.end());

				// 이름
				string strDisplay;
				if (m_iSelectedSound_Index == -1)
					strDisplay = u8"선택해 주세요";
				else
					strDisplay = SoundGroupNames[m_iSelectedSound_Index];

				// 사운드 이름 선택
				if (ImGui::BeginCombo(u8"사운드 이름 선택하기", strDisplay.c_str(), ImGuiComboFlags_HeightLargest))
				{
					for (_uint i = 0; i < SoundGroupNames.size(); i++)
					{
						_bool bIsSelected = (m_iSelectedSound_Index == i);
						if (ImGui::Selectable((SoundGroupNames[i]).c_str(), bIsSelected))
						{
							m_iSelectedSound_Index = i;
						}
						if (bIsSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
				m_strAdd_SoundName = strDisplay;
			}

			if (ImGui::Button(u8"음원 재생"))
			{
				m_pGameInstance->Play_Sound(ConvertToWstring(m_strAdd_SoundGroup), ConvertToWstring(m_strAdd_SoundName),
					ESoundGroup::Environment, 1.f);
			}

			if (ImGui::Button(u8"추가하기"))
			{
				m_SoundPointDesc.GroupNames.push_back(m_strAdd_SoundGroup);
				m_SoundPointDesc.SoundNames.push_back(m_strAdd_SoundName);
				// 사운드 노티파이 추가 변수 초기화
				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();
			if (ImGui::Button(u8"닫기"))
			{
				// 사운드 노티파이 추가 변수 초기화
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
#pragma endregion
		}

		// 사운드 정보, 카테고리, 사운드 이름
		if (ImGui::BeginListBox(u8"사운드 사운드 이름##SoundTab"))
		{
			for (_int i = 0; i < Cast<_int>(m_SoundPointDesc.GroupNames.size()); i++)
			{
				_bool bIsDelete = false;
				if (ImGui::Selectable(m_SoundPointDesc.SoundNames[i].c_str(), false))
				{
					bIsDelete = true;
				}
				if (bIsDelete)
				{
					auto iterGroup = m_SoundPointDesc.GroupNames.begin() + i;
					m_SoundPointDesc.GroupNames.erase(iterGroup);
					auto iterSound = m_SoundPointDesc.SoundNames.begin() + i;
					m_SoundPointDesc.SoundNames.erase(iterSound);
					--i;
				}

				/*if (bIsSelected)
					ImGui::SetItemDefaultFocus();*/
			}

			ImGui::EndListBox();
		}

		// 사운드 그룹
		_int iSoundGroup = (_int)m_SoundPointDesc.eSoundGroup;
		ImGui::InputInt("사운드 객체 그룹##SoundTab", &iSoundGroup, 1);
		m_SoundPointDesc.eSoundGroup = (ESoundGroup)iSoundGroup;

		// RollOff
		ImGui::InputInt(u8"사운드 객체 RollOffMode##SoundTab", (_int*)&m_SoundPointDesc.eRollOffMode);

		// 볼륨
		ImGui::DragFloat("사운드 객체 볼륨##SoundTab", &m_SoundPointDesc.fVolume, 0.01f, 0.f, 1.f, "%.3f");

		// 위치
		ImGui::DragFloat3(u8"사운드 객체 위치##SoundTab", (_float*)&m_SoundPointDesc.vPosition, 0.01f, -FLT_MAX, FLT_MAX, "%.3f");

		// Min
		ImGui::DragFloat(u8"사운드 객체 Min##SoundTab", &m_SoundPointDesc.f3DMin, 0.01f, 0.f, 1000.f, "%.3f");

		// Max
		ImGui::DragFloat(u8"사운드 객체 Max##SoundTab", &m_SoundPointDesc.f3DMax, 0.01f, 0.f, 1000.f, "%.3f");

		ImGui::PopItemWidth();
		ImGui::Separator();
	}
#pragma endregion




#pragma region 생성
	ImGui::SeparatorText(u8"사운드 객체 [생성, 복사]");
	{
		// 이름
		_char szName[MAX_PATH] = "";
		strcpy_s(szName, m_strCreateSoundPointName.c_str());
		if (ImGui::InputText(u8"생성 사운드 객체 이름", szName, MAX_PATH))
		{
			m_strCreateSoundPointName = szName;
		}

		// 모달 생성 버튼
		if (ImGui::Button(u8"사운드 추가##SoundTab"))
		{
			ImGui::OpenPopup(u8"사운드 추가##SoundTab");
		}
		// 모달
		if (ImGui::BeginPopupModal(u8"사운드 추가##SoundTab"))
		{
			ImGui::Text(u8"사운드 추가하기");

#pragma region 사운드 그룹 콤보박스
			{
				vector<string> SoundGroupNames;
				auto SoundMap = m_pGameInstance->CRef_SoundMap();
				for (auto iter = SoundMap->begin(); iter != SoundMap->end(); iter++)
				{
					SoundGroupNames.push_back(ConvertToString((*iter).first));
				}
				sort(SoundGroupNames.begin(), SoundGroupNames.end());

				string strDisplay;
				if (m_iSelectedSoundGroup_Index == -1)
					strDisplay = u8"선택해 주세요";
				else
					m_strCurrentSound_Name = strDisplay = SoundGroupNames[m_iSelectedSoundGroup_Index];

				if (ImGui::BeginCombo(u8"사운드 그룹 선택하기", strDisplay.c_str()))
				{
					for (_uint i = 0; i < SoundGroupNames.size(); i++)
					{
						_bool bIsSelected = (m_iSelectedSoundGroup_Index == i);
						if (ImGui::Selectable((SoundGroupNames[i]).c_str(), bIsSelected))
						{
							m_iSelectedSoundGroup_Index = i;
							m_iSelectedSound_Index = -1;
						}
						if (bIsSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
				m_strAdd_SoundGroup = strDisplay;
			}
#pragma endregion

#pragma region 사운드 이름 콤보박스
			{
				vector<string> SoundGroupNames;
				auto SoundNames = m_pGameInstance->Provide_GroupSoundNames(ConvertToWstring(m_strCurrentSound_Name));
				for (auto iter = SoundNames.begin(); iter != SoundNames.end(); iter++)
				{
					SoundGroupNames.push_back(ConvertToString(*iter));
				}
				sort(SoundGroupNames.begin(), SoundGroupNames.end());

				// 이름
				string strDisplay;
				if (m_iSelectedSound_Index == -1)
					strDisplay = u8"선택해 주세요";
				else
					strDisplay = SoundGroupNames[m_iSelectedSound_Index];

				// 사운드 이름 선택
				if (ImGui::BeginCombo(u8"사운드 이름 선택하기", strDisplay.c_str(), ImGuiComboFlags_HeightLargest))
				{
					for (_uint i = 0; i < SoundGroupNames.size(); i++)
					{
						_bool bIsSelected = (m_iSelectedSound_Index == i);
						if (ImGui::Selectable((SoundGroupNames[i]).c_str(), bIsSelected))
						{
							m_iSelectedSound_Index = i;
						}
						if (bIsSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
				m_strAdd_SoundName = strDisplay;
			}

			if (ImGui::Button(u8"음원 재생"))
			{
				m_pGameInstance->Play_Sound(ConvertToWstring(m_strAdd_SoundGroup), ConvertToWstring(m_strAdd_SoundName),
					ESoundGroup::Environment, 1.f);
			}

			if (ImGui::Button(u8"추가하기"))
			{
				m_CreateSoundPointDesc.GroupNames.push_back(m_strAdd_SoundGroup);
				m_CreateSoundPointDesc.SoundNames.push_back(m_strAdd_SoundName);
				// 사운드 노티파이 추가 변수 초기화
				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();
			if (ImGui::Button(u8"닫기"))
			{
				// 사운드 노티파이 추가 변수 초기화
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
#pragma endregion
		}

		// 사운드 정보, 카테고리, 사운드 이름
		if (ImGui::BeginListBox(u8"생성 사운드 이름##SoundTab"))
		{
			for (_int i = 0; i < Cast<_int>(m_CreateSoundPointDesc.GroupNames.size()); i++)
			{
				_bool bIsDelete = false;
				if (ImGui::Selectable(m_CreateSoundPointDesc.SoundNames[i].c_str(), false))
				{
					bIsDelete = true;
				}
				if (bIsDelete)
				{
					auto iterGroup = m_CreateSoundPointDesc.GroupNames.begin() + i;
					m_CreateSoundPointDesc.GroupNames.erase(iterGroup);
					auto iterSound = m_CreateSoundPointDesc.SoundNames.begin() + i;
					m_CreateSoundPointDesc.SoundNames.erase(iterSound);
					--i;
				}

				/*if (bIsSelected)
					ImGui::SetItemDefaultFocus();*/
			}

			ImGui::EndListBox();
		}

		// 사운드 그룹
		_int iSoundGroup = (_int)m_CreateSoundPointDesc.eSoundGroup;
		ImGui::InputInt(u8"생성 객체 사운드 그룹##SoundTab", &iSoundGroup);
		m_CreateSoundPointDesc.eSoundGroup = (ESoundGroup)iSoundGroup;

		ImGui::InputInt(u8"생성 객체 RollOffMode##SoundTab", (_int*)&m_CreateSoundPointDesc.eRollOffMode);

		// 볼륨
		ImGui::DragFloat(u8"생성 객체 볼륨##SoundTab", &m_CreateSoundPointDesc.fVolume, 0.01f, 0.f, 1.f, "%.3f");

		// Min
		ImGui::DragFloat(u8"생성 객체 Min##SoundTab", &m_CreateSoundPointDesc.f3DMin, 0.01f, 0.f, 1000.f, "%.3f");

		// Max
		ImGui::DragFloat(u8"생성 객체 Max##SoundTab", &m_CreateSoundPointDesc.f3DMax, 0.01f, 0.f, 1000.f, "%.3f");

		// 위치
		ImGui::DragFloat3(u8"생성 객체 위치##SoundTab", (_float*)&m_CreateSoundPointDesc.vPosition, 0.01f, FLT_MAX, FLT_MAX, "%.3f");
	}

	if (ImGui::Button(u8"생성하기##SoundPoint"))
	{
		shared_ptr<CSoundPoint> pSoundPoint = { nullptr };

		Create_SoundPoint(m_strCreateSoundPointName, m_CreateSoundPointDesc, &pSoundPoint);
	}
	ImGui::SameLine();
#pragma endregion




#pragma region 복사
	if (ImGui::Button(u8"복사##SoundPoint")
		|| (m_pGameInstance->Key_DownEx(DIK_X, DIKK_MD_LCTRL_SHIFT)))
	{
		if ((m_pPickedSound != nullptr) && (m_iPickedIndex != -1))
		{
			m_strCreateSoundPointName = m_strSoundPointName;
			m_CreateSoundPointDesc = m_SoundPointDesc;
			//shared_ptr<CSoundPoint> pSoundPointTemp = m_pPickedSound;
			//GI()->Add_SoundPoint(string("Cloned_") + to_string(GI()->RandomInt(0, 10000000))
			//	, pSoundPointTemp->Get_SoundPointDesc(), &pSoundPointTemp);

			//// 복사된 녀석으로 전환
			//if (pSoundPointTemp)
			//{
			//	m_SoundPointsList.emplace_back(pSoundPointTemp);

			//	m_pPickedSound = pSoundPointTemp;
			//}
		}
	}
#pragma endregion




#pragma region 삭제, 리셋
	ImGui::SeparatorText("!!!Warining!!!");
	if (ImGui::CollapsingHeader(u8"삭제##SoundTab"))
	{
		if (ImGui::Button(u8"사운드 포인트 삭제"))
		{
			if ((m_pPickedSound != nullptr) && (-1 != m_iPickedIndex))
			{
				m_pGameInstance->Release_SoundPoint(m_pPickedSound);
				m_SoundPointsList.erase(m_SoundPointsList.begin() + m_iPickedIndex);

				m_pPickedSound = nullptr;
				if (m_iPickedIndex > 0)
				{
					m_iPickedIndex--;
				}
				else
				{
					m_iPickedIndex = 0;
				}

				m_pPickedSound = m_SoundPointsList[m_iPickedIndex];
			}
		}
	}

	if (ImGui::CollapsingHeader(u8"리셋##SoundTab"))
	{
		if (ImGui::Button(u8"사운드 포인트 리셋"))
		{
			for (auto& pSoundPoint : m_SoundPointsList)
				m_pGameInstance->Release_SoundPoint(pSoundPoint);
			m_SoundPointsList.clear();

			m_pPickedSound = nullptr;
			m_iPickedIndex = -1;
		}
	}
#pragma endregion


	/*ImGui::Separator();

	if (m_bPickingCreateMode && m_pGameInstance->Mouse_Down(DIM_LB)
		 && !ImGuizmo::IsOver() && !ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered())
	{
		_float3 vPickMousePos;
		_int TempIndex = 0;
		if (CImgui_Manager::GetInstance()->Picking_Object(SP_WORLD, L_OBJECT, &vPickMousePos, &TempIndex, nullptr))
		{
			shared_ptr<class CLight> pLightTemp = { nullptr };
			m_tLightDesc.vPosition = _float4(vPickMousePos.x, vPickMousePos.y, vPickMousePos.z, 1.f);
			Create_Light(m_tLightDesc, &pLightTemp);
			m_bPickingCreateMode = false;
		}
	}*/
}

void CImgui_Tab_Sound::Layout_SaveLoad_SoundData()
{
	ImVec2 vSize2 = ImVec2(80, 30);

	ImGui::SeparatorText("File");

	if (ImGui::Button(" Save ", vSize2))
	{
		ImGuiFileDialog::Instance()->OpenDialog("Save_SoundPoint", "Save SoundPoint File", ".json", 
			"../Bin/Data/SoundPoint_Data/");
	}
	ImGui::SameLine();
	if (ImGui::Button(" Load ", vSize2))
	{
		ImGuiFileDialog::Instance()->OpenDialog("Load_SoundPoint", "Load SoundPoint File", ".json", 
			"../Bin/Data/SoundPoint_Data/");
	}

	if (ImGuiFileDialog::Instance()->Display("Save_SoundPoint"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
#pragma region 저장 시작
			FRapidJson OutData;
			string filePath = ImGuiFileDialog::Instance()->GetFilePathName();

			for (auto iter = m_SoundPointsList.begin(); iter != m_SoundPointsList.end(); ++iter)
			{
				FRapidJson SoundData;

				const string strName = (*iter)->Get_Name();
				const vector<string>& GroupNames = (*iter)->Get_GroupNames();
				const vector<string>& SoundNames = (*iter)->Get_SoundNames();
				const _float fVolume = (*iter)->Get_Volume();
				const _uint eSoundGroup = (_uint)(*iter)->Get_SoundGroup();
				const _float3 vPosition = (*iter)->Get_Position();
				const TSoundPointDesc Desc = (*iter)->Get_SoundPointDesc();

				SoundData.Write_StringData("Name", strName);
				for (_uint i = 0; i < (_uint)GroupNames.size(); i++)
					SoundData.Pushback_StringToArray("GroupNames", GroupNames[i]);
				for (_uint i = 0; i < (_uint)SoundNames.size(); i++)
					SoundData.Pushback_StringToArray("SoundNames", SoundNames[i]);
				SoundData.Write_Data("SoundGroup", eSoundGroup);
				SoundData.Write_Data("RollOffMode", Desc.eRollOffMode);
				SoundData.Write_Data("Volume", fVolume);
				SoundData.Write_Data("3DMin", Desc.f3DMin);
				SoundData.Write_Data("3DMax", Desc.f3DMax);
				SoundData.Write_Data("Position", vPosition);

				OutData.Pushback_ObjectToArray("SoundPoints", SoundData);
			}
#pragma endregion


			m_pPickedSound = { nullptr };
			m_iPickedIndex = -1;

			string szFilePath = filePath;
			OutData.Output_Json(ConvertToWstring(szFilePath));
		}
		ImGuiFileDialog::Instance()->Close();
	}

	if (ImGuiFileDialog::Instance()->Display("Load_SoundPoint"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			m_iPickedIndex = { -1 };
			m_pPickedSound = { nullptr };
			m_SoundPointsList.clear();
			m_pGameInstance->Reset_SoundPointManager();

			string filePath = ImGuiFileDialog::Instance()->GetFilePathName();

#pragma region 로드 시작
			FRapidJson InData;
			if (FAILED(InData.Input_Json(ConvertToWstring(filePath))))
				return;

			shared_ptr<CSoundPoint>	pSoundPoint = { nullptr };

			_uint iSize = InData.Read_ArraySize("SoundPoints");
			for (_uint i = 0; i < iSize; i++)
			{
				TSoundPointDesc Desc = {};

				FRapidJson SoundData;
				InData.Read_ObjectFromArray("SoundPoints", i, SoundData);

				_uint iGroupSize = SoundData.Read_ArraySize("GroupNames");
				string strName;

				for (_uint j = 0; j < iGroupSize; j++)
				{
					string strGroupName;
					string strSoundName;
					_uint	iSoundGroup;

					SoundData.Read_Data("Name", strName);
					SoundData.Read_DataFromArray("GroupNames", j, strGroupName);
					SoundData.Read_DataFromArray("SoundNames", j, strSoundName);
					SoundData.Read_Data("SoundGroup", iSoundGroup);
					SoundData.Read_Data("RollOffMode", Desc.eRollOffMode);
					SoundData.Read_Data("Volume", Desc.fVolume);
					SoundData.Read_Data("3DMin", Desc.f3DMin);
					SoundData.Read_Data("3DMax", Desc.f3DMax);
					SoundData.Read_Data("Position", Desc.vPosition);

					Desc.GroupNames.push_back(strGroupName);
					Desc.SoundNames.push_back(strSoundName);
					Desc.eSoundGroup = (ESoundGroup)iSoundGroup;
				}

				Create_SoundPoint(strName, Desc, &pSoundPoint);
			}
#pragma endregion


			m_pPickedSound = { nullptr };
			m_iPickedIndex = -1;
			
			//m_iCreateIndex = m_SoundPointsList.size();
		}
		ImGuiFileDialog::Instance()->Close();
	}	
}

void CImgui_Tab_Sound::Layout_Update_SoundData()
{
	// 피킹 데이터 업데이트
	if ((m_pPickedSound != nullptr) && (-1 != m_iPickedIndex))
	{
		TSoundPointDesc	pDesc = m_pPickedSound->Get_SoundPointDesc();
		string strName = m_pPickedSound->Get_Name();
		
		pDesc = m_SoundPointDesc;
		m_pPickedSound->Set_SoundPointDesc(pDesc);
	}

	// 피킹 취소
	if (m_pGameInstance->Key_DownEx(DIK_Q, DIK_MD_LCTRL))
	{
		m_pPickedSound = nullptr;
		m_iPickedIndex = -1;
	}
}

HRESULT CImgui_Tab_Sound::Create_SoundPoint(const string& strFindName, const TSoundPointDesc SoundPointDesc
	, shared_ptr<CSoundPoint>* ppOut)
{
	shared_ptr<CSoundPoint>	pSoundPoint;

	if (m_pGameInstance->Find_SoundPoint(strFindName) != nullptr)
	{
		MSG_BOX("Light Create Failed : Same Name Light is Exist");
		return E_FAIL;
	}

	m_pGameInstance->Add_SoundPoint(strFindName, SoundPointDesc, &pSoundPoint);
	m_SoundPointsList.push_back(pSoundPoint);

	if (ppOut != nullptr)
		*ppOut = pSoundPoint;

	m_pPickedSound = pSoundPoint;
	m_iPickedIndex = m_SoundPointsList.size() - 1;
	Invalidate_SoundPointData(pSoundPoint.get());

	return S_OK;
}

void CImgui_Tab_Sound::Use_ImGuizmo()
{
	ImGuizmo::SetOrthographic(false);
	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);

	_float* arrView = CImgui_Manager::GetInstance()->m_arrView;
	_float* arrProj = CImgui_Manager::GetInstance()->m_arrProj;

	if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_T))
		m_CurrentGizmoOperation = ImGuizmo::TRANSLATE;
	/*if (ImGui::IsKeyPressed(ImGuiKey_R))
		m_CurrentGizmoOperation = ImGuizmo::ROTATE;*/
	//if (ImGui::IsKeyPressed(ImGuiKey_E))
	//	m_CurrentGizmoOperation = ImGuizmo::SCALE;

	if (ImGui::RadioButton("Translate", m_CurrentGizmoOperation == ImGuizmo::TRANSLATE))
		m_CurrentGizmoOperation = ImGuizmo::TRANSLATE;

	_float4x4 matWorld;
	matWorld = XMMatrixIdentity();
	_float3 vPosition = m_pPickedSound->Get_Position();

	_float arrWorld[] = { matWorld._11,matWorld._12,matWorld._13,matWorld._14,
						  matWorld._21,matWorld._22,matWorld._23,matWorld._24,
						  matWorld._31,matWorld._32,matWorld._33,matWorld._34,
							vPosition.x, vPosition.y, vPosition.z, matWorld._44 };

	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents(arrWorld, matrixTranslation, matrixRotation, matrixScale);
	ImGui::DragFloat3("Tr", matrixTranslation);
	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, arrWorld);

	ImGui::Checkbox("UseSnap", &useSnap);
	ImGui::SameLine();

	switch (m_CurrentGizmoOperation)
	{
	case ImGuizmo::TRANSLATE:
		ImGui::DragFloat3("Snap", &snap[0]);
		break;
	}

	ImGuizmo::Manipulate(arrView, arrProj, m_CurrentGizmoOperation, mCurrentGizmoMode, arrWorld, NULL, useSnap ? &snap[0] : NULL);

	_float4x4 matW = { float4(arrWorld[0],arrWorld[1],arrWorld[2],arrWorld[3]),
					float4(arrWorld[4],arrWorld[5],arrWorld[6],arrWorld[7]),
					float4(arrWorld[8],arrWorld[9],arrWorld[10],arrWorld[11]),
					float4(arrWorld[12],arrWorld[13],arrWorld[14],arrWorld[15]) };
	
	_vector vPos, vRot, vScale;
	XMMatrixDecompose(&vScale, &vRot, &vPos, matW);

	m_pPickedSound->Set_Position(vPos);
}

void CImgui_Tab_Sound::Invalidate_SoundPointData(CSoundPoint* pSoundPoint)
{
	m_strSoundPointName = pSoundPoint->Get_Name();
	m_SoundPointDesc = pSoundPoint->Get_SoundPointDesc();
}

CImgui_Tab_Sound* CImgui_Tab_Sound::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	CImgui_Tab_Sound* pInstance = new CImgui_Tab_Sound(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CImgui_Tab_Sound");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CImgui_Tab_Sound::Free()
{
	__super::Free();
}

#endif // DEBUG
