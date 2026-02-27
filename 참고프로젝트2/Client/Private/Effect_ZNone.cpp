#pragma once
#include "stdafx.h"
#include "Effect_ZNone.h"
#include "GameInstance.h"
#include "RenderInstance.h"
#include "Effect_Animation.h"

CEffect_ZNone::CEffect_ZNone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice ,pContext }
{
}

CEffect_ZNone::CEffect_ZNone(const CEffect_ZNone& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CEffect_ZNone::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_ZNone::Initialize(void* pArg)
{
	m_eEffect_Type = EFFECT_ZNONE;
	//m_iPassIndex = 6;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		EFFECT_DESC* pEffectDesc = static_cast<EFFECT_DESC*>(pArg);
		m_ForCopyInform = *pEffectDesc;

		m_bIsCopy = pEffectDesc->bIsCopy;

		_float3 vPos = pEffectDesc->vPosition;
		_float3 vScale = pEffectDesc->vScaled;
		_float3 vRot = pEffectDesc->vRotation;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(vPos.x, vPos.y, vPos.z, 1.f));
		m_pTransformCom->Set_Scaled(vScale.x, vScale.y, vScale.z);
		m_pTransformCom->Rotate(vRot);

		m_EffectName = pEffectDesc->EffectName;
		m_ModelName = pEffectDesc->ModelName;
		m_MaskTextureName = pEffectDesc->MaskTextureName;
		m_DiffuseTextureName = pEffectDesc->DiffuseTextureName;
		m_fGlowFactor = pEffectDesc->fGlowFactor;
		m_iRenderIndex = pEffectDesc->iRenderIndex;
		//m_iPassIndex = pEffectDesc->iPassIndex;
		m_iNumWidthImage = pEffectDesc->iNumWidthImage;
		m_iNumHeighthImage = pEffectDesc->iNumHeightImage;

		m_iUnique_Index = pEffectDesc->iUnique_Index;
		m_bIsBillboarding = pEffectDesc->bIsBillboarding;

		m_vColor = pEffectDesc->vColor;
		m_LayerMatrix = pEffectDesc->LayerMatrix;
		m_isGlow = pEffectDesc->isGlow;

		m_iGameObjectData = m_isGlow;

		if (pEffectDesc->vGlowColor.x == 0.f)
		{
			m_bIsBackSideEffect = false;
		}
		else if (pEffectDesc->vGlowColor.x == 1.f)
		{
			m_bIsBackSideEffect = true;
		}

		if (pEffectDesc->vGlowColor.y == 0.f)
		{
			m_bIsShaderLoop = false;
		}
		else if (pEffectDesc->vGlowColor.y == 1.f)
		{
			m_bIsShaderLoop = true;
		}

		if (m_iGameObjectData <= -2)
		{
			/* 글로우 강도 */
			m_iObjectRenderData = (_int)m_fGlowFactor + 5 - 1;
		}
		else if (m_iGameObjectData == -1)
		{
			/* 글로우 강도 */
			m_iObjectRenderData = 0;
		}
		else if (m_iGameObjectData >= 0)
		{
			/* 글로우 강도 */
			m_iObjectRenderData = (_int)m_fGlowFactor - 1;
		}

		if (m_vColor.x != 0.0f || m_vColor.y != 0.0f || m_vColor.z != 0.0f || m_vColor.w != 30.0f)
		{
			m_IsColorEffect = true;
		}

		if (FAILED(Ready_Components(&m_ModelName, &m_MaskTextureName, &m_DiffuseTextureName)))
			return E_FAIL;

		if (pEffectDesc->SRV_Ptr != nullptr)
			m_pDiffuseTextureCom->Set_SRV(static_cast<ID3D11ShaderResourceView*>(pEffectDesc->SRV_Ptr));
		return S_OK;
	}

	m_pTransformCom->Set_Matrix(m_LayerMatrix);

	if (FAILED(Ready_Components(&m_ModelName, &m_MaskTextureName, &m_DiffuseTextureName)))
		return S_OK;

}

void CEffect_ZNone::Camera_Update(_float fTimeDelta)
{
}

void CEffect_ZNone::Update(_float fTimeDelta)
{

}

void CEffect_ZNone::Late_Update(_float fTimeDelta)
{
	
	if (!m_bIsNotPlaying)
	{
		if (m_pRenderInstance->Get_isLayerView() == true)
		{
			if (m_iRenderIndex == 2) //레이어
			{
				m_pRenderInstance->Add_RenderObject(static_cast<CRenderer::RENDERGROUP>(m_iRenderIndex), this);
				m_pRenderInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
			}
		}
		else
		{
			if (m_iRenderIndex == 1) //테스트
			{
				m_pRenderInstance->Add_RenderObject(static_cast<CRenderer::RENDERGROUP>(m_iRenderIndex), this);
				m_pRenderInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
			}
		}
	}

}

HRESULT CEffect_ZNone::Priority_Render(_float fTimeDelta)
{
	if (m_iPassIndex != 1)
		__super::Priority_Render(fTimeDelta);

	return S_OK;
}

HRESULT CEffect_ZNone::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pDiffuseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
			return E_FAIL;

		if (FAILED(m_pDiffuseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_AlphaTexture", 1)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(m_iPassIndex))) // 2
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	if (m_iPassIndex == 1)
		m_iPassIndex = 6;
	else
		m_iPassIndex = 1;


	return S_OK;
}

HRESULT CEffect_ZNone::Ready_Components(_wstring* pModelName, _wstring* pMaskTextureName, _wstring* pDiffuseTexturueName)
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Single_Eff_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	wstring ModelName = L"Prototype_Component_" + *pModelName;

	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, ModelName,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	wstring MaskTextureName = L"Prototype_Component_" + *pMaskTextureName;

	///* Com_Texture */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, MaskTextureName,
	//	TEXT("Com_MaskTexture"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
	//	return E_FAIL;

	wstring DiffuseTexturueName = L"Prototype_Component_" + *pDiffuseTexturueName;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, DiffuseTexturueName,
		TEXT("Com_DiffuseTexture"), reinterpret_cast<CComponent**>(&m_pDiffuseTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_ZNone::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_iUnique_Index", &m_iUnique_Index, sizeof(int))))
		return E_FAIL;

	_vector Color = XMVectorSet(m_vColor.x, m_vColor.y, m_vColor.z, m_vColor.w);

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &Color, sizeof(Color))))
		return E_FAIL;

	if (m_vColor.x != 0.0f || m_vColor.y != 0.0f || m_vColor.z != 0.0f || m_vColor.w != 30.0f)
	{
		m_IsColorEffect = true;
	}
	else
		m_IsColorEffect = false;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bColorChange", &m_IsColorEffect, sizeof(m_IsColorEffect))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fGlowFactor", &m_fGlowFactor, sizeof(float))))
		return E_FAIL;
	return S_OK;
}

CEffect_ZNone* CEffect_ZNone::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_ZNone* pInstance = new CEffect_ZNone(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_ZNone"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_ZNone::Clone(void* pArg)
{
	CEffect_ZNone* pInstance = new CEffect_ZNone(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEffect_ZNone"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_ZNone::Free()
{
	__super::Free();

}
