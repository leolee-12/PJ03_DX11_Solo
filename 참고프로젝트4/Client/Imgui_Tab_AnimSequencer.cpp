#include "stdafx.h"
#ifdef _DEBUG
#include "Imgui_Tab_AnimSequencer.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"

#include "GameInstance.h"
#include "Imgui_Manager.h"
#include "ImGuiFileDialog.h"
#include "Imgui_Window_AnimMainEdit.h"
#include "Imgui_Tab_AnimObject.h"
#include "imgui_neo_sequencer.h"
#include "Sound_Manager.h"
#include "Utility/RapidJson_Utility.h"
#include "Utility_File.h"
#include "Imgui_Tab_AnimOffset.h"
#include "AnimObject.h"

#include <winsock2.h>



CImgui_Tab_AnimSequencer::CImgui_Tab_AnimSequencer(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: BASE(pGameObjectList, pDevice, pContext)
{
}

HRESULT CImgui_Tab_AnimSequencer::Initialize()
{
	m_pNotifyComp = CAnimNotifyComp::Create(m_pDevice, m_pContext);
	m_pNotifyComp->Regist_AnimNotifyCreatorCallback(&CAnimNotifyCollection_Client::Create_Callback);

	return S_OK;
}

void CImgui_Tab_AnimSequencer::Tick(_cref_time fTimeDelta)  //X
{

}

HRESULT CImgui_Tab_AnimSequencer::Render()
{
	auto pAnimEdit = m_pImgui_Manger->Find_Window<CImgui_Window_AnimEdit>(CImgui_Manager::TOOL_ANIM);
	auto pAnimObjectTab = pAnimEdit->Tab_AnimObject();

	// 매프레임 오브젝트 윈도우로부터 정보를 읽어와 동기화 해준다.
	{
		// 만료된 객체는 지운다.
		if (m_pAnimObject.expired())
			m_pNotifyComp->Clear_NotifyAll();

		m_pAnimObject = pAnimObjectTab->Get_AnimObject();
		if (m_pAnimObject.lock() != nullptr)
		{
			string strAnimName = m_pAnimObject.lock()->Get_ModelCom().lock()->Get_AnimationComponent()->Get_CurrentAnimName();
			m_pNotifyComp->Set_CurrentAnimation(strAnimName);
			m_pNotifyComp->Regist_ModelComponent(m_pAnimObject.lock()->Get_ModelCom());
			Reserve_NotifyContainer();
		}
		
	}

	if (ImGui::InputInt(u8"##SequenceMin", &m_iStartFrame))
	{

	}

	// 재생, 멈춤
	ImGui::SameLine();
	if (ImGui::Button(u8">") || (ImGui::IsKeyDown(ImGuiKey_LeftShift) && ImGui::IsKeyPressed(ImGuiKey_Space)))
	{
		// 재생기능
		pAnimEdit->Tab_AnimObject()->TogglePlay_AnimObject();
	}

	// 트랙 포즈 얻어오기
	{
		m_iCurrentFrame = pAnimEdit->Tab_AnimObject()->Get_TrackPos_AnimObject();
	}

	// 현재 트랙 포즈 표시
	ImGui::SameLine();
	if (ImGui::Button(to_string(m_iCurrentFrame).c_str()))
	{

	}

	ImGui::SameLine();
	if (ImGui::InputInt(u8"##SequenceMax", &m_iEndFrame))
	{

	}

	if ((ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_R)))
	{
		m_iStartFrame = 0;
		m_iEndFrame = pAnimEdit->Tab_AnimObject()->Get_Duration_AnimObject();
	}

	// 추가 버튼
	ImGui::SameLine();
	if (ImGui::Button(u8"이벤트 초기화")
		|| m_iCurrentFrame == 0)
	{
		Reset_NotifyEvents();
	}
	ImGui::SameLine();

	Render_EffectNotify();
	Render_SoundNotify();
	Render_EventNotify();
	Render_BakeNotifies();


	// 애니메이션 길이 업데이트
	{
		m_Keys[ESeqType_Animation].clear();
		m_Keys[ESeqType_Animation].push_back(0);
		m_Keys[ESeqType_Animation].push_back(pAnimEdit->Tab_AnimObject()->Get_Duration_AnimObject());
	}

	// 키 업데이트
	if (IsNotifyCollection_Valid())
	{
		if (m_pNotifyComp->Get_CollectionByCurAnim()->Get_NotifyGroupPtr("Effect"))
		{
			m_Keys[ESeqType_Effect].resize((*m_pNotifyComp->Get_CollectionByCurAnim()->
				Get_NotifyGroupPtr("Effect")).size());
			for (_uint i = 0; i < m_Keys[ESeqType_Effect].size(); i++)
			{
				m_Keys[ESeqType_Effect][i] = (_int)(*m_pNotifyComp->Get_CollectionByCurAnim()->
					Get_NotifyGroupPtr("Effect"))[i]->Get_NotifyTrackPos();
			}
		}

		if (m_pNotifyComp->Get_CollectionByCurAnim()->Get_NotifyGroupPtr("Sound"))
		{
			m_Keys[ESeqType_Sound].resize((*m_pNotifyComp->Get_CollectionByCurAnim()->
				Get_NotifyGroupPtr("Sound")).size());
			for (_uint i = 0; i < m_Keys[ESeqType_Sound].size(); i++)
			{
				m_Keys[ESeqType_Sound][i] = (_int)(*m_pNotifyComp->Get_CollectionByCurAnim()->
					Get_NotifyGroupPtr("Sound"))[i]->Get_NotifyTrackPos();
			}
		}

		if (m_pNotifyComp->Get_CollectionByCurAnim()->Get_NotifyGroupPtr("Event"))
		{
			m_Keys[ESeqType_Event].resize((*m_pNotifyComp->Get_CollectionByCurAnim()->
				Get_NotifyGroupPtr("Event")).size());
			for (_uint i = 0; i < m_Keys[ESeqType_Event].size(); i++)
			{
				m_Keys[ESeqType_Event][i] = (_int)(*m_pNotifyComp->Get_CollectionByCurAnim()->
					Get_NotifyGroupPtr("Event"))[i]->Get_NotifyTrackPos();
			}
		}
	}

	if (ImGui::BeginNeoSequencer("SequencerNeo", &m_iCurrentFrame, &m_iStartFrame, &m_iEndFrame, { 0, 0 },
		ImGuiNeoSequencerFlags_EnableSelection |
		ImGuiNeoSequencerFlags_Selection_EnableDragging |
		ImGuiNeoSequencerFlags_Selection_EnableDeletion))
	{
		if (ImGui::BeginNeoGroup("Animation##AnimSequence", &m_bIsSeqTrackOpen[ESeqType_Animation]))
		{
			Render_AnimationTrack();
			Render_EffectTrack();
			Render_SoundTrack();
			Render_EventTrack();

			ImGui::EndNeoGroup();
		}

		ImGui::EndNeoSequencer();
	}

	// 이동된 키 업데이트
	if (IsNotifyCollection_Valid())
	{
		for (_uint i = 0; i < m_Keys[ESeqType_Effect].size(); i++)
		{
			(*m_pNotifyComp->Get_CollectionByCurAnim()->
				Get_NotifyGroupPtr("Effect"))[i]->Set_NotifyTrackPos((_float)m_Keys[ESeqType_Effect][i]);
		}

		for (_uint i = 0; i < m_Keys[ESeqType_Sound].size(); i++)
		{
			(*m_pNotifyComp->Get_CollectionByCurAnim()->
				Get_NotifyGroupPtr("Sound"))[i]->Set_NotifyTrackPos((_float)m_Keys[ESeqType_Sound][i]);
		}

		for (_uint i = 0; i < m_Keys[ESeqType_Event].size(); i++)
		{
			(*m_pNotifyComp->Get_CollectionByCurAnim()->
				Get_NotifyGroupPtr("Event"))[i]->Set_NotifyTrackPos((_float)m_Keys[ESeqType_Event][i]);
		}
		m_pNotifyComp->Get_CollectionByCurAnim()->Sort_Notifies();
	}
	
	// 현재 프레임 변경됨
	if (m_iPrevFrame != m_iCurrentFrame)
	{
		pAnimEdit->Tab_AnimObject()->Set_TrackPos_AnimObject(m_iCurrentFrame);

		// 트리거 작동
		if (IsNotifyCollection_Valid())
		{
			m_pNotifyComp->Get_CollectionByCurAnim()->OnTrigger((_float)m_iCurrentFrame);
		}
	}
	// 프레임 저장
	m_iPrevFrame = m_iCurrentFrame;


	

	return S_OK;
}

CImgui_Tab_AnimSequencer* CImgui_Tab_AnimSequencer::Create(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	CImgui_Tab_AnimSequencer* pInstance = new CImgui_Tab_AnimSequencer(pGameObjectList, pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CImgui_Tab_AnimSequencer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CImgui_Tab_AnimSequencer::Free()
{
	__super::Free();

	m_pNotifyComp->Clear_NotifyAll();
}

HRESULT CImgui_Tab_AnimSequencer::Render_AnimationTrack()
{
	if (ImGui::BeginNeoTimelineEx("Range"), nullptr, ImGuiNeoTimelineFlags_AllowFrameChanging)
	{
		for (auto&& v : m_Keys[ESeqType_Animation])
		{
			ImGui::NeoKeyframe(&v);
		}

		if (ImGui::IsKeyPressed(ImGuiKey_Delete))
		{
			_uint iCount = ImGui::GetNeoKeyframeSelectionSize();

			ImGui::FrameIndexType* toRemove = new ImGui::FrameIndexType[iCount];

			ImGui::GetNeoKeyframeSelection(toRemove);
		}

		ImGui::EndNeoTimeLine();
	}

	return S_OK;
}

HRESULT CImgui_Tab_AnimSequencer::Render_EffectTrack()
{
	// 
	if (ImGui::BeginNeoTimelineEx("Effect"), nullptr, ImGuiNeoTimelineFlags_AllowFrameChanging)
	{
		for (auto&& v : m_Keys[ESeqType_Effect])
		{
			ImGui::NeoKeyframe(&v);
		}
		/*if (ImGui::IsKeyPressed(ImGuiKey_Delete))
		{
			_uint iCount = ImGui::GetNeoKeyframeSelectionSize();

			ImGui::FrameIndexType* toRemove = new ImGui::FrameIndexType[iCount];

			ImGui::GetNeoKeyframeSelection(toRemove);
		}*/

		ImGui::EndNeoTimeLine();
	}

	return S_OK;
}

HRESULT CImgui_Tab_AnimSequencer::Render_SoundTrack()
{
	if (ImGui::BeginNeoTimelineEx("Sound"), nullptr, ImGuiNeoTimelineFlags_AllowFrameChanging)
	{
		for (auto&& v : m_Keys[ESeqType_Sound])
		{
			ImGui::NeoKeyframe(&v);
		}

		/*if (ImGui::IsKeyPressed(ImGuiKey_Delete))
		{
			_uint iCount = ImGui::GetNeoKeyframeSelectionSize();

			ImGui::FrameIndexType* toRemove = new ImGui::FrameIndexType[iCount];

			ImGui::GetNeoKeyframeSelection(toRemove);
		}*/

		ImGui::EndNeoTimeLine();
	}

	return S_OK;
}

HRESULT CImgui_Tab_AnimSequencer::Render_EventTrack()
{
	if (ImGui::BeginNeoTimelineEx("Event"))
	{
		for (auto&& v : m_Keys[ESeqType_Event])
		{
			ImGui::NeoKeyframe(&v);
		}

		if (ImGui::IsKeyPressed(ImGuiKey_Delete))
		{
			_uint iCount = ImGui::GetNeoKeyframeSelectionSize();

			ImGui::FrameIndexType* toRemove = new ImGui::FrameIndexType[iCount];

			ImGui::GetNeoKeyframeSelection(toRemove);
		}

		ImGui::EndNeoTimeLine();
	}

	return S_OK;
}

HRESULT CImgui_Tab_AnimSequencer::Render_EffectNotify()
{

	// 노티파이 추가 버튼
	ImGui::SameLine();
	if (ImGui::Button(u8"이펙트 노티파이 추가"))
	{
		if (!m_pAnimObject.expired())
		{
			ImGui::OpenPopup(u8"이펙트 노티파이 추가");
			m_iSoundNotify_TrackPos = m_iCurrentFrame;
		}
	}
	// 모달
	if (ImGui::BeginPopupModal(u8"이펙트 노티파이 추가"))
	{
		ImGui::Text(u8"이펙트 추가하기");

		ImGui::InputInt(u8"키프레임##Notify_Effect", &m_iSoundNotify_TrackPos);


#pragma region 이펙트 그룹
		{
			string strDisplay;
			if (m_iSelectedEffectGroup_Index == -1)
				strDisplay = u8"선택해 주세요";
			else
				m_strCurrentEffectGroup_Name = strDisplay = m_EffectNotifyNameList[m_iSelectedEffectGroup_Index];

			if (ImGui::BeginCombo(u8"이펙트 그룹 선택하기", strDisplay.c_str()))
			{
				for (_uint i = 0; i < 5; i++)
				{
					_bool bIsSelected = (m_iSelectedEffectGroup_Index == i);
					if (ImGui::Selectable(m_EffectNotifyNameList[i], bIsSelected))
					{
						m_iSelectedEffectGroup_Index = i;
						m_iSelectedEffect_Index = -1;
					}
					if (bIsSelected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			m_EffectNotifyDesc.iEffectType = Cast<EFFECT_TYPE>(m_iSelectedEffectGroup_Index);
		}
#pragma endregion


#pragma region 이펙트 이름 콤보박스
		{
			//CUtility_File::Get_FilePath(CPath_Mgr::DATA_FILE, );

			auto DataPathes = CUtility_File::CRef_DataPathes();
			vector<string> EffectFileName;
			for (auto iter = DataPathes.begin(); iter != DataPathes.end(); ++iter)
			{
				if ((*iter).second.find(StrToWstr(m_strCurrentEffectGroup_Name)) != wstring::npos)
					EffectFileName.push_back(WstrToStr((*iter).first));
			}

			string strDisplay;
			if (m_iSelectedEffect_Index == -1)
				strDisplay = u8"선택해 주세요";
			else
				strDisplay = EffectFileName[m_iSelectedEffect_Index];

			if (ImGui::BeginCombo(u8"이펙트 이름 선택하기", strDisplay.c_str()))
			{
				for (_uint i = 0; i < EffectFileName.size(); i++)
				{
					_bool bIsSelected = (m_iSelectedEffect_Index == i);
					if (ImGui::Selectable((EffectFileName[i]).c_str(), bIsSelected))
					{
						m_iSelectedEffect_Index = i;
					}
					if (bIsSelected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			m_EffectNotifyDesc.strEffectName = ConvertToWstring(strDisplay);
		}
#pragma endregion
		if (ImGui::Button(u8"추가하기"))
		{
			// 행렬을 오프셋으로
			auto pAnimEdit = m_pImgui_Manger->Find_Window<CImgui_Window_AnimEdit>(CImgui_Manager::TOOL_ANIM);
			auto pAnimOffsetTab = pAnimEdit->Tab_AnimOffset();
			auto pAnimObjectTab = pAnimEdit->Tab_AnimObject();
			auto pAnimObject = pAnimObjectTab->Get_AnimObject();
			_float4x4 MatW = pAnimOffsetTab->Get_OffsetMatrix();

			_vector vPos, vRot, vScale;
			XMMatrixDecompose(&vScale, &vRot, &vPos, MatW);
			m_EffectNotifyDesc.vPos = vPos;
			m_EffectNotifyDesc.vRot = vRot;

			m_pNotifyComp->Get_CollectionByCurAnim()->Add_Notify("Effect", DynPtrCast<CAnimNotify>(
					CAnimNotify_Effect::Create((_float)m_iSoundNotify_TrackPos, m_EffectNotifyDesc)));
			m_pNotifyComp->Get_CollectionByCurAnim()->Regist_ModelComponent(pAnimObject->Get_ModelCom());

				// 사운드 노티파이 추가 변수 초기화
			m_EffectNotifyDesc = {};
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();
		if (ImGui::Button(u8"닫기"))
		{
			// 사운드 노티파이 추가 변수 초기화
			m_EffectNotifyDesc = {};
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	return S_OK;
}

HRESULT CImgui_Tab_AnimSequencer::Render_SoundNotify()
{
	// 모달 생성 버튼
	ImGui::SameLine();
	if (ImGui::Button(u8"사운드 노티파이 추가"))
	{
		if (!m_pAnimObject.expired())
		{
			ImGui::OpenPopup(u8"사운드 노티파이 추가");
			//m_iSelectedSoundGroup_Index = -1;
			//m_iSelectedSound_Index = -1;
			m_SoundNotifyDesc.fSoundVolume = 1.f;
			m_SoundNotifyDesc.fRandom = 100.f;
			m_iSoundNotify_TrackPos = m_iCurrentFrame;
		}
	}
	// 모달
	if (ImGui::BeginPopupModal(u8"사운드 노티파이 추가"))
	{
		ImGui::Text(u8"사운드 추가하기");

		ImGui::InputInt(u8"키프레임##Notify_Sound", &m_iSoundNotify_TrackPos);

#pragma region 노티파이 이름
		{
			_char szName[MAX_PATH] = {};
			strcpy_s(szName, m_SoundNotifyDesc.strName.c_str());
			if (ImGui::InputText(u8"노티파이 이름##SoudNotify", szName, MAX_PATH,
				ImGuiInputTextFlags_EscapeClearsAll | ImGuiInputTextFlags_EnterReturnsTrue))
			{
				m_SoundNotifyDesc.strName = szName;
			}
		}
#pragma endregion


		// 사운드 그룹 콤보박스
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
			m_strAddNotify_SoundGroup = strDisplay;
		}
#pragma endregion




		// 사운드 이름 콤보박스
#pragma region 사운드 이름 콤보박스
		{
			vector<string> SoundGroupNames;
			auto SoundNames = m_pGameInstance->Provide_GroupSoundNames(ConvertToWstring(m_strCurrentSound_Name));
			for (auto iter = SoundNames.begin(); iter != SoundNames.end(); iter++)
			{
				SoundGroupNames.push_back(ConvertToString(*iter));
			}
			sort(SoundGroupNames.begin(), SoundGroupNames.end());
			string strDisplay;
			if (m_iSelectedSound_Index == -1)
				strDisplay = u8"선택해 주세요";
			else
				strDisplay = SoundGroupNames[m_iSelectedSound_Index];

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
			m_strAddNotify_SoundName = strDisplay;
		}
#pragma endregion


		

#pragma region 볼륨, 채널
		// 볼륨, 채널
		ImGui::SliderFloat(u8"사운드 볼륨##Sound_Notify", &m_SoundNotifyDesc.fSoundVolume, 0, 1.f, "%.2f");
		if (ImGui::BeginCombo(u8"사운드 채널그룹##Sound_Notify", SoundGroupNames[m_SoundNotifyDesc.iSoundChannel]))
		{
			for (_int i = 0; i < ECast(ESoundGroup::End); i++)
			{
				_bool bIsSelected = (m_SoundNotifyDesc.iSoundChannel == i);
				if (ImGui::Selectable(SoundGroupNames[i], bIsSelected))
				{
					m_SoundNotifyDesc.iSoundChannel = i;
				}
				if (bIsSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		ImGui::SliderFloat(u8"사운드 재생확률##Sound_Notify", &m_SoundNotifyDesc.fRandom, 0, 100.f, "%.2f");
#pragma endregion



#pragma region 테스트 재생
		if (ImGui::Button(u8"음원 재생"))
		{
			m_pGameInstance->Play_Sound(ConvertToWstring(m_strAddNotify_SoundGroup), ConvertToWstring(m_strAddNotify_SoundName),
				(ESoundGroup)m_SoundNotifyDesc.iSoundChannel, m_SoundNotifyDesc.fSoundVolume);
		}
#pragma endregion


#pragma region 음원 목록 추가
		if (ImGui::BeginListBox(u8"추가된 음원##AddedSoundNotify"))
		{
			_int iDelete = -1;
			for (_int i = 0; i < Cast<_int>(m_SoundNotifyDesc.strSoundNameTags.size()); i++)
			{
				if (ImGui::Selectable(ConvertToString(m_SoundNotifyDesc.strSoundNameTags[i]).c_str(), false))
				{
					iDelete = i;
				}
				/*if (bIsSelected)
					ImGui::SetItemDefaultFocus();*/
			}

			if (iDelete != -1)
			{
				auto iter = m_SoundNotifyDesc.strSoundGroupTags.begin() + iDelete;
				m_SoundNotifyDesc.strSoundGroupTags.erase(iter);
				iter = m_SoundNotifyDesc.strSoundNameTags.begin() + iDelete;
				m_SoundNotifyDesc.strSoundNameTags.erase(iter);
			}

			ImGui::EndListBox();
		}

		if (ImGui::Button(u8"음원 목록 추가"))
		{
			m_SoundNotifyDesc.strSoundGroupTags.push_back(ConvertToWstring(m_strAddNotify_SoundGroup));
			m_SoundNotifyDesc.strSoundNameTags.push_back(ConvertToWstring(m_strAddNotify_SoundName));
		}
#pragma endregion



#pragma region 완료, 취소
		if (ImGui::Button(u8"추가하기") && !m_pAnimObject.expired())
		{
			m_pNotifyComp->Get_CollectionByCurAnim()->Add_Notify("Sound", DynPtrCast<CAnimNotify>
					(CAnimNotify_Sound::Create((_float)m_iSoundNotify_TrackPos, m_SoundNotifyDesc)));

			// 사운드 노티파이 추가 변수 초기화
			string strTemp = m_SoundNotifyDesc.strName;
			_int iTemp = m_SoundNotifyDesc.iSoundChannel;
			m_SoundNotifyDesc = {};
			m_SoundNotifyDesc.strName = strTemp;
			m_SoundNotifyDesc.iSoundChannel = iTemp;
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();
		if (ImGui::Button(u8"닫기"))
		{
			// 사운드 노티파이 추가 변수 초기화
			string strTemp = m_SoundNotifyDesc.strName;
			_int iTemp = m_SoundNotifyDesc.iSoundChannel;
			m_SoundNotifyDesc = {};
			m_SoundNotifyDesc.strName = strTemp;
			m_SoundNotifyDesc.iSoundChannel = iTemp;
			ImGui::CloseCurrentPopup();
		}
#pragma endregion

		ImGui::EndPopup();
	}

	return S_OK;
}

HRESULT CImgui_Tab_AnimSequencer::Render_EventNotify()
{
	ImGui::SameLine();
	if (ImGui::Button(u8"이벤트 노티파이 추가"))
	{
		if (!m_pAnimObject.expired())
		{
			ImGui::OpenPopup(u8"이벤트 노티파이 추가");
			m_iSoundNotify_TrackPos = m_iCurrentFrame;
		}
	}
	if (ImGui::BeginPopupModal(u8"이벤트 노티파이 추가"))
	{
		ImGui::Text(u8"이벤트 추가하기");

		ImGui::InputInt(u8"키프레임##Notify_Event", &m_iSoundNotify_TrackPos);

#pragma region 노티파이 이름
		{
			_char szName[MAX_PATH] = {};
			strcpy_s(szName, m_EventNotifyDesc.strName.c_str());
			if (ImGui::InputText(u8"노티파이 이름##SoudNotify", szName, MAX_PATH,
				ImGuiInputTextFlags_EscapeClearsAll | ImGuiInputTextFlags_EnterReturnsTrue))
			{
				m_EventNotifyDesc.strName = szName;
			}
		}
#pragma endregion

#pragma region 완료, 취소
		if (ImGui::Button(u8"추가하기") && !m_pAnimObject.expired())
		{
			m_pNotifyComp->Get_CollectionByCurAnim()->Add_Notify("Event", DynPtrCast<CAnimNotify>
				(CAnimNotify_Event::Create((_float)m_iSoundNotify_TrackPos, m_EventNotifyDesc)));

			// 사운드 노티파이 추가 변수 초기화
			string strTemp = m_EventNotifyDesc.strName;
			m_EventNotifyDesc = {};
			m_EventNotifyDesc.strName = strTemp;
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();
		if (ImGui::Button(u8"닫기"))
		{
			// 사운드 노티파이 추가 변수 초기화
			string strTemp = m_EventNotifyDesc.strName;
			m_EventNotifyDesc = {};
			m_EventNotifyDesc.strName = strTemp;
			ImGui::CloseCurrentPopup();
		}
#pragma endregion

		ImGui::EndPopup();
	}

	return S_OK;
}

HRESULT CImgui_Tab_AnimSequencer::Render_BakeNotifies()
{
	ImGui::SameLine();
	if (ImGui::Button(u8"노티파이 내보내기"))
	{
		if (!m_pAnimObject.expired())
		{
			ImGuiFileDialog::Instance()->OpenDialog("SaveNotify", "Save Notify File", ".json", "../Bin/Data/Anim_Data/");
		}
	}
	if (ImGuiFileDialog::Instance()->Display("SaveNotify"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			Parse_Notifies(ConvertToWstring(filePathName));
		}
		ImGuiFileDialog::Instance()->Close();
	}
	ImGui::SameLine();
	if (ImGui::Button(u8"노티파이 부르기"))
	{
		if (!m_pAnimObject.expired())
		{
			ImGuiFileDialog::Instance()->OpenDialog("LoadNotify", "Load Notify File", ".json", "../Bin/Data/Anim_Data/");
		}
	}
	if (ImGuiFileDialog::Instance()->Display("LoadNotify"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			Read_Notifies(ConvertToWstring(filePathName));
		}
		ImGuiFileDialog::Instance()->Close();
	}

	return S_OK;
}

void CImgui_Tab_AnimSequencer::Regist_NotifyCreatorCallback()
{
	if (m_pNotifyComp == nullptr)
		return;

	m_pNotifyComp->Regist_AnimNotifyCreatorCallback(&CAnimNotifyCollection_Client::Create_Callback);
}

_bool CImgui_Tab_AnimSequencer::IsNotifyCollection_Valid()
{
	return !m_pAnimObject.expired() && (m_pNotifyComp->Get_CollectionByCurAnim() != nullptr);
}

HRESULT CImgui_Tab_AnimSequencer::Parse_Notifies(const wstring& strFilePath)
{
	if (m_pNotifyComp == nullptr)
		RETURN_EFAIL;

	if (m_pAnimObject.expired())
		return E_FAIL;

	m_pNotifyComp->Output_AnimNotify(strFilePath);

	return S_OK;
}

HRESULT CImgui_Tab_AnimSequencer::Read_Notifies(const wstring& strFilePath)
{
	if (m_pNotifyComp == nullptr)
		RETURN_EFAIL;

	if (m_pAnimObject.expired())
		return E_FAIL;

	m_pNotifyComp->Clear_NotifyAll();
	m_pNotifyComp->Load_AnimNotify(strFilePath);

	return S_OK;
}

void CImgui_Tab_AnimSequencer::Reset_NotifyEvents()
{
	if (!IsNotifyCollection_Valid())
		return;

	m_pNotifyComp->Get_CollectionByCurAnim()->OnReset();
}

void CImgui_Tab_AnimSequencer::Reserve_NotifyContainer()
{
	if (m_pNotifyComp == nullptr)
		return;

	m_pNotifyComp->Reserve_Collection("Effect");
	m_pNotifyComp->Reserve_Collection("Sound");
	m_pNotifyComp->Reserve_Collection("Event");
}

#endif // DEBUG
