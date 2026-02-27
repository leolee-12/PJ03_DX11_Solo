#include "stdafx.h"
#include "..\Public\Opening_Kririn.h"

#include "RenderInstance.h"
#include "GameInstance.h"

#include "AttackObject.h"
#include "AttackObject_Grab.h"
#include "AttackObject_CommandGrab.h"
#include "AttackObject_Ranged.h"

#include "AttackObject_Energy.h"

#include "AttackObject_Reflect.h"

#include "UI_Manager.h"
#include "iostream"


#include "BattleInterface.h"


#include "Main_Camera.h"

#include "BoneEffectObject.h"
#include "Effect_Manager.h"	

#include "Effect_Layer.h"

#include "Animation.h"
#include "Main_Camera.h"

#include "QTE_Manager.h"

COpening_Kririn::COpening_Kririn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCharacter{ pDevice, pContext }
{

}

COpening_Kririn::COpening_Kririn(const COpening_Kririn& Prototype)
	: CCharacter{ Prototype }
{

}

HRESULT COpening_Kririn::Initialize_Prototype()
{
	//m_pFrameEvent = CFrameEvent_Manager::Get_Instance()->Get_pFrameEventMap();
	//CFrameEvent_Manager::Get_Instance()->Initalize_NameMap();
//	CFrameEvent_Manager::Get_Instance()->LoadFile2("../Bin/FrameEventData/EventData_Hit.txt");

	return S_OK;
}

HRESULT COpening_Kririn::Initialize(void* pArg)
{

	m_fAIrGrabEndAnimationPositon = 39.99f;

	m_eCharacterIndex = OPENING_KRN;
	m_iFallAnimationIndex =			 1000;
	m_iIdleAnimationIndex = 1000;
	m_iStandingMidAttackAnimationIndex = 1000;
	m_iChaseAnimationIndex = 1000;
	m_iForwardDashAnimationIndex = 1000;

	m_iForwardDashEndAnimationIndex = { 1000 };

	m_iCrouchAnimationIndex = { 1000 };
	m_iBackWalkAnimationIndex = { 1000 };
	m_iForwardWalkAnimationIndex = { 1000 };
	m_iHit_Away_LeftAnimationIndex = { 1000 };
	m_iHit_Away_UpAnimationIndex = { 1000 };
	m_iHit_Away_LeftDownAnimationIndex = { 1000 };

	m_iHit_WallBouce = { 1000 };

	m_iHit_Air_LightAnimationIndex = { 1000 };
	m_iHit_Air_FallAnimationIndex = { 1000 };

	m_iHit_Air_Spin_LeftUp = { 1000 };
	m_iHit_Air_Spin_Up = { 1000 };

	m_iAttack_Air1 = { 1000 };
	m_iAttack_Air2 = { 1000 };
	m_iAttack_Air3 = { 1000 };
	m_iAttack_AirUpper = { 1000 };

	m_iAttack_LightLast = { 1000 };
	m_iAttack_Heavy = { 1000 };
	m_iAttack_Crouch_Heavy = { 1000 };

	m_iBound_Ground = { 1000 };
	m_iLayUp = { 1000 };

	m_iGuard_GroundAnimationIndex = { 1000 };
	m_iGuard_CrouchAnimationIndex = { 1000 };
	m_iGuard_AirAnimationIndex = { 1000 };

	m_iGrabReadyAnimationIndex = { 1000 };
	m_iGrabAnimationIndex = { 1000 };




	m_iJumpAnimationIndex = { 1000 };
	m_iHit_Stand_LightAnimationIndex = { 1000 };
	m_iHit_Stand_MediumAnimationIndex = { 1000 };
	m_iHit_Crouch_AnimationIndex = { 1000 };
	m_iBreakFall_Ground = { 1000 };   //100
	m_iBreakFall_Air = { 1000 };   //101


	m_iSparkingAnimationIndex = { 1000 };
	m_iDyingStandingAnimationIndex = { 1000 };

	m_iReflectAnimationIndex = { 1000 };


	m_iStartAnimatonIndex = { 1000 }; //600cs 
	m_iWinAnimationIndex = { 1000 };	//610cs
	m_iNextRound_RightHandAppear_Cutscene_AnimationIndex = { 1000 }; //620c
	m_iNextRound_RightHand_AnimationIndex = { 1000 };  //621cs ->631으로 연계

	m_iNextRound_LeftHand_Cutscene_AnimationIndex = { 1000 };  //630 Durtaio
	m_iNextRound_LeftHand_AnimationIndex = { 1000 };  //631 Durtaion 24


	m_iNextAnimation.first = ANIME_IDLE;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	Character_DESC* pDesc = static_cast<Character_DESC*>(pArg);
	m_strName = "HIT_" + to_string(m_iPlayerTeam) + "_" + to_string(pDesc->ePlayerSlot);
	m_RendererDesc.strName = m_strName;
	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;

	//LightDesc.vDirection = _float4(-0.15f, -0.7f, 0.5f, 0.f);
	LightDesc.vDirection = _float4(-1.f, -0.2f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.0f, 1.0f);
	LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
	LightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 1.f);
	LightDesc.vAuraColor = _float4(16.76, 1.5333, 27.86, 5.490);
	LightDesc.pPlayerDirection = &m_iLookDirection;
	LightDesc.strName = m_strName;

	if (FAILED(m_pRenderInstance->Add_Player_Light(m_strName, LightDesc, _float4(2.f, 1.10196f, 1.73333f, 1.f), &m_bChase)))
		return E_FAIL;
	
	m_pModelCom->SetUp_Animation(0, true);
	m_pModelCom->Play_Animation(0.f);


	m_pModelCom->Set_MaxAnimationUpdate_Time(0.05);
	m_pModelCom->Get_pCurrentAnimation()->m_fTickPerSecond = 30.f;

	
	
	Set_AnimationStop(2.f);


	m_bInvisible = true;

	m_eCharacterID = CUI_Define::PLAYER_ID::PAWN_END;


	return S_OK;
}

void COpening_Kririn::Player_Update(_float fTimeDelta)
{

	if (m_pGameInstance->Key_Down(DIK_F1))
	{
		m_bDebugInputLock = !m_bDebugInputLock;
	}


	if (m_bDebugInputLock)
		return;


	//m_pModelCom->Play_Animation_Lick(fTimeDelta);

	if (m_bAnimationLock == true)
		Update_AnimationLock(fTimeDelta);
	else
	{
	
		m_pModelCom->Play_Animation_Lick(fTimeDelta);
	}





}

void COpening_Kririn::Camera_Update(_float fTimeDelta)
{

}

void COpening_Kririn::Update(_float fTimeDelta)
{
	__super::Player_Update(fTimeDelta);

}

_int a = 0;
void COpening_Kririn::Late_Update(_float fTimeDelta)
{
	//__super::Late_Update(fTimeDelta);


	m_pRenderInstance->Add_RenderObject(CRenderer::RG_PLAYER, this, &m_RendererDesc);

	if (m_pGameInstance->Key_Down(DIK_1))
	{
		a++;
		if (a > 5)
			a = 0;
	}
}

HRESULT COpening_Kririn::Render(_float fTimeDelta)
{
	if (m_bInvisible == true)
		return S_OK;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
	/*
			CTexture* m_pDiffuseTextureCom = { nullptr };
	CTexture* m_pDecalTextureCom = { nullptr };
	CTexture* m_pIlmTextureCom = { nullptr };
	*/
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (i == 1 || i == 2 || i == 4)
			continue;

		_int iPassIndex = 0;

		if (i == 0)
		{
			if (FAILED(m_pDiffuseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
				return E_FAIL;

			if (FAILED(m_pIlmTextureCom->Bind_ShaderResource(m_pShaderCom, "g_OutLineTexture", 0)))
				return E_FAIL;
		}
		else if (i == 3)
		{

			if (FAILED(m_pDecalTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DecalTexture", 0)))
				return E_FAIL;
			iPassIndex = 4;
		}

		//m_pModelCom->Bind_MaterialSRV(m_pShaderCom, aiTextureType_NORMALS, "g_NormalTexture", i);

	   /* 모델이 가지고 있는 뼈들 중에서 현재 렌더링할려고 했던 i번째ㅑ 메시가 사용하는 뼈들을 배열로 만들어서 쉐이더로 던져준다.  */
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pShaderCom->Begin(iPassIndex)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;

	}


return E_FAIL;
	//}
#ifdef _DEBUG
	m_pColliderCom->Render(fTimeDelta);
#endif // DEBUG

	return S_OK;
}








void COpening_Kririn::Set_Delete()
{
	m_bInvisible = true;
	m_bDead = true;
	Destory();
}

HRESULT COpening_Kririn::Ready_Components()
{
	
	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_KRN"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_KRN_ilm"),
		TEXT("Com_IlmTexture"), reinterpret_cast<CComponent**>(&m_pIlmTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_KRN_decal"),
		TEXT("Com_DecalTexture"), reinterpret_cast<CComponent**>(&m_pDecalTextureCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_KRN_base"),
		TEXT("Com_DiffuseTexture"), reinterpret_cast<CComponent**>(&m_pDiffuseTextureCom))))
		return E_FAIL;

	/*


	*/

	return S_OK;
}

HRESULT COpening_Kririn::Bind_ShaderResources()
{
	
		if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
			return E_FAIL;


	return S_OK;
}




_short COpening_Kririn::Check_bCurAnimationisCanChase()
{
	

	return 0;

}

void COpening_Kririn::Reset_AttackCount()
{

	__super::Reset_AttackCount();

	for (size_t i = 0; i < COUNT_END; i++)
		m_bAttackCount[i] = true;


}

void COpening_Kririn::Gravity(_float fTimeDelta)
{
	
	;

}

void COpening_Kririn::AttackEvent(_int iAttackEvent, _int AddEvent)
{
	
}

void COpening_Kririn::Play_Sound(_uint SoundName, _bool bisLoof, _float fvolume)
{
	m_pGameInstance->Play_Sound((CSound_Manager::SOUND_KEY_NAME)SoundName, bisLoof, fvolume);
}

void COpening_Kririn::Play_Group_Sound(_uint groupKey, _bool loop, _float volume)
{
	m_pGameInstance->Play_Group_Sound((CSound_Manager::SOUND_GROUP_KEY)groupKey, loop, volume);
}




void COpening_Kririn::Add_YellowLight()
{
	LIGHT_DESC			LightDesc{};

	ZeroMemory(&LightDesc, sizeof(LIGHT_DESC));
	LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	LightDesc.vPosition = _float4(0.f, 0.f, 0.f, 1.f);
	LightDesc.fRange = 30.f;
	LightDesc.vDiffuse = _float4(1.2f, 1.15f, 0.7f, 1.0f);
	//LightDesc.vDiffuse = _float4(1.0f, 0.f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(0.1f, 0.1f, 0.1f, 1.f);
	LightDesc.vSpecular = _float4(1.0f, 0.95f, 0.45f, 1.f);

	LightDesc.fAccTime = 0.f;
	LightDesc.fLifeTime = 1.f;
	LightDesc.strName = "Explosion";
	if (FAILED(m_pRenderInstance->Add_Effect_Light(LightDesc.strName, LightDesc)))
		return;
}

void COpening_Kririn::Add_BlueLight()
{
	LIGHT_DESC			LightDesc{};

	ZeroMemory(&LightDesc, sizeof(LIGHT_DESC));
	LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	LightDesc.vPosition = _float4(0.f, 0.f, 0.f, 1.f);
	LightDesc.fRange = 30.f;

	LightDesc.vDiffuse = _float4(0.9f, 1.1f, 1.7f, 1.0f); // 파란빛 계열로 변경

	LightDesc.vAmbient = _float4(0.1f, 0.1f, 0.1f, 1.f);
	LightDesc.vSpecular = _float4(1.0f, 0.95f, 0.45f, 1.f);

	LightDesc.fAccTime = 0.f;
	LightDesc.fLifeTime = 2.7f;
	LightDesc.strName = "Ray";
	if (FAILED(m_pRenderInstance->Add_Effect_Light(LightDesc.strName, LightDesc)))
		return;
}







COpening_Kririn* COpening_Kririn::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	COpening_Kririn* pInstance = new COpening_Kririn(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : COpening_Kririn"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* COpening_Kririn::Clone(void* pArg)
{
	COpening_Kririn* pInstance = new COpening_Kririn(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : COpening_Kririn"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COpening_Kririn::Free()
{
	__super::Free();


	//Safe_Release(m_pShaderCom);
	//Safe_Release(m_pModelCom);

	Safe_Release(m_pIlmTextureCom);
	Safe_Release(m_pDecalTextureCom);
	Safe_Release(m_pIlmTextureCom);
	Safe_Release(m_pDiffuseTextureCom);
	Safe_Release(m_p2PTextureCom);
	Safe_Release(m_p1PTextureCom);

}
