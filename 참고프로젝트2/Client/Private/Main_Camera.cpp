#include "stdafx.h"
#include "..\Public\Main_Camera.h"
#include "Virtual_Camera.h"
#include "GameInstance.h"

CMain_Camera::CMain_Camera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext }
{

}

CMain_Camera::CMain_Camera(const CMain_Camera& Prototype)
	: CCamera{ Prototype }
{

}

HRESULT CMain_Camera::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMain_Camera::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_vecVirtualCamera.reserve(VIRTUAL_CAMERA_END);

	_char* name = "";

	for (size_t i = VIRTUAL_CAMERA_NORMAL; i < VIRTUAL_CAMERA_END; i++)
	{
		switch (i)
		{
		case VIRTUAL_CAMERA_NORMAL:
			name = "Camera_Normal";
			break;
		case VIRTUAL_CAMERA_FREE:
			name = "Camera_Free";
			break;
		case VIRTUAL_CAMERA_MAP:
			name = "Camera_Map";
			break;
		case VIRTUAL_CAMERA_SON_HEAVY:
			name = "Camera_Son_Heavy";
			break;
		case VIRTUAL_CAMERA_SON_KNOCK_AWAY_UP:
			name = "Camera_Son_Knock_Away_Up";
			break;
		case VIRTUAL_CAMERA_SON_AIR_SMASH:
			name = "Camera_Son_Air_Smash";
			break;
		case VIRTUAL_CAMERA_SON_GRAB:
			name = "Camera_Son_Grab";
			break;
		case VIRTUAL_CAMERA_SON_SAME_GRAB:
			name = "Camera_Son_Same_Grab";
			break;
		case VIRTUAL_CAMERA_SON_ENERGY:
			name = "Camera_Son_Energy";
			break;
		case VIRTUAL_CAMERA_SON_ULTIMATE:
			name = "Camera_Son_Ultimate";
			break;
		case VIRTUAL_CAMERA_21_HEAVY:
			name = "Camera_21_Heavy";
			break;
		case VIRTUAL_CAMERA_21_KNOCK_AWAY_UP:
			name = "Camera_21_Knock_Away_Up";
			break;
		case VIRTUAL_CAMERA_21_AIR_SMASH:
			name = "Camera_21_Air_Smash";
			break;
		case VIRTUAL_CAMERA_21_GRAB:
			name = "Camera_21_Grab";
			break;
		case VIRTUAL_CAMERA_21_SAME_GRAB:
			name = "Camera_21_Same_Grab";
			break;
		case VIRTUAL_CAMERA_21_GRAB_SPECIAL:
			name = "Camera_21_Grab_Special";
			break;
		case VIRTUAL_CAMERA_21_ENERGY:
			name = "Camera_21_Energy";
			break;
		case VIRTUAL_CAMERA_21_ULTIMATE:
			name = "Camera_21_Ultimate";
			break;
		case VIRTUAL_CAMERA_HIT_HEAVY:
			name = "Camera_Hit_Heavy";
			break;
		case VIRTUAL_CAMERA_HIT_KNOCK_AWAY_UP:
			name = "Camera_Hit_Knock_Away_Up";
			break;
		case VIRTUAL_CAMERA_HIT_GRAB:
			name = "Camera_Hit_Grab";
			break;
		case VIRTUAL_CAMERA_HIT_214_MIDDLE:
			name = "Camera_Hit_214_Middle";
			break;
		case VIRTUAL_CAMERA_HIT_236_MIDDLE:
			name = "Camera_Hit_236_Middle";
			break;
		case VIRTUAL_CAMERA_HIT_236_HEAVY:
			name = "Camera_Hit_236_Heavy";
			break;
		case VIRTUAL_CAMERA_HIT_1_ULTIMATE:
			name = "Camera_Hit_1_Ultimate";
			break;
		case VIRTUAL_CAMERA_HIT_3_ULTIMATE:
			name = "Camera_Hit_3_Ultimate";
			break;
		case VIRTUAL_CAMERA_FRIEZA_LIGHT_FINAL:
			name = "Camera_Frieza_Light_Final";
			break;
		case VIRTUAL_CAMERA_FRIEZA_HEAVY:
			name = "Camera_Frieza_Heavy";
			break;
		case VIRTUAL_CAMERA_FRIEZA_KNOCK_AWAY_UP:
			name = "Camera_Frieza_Knock_Away_Up";
			break;
		case VIRTUAL_CAMERA_FRIEZA_GRAB:
			name = "Camera_Frieza_Grab";
			break;
		case VIRTUAL_CAMERA_FRIEZA_ULTIMATE_1:
			name = "Camera_Frieza_Ultimate_1";
			break;
		case VIRTUAL_CAMERA_FRIEZA_ULTIMATE_3:
			name = "Camera_Frieza_Ultimate_3";
			break;
		case VIRTUAL_CAMERA_FRIEZA_GOLDEN:
			name = "Camera_Frieza_Golden";
			break;
		case VIRTUAL_CAMERA_FRIEZA_DOWN_FORWARD_J:
			name = "Camera_Frieza_Down_Forward_J";
			break;
		case VIRTUAL_CAMERA_FRIEZA_FALL_REFLECT:
			name = "Camera_Frieza_Fall_Reflect";
			break;
		case VIRTUAL_CAMERA_COMMON_APPEAR:
			name = "Camera_Common_Appear";
			break;
		case VIRTUAL_CAMERA_GOKU_ENTRY:
			name = "Camera_Goku_Entry";
			break;
		case VIRTUAL_CAMERA_21_ENTRY:
			name = "Camera_21_Entry";
			break;
		case VIRTUAL_CAMERA_HIT_ENTRY:
			name = "Camera_Hit_Entry";
			break;
		case VIRTUAL_CAMERA_FRIEZA_ENTRY:
			name = "Camera_Frieza_Entry";
			break;
		case VIRTUAL_CAMERA_GOKU_VS_FRIEZA_ENTRY:
			name = "Camera_Goku_vs_Frieza_Entry";
			break;
		case VIRTUAL_CAMERA_HIT_WIN:
			name = "Camera_Hit_Win";
			break;
		}

		CGameObject* virtualCamera_Skill = m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Virtual_Camera"), &name);
		m_vecVirtualCamera.push_back(static_cast<CVirtual_Camera*>(virtualCamera_Skill));
	}

#pragma region 손오공

	stringToSkillID["Camera_Son_Heavy"] = VIRTUAL_CAMERA_SON_HEAVY;
	stringToSkillID["Camera_Son_Knock_Away_Up"] = VIRTUAL_CAMERA_SON_KNOCK_AWAY_UP;
	stringToSkillID["Camera_Son_Air_Smash"] = VIRTUAL_CAMERA_SON_AIR_SMASH;
	stringToSkillID["Camera_Son_Grab"] = VIRTUAL_CAMERA_SON_GRAB;
	stringToSkillID["Camera_Son_Same_Grab"] = VIRTUAL_CAMERA_SON_SAME_GRAB;
	stringToSkillID["Camera_Son_Energy"] = VIRTUAL_CAMERA_SON_ENERGY;
	stringToSkillID["Camera_Son_Ultimate"] = VIRTUAL_CAMERA_SON_ULTIMATE;

	stringToAnimID["Son_Heavy_Anim1"] = 0;
	stringToAnimID["Son_Knock_Away_Up_Anim1"] = 0;
	stringToAnimID["Son_Air_Smash_Anim1"] = 0;
	stringToAnimID["Son_Grab_Anim1"] = 0;
	stringToAnimID["Son_Same_Grab_Anim1"] = 0;
	stringToAnimID["Son_Energy_Anim1"] = 0;
	stringToAnimID["Son_Ultimate_Anim1"] = 0;
	stringToAnimID["Son_Ultimate_Anim2"] = 1;
	stringToAnimID["Son_Ultimate_Anim3"] = 2;
	stringToAnimID["Son_Ultimate_Anim4"] = 3;
	stringToAnimID["Son_Ultimate_Anim5"] = 4;
#pragma endregion

#pragma region 21

	stringToSkillID["Camera_21_Heavy"] = VIRTUAL_CAMERA_21_HEAVY;
	stringToSkillID["Camera_21_Knock_Away_Up"] = VIRTUAL_CAMERA_21_KNOCK_AWAY_UP;
	stringToSkillID["Camera_21_Air_Smash"] = VIRTUAL_CAMERA_21_AIR_SMASH;
	stringToSkillID["Camera_21_Grab"] = VIRTUAL_CAMERA_21_GRAB;
	stringToSkillID["Camera_21_Same_Grab"] = VIRTUAL_CAMERA_21_SAME_GRAB;
	stringToSkillID["Camera_21_Grab_Special"] = VIRTUAL_CAMERA_21_GRAB_SPECIAL;
	stringToSkillID["Camera_21_Energy"] = VIRTUAL_CAMERA_21_ENERGY;
	stringToSkillID["Camera_21_Ultimate"] = VIRTUAL_CAMERA_21_ULTIMATE;

	stringToAnimID["21_Heavy_Anim1"] = 0;
	stringToAnimID["21_Knock_Away_Up_Anim1"] = 0;
	stringToAnimID["21_Air_Smash_Anim1"] = 0;
	stringToAnimID["21_Grab_Anim1"] = 0;
	stringToAnimID["21_Same_Grab_Anim1"] = 0;
	stringToAnimID["21_Grab_Special_Anim1"] = 0;
	stringToAnimID["21_Energy_Anim1"] = 0;
	stringToAnimID["21_Ultimate_Anim1"] = 0;
	stringToAnimID["21_Ultimate_Anim2"] = 1;
	stringToAnimID["21_Ultimate_Anim3_Success"] = 2;
	stringToAnimID["21_Ultimate_Anim3_Fail"] = 3;
	stringToAnimID["21_Ultimate_Anim4"] = 4;
	stringToAnimID["21_Ultimate_Anim4_Flip"] = 5;

#pragma endregion

#pragma region Hit
	stringToSkillID["Camera_Hit_Heavy"] = VIRTUAL_CAMERA_HIT_HEAVY;
	stringToSkillID["Camera_Hit_Knock_Away_Up"] = VIRTUAL_CAMERA_HIT_KNOCK_AWAY_UP;
	stringToSkillID["Camera_Hit_Grab"] = VIRTUAL_CAMERA_HIT_GRAB;
	stringToSkillID["Camera_Hit_214_Middle"] = VIRTUAL_CAMERA_HIT_214_MIDDLE;
	stringToSkillID["Camera_Hit_236_Middle"] = VIRTUAL_CAMERA_HIT_236_MIDDLE;
	stringToSkillID["Camera_Hit_236_Heavy"] = VIRTUAL_CAMERA_HIT_236_HEAVY;
	stringToSkillID["Camera_Hit_1_Ultimate"] = VIRTUAL_CAMERA_HIT_1_ULTIMATE;
	stringToSkillID["Camera_Hit_3_Ultimate"] = VIRTUAL_CAMERA_HIT_3_ULTIMATE;

	stringToAnimID["Hit_Heavy_Anim1"] = 0;
	stringToAnimID["Hit_Knock_Away_Up_Anim1"] = 0;
	stringToAnimID["Hit_Grab_Anim1"] = 0;
	stringToAnimID["Hit_214_Middle_Anim1"] = 0;
	stringToAnimID["Hit_236_Middle_Anim1"] = 0;
	stringToAnimID["Hit_236_Heavy_Anim1"] = 0;
	stringToAnimID["Hit_1_Ultimate_Anim1"] = 0;
	stringToAnimID["Hit_1_Ultimate_Anim2"] = 1;
	stringToAnimID["Hit_3_Ultimate_Anim1"] = 0;
	stringToAnimID["Hit_3_Ultimate_Anim2"] = 1;
	stringToAnimID["Hit_3_Ultimate_Anim3"] = 2;
	stringToAnimID["Hit_3_Ultimate_Anim4"] = 3;

#pragma endregion

#pragma region Frieza

	stringToSkillID["Camera_Frieza_Light_Final"] = VIRTUAL_CAMERA_FRIEZA_LIGHT_FINAL;
	stringToSkillID["Camera_Frieza_Heavy"] = VIRTUAL_CAMERA_FRIEZA_HEAVY;
	stringToSkillID["Camera_Frieza_Knock_Away_Up"] = VIRTUAL_CAMERA_FRIEZA_KNOCK_AWAY_UP;
	stringToSkillID["Camera_Frieza_Grab"] = VIRTUAL_CAMERA_FRIEZA_GRAB;
	stringToSkillID["Camera_Frieza_Ultimate_1"] = VIRTUAL_CAMERA_FRIEZA_ULTIMATE_1;
	stringToSkillID["Camera_Frieza_Ultimate_3"] = VIRTUAL_CAMERA_FRIEZA_ULTIMATE_3;
	stringToSkillID["Camera_Frieza_Golden"] = VIRTUAL_CAMERA_FRIEZA_GOLDEN;
	stringToSkillID["Camera_Frieza_Down_Forward_J"] = VIRTUAL_CAMERA_FRIEZA_DOWN_FORWARD_J;
	stringToSkillID["Camera_Frieza_Fall_Reflect"] = VIRTUAL_CAMERA_FRIEZA_FALL_REFLECT;
	stringToSkillID["Camera_Common_Appear"] = VIRTUAL_CAMERA_COMMON_APPEAR;
	stringToSkillID["Camera_Goku_Entry"] = VIRTUAL_CAMERA_GOKU_ENTRY;
	stringToSkillID["Camera_21_Entry"] = VIRTUAL_CAMERA_21_ENTRY;
	stringToSkillID["Camera_Hit_Entry"] = VIRTUAL_CAMERA_HIT_ENTRY;
	stringToSkillID["Camera_Frieza_Entry"] = VIRTUAL_CAMERA_FRIEZA_ENTRY;
	stringToSkillID["Camera_Goku_vs_Frieza_Entry"] = VIRTUAL_CAMERA_GOKU_VS_FRIEZA_ENTRY;
	stringToSkillID["Camera_Hit_Win"] = VIRTUAL_CAMERA_HIT_WIN;


	stringToAnimID["Frieza_Light_Final_Anim1"] = 0;
	stringToAnimID["Frieza_Light_Final_Anim1_Flip"] = 1;
	stringToAnimID["Frieza_Light_Final_Anim2"] = 2;
	stringToAnimID["Frieza_Light_Final_Anim2_Flip"] = 3;
	stringToAnimID["Frieza_Heavy_Anim1"] = 0;
	stringToAnimID["Frieza_Knock_Away_Up_Anim1"] = 0;
	stringToAnimID["Frieza_Grab_Anim1"] = 0;
	stringToAnimID["Frieza_Ultimate_1_Anim1"] = 0;
	stringToAnimID["Frieza_Ultimate_1_Anim1_Flip"] = 1;
	stringToAnimID["Frieza_Ultimate_3_Anim1"] = 0;
	stringToAnimID["Frieza_Ultimate_3_Anim1_Flip"] = 1;
	stringToAnimID["Frieza_Golden_Anim1"] = 0;
	stringToAnimID["Frieza_Down_Forward_J"] = 0;
	stringToAnimID["Frieza_Down_Forward_J_Flip"] = 1;
	stringToAnimID["Frieza_Fall_Reflect"] = 0;
	stringToAnimID["Common_Appear_Anim_1"] = 0;
	stringToAnimID["Common_Appear_Anim_1_Flip"] = 1;
	stringToAnimID["Goku_Entry_Anim_1"] = 0;
	stringToAnimID["Goku_Entry_Anim_1_Flip"] = 1;
	stringToAnimID["21_Entry_Anim_1"] = 0;
	stringToAnimID["21_Entry_Anim_1_Flip"] = 1;
	stringToAnimID["Hit_Entry_Anim_1"] = 0;
	stringToAnimID["Hit_Entry_Anim_1_Flip"] = 1;
	stringToAnimID["Frieza_Entry_Anim_1"] = 0;
	stringToAnimID["Frieza_Entry_Anim_1_Flip"] = 1;
	stringToAnimID["Goku_VS_Frieza_Entry_Anim_1"] = 0;
	stringToAnimID["Goku_VS_Frieza_Entry_Anim_1_Flip"] = 1;
	stringToAnimID["Hit_Win_Anim_1"] = 0;

#pragma endregion


	return S_OK;
}

void CMain_Camera::Camera_Update(_float fTimeDelta)
{
	//선택된 가상카메라 업데이트
	m_vecVirtualCamera[m_currentVirtualMode]->Camera_Update(fTimeDelta);

	//가상카메라의 포지션의 정보를 메인카메라의 셋팅하기
	_vector position = static_cast<CTransform*>(m_vecVirtualCamera[m_currentVirtualMode]->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, position);

	//선택된 가상카메라의 정보로 뷰 투영 만들기
	Update_Camera(m_vecVirtualCamera[m_currentVirtualMode], fTimeDelta);

}

void CMain_Camera::Update(_float fTimeDelta)
{
	m_vecVirtualCamera[m_currentVirtualMode]->Update(fTimeDelta);
}

void CMain_Camera::Late_Update(_float fTimeDelta)
{
	m_vecVirtualCamera[m_currentVirtualMode]->Late_Update(fTimeDelta);
}

void CMain_Camera::Set_Virtual_Camera(VIRTUAL_CAMERA mode)
{
	m_currentVirtualMode = mode;
	m_vecVirtualCamera[m_currentVirtualMode]->Set_CameraMode(mode);
}

void CMain_Camera::Add_Point(_float duration, _int type, const _float4x4* worldMatrixPtr, _float damping, _bool hasWorldFloat4x4, _int animationIndex)
{
	m_vecVirtualCamera[m_currentVirtualMode]->Add_Point(duration, type, worldMatrixPtr, damping, hasWorldFloat4x4, animationIndex);
}

void CMain_Camera::Add_NormalPoint(_float duration, _int type, const _float4x4* worldMatrixPtr, _float damping, _bool hasWorldFloat4x4, _int animationIndex)
{
	CTransform* transform = static_cast<CTransform*>(m_vecVirtualCamera[VIRTUAL_CAMERA_NORMAL]->Get_Component(TEXT("Com_Transform")));
	m_vecVirtualCamera[m_currentVirtualMode]->Add_NormalPoint(duration, type, worldMatrixPtr, damping, hasWorldFloat4x4, animationIndex, transform);
}

void CMain_Camera::Remove_Point(_int currentIndex, _int animationIndex)
{
	m_vecVirtualCamera[m_currentVirtualMode]->Remove_Point(currentIndex, animationIndex);
}

vector<CameraPoint>& CMain_Camera::Get_VectorPoint(_int index)
{
	//현재 메인카메라가 사용하고 있는 가상카메라의 listPoints를 가져온다.
	return m_vecVirtualCamera[m_currentVirtualMode]->m_mapPoints[index];
}

void CMain_Camera::IMGUI_Play(_int animationIndex, CGameObject* gameObject)
{
	m_vecVirtualCamera[m_currentVirtualMode]->Start_Play(animationIndex, true, gameObject);
}

void CMain_Camera::Play(VIRTUAL_CAMERA cameraID, _int animationIndex, CGameObject* gameObject, CGameObject* EnemyObject, _bool ignoreFlip)
{
	//현재 재생중인 Stop
	Stop();
	Set_Player(gameObject, EnemyObject);
	//가상카메라를 CameraID에 따라 셋팅
	Set_Virtual_Camera(cameraID);

	//EnemyObject가 들어오면 EnemyObject로 들어가야함
	CGameObject* selectObject = EnemyObject == nullptr ? gameObject : EnemyObject;
	m_vecVirtualCamera[m_currentVirtualMode]->Start_Play(animationIndex, false, selectObject, ignoreFlip);
}

void CMain_Camera::Stop()
{
	m_vecVirtualCamera[m_currentVirtualMode]->Stop();
}

void CMain_Camera::Pause()
{
	m_vecVirtualCamera[m_currentVirtualMode]->Pause();
}

void CMain_Camera::Button_Stop()
{
	m_vecVirtualCamera[m_currentVirtualMode]->Button_Stop();
}

void CMain_Camera::StartCameraShake(_float fDuration, _float fMagnitude)
{
	m_vecVirtualCamera[m_currentVirtualMode]->StartCameraShake(fDuration, fMagnitude);
}

void CMain_Camera::Move_Point(_int index, _int animationIndex)
{
	m_vecVirtualCamera[m_currentVirtualMode]->Move_Point(index, animationIndex);
}

void CMain_Camera::Modify_Transform(_int index, _int animationIndex)
{
	m_vecVirtualCamera[m_currentVirtualMode]->Modify_Transform(index, animationIndex);
}

void CMain_Camera::ApplyCameraData(CameraSaveData& cameraData)
{
	for (const auto& modelData : cameraData.models)
	{
		// 모델의 스킬마다 돈다
		for (const auto& skillData : modelData.skills)
		{
			// 스킬 이름을 스킬 ID로 매핑
			std::string skillNameStr = skillData.skillName;
			auto skillIt = stringToSkillID.find(skillNameStr);
			if (skillIt == stringToSkillID.end())
			{
				// 스킬이 맵에 없으면 건너뜁니다.
				continue;
			}

			_int skillID = skillIt->second;

			// 카메라 인덱스 얻기
			_int cameraIndex = skillID;
			if (cameraIndex >= m_vecVirtualCamera.size())
			{
				// 유효하지 않은 인덱스면 건너뜁니다.
				continue;
			}

			CVirtual_Camera* pCurrentCamera = m_vecVirtualCamera[cameraIndex];

			// 스킬에 있는 애니메이션
			for (const auto& animData : skillData.animations)
			{
				// 애니메이션 이름을 애니메이션 ID로 매핑
				std::string animNameStr = animData.animationName;
				auto animIt = stringToAnimID.find(animNameStr);
				if (animIt == stringToAnimID.end())
				{
					// 애니메이션이 맵에 없으면 건너뜁니다.
					continue;
				}

				_int animID = animIt->second;

				// 기존 포인트 초기화
				pCurrentCamera->m_mapPoints[animID].clear();

				// 새로운 포인트 할당
				vector<CameraPoint> points;

				for (const auto& pointData : animData.points)
				{
					CameraPoint point = pointData;

					point.pWorldFloat4x4 = nullptr;

					// 포인트 벡터에 추가
					points.push_back(point);
				}

				// 포인트 저장
				pCurrentCamera->m_mapPoints[animID] = points;
			}
		}
	}
}

_int CMain_Camera::Get_CameraIndex(_int modelID, _int skillID)
{
	_int index = -1;

	if (modelID == 0)
		index = VIRTUAL_CAMERA_NORMAL;

	else if (modelID == 1) { // MODELID_SON
		if (skillID == 0)
			index = VIRTUAL_CAMERA_SON_HEAVY;
		else if (skillID == 1)
			index = VIRTUAL_CAMERA_SON_KNOCK_AWAY_UP;
		else if (skillID == 2)
			index = VIRTUAL_CAMERA_SON_AIR_SMASH;
		else if (skillID == 3)
			index = VIRTUAL_CAMERA_SON_GRAB;
		else if (skillID == 4)
			index = VIRTUAL_CAMERA_SON_SAME_GRAB;
		else if (skillID == 5)
			index = VIRTUAL_CAMERA_SON_ENERGY;
		else if (skillID == 6)
			index = VIRTUAL_CAMERA_SON_ULTIMATE;
	}
	else if (modelID == 2) { // MODELID_21
		if (skillID == 0)
			index = VIRTUAL_CAMERA_21_HEAVY;
		else if (skillID == 1)
			index = VIRTUAL_CAMERA_21_KNOCK_AWAY_UP;
		else if (skillID == 2)
			index = VIRTUAL_CAMERA_21_AIR_SMASH;
		else if (skillID == 3)
			index = VIRTUAL_CAMERA_21_GRAB;
		else if (skillID == 4)
			index = VIRTUAL_CAMERA_21_SAME_GRAB;
		else if (skillID == 5)
			index = VIRTUAL_CAMERA_21_GRAB_SPECIAL;
		else if (skillID == 6)
			index = VIRTUAL_CAMERA_21_ENERGY;
		else if (skillID == 7)
			index = VIRTUAL_CAMERA_21_ULTIMATE;
	}
	else if (modelID == 3) { // MODELID_HIT
		if (skillID == 0)
			index = VIRTUAL_CAMERA_HIT_HEAVY;
		else if (skillID == 1)
			index = VIRTUAL_CAMERA_HIT_KNOCK_AWAY_UP;
		else if (skillID == 2)
			index = VIRTUAL_CAMERA_HIT_GRAB;
		else if (skillID == 3)
			index = VIRTUAL_CAMERA_HIT_214_MIDDLE;
		else if (skillID == 4)
			index = VIRTUAL_CAMERA_HIT_236_MIDDLE;
		else if (skillID == 5)
			index = VIRTUAL_CAMERA_HIT_236_HEAVY;
		else if (skillID == 6)
			index = VIRTUAL_CAMERA_HIT_1_ULTIMATE;
		else if (skillID == 7)
			index = VIRTUAL_CAMERA_HIT_3_ULTIMATE;
	}
	else if (modelID == 4) { // MODELID_Frieza
		if (skillID == 0)
			index = VIRTUAL_CAMERA_FRIEZA_LIGHT_FINAL;
		else if (skillID == 1)
			index = VIRTUAL_CAMERA_FRIEZA_HEAVY;
		else if (skillID == 2)
			index = VIRTUAL_CAMERA_FRIEZA_KNOCK_AWAY_UP;
		else if (skillID == 3)
			index = VIRTUAL_CAMERA_FRIEZA_GRAB;
		else if (skillID == 4)
			index = VIRTUAL_CAMERA_FRIEZA_ULTIMATE_1;
		else if (skillID == 5)
			index = VIRTUAL_CAMERA_FRIEZA_ULTIMATE_3;
		else if (skillID == 6)
			index = VIRTUAL_CAMERA_FRIEZA_GOLDEN;
		else if (skillID == 7)
			index = VIRTUAL_CAMERA_FRIEZA_DOWN_FORWARD_J;
		else if (skillID == 8)
			index = VIRTUAL_CAMERA_FRIEZA_FALL_REFLECT;
		else if (skillID == 9)
			index = VIRTUAL_CAMERA_COMMON_APPEAR;
		else if (skillID == 10)
			index = VIRTUAL_CAMERA_GOKU_ENTRY;
		else if (skillID == 11)
			index = VIRTUAL_CAMERA_21_ENTRY;
		else if (skillID == 12)
			index = VIRTUAL_CAMERA_HIT_ENTRY;
		else if (skillID == 13)
			index = VIRTUAL_CAMERA_FRIEZA_ENTRY;
		else if (skillID == 14)
			index = VIRTUAL_CAMERA_GOKU_VS_FRIEZA_ENTRY;
		else if (skillID == 15)
			index = VIRTUAL_CAMERA_HIT_WIN;
	}

	return index;
}

void CMain_Camera::Delete_Points(_int animationIndex)
{
	m_vecVirtualCamera[m_currentVirtualMode]->Delete_Points(animationIndex);
}

void CMain_Camera::SetPosition(_fvector position)
{
	CTransform* virtual_Transform = static_cast<CTransform*>(m_vecVirtualCamera[m_currentVirtualMode]->Get_Component(TEXT("Com_Transform")));
	virtual_Transform->Set_State(CTransform::STATE_POSITION, position);
}

void CMain_Camera::Set_Player(CGameObject* pPlayer, CGameObject* pEnemy)
{
	for (size_t i = VIRTUAL_CAMERA_NORMAL; i < VIRTUAL_CAMERA_END; i++)
		m_vecVirtualCamera[i]->Set_Player(pPlayer, pEnemy);
}

const char* CMain_Camera::Get_Current_CameraName()
{
	return m_vecVirtualCamera[m_currentVirtualMode]->GetTabName();
}

_bool CMain_Camera::Get_IsPlay()
{
	return m_vecVirtualCamera[m_currentVirtualMode]->Get_IsPlay();
}

void CMain_Camera::Set_DyingTeam(_uint iDyingTeam)
{

	m_vecVirtualCamera[VIRTUAL_CAMERA_MAP]->Set_DyingTeam(iDyingTeam,
		static_cast<CTransform*>(m_vecVirtualCamera[m_currentVirtualMode]->Get_Component(TEXT("Com_Transform")))->Get_WorldMatrix());
	Set_Virtual_Camera(VIRTUAL_CAMERA_MAP);
}

void CMain_Camera::Set_EastFinish(_float4 InitPos)
{
	static_cast<CTransform*>(m_vecVirtualCamera[VIRTUAL_CAMERA_MAP]->Get_Component(TEXT("Com_Transform")))
		->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&InitPos));

	m_vecVirtualCamera[VIRTUAL_CAMERA_MAP]->Set_EastFinish();
	Set_Virtual_Camera(VIRTUAL_CAMERA_MAP);
}

void CMain_Camera::Set_CirclePlay(_bool isClockwise, _float rotationSpeed)
{
	m_vecVirtualCamera[m_currentVirtualMode]->SetCirclePlay(isClockwise, rotationSpeed);
}

HRESULT CMain_Camera::Render(_float fTimeDelta)
{
	return S_OK;
}

CMain_Camera* CMain_Camera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMain_Camera* pInstance = new CMain_Camera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMain_Camera"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMain_Camera::Clone(void* pArg)
{
	CMain_Camera* pInstance = new CMain_Camera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMain_Camera"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMain_Camera::Free()
{
	for (auto& iter : m_vecVirtualCamera)
		Safe_Release(iter);

	__super::Free();

}
