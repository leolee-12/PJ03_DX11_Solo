#include "stdafx.h"
#include "..\Public\SpaceMeteoBreak.h"

#include "RenderInstance.h"
#include "GameInstance.h"
#include "Effect_Manager.h"
#include "Main_Camera.h"
CSpaceMeteoBreak::CSpaceMeteoBreak(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject { pDevice, pContext }
{

}

CSpaceMeteoBreak::CSpaceMeteoBreak(const CSpaceMeteoBreak & Prototype)
	: CGameObject{ Prototype },
	m_vFragmentMoveDir{
		{0.f, 1.f, 0.f},
		{0.f, 1.f, 1.f},
		{0.f, 0.f, 1.f},
		{-0.5f, 0.1f, 1.f}, // 오른쪽 속도 1 말고 다르게
		{0.f, 0.1f, -1.2f},
		{-0.5f, -0.1f, -0.3f}, //6
		{-0.3f, -0.2f, 0.8f},
		{0.f, -1.f, 0.3f},
		{0.f, -0.1f, -1.2f},
		{-0.2f, -0.2f, -0.8f},
		{0.f, -1.f, -0.3f}
	}
{

}

HRESULT CSpaceMeteoBreak::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSpaceMeteoBreak::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//m_pTransformCom->Set_Scaled(0.01f, 0.01f, 0.01f);
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(100.f, 30.f, 0.f, 1.f));
	_vector vMainPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	for (size_t i = 0; i < 11; i++)
		XMStoreFloat4(&m_vFragmentPosition[i], vMainPos);

	m_pEffectTransform = CTransform::Create(m_pDevice, m_pContext);
	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;
	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;

	m_pEffectTransform->Set_Scaled(m_fSizeX, m_fSizeY, 1.f);
	m_pEffectTransform->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

	XMStoreFloat4x4(&m_Result4x4, m_pTransformCom->Get_WorldMatrix());
	CEffect_Layer::COPY_DESC tDesc{};
	tDesc.pPlayertMatrix = &m_Result4x4;
	m_pEffectLayer = CEffect_Manager::Get_Instance()->Copy_Layer_OverTheHandle(TEXT("testtest"), &tDesc);

	CMap_Manager::Get_Instance()->Push_MapObject(CMap_Manager::MAP_DEST_SPACE,
		static_cast<CMap_Manager::Map_Object_Key*>(pArg)->m_PrototypeKey, this);
	return S_OK;
}

void CSpaceMeteoBreak::Camera_Update(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_F10))
	{
		//Start_Space_DestructiveFinish(false);
		m_pRenderInstance->Switch_BlackOut(false);
	}
	if (m_pGameInstance->Key_Down(DIK_F11))
	{
		//Start_Space_DestructiveFinish(true);
		m_pRenderInstance->Switch_BlackOut(true);
	}
	
	//
}

void CSpaceMeteoBreak::Start_Space_DestructiveFinish(_bool isRight)
{
	m_isRight = isRight;
	if (isRight == false)
	{
		m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-100.f, 30.f, 0.f, 1.f));
	}
	else
	{
		m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(0));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(100.f, 30.f, 0.f, 1.f));
	}

	_vector vMainPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	for (size_t i = 0; i < 11; i++)
		XMStoreFloat4(&m_vFragmentPosition[i], vMainPos);

	m_iRGIndex = 0;
	m_fBrakeSwitchTime = false;
	m_isBrakeSwitch = false;
	m_isFastSwitch = true;
	m_fAccTime = 0.f;
	m_fBrakeSwitchTime = 0.f;
	m_fSpeed = 10.5f;
	m_isStart = true;
	
}

void CSpaceMeteoBreak::IsDone_Active_Init()
{
	m_isStart = false;
}

void CSpaceMeteoBreak::Update(_float fTimeDelta)
{
	if (m_isStart == false)
		return;

	if (m_isBrakeSwitch == false)
	{
		m_fBrakeSwitchTime += fTimeDelta;

		if (m_fBrakeSwitchTime > 0.8f)
		{

			m_fBrakeSwitchTime = 0.f;
			m_isBrakeSwitch = true;
			static_cast<CMain_Camera*>(*(m_pGameInstance->Get_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")).begin()))->StartCameraShake(3.f, 0.3);
			XMStoreFloat4x4(&m_Result4x4, m_pTransformCom->Get_WorldMatrix());

			_float4x4 Result4x4;
			XMStoreFloat4x4(&Result4x4, XMMatrixIdentity());

			Result4x4._41 = 0.f;
			Result4x4._42 = 0.f;
			Result4x4._43 = -5.f;
			
			CEffect_Layer::COPY_DESC tDesc{};
			tDesc.pPlayertMatrix = &m_Result4x4;
			CEffect_Layer::COPY_DESC m_tDesc{};
			m_tDesc.pPlayertMatrix = &Result4x4;
			CEffect_Layer* paEffect = CEffect_Manager::Get_Instance()->Copy_Layer_AndGet(TEXT("Meteo_Wind"), &tDesc);
			CEffect* pRotationEffect = { nullptr };
			CEffect_Layer* pRotationEffectToLayer = { nullptr };
			if (m_isRight == true)
			{
				CEffect_Manager::Get_Instance()->Copy_Layer_AndGet(TEXT("Meteo_Dust"), &m_tDesc);
				pRotationEffectToLayer = CEffect_Manager::Get_Instance()->Copy_Layer_AndGet(TEXT("Meteo_Dust"), &m_tDesc);
			}
			else
			{
				CEffect_Manager::Get_Instance()->Copy_Layer_AndGet(TEXT("Meteo_Dust_L"), &m_tDesc);
				pRotationEffectToLayer = CEffect_Manager::Get_Instance()->Copy_Layer_AndGet(TEXT("Meteo_Dust_L"), &m_tDesc);
			}

			pRotationEffectToLayer->Set_Layer_Rotation(_float3(5.f, 0.f, 0.f));

			if (paEffect != nullptr)
				(*paEffect->m_MixtureEffects.begin())->m_iRenderGroupIndex = static_cast<_int>(CRenderer::RG_BACKSIDE_EFFECT);
			////if (paEffect != nullptr)
			//	paEffect->Set_Layer_Scaled({ 30.f,30.f,30.f });

	
		}
	}
	else
	{
		m_fAccTime += fTimeDelta;

		if (m_isFastSwitch == true && m_fFastTimeLimit < m_fAccTime)
		{
			m_isFastSwitch = false;
			m_fSpeed = 0.3f;
		}


		for (size_t i = 0; i < 11; i++)
		{
			m_vFragmentPosition[i].y += m_vFragmentMoveDir[i].y * fTimeDelta * m_fSpeed;

			if (m_isRight == true)
			{
				m_vFragmentPosition[i].x += m_vFragmentMoveDir[i].x * fTimeDelta * m_fSpeed;
				m_vFragmentPosition[i].z += m_vFragmentMoveDir[i].z * fTimeDelta * m_fSpeed;
			}
			else
			{
				m_vFragmentPosition[i].x -= m_vFragmentMoveDir[i].x * fTimeDelta * m_fSpeed;
				m_vFragmentPosition[i].z -= m_vFragmentMoveDir[i].z * fTimeDelta * m_fSpeed;
			}
		}
	}
}

void CSpaceMeteoBreak::Late_Update(_float fTimeDelta)
{
	if (m_isStart == false)
		return;

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

	//if (m_isBrakeSwitch == false)
	//	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CSpaceMeteoBreak::Priority_Render(_float fTimeDelta)
{
	//m_pEffect->Priority_Render(fTimeDelta);
	//m_pEffect->Render(fTimeDelta);

	//for (auto& iter : m_pEffectLayer->m_MixtureEffects)
	//{
	//	iter->Priority_Render(-10.f);
	//	iter->Render(-10.f);
	//}
	// 
	/*if (m_isBrakeSwitch == false)
	{
		if (FAILED(m_pEffectTransform->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
			return E_FAIL;

		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(13)))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Bind_Buffers()))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;

		m_iRGIndex = 0;
	}*/

	return S_OK;
}

HRESULT CSpaceMeteoBreak::Render(_float fTimeDelta)
{
	if (m_isBrakeSwitch == false)
	{
		if (FAILED(Bind_ShaderResources()))
			return E_FAIL;

		_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

		for (size_t i = 0; i < iNumMeshes; i++)
		{
			if (FAILED(m_pModelCom->Bind_MaterialSRV(m_pShaderCom, aiTextureType_DIFFUSE, "g_DiffuseTexture", i)))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(12)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render(i)))
				return E_FAIL;
		}
	}
	else
	{
		if (FAILED(Bind_ShaderResources()))
			return E_FAIL;

		for (size_t i = 0; i < 11; i++)
		{
			_uint		iNumMeshes = m_pFragmentModelCom[i]->Get_NumMeshes();

			for (size_t j = 0; j < iNumMeshes; j++)
			{
				if (FAILED(m_pTextureCom_Diffuse->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
					return E_FAIL;

				_int iMeshIndex = j;
				if (FAILED(m_pShaderCom->Bind_RawValue("g_iMeteoIndex", &iMeshIndex, sizeof(_int))))
					return E_FAIL;

				if (FAILED(m_pShaderCom->Bind_RawValue("g_iMeteoPosition", &m_vFragmentPosition[i], sizeof(_float4))))
					return E_FAIL;
		
				if (FAILED(m_pShaderCom->Begin(11)))
					return E_FAIL;

				if (FAILED(m_pFragmentModelCom[i]->Render(j)))
					return E_FAIL;
			}
		}
	}
	
	return S_OK;
}

HRESULT CSpaceMeteoBreak::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxSpace"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_sp_meteobrake01"),
		TEXT("Com_Texture_Diffuse"), reinterpret_cast<CComponent**>(&m_pTextureCom_Diffuse))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_MeteoBreak"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_scrline00"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;
	
	for (size_t i = 0; i < 11; i++)
	{
		wstring strTagName = TEXT("Prototype_Component_Model_MeteoBrake_") + to_wstring(i + 1);
		wstring strComName = TEXT("Com_FragmentModel_") + to_wstring(i + 1);
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, strTagName.c_str(),
			strComName.c_str(), reinterpret_cast<CComponent**>(&m_pFragmentModelCom[i]))))
			return E_FAIL;
	}


	
	return S_OK;
}

HRESULT CSpaceMeteoBreak::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	//if (FAILED(m_pShaderCom->Bind_RawValue("g_fSpriteSize", &m_fSpriteSize, sizeof(_float2))))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_fSpriteCurPos", &m_fSpriteCurPos, sizeof(_float2))))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_Time", &m_fAccTime, sizeof(float))))
	//	return E_FAIL;
	
	return S_OK;
}

CSpaceMeteoBreak * CSpaceMeteoBreak::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSpaceMeteoBreak*		pInstance = new CSpaceMeteoBreak(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSpaceMeteoBreak"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSpaceMeteoBreak::Clone(void * pArg)
{
	CSpaceMeteoBreak*		pInstance = new CSpaceMeteoBreak(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSpaceMeteoBreak"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSpaceMeteoBreak::Free()
{
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pEffectTransform);

	Safe_Release(m_pTextureCom_Diffuse);
	for (size_t i = 0; i < 11; i++)
		Safe_Release(m_pFragmentModelCom[i]);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	Safe_Release(m_pEffectLayer);


	__super::Free();
}
