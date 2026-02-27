#pragma once
#include "stdafx.h"
#include "Map_Manager.h"

#include "Effect.h"
#include "GameInstance.h"
#include "RenderInstance.h"
#include "Imgui_Manager.h"
#include <string>
#include <locale>
#include <codecvt>
#include "Main_Camera.h"
#include "Volcano_Destructive.h"
#include "Volcano_SkyCloud.h"
#include "SpaceMeteoBreak.h"
#include "Effect_Manager.h"
#include "Virtual_Camera.h"
#include "SpaceEF.h"
#include "VolcanoEF.h"
#include "Sound_Manager.h"
IMPLEMENT_SINGLETON(CMap_Manager)

CMap_Manager::CMap_Manager()
	:m_pGameInstance{ CGameInstance::Get_Instance() },
	m_pRenderInstance{ CRenderInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pRenderInstance);
}

HRESULT CMap_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

void CMap_Manager::Camera_Update(_float fTimeDelta)
{
}

void CMap_Manager::Update(_float fTimeDelta)
{
#pragma region DestructiveFinish
	if (m_isEastFinishStart == true)
	{
		m_fEastAccTime += fTimeDelta;

		if (m_fEastAccTime >= 10.f)
		{
			Map_Change(m_eCurMap);
			m_pEastEffect_Layer->m_bIsDoneAnim = true;
			static_cast<CMain_Camera*>(*(m_pGameInstance->Get_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")).begin()))
				->Set_Virtual_Camera(CMain_Camera::VIRTUAL_CAMERA_NORMAL);
			m_isEastFinishStart = false;
			m_fEastAccTime = 0.f;
		}
	}


	//날아가기
	if (m_isDestructive_Active == true)
	{
		m_AccTime += fTimeDelta;
		if (m_AccTime >= 2.f)
		{
			Active_DestructiveFinish(m_isRight);
			m_isDestructive_Active = false;
			m_isDestructive_View = true;
			m_AccTime = 0.f;
		}
	}

	//날아가기
	if (m_isDestructive_Active == true)
	{
		if (m_AccTime >= 1.f)
		{
			m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::SCENE_CHANGE_START, false, 1.f);
			m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::SCENE_CHANGE_Futtobi, false, 1.f);
		}
	}

	if (m_isDestructive_View == true)
	{
		if (m_MapViewTime > 0.5f)
		{
			m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::SCENE_CHANGE_PreFight, false, 1.f);
			m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::SCENE_CHANGE_Tunagi, false, 1.f);
			m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::SCENE_CHANGE_Far_Expl, false, 1.f);
		}
	}

	//부딪히기
	if (m_isDestructive_View == true)
	{
		m_MapViewTime += fTimeDelta;
		if (m_MapViewTime > 5.f)
		{
			IsDone_Active();
			m_isDestructive_View = false;
			m_MapViewTime = 0.f;
		}
	}
#pragma endregion

	if (m_isEastFinish == true)
	{
		if (m_isWhiteDoneCheck == true)
		{
			Active_EastFinish();
			m_isWhiteDoneCheck = false;
		}
	}

	if (m_pGameInstance->Key_Pressing(DIK_F8))
		Map_Change(MAP_SPACE);
	/*if (m_pGameInstance->Key_Pressing(DIK_F7))
		Active_EastFinish(EAST_LASER);*/

	if (m_pGameInstance->Key_Pressing(DIK_F7))
		PlayerCall_EastFinish(EAST_LASER, 1.f);


}

void CMap_Manager::Late_Update(_float fTimeDelta)
{

}

void CMap_Manager::Render(_float fTimeDelta)
{

}

void CMap_Manager::Map_Change(MAP_TYPE eMapType)
{
	//m_pRenderInstance->Delete_LoopDistortion();

	switch (eMapType)
	{
	case MAP_SPACE:
	case MAP_DEST_SPACE:
		for (auto& iter : m_SpaceModels)
			iter.second->SetActive(true);
		for (auto& iter : m_Destructive_SpaceModels)
			iter.second->SetActive(true);
		for (auto& iter : m_VolcanoModels)
			iter.second->SetActive(false);
		for (auto& iter : m_Destructive_VolcanoModels)
			iter.second->SetActive(false);

		m_SpaceModels[L"Prototype_GameObject_SpaceEF"]->m_bIsActive = false;

		m_eCurMap = MAP_SPACE;
		m_pRenderInstance->Set_CurMapType(CRenderer::MAP_SPACE);

		//화산맵 음원 정지
		m_pGameInstance->Stop_Sound(CSound_Manager::SOUND_KEY_NAME::VOLCANO_BGM);
		//Space 음원 재생
		m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::SPACE_BGM, true, 1.f);
		break;
	case MAP_VOLCANO:
	case MAP_DEST_VOLCANO:
		for (auto& iter : m_VolcanoModels)
			iter.second->SetActive(true);
		for (auto& iter : m_Destructive_VolcanoModels)
			iter.second->SetActive(true);
		for (auto& iter : m_SpaceModels)
			iter.second->SetActive(false);
		for (auto& iter : m_Destructive_SpaceModels)
			iter.second->SetActive(false);

		m_VolcanoModels[L"Prototype_GameObject_VolcanoEF"]->m_bIsActive = false;

		m_eCurMap = MAP_VOLCANO;
		m_pRenderInstance->Set_CurMapType(CRenderer::MAP_VOLCANO);
		//DISTORTION_DESC tDistortionDesc{};
		//tDistortionDesc.vPosition = { 0.f,0.f,0.f,1.f };
		//tDistortionDesc.fLifeTime = 3.f;
		//tDistortionDesc.vScale = { 20.f,20.f };
		//tDistortionDesc.fFactor = 300.f;
		//tDistortionDesc.fMaxTime = 3.f;
		//tDistortionDesc.isLoop = true;
		//tDistortionDesc.vDir = { 1.f,0.f,0.f };
		//m_pRenderInstance->Create_Distortion(tDistortionDesc);

		//Space 음원 정지
		m_pGameInstance->Stop_Sound(CSound_Manager::SOUND_KEY_NAME::SPACE_BGM);
		//화산맵 음원 재생
		m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::VOLCANO_BGM, true, 1.f);
		break;
	}
}

_float2 CMap_Manager::Active_DestructiveFinish(_bool isRight)
{
	_float2 fMapToImpulse = { 0.f,0.f };
	m_isDestructive_Active = true;
	m_isRight = isRight;
	if (m_AccTime < 2)
	{
		switch (m_eCurMap)
		{
		case MAP_SPACE:
			fMapToImpulse = { (-100.f * (isRight == true ? -1.f : 1.f)),30.f };
			break;
		case MAP_VOLCANO:
			fMapToImpulse = { (-100.f * (isRight == true ? -1.f : 1.f)),40.f };
			break;
		}

		return fMapToImpulse;
	}

	switch (m_eCurMap)
	{
	case MAP_SPACE:
		static_cast<CSpaceMeteoBreak*>(m_Destructive_SpaceModels[L"Prototype_GameObject_SpaceMeteoBreak"])->Start_Space_DestructiveFinish(isRight);
		fMapToImpulse = { (-100.f * (isRight == true ? -1.f : 1.f)),30.f };
		break;
	case MAP_VOLCANO:
		for (auto& iter : m_VolcanoModels)
			iter.second->SetActive(false);

		m_VolcanoModels[L"Prototype_GameObject_Volcano_SkyCloud"]->m_bIsActive = true;

		static_cast<CTransform*>(static_cast<CVolcano_SkyCloud*>(m_VolcanoModels[L"Prototype_GameObject_Volcano_SkyCloud"])
			->Get_Component(TEXT("Com_Transform")))->Set_State(CTransform::STATE_POSITION, XMVectorSet(1500 * (isRight == true ? 1.f : -1.f), 0, -1000, 1.f));

		static_cast<CTransform*>(static_cast<CVolcano_SkyCloud*>(m_VolcanoModels[L"Prototype_GameObject_Volcano_SkyCloud"])
			->Get_Component(TEXT("Com_Transform")))->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(-10.f * (isRight == true ? 1.f : -1.f)));

		m_VolcanoModels[L"Prototype_GameObject_Volcano_Lava_Ground"]->m_bIsActive = true;
		m_VolcanoModels[L"Prototype_GameObject_Volcano_Stage"]->m_bIsActive = true;
		fMapToImpulse = { (-100.f * (isRight == true ? -1.f : 1.f)),30.f };
		static_cast<CVolcano_Destructive*>(m_Destructive_VolcanoModels[L"Prototype_GameObject_Volcano_Destructive"])->Start_Volcano_DestructiveFinish(isRight);
		break;
	}

	return fMapToImpulse;
}

_float2 CMap_Manager::Active_EastFinish()
{

	for (auto& iter : m_SpaceModels)
		iter.second->SetActive(false);
	for (auto& iter : m_Destructive_SpaceModels)
		iter.second->SetActive(false);
	for (auto& iter : m_VolcanoModels)
		iter.second->SetActive(false);
	for (auto& iter : m_Destructive_VolcanoModels)
		iter.second->SetActive(false);

	m_isEastFinishStart = true;
	m_fEastAccTime = 0.f;
	_float4 vMoveCamPos = {};
	_vector vLookAtPos;
	if (m_eCurMap == MAP_SPACE)
	{
		m_SpaceModels[L"Prototype_GameObject_SpaceSky"]->m_bIsActive = true;
		m_SpaceModels[L"Prototype_GameObject_SpaceEF"]->m_bIsActive = true;
		static_cast<CVolcanoEF*>(m_SpaceModels[L"Prototype_GameObject_SpaceEF"])->m_vEastColor = _float3(0.f, 0.68627f, 1.f);
		vMoveCamPos = _float4(0.f, 0.5f, -194.7f, 1.f);
		vLookAtPos = XMVectorSet(0.f, 1.f, 0.f, 1.f);

		m_pGameInstance->Set_Volume(CSound_Manager::SOUND_KEY_NAME::SPACE_BGM, 0.5f);
	}
	else if (m_eCurMap == MAP_VOLCANO)
	{
		//m_SpaceModels[L"Prototype_GameObject_SpaceSky"]->m_bIsActive = true;
		m_VolcanoModels[L"Prototype_GameObject_VolcanoEF"]->m_bIsActive = true;
		static_cast<CVolcanoEF*>(m_VolcanoModels[L"Prototype_GameObject_VolcanoEF"])->m_vEastColor = _float3(0.f, 0.68627f, 1.f);
		vMoveCamPos = _float4(-2.30689f, -0.06469f, -161.586121f, 1.f);
		vLookAtPos = XMVectorSet(0.f, 10.f, 0.f, 1.f);

		m_pGameInstance->Set_Volume(CSound_Manager::SOUND_KEY_NAME::VOLCANO_BGM, 0.5f);
	}

	XMStoreFloat4x4(&Result4x4, XMMatrixIdentity());
	CEffect_Layer::COPY_DESC tDesc{};
	tDesc.pPlayertMatrix = &Result4x4;

	switch (m_eEastEffectType)
	{
	case EAST_LASER:
		m_pEastEffect_Layer = CEffect_Manager::Get_Instance()->Copy_Layer_AndGet(TEXT("EF_EFFECT"), &tDesc);
		if (m_eCurMap == MAP_SPACE)
			static_cast<CVolcanoEF*>(m_SpaceModels[L"Prototype_GameObject_SpaceEF"])->m_vEastColor = _float3(0.f, 0.68627f, 1.f);
		else
			static_cast<CVolcanoEF*>(m_VolcanoModels[L"Prototype_GameObject_VolcanoEF"])->m_vEastColor = _float3(0.f, 0.68627f, 1.f);
		break;
	case EAST_SPHERE:
		m_pEastEffect_Layer = CEffect_Manager::Get_Instance()->Copy_Layer_AndGet(TEXT("EF_SPHERE_ENDING"), &tDesc);
		if (m_eCurMap == MAP_SPACE)
			static_cast<CVolcanoEF*>(m_SpaceModels[L"Prototype_GameObject_SpaceEF"])->m_vEastColor = _float3(0.86666f, 0.3254f, 0.f);
		else
			static_cast<CVolcanoEF*>(m_VolcanoModels[L"Prototype_GameObject_VolcanoEF"])->m_vEastColor = _float3(0.86666f, 0.3254f, 0.f);
		break;
	}

	static_cast<CMain_Camera*>(*(m_pGameInstance->Get_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")).begin()))
		->Set_EastFinish(vMoveCamPos);

	static_cast<CTransform*>(
		static_cast<CVirtual_Camera*>(
			static_cast<CMain_Camera*>(*(m_pGameInstance->Get_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")).begin()))
			->m_vecVirtualCamera[CMain_Camera::VIRTUAL_CAMERA_MAP])->Get_Component(TEXT("Com_Transform")))
		->LookAt(vLookAtPos);

	static_cast<CMain_Camera*>(*(m_pGameInstance->Get_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")).begin()))->StartCameraShake(10.f, 0.06);

	/* ------------------------------------------------------테스트용 프리카메라 무빙------------------------------------------------------ */
	static_cast<CTransform*>(
		static_cast<CVirtual_Camera*>(
			static_cast<CMain_Camera*>(*(m_pGameInstance->Get_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")).begin()))
			->m_vecVirtualCamera[CMain_Camera::VIRTUAL_CAMERA_FREE])->Get_Component(TEXT("Com_Transform")))
		->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.5f, -184.7f, 1.f));

	static_cast<CTransform*>(
		static_cast<CVirtual_Camera*>(
			static_cast<CMain_Camera*>(*(m_pGameInstance->Get_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")).begin()))
			->m_vecVirtualCamera[CMain_Camera::VIRTUAL_CAMERA_FREE])->Get_Component(TEXT("Com_Transform")))
		->LookAt(XMVectorSet(0.f, 1.f, 0.f, 0.f));
	/*------------------------------------------------------------------------------------------------------------*/

	m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::SCENE_CHANGE_Far_Expl, false, 1.f);

	return _float2();
}

void CMap_Manager::PlayerCall_EastFinish(East_Finish_Type eEastEffectType, _float fWhiteSpeed)
{
	m_isEastFinish = true;
	if (eEastEffectType == EAST_LASER)
		m_pRenderInstance->Start_WhiteOut(_float2(1.f, 0.f), &m_isWhiteDoneCheck, fWhiteSpeed);
	else
		m_pRenderInstance->Start_WhiteOut(_float2(0.f, 1.f), &m_isWhiteDoneCheck, fWhiteSpeed);
	m_eEastEffectType = eEastEffectType;
}

void CMap_Manager::IsDone_Active()
{
	switch (m_eCurMap)
	{
	case MAP_SPACE:
		static_cast<CSpaceMeteoBreak*>(m_Destructive_SpaceModels[L"Prototype_GameObject_SpaceMeteoBreak"])->IsDone_Active_Init();

		break;
	case MAP_VOLCANO:
		for (auto& iter : m_VolcanoModels)
			iter.second->SetActive(true);

		static_cast<CTransform*>(static_cast<CVolcano_SkyCloud*>(m_VolcanoModels[L"Prototype_GameObject_Volcano_SkyCloud"])
			->Get_Component(TEXT("Com_Transform")))->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

		static_cast<CTransform*>(static_cast<CVolcano_SkyCloud*>(m_VolcanoModels[L"Prototype_GameObject_Volcano_SkyCloud"])
			->Get_Component(TEXT("Com_Transform")))->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(0.f));

		static_cast<CVolcano_Destructive*>(m_Destructive_VolcanoModels[L"Prototype_GameObject_Volcano_Destructive"])->IsDone_Active_Init();

		break;
	}

	static_cast<CMain_Camera*>(*(m_pGameInstance->Get_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")).begin()))->Set_Virtual_Camera(CMain_Camera::VIRTUAL_CAMERA_NORMAL);

}

void CMap_Manager::Push_MapObject(MAP_TYPE eMapType, _wstring& strKey, CGameObject* pGameObject)
{
	if (pGameObject == nullptr)
		return;

	Safe_AddRef(pGameObject);

	switch (eMapType)
	{
	case MAP_SPACE:			m_SpaceModels.emplace(strKey, pGameObject);					/*pGameObject->SetActive(false);*/	break;
	case MAP_DEST_SPACE:	m_Destructive_SpaceModels.emplace(strKey, pGameObject);		/*pGameObject->SetActive(false);*/	break;
	case MAP_VOLCANO:		m_VolcanoModels.emplace(strKey, pGameObject);				pGameObject->SetActive(false);	break;
	case MAP_DEST_VOLCANO:	m_Destructive_VolcanoModels.emplace(strKey, pGameObject);	pGameObject->SetActive(false);	break;
	}
}

void CMap_Manager::All_Black(_bool isTrue)
{
	if (isTrue == true)
	{
		for (auto& iter : m_VolcanoModels)
			iter.second->SetActive(false);
		for (auto& iter : m_Destructive_VolcanoModels)
			iter.second->SetActive(false);
		for (auto& iter : m_SpaceModels)
			iter.second->SetActive(false);
		for (auto& iter : m_Destructive_SpaceModels)
			iter.second->SetActive(false);
	}
	else
	{
		if (m_eCurMap == MAP_SPACE)
		{
			for (auto& iter : m_VolcanoModels)
				iter.second->SetActive(false);
			for (auto& iter : m_Destructive_VolcanoModels)
				iter.second->SetActive(false);
			for (auto& iter : m_SpaceModels)
				iter.second->SetActive(true);
			for (auto& iter : m_Destructive_SpaceModels)
				iter.second->SetActive(false);

			m_SpaceModels[L"Prototype_GameObject_SpaceEF"]->m_bIsActive = false;
		}
		else
		{
			for (auto& iter : m_VolcanoModels)
				iter.second->SetActive(true);
			for (auto& iter : m_Destructive_VolcanoModels)
				iter.second->SetActive(false);
			for (auto& iter : m_SpaceModels)
				iter.second->SetActive(false);
			for (auto& iter : m_Destructive_SpaceModels)
				iter.second->SetActive(false);

			m_VolcanoModels[L"Prototype_GameObject_VolcanoEF"]->m_bIsActive = false;
		}
	}

}


HRESULT CMap_Manager::Ready_Components()
{
	return S_OK;
}

void CMap_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pRenderInstance);
	for (auto& iter : m_SpaceModels)
		Safe_Release(iter.second);
	for (auto& iter : m_Destructive_SpaceModels)
		Safe_Release(iter.second);
	for (auto& iter : m_VolcanoModels)
		Safe_Release(iter.second);
	for (auto& iter : m_Destructive_VolcanoModels)
		Safe_Release(iter.second);

	m_SpaceModels.clear();
	m_Destructive_SpaceModels.clear();
	m_VolcanoModels.clear();
	m_Destructive_VolcanoModels.clear();
}
