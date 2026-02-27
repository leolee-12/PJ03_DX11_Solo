#include "stdafx.h"
#include "Aerith/Aerith.h"
#include "Aerith/State/State_List_Aerith.h"
#include "Aerith/Parts/Aerith_Weapon.h"

#include "GameInstance.h"
#include "PhysX_Manager.h"
#include "Cloud_Weapon.h"
#include "Camera_Manager.h"
#include "StatusComp.h"

#include "Level_Test_Defines.h"

IMPLEMENT_CREATE(CAerith)
IMPLEMENT_CLONE(CAerith, CGameObject)

CAerith::CAerith(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: BASE(pDevice, pContext)
{
}

CAerith::CAerith(const CAerith& rhs)
	:BASE(rhs)
{
	m_eObjectType = OBJTYPE::PLAYER;
	m_ePlayerType = AERITH;
}

HRESULT CAerith::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;
	return S_OK;
}

HRESULT CAerith::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	if (FAILED(Ready_Components(pArg)))
		RETURN_EFAIL;

	if (FAILED(Ready_PartObjects()))
		RETURN_EFAIL;

	m_pStateMachineCom->Add_State<CState_Aerith_Idle>();
	m_pStateMachineCom->Add_State<CState_Aerith_Run>();
	m_pStateMachineCom->Add_State<CState_Aerith_Walk>();
	m_pStateMachineCom->Add_State<CState_Aerith_WalkB>();
	m_pStateMachineCom->Add_State<CState_Aerith_WalkL>();
	m_pStateMachineCom->Add_State<CState_Aerith_WalkR>();
	m_pStateMachineCom->Add_State<CState_Aerith_NormalAttack01>();
	m_pStateMachineCom->Add_State<CState_Aerith_NormalAttack02>();
	m_pStateMachineCom->Add_State<CState_Aerith_NormalAttack03>();
	m_pStateMachineCom->Add_State<CState_Aerith_NormalAttack04>();
	m_pStateMachineCom->Add_State<CState_Aerith_NormalAttack05>();
	m_pStateMachineCom->Add_State<CState_Aerith_SpecialAttack01>();
	m_pStateMachineCom->Add_State<CState_Aerith_SpecialAttack02>();
	m_pStateMachineCom->Add_State<CState_Aerith_SpecialAttack03>();
	m_pStateMachineCom->Add_State<CState_Aerith_SpecialAttack_Tempest>();
	m_pStateMachineCom->Add_State<CState_Aerith_AbilityAttack_RayOfJudge>();
	m_pStateMachineCom->Add_State<CState_Aerith_AbilityAttack_LustrousShield>();
	m_pStateMachineCom->Add_State<CState_Aerith_AbilityAttack_SorcerousStorm>();
	m_pStateMachineCom->Add_State<CState_Aerith_AbilityAttack_FlameSpell>();
	m_pStateMachineCom->Add_State<CState_Aerith_LimitAttack_GuardianOfGuns>();
	
	m_pStateMachineCom->Add_State<CState_Aerith_Avoid>();
	m_pStateMachineCom->Add_State<CState_Aerith_Abnormal>();
	m_pStateMachineCom->Add_State<CState_Aerith_Damage_Back>();
	m_pStateMachineCom->Add_State<CState_Aerith_Damage_Catch>();
	m_pStateMachineCom->Add_State<CState_Aerith_Damage_Front>();
	m_pStateMachineCom->Add_State<CState_Aerith_Die>();
	m_pStateMachineCom->Add_State<CState_Aerith_Event_PassOver>();
	m_pStateMachineCom->Add_State<CState_Aerith_Guard>();
	m_pStateMachineCom->Add_State<CState_Aerith_Item_Potion>();
	m_pStateMachineCom->Add_State<CState_Aerith_LadderDown>();
	m_pStateMachineCom->Add_State<CState_Aerith_LadderUp>();
	m_pStateMachineCom->Add_State<CState_Aerith_LadderDownFinished>();
	m_pStateMachineCom->Add_State<CState_Aerith_LadderUpFinished>();
	m_pStateMachineCom->Add_State<CState_Aerith_LeverDown>();
	m_pStateMachineCom->Add_State<CState_Aerith_Switch>();
	m_pStateMachineCom->Add_State<CState_Aerith_AI_AttackMode>();
	m_pStateMachineCom->Add_State<CState_Aerith_AI_MoveMode>();

	m_pStateMachineCom->Set_State<CState_Aerith_Idle>();

	GET_SINGLE(CCamera_Manager)->Set_CamType(CCamera_Manager::CAM_THIRD);

	//유진 추가//리미트 세팅
	m_pStatusCom->Init_LimitBreak(100.f);
	
	GET_SINGLE(CClient_Manager)->Register_Player(PLAYER_TYPE::AERITH, static_pointer_cast<CPlayer>(shared_from_this()));

	return S_OK;
}

void CAerith::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);

	
	//지우시면 안됩니다!!!!!!
#if LEVEL_STAGE1_TEST_MAP == 0
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float4{ -30.f, 5.f, 15.f,1.f });
#elif LEVEL_STAGE1_TEST_MAP == 1
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float4{ 0.f, 5.f, 1.f,1.f });
#elif LEVEL_STAGE1_TEST_MAP == 97
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float4{ 6.f, 5.f, -2.5f,1.f });
#elif LEVEL_STAGE1_TEST_MAP == 98
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float4{ 0.f, 5.f, 1.f,1.f });
#elif LEVEL_STAGE1_TEST_MAP == 99
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float4{ 84.f, 3.f, 84.2f,1.f });
#endif

	if (m_pModelCom)
	{
		PHYSXCONTROLLER_DESC ControllerDesc = {};
		PhysXControllerDesc::Setting_Controller(ControllerDesc, m_pTransformCom, CL_PLAYER_CONTROLLER, 1.0f, 0.3f);

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Controller"),
			TEXT("Com_PhysXControllerCom"), &(m_pPhysXControllerCom), &ControllerDesc)))
			return;

		m_pTransformCom->Set_PhysXControllerCom(m_pPhysXControllerCom);

		PHYSXCOLLIDER_DESC ColliderDesc = {};
		PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc,PHYSXCOLLIDER_TYPE::BOX, CL_PLAYER_BODY,m_pTransformCom,{ 0.3f,1.5f,0.3f }, false, nullptr, true);
		m_vPhysXColliderLocalOffset = { 0.f,0.9f,0.f };

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
			TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
			return;	
	}

	// 스탯 설정
#pragma TODO_MSG(임시 스테이터스 설정 나중에 데이터 매니저에 할당하고 다운로드 받는 방식으로 바꿀 것.)
	m_pStatusCom->Init_StatsByTable(CStatusComp::ETableType::Character, "Aerith");

	GET_SINGLE(CCamera_Manager)->Set_Target_ThirdPersonCamera(AERITH,m_pTransformCom);

	m_pModelCom->Set_Animation(0, 1.f, true);
}

void CAerith::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
	
	//테스트용
	if (m_pGameInstance->Key_Pressing(DIK_L))
	{
		m_pTransformCom->Go_Up(fTimeDelta);
		m_pPhysXControllerCom->Reset_Gravity();
	}	
	//if (m_pGameInstance->Key_Pressing(DIK_9))
	//{
	//	m_pStatusCom->Set_Damage(-1.f);
	//	GET_SINGLE(CClient_Manager)->Set_PlayerHP(AERITH, m_pStatusCom->Get_CurHP());
	//	GET_SINGLE(CClient_Manager)->Set_BattleMode(true);
	//}

	_float fDistanceX = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_RIGHT)));
	_float fDistanceY = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_UP)));
	_float fDistanceZ = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_LOOK)));
	if (fDistanceX < 0.9f || fDistanceY < 0.9f || fDistanceZ < 0.9f)
	{
		int a = 0;
	}

	if (m_pGameInstance->Key_Down(DIK_6))
	{
		m_pStatusCom->Set_HP_Full();
		m_pStatusCom->Add_LimitBreak(50);
	}
	if (m_pGameInstance->Key_Down(DIK_7))
	{
		m_pStatusCom->Set_CurHP(7648);
		m_pStatusCom->Add_LimitBreak(100);
	}
}

void CAerith::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	/*PxRaycastHit hit;
	if (GET_SINGLE(CPhysX_Manager)->RaycastSingle(CL_PLAYER_ATTACK, m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pTransformCom->Get_State(CTransform::STATE_LOOK), 100.f, hit))
	{		
		if (hit.actor->userData != nullptr)
		{
			CPhysX_Collider* pCollider = (CPhysX_Collider*)hit.actor->userData;
			int a = 0;
		}
	}*/

	//if (m_pGameInstance->Key_DownEx(DIK_Z, DIK_MD_LCTRL))
	//{
	//	m_pModelCom->Set_Animation(m_iTestAnimIndex++, 1.f, 1.f, false, 0.1f, true);
	//}
	//else if (m_pGameInstance->Key_DownEx(DIK_Z, DIKK_MD_LCTRL_SHIFT))
	//	m_pModelCom->Set_Animation(m_iTestAnimIndex++, 1.f, 1.f, false, 0.1f, false);

	//if (m_pGameInstance->Key_DownEx(DIK_X, DIK_MD_LCTRL))
	//{
	//	if (m_iTestAnimIndex > 0)
	//		m_pModelCom->Set_Animation(m_iTestAnimIndex--, 1.f, 1.f, false, 0.1f, true);
	//}
	//else if (m_pGameInstance->Key_DownEx(DIK_X, DIKK_MD_LCTRL_SHIFT))
	//{
	//	if (m_iTestAnimIndex > 0)
	//		m_pModelCom->Set_Animation(m_iTestAnimIndex--, 1.f, 1.f, false, 0.1f, false);
	//}
	_float fDistanceX = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_RIGHT)));
	_float fDistanceY = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_UP)));
	_float fDistanceZ = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_LOOK)));
	if (fDistanceX < 0.9f || fDistanceY < 0.9f || fDistanceZ < 0.9f)
	{
		int a = 0;
	}
}

void CAerith::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	_float fDistanceX = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_RIGHT)));
	_float fDistanceY = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_UP)));
	_float fDistanceZ = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_LOOK)));
	if (fDistanceX < 0.9f || fDistanceY < 0.9f || fDistanceZ < 0.9f)
	{
		int a = 0;
	}
}

void CAerith::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND_CHARACTER, shared_from_this())))
		return;
}

void CAerith::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CAerith::Render()
{
	if (m_pModelCom)
	{
		if (FAILED(Bind_ShaderResources()))
			RETURN_EFAIL;

		auto ActiveMeshes = m_pModelCom->Get_ActiveMeshes();
		for (_uint i = 0; i < ActiveMeshes.size(); ++i)
		{
			// 뼈 바인딩
			if (FAILED(m_pModelCom->Bind_BoneToShader(ActiveMeshes[i], "g_BoneMatrices")))
				RETURN_EFAIL;

			// 텍스처 바인딩
			if (FAILED(m_pModelCom->Bind_MeshMaterialToShader(ActiveMeshes[i], TextureType_DIFFUSE, "g_DiffuseTexture")))
				RETURN_EFAIL;
			if (FAILED(m_pModelCom->Bind_MeshMaterialToShader(ActiveMeshes[i], TextureType_NORMALS, "g_NormalTexture")))
				RETURN_EFAIL;
			if (FAILED(m_pModelCom->Bind_MeshMaterialToShader(ActiveMeshes[i], TextureType_DIFFUSE_ROUGHNESS, "g_MRVTexture")))
				RETURN_EFAIL;
			if (FAILED(m_pModelCom->Bind_MeshMaterialToShader(ActiveMeshes[i], TextureType_METALNESS, "g_MTexture")))
				RETURN_EFAIL;
			if (FAILED(m_pModelCom->Bind_MeshMaterialToShader(ActiveMeshes[i], TextureType_EMISSION_COLOR, "g_EmissiveTexture")))
				RETURN_EFAIL;
			if (FAILED(m_pModelCom->Bind_MeshMaterialToShader(ActiveMeshes[i], TextureType_AMBIENT_OCCLUSION, "g_AOTexture")))
				RETURN_EFAIL;
			if (FAILED(m_pModelCom->Bind_MeshMaterialToShader(ActiveMeshes[i], TextureType_OPACITY, "g_AlphaTexture")))
				RETURN_EFAIL;

			// 패스, 버퍼 바인딩
			if (FAILED(m_pModelCom->ShaderComp()->Begin(0)))
				RETURN_EFAIL;
			//버퍼 렌더
			if (FAILED(m_pModelCom->BindAndRender_Mesh(ActiveMeshes[i])))
				RETURN_EFAIL;
		}
	}
	return S_OK;
}

HRESULT CAerith::Ready_Components(void* pArg)
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_AnimCommonModel"),
		TEXT("Com_ModelCom"), &(m_pModelCom), nullptr)))
		RETURN_EFAIL;

	if (m_pModelCom)
	{
		m_pModelCom->Link_Model(CCommonModelComp::TYPE_ANIM, TEXT("PC_0003_00_Aerith"));
		if (FAILED(m_pModelCom->Link_Shader(L"Shader_AnimModel", VTXMESHANIM::Elements, VTXMESHANIM::iNumElements)))
			RETURN_EFAIL;

		m_pModelCom->Set_PreRotate(
			XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f)));

		// 노티파이 프로토타입 클로닝 + 모델 컴포넌트 전달(Weak)
		if (FAILED(m_pModelCom->Clone_NotifyFromPrototype(TEXT("Prototype_Component_Notify_Aerith"))))
			RETURN_EFAIL;
		m_pModelCom->Regist_ModelCompToNotify(m_pModelCom);
	}

	return S_OK;
}

HRESULT CAerith::Bind_ShaderResources()
{	
	// 공통 행렬 바인딩
	if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_ViewMatrix",&m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_VIEW))))
		RETURN_EFAIL;
	if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_ProjMatrix",&m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_PROJ))))
		RETURN_EFAIL;
			
	// 월드 행렬 바인딩
	if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_WorldMatrix",&m_pTransformCom->Get_WorldFloat4x4())))
		RETURN_EFAIL;
		
	return S_OK;
}

HRESULT CAerith::Ready_PartObjects()
{
	/* For.Part_Weapon*/
	CPartObject::PARTOBJECT_DESC Desc;
	Desc.pParentTransform = m_pTransformCom;
	Desc.pBoneGroup = m_pModelCom->Get_BoneGroup();
	Desc.strBoneName = TEXT("R_Weapon_a");

	if (FAILED(Add_PartObject<CAerith_Weapon>(TEXT("Prototype_GameObject_Aerith_Weapon"), TEXT("Part_Weapon"), &Desc, nullptr)))
		RETURN_EFAIL;


	return S_OK;
}

void CAerith::OnCollision_Enter(CCollider* pThisCol, CCollider* pOtherCol)
{
	__super::OnCollision_Enter(pThisCol, pOtherCol);
}

void CAerith::OnCollision_Stay(CCollider* pThisCol, CCollider* pOtherCol)
{
	__super::OnCollision_Stay(pThisCol, pOtherCol);
}

void CAerith::OnCollision_Exit(CCollider* pThisCol, CCollider* pOtherCol)
{
	__super::OnCollision_Exit(pThisCol, pOtherCol);
}

void CAerith::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);

	if (pOtherCol->Get_ColliderDesc().iFilterType == CL_MONSTER_ATTACK || pOtherCol->Get_ColliderDesc().iFilterType == CL_LASER)
	{
		CState* pCurState = m_pStateMachineCom->Get_CurState();
		if(typeid(*pCurState) != typeid(CState_Aerith_Avoid)
			&& typeid(*pCurState) != typeid(CState_Aerith_AbilityAttack_RayOfJudge) 
			&& typeid(*pCurState) != typeid(CState_Aerith_AbilityAttack_SorcerousStorm) 
			&& typeid(*pCurState) != typeid(CState_Aerith_AbilityAttack_LustrousShield)
			&& typeid(*pCurState) != typeid(CState_Aerith_LimitAttack_GuardianOfGuns)
			&& typeid(*pCurState) != typeid(CState_Aerith_Damage_Front)
			&& typeid(*pCurState) != typeid(CState_Aerith_Damage_Back))
		{
			PxContactPairPoint* pContactPoint = new PxContactPairPoint[ContactInfo.contactCount];
			PxU32 nbContacts = ContactInfo.extractContacts(pContactPoint, ContactInfo.contactCount);

			_vector vHitDir = XMVector3Normalize(XMVectorSetY(Convert_Vector(pContactPoint[0].normal), 0.f));
			_vector vLookDir = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

			if (XMVectorGetX(XMVector3Length(vHitDir - vLookDir)) <= XMVectorGetX(XMVector3Length(vHitDir + vLookDir)))
			{
				m_pStateMachineCom->Set_State<CState_Aerith_Damage_Front>();
			}
			else
			{
				m_pStateMachineCom->Set_State<CState_Aerith_Damage_Back>();
			}

			Safe_Delete(pContactPoint);
		} 
	}
}

void CAerith::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);

	
}

void CAerith::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}

void CAerith::PhysX_Raycast_Stay(weak_ptr<CGameObject> pOther, _uint iOtherColLayer, _float4 vOrigin, _float4 vDirection, _float4 vColPosition)
{
	if (iOtherColLayer == CL_MONSTER_ATTACK)
	{
		CState* pCurState = m_pStateMachineCom->Get_CurState();
		if (typeid(*pCurState) != typeid(CState_Aerith_Avoid)
			&& typeid(*pCurState) != typeid(CState_Aerith_AbilityAttack_RayOfJudge)
			&& typeid(*pCurState) != typeid(CState_Aerith_AbilityAttack_SorcerousStorm)
			&& typeid(*pCurState) != typeid(CState_Aerith_AbilityAttack_LustrousShield)
			&& typeid(*pCurState) != typeid(CState_Aerith_LimitAttack_GuardianOfGuns)
			&& typeid(*pCurState) != typeid(CState_Aerith_Damage_Front)
			&& typeid(*pCurState) != typeid(CState_Aerith_Damage_Back))
		{
			_vector vHitDir = XMVector3Normalize(XMVectorSetY(vDirection, 0.f));
			_vector vLookDir = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
			if (XMVectorGetX(XMVector3Length(vHitDir - vLookDir)) <= XMVectorGetX(XMVector3Length(vHitDir + vLookDir)))
				m_pStateMachineCom->Set_State<CState_Aerith_Damage_Front>();
			else
				m_pStateMachineCom->Set_State<CState_Aerith_Damage_Back>();

		}
	}
		
}

void CAerith::Status_Damaged(_uint iStatusDamaged, _uint iHitPower, _uint iAddHitPower)
{
	m_pStateMachineCom->Find_State<CState_Aerith_Damage_Front>()->Set_DamageType(iHitPower);
	m_pStateMachineCom->Find_State<CState_Aerith_Damage_Back>()->Set_DamageType(iHitPower);

	CState* pCurState = m_pStateMachineCom->Get_CurState();
	if (typeid(*pCurState) != typeid(CState_Aerith_Avoid)
		&& typeid(*pCurState) != typeid(CState_Aerith_AbilityAttack_RayOfJudge)
		&& typeid(*pCurState) != typeid(CState_Aerith_AbilityAttack_SorcerousStorm)
		&& typeid(*pCurState) != typeid(CState_Aerith_AbilityAttack_LustrousShield)
		&& typeid(*pCurState) != typeid(CState_Aerith_LimitAttack_GuardianOfGuns)
		&& typeid(*pCurState) != typeid(CState_Aerith_Damage_Front)
		&& typeid(*pCurState) != typeid(CState_Aerith_Damage_Back))
	{
		switch (iHitPower)
		{
		case 5:
			m_pModelCom->AnimationComp()->Set_ADD_Animation(0, L"Battle00|B_Dmg01_Add", L"C_Spine_a", { L"C_Spine_c" }
			, 4, 1.f, 1.f, false);
		case 0:
			m_pStateMachineCom->Set_NextState(m_pStateMachineCom->Get_CurState());
			break;
		defualt:
			break;
		}
	}
}

void CAerith::Set_State_AIMode(_bool bAIMode)
{
	if (bAIMode)
	{
		if(GET_SINGLE(CClient_Manager)->Get_AttackMode())
			m_pStateMachineCom->Set_State<CState_Aerith_AI_AttackMode>();
		else
			m_pStateMachineCom->Set_State<CState_Aerith_AI_MoveMode>();
	}
	else
		m_pStateMachineCom->Wait_State<CState_Aerith_Idle>();
}

void CAerith::Free()
{
	__super::Free();
}