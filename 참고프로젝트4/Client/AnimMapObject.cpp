#include "stdafx.h"
#include "AnimMapObject.h"
#include "CommonModelComp.h"
#include "GameInstance.h"
#include "State_List_Cloud.h"

IMPLEMENT_CREATE(CAnimMapObject)
IMPLEMENT_CLONE(CAnimMapObject, CGameObject)

CAnimMapObject::CAnimMapObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CGameObject(pDevice, pContext)
{

}

CAnimMapObject::CAnimMapObject(const CAnimMapObject& rhs)
	: CGameObject(rhs)
{
}

HRESULT CAnimMapObject::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CAnimMapObject::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	if (FAILED(Ready_Components(pArg)))
		RETURN_EFAIL;

	return S_OK;
}

void CAnimMapObject::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);

	if (m_pModelCom != nullptr)
	{
		m_pModelCom->Set_Animation(0, 1.f, false);
		m_pModelCom->Tick_AnimTime(fTimeDelta);
		if (m_strModelTag == L"Fan1")
		{
			m_pModelCom->Set_Animation("Fan1|N_Rotation_1", 1.f, true);
			m_bIsAnimPlaying = true;
		}
		else if (m_strModelTag == L"Fan2")
		{
			m_pModelCom->Set_Animation("Fan2|N_Rotation_1", 1.f, true);
			m_bIsAnimPlaying = true;
		}

		if (m_pGameInstance->Get_CurrentLevelIndex() == LEVEL_STAGE3)
		{
			auto pObject = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_ANIMOBJECT, 0);
			pObject->Get_ModelCom().lock()->Set_Animation("Door2|N_Open_1", 1.f, false);
			dynamic_pointer_cast<CAnimMapObject>(pObject)->Set_IsAnimPlaying(true);
		}

	}
}


void CAnimMapObject::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	if (m_pGameInstance->Get_CurrentLevelIndex() == LEVEL_STAGE1)
	{
		if (m_bCollision && m_pGameInstance->Key_Down(DIK_E))
		{
			m_bIsAnimPlaying = true;

			if (m_strModelTag == L"TreasuerBoxN_Treasure01" || m_strModelTag == L"N_Treasure01")
			{
				if (m_bIsOpenBox == false) {
					random_device rd;
					mt19937 gen(rd());
					uniform_int_distribution<_int> distribution(1, 3);

					_int RandomGIL = distribution(gen);

					GET_SINGLE(CUI_Manager)->Make_EventLogPopUp(1, RandomGIL);
					GET_SINGLE(CUI_Manager)->Make_EventLogPopUp(3);

					m_bIsOpenBox = true;
				}
			}
		}
		if (m_strModelTag == L"Lever_Down" && m_bIsAnimPlaying && m_bLeverOnce)
		{
			auto pObject = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_PLAYER, 0);
			pObject->Get_StateMachineCom().lock()->Set_State<CState_Cloud_LeverDown>();
			pObject->Get_TransformCom().lock()->Set_Look(_float3(1.f, 0.f, 0.f), _float3(0.f, 1.f, 0.f));
			pObject->Get_TransformCom().lock()->Set_Position(1.f, _float4(85.3f, 2.6f, 64.94f, 1.f));
			m_bIsAnimPlaying = false;
			m_bLeverOnce = false;
		}
		else if (m_strModelTag == L"Lever_Down" && m_pModelCom->IsAnimation_Finished("Lever|N_Down_1"))
		{
			m_fTimeAcc += fTimeDelta;

			if (m_bFirst && m_fTimeAcc > 0.7f)
			{
				m_pGameInstance->Excute_LevelEvent(m_pGameInstance->Get_CreateLevelIndex(), "Door2_Open", shared_from_this());
				m_bFirst = false;
			}
		}


		if (m_strModelTag == L"Door1" && m_bIsAnimPlaying && m_pModelCom->IsAnimation_Frame_Once(1.f))
		{
			auto pObject_StateMachineCom = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_PLAYER, 0)->Get_StateMachineCom();
			pObject_StateMachineCom.lock()->Set_State<CState_Cloud_Switch>();
		}
		else if (m_strModelTag == L"Door1" && m_pModelCom->IsAnimation_Range(40, 60) && m_bDoor1)
		{
			auto pObject = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_COLLIDER, 207);
			if (pObject != nullptr)
				pObject->TurnOff_State(OBJSTATE::Active);
			m_bDoor1 = false;
		}


		if (m_strModelTag == L"Door2" && m_bIsAnimPlaying && m_pModelCom->IsAnimation_Frame_Once(1.f))
		{
			auto pObject_StateMachineCom = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_PLAYER, 0)->Get_StateMachineCom();
			pObject_StateMachineCom.lock()->Set_State<CState_Cloud_Switch>();
		}
		else if (m_strModelTag == L"Door2" && m_pModelCom->IsAnimation_Range(10, 20) && m_bDoor2)
		{
			auto pObject = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_COLLIDER, 208);
			if (pObject != nullptr)
				pObject->TurnOff_State(OBJSTATE::Active);
			m_bDoor2 = false;

			pObject = m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CreateLevelIndex(), L_MONSTER)->back();
			pObject->TurnOn_State(OBJSTATE::Active);
		}

		if (m_strModelTag == L"Door3" && m_bIsAnimPlaying && m_pModelCom->IsAnimation_Frame_Once(1.f))
		{
			auto pObject_StateMachineCom = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_PLAYER, 0)->Get_StateMachineCom();
			pObject_StateMachineCom.lock()->Set_State<CState_Cloud_Switch>();
		}
		else if (m_strModelTag == L"Door3" && m_pModelCom->IsAnimation_Frame_Once(15) && m_bDoor3_1)
		{
			auto pObject = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_COLLIDER, 209);
			if (pObject != nullptr)
				pObject->TurnOff_State(OBJSTATE::Active);
			m_bDoor3_1 = false;
		}

		else if (m_strModelTag == L"Door3" && m_pModelCom->IsAnimation_Frame_Once(15) && m_bDoor3_2)
		{
			auto pObject = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_COLLIDER, 210);
			if (pObject != nullptr)
				pObject->TurnOff_State(OBJSTATE::Active);
			m_bDoor3_2 = false;
		}


		if (m_strModelTag == L"Lift_Up" && m_bIsAnimPlaying && m_pModelCom->IsAnimation_Frame_Once(1.f))
		{

			auto pObject_StateMachineCom = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_PLAYER, 0)->Get_StateMachineCom();
			pObject_StateMachineCom.lock()->Set_State<CState_Cloud_Switch>();
		}

		else if (m_strModelTag == L"Lift_Up" && m_pModelCom->IsAnimation_Range(155, 230))
		{
			auto pObject = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_COLLIDER, 212);
			auto pPlayer = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_PLAYER, 0);
			pPlayer->Get_PhysXControllerCom().lock()->Enable_Gravity(false);
			if (m_pModelCom->IsAnimation_Range(160, 230))
				pObject->Get_TransformCom().lock()->Go_Up(fTimeDelta * 0.29f);
			pPlayer->Get_TransformCom().lock()->Go_Up(fTimeDelta * 0.305f);
		}
	}
	else if (m_pGameInstance->Get_CurrentLevelIndex() == LEVEL_STAGE2)
	{
		if (m_bCollision && m_pGameInstance->Key_Down(DIK_E))
		{
			m_bIsAnimPlaying = true;
		}

		if (m_strModelTag == L"Door3" && m_pModelCom->IsAnimation_Frame_Once(10))
		{
			auto pObject = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_COLLIDER, 317);
			auto pObject2 = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_COLLIDER, 318);
			if (m_pModelCom->Get_CurrentAnimationName() == "Main|N_Open_1")
			{
				pObject->TurnOff_State(OBJSTATE::Active);
				pObject2->TurnOff_State(OBJSTATE::Active);
			}
			else if (m_pModelCom->Get_CurrentAnimationName() == "Main|N_Close_1")
			{
				pObject->TurnOn_State(OBJSTATE::Active);
				pObject2->TurnOn_State(OBJSTATE::Active);
			}
		}
		if (m_strModelTag == L"Door4" && m_pModelCom->IsAnimation_Frame_Once(20))
		{
			auto pObject = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_COLLIDER, 316);
			if (m_pModelCom->Get_CurrentAnimationName() == "Main|N_Open_1")
				pObject->TurnOff_State(OBJSTATE::Active);
			else if(m_pModelCom->Get_CurrentAnimationName() == "Main|N_Close_1")
				pObject->TurnOn_State(OBJSTATE::Active);
		}
	}
}


void CAnimMapObject::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
	if (m_pModelCom->IsAnimation_Finished())
	{
		m_bIsAnimPlaying = false;
		m_bIsAnimPlayLever = false;
		m_bCollision = false;
	}
}

void CAnimMapObject::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_bIsAnimPlaying || m_bIsAnimPlayLever)
	{
		m_pModelCom->Tick_AnimTime(fTimeDelta);
	}

}

void CAnimMapObject::Before_Render(_cref_time fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND_CHARACTER, shared_from_this())))
		return;
}

HRESULT CAnimMapObject::Render()
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

HRESULT CAnimMapObject::Ready_Components(void* pArg)
{
	TMODEL_DESC Desc = {};

	if (nullptr != pArg)
	{
		Desc = (*ReCast<TMODEL_DESC*>(pArg));

		m_strModelTag = Desc.strModelTag;

		if (Desc.vPosition.x != 0.f && Desc.vPosition.y != 0.f && Desc.vPosition.z != 0.f)
		{
			m_vPosition = Desc.vPosition;

			m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vPosition);
		}

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_AnimCommonModel"),
			TEXT("Com_ModelCom"), &(m_pModelCom), nullptr)))
			RETURN_EFAIL;


		if (m_pModelCom)
		{
			if (FAILED(m_pModelCom->Link_Model(CCommonModelComp::TYPE_ANIM, m_strModelTag)))
				RETURN_EFAIL;
			if (FAILED(m_pModelCom->Link_Shader(L"Shader_AnimModel", VTXMESHANIM::Elements, VTXMESHANIM::iNumElements)))
				RETURN_EFAIL;
			m_pModelCom->Set_Animation(0, 1.f, false);
			m_pModelCom->Set_PreRotate(
				XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f)));
		}

	}
	return S_OK;
}

HRESULT CAnimMapObject::Bind_ShaderResources()
{
	// 모델 렌더
	if (m_pModelCom)
	{
		if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_VIEW))))
			RETURN_EFAIL;
		if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_PROJ))))
			RETURN_EFAIL;

		// 월드 행렬 바인딩
		if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_WorldMatrix", &m_pModelCom->Calculate_TransformFloat4x4FromParent())))
			RETURN_EFAIL;
	}

	return S_OK;
}

void CAnimMapObject::Write_Json(json& Out_Json)
{
	Out_Json["ModelTag"] = WstrToStr(m_strModelTag);

	__super::Write_Json(Out_Json);
}

void CAnimMapObject::Load_Json(const json& In_Json)
{
	m_strModelTag = StrToWstr(In_Json["ModelTag"]);

	__super::Load_Json(In_Json);

	TMODEL_DESC Desc = {};
	Desc.strModelTag = m_strModelTag;
	Desc.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	Ready_Components(&Desc);
}

void CAnimMapObject::Free()
{
	__super::Free();
}

