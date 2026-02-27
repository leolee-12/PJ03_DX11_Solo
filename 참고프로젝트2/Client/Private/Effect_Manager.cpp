#pragma once
#include "stdafx.h"
#include "Effect_Manager.h"
#include "Imgui_Manager.h"
#include "GameInstance.h"
#include "Effect.h"
#include <string>
#include <locale>
#include <codecvt>
#include "IMGUI_Shader_Tab.h"
#include "Effect_Nonelight.h"
#include "Effect_Blend.h"
#include "Effect_ZNone.h"
#include "Effect_Overlap.h"

IMPLEMENT_SINGLETON(CEffect_Manager)

CEffect_Manager::CEffect_Manager()
	:m_pGameInstance{CGameInstance::Get_Instance()}
{
	Safe_AddRef(m_pGameInstance); 
}

HRESULT CEffect_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

void CEffect_Manager::Camera_Update(_float fTimeDelta)
{
	//for (auto& Pair : m_FinalEffects)
	//	Pair.second->Camera_Update(fTimeDelta);

	//for (auto& Pair : m_TestEffect)
	//	Pair->Camera_Update(fTimeDelta);

	for (auto& Pair : m_UsingEffect)
		Pair->Camera_Update(fTimeDelta);

}

void CEffect_Manager::Update(_float fTimeDelta)
{
	//CEffect_Layer* pLayer = Find_Effect_Layer(m_strSelectLayerName);

	//if (pLayer != nullptr)
	//	pLayer->Update(fTimeDelta);
	

	for (auto& Pair : m_TestEffect)
		Pair->Update(fTimeDelta);

	for (auto iter = m_UsingEffect.begin(); iter != m_UsingEffect.end(); )
	{
		if ((*iter)->m_bIsDoneAnim)
		{
			
			(*iter)->Free();
			iter = m_UsingEffect.erase(iter);
		}
		else
		{
			(*iter)->Update(fTimeDelta);
			++iter;
		}
	}
}

void CEffect_Manager::Late_Update(_float fTimeDelta)
{
	CEffect_Layer* pLayer = Find_Effect_Layer(m_strSelectLayerName);

	if (pLayer != nullptr)
		pLayer->Late_Update(fTimeDelta);
	

	for (auto& Pair : m_TestEffect)
		Pair->Late_Update(fTimeDelta);

	for (auto& Pair : m_UsingEffect)
		if (Pair->m_bIsCopy)
		{
			Pair->Late_Update(fTimeDelta);
		}

}

void CEffect_Manager::Render(_float fTimeDelta)
{
	//for (auto& Pair : m_FinalEffects)
	//	Pair.second->Render(fTimeDelta);
		
	//for (auto& Pair : m_TestEffect)
	//	Pair->Render(fTimeDelta);

	//for (auto& Pair : m_UsingEffect)
	//	Pair->Render(fTimeDelta);
}

HRESULT CEffect_Manager::Copy_Layer(const wstring& strEffectLayerTag, CEffect_Layer::COPY_DESC* pArg)
{
	CEffect_Layer* pLayer = Find_Effect_Layer(strEffectLayerTag);

	if (pLayer == nullptr)
		return E_FAIL;

	if (strEffectLayerTag.find(L"Smoke") != wstring::npos)
		m_UsingEffect.push_back(pLayer->Clone(pArg, false));
	else
	{
		if (strEffectLayerTag.find(L"Aura01") != wstring::npos)
			m_UsingEffect.push_back(pLayer->Clone(pArg, false));
		else
		{
			m_UsingEffect.push_back(pLayer->Clone(pArg, true));
		}
	}

	return S_OK;
}


CEffect_Layer* CEffect_Manager::Copy_Layer_AndGet(const wstring& strEffectLayerTag, CEffect_Layer::COPY_DESC* pArg)
{
	CEffect_Layer* pLayer = Find_Effect_Layer(strEffectLayerTag);

	if (pLayer == nullptr)
		return nullptr;

	if (strEffectLayerTag.find(L"Smoke") != wstring::npos)
		m_UsingEffect.push_back(pLayer->Clone(pArg, false));
	else
	{
		if (strEffectLayerTag.find(L"Aura01") != wstring::npos)
			m_UsingEffect.push_back(pLayer->Clone(pArg, false));
		else
		{

			m_UsingEffect.push_back(pLayer->Clone(pArg, true));
		}
	}

	return m_UsingEffect.back();
}

CEffect_Layer* CEffect_Manager::Copy_Layer_OverTheHandle(const wstring& strEffectLayerTag, CEffect_Layer::COPY_DESC* pArg)
{
	CEffect_Layer* pLayer = Find_Effect_Layer(strEffectLayerTag);

	if (pLayer == nullptr)
		return nullptr;

	return pLayer->Clone(pArg, true);
}

HRESULT CEffect_Manager::Set_Saved_Effects(vector<EFFECT_LAYER_DATA>* pSavedEffect)
{
	if (!pSavedEffect) 
		return E_FAIL;

	for (const auto& layerData : *pSavedEffect)
	{
		CEffect_Layer::LAYER_DESC pLayerDesc;
		pLayerDesc.vPosition = layerData.vPosition;
		pLayerDesc.vScaled = layerData.vScaled;
		pLayerDesc.vRotation = layerData.vRotation;

		CEffect_Layer* pLayer = CEffect_Layer::Create(m_pDevice, m_pContext, &pLayerDesc);
		if (!pLayer) continue; 

		pLayer->m_fDuration = layerData.duration;
		pLayer->m_fTickPerSecond = layerData.tickPerSecond;
		pLayer->m_iNumKeyFrames = layerData.keyFramesCount;
		pLayer->m_bIsFollowing = layerData.bIsFollowing;

		for (const auto& effectData : layerData.effects)
		{
			CEffect::EFFECT_DESC EffectDesc;
			EffectDesc.ModelName = effectData.modelName;
			EffectDesc.EffectName = effectData.effectName;
			EffectDesc.MaskTextureName = effectData.maskTextureName;
			EffectDesc.DiffuseTextureName = effectData.diffuseTextureName;
			EffectDesc.EffectType = static_cast<EFFECT_TYPE>(effectData.effectType);
			EffectDesc.vPosition = effectData.position;
			EffectDesc.vScaled = effectData.scale;
			EffectDesc.vRotation = effectData.rotation;
			EffectDesc.iUnique_Index = m_TestEffect_Count++;
			EffectDesc.iRenderIndex = effectData.renderIndex;
			EffectDesc.bIsBillboarding = effectData.isLoop;
			EffectDesc.iPassIndex = effectData.passIndex;
			EffectDesc.vColor = effectData.vColor;
			EffectDesc.vGlowColor = effectData.vGlowColor;
			EffectDesc.fGlowFactor = effectData.fGlowFactor;
			EffectDesc.DerredPassIndex = effectData.iDerredPassIndex;
			EffectDesc.SRV_Ptr = nullptr;  // SRV는 nullptr로 초기화; 필요한 경우 적절히 설정
			EffectDesc.bIsCopy = false;
			
			EffectDesc.LayerMatrix = pLayer->m_pTransformCom->Get_WorldMatrix();

			EffectDesc.isGlow = effectData.uniqueIndex;

			CEffect_NoneLight* pNonelight = { nullptr };
			CEffect_Blend* pBlend = { nullptr };
			CEffect_ZNone* pZNone = { nullptr };
			CEffect_Overlap* pOverlap = { nullptr };

			wstring_convert<codecvt_utf8<wchar_t>, wchar_t> converter;
			string sMaskTextureName = converter.to_bytes(EffectDesc.MaskTextureName);

			switch (EffectDesc.EffectType)
			{
			case EFFECT_NONELIGHT:
				pNonelight = static_cast<CEffect_NoneLight*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_NoneLight"), &EffectDesc));

				if (!pNonelight) 
				{
					Safe_Release(pLayer);
					return E_FAIL;
				}

				CImgui_Manager::Get_Instance()->Load_Shader_Tab(static_cast<CTexture*>(pNonelight->Get_Component(TEXT("Com_DiffuseTexture"))), sMaskTextureName, EffectDesc.iUnique_Index,static_cast<CEffect*>(pNonelight));
				
				pNonelight->m_bIsNotPlaying = effectData.isNotPlaying;
				//pNonelight->m_bIsLoop = effectData.isLoop;

				for (const auto& keyFrameData : effectData.keyframes)
				{
					EFFECT_KEYFRAME keyFrame;
					keyFrame.vPosition = keyFrameData.position;
					keyFrame.vScale = keyFrameData.scale;
					keyFrame.vRotation = keyFrameData.rotation;
					keyFrame.fCurTime = keyFrameData.curTime;
					keyFrame.fDuration = keyFrameData.duration;
					keyFrame.bIsNotPlaying = keyFrameData.bIsNotPlaying;

					pNonelight->Add_KeyFrame(keyFrameData.keyFrameNumber, keyFrame);
				}

				if (FAILED(pLayer->Add_Effect(pNonelight))) 
				{
					Safe_Release(pNonelight);
					Safe_Release(pLayer);
					return E_FAIL;
				}
				break;
			case EFFECT_BLEND:
				pBlend = static_cast<CEffect_Blend*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Blend"), &EffectDesc));
				
				if (!pBlend)
				{
					Safe_Release(pLayer);
					return E_FAIL;
				}

				CImgui_Manager::Get_Instance()->Load_Shader_Tab(static_cast<CTexture*>(pBlend->Get_Component(TEXT("Com_DiffuseTexture"))), sMaskTextureName, EffectDesc.iUnique_Index, static_cast<CEffect*>(pBlend));

				pBlend->m_bIsNotPlaying = effectData.isNotPlaying;
				//pBlend->m_bIsLoop = effectData.isLoop;

				for (const auto& keyFrameData : effectData.keyframes)
				{
					EFFECT_KEYFRAME keyFrame;
					keyFrame.vPosition = keyFrameData.position;
					keyFrame.vScale = keyFrameData.scale;
					keyFrame.vRotation = keyFrameData.rotation;
					keyFrame.fCurTime = keyFrameData.curTime;
					keyFrame.fDuration = keyFrameData.duration;
					keyFrame.bIsNotPlaying = keyFrameData.bIsNotPlaying;

					pBlend->Add_KeyFrame(keyFrameData.keyFrameNumber, keyFrame);
				}

				if (FAILED(pLayer->Add_Effect(pBlend)))
				{
					Safe_Release(pBlend);
					Safe_Release(pLayer);
					return E_FAIL;
				}
				break;
			case EFFECT_ZNONE:
				pZNone = static_cast<CEffect_ZNone*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_ZNone"), &EffectDesc));

				if (!pZNone)
				{
					Safe_Release(pLayer);
					return E_FAIL;
				}

				CImgui_Manager::Get_Instance()->Load_Shader_Tab(static_cast<CTexture*>(pZNone->Get_Component(TEXT("Com_DiffuseTexture"))), sMaskTextureName, EffectDesc.iUnique_Index, static_cast<CEffect*>(pNonelight));

				pZNone->m_bIsNotPlaying = effectData.isNotPlaying;
				//pZNone->m_bIsLoop = effectData.isLoop;

				for (const auto& keyFrameData : effectData.keyframes)
				{
					EFFECT_KEYFRAME keyFrame;
					keyFrame.vPosition = keyFrameData.position;
					keyFrame.vScale = keyFrameData.scale;
					keyFrame.vRotation = keyFrameData.rotation;
					keyFrame.fCurTime = keyFrameData.curTime;
					keyFrame.fDuration = keyFrameData.duration;
					keyFrame.bIsNotPlaying = keyFrameData.bIsNotPlaying;

					pZNone->Add_KeyFrame(keyFrameData.keyFrameNumber, keyFrame);
				}

				if (FAILED(pLayer->Add_Effect(pZNone)))
				{
					Safe_Release(pZNone);
					Safe_Release(pLayer);
					return E_FAIL;
				}
				break;
			case EFFECT_OVERLAP:
				pOverlap = static_cast<CEffect_Overlap*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Overlap"), &EffectDesc));

				if (!pOverlap)
				{
					Safe_Release(pLayer);
					return E_FAIL;
				}

				CImgui_Manager::Get_Instance()->Load_Shader_Tab(static_cast<CTexture*>(pOverlap->Get_Component(TEXT("Com_DiffuseTexture"))), sMaskTextureName, EffectDesc.iUnique_Index, static_cast<CEffect*>(pOverlap));

				pOverlap->m_bIsNotPlaying = effectData.isNotPlaying;
				//pOverlap->m_bIsLoop = effectData.isLoop;

				for (const auto& keyFrameData : effectData.keyframes)
				{
					EFFECT_KEYFRAME keyFrame;
					keyFrame.vPosition = keyFrameData.position;
					keyFrame.vScale = keyFrameData.scale;
					keyFrame.vRotation = keyFrameData.rotation;
					keyFrame.fCurTime = keyFrameData.curTime;
					keyFrame.fDuration = keyFrameData.duration;
					keyFrame.bIsNotPlaying = keyFrameData.bIsNotPlaying;

					pOverlap->Add_KeyFrame(keyFrameData.keyFrameNumber, keyFrame);
				}

				if (FAILED(pLayer->Add_Effect(pOverlap)))
				{
					Safe_Release(pOverlap);
					Safe_Release(pLayer);
					return E_FAIL;
				}
				break;
			}

		}

		m_FinalEffects.emplace(layerData.layerName, pLayer);
	}

	return S_OK;
}

CEffect_Layer* CEffect_Manager::Find_Effect_Layer(const wstring& strEffectLayerTag)
{
	auto	iter = m_FinalEffects.find(strEffectLayerTag);

	if (iter == m_FinalEffects.end())
		return nullptr;

	return iter->second;
}

CEffect* CEffect_Manager::Find_In_Layer_Effect(wstring& layerName, wstring& effectName)
{
	CEffect_Layer* pLayer = Find_Effect_Layer(layerName);

	if (pLayer == nullptr)
		return nullptr;

	return pLayer->Find_Effect(effectName);
}

HRESULT CEffect_Manager::Set_In_Layer_Effect_Layer_Transform(wstring& layerName)
{
	CEffect_Layer* pLayer = Find_Effect_Layer(layerName);

	if (pLayer == nullptr)
		return E_FAIL;

	pLayer->Set_In_Layer_Effect();

	return S_OK;
}

_bool CEffect_Manager::Find_KeyFrame(wstring& layerName, wstring& effectName, _uint frameNumber)
{
	CEffect_Layer* pLayer = Find_Effect_Layer(layerName);
	if (!pLayer)
		return false;

	CEffect* pEffect = pLayer->Find_Effect(effectName);
	if (!pEffect)
		return false;

	return pEffect->Find_KeyFrame(frameNumber);
}

void CEffect_Manager::Set_Render_Layer(const wstring& strEffectLayerTag)
{
	for (auto& Pair : m_FinalEffects)
		Pair.second->m_bIsRender = { false };

	Find_Effect_Layer(strEffectLayerTag)->m_bIsRender = { true };
}

HRESULT CEffect_Manager::Set_Test_Effect_Color(_int iCurTestEffectIndex, _float4 vColor)
{
	for (auto& iter : m_TestEffect)
	{
		if (iter->m_iUnique_Index == iCurTestEffectIndex)
		{
			iter->Set_Effect_Color(vColor);
		}
	}

	return S_OK;
}

HRESULT CEffect_Manager::Set_Layer_Effect_Color(wstring& layerName, wstring& effectName, _float4 vColor)
{
	CEffect* pEffect = Find_In_Layer_Effect(layerName, effectName);

	if (pEffect == nullptr)
		return E_FAIL;

	pEffect->Set_Effect_Color(vColor);
	
	return S_OK;
}

EFFECT_KEYFRAME CEffect_Manager::Get_KeyFrame(wstring& layerName, wstring& effectName, _uint frameNumber)
{
	CEffect_Layer* pLayer = Find_Effect_Layer(layerName);

	CEffect* pEffect = pLayer->Find_Effect(effectName);

	return pEffect->Get_KeyFrame(frameNumber);
}

EFFECT_KEYFRAME CEffect_Manager::Get_Near_Front_KeyFrame(wstring& layerName, wstring& effectName, _uint frameNumber)
{
	CEffect_Layer* pLayer = Find_Effect_Layer(layerName);

	CEffect* pEffect = pLayer->Find_Effect(effectName);

	return pEffect->Get_Near_Front_KeyFrame(frameNumber);
}

HRESULT CEffect_Manager::Add_Effect_To_Layer(_int iCurTestEffectIndex, const wstring& strEffectLayerTag, void* pArg)
{
	CEffect_Layer* pLayer = Find_Effect_Layer(strEffectLayerTag);

	if (nullptr == pLayer)
	{
		CEffect_Layer* pLayer = CEffect_Layer::Create(m_pDevice, m_pContext, pArg);

		CEffect::EFFECT_DESC EffectDesc {};
		for (auto& iter : m_TestEffect)
		{
			if (iter->m_iUnique_Index == iCurTestEffectIndex)
			{
				EffectDesc.ModelName =iter->m_ModelName;
				EffectDesc.EffectName =iter->m_EffectName;
				EffectDesc.MaskTextureName =iter->m_MaskTextureName;
				EffectDesc.DiffuseTextureName =iter->m_DiffuseTextureName;
				EffectDesc.vPosition =iter->Get_Effect_Position();
				EffectDesc.vScaled =iter->Get_Effect_Scaled();
				EffectDesc.vRotation =iter->Get_Effect_Rotation();
				EffectDesc.iUnique_Index =iter->m_iUnique_Index;
				EffectDesc.SRV_Ptr = static_cast<CTexture*>(iter->Get_Component(TEXT("Com_DiffuseTexture")))->Get_SRV(0);
				EffectDesc.iRenderIndex = 2;
				EffectDesc.isGlow = iter->m_isGlow;
				EffectDesc.fGlowFactor =iter->m_fGlowFactor;
				EffectDesc.vColor = iter->m_vColor;
				EffectDesc.LayerMatrix = pLayer->m_pTransformCom->Get_WorldMatrix();

				CEffect* pClone = static_cast<CEffect*>(iter->Clone(&EffectDesc));

				CImgui_Manager::Get_Instance()->Access_Shader_Tab(EffectDesc.iUnique_Index)->Update_TestToLayer_TextureCom(static_cast<CTexture*>(pClone->Get_Component(TEXT("Com_DiffuseTexture"))), pClone);

				pLayer->Add_Effect(pClone);

				m_FinalEffects.emplace(strEffectLayerTag, pLayer);

				EFFECT_KEYFRAME FirstKeyFrame{};

				FirstKeyFrame.vPosition = EffectDesc.vPosition;
				FirstKeyFrame.vRotation = EffectDesc.vRotation;
				FirstKeyFrame.vScale = EffectDesc.vScaled;

				pLayer->Find_Effect(EffectDesc.EffectName)->Add_KeyFrame(0, FirstKeyFrame);

				_int iCount = 0;
				for (auto& iter : m_TestEffect)
				{
					if (iter->m_iUnique_Index == iCurTestEffectIndex)
					{
						Safe_Release(iter);
						m_TestEffect.erase(m_TestEffect.begin() + iCount);
						break;
					}
					iCount++;
				}

			}
		}
	}
	else
	{
		CEffect::EFFECT_DESC EffectDesc{};
		for (auto& iter : m_TestEffect)
		{
			if (iter->m_iUnique_Index == iCurTestEffectIndex)
			{
				EffectDesc.ModelName =iter->m_ModelName;
				EffectDesc.EffectName =iter->m_EffectName;
				EffectDesc.MaskTextureName =iter->m_MaskTextureName;
				EffectDesc.DiffuseTextureName =iter->m_DiffuseTextureName;
				EffectDesc.vPosition =iter->Get_Effect_Position();
				EffectDesc.vScaled =iter->Get_Effect_Scaled();
				EffectDesc.vRotation =iter->Get_Effect_Rotation();
				EffectDesc.iUnique_Index =iter->m_iUnique_Index;
				EffectDesc.SRV_Ptr = static_cast<CTexture*>(iter->Get_Component(TEXT("Com_DiffuseTexture")))->Get_SRV(0);
				EffectDesc.iRenderIndex = 2;
				EffectDesc.isGlow = iter->m_isGlow;
				EffectDesc.fGlowFactor = iter->m_fGlowFactor;
				EffectDesc.vColor = iter->m_vColor;
				EffectDesc.LayerMatrix = pLayer->m_pTransformCom->Get_WorldMatrix();
				CEffect* pClone = static_cast<CEffect*>(iter->Clone(&EffectDesc));
				
				CImgui_Manager::Get_Instance()->Access_Shader_Tab(EffectDesc.iUnique_Index)->Update_TestToLayer_TextureCom(static_cast<CTexture*>(pClone->Get_Component(TEXT("Com_DiffuseTexture"))), pClone);
					
				pLayer->Add_Effect(pClone);

				EFFECT_KEYFRAME FirstKeyFrame{};

				FirstKeyFrame.vPosition = EffectDesc.vPosition;
				FirstKeyFrame.vRotation = EffectDesc.vRotation;
				FirstKeyFrame.vScale = EffectDesc.vScaled;

				pLayer->Find_Effect(EffectDesc.EffectName)->Add_KeyFrame(0, FirstKeyFrame);

				_int iCount = 0;
				for (auto& iter : m_TestEffect)
				{
					if (iter->m_iUnique_Index == iCurTestEffectIndex)
					{
						Safe_Release(iter);
						m_TestEffect.erase(m_TestEffect.begin() + iCount);
						break;
					}
					iCount++;
				}
			}
		}
	
	}

	return S_OK;
}

HRESULT CEffect_Manager::Add_All_Effect_To_Layer(const wstring& strEffectLayerTag, void* pArg)
{
	CEffect_Layer* pLayer = Find_Effect_Layer(strEffectLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CEffect_Layer::Create(m_pDevice, m_pContext, pArg);
		m_FinalEffects.emplace(strEffectLayerTag, pLayer);
	}

	for (auto& pEffect : m_TestEffect)
	{
		if (pEffect != nullptr)
		{
			CEffect::EFFECT_DESC EffectDesc{};
			EffectDesc.ModelName = pEffect->m_ModelName;
			EffectDesc.EffectName = pEffect->m_EffectName;
			EffectDesc.MaskTextureName = pEffect->m_MaskTextureName;
			EffectDesc.DiffuseTextureName = pEffect->m_DiffuseTextureName;
			EffectDesc.vPosition = pEffect->Get_Effect_Position();
			EffectDesc.vScaled = pEffect->Get_Effect_Scaled();
			EffectDesc.vRotation = pEffect->Get_Effect_Rotation();
			EffectDesc.iUnique_Index = pEffect->m_iUnique_Index;
			EffectDesc.SRV_Ptr = static_cast<CTexture*>(pEffect->Get_Component(TEXT("Com_DiffuseTexture")))->Get_SRV(0);
			EffectDesc.iRenderIndex = 2;
			EffectDesc.isGlow = pEffect->m_isGlow;
			EffectDesc.fGlowFactor = pEffect->m_fGlowFactor;
			EffectDesc.LayerMatrix = pLayer->m_pTransformCom->Get_WorldMatrix();
			EffectDesc.vColor = pEffect->m_vColor;

			CEffect* pClone = static_cast<CEffect*>(pEffect->Clone(&EffectDesc));

			CImgui_Manager::Get_Instance()->Access_Shader_Tab(EffectDesc.iUnique_Index)->Update_TestToLayer_TextureCom(static_cast<CTexture*>(pClone->Get_Component(TEXT("Com_DiffuseTexture"))), pClone);

			pLayer->Add_Effect(pClone);

			EFFECT_KEYFRAME FirstKeyFrame{};

			FirstKeyFrame.vPosition = EffectDesc.vPosition;
			FirstKeyFrame.vRotation = EffectDesc.vRotation;
			FirstKeyFrame.vScale = EffectDesc.vScaled;

			pLayer->Find_Effect(EffectDesc.EffectName)->Add_KeyFrame(0, FirstKeyFrame);
		}
	}

	for (auto& pEffect : m_TestEffect)
	{
		Safe_Release(pEffect);
	}
	m_TestEffect.clear();

	return S_OK;
}

HRESULT CEffect_Manager::Delete_Layer(const wstring& strEffectLayerTag)
{
	CEffect_Layer* pLayer = Find_Effect_Layer(strEffectLayerTag);

	if (pLayer == nullptr)
	{
		return E_FAIL;
	}

	Safe_Release(pLayer);

	m_FinalEffects.erase(strEffectLayerTag);

	return S_OK;
}

vector<wstring> CEffect_Manager::Get_Layer_List()
{
	vector<wstring> LayerList;

	// m_FinalEffects의 키 값만 벡터에 추가
	for (const auto& pair : m_FinalEffects)
	{
		LayerList.push_back(pair.first);
	}

	return LayerList;
}

vector<wstring> CEffect_Manager::Get_In_Layer_Effect_List(wstring* LayerName)
{
	vector<wstring> EffectNames;

	auto it = m_FinalEffects.find(*LayerName);
	if (it != m_FinalEffects.end())
	{
		CEffect_Layer* pLayer = it->second;
		const auto& effects = pLayer->Get_Effects();

		for (const auto& effect : effects)
		{
			if (effect)
			{
				EffectNames.push_back(effect->m_EffectName);
			}
		}
	}

	return EffectNames;
}

HRESULT CEffect_Manager::Add_Test_Effect(EFFECT_TYPE eEffectType, wstring* EffectName, wstring* ModelName)
{
	CEffect::EFFECT_DESC EffectDesc{};

	EffectDesc.ModelName = *ModelName;
	EffectDesc.EffectName = *EffectName;
	EffectDesc.MaskTextureName = TEXT("Texture_Effect_Default_Mask");
	EffectDesc.DiffuseTextureName = TEXT("Texture_Effect_cmn_test");
	EffectDesc.iUnique_Index = m_TestEffect_Count++;
	EffectDesc.vPosition = { 0.f, 0.f, 0.f };
	EffectDesc.vScaled = { 1.f, 1.f, 1.f };
	EffectDesc.vRotation = { 0.f, 0.f, 0.f };
	EffectDesc.iRenderIndex = 1;
	EffectDesc.vColor = { 0.f, 0.f, 0.f, 30.f };
	EffectDesc.isGlow = -1;
	EffectDesc.fGlowFactor = 0;
	EffectDesc.LayerMatrix = XMMatrixIdentity();

	CGameObject* pEffect = nullptr;
	CEffect* pTestEffect = nullptr;

	switch (eEffectType)
	{
	case EFFECT_NONELIGHT:
		pEffect = m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_NoneLight"), &EffectDesc);
		pTestEffect = static_cast<CEffect*>(pEffect);

		if (pTestEffect == nullptr)
			return E_FAIL;

		CImgui_Manager::Get_Instance()->Push_Shader_Tab(static_cast<CTexture*>(pTestEffect->Get_Component(TEXT("Com_DiffuseTexture"))),pTestEffect);

		m_TestEffect.push_back(pTestEffect);
		break;
	case EFFECT_BLEND:
		pEffect = m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Blend"), &EffectDesc);
		pTestEffect = static_cast<CEffect*>(pEffect);

		if (pTestEffect == nullptr)
			return E_FAIL;

		CImgui_Manager::Get_Instance()->Push_Shader_Tab(static_cast<CTexture*>(pTestEffect->Get_Component(TEXT("Com_DiffuseTexture"))), pTestEffect);

		m_TestEffect.push_back(pTestEffect);
		break;
	case EFFECT_ZNONE:
		pEffect = m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_ZNone"), &EffectDesc);
		pTestEffect = static_cast<CEffect*>(pEffect);

		if (pTestEffect == nullptr)
			return E_FAIL;

		CImgui_Manager::Get_Instance()->Push_Shader_Tab(static_cast<CTexture*>(pTestEffect->Get_Component(TEXT("Com_DiffuseTexture"))), pTestEffect);

		m_TestEffect.push_back(pTestEffect);
		break;
	case EFFECT_OVERLAP:
		pEffect = m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Overlap"), &EffectDesc);
		pTestEffect = static_cast<CEffect*>(pEffect);

		if (pTestEffect == nullptr)
			return E_FAIL;

		CImgui_Manager::Get_Instance()->Push_Shader_Tab(static_cast<CTexture*>(pTestEffect->Get_Component(TEXT("Com_DiffuseTexture"))), pTestEffect);

		m_TestEffect.push_back(pTestEffect);
		break;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Delete_Test_Effect(_uint iCurTestEffectID)
{
	for (auto it = m_TestEffect.begin(); it != m_TestEffect.end(); ++it)
	{
		CEffect* pEffect = *it;

		if (pEffect && pEffect->m_iUnique_Index == iCurTestEffectID)
		{
			Safe_Release(pEffect);
			m_TestEffect.erase(it);
			return S_OK; 
		}
	}

	return E_FAIL;
}

vector<_int> CEffect_Manager::Delete_All_Test_Effect()
{
	vector<_int> UniqueIDs;

	for (auto& pEffect : m_TestEffect)
	{
		if (pEffect)
		{
			UniqueIDs.push_back(pEffect->m_iUnique_Index);
			Safe_Release(pEffect);
		}
	}

	m_TestEffect.clear();

	return UniqueIDs;
}

HRESULT CEffect_Manager::Set_Effect_Scaled(_int EffectId, _float3 ChangeScaled)
{
	for (auto it = m_TestEffect.begin(); it != m_TestEffect.end(); ++it)
	{
		CEffect* pEffect = *it;

		if (pEffect && pEffect->m_iUnique_Index == EffectId)
		{
			pEffect->Set_Effect_Scaled(ChangeScaled);

			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CEffect_Manager::Set_Effect_Position(_int EffectId, _float3 ChangePosition)
{
	for (auto it = m_TestEffect.begin(); it != m_TestEffect.end(); ++it)
	{
		CEffect* pEffect = *it;

		if (pEffect && pEffect->m_iUnique_Index == EffectId)
		{
			pEffect->Set_Effect_Position(ChangePosition);

			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CEffect_Manager::Set_Effect_Rotation(_int EffectId, _float3 ChangeRotation)
{
	for (auto it = m_TestEffect.begin(); it != m_TestEffect.end(); ++it)
	{
		CEffect* pEffect = *it;

		if (pEffect && pEffect->m_iUnique_Index == EffectId)
		{
			pEffect->Set_Effect_Rotation(ChangeRotation);

			return S_OK;
		}
	}

	return E_FAIL;
}

_float3 CEffect_Manager::Get_Effect_Scaled(_int EffectId)
{
	for (auto it = m_TestEffect.begin(); it != m_TestEffect.end(); ++it)
	{
		CEffect* pEffect = *it;

		if (pEffect && pEffect->m_iUnique_Index == EffectId)
		{
			return pEffect->Get_Effect_Scaled();
			
		}
	}

	return _float3(0.f, 0.f, 0.f);
}

_float3 CEffect_Manager::Get_Effect_Position(_int EffectId)
{
	for (auto it = m_TestEffect.begin(); it != m_TestEffect.end(); ++it)
	{
		CEffect* pEffect = *it;

		if (pEffect && pEffect->m_iUnique_Index == EffectId)
		{
			return pEffect->Get_Effect_Position();

		}
	}

	return _float3(0.f, 0.f, 0.f);
}

_float3 CEffect_Manager::Get_Effect_Rotation(_int EffectId)
{
	for (auto it = m_TestEffect.begin(); it != m_TestEffect.end(); ++it)
	{
		CEffect* pEffect = *it;

		if (pEffect && pEffect->m_iUnique_Index == EffectId)
		{
			return pEffect->Get_Effect_Rotation();

		}
	}

	return _float3(0.f, 0.f, 0.f);
}

HRESULT CEffect_Manager::Set_Layer_Effect_Scaled(wstring& layerName, wstring& effectName, _float3 ChangeScaled)
{
	CEffect* pEffect = Find_In_Layer_Effect(layerName, effectName);

	if (pEffect) {
		pEffect->Set_Effect_Scaled(ChangeScaled);
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CEffect_Manager::Set_Layer_Effect_Position(wstring& layerName, wstring& effectName, _float3 ChangePosition)
{
	CEffect* pEffect = Find_In_Layer_Effect(layerName, effectName);

	if (pEffect) {
		pEffect->Set_Effect_Position(ChangePosition);
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CEffect_Manager::Set_Layer_Effect_Rotation(wstring& layerName, wstring& effectName, _float3 ChangeRotation)
{
	CEffect* pEffect = Find_In_Layer_Effect(layerName, effectName);

	if (pEffect) {
		pEffect->Set_Effect_Rotation(ChangeRotation);
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CEffect_Manager::Set_Layer_Effect_IsNotPlaying(wstring& layerName, wstring& effectName, _bool bIsNotPlaying)
{
	CEffect* pEffect = Find_In_Layer_Effect(layerName, effectName);

	if (pEffect) {
		pEffect->Set_Effect_IsNotPlaying(bIsNotPlaying);
		return S_OK;
	}
	return E_FAIL;
}

void CEffect_Manager::Set_ParentMatrixMultiply_LocalMatrix(wstring& layerName, wstring& effectName)
{
	Find_In_Layer_Effect(layerName, effectName)->Set_ParentMatrixMultiply_LocalMatrix();
}

_float3 CEffect_Manager::Get_Layer_Effect_Scaled(wstring& layerName, wstring& effectName)
{
	CEffect* pEffect = Find_In_Layer_Effect(layerName, effectName);

	if (pEffect) {
		return pEffect->Get_Effect_Scaled();
	}
	return _float3(0.f, 0.f, 0.f); // 기본값 반환
}

_float3 CEffect_Manager::Get_Layer_Effect_Position(wstring& layerName, wstring& effectName)
{
	CEffect* pEffect = Find_In_Layer_Effect(layerName, effectName);

	if (pEffect) {
		return pEffect->Get_Effect_Position();
	}
	return _float3(0.f, 0.f, 0.f); // 기본값 반환
}

_float3 CEffect_Manager::Get_Layer_Effect_Rotation(wstring& layerName, wstring& effectName)
{
	CEffect* pEffect = Find_In_Layer_Effect(layerName, effectName);

	if (pEffect) {
		return pEffect->Get_Effect_Rotation();
	}
	return _float3(0.f, 0.f, 0.f); // 기본값 반환
}

_bool CEffect_Manager::Get_Layer_Effect_IsPlaying(wstring& layerName, wstring& effectName)
{
	CEffect* pEffect = Find_In_Layer_Effect(layerName, effectName);

	if (pEffect) {
		return pEffect->m_bIsNotPlaying;
	}

	return false;
}

void CEffect_Manager::Delete_KeyFrame(const wstring& LayerName, const wstring& EffectName, _uint KeyFrameNumber)
{
	auto layerIt = m_FinalEffects.find(LayerName);
	if (layerIt != m_FinalEffects.end())
	{
		CEffect_Layer* pLayer = layerIt->second;
		if (pLayer)
		{
			CEffect* pEffect = pLayer->Find_Effect(EffectName);
			if (pEffect)
			{
				pEffect->Delete_KeyFrame(KeyFrameNumber);
				return;
			}
		}
	}
}

void CEffect_Manager::Add_KeyFrame(const wstring& LayerName, const wstring& EffectName, _uint KeyFrameNumber, EFFECT_KEYFRAME NewKeyFrame)
{
	auto layerIt = m_FinalEffects.find(LayerName);
	if (layerIt != m_FinalEffects.end())
	{
		CEffect_Layer* pLayer = layerIt->second;
		if (pLayer)
		{
			CEffect* pEffect = pLayer->Find_Effect(EffectName);
			if (pEffect)
			{
				pEffect->Add_KeyFrame(KeyFrameNumber, NewKeyFrame);
				return;
			}
		}
	}
}

EFFECT_KEYFRAME CEffect_Manager::Get_Layer_Effect_KeyFrame(wstring& layerName, wstring& effectName, _uint KeyFrameNumber)
{
	CEffect* pEffect = Find_In_Layer_Effect(layerName, effectName);

	if (pEffect) {
		return pEffect->Get_KeyFrame(KeyFrameNumber);
	}

	return EFFECT_KEYFRAME();
}

HRESULT CEffect_Manager::Play_Layer_Animation(_float fTimeDelta, const wstring& LayerName)
{
	CEffect_Layer* pLayer = Find_Effect_Layer(LayerName);

	if (pLayer)
	{
		pLayer->Play_Effect_Animation(fTimeDelta);
	}
	else
		return E_FAIL;
	

	return S_OK;
}

HRESULT CEffect_Manager::Set_Layer_Animation_Position(const wstring& LayerName, _float CurAnimPos)
{
	CEffect_Layer* pLayer = Find_Effect_Layer(LayerName);

	if (pLayer)
	{
		pLayer->Set_Animation_Position(CurAnimPos);
	}
	else
		return E_FAIL;


	return S_OK;
}

HRESULT CEffect_Manager::Set_Layer_Scaled(wstring& layerName, _float3 ChangeScaled)
{
	CEffect_Layer* pLayer = Find_Effect_Layer(layerName);

	if (pLayer)
	{
		pLayer->Set_Layer_Scaled(ChangeScaled);
	}
	else
		return E_FAIL;


	return S_OK;
}

HRESULT CEffect_Manager::Set_Layer_Position(wstring& layerName, _float3 ChangePosition)
{
	CEffect_Layer* pLayer = Find_Effect_Layer(layerName);

	if (pLayer)
	{
		pLayer->Set_Layer_Position(ChangePosition);
	}
	else
		return E_FAIL;


	return S_OK;
}

HRESULT CEffect_Manager::Set_Layer_Rotation(wstring& layerName, _float3 ChangeRotation)
{
	CEffect_Layer* pLayer = Find_Effect_Layer(layerName);

	if (pLayer)
	{
		pLayer->Set_Layer_Rotation(ChangeRotation);
	}
	else
		return E_FAIL;


	return S_OK;
}

_float3 CEffect_Manager::Get_Layer_Scaled(wstring& layerName)
{
	CEffect_Layer* pLayer = Find_Effect_Layer(layerName);

	if (pLayer)
	{
		return pLayer->Get_Layer_Scaled();
	}

	return _float3(0.f, 0.f, 0.f);
}

_float3 CEffect_Manager::Get_Layer_Position(wstring& layerName)
{
	CEffect_Layer* pLayer = Find_Effect_Layer(layerName);

	if (pLayer)
	{
		return pLayer->Get_Layer_Position();
	}

	return _float3(0.f, 0.f, 0.f);
}

_float3 CEffect_Manager::Get_Layer_Rotation(wstring& layerName)
{
	CEffect_Layer* pLayer = Find_Effect_Layer(layerName);

	if (pLayer)
	{
		return pLayer->Get_Layer_Rotation();
	}

	return _float3(0.f, 0.f, 0.f);
}

HRESULT CEffect_Manager::Ready_Components()
{
	vector<const _wstring*>* pModelKeys = m_pGameInstance->Find_Prototype_Include_Key(LEVEL_GAMEPLAY, TEXT("Model_Effect"));

	if (pModelKeys == nullptr)
		return E_FAIL;

	for (size_t i = 0; i < pModelKeys->size(); i++)
	{
		const wstring&	pPrototypeTag = *pModelKeys->at(i);

		CModel* pModelComponent = reinterpret_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, pPrototypeTag));

		wstring prototypeTag = pPrototypeTag;

		const wstring prefixToRemove = L"Prototype_Component_";
		size_t pos = prototypeTag.find(prefixToRemove);
		if (pos != wstring::npos) {
			prototypeTag.erase(pos, prefixToRemove.length());
		}

		m_EffectModel.emplace(prototypeTag, pModelComponent);
	}
	
	vector<const _wstring*>* pTextureKeys = m_pGameInstance->Find_Prototype_Include_Key(LEVEL_GAMEPLAY, TEXT("Texture_Effect"));

	if (pTextureKeys == nullptr)
		return E_FAIL;

	for (size_t i = 0; i < pTextureKeys->size(); i++)
	{
		const wstring& pPrototypeTag = *pTextureKeys->at(i);

		CTexture* pModelComponent = reinterpret_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, pPrototypeTag));

		wstring prototypeTag = pPrototypeTag;

		const wstring prefixToRemove = L"Prototype_Component_";
		size_t pos = prototypeTag.find(prefixToRemove);
		if (pos != wstring::npos) {
			prototypeTag.erase(pos, prefixToRemove.length());
		}

		m_EffectTexture.emplace(prototypeTag, pModelComponent);
	}

	return S_OK;
}

void CEffect_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

	for (auto& Pair : m_FinalEffects)
		Safe_Release(Pair.second);

	m_FinalEffects.clear();

	for (auto& Pair : m_EffectModel)
		Safe_Release(Pair.second);

	m_EffectModel.clear();

	for (auto& Pair : m_EffectTexture)
		Safe_Release(Pair.second);

	m_EffectTexture.clear();

	for (auto& Pair : m_TestEffect)
		Safe_Release(Pair);

	m_TestEffect.clear();

	for (auto& Pair : m_UsingEffect)
		Safe_Release(Pair);

	m_UsingEffect.clear();

}
