#pragma once
#include "stdafx.h"
#include "..\Public\Effect.h"
#include "GameInstance.h"
#include "IMGUI_Shader_Tab.h"
#include "Imgui_Manager.h"
#include "Effect_Animation.h"

CEffect::CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice , pContext }
{
}

CEffect::CEffect(const CEffect& Prototype)
	:CGameObject{ Prototype }
{
	m_pAnimation = (Prototype.m_pAnimation)->Clone();
}

HRESULT CEffect::Initialize_Prototype()
{
	m_pAnimation = CEffect_Animation::Create();
	if (nullptr == m_pAnimation)
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC tDesc{};
	tDesc.fRotationPerSec = XMConvertToRadians(90.f);
	m_iObjectRenderData = 2;
	if (FAILED(__super::Initialize(&tDesc)))
		return E_FAIL;


	return S_OK;
}

void CEffect::Camera_Update(_float fTimeDelta)
{

}

void CEffect::Update(_float fTimeDelta)
{

}

void CEffect::Late_Update(_float fTimeDelta)
{
	//m_fAccTime += fTimeDelta;
}

HRESULT CEffect::Priority_Render(_float fTimeDelta)
{
	if (m_bIsCopy == true)
	{
		_int iCheckSpriteEnd = static_cast<CIMGUI_Shader_Tab*>(CImgui_Manager::Get_Instance()
			->Access_Shader_Tab(m_iUnique_Index))
			->Update_Clone_EffectToShader_Texture(this, fTimeDelta);

		if (iCheckSpriteEnd == 1)
		{
			if(!m_bIsShaderLoop)
				m_bIsSpriteEnd = true;
		}
		else
			int a = 0;
	}
	return S_OK;
}

HRESULT CEffect::Render(_float fTimeDelta)
{

	
	return S_OK;
}

void CEffect::Add_KeyFrame(_uint KeyFrameNumber, EFFECT_KEYFRAME NewKeyFrame)
{
	m_pAnimation->Add_KeyFrame(KeyFrameNumber, NewKeyFrame);
}

void CEffect::Delete_KeyFrame(_uint KeyFrameNumber)
{
	m_pAnimation->Delete_KeyFrame(KeyFrameNumber);
}

_bool CEffect::Find_KeyFrame(_uint KeyFrameNumber)
{
	return m_pAnimation->Find_KeyFrame(KeyFrameNumber);
}

EFFECT_KEYFRAME CEffect::Get_KeyFrame(_uint KeyFrameNumber)
{
	EFFECT_KEYFRAME KeyFrame = m_pAnimation->Get_KeyFrame(KeyFrameNumber);

	Set_Effect_Scaled(KeyFrame.vScale);
	Set_Effect_Position(KeyFrame.vPosition);
	Set_Effect_Rotation(KeyFrame.vRotation);

	EFFECT_KEYFRAME ResultKeyFrame;

	ResultKeyFrame.vPosition.x = XMVectorGetX(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	ResultKeyFrame.vPosition.y = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	ResultKeyFrame.vPosition.z = XMVectorGetZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	ResultKeyFrame.vScale.x = GetVectorLength(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));
	ResultKeyFrame.vScale.y = GetVectorLength(m_pTransformCom->Get_State(CTransform::STATE_UP));
	ResultKeyFrame.vScale.z = GetVectorLength(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

	ResultKeyFrame.vRotation = m_pTransformCom->Get_Rotation();

	ResultKeyFrame.bIsNotPlaying = KeyFrame.bIsNotPlaying;
	ResultKeyFrame.fCurTime = KeyFrame.fCurTime;
	ResultKeyFrame.fDuration = KeyFrame.fDuration;

	m_pTransformCom->Set_Matrix(m_LayerMatrix);

	return ResultKeyFrame;
}

void CEffect::Set_ParentMatrixMultiply_LocalMatrix()
{
	_float4x4 LayerMatrix;
	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();

	//LayerMatrix * 
	_matrix TestMatrix = WorldMatrix * m_LayerMatrix;
	XMStoreFloat4x4(&LayerMatrix, TestMatrix);
	//m_pTransformCom->Set_WorldMatrix(LayerMatrix);
	
	XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix* m_LayerMatrix);
	//m_pTransformCom->Set_Matrix(TestMatrix);
//	m_pTransformCom->Set_Matrix(m_pTransformCom->Get_WorldMatrix_Inverse());
	//m_pTransformCom->Set_Matrix(m_LayerMatrix);

}

EFFECT_KEYFRAME CEffect::Get_Near_Front_KeyFrame(_uint frameNumber)
{
	return m_pAnimation->Get_Near_Front_KeyFrame(frameNumber);
}

void CEffect::Set_Effect_Scaled(_float3 ChangeScaled)
{
	m_pTransformCom->Set_Scaled(ChangeScaled.x, ChangeScaled.y, ChangeScaled.z);
}

void CEffect::Set_Effect_Position(_float3 ChangePosition)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(ChangePosition.x, ChangePosition.y, ChangePosition.z, 1.f));
}

void CEffect::Set_Effect_Rotation(_float3 ChangeRotation)
{
	m_pTransformCom->Rotate(ChangeRotation);
}

void CEffect::Set_Effect_IsNotPlaying(_bool bIsNotPlaying)
{
	m_bIsNotPlaying = bIsNotPlaying;
}

_float3 CEffect::Get_Effect_Scaled()
{
	return m_pTransformCom->Get_Scaled();
}

_float3 CEffect::Get_Effect_Position()
{
	_float3 Position;

	Position.x = XMVectorGetX(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	Position.y = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	Position.z = XMVectorGetZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return Position;
}

_float3 CEffect::Get_Effect_Rotation()
{
	return m_pTransformCom->Get_Rotation();
}

HRESULT CEffect::Play_Animation(_float CurrentFrame)
{
	if (m_pAnimation == nullptr)
		return E_FAIL;


	EFFECT_KEYFRAME ResultKeyFrame = m_pAnimation->Play_Animation(CurrentFrame, m_bIsLoop);

	if (ResultKeyFrame.bIsNotPlaying == true)
	{
		m_bIsNotPlaying = true;
		return S_OK;
	}
	else
	{
		m_bIsNotPlaying = false;
	}

	Set_Effect_Scaled(ResultKeyFrame.vScale);
	Set_Effect_Position(ResultKeyFrame.vPosition);
	Set_Effect_Rotation(ResultKeyFrame.vRotation);
	

	//_float fOriginZ = XMVectorGetZ(m_pTransformCom->Get_WorldMatrix().r[3]);
	//_float fOriginY = XMVectorGetY(m_pTransformCom->Get_WorldMatrix().r[3]);
	

	//XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * m_LayerMatrix);


	if (m_bIsBillboarding  == true && m_bIsAlreadyBillboading == false)
	{
		XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * m_LayerMatrix);
		m_pTransformCom->Set_WorldMatrix(m_WorldMatrix);
		m_pTransformCom->LookAt(m_pGameInstance->Get_CamPosition_Vector());
		XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());
	}
	else
	{
		XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * m_LayerMatrix);
	}

	m_bIsAlreadyBillboading = false;

	//m_WorldMatrix._43 = fOriginZ;
	//m_WorldMatrix._42 = fOriginY;
	
	return S_OK;
}

void CEffect::Set_Effect_Color(_float4 vColor)
{
	m_vColor = vColor;
}

void CEffect::Get_Layer_Matrix(_matrix LayerMatrix)
{
	m_LayerMatrix = LayerMatrix;
}

void CEffect::Set_Layer_Matrix()
{
	m_pTransformCom->Set_Matrix(m_LayerMatrix);
}

HRESULT CEffect::Ready_Components(_wstring* pModelName, _wstring* pMaskTextureName, _wstring* pDiffuseTexturueName)
{
	return S_OK;
}

HRESULT CEffect::Bind_ShaderResources()
{


	return S_OK;
}

CEffect* CEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect* pInstance = new CEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect::Clone(void* pArg)
{
	CEffect* pInstance = new CEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pDiffuseTextureCom);
	Safe_Release(m_pMaskTextureCom);
	//Safe_Release(m_pAnimation);
}


