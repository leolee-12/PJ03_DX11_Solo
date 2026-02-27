#include "stdafx.h"
//
//#include "imgui.h"
//#include "imgui_impl_win32.h"
//#include "imgui_impl_dx11.h"
//#include "imgui_internal.h"
//
//#include "Imgui_Tab_ModelAnim.h"
//
//#include "Imgui_Manager.h"
//#include "Animation.h"
//#include "PartObject.h"
//#include "Model.h"
//#include "Particle.h"
//
//
//CImgui_Tab_ModelAnim::CImgui_Tab_ModelAnim(vector< CGameObject*>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
//	: CImgui_Tab(pGameObjectList, pDevice, pContext)
//{
//}
//
//HRESULT CImgui_Tab_ModelAnim::Initialize(CImgui_Tab_ParticleEdit* pParticleTab, CImgui_Tab_TrailEdit* pTrailTab)
//{
//	m_pParticleTab = pParticleTab;
//	m_pTrailTab = pTrailTab;
//
//    return S_OK;
//}
//
//void CImgui_Tab_ModelAnim::Tick(_cref_time fTimeDelta)
//{
//}
//
//HRESULT CImgui_Tab_ModelAnim::Render()
//{
//	if (CharacterList.empty())
//	{
//		for (auto iter : *(m_pGameInstance->Get_ObjectPrototypeList()))
//		{
//			CCharacter* pCharacter = nullptr;
//			pCharacter = dynamic_cast<CCharacter*> (iter.second);
//			if (pCharacter != nullptr)
//			{
//				//CGameObject::GAMEOBJECT_DESC Desc();
//				if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_TOOL, L_PLAYER, iter.first, nullptr, reinterpret_cast<CGameObject**>(&pCharacter))))
//					RETURN_EFAIL;
//				pCharacter->TurnOff_State(OBJSTATE::Active);
//				CharacterList.push_back(pCharacter);
//			}
//		}
//	}
//
//
//	ImVec2 vSize = ImVec2(180, 100);
//	ImGui::BeginListBox("Character_List", vSize);
//	for (int i = 0; i < CharacterList.size(); i++)
//	{
//		CCharacter* pGameObject = CharacterList[i]; 
//		vector<wstring> PrtTag = CImgui_Manager::GetInstance()->Split_Wstring(pGameObject->Get_PrototypeTag(), L'_');
//		wstring wstrTag = L"[" + PrtTag[PrtTag.size() - 1] + L"] ";
//
//		string  str;
//		str = WstrToStr(wstrTag);
//		if (ImGui::Selectable(str.c_str(), m_iPickIndex == i))
//		{
//			m_iPickIndex = i;
//			m_pPickCharacter = pGameObject;
//			m_pPickCharacter->TurnOn_State(OBJSTATE::Active);
//			CPartObject* pPartObject = dynamic_cast<CPartObject*>(m_pPickCharacter->Find_PartObject(TEXT("Part_Body")));
//			m_pModelCom = pPartObject->Get_ModelCom();
//			m_pModelCom->Set_Animation(0, m_bAnimLoop, m_fAnimSpeed, 0.2f, true);
//			m_AnimationsName = m_pModelCom->Get_AnimationsName();
//			m_strCurAnimname = m_pModelCom->Get_CurrentAnimation()->Get_Name();
//		}
//		else
//			pGameObject->TurnOff_State(OBJSTATE::Active);
//	}
//	ImGui::EndListBox();
//	CharacterList[m_iPickIndex]->TurnOn_State(OBJSTATE::Active);
//	m_pTrailTab->Set_EnableCharacter(CharacterList[m_iPickIndex]);
//
//	if (m_pModelCom != nullptr)
//	{
//		ImGui::BeginListBox("Animation_List", vSize);
//		_int i = 0;
//		for (auto iter = m_AnimationsName->begin(); iter != m_AnimationsName->end(); iter++)
//		{
//			string  str;
//			str = iter->first;
//			if (ImGui::Selectable(str.c_str(), m_strCurAnimname == str))
//			{				
//				m_pModelCom->Set_Animation(iter->first, m_bAnimLoop,m_fAnimSpeed,0.2f,true);
//				m_strCurAnimname = iter->first;
//			}
//		}
//		ImGui::EndListBox();
//
//
//		if (ImGui::Button("Reset_Position"))
//		{
//			CharacterList[m_iPickIndex]->Get_TransformCom().lock()->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
//		}
//		ImGui::Checkbox("IsMove", &m_bCharacterMove);
//		CharacterList[m_iPickIndex]->Get_TransformCom().lock()->Set_Move(m_bCharacterMove);
//	
//		ImGui::Checkbox("AnimLoop", &m_bAnimLoop);
//		ImGui::SameLine();
//		if (ImGui::Button("Animation Play"))
//		{
//			m_pModelCom->Set_Animation(m_strCurAnimname, m_bAnimLoop, m_fAnimSpeed, 0.2f, true);
//		}
//	}
//	
//
//	ImGui::Separator();	
//	if (m_pModelCom != nullptr && m_pModelCom->Get_ModelType() == ANIM)
//	{
//		ImGui::Text("Animation_Count: %d ", m_pModelCom->Get_NumAnimations());
//		ImGui::Text("Animation_Index: %d ", m_pModelCom->Get_CurrentAnimIndex());
//		ImGui::Text("Animation_Name: ");
//		ImGui::SameLine();
//		ImGui::Text(m_pModelCom->Get_CurrentAnimation()->Get_Name());
//		m_iCurFrameIndex = m_pModelCom->Get_CurrentAnimation()->Get_TrackFrameIndex();
//		ImGui::Text("Animation_TrackFrame_Index: %d ",m_iCurFrameIndex );
//	}
//	ImGui::Checkbox("PlayOnFrame", &m_bPlayOnFrame);
//	if(m_bPlayOnFrame)
//	{
//		m_pImgui_Manger->Arrow_Button("Play_Particle_Frame", 1, &m_iPlayParticleFrameIndex);
//		m_pImgui_Manger->Arrow_Button("Play_Trail_Frame", 1, &m_iPlayTrailFrameIndex);
//
//		if (m_iPlayParticleFrameIndex == m_iCurFrameIndex && !m_bPlayOnce[0])
//		{
//			if (m_pParticleTab->m_pParticle != nullptr)
//			{
//				static_cast<CVIBuffer_Instancing*>(m_pParticleTab->m_pParticle->Get_VIBufferCom())->Reset_AllParticles();
//				m_pParticleTab->m_pParticle->Set_Play(true);
//			}			
//			m_bPlayOnce[0] = true;
//		}
//		else
//			m_bPlayOnce[0] = false;
//
//
//		if (m_iPlayTrailFrameIndex == m_iCurFrameIndex && !m_bPlayOnce[1])
//		{
//			if (m_pTrailTab->m_pTrailEffect != nullptr)
//			{
//				m_pTrailTab->m_pTrailEffect->Reset();
//				m_pTrailTab->m_pTrailEffect->Set_Play(true);
//			}
//			m_bPlayOnce[1] = true;
//		}
//		else
//			m_bPlayOnce[1] = false;
//	}
//
//
//    return S_OK;
//}
//
//CImgui_Tab_ModelAnim* CImgui_Tab_ModelAnim::Create(vector<CGameObject*>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, CImgui_Tab_ParticleEdit* pParticleTab, CImgui_Tab_TrailEdit* pTrailTab)
//{
//	CImgui_Tab_ModelAnim* pInstance = new CImgui_Tab_ModelAnim(pGameObjectList, pDevice, pContext);
//
//	if (FAILED(pInstance->Initialize(pParticleTab, pTrailTab)))
//	{
//		MSG_BOX("Failed to Created : CImgui_Tab_ModelAnim");
//		Safe_Release(pInstance);
//	}
//	return pInstance;
//}
//
//void CImgui_Tab_ModelAnim::Free()
//{
//}
