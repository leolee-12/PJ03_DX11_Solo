#include "stdafx.h"
#include "..\Public\Loader.h"
#include "GameInstance.h"
#include <process.h>

#pragma region GAMEOBJECT

#include "Camera_Dynamic.h"
#include "Camera_Action.h"
#include "Camera_ThirdPerson.h"
#include "Camera_Action.h"

#include "BasicUI.h"
#include "UI_Include_List.h"

#include "MapObject.h"
#include "AnimObject.h"
#include "AnimMapObject.h"
#include "Instance_Object.h"
#include "Model_Inst.h"
#include "Tool_Trigger.h"
#include "Tool_Collider.h"

#include "SkyBox.h"
#include "Particle.h"
#include "Sprite.h"
#include "Trail_Effect.h"
#include "Trail_Buffer.h"
#include "Effect_Group.h"
#include "Effect_Manager.h"

#include "PhysX_Collider.h"
#include "PhysX_Controller.h"
#include "PhysX_Trigger.h"
#include "PhysX_Wall.h"

#include "Cloud.h"
#include "Cloud_Weapon.h"
#include "AttackCollider.h"

#include "Aerith/Aerith.h"
#include "Aerith/Parts/Aerith_Weapon.h"
#include "Aerith/Weapon/Aerith_List_Weapon.h"

#include "Boss/AirBurster/AirBurster.h"
#include "Boss/AirBurster/Parts/AirBurster_LeftArm.h"
#include "Boss/AirBurster/Parts/AirBurster_RightArm.h"
#include "Boss/AirBurster/State_List_AirBurster.h"

#include "Boss/Bahamut/Bahamut.h"
#include "Boss/Bahamut/State_List_Bahamut.h"

#include "Monster,MonsterWeapon_List.h"
#include "NPC,MobList.h"
#include "Map_Laser.h"

#include "Demo_Parts.h"

#include "InteractionBox.h"
#include "InteractionBox_Split.h"

#pragma endregion

#include "ModelBinary.h"
#include "Utility_File.h"
#include "CommonModelComp.h"
#include "StatusComp.h"

#include "Common_Shield.h"

#include "AnimNotifyComp.h"
#include "AnimNotifyCollection_Client.h"
#include "Utility/DelegateTemplate.h"

CLoader::CLoader(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);


}

// typedef unsigned(__stdcall* _beginthreadex_proc_type)(void*);

_uint APIENTRY LoadingThread(void* pArg)
{
	if (FAILED(CoInitializeEx(nullptr, 0)))
		return 0;

	CLoader* pLoader = (CLoader*)pArg;

	pLoader->Loading();

	return 0;
}


HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	/* 어떤 레벨의 자원을 로드해야하는지? */
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);

	/* 스레드를 생성한다. */
	/* LoadingThread : 생성한 스레드가 가장 먼저 호출해야할 함수 */
	/* this : 위 함수를 호출하면서 인자로 전달할 데이터. */
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingThread, this, 0, nullptr);
	if (0 == m_hThread)
		RETURN_EFAIL;

	return S_OK;
}

void CLoader::Print_LoadingText()
{
	SetWindowText(g_hWnd, m_szLoadingText);
}

HRESULT CLoader::Loading()
{
	EnterCriticalSection(&m_CriticalSection);

	HRESULT hr = 0;

	switch (m_eNextLevelID)
	{
	case LEVEL_LOGO:
		hr = Loading_For_Logo_Level(LEVEL_LOGO);
		break;

	case LEVEL_STAGE1:
		hr = Loading_For_GamePlay_Level(LEVEL_STAGE1);
		break;

	case LEVEL_STAGE2:
		hr = Loading_For_GamePlay_Level(LEVEL_STAGE2);
		break;

	case LEVEL_STAGE3:
		hr = Loading_For_GamePlay_Level(LEVEL_STAGE3);
		break;

	case LEVEL_VILLAGE1:
		hr = Loading_For_GamePlay_Level(LEVEL_VILLAGE1);
		break;

	case LEVEL_VILLAGE2:
		hr = Loading_For_GamePlay_Level(LEVEL_VILLAGE2);
		break;

	case LEVEL_BOSS1:
		hr = Loading_For_GamePlay_Level(LEVEL_BOSS1);
		break;

	case LEVEL_TOOL:
		hr = Loading_For_Tool_Level();
		break;
	}

	if (FAILED(hr))
	{
		LeaveCriticalSection(&m_CriticalSection);
		cerr << "로딩 실패" << endl;
		RETURN_EFAIL;
	}

	m_isFinished = true;

	LeaveCriticalSection(&m_CriticalSection);

	return S_OK;
}

HRESULT CLoader::Loading_For_Logo_Level(LEVEL eLevelID)
{
	/* 로고 레벨에 필요한 자원을 로드하자. */
	lstrcpy(m_szLoadingText, TEXT("데이터 경로를 로드하는 중입니다."));

	/* 로고 레벨에 필요한 자원을 로드하자. */
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로드하는 중입니다."));

	/* For.Prototype_Component_Texture_Logo */

	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로드하는 중입니다."));


	lstrcpy(m_szLoadingText, TEXT("셰이더를(을) 로드하는 중입니다."));


	if (!m_pGameInstance->IsVisitedLevel(LEVEL_LOGO))
	{
		lstrcpy(m_szLoadingText, TEXT("원형객체를(을) 로드하는 중입니다."));

		/* For.Prototype_GameObject_BackGround */
		/*if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BasicUI"),
			CBasicUI::Create(m_pDevice, m_pContext))))
			RETURN_EFAIL;*/

		m_pGameInstance->Set_VisitedLevel(LEVEL_LOGO);
	}

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay_Level(LEVEL eLevelID)
{
	/* 게임플레이 레벨에 필요한 자원을 로드하자. */
	lstrcpy(m_szLoadingText, TEXT("[Game] 텍스쳐를 로드하는 중입니다."));


	lstrcpy(m_szLoadingText, TEXT("[Game] 모델을 로드하는 중입니다."));


	if (!m_pGameInstance->IsVisitedLevel(LEVEL_TOOL))
	{
		lstrcpy(m_szLoadingText, TEXT("[Game] Static 컴포넌트를 로드하는 중입니다."));
		{
#pragma region Physx 원형
			/* For.Prototype_Component_Collider_AABB */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
				CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
				RETURN_EFAIL;

			/* For.Prototype_Component_Collider_OBB */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
				CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
				RETURN_EFAIL;

			/* For.Prototype_Component_Collider_Sphere */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
				CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
				RETURN_EFAIL;

			/* For.Prototype_Component_Collider_Sphere */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Ray"),
				CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_RAY))))
				RETURN_EFAIL;

			/* For.Prototype_Component_PhysX_Collider */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
				CPhysX_Collider::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			/* For.Prototype_Component_PhysX_Controller */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Controller"),
				CPhysX_Controller::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
#pragma endregion

#pragma region 기타 원형 컴포넌트
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_StateMachine"),
				CStateMachine::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_CommonModel"),
				CCommonModelComp::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_AnimCommonModel"),
				CCommonModelComp::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			/* For.Prototype_Component_VIBuffer_Cube */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"),
				CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			/* For.Prototype_Component_Material */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Material"),
				CMaterialComponent::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			/* For.Prototype_Component_VIBuffer_Particle_Point */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Particle_Point"),
				CVIBuffer_Particle_Point::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			/* For.Prototype_Component_VIBuffer_Trail */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Trail"),
				CVIBuffer_Trail::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Model_Inst"),
				CModel_Inst::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
#pragma endregion




#pragma region 셰이더 원형
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxCube"),
				CShader::Create(m_pDevice, m_pContext, TEXT("Shader_VtxCube"),
					VTXCUBE::Elements, VTXCUBE::iNumElements))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"),
				CShader::Create(m_pDevice, m_pContext, TEXT("Shader_AnimModel"),
					VTXMESHANIM::Elements, VTXMESHANIM::iNumElements))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Particle_Point"),
				CShader::Create(m_pDevice, m_pContext, TEXT("Shader_Particle_Point"),
					VTX_PARTICLE_POINT::Elements, VTX_PARTICLE_POINT::iNumElements))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect_Rect"),
				CShader::Create(m_pDevice, m_pContext, TEXT("Shader_Effect_Rect"),
					VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect_Mesh"),
				CShader::Create(m_pDevice, m_pContext, TEXT("Shader_Effect_Mesh"),
					VTXMESH::Elements, VTXMESH::iNumElements))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect_Trail"),
				CShader::Create(m_pDevice, m_pContext, TEXT("Shader_Effect_Trail"),
					VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model"),
				CShader::Create(m_pDevice, m_pContext, TEXT("Shader_Model"),
					VTXMESH::Elements, VTXMESH::iNumElements))))
				RETURN_EFAIL;
#pragma endregion



#pragma region 노티파이 원형
			

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Notify_Cloud"),
				CAnimNotifyComp::Create(m_pDevice, m_pContext,
					m_pGameInstance->MainDataPath() + TEXT("Anim_Data/Cloud_Notify.json"),
					&CAnimNotifyCollection_Client::Create_Callback))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Notify_Aerith"),
				CAnimNotifyComp::Create(m_pDevice, m_pContext,
					m_pGameInstance->MainDataPath() + TEXT("Anim_Data/Aerith_Notify.json"),
					&CAnimNotifyCollection_Client::Create_Callback))))
				RETURN_EFAIL;

			

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Notify_Bahamut"),
				CAnimNotifyComp::Create(m_pDevice, m_pContext,
					m_pGameInstance->MainDataPath() + TEXT("Anim_Data/Bahamut_Notify.json"),
					&CAnimNotifyCollection_Client::Create_Callback))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Notify_AirBuster"),
				CAnimNotifyComp::Create(m_pDevice, m_pContext,
					m_pGameInstance->MainDataPath() + TEXT("Anim_Data/AirBurster_Notify.json"),
					&CAnimNotifyCollection_Client::Create_Callback))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Notify_Soldier"),
				CAnimNotifyComp::Create(m_pDevice, m_pContext,
					m_pGameInstance->MainDataPath() + TEXT("Anim_Data/Soldier_Notify.json"),
					&CAnimNotifyCollection_Client::Create_Callback))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Notify_MonoDrive"),
				CAnimNotifyComp::Create(m_pDevice, m_pContext,
					m_pGameInstance->MainDataPath() + TEXT("Anim_Data/MonoDrive_Notify.json"),
					&CAnimNotifyCollection_Client::Create_Callback))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Notify_Stray"),
				CAnimNotifyComp::Create(m_pDevice, m_pContext,
					m_pGameInstance->MainDataPath() + TEXT("Anim_Data/Stray_Notify.json"),
					&CAnimNotifyCollection_Client::Create_Callback))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Notify_GuardHound"),
				CAnimNotifyComp::Create(m_pDevice, m_pContext,
					m_pGameInstance->MainDataPath() + TEXT("Anim_Data/GuardHound_Notify.json"),
					&CAnimNotifyCollection_Client::Create_Callback))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Notify_Sweeper"),
				CAnimNotifyComp::Create(m_pDevice, m_pContext,
					m_pGameInstance->MainDataPath() + TEXT("Anim_Data/Sweeper_Notify.json"),
					&CAnimNotifyCollection_Client::Create_Callback))))
				RETURN_EFAIL;
#pragma endregion



#pragma region 스테이터스 원형
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Status"),
				CStatusComp::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
#pragma endregion


		}


		lstrcpy(m_szLoadingText, TEXT("[Game] GameObject 원형객체를 로드하는 중입니다."));
		{
			/* For.Prototype_GameObject_Camera_Dynamic */
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Dynamic"),
				CCamera_Dynamic::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			/* For.Prototype_GameObject_Camera_Action */
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Action"),
				CCamera_Action::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			/* For.Prototype_GameObject_Camera_ThirdPerson */
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_ThirdPerson"),
				CCamera_ThirdPerson::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MapObject"),
				CMapObject::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Instance_Object"),
				CInstance_Object::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AnimObject"),
				CAnimObject::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AnimMapObject"),
				CAnimMapObject::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Trigger"),
				CTool_Trigger::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Tool_Collider"),
				CTool_Collider::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PhysTrigger"),
				CPhysX_Trigger::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PhysWall"),
				CPhysX_Wall::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AttackCollider"),
				CAttackCollider::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

#pragma region UI

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_EventActionMarker"),
				CUI_EventActionMarker::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Command"),
				CUI_Command::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_HPBar"),
				CUI_HPBar::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MainPopUp"),
				CUI_MainPopUp::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_LockOnMarker"),
				CUI_LockOnMarker::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_KeyMenual"),
				CUI_KeyMenual::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_DamageFont"),
				CUI_DamageFont::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_DamageNum"),
				CUI_DamageNum::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_FloorDir"),
				CUI_FloorDir::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Monster"),
				CUI_Monster::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MonsterUIPart"),
				CUI_MonsterUIPart::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Tutorial"),
				CUI_Tutorial::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_TutorialInner"),
				CUI_TutorialInner::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MapTitle"),
				CUI_MapTitle::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_SpeechDlg"),
				CUI_SpeechDlg::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_EventLogPopUp"),
				CUI_EventPopUp::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_EscapeTimeCount"),
				CUI_EscapeTimeCount::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MiniGame"),
				CUI_MiniGame::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MiniGame_Button"),
				CUI_MiniGame_Button::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MouseCursor"),
				CUI_MouseCursor::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Shop"),
				CUI_Shop::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Dialog"),
				CUI_Dialog::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;


#pragma endregion


			/* For.Prototype_GameObject_SkyBox */
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SkyBox_Test"),
				CSkyBox::Create(m_pDevice, m_pContext, L"Night01"))))
				RETURN_EFAIL;




#pragma region 무기 원형
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Soldier_Bullet"),
				CSoldier_Bullet::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Soldier_Bomb"),
				CSoldier_Bomb::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sweeper_Bullet"),
				CSweeper_Bullet::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sweeper_Bomb"),
				CSweeper_Bomb::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sweeper_FlameGas"),
				CSweeper_FlameGas::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sweeper_Bombing"),
				CSweeper_Bombing::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sweeper_Flame"),
				CSweeper_Flame::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sweeper_RushColl"),
				CSweeper_RushColl::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MonoDrive_FireBall"),
				CMonoDrive_FireBall::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Soldier_Kick"),
				CSoldier_Kick::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Soldier_Combat"),
				CSoldier_Combat::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Map_Laser"),
				CMap_Laser::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Guard_Hound_Bite"),
				CGuard_Hound_BiteColl::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Guard_Hound_Whip"),
				CGuard_Hound_WhipColl::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Guard_Hound_Claw"),
				CGuard_Hound_ClawColl::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MonoDrive_Drill"),
				CMonoDrive_DrillColl::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Stray_Laser"),
				CStray_Laser::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Stray_Charge"),
				CStray_Charge::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AirBurster_ShoulderBeam"),
				CAirBurster_ShoulderBeam::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AirBurster_EnergyBall"),
				CAirBurster_EnergyBall::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AirBurster_TankBurster"),
				CAirBurster_TankBurster::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AirBurster_EMField"),
				CAirBurster_EMField::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AirBurster_Burner"),
				CAirBurster_Burner::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AirBurster_FingerBeam"),
				CAirBurster_FingerBeam::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AirBurster_MachineGun"),
				CAirBurster_MachineGun::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AirBurster_Hook"),
				CAirBurster_Hook::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AirBurster_Block"),
				CAirBurster_Block::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;


			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bahamut_Clo"),
				CBahamut_Clo::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bahamut_Breath"),
				CBahamut_Breath::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bahamut_Body"),
				CBahamut_Body::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bahamut_DarkClaw"),
				CBahamut_DarkClaw::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bahamut_HeavyStrike"),
				CBahamut_HeavyStrike::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bahamut_Inferno"),
				CBahamut_Inferno::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bahamut_Grab"),
				CBahamut_Grab::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bahamut_FireBall"),
				CBahamut_FireBall::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bahamut_Aura"),
				CBahamut_Aura::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bahamut_Meteor"),
				CBahamut_Meteor::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bahamut_FlareBall"),
				CBahamut_FlareBall::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;


			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Aerith_NormalBullet"),
				CAerith_NormalBullet::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Aerith_FlameBall"),
				CAerith_FlameBall::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Aerith_FlameBlast"),
				CAerith_FlameBlast::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Aerith_RayOfJudge"),
				CAerith_RayOfJudge::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Aerith_TempestBullet"),
				CAerith_TempestBullet::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Aerith_TempestDuration"),
				CAerith_TempestDuration::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Aerith_TempestBlast"),
				CAerith_TempestBlast::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Aerith_GunArea"),
				CAerith_GunArea::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Aerith_GunArea_Vulcan"),
				CAerith_GunArea_Vulcan::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Aerith_GunArea_Bullet"),
				CAerith_GunArea_Bullet::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Aerith_LustrousShield"),
				CAerith_LustrousShield::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Aerith_SorcerousStorm"),
				CAerith_SorcerousStorm::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;


			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Common_Shield"),
				CCommon_Shield::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

#pragma endregion

		}
	}

	lstrcpy(m_szLoadingText, TEXT("[Game] 데이터 로딩이 완료되었습니다."));

	return S_OK;
}

HRESULT CLoader::Loading_For_Tool_Level()
{

	if (FAILED(Loading_For_GamePlay_Level(LEVEL_TOOL)))
		RETURN_EFAIL;

#if EFFECT_LOAD == 0 || EFFECT_LOAD == 2

	if (!m_pGameInstance->IsVisitedLevel(LEVEL_TOOL))
	{

		if (FAILED(Ready_Effect_Texture()))
			RETURN_EFAIL;

		if (FAILED(CEffect_Manager::GetInstance()->Create_Data_Prototype(m_pDevice, m_pContext)))
			RETURN_EFAIL;

		lstrcpy(m_szLoadingText, TEXT("[TOOL] 프로토타입 컴포넌트를 로드중입니다."));
		{
			if (FAILED(Ready_Effect_Model()))
				RETURN_EFAIL;
		}
	}

#endif

	lstrcpy(m_szLoadingText, TEXT("[TOOL] 프로토타입 캐릭터를 로드중입니다."));
	if (!m_pGameInstance->IsVisitedLevel(LEVEL_TOOL))
	{
		{
			m_pGameInstance->Load_DirectorySub_Models(TEXT("Character/"));

#if EFFECT_LOAD == 0 || EFFECT_LOAD == 2
			/* For.Prototype_GameObject_Particle */
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle"),
				CParticle::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Rect"),
				CSprite::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TrailEffect"),
				CTrail_Effect::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TrailBuffer"),
				CTrail_Buffer::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EffectGroup"),
				CEffect_Group::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
#endif
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Cloud"),
				CCloud::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Aerith"),
				CAerith::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GuardHound"),
				CGuardHound::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Security_Soldier"),
				CSecurity_Soldier::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MonoDrive"),
				CMonoDrive::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Stray"),
				CStray::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sweeper"),
				CSweeper::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Soldier_Gun"),
				CSoldier_Gun::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Soldier_Grenade"),
				CSoldier_Grenade::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Soldier_Tonfa"),
				CSoldier_Tonfa::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;


			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Wedge_Gun"),
				CWedge_Gun::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Biggs_Gun"),
				CBiggs_Gun::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_NPC_Gun"),
				CNPC_Gun::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Cloud_Weapon"),
				CCloud_Weapon::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Aerith_Weapon"),
				CAerith_Weapon::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

#pragma region 에어버스터

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AirBurster"),
				CAirBurster::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AirBurster_LeftArm"),
				CAirBurster_LeftArm::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AirBurster_RightArm"),
				CAirBurster_RightArm::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

#pragma endregion

#pragma  region 바하무트
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bahamut"),
				CBahamut::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

#pragma endregion

#pragma region 
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Rat"),
				CAnim_Rat::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Cat"),
				CAnim_Cat::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dog"),
				CAnim_Dog::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pigeon"),
				CAnim_Pigeon::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Chocobo"),
				CAnim_Chocobo::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
#pragma endregion Mob 

#pragma region 
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Man"),
				CAnim_Man::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Woman"),
				CAnim_Woman::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Biggs"),
				CAnim_Biggs::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ChocoboDriver"),
				CAnim_ChocoboDriver::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DanceManager"),
				CAnim_DanceManager::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GymMember"),
				CAnim_GymMember::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ItemShop"),
				CAnim_ItemShop::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_StationStaff"),
				CAnim_StationStaff::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WeaponShop"),
				CAnim_WeaponShop::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Wedge"),
				CAnim_Wedge::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;
#pragma endregion NPC

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Demo_PartsObject"),
				CDemo_Parts::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;


#pragma region INTERACTION_BOX
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_InteractionBox_Split"),
				CInteractionBox_Split::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_InteractionBox"),
				CInteractionBox::Create(m_pDevice, m_pContext))))
				RETURN_EFAIL;

#pragma endregion INTERACTION_BOX
		}


		lstrcpy(m_szLoadingText, TEXT("[TOOL] 맵 모델을 로드중입니다."));
		{
			m_pGameInstance->Load_DirectorySub_Models(L"Stage1/", true);
		}
		lstrcpy(m_szLoadingText, TEXT("[TOOL] 맵 모델 로딩이 완료되었습니다."));

	}

	lstrcpy(m_szLoadingText, TEXT("[TOOL] 로딩이 완료되었습니다."));

	return S_OK;
}

HRESULT CLoader::Ready_Effect_Texture()
{
#if EFFECT_LOAD == 0 || EFFECT_LOAD == 2

	if (FAILED(m_pGameInstance->Include_All_Textures(TEXT("Effect/"), true)))
		RETURN_EFAIL;

#ifdef _DEBUG
	if (FAILED(m_pGameInstance->Include_All_Textures(TEXT("Easing/"), true)))
		RETURN_EFAIL;
#endif

#endif // DEBUG

	return S_OK;
}

HRESULT CLoader::Ready_Effect_Model()
{

	if (FAILED(m_pGameInstance->Load_DirectorySub_Models(L"Effect/", true)))
		RETURN_EFAIL;

	return S_OK;
}

CLoader* CLoader::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, LEVEL eNextLevelID)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Created : CLoader");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteCriticalSection(&m_CriticalSection);

	DeleteObject(m_hThread);

	CloseHandle(m_hThread);

	CoUninitialize();

	Safe_Release(m_pGameInstance);
}
