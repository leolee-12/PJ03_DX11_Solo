#include "stdafx.h"
#include "..\Public\Volcano_Destructive.h"

#include "RenderInstance.h"
#include "GameInstance.h"
#include "Effect_Manager.h"
#include "Main_Camera.h"
CVolcano_Destructive::CVolcano_Destructive(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CVolcano_Destructive::CVolcano_Destructive(const CVolcano_Destructive& Prototype)
	: CGameObject{ Prototype },
	m_vFragmentMoveDir{
		{0.f, 0.f, -2.f}, //0
		{-0.5f, 0.f, -0.7f},//1
		{-0.6f, 0.f, -0.5f},//2
		{-0.2f, 0.7f, -1.f},//3
		{-0.2f, 0.5f, -0.3f},//4
		{-0.2f, 0.4f, -0.2f},//5
		{-0.3f, 0.2f, 0.2f},//6
		{0.f, 0.3f, 0.1f},//7
		{0.f, 0.7f, -0.1f},//8
		{-0.2f, 0.6f, 0.4f},//9
		{-0.3f, 0.9f, 0.8f},//10
		{-0.3f, 0.9f, 0.8f},//11
		{ -0.1f, 0.1f, 0.3f },//12
		{ 0.f, 1.f, 0.f },//13
		{ 0.f, 1.f, 0.3f },//14
	}
{

}

HRESULT CVolcano_Destructive::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVolcano_Destructive::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//m_pTransformCom->Set_Scaled(0.01f, 0.01f, 0.01f);
	CTransform::TRANSFORM_DESC tTransformDesc{};
	tTransformDesc.fRotationPerSec = 1.f;
	m_pTransformCom->SetUp_TransformDesc(&tTransformDesc);
	//m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(0.f));
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(100.f, 30.f, 0.f, 1.f));
	_vector vMainPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	for (size_t i = 0; i < 15; i++)
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

	CMap_Manager::Get_Instance()->Push_MapObject(CMap_Manager::MAP_DEST_VOLCANO,
		static_cast<CMap_Manager::Map_Object_Key*>(pArg)->m_PrototypeKey, this);

	return S_OK;
}

void CVolcano_Destructive::Camera_Update(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_F10))
	{
		Start_Volcano_DestructiveFinish(false);
	}
	if (m_pGameInstance->Key_Down(DIK_F11))
	{
		Start_Volcano_DestructiveFinish(true);
	}
}

void CVolcano_Destructive::Start_Volcano_DestructiveFinish(_bool isRight)
{
	m_isRight = isRight;
	if (isRight == false)
	{
		m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
		m_pTransformCom->Turn(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(-20));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-10.f, 0.f, -5.f, 1.f));
	}
	else
	{
		m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(0));
		m_pTransformCom->Turn(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(20));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(10.f, 0.f, -5.f, 1.f));
	}

	_vector vMainPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	for (size_t i = 0; i < 15; i++)
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

void CVolcano_Destructive::IsDone_Active_Init()
{
	m_isStart = false;
}

void CVolcano_Destructive::Update(_float fTimeDelta)
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

			XMStoreFloat4x4(&m_Result4x4, m_pTransformCom->Get_WorldMatrix());

			_float4x4 Result4x4;
			XMStoreFloat4x4(&Result4x4, XMMatrixIdentity());

			Result4x4._41 = 0.f;
			Result4x4._42 = 0.f;
			Result4x4._43 = -5.f;
			CEffect_Layer::COPY_DESC tDesc{};

			XMStoreFloat4x4(&m_IdentityMatrix, XMMatrixIdentity());
			tDesc.pPlayertMatrix = &m_IdentityMatrix;
			CEffect_Layer::COPY_DESC m_tDesc{};
			m_tDesc.pPlayertMatrix = &Result4x4;

			CEffect_Layer* pBeamEffect = { nullptr }; 
			CEffect_Layer* pBustEffect = { nullptr };
			CEffect_Layer* pMeteo_Dust_Effect = { nullptr };
			CEffect_Layer* pWindEffect = { nullptr };

			static_cast<CMain_Camera*>(*(m_pGameInstance->Get_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")).begin()))->StartCameraShake(3.f, 1.f);
			//
			if (m_isRight == true)
			{
				pBeamEffect = CEffect_Manager::Get_Instance()->Copy_Layer_AndGet(TEXT("Meteo_BeamCore"), &tDesc);
				pBustEffect = CEffect_Manager::Get_Instance()->Copy_Layer_AndGet(TEXT("Volcano_Dust"), &tDesc);
				pMeteo_Dust_Effect = CEffect_Manager::Get_Instance()->Copy_Layer_AndGet(TEXT("Meteo_Dust"), &tDesc);
				pWindEffect = CEffect_Manager::Get_Instance()->Copy_Layer_AndGet(TEXT("Volcano_Wind"), &tDesc);
			}
			else
			{
				pBeamEffect = CEffect_Manager::Get_Instance()->Copy_Layer_AndGet(TEXT("Meteo_BeamCore_Left"), &tDesc);
				pBustEffect = CEffect_Manager::Get_Instance()->Copy_Layer_AndGet(TEXT("Volcano_Dust_Left"), &tDesc);
				pMeteo_Dust_Effect = CEffect_Manager::Get_Instance()->Copy_Layer_AndGet(TEXT("Meteo_Dust_L"), &tDesc);
				pWindEffect = CEffect_Manager::Get_Instance()->Copy_Layer_AndGet(TEXT("Volcano_Wind_Left"), &tDesc);
			}
			CEffect_Layer* pExplosionLayer = CEffect_Manager::Get_Instance()->Copy_Layer_AndGet(TEXT("Explosion_Volcano"), &tDesc);
		
			(*pWindEffect->m_MixtureEffects.begin())->m_iRenderGroupIndex = static_cast<_int>(CRenderer::RG_CUTSCENE_LATE_EFFECT);
			(*pWindEffect->m_MixtureEffects.begin())->m_iChangePassIndex = 7;

			(*pBustEffect->m_MixtureEffects.begin())->m_iRenderGroupIndex = static_cast<_int>(CRenderer::RG_CUTSCENE_LATE_EFFECT);
			(*pBustEffect->m_MixtureEffects.begin())->m_iChangePassIndex = 9;

			(*pBeamEffect->m_MixtureEffects.begin())->m_iRenderGroupIndex = static_cast<_int>(CRenderer::RG_CUTSCENE_LATE_EFFECT);
			(*pBeamEffect->m_MixtureEffects.begin())->m_iChangePassIndex = 8;

			(*pMeteo_Dust_Effect->m_MixtureEffects.begin())->m_iRenderGroupIndex = static_cast<_int>(CRenderer::RG_CUTSCENE_LATE_EFFECT);
			(*pMeteo_Dust_Effect->m_MixtureEffects.begin())->m_iChangePassIndex = 8;
			(*pMeteo_Dust_Effect->m_MixtureEffects.begin())->m_vColor = _float4(38.f, 1.f, 4.f, 1.9f);
			
			//if (pExplosionLayer != nullptr)
			//{	
			//	for (auto& iter : pExplosionLayer->m_MixtureEffects)
			//	{
			//		iter->m_iRenderGroupIndex = static_cast<_int>(CRenderer::RG_BACKSIDE_EFFECT);
			//	}
			//	//(*pExplosionLayer->m_MixtureEffects.begin())->m_iRenderGroupIndex = static_cast<_int>(CRenderer::RG_BACKSIDE_EFFECT);
			//	//pExplosionLayer->Set_Copy_Layer_Scaled({ 10.f,10.f,10.f });
			//	//pExplosionLayer->Set_Copy_Layer_Rotation(_float3(0.f, 90.f, 90.f));
			//	//pExplosionLayer->Set_Copy_Layer_Position(_float3(150.f, 0.f, 10.f));
			//}


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


		for (size_t i = 0; i < 15; i++)
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

void CVolcano_Destructive::Late_Update(_float fTimeDelta)
{
	if (m_isStart == false)
		return;


	m_pRenderInstance->Add_RenderObject(CRenderer::RG_CUTSCENE_OBJECT, this);

}

HRESULT CVolcano_Destructive::Priority_Render(_float fTimeDelta)
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

HRESULT CVolcano_Destructive::Render(_float fTimeDelta)
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

			if (FAILED(m_pShaderCom->Begin(i == 0 ? 19 : 18)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render(i)))
				return E_FAIL;
		}
	}
	else
	{
		if (FAILED(Bind_ShaderResources()))
			return E_FAIL;

		_uint		iMainNumMeshes = m_pModelCom->Get_NumMeshes();

		for (size_t i = 1; i < iMainNumMeshes; i++)
		{
			if (FAILED(m_pModelCom->Bind_MaterialSRV(m_pShaderCom, aiTextureType_DIFFUSE, "g_DiffuseTexture", i)))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(i == 0 ? 19 : 18)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render(i)))
				return E_FAIL;
		}

		if (FAILED(m_pBRModelCom->Bind_MaterialSRV(m_pShaderCom, aiTextureType_DIFFUSE, "g_DiffuseTexture", 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(19)))
			return E_FAIL;

		if (FAILED(m_pBRModelCom->Render(0)))
			return E_FAIL;
		
		for (size_t i = 0; i < 15; i++)
		{
			_uint		iNumMeshes = m_pFragmentModelCom[i]->Get_NumMeshes();

			for (size_t j = 0; j < iNumMeshes; j++)
			{
				if (FAILED(m_pFragmentModelCom[i]->Bind_MaterialSRV(m_pShaderCom, aiTextureType_DIFFUSE, "g_DiffuseTexture", j)))
					return E_FAIL;

				//_int iMeshIndex = j;
				//if (FAILED(m_pShaderCom->Bind_RawValue("g_iMeteoIndex", &iMeshIndex, sizeof(_int))))
				//	return E_FAIL;

				if (FAILED(m_pShaderCom->Bind_RawValue("g_vDestructivePos", &m_vFragmentPosition[i], sizeof(_float4))))
					return E_FAIL;

				if (FAILED(m_pShaderCom->Begin(20)))
					return E_FAIL;

				if (FAILED(m_pFragmentModelCom[i]->Render(j)))
					return E_FAIL;
			}
		}
	}

	return S_OK;
}

HRESULT CVolcano_Destructive::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxVolcano"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_sp_meteobrake01"),
		TEXT("Com_Texture_Diffuse"), reinterpret_cast<CComponent**>(&m_pTextureCom_Diffuse))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_vo_DestructiveFinish"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_vo_SFmountain01"),
		TEXT("Com_BRModel"), reinterpret_cast<CComponent**>(&m_pBRModelCom))))
		return E_FAIL;
	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_scrline00"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;
	
	for (size_t i = 0; i < 15; i++)
	{
		wstring strTagName = TEXT("Prototype_Component_Model_vo_SFmountainrRock") + to_wstring(i);
		wstring strComName = TEXT("Com_FragmentModel_") + to_wstring(i);
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, strTagName.c_str(),
			strComName.c_str(), reinterpret_cast<CComponent**>(&m_pFragmentModelCom[i]))))
			return E_FAIL;
	}



	return S_OK;
}

HRESULT CVolcano_Destructive::Bind_ShaderResources()
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

CVolcano_Destructive* CVolcano_Destructive::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVolcano_Destructive* pInstance = new CVolcano_Destructive(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CVolcano_Destructive"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CVolcano_Destructive::Clone(void* pArg)
{
	CVolcano_Destructive* pInstance = new CVolcano_Destructive(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CVolcano_Destructive"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVolcano_Destructive::Free()
{
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pEffectTransform);
	Safe_Release(m_pBRModelCom);
	Safe_Release(m_pTextureCom_Diffuse);
	for (size_t i = 0; i < 15; i++)
		Safe_Release(m_pFragmentModelCom[i]);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	Safe_Release(m_pEffectLayer);


	__super::Free();
}
