#include "stdafx.h"
#include "..\Public\BoneEffectObject.h"

#include "RenderInstance.h"
#include "GameInstance.h"

#include "Character.h"

#include "Effect_Manager.h"

CBoneEffectObject::CBoneEffectObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CBoneEffectObject::CBoneEffectObject(const CBoneEffectObject& Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CBoneEffectObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBoneEffectObject::Initialize(void* pArg)
{

	if (nullptr == pArg)
		return E_FAIL;

	BONE_EFFECT_DESC* pDesc = static_cast<BONE_EFFECT_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	m_pSocketMatrix = pDesc->pSocketBoneMatrix;
	m_pParentMatrix = pDesc->pParentMatrix;

	

	//XMMATRIX matrixA = XMLoadFloat4x4(m_pSocketMatrix);
	//XMMATRIX matrixB = XMLoadFloat4x4(m_pParentMatrix);
	//XMMATRIX resultMatrix = matrixA * matrixB;
	//XMStoreFloat4x4(&m_tFinalMatrix, resultMatrix);
	//
	//
	//CEffect_Manager::Get_Instance()->Copy_Layer(pDesc->strEffectName, &m_tFinalMatrix);


	//XMMATRIX matrixA = XMLoadFloat4x4(m_pSocketMatrix);
	//XMMATRIX matrixB = XMLoadFloat4x4(m_pParentMatrix);
	//XMMATRIX resultMatrix = matrixA * matrixB;
	//XMStoreFloat4x4(&m_tFinalMatrix, resultMatrix);




	//따라가지 않는 경우
	XMMATRIX matrixA = XMLoadFloat4x4(m_pSocketMatrix);
	XMMATRIX matrixB = XMLoadFloat4x4(m_pParentMatrix);
	XMMATRIX resultMatrix = matrixA * matrixB;

	_float4x4 tFinalMatrix;
	XMStoreFloat4x4(&tFinalMatrix, resultMatrix);
	CEffect_Layer::COPY_DESC tDesc{};
	tDesc.pPlayertMatrix = &tFinalMatrix;
	CEffect_Manager::Get_Instance()->Copy_Layer(pDesc->strEffectName, &tDesc);




	return S_OK;
}



void CBoneEffectObject::Update(_float fTimeDelta)
{

	Destory();

	//_matrix		SocketBoneMatrix = XMLoadFloat4x4(m_pSocketMatrix);
	//
	//for (size_t i = 0; i < 3; i++)
	//	SocketBoneMatrix.r[i] = XMVector3Normalize(SocketBoneMatrix.r[i]);
	//
	//
	//XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketBoneMatrix * XMLoadFloat4x4(m_pParentMatrix));


}

void CBoneEffectObject::Late_Update(_float fTimeDelta)
{

	//UI아닐것
	//m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CBoneEffectObject::Render(_float fTimeDelta)
{

#ifdef _DEBUG
	//m_pColliderCom->Render(fTimeDelta);
#endif // DEBUG


	return S_OK;
}








CBoneEffectObject* CBoneEffectObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBoneEffectObject* pInstance = new CBoneEffectObject(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBoneEffectObject"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBoneEffectObject::Clone(void* pArg)
{
	CBoneEffectObject* pInstance = new CBoneEffectObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CBoneEffectObject"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoneEffectObject::Free()
{
	__super::Free();
}
