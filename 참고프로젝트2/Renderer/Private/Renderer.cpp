#include <random>
#include <vector>

#include "Renderer.h"
#include "GameObject.h"
#include "RenderInstance.h"
#include "VIBuffer_Rect.h"
#include "Shader.h"
#include "GameInstance.h"
#include "Component.h"
#include "Light_Manager.h"
#include "Transform.h"
_uint		g_iSizeX = 1920 * 2;
_uint		g_iSizeY = 1080 * 2;

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pRenderInstance{ CRenderInstance::Get_Instance() }
	, m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pRenderInstance);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CRenderer::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	D3D11_VIEWPORT			ViewportDesc{};
	_uint					iNumViewport = { 1 };
	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	Initialize_RenderTarget();

	XMStoreFloat4x4(&m_DownWorldMatrix, XMMatrixScaling(ViewportDesc.Width / 2, ViewportDesc.Height / 2, 1.f));
	m_DownWorldMatrix._41 = -((ViewportDesc.Width / 2) / 2);
	m_DownWorldMatrix._42 = ((ViewportDesc.Height / 2) / 2);

	XMStoreFloat4x4(&m_DownWorldMatrix_Second, XMMatrixScaling(ViewportDesc.Width / 4, ViewportDesc.Height / 4, 1.f));
	m_DownWorldMatrix_Second._41 = -((ViewportDesc.Width / 2) / 2) * 1.5;
	m_DownWorldMatrix_Second._42 = ((ViewportDesc.Height / 2) / 2) * 1.5;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	m_pGlowShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred_Glow.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (nullptr == m_pGlowShader)
		return E_FAIL;

	m_pUI_GlowShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred_UI_Glow.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (nullptr == m_pUI_GlowShader)
		return E_FAIL;

	m_pAuraTextureCom = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Aura/cmn_Fractal%d.dds"), 6);
	m_pEastFinish_TextureCom = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_scrRock0%d.dds"), 2);
	m_pDistortionShaderCom = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred_Distortion.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	m_pDistortionTextureCom = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Distortion/Distortion_%d.png"), 6);
	m_pDistortionTransformCom = CTransform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pDistortionTextureCom || nullptr == m_pDistortionTransformCom || nullptr == m_pDistortionShaderCom)
		return E_FAIL;
	m_pBackBufferSRV = m_pGameInstance->Get_BackBufferShaderResourceView();
	Safe_AddRef(m_pBackBufferSRV);

	ID3D11Texture2D* pDepthStencilTexture = nullptr;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = g_iSizeX;
	TextureDesc.Height = g_iSizeY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL
		/*| D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE*/;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pShadowDSV)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);

	_float offsetY = 18.f;
#ifdef _DEBUG
	if (FAILED(m_pRenderInstance->Ready_RT_Debug(TEXT("Target_Player_AuraMask"), 100.f, 100.f + offsetY, 200.0f, 200.0f)))
		return E_FAIL;
	//if (FAILED(m_pRenderInstance->Ready_RT_Debug(TEXT("Target_StageDepth"), 100.f, 300.f, 200.0f, 200.0f)))
	//	return E_FAIL;

	//if (FAILED(m_pRenderInstance->Ready_RT_Debug(TEXT("Target_Player_AuraMask"), 100.f, 500.f, 200.0f, 200.0f)))
	//	return E_FAIL;
	//if (FAILED(m_pRenderInstance->Ready_RT_Debug(TEXT("Target_Distortion"), 600.f, 100.f, 200.0f, 200.0f)))
	//	return E_FAIL;
	//if (FAILED(m_pRenderInstance->Ready_RT_Debug(TEXT("Target_ResultDistortion_BackBuffer"), 350.f, 150.f, 300.f, 300.f)))
	//	return E_FAIL;
	//if (FAILED(m_pRenderInstance->Ready_RT_Debug(TEXT("Target_MapBloomAlpha"), 600.f, 100.f, 200.0f, 200.0f)))
	//	return E_FAIL;
	//if (FAILED(m_pRenderInstance->Ready_RT_Debug(TEXT("Target_MapBloomDiffuse"), 350.f, 150.f, 300.f, 300.f)))
	//	return E_FAIL;

	//if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_AllGlowDiffuse"), TEXT("Target_AllGlowDiffuse"))))
	//	return E_FAIL;
	//if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_AllGlowDiffuse"), TEXT("Target_AllGlowAlpha"))))


	//if (FAILED(m_pRenderInstance->Ready_RT_Debug(TEXT("Target_Diffuse"), 100.f, 100.f, 200.0f, 200.0f)))
	//	return E_FAIL;
	//if (FAILED(m_pRenderInstance->Ready_RT_Debug(TEXT("Target_UpTarget_Second"), 100.f, 500.f, 200.0f, 200.0f)))
	//	return E_FAIL;
	//if (FAILED(m_pRenderInstance->Ready_RT_Debug(TEXT("Target_All_Blur_X"), 600.f, 100.f, 200.0f, 200.0f)))
	//	return E_FAIL;
	//if (FAILED(m_pRenderInstance->Ready_RT_Debug(TEXT("Target_All_Blur_Y"), 350.f, 150.f, 300.f, 300.f)))
	//	return E_FAIL; 
	/*
		if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_Player"), TEXT("Target_Player_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_Player"), TEXT("Target_Player_Normal"))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_Player"), TEXT("Target_Player_Depth"))))
		return E_FAIL;
	*/
	//if (FAILED(m_pRenderInstance->Ready_RT_Debug(TEXT("Target_Player_Diffuse"), 100.f, 100.f, 200.0f, 200.0f)))
	//	return E_FAIL;
	//if (FAILED(m_pRenderInstance->Ready_RT_Debug(TEXT("Target_Player_Normal"), 100.f, 300.f, 200.0f, 200.0f)))
	//	return E_FAIL;
	//if (FAILED(m_pRenderInstance->Ready_RT_Debug(TEXT("Target_Player_Depth"), 100.f, 500.f, 200.0f, 200.0f)))
	//	return E_FAIL;
	////if (FAILED(m_pRenderInstance->Ready_RT_Debug(TEXT("Target_PickDepth"), 600.f, 100.f, 200.0f, 200.0f)))
	////	return E_FAIL;
	//if (FAILED(m_pRenderInstance->Ready_RT_Debug(TEXT("Target_Shade"), 350.f, 150.f, 300.f, 300.f)))
	//	return E_FAIL;
	//if (FAILED(m_pRenderInstance->Ready_RT_Debug(TEXT("Target_Specular"), 350.f, 450.f, 300.f, 300.f)))
	//	return E_FAIL;

	//if (FAILED(m_pRenderInstance->Ready_RT_Debug(TEXT("Target_PickDepth"), 350.f, 450.f, 300.f, 300.f)))
	//	return E_FAIL;
	//if (FAILED(m_pRenderInstance->Ready_RT_Debug(TEXT("Target_LightDepth"), 1920.0f - 150.0f, 150.f, 300.f, 300.f)))
	//	return E_FAIL;
#endif

	return S_OK;
}

HRESULT CRenderer::Add_RenderObject(RENDERGROUP eRenderGroup, CGameObject* pRenderObject, RENDER_OBJECT* pDesc)
{
	if (eRenderGroup >= RG_END)
		return E_FAIL;

	m_RenderObjects[eRenderGroup].emplace_back(pRenderObject);

	Safe_AddRef(pRenderObject);

	/* 특수한 효과는 모두 이제 이쪽으로 처리한다 */
	if (nullptr != pDesc)
	{
		if (pDesc->strName != "")
			m_PlayerStrNames.push_back(pDesc->strName);
		else
			Add_Render_GlowDesc(eRenderGroup, &pDesc->tGlowDesc);
	}

	return S_OK;
}

HRESULT CRenderer::Add_Render_GlowDesc(RENDERGROUP eRenderGroup, GLOW_DESC* pDesc)
{
	switch (eRenderGroup)
	{
	case RG_GLOW_PRI:  m_GlowDescs[GLOW_PRI].push_back(*pDesc);		return S_OK;
	case RG_GLOW_STAR: m_GlowDescs[GLOW_STAR].push_back(*pDesc);	return S_OK;
	case RG_GLOW:	   m_GlowDescs[GLOW_MAIN].push_back(*pDesc);	return S_OK;
	case RG_UI_GLOW:   m_GlowDescs[GLOW_UI].push_back(*pDesc);		return S_OK;
	case RG_MULTY_GLOW:   m_GlowDescs[GLOW_UI_MULTY].push_back(*pDesc);	return S_OK;
	}
}

HRESULT CRenderer::Add_DebugComponent(CComponent* pDebugComponent)
{
	m_DebugComponent.emplace_back(pDebugComponent);

	Safe_AddRef(pDebugComponent);

	return S_OK;
}

HRESULT CRenderer::Draw(_float fTimeDelta)
{
	if (FAILED(Render_Node(fTimeDelta)))
		return E_FAIL;
	/*-----------------디버깅용------------------*/
	if (FAILED(Render_NonBlend_Test(fTimeDelta)))
		return E_FAIL;
	if (FAILED(Render_NonBlend_Layer(fTimeDelta)))
		return E_FAIL;
	/*-------------------------------------------*/
	if (FAILED(Render_StageDepth(fTimeDelta)))
		return E_FAIL;

	if (FAILED(Render_Priority(fTimeDelta)))
		return E_FAIL;
	if (FAILED(Render_Glow_Priority(fTimeDelta)))
		return E_FAIL;
	if (FAILED(Render_Blend_Priority(fTimeDelta)))
		return E_FAIL;
	/*----------------- 플레이어가 아닌 다른 오브젝트 -----------------*/
	if (FAILED(Render_ShadowObj(fTimeDelta)))
		return E_FAIL;
	if (FAILED(Render_Map(fTimeDelta)))
		return E_FAIL;
	if (FAILED(Render_NonBlend(fTimeDelta)))
		return E_FAIL;

	if (FAILED(Render_Lights(fTimeDelta)))
		return E_FAIL;
	if (FAILED(Render_Deferred(fTimeDelta)))
		return E_FAIL;
	/*-----------------------------------------------------------------*/

	if (FAILED(Render_NonLight(fTimeDelta)))
		return E_FAIL;

	if (FAILED(Render_Metallic(fTimeDelta)))
		return E_FAIL;

	if (FAILED(Draw_Test_PostProcess(fTimeDelta)))
		return E_FAIL;

	/* 맵을 어둡게 할려고 여기 호출하지만 캐릭터는*/
	if (FAILED(Draw_MapBlackOut(fTimeDelta)))
		return E_FAIL;
	if (FAILED(Render_Player(fTimeDelta)))
		return E_FAIL;

	if (FAILED(Render_StageDeferred(fTimeDelta)))
		return E_FAIL;

	/* 여기서 그리고 있음 이펙트를 위한 행동 하지만 위에서 블러를 먹인 그림을 가지고만 있고 그리진 않아서 영향이 안가짐 */
	if (FAILED(Render_AllGlow_Effect_BackSide(fTimeDelta)))
		return E_FAIL;

	if (FAILED(Render_NonLight_Effect(fTimeDelta)))
		return E_FAIL;
	if (FAILED(Render_AllGlow_Effect_Pri(fTimeDelta)))
		return E_FAIL;
	if (FAILED(Render_Blend(fTimeDelta)))
		return E_FAIL;
	if (FAILED(Render_Glow(fTimeDelta)))
		return E_FAIL;
	
	/* 맵이 어두워진 상태에서 디스토션하는게 자연스러운가? 테스트 필요 */
	if (FAILED(Render_Distortion(fTimeDelta)))
		return E_FAIL;


	if (FAILED(Render_MultyGlow_UI(fTimeDelta)))
		return E_FAIL;

	if (FAILED(Render_UI(fTimeDelta)))
		return E_FAIL;
	if (FAILED(Render_Glow_UI(fTimeDelta)))
		return E_FAIL;
	if (FAILED(Render_AllGlow_Effect(fTimeDelta)))
		return E_FAIL;
	if (FAILED(Render_CutScene_Pri_Effect(fTimeDelta)))
		return E_FAIL;
	if (FAILED(Render_CutScene_Object(fTimeDelta)))
		return E_FAIL;
	if (FAILED(Render_CutScene_Late_Effect(fTimeDelta)))
		return E_FAIL;

	if (FAILED(Draw_AllBlackOut(fTimeDelta)))
		return E_FAIL;
	if (FAILED(Draw_WhiteBlack_Mode(fTimeDelta)))
		return E_FAIL;
	if (FAILED(Draw_AllWhiteOut(fTimeDelta)))
		return E_FAIL;

	//if (FAILED(Draw_Test_PostProcess(fTimeDelta)))
	//	return E_FAIL;


#ifdef _DEBUG
	if (FAILED(Render_Debug(fTimeDelta)))
		return E_FAIL;
#endif
	return S_OK;
}

void CRenderer::Create_Distortion(DISTORTION_DESC& tDistortionDesc)
{
	m_Distortions.push_back(tDistortionDesc);
}

void CRenderer::Delete_LoopDistortion()
{
	m_Distortions.erase(
		remove_if(m_Distortions.begin(),m_Distortions.end(),[](const DISTORTION_DESC& distortion) {
				return distortion.isLoop;
			}
		),
		m_Distortions.end()
	);
}


void CRenderer::Create_HitDistortion(_float4 vPlayerPos, _float3 vDir, _float2 vOffSetPos, _float2 vOffSetScale, _float fLifeTime)
{
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<_float> offsetDist(0.04f, 0.08f);
	uniform_real_distribution<_float> scaleDist(0.8f, 1.2f);   
	uniform_real_distribution<_float> factorDist(0.8f, 1.2f);
	_float yOffset = 1.55f;	
	_float yDecrement = 0.2f;
	_float xStep = 0.12f;	

	if (vDir.x != 0)
	{

		for (_int i = 0; i < 8; ++i)
		{
			DISTORTION_DESC tDistortionDesc{};
			_float4 vStrainPos = vPlayerPos;

			_float xDirection = (i % 2 == 0) ? 1.0f : -1.0f;
			//tDistortionDesc.vDir = _float3(vDir.x * xDirection, vDir.y, vDir.z);
			vStrainPos.x += xDirection * xStep;
			vStrainPos.y += yOffset - offsetDist(gen);

			tDistortionDesc.vPosition = {
				vStrainPos.x + vOffSetPos.x + offsetDist(gen),
				vStrainPos.y + vOffSetPos.y /*+ offsetDist(gen)*/,
				vStrainPos.z,
				1.0f
			};

			float scaleModifier = scaleDist(gen);
			tDistortionDesc.vScale = {
				2.0f * vOffSetScale.x * scaleModifier,
				0.47f * vOffSetScale.y * scaleModifier
			};

			tDistortionDesc.fLifeTime = fLifeTime;
			tDistortionDesc.fMaxTime = fLifeTime;
			tDistortionDesc.fFactor = factorDist(gen);
			tDistortionDesc.vDir = vDir;

			m_Distortions.push_back(tDistortionDesc);

			yOffset -= yDecrement;

			xStep += 0.02f;
		}
	}
	else
	{
		yOffset = 1.f;
		//yDecrement = 1.2f;
		for (_int i = 0; i < 6; ++i)
		{
			DISTORTION_DESC tDistortionDesc{};
			_float4 vStrainPos = vPlayerPos;

			_float xDirection = (i % 2 == 0) ? 1.0f : -1.0f;
			//tDistortionDesc.vDir = _float3(vDir.x * xDirection, vDir.y, vDir.z);
			vStrainPos.x += yOffset - offsetDist(gen);
			vStrainPos.y += 1 + xDirection * xStep;

			tDistortionDesc.vPosition = {
				vStrainPos.x + vOffSetPos.x + offsetDist(gen),
				vStrainPos.y + vOffSetPos.y /*+ offsetDist(gen)*/,
				vStrainPos.z,
				1.0f
			};

			float scaleModifier = scaleDist(gen);
			tDistortionDesc.vScale = {
				0.47f * vOffSetScale.x * scaleModifier,
				2.f * vOffSetScale.y * scaleModifier
			};

			tDistortionDesc.fLifeTime = fLifeTime;
			tDistortionDesc.fMaxTime = fLifeTime;
			tDistortionDesc.fFactor = factorDist(gen);
			tDistortionDesc.vDir = vDir;

			m_Distortions.push_back(tDistortionDesc);

			yOffset -= yDecrement;

			xStep += 0.02f;
		}
	}

}

HRESULT CRenderer::Render_Priority(_float fTimeDelta)
{
	for (auto& pRenderObject : m_RenderObjects[RG_PRIORITY])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render(fTimeDelta);

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[RG_PRIORITY].clear();

	return S_OK;
}

HRESULT CRenderer::Render_ShadowObj(_float fTimeDelta)
{
	/* Target_LightDepth */
	if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_ShadowObjects"), m_pShadowDSV)))
		return E_FAIL;
	//if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_ShadowObjects"))))
	//	return E_FAIL;
	//D3D11_VIEWPORT			ViewPortDesc;
	//ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	//ViewPortDesc.TopLeftX = 0;
	//ViewPortDesc.TopLeftY = 0;
	//ViewPortDesc.Width = (_float)g_iSizeX;
	//ViewPortDesc.Height = (_float)g_iSizeY;
	//ViewPortDesc.MinDepth = 0.f;
	//ViewPortDesc.MaxDepth = 1.f;

	//m_pContext->RSSetViewports(1, &ViewPortDesc);


	for (auto& pRenderObject : m_RenderObjects[RG_SHADOWOBJ])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Shadow_Render(fTimeDelta);

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[RG_SHADOWOBJ].clear();

	if (FAILED(m_pRenderInstance->End_MRT()))
		return E_FAIL;

	//ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	//ViewPortDesc.TopLeftX = 0;
	//ViewPortDesc.TopLeftY = 0;
	//ViewPortDesc.Width = 1920.f;
	//ViewPortDesc.Height = 1080.0f;
	//ViewPortDesc.MinDepth = 0.f;
	//ViewPortDesc.MaxDepth = 1.f;

	//m_pContext->RSSetViewports(1, &ViewPortDesc);

	return S_OK;
}

HRESULT CRenderer::Render_Glow_Priority(_float fTimeDelta)
{
	size_t iCount = 0;
	for (auto& pRenderObject : m_RenderObjects[RG_GLOW_PRI])
	{
		if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_GlowDiffuse"))))
			return E_FAIL;

		if (nullptr != pRenderObject)
			pRenderObject->Render(fTimeDelta);

		if (FAILED(m_pRenderInstance->End_MRT()))
			return E_FAIL;

		if (0 != m_GlowDescs[GLOW_PRI].size())
		{
			auto iter = m_GlowDescs[GLOW_PRI].begin();
			advance(iter, iCount);
			if (FAILED(Draw_Glow(m_pGlowShader, &(*iter))))
				return E_FAIL;
		}
		//if (FAILED(Draw_Glow(-1.f, pRenderObject->Get_GameObjectData())))
		//	return E_FAIL;

		Safe_Release(pRenderObject);
		iCount++;
	}

	m_GlowDescs[GLOW_PRI].clear();
	m_RenderObjects[RG_GLOW_PRI].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Blend_Priority(_float fTimeDelta)
{
	size_t iCount = 0;
	for (auto& pRenderObject : m_RenderObjects[RG_BLEND_PRI])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render(fTimeDelta);

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[RG_BLEND_PRI].clear();

	for (auto& pRenderObject : m_RenderObjects[RG_GLOW_STAR])
	{
		if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_GlowDiffuse"))))
			return E_FAIL;

		if (nullptr != pRenderObject)
			pRenderObject->Render(fTimeDelta);

		if (FAILED(m_pRenderInstance->End_MRT()))
			return E_FAIL;

		if (0 != m_GlowDescs[GLOW_STAR].size())
		{
			auto iter = m_GlowDescs[GLOW_STAR].begin();
			advance(iter, iCount);

			if (FAILED(Draw_Glow(m_pGlowShader, &(*iter))))
				return E_FAIL;
		}

		Safe_Release(pRenderObject);
		iCount++;
	}

	m_GlowDescs[GLOW_STAR].clear();
	m_RenderObjects[RG_GLOW_STAR].clear();

	return S_OK;
}

HRESULT CRenderer::Render_StageDepth(_float fTimeDelta)
{

	if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_StageDepth"))))
		return E_FAIL;

	for (auto& pRenderObject : m_RenderObjects[RG_STAGE])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Shadow_Render(fTimeDelta);

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[RG_STAGE].clear();

	if (FAILED(m_pRenderInstance->End_MRT()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CRenderer::Render_StageDeferred(_float fTimeDelta)
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	_float4x4 viewMatrixInv = m_pGameInstance->Get_Transform_Inverse_Float4x4(CPipeLine::D3DTS_VIEW);
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", &viewMatrixInv)))
		return E_FAIL;

	_float4x4 projMatrixInv = m_pGameInstance->Get_Transform_Inverse_Float4x4(CPipeLine::D3DTS_PROJ);
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", &projMatrixInv)))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pShader, "g_DiffuseTexture", TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pShader, "g_ShadeTexture", TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pShader, "g_SpecularTexture", TEXT("Target_Specular"))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pShader, "g_LightDepthTexture", TEXT("Target_LightDepth"))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pShader, "g_DepthTexture", TEXT("Target_StageDepth"))))
		return E_FAIL;



	_float4x4			LightViewMatrix, LightProjMatrix;

	XMStoreFloat4x4(&LightViewMatrix, m_pGameInstance->Get_ShadowTransform_Matrix(CPipeLine::D3DTS_VIEW));
	XMStoreFloat4x4(&LightProjMatrix, m_pGameInstance->Get_ShadowTransform_Matrix(CPipeLine::D3DTS_PROJ));

	if (FAILED(m_pShader->Bind_Matrix("g_LightViewMatrix", &LightViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_LightProjMatrix", &LightProjMatrix)))
		return E_FAIL;

	m_pShader->Begin(12);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();
	return S_OK;

	
}

HRESULT CRenderer::Render_Map(_float fTimeDelta)
{
	_int iRenderCount = { 0 };

	if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_BloomDiffuse"))))
		return E_FAIL;

	for (auto& pRenderObject : m_RenderObjects[RG_MAP])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render(fTimeDelta);

		iRenderCount++;
		Safe_Release(pRenderObject);
	}

	m_RenderObjects[RG_MAP].clear();

	if (FAILED(m_pRenderInstance->End_MRT()))
		return E_FAIL;

	if(iRenderCount > 0)
		Draw_MapBloom();

	for (auto& pRenderObject : m_RenderObjects[RG_SPACEMAP])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render(fTimeDelta);

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[RG_SPACEMAP].clear();

	return S_OK;
}

HRESULT CRenderer::Render_NonBlend(_float fTimeDelta)
{
	for (auto& pRenderObject : m_RenderObjects[RG_NONBLEND])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Priority_Render(fTimeDelta);
	}

	if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_GameObjects"))))
		return E_FAIL;

	for (auto& pRenderObject : m_RenderObjects[RG_NONBLEND])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render(fTimeDelta);

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[RG_NONBLEND].clear();

	if (FAILED(m_pRenderInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Player(_float fTimeDelta)
{
	m_iRenderPlayerCount = 0;
	
	for (auto& pRenderObject : m_RenderObjects[RG_PLAYER])
	{
		if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_Player"))))
			return E_FAIL;

		if (nullptr != pRenderObject)
		{
			pRenderObject->Render(fTimeDelta);
		}

		Safe_Release(pRenderObject);

		if (FAILED(m_pRenderInstance->End_MRT()))
			return E_FAIL;

		Render_PlayerLight(fTimeDelta, m_iRenderPlayerCount);

		if (m_iRenderPlayerCount == 0)
		{
			if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_PlayerDefferd"))))
				return E_FAIL;
			Render_PlayerDeferred(fTimeDelta);
			if (FAILED(m_pRenderInstance->End_MRT()))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pRenderInstance->Begin_MRT_DoNotClear(TEXT("MRT_PlayerDefferd"))))
				return E_FAIL;

			Render_PlayerDeferred(fTimeDelta);

			if (FAILED(m_pRenderInstance->End_MRT()))
				return E_FAIL;
		}

		m_iRenderPlayerCount++;
	}

	if(m_iRenderPlayerCount != 0)
		Render_PlayerBlur(fTimeDelta);

	m_RenderObjects[RG_PLAYER].clear();
	m_PlayerStrNames.clear();

	return S_OK;
}

HRESULT CRenderer::Render_PlayerLight(_float fTimeDelta, _int iCount)
{
	if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_LightAcc"))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	_float4x4 viewMatrixInv = m_pGameInstance->Get_Transform_Inverse_Float4x4(CPipeLine::D3DTS_VIEW);
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", &viewMatrixInv)))
		return E_FAIL;

	_float4x4 projMatrixInv = m_pGameInstance->Get_Transform_Inverse_Float4x4(CPipeLine::D3DTS_PROJ);
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", &projMatrixInv)))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pShader, "g_NormalTexture", TEXT("Target_Player_Normal"))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pShader, "g_DepthTexture", TEXT("Target_Player_Depth"))))
		return E_FAIL;

	_float4 camPosition = m_pGameInstance->Get_CamPosition_Float4();
	if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", &camPosition, sizeof(_float4))))
		return E_FAIL;

	m_pVIBuffer->Bind_Buffers();
	auto strName = m_PlayerStrNames.begin() + iCount;
	LIGHT_DESC* pLightDesc = m_pRenderInstance->Get_LightDesc(CLight_Manager::LIGHT_PLAYER, 0, *strName);
	if(pLightDesc != nullptr)
		m_pRenderInstance->Render_Lights(CLight_Manager::LIGHT_PLAYER, m_pShader, m_pVIBuffer, pLightDesc->strName, fTimeDelta); // 수정

	if (NULL != m_pRenderInstance->Check_EffectLights())
	{
		if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
			return E_FAIL;

		_float4x4 viewMatrixInv = m_pGameInstance->Get_Transform_Inverse_Float4x4(CPipeLine::D3DTS_VIEW);
		if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", &viewMatrixInv)))
			return E_FAIL;

		_float4x4 projMatrixInv = m_pGameInstance->Get_Transform_Inverse_Float4x4(CPipeLine::D3DTS_PROJ);
		if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", &projMatrixInv)))
			return E_FAIL;

		if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pShader, "g_NormalTexture", TEXT("Target_Player_Normal"))))
			return E_FAIL;
		if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pShader, "g_DepthTexture", TEXT("Target_Player_Depth"))))
			return E_FAIL;

		_float4 camPosition = m_pGameInstance->Get_CamPosition_Float4();
		if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", &camPosition, sizeof(_float4))))
			return E_FAIL;

		m_pRenderInstance->Render_Lights(CLight_Manager::LIGHT_EFFECT, m_pShader, m_pVIBuffer, pLightDesc->strName, fTimeDelta);
	}

	if (FAILED(m_pRenderInstance->End_MRT()))
		return E_FAIL;

	if (pLightDesc != nullptr)
	{
		Render_PlayerAuraMaskBlur(fTimeDelta, pLightDesc->vAuraColor);
	}
	
	
	return S_OK;
}

HRESULT CRenderer::Render_PlayerDeferred(_float fTimeDelta)
{

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pShader, "g_DiffuseTexture", TEXT("Target_Player_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pShader, "g_ShadeTexture", TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pShader, "g_SpecularTexture", TEXT("Target_Specular"))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pShader, "g_LightDepthTexture", TEXT("Target_LightDepth"))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pShader, "g_DepthTexture", TEXT("Target_Player_Depth"))))
		return E_FAIL;


	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pShader, "g_MetallicTexture", TEXT("Target_Metallic"))))
		return E_FAIL;


	_float4x4			LightViewMatrix, LightProjMatrix;

	XMStoreFloat4x4(&LightViewMatrix, XMMatrixLookAtLH(XMVectorSet(0.f, 10.f, 0.f, 1.f), XMVectorSet(1.f, -1.f, 1.f, 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	XMStoreFloat4x4(&LightProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(120.0f), (_float)1920.f / 1080.f, 0.1f, 1000.f));

	if (FAILED(m_pShader->Bind_Matrix("g_LightViewMatrix", &LightViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_LightProjMatrix", &LightProjMatrix)))
		return E_FAIL;

	m_pShader->Begin(3);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();


	return S_OK;
}

HRESULT CRenderer::Render_PlayerAuraMaskBlur(_float fTimeDelta, _float4 vColor)
{
	m_fAuraAccTime += fTimeDelta;
	if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_Down"))))
		return E_FAIL;

	if (FAILED(m_pGlowShader->Bind_Matrix("g_WorldMatrix", &m_DownWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pGlowShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pGlowShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	_float2 fTexSize = { 1920.f, 1080.f };
	if (FAILED(m_pGlowShader->Bind_RawValue("g_DownTexSize", &fTexSize, sizeof(_float2))))
		return E_FAIL;

	_float2 fSamplingSize = { 4.f,4.f };
	if (FAILED(m_pGlowShader->Bind_RawValue("g_DownSamplingSize", &fSamplingSize, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pGlowShader, "g_Texture", TEXT("Target_Player_AuraMask"))))
		return E_FAIL;

	m_pGlowShader->Begin(3);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if (FAILED(m_pRenderInstance->End_MRT()))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_DownSecond"))))
		return E_FAIL;

	if (FAILED(m_pGlowShader->Bind_Matrix("g_WorldMatrix", &m_DownWorldMatrix_Second)))
		return E_FAIL;

	if (FAILED(m_pGlowShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pGlowShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	fTexSize = { 1920.f * 0.5f, 1080.f * 0.5f };
	if (FAILED(m_pGlowShader->Bind_RawValue("g_DownTexSize", &fTexSize, sizeof(_float2))))
		return E_FAIL;

	fSamplingSize = { 6.f,6.f };
	if (FAILED(m_pGlowShader->Bind_RawValue("g_DownSamplingSize", &fSamplingSize, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pGlowShader, "g_Texture", TEXT("Target_DownTarget"))))
		return E_FAIL;

	m_pGlowShader->Begin(3);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if (FAILED(m_pRenderInstance->End_MRT()))
		return E_FAIL;
	//-------------------------------------------
	//return S_OK;
	//-------------------------------------------
	if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_Blur_X"))))
		return E_FAIL;

	if (FAILED(m_pGlowShader->Bind_Matrix("g_WorldMatrix", &m_DownWorldMatrix_Second)))
		return E_FAIL;
	if (FAILED(m_pGlowShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pGlowShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pGlowShader, "g_Texture", TEXT("Target_DownTarget_Second"))))
		return E_FAIL;

	m_pGlowShader->Begin(0);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if (FAILED(m_pRenderInstance->End_MRT()))
		return E_FAIL;
	//****************************************
	if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_Blur_Y"))))
		return E_FAIL;

	if (FAILED(m_pGlowShader->Bind_Matrix("g_WorldMatrix", &m_DownWorldMatrix_Second)))
		return E_FAIL;
	if (FAILED(m_pGlowShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pGlowShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pGlowShader, "g_Texture", TEXT("Target_Blur_X"))))
		return E_FAIL;

	m_pGlowShader->Begin(1);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if (FAILED(m_pRenderInstance->End_MRT()))
		return E_FAIL;

	//***************************************************

	if (FAILED(m_pGlowShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pGlowShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pGlowShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pGlowShader, "g_BlurTexture", TEXT("Target_Blur_Y"))))
		return E_FAIL;

	m_pAuraTextureCom->Bind_ShaderResource(m_pGlowShader, "g_AuraTexture", 4);
	m_pAuraTextureCom->Bind_ShaderResource(m_pGlowShader, "g_AuraMaskTexture", 5);
	m_pGlowShader->Bind_RawValue("g_Time", &m_fAuraAccTime, sizeof(_float));
	m_pGlowShader->Bind_RawValue("g_vAuraColor", &vColor, sizeof(_float4));
	
	m_pGlowShader->Begin(17);

	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	/*---------------------------------------------------------------------------------------------------------------------------------------------- */
	//if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_Player_Blur_X"))))
	//	return E_FAIL;

	//if (FAILED(m_pGlowShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
	//	return E_FAIL;
	//if (FAILED(m_pGlowShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
	//	return E_FAIL;
	//if (FAILED(m_pGlowShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
	//	return E_FAIL;

	//if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pGlowShader, "g_Texture", TEXT("Target_Player_AuraMask"))))
	//	return E_FAIL;

	//m_pGlowShader->Begin(18);
	//m_pVIBuffer->Bind_Buffers();
	//m_pVIBuffer->Render();

	//if (FAILED(m_pRenderInstance->End_MRT()))
	//	return E_FAIL;
	////****************************************
	//if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_Player_Blur_Y"))))
	//	return E_FAIL;

	//if (FAILED(m_pGlowShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
	//	return E_FAIL;
	//if (FAILED(m_pGlowShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
	//	return E_FAIL;
	//if (FAILED(m_pGlowShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
	//	return E_FAIL;

	//if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pGlowShader, "g_Texture", TEXT("Target_Player_Blur_X"))))
	//	return E_FAIL;

	///*
	//	각자 거기서 나온걸로 미리 다른 렌더타겟에 넣고 디퓨즈까지 넣어주자
	//*/
	//m_pGlowShader->Begin(19);
	//m_pVIBuffer->Bind_Buffers();
	//m_pVIBuffer->Render();

	//if (FAILED(m_pRenderInstance->End_MRT()))
	//	return E_FAIL;


	//if (FAILED(m_pGlowShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
	//	return E_FAIL;
	//if (FAILED(m_pGlowShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
	//	return E_FAIL;
	//if (FAILED(m_pGlowShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
	//	return E_FAIL;

	////if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(pShader, "g_BlurTexture", TEXT("Target_UpTarget_Second"))))
	////	return E_FAIL;
	//if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pGlowShader, "g_BlurTexture", TEXT("Target_Player_Blur_Y"))))
	//	return E_FAIL;

	//m_pAuraTextureCom->Bind_ShaderResource(m_pGlowShader, "g_AuraTexture", 0);

	//m_pGlowShader->Begin(17);
	//m_pVIBuffer->Bind_Buffers();
	//m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::Render_PlayerBlur(_float fTimeDelta)
{
	if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_Player_Blur_X"))))
		return E_FAIL;

	if (FAILED(m_pGlowShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pGlowShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pGlowShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pGlowShader, "g_Texture", TEXT("Target_PlayerDefferd"))))
		return E_FAIL;

	m_pGlowShader->Begin(10);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if (FAILED(m_pRenderInstance->End_MRT()))
		return E_FAIL;
	//****************************************
	if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_Player_Blur_Y"))))
		return E_FAIL;

	if (FAILED(m_pGlowShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pGlowShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pGlowShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pGlowShader, "g_Texture", TEXT("Target_Player_Blur_X"))))
		return E_FAIL;

	/*
		각자 거기서 나온걸로 미리 다른 렌더타겟에 넣고 디퓨즈까지 넣어주자
	*/
	m_pGlowShader->Begin(11);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if (FAILED(m_pRenderInstance->End_MRT()))
		return E_FAIL;

	//***************************************************

	return S_OK;
}

HRESULT CRenderer::Render_NonBlend_Test(_float fTimeDelta)
{
	if (m_isLayerView == true)
	{
		/* Diffuse + Normal */
		//if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_GameObjects"))))
		//	return E_FAIL;

		for (auto& pRenderObject : m_RenderObjects[RG_NONBLEND_TEST])
		{
			Safe_Release(pRenderObject);
		}

		m_RenderObjects[RG_NONBLEND_TEST].clear();

		//if (FAILED(m_pRenderInstance->End_MRT()))
		//	return E_FAIL;

		return S_OK;
	}

	/* Diffuse + Normal */
	if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_EffectToolPick"))))
		return E_FAIL;

	for (auto& pRenderObject : m_RenderObjects[RG_NONBLEND_TEST])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render(fTimeDelta);

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[RG_NONBLEND_TEST].clear();

	if (FAILED(m_pRenderInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_NonBlend_Layer(_float fTimeDelta)
{
	if (m_isLayerView == false)
	{
		for (auto& pRenderObject : m_RenderObjects[RG_NONBLEND_LAYER])
		{
			Safe_Release(pRenderObject);
		}

		m_RenderObjects[RG_NONBLEND_LAYER].clear();

		return S_OK;
	}

	/* Diffuse + Normal */
	if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_EffectToolPick"))))
		return E_FAIL;

	for (auto& pRenderObject : m_RenderObjects[RG_NONBLEND_LAYER])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render(fTimeDelta);

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[RG_NONBLEND_LAYER].clear();

	if (FAILED(m_pRenderInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Lights(_float fTimeDelta)
{
	/* 빛 연산한 결과를 렌더타겟에 기록해준다. (명암 + ) */
	/* Bind : Shade */
	if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_LightAcc"))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	_float4x4 viewMatrixInv = m_pGameInstance->Get_Transform_Inverse_Float4x4(CPipeLine::D3DTS_VIEW);
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", &viewMatrixInv)))
		return E_FAIL;

	_float4x4 projMatrixInv = m_pGameInstance->Get_Transform_Inverse_Float4x4(CPipeLine::D3DTS_PROJ);
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", &projMatrixInv)))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pShader, "g_NormalTexture", TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pShader, "g_DepthTexture", TEXT("Target_Depth"))))
		return E_FAIL;

		_float4 camPosition = m_pGameInstance->Get_CamPosition_Float4();
		if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", &camPosition, sizeof(_float4))))
			return E_FAIL;

	m_pVIBuffer->Bind_Buffers();

	m_pRenderInstance->Render_Lights(CLight_Manager::LIGHT_BACKGROUND, m_pShader, m_pVIBuffer, "", fTimeDelta); // 수정

	if (FAILED(m_pRenderInstance->End_MRT()))
		return E_FAIL;


	return S_OK;
}

HRESULT CRenderer::Render_Deferred(_float fTimeDelta)
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("isOutLine", &m_isOutLine, sizeof(bool))))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pShader, "g_DiffuseTexture", TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pShader, "g_ShadeTexture", TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pShader, "g_SpecularTexture", TEXT("Target_Specular"))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pShader, "g_LightDepthTexture", TEXT("Target_LightDepth"))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pShader, "g_DepthTexture", TEXT("Target_Depth"))))
		return E_FAIL;



	_float4x4			LightViewMatrix, LightProjMatrix;

	XMStoreFloat4x4(&LightViewMatrix, XMMatrixLookAtLH(XMVectorSet(0.f, 10.f, 0.f, 1.f), XMVectorSet(1.f, -1.f, 1.f, 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	XMStoreFloat4x4(&LightProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(120.0f), (_float)1920.f / 1080.f, 0.1f, 1000.f));
	
	if (FAILED(m_pShader->Bind_Matrix("g_LightViewMatrix", &LightViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_LightProjMatrix", &LightProjMatrix)))
		return E_FAIL;

	m_pShader->Begin(4);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();
	return S_OK;
}

HRESULT CRenderer::Render_NonLight(_float fTimeDelta)
{
	for (auto& pRenderObject : m_RenderObjects[RG_NONLIGHT])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render(fTimeDelta);

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[RG_NONLIGHT].clear();

	return S_OK;
}

HRESULT CRenderer::Render_NonLight_Effect(_float fTimeDelta)
{
	m_iEffectRenderCount = 0;
	_int iOutLineCount = 0;
	for (auto& pRenderObject : m_RenderObjects[RG_NONLIGHT_EFFECT])
	{
		if (pRenderObject->Get_GameObjectData() > -1)
		{
			if (nullptr != pRenderObject)
				pRenderObject->Priority_Render(fTimeDelta);

			m_pRenderInstance->Begin_EffectMRT(TEXT("MRT_AllGlowDiffuse_"), pRenderObject->Get_ObjectRenderData());

			if (nullptr != pRenderObject)
				pRenderObject->Render(fTimeDelta);

			Safe_Release(pRenderObject);

			if (FAILED(m_pRenderInstance->End_MRT()))
				return E_FAIL;
			
			m_iEffectRenderCount++;
		}
		else
		{
			if (nullptr != pRenderObject)
				pRenderObject->Priority_Render(fTimeDelta);

			if (iOutLineCount == 0)
			{
				if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_GameObjects"))))
					return E_FAIL;
			
				if (nullptr != pRenderObject)
					pRenderObject->Render(fTimeDelta);

				Safe_Release(pRenderObject);

				if (FAILED(m_pRenderInstance->End_MRT()))
					return E_FAIL;
			}
			else
			{
				if (FAILED(m_pRenderInstance->Begin_MRT_DoNotClear(TEXT("MRT_GameObjects"))))
					return E_FAIL;

				if (nullptr != pRenderObject)
					pRenderObject->Render(fTimeDelta);

				Safe_Release(pRenderObject);

				if (FAILED(m_pRenderInstance->End_MRT()))
					return E_FAIL;
			}

			iOutLineCount++;
		}
	}

	m_RenderObjects[RG_NONLIGHT_EFFECT].clear();

	if (iOutLineCount > 0)
	{
		Draw_OutLine_Effect();
	}
	return S_OK;
}

HRESULT CRenderer::Render_Glow(_float fTimeDelta)
{
	size_t iCount = 0;
	for (auto& pRenderObject : m_RenderObjects[RG_GLOW])
	{
		if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_GlowDiffuse"))))
			return E_FAIL;

		if (nullptr != pRenderObject)
			pRenderObject->Render(fTimeDelta);

		Safe_Release(pRenderObject);

		if (FAILED(m_pRenderInstance->End_MRT()))
			return E_FAIL;

		if (0 != m_GlowDescs[GLOW_MAIN].size())
		{
			auto iter = m_GlowDescs[GLOW_MAIN].begin();
			advance(iter, iCount);
			if (FAILED(Draw_Glow(m_pGlowShader, &(*iter))))
				return E_FAIL;
		}

		iCount++;
	}

	m_GlowDescs[GLOW_MAIN].clear();
	m_RenderObjects[RG_GLOW].clear();

	return S_OK;
}

HRESULT CRenderer::Render_AllGlow_Effect_BackSide(_float fTimeDelta)
{
	_uint iEffectGlowPri_RenderCount = 0;
	for (auto& pRenderObject : m_RenderObjects[RG_BACKSIDE_EFFECT])
	{
		if (pRenderObject->Get_GameObjectData() == -2) // -2 면 먼저 글로우 프리로 그려버린다
		{
			if (nullptr != pRenderObject)
				pRenderObject->Priority_Render(fTimeDelta);

			m_pRenderInstance->Begin_EffectMRT(TEXT("MRT_AllGlowDiffuse_"), pRenderObject->Get_ObjectRenderData());

			if (nullptr != pRenderObject)
				pRenderObject->Render(fTimeDelta);

			if (FAILED(m_pRenderInstance->End_MRT()))
				return E_FAIL;

			Safe_Release(pRenderObject);

			iEffectGlowPri_RenderCount++;
		}
	}

	if (iEffectGlowPri_RenderCount > 0)
		Draw_AllGlow_Effect(true);

	_uint iEffectGlow_RenderCount = 0;
	for (auto& pRenderObject : m_RenderObjects[RG_BACKSIDE_EFFECT])
	{
		if (pRenderObject->Get_GameObjectData() > -1) //  -1 이상이면 글로우 뒤로 그리는거
		{
			if (nullptr != pRenderObject)
				pRenderObject->Priority_Render(fTimeDelta);

			m_pRenderInstance->Begin_EffectMRT(TEXT("MRT_AllGlowDiffuse_"), pRenderObject->Get_ObjectRenderData());

			if (nullptr != pRenderObject)
				pRenderObject->Render(fTimeDelta);

			Safe_Release(pRenderObject);

			if (FAILED(m_pRenderInstance->End_MRT()))
				return E_FAIL;

			iEffectGlow_RenderCount++;
		}
		else if (pRenderObject->Get_GameObjectData() == -1) // -1 바로 그린다
		{
			if (nullptr != pRenderObject)
				pRenderObject->Priority_Render(fTimeDelta);

			if (nullptr != pRenderObject)
				pRenderObject->Render(fTimeDelta);

			Safe_Release(pRenderObject);
		}
	}

	m_RenderObjects[RG_BACKSIDE_EFFECT].clear();
	
	if (iEffectGlow_RenderCount > 0)
		Draw_AllGlow_Effect(false);

	if (m_iRenderPlayerCount != 0)
	{
		if (FAILED(m_pGlowShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
			return E_FAIL;
		if (FAILED(m_pGlowShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
			return E_FAIL;
		if (FAILED(m_pGlowShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
			return E_FAIL;

		if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pGlowShader, "g_Texture", TEXT("Target_PlayerDefferd"))))
			return E_FAIL;
		//if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(pShader, "g_BlurTexture", TEXT("Target_UpTarget_Second"))))
		//	return E_FAIL;
		if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pGlowShader, "g_BlurTexture", TEXT("Target_Player_Blur_Y"))))
			return E_FAIL;

	
		m_pGlowShader->Begin(9);
		m_pVIBuffer->Bind_Buffers();
		m_pVIBuffer->Render();
	}
	
	return S_OK;
}

HRESULT CRenderer::Render_AllGlow_Effect_Pri(_float fTimeDelta)
{
	m_iEffectGlowPri_RenderCount = 0;
	for (auto& pRenderObject : m_RenderObjects[RG_BLEND])
	{
		if (pRenderObject->Get_GameObjectData() == -2) // -2 면 먼저 글로우 프리로 그려버린다
		{
			if (nullptr != pRenderObject)
				pRenderObject->Priority_Render(fTimeDelta);

			m_pRenderInstance->Begin_EffectMRT(TEXT("MRT_AllGlowDiffuse_"), pRenderObject->Get_ObjectRenderData());

			if (nullptr != pRenderObject)
				pRenderObject->Render(fTimeDelta);

			if (FAILED(m_pRenderInstance->End_MRT()))
				return E_FAIL;

			Safe_Release(pRenderObject);

			m_iEffectGlowPri_RenderCount++;
		}
	}

	if (m_iEffectGlowPri_RenderCount > 0)
		Draw_AllGlow_Effect(true);

	return S_OK;
}

HRESULT CRenderer::Render_Blend(_float fTimeDelta)
{
	for (auto& pRenderObject : m_RenderObjects[RG_BLEND])
	{
		if (pRenderObject->Get_GameObjectData() > -1) //  -1 이상이면 글로우 뒤로 그리는거
		{
			if (nullptr != pRenderObject)
				pRenderObject->Priority_Render(fTimeDelta);

			m_pRenderInstance->Begin_EffectMRT(TEXT("MRT_AllGlowDiffuse_"), pRenderObject->Get_ObjectRenderData());

			if (nullptr != pRenderObject)
				pRenderObject->Render(fTimeDelta);

			Safe_Release(pRenderObject);

			if (FAILED(m_pRenderInstance->End_MRT()))
				return E_FAIL;

			m_iEffectRenderCount++;
		}
		else if (pRenderObject->Get_GameObjectData() == -1) // -1 바로 그린다
		{
			if (nullptr != pRenderObject)
				pRenderObject->Priority_Render(fTimeDelta);

			if (nullptr != pRenderObject)
				pRenderObject->Render(fTimeDelta);

			Safe_Release(pRenderObject);
		}
	}

	m_RenderObjects[RG_BLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Render_UI(_float fTimeDelta)
{
	for (auto& pRenderObject : m_RenderObjects[RG_UI])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render(fTimeDelta);

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[RG_UI].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Glow_UI(_float fTimeDelta)
{

	if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_GlowDiffuse"))))
		return E_FAIL;

	for (auto& pRenderObject : m_RenderObjects[RG_UI_GLOW])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render(fTimeDelta);

		Safe_Release(pRenderObject);
	}

	if (FAILED(m_pRenderInstance->End_MRT()))
		return E_FAIL;

	if (0 != m_GlowDescs[GLOW_UI].size())
	{
		auto iter = m_GlowDescs[GLOW_UI].begin();

		if (FAILED(Draw_Glow(m_pUI_GlowShader, &(*iter))))
			return E_FAIL;
	}

	m_GlowDescs[GLOW_UI].clear();
	m_RenderObjects[RG_UI_GLOW].clear();

	return S_OK;
}

HRESULT CRenderer::Render_MultyGlow_UI(_float fTimeDelta)
{
	size_t iCount = 0;

	for (auto& pRenderObject : m_RenderObjects[RG_MULTY_GLOW])
	{
		if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_GlowDiffuse"))))
			return E_FAIL;

		if (nullptr != pRenderObject)
			pRenderObject->Render(fTimeDelta);

		Safe_Release(pRenderObject);

		if (FAILED(m_pRenderInstance->End_MRT()))
			return E_FAIL;

		if (0 != m_GlowDescs[GLOW_UI_MULTY].size())
		{
			auto iter = m_GlowDescs[GLOW_UI_MULTY].begin();
			advance(iter, iCount);

			if (FAILED(Draw_Glow(m_pUI_GlowShader, &(*iter))))
				return E_FAIL;
		}

		iCount++;
	}

	m_GlowDescs[GLOW_UI_MULTY].clear();
	m_RenderObjects[RG_MULTY_GLOW].clear();

	return S_OK;
}

HRESULT CRenderer::Render_AllGlow_Effect(_float fTimeDelta)
{
	if (m_iEffectRenderCount > 0)
	{
		Draw_AllGlow_Effect(false);
	}

	return S_OK;
}

HRESULT CRenderer::Render_CutScene_Pri_Effect(_float fTimeDelta)
{
	for (auto& pRenderObject : m_RenderObjects[RG_CUTSCENE_PRI_EFFECT])
	{
		if (nullptr != pRenderObject)
		{
			pRenderObject->Priority_Render(fTimeDelta);
			pRenderObject->Render(fTimeDelta);
		}

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[RG_CUTSCENE_PRI_EFFECT].clear();

	return S_OK;
}

HRESULT CRenderer::Render_CutScene_Object(_float fTimeDelta)
{
	for (auto& pRenderObject : m_RenderObjects[RG_CUTSCENE_OBJECT])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render(fTimeDelta);

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[RG_CUTSCENE_OBJECT].clear();

	return S_OK;
}

HRESULT CRenderer::Render_CutScene_Late_Effect(_float fTimeDelta)
{
	

	for (auto& pRenderObject : m_RenderObjects[RG_CUTSCENE_LATE_EFFECT])
	{
		if (nullptr != pRenderObject)
		{
			pRenderObject->Priority_Render(fTimeDelta);
			pRenderObject->Render(fTimeDelta);
		}

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[RG_CUTSCENE_LATE_EFFECT].clear();
	return S_OK;
}

HRESULT CRenderer::Render_Node(_float fTimeDelta)
{
	for (auto& pRenderObject : m_RenderObjects[RG_NODE])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render(fTimeDelta);

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[RG_NODE].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Distortion(_float fTimeDelta)
{
	if (NULL == m_Distortions.size())
		return S_OK;
	
	m_fAccTime += fTimeDelta;

	/* 벡터를 순회하면서 현재 기록된 위치에 디스토션 마스크를 한 렌더타겟에 한번에 그림 */
	if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_Distortion"))))
		return E_FAIL;

	_float4x4 viewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
	if (FAILED(m_pDistortionShaderCom->Bind_Matrix("g_ViewMatrix", &viewMatrix)))
		return E_FAIL;

	_float4x4 projMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);
	if (FAILED(m_pDistortionShaderCom->Bind_Matrix("g_ProjMatrix", &projMatrix)))
		return E_FAIL;

	if (FAILED(m_pDistortionTextureCom->Bind_ShaderResource(m_pDistortionShaderCom, "g_Texture", 0)))
		return E_FAIL;
	 
	for (auto iter = m_Distortions.begin(); iter != m_Distortions.end(); )
	{
		iter->fLifeTime -= fTimeDelta;

		if (iter->fLifeTime <= 0)
		{
			if (iter->isLoop == true)
			{
				iter->fLifeTime = iter->fMaxTime;
			}
			else
			{
				iter = m_Distortions.erase(iter);
				continue;
			}
		}

		m_pDistortionTransformCom->Set_Scaled(iter->vScale.x, iter->vScale.y, 1.f);
		m_pDistortionTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&iter->vPosition));
		m_pDistortionTransformCom->LookAt(m_pGameInstance->Get_CamPosition_Vector());
		//_vector vCamPos = m_pGameInstance->Get_CamPosition_Vector();
		//if (iter->vDir.x == -1.f)
		//{
		//	_vector vPos = m_pDistortionTransformCom->Get_State(CTransform::STATE_POSITION);

		//	_vector vDir = vPos - vCamPos;
		//	vCamPos += vDir * 2.f;
		//}
		//m_pDistortionTransformCom->LookAt(vCamPos);
		if (iter->vDir.y == 1)
		{
			if (FAILED(m_pDistortionTextureCom->Bind_ShaderResource(m_pDistortionShaderCom, "g_MaskTexture", 4)))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pDistortionTextureCom->Bind_ShaderResource(m_pDistortionShaderCom, "g_MaskTexture", 1)))
				return E_FAIL;
		}

		if (FAILED(m_pDistortionTransformCom->Bind_ShaderResource(m_pDistortionShaderCom, "g_WorldMatrix")))
			return E_FAIL;

		m_pDistortionShaderCom->Bind_RawValue("g_Time", &m_fAccTime, sizeof(_float));
		m_pDistortionShaderCom->Bind_RawValue("fLifeTime", &iter->fLifeTime, sizeof(_float));
		m_pDistortionShaderCom->Bind_RawValue("fLifeMaxTime", &iter->fMaxTime, sizeof(_float));
		m_pDistortionShaderCom->Bind_RawValue("g_vDir", &iter->vDir, sizeof(_float3));
		m_pDistortionShaderCom->Bind_RawValue("g_Factor", &iter->fFactor, sizeof(_float));

		if(iter->isLoop == false)
			m_pDistortionShaderCom->Begin(0);
		else
			m_pDistortionShaderCom->Begin(4);

		m_pVIBuffer->Bind_Buffers();
		m_pVIBuffer->Render();

		++iter;
	}

	if (FAILED(m_pRenderInstance->End_MRT()))
		return E_FAIL;

	/* 백버퍼 쉐이더리소스뷰를 바로 렌더타겟으로 있는 상태에선 불가능함
	   그래서 임의의 다른 렌더타겟의 백버퍼와 여러 디스토션을 그린 렌더타겟을 이용해서 효과를 줌 */
	if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_ResultDistortion_BackBuffer"))))
		return E_FAIL;
	
	if (FAILED(m_pDistortionShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pDistortionShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pDistortionShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pDistortionShaderCom, "g_Texture", TEXT("Target_Distortion"))))
		return E_FAIL;

	if (FAILED(m_pDistortionShaderCom->Bind_ShaderResourceView("g_BackBufferTexture", m_pBackBufferSRV)))
		return E_FAIL;

	

	m_pDistortionShaderCom->Begin(1);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if (FAILED(m_pRenderInstance->End_MRT()))
		return E_FAIL;

	/* 나온 결과를 바로 백버퍼에 덮어씀 */
	if (FAILED(m_pDistortionShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pDistortionShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pDistortionShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pDistortionShaderCom, "g_Texture", TEXT("Target_ResultDistortion_BackBuffer"))))
		return E_FAIL;

	m_pDistortionShaderCom->Begin(2);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	return S_OK;
}


#ifdef _DEBUG

HRESULT CRenderer::Render_Debug(_float fTimeDelta)
{
	// Debug Component 관련 처리
	if (m_bShow_Debug_Component)
	{
		for (auto& pComponent : m_DebugComponent)
		{
			pComponent->Render(fTimeDelta);
			Safe_Release(pComponent);
		}

		m_DebugComponent.clear();
	}

	// Render Target 관련 처리
	if (m_bShow_RenderTarget)
	{
		// Matrix 바인딩
		if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
			return E_FAIL;

		//if (FAILED(m_pRenderInstance->Render_RT_Debug(TEXT("MRT_BloomDiffuse"), m_pShader, m_pVIBuffer)))
		//	return E_FAIL;
		if (FAILED(m_pRenderInstance->Render_RT_Debug(TEXT("MRT_Player"), m_pShader, m_pVIBuffer)))
			return E_FAIL;
		/*if (FAILED(m_pRenderInstance->Render_RT_Debug(TEXT("MRT_ShadowObjects"), m_pShader, m_pVIBuffer)))
			return E_FAIL;*/

		//if (FAILED(m_pRenderInstance->Render_RT_Debug(TEXT("MRT_EffectToolPick"), m_pShader, m_pVIBuffer)))
		//	return E_FAIL;

		/*
		MRT_Distortion
		MRT_ResultDistortion_BackBuffer
		*/
		//if (FAILED(m_pRenderInstance->Render_RT_Debug(TEXT("MRT_All_Blur_X"), m_pShader, m_pVIBuffer)))
		//	return E_FAIL;
		//if (FAILED(m_pRenderInstance->Render_RT_Debug(TEXT("MRT_All_Blur_Y"), m_pShader, m_pVIBuffer)))
		//	return E_FAIL;
		//// Render Target 디버그 렌더링
		//if (FAILED(m_pRenderInstance->Render_RT_Debug(TEXT("MRT_AllGlowDiffuse"), m_pShader, m_pVIBuffer)))
		//	return E_FAIL;
		//if (FAILED(m_pRenderInstance->Render_RT_Debug(TEXT("MRT_LightAcc"), m_pShader, m_pVIBuffer)))
		//	return E_FAIL;

		//if (FAILED(m_pRenderInstance->Render_RT_Debug(TEXT("MRT_ShadowObjects"), m_pShader, m_pVIBuffer)))
		//	return E_FAIL;
	}
	if (!m_bShow_RenderTarget)
		return S_OK;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	//if (FAILED(m_pRenderInstance->Render_RT_Debug(TEXT("MRT_UI_Blur_X"), m_pShader, m_pVIBuffer)))
	//	return E_FAIL;
	//if (FAILED(m_pRenderInstance->Render_RT_Debug(TEXT("MRT_UI_Blur_Y"), m_pShader, m_pVIBuffer)))
	//	return E_FAIL;
	//if (FAILED(m_pRenderInstance->Render_RT_Debug(TEXT("MRT_UI_Down"), m_pShader, m_pVIBuffer)))
	//	return E_FAIL;
	//if (FAILED(m_pRenderInstance->Render_RT_Debug(TEXT("MRT_UI_DownSecond"), m_pShader, m_pVIBuffer)))
	//	return E_FAIL;

	//if (FAILED(m_pRenderInstance->Render_RT_Debug(TEXT("MRT_UpSecond"), m_pShader, m_pVIBuffer)))
	//	return E_FAIL;

	if (FAILED(m_pRenderInstance->Render_RT_Debug(TEXT("MRT_EffectToolPick"), m_pShader, m_pVIBuffer)))
		return E_FAIL;
	//if (FAILED(m_pRenderInstance->Render_RT_Debug(TEXT("MRT_GameObjects"), m_pShader, m_pVIBuffer)))
	//	return E_FAIL;
	//if (FAILED(m_pRenderInstance->Render_RT_Debug(TEXT("MRT_LightAcc"), m_pShader, m_pVIBuffer)))
	//	return E_FAIL;
	//if (FAILED(m_pRenderInstance->Render_RT_Debug(TEXT("MRT_LightAcc"), m_pShader, m_pVIBuffer)))
	//	return E_FAIL;
	//if (FAILED(m_pRenderInstance->Render_RT_Debug(TEXT("MRT_ShadowObjects"), m_pShader, m_pVIBuffer)))
	//	return E_FAIL;

	return S_OK;


}
#endif // _DEBUG

HRESULT CRenderer::Render_Metallic(_float fTimeDelta)
{
	if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_Matallic"))))
		return E_FAIL;

	for (auto& pRenderObject : m_RenderObjects[RG_PLAYER_METALLIC])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render(fTimeDelta);

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[RG_PLAYER_METALLIC].clear();

	if (FAILED(m_pRenderInstance->End_MRT()))
		return E_FAIL;
}

HRESULT CRenderer::Draw_AllBlackOut(_float fTimeDelta)
{

	if (m_isStartAllBlackOut == false)
		return S_OK;

	if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_AllBlackOut"))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->End_MRT()))
		return E_FAIL;

	if (m_isDownAllBlack == false)
		m_fAccAllBlackTime += fTimeDelta * 2.f;
	else
		m_fAccAllBlackTime -= fTimeDelta * 2.f;

	if (m_fAccAllBlackTime >= 1.f)
	{
		m_isDownAllBlack = true;
		m_fAccAllBlackTime = 1.f;
	}
	else if (m_fAccAllBlackTime <= 0)
	{
		m_isStartAllBlackOut = false;
		m_isDownAllBlack = false;
		m_fAccAllBlackTime = 0.f;
	}

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	/*
	_bool m_isStartBlackOut = { false };
	_float m_fAccBlackTime = { 0.f };
	const _float m_fBlackTime = { 1.f };
	*/

	if (FAILED(m_pShader->Bind_RawValue("g_isStartBlackOut", &m_isStartAllBlackOut, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_fAccBlackTime", &m_fAccAllBlackTime, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pShader, "g_Texture", TEXT("Target_AllBlackOut"))))
		return E_FAIL;

	m_pShader->Begin(13);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();


	return S_OK;

}

HRESULT CRenderer::Draw_AllWhiteOut(_float fTimeDelta)
{
	if (m_isStartAllWhiteOut == false)
		return S_OK;
	/*
	m_isStartAllWhiteOut = true;
	m_fAccAllWhiteTime = 1.f;
	m_isDownAllWhite = false;
	m_fDurationTime = fWhiteTime;
	m_fWhiteSpeed = fSpeed;
	*/
	if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_AllWhiteOut"))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->End_MRT()))
		return E_FAIL;

	if (m_isDownAllWhite == false)
		m_fAccAllWhiteTime -= fTimeDelta;
	else
		m_fAccAllWhiteTime -= fTimeDelta * m_fAllWhiteSpeed;

	if (m_fAccAllWhiteTime <= 1.f)
	{
		m_isDownAllWhite = true;
		//m_fAccAllWhiteTime = 1.f;
	}
	else if (m_fAccAllWhiteTime <= 0)
	{
		m_isStartAllWhiteOut = false;
		m_isDownAllWhite = false;
		m_fAccAllWhiteTime = 0.f;
	}

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	/*
	_bool m_isStartBlackOut = { false };
	_float m_fAccBlackTime = { 0.f };
	const _float m_fBlackTime = { 1.f };
	*/

	if (FAILED(m_pShader->Bind_RawValue("g_isStartBlackOut", &m_isStartAllWhiteOut, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_fAccBlackTime", &m_fAccAllWhiteTime, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pShader, "g_Texture", TEXT("Target_AllWhiteOut"))))
		return E_FAIL;

	m_pShader->Begin(13);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();


	return S_OK;
}


HRESULT CRenderer::Draw_MapBlackOut(_float fTimeDelta)
{
	if (m_fAccBlackTime == 0.f && m_isStartBlackOut == false)
		return S_OK;

	if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_BlackOut"))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->End_MRT()))
		return E_FAIL;

	if(m_isStartBlackOut == true)
		m_fAccBlackTime += fTimeDelta;
	else
		m_fAccBlackTime -= fTimeDelta;

	if (m_fAccBlackTime >= m_fBlackTime)
		m_fAccBlackTime = m_fBlackTime;
	else if (m_fAccBlackTime <= 0)
		m_fAccBlackTime = 0.f;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	/*
	_bool m_isStartBlackOut = { false };
	_float m_fAccBlackTime = { 0.f };
	const _float m_fBlackTime = { 1.f };
	*/
	
	if (FAILED(m_pShader->Bind_RawValue("g_isStartBlackOut", &m_isStartBlackOut, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_fAccBlackTime", &m_fAccBlackTime, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pShader, "g_Texture", TEXT("Target_BlackOut"))))
		return E_FAIL;

	m_pShader->Begin(8);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();
	

	return S_OK;
}

HRESULT CRenderer::Draw_OutLine_Effect()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pShader, "g_DiffuseTexture", TEXT("Target_Diffuse"))))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pShader, "g_DepthTexture", TEXT("Target_Depth"))))
		return E_FAIL;

	m_pShader->Begin(7);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::Draw_AllGlow_Effect(_int isPri)
{
	_int iLoopCount = m_pRenderInstance->Get_FrameGlowCount(isPri);

	for (size_t i = 0; i < iLoopCount; i++)
	{

		if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_Down")))) 
			return E_FAIL;

		if (FAILED(m_pGlowShader->Bind_Matrix("g_WorldMatrix", &m_DownWorldMatrix)))
			return E_FAIL;
		if (FAILED(m_pGlowShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
			return E_FAIL;
		if (FAILED(m_pGlowShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
			return E_FAIL;

		_float2 fTexSize = { 1920.f, 1080.f };
		if (FAILED(m_pGlowShader->Bind_RawValue("g_DownTexSize", &fTexSize, sizeof(_float2))))
			return E_FAIL;

		_float2 fSamplingSize = { 4.f,4.f };
		if (FAILED(m_pGlowShader->Bind_RawValue("g_DownSamplingSize", &fSamplingSize, sizeof(_float2))))
			return E_FAIL;

		// g_DownTexSize, g_DownSamplingSize
		if (FAILED(m_pRenderInstance->Bind_RT_EffectShaderResource(m_pGlowShader, "g_Texture", TEXT("Target_AllGlowDiffuse_"),isPri)))
			return E_FAIL;


		m_pGlowShader->Begin(3);
		m_pVIBuffer->Bind_Buffers();
		m_pVIBuffer->Render();

		if (FAILED(m_pRenderInstance->End_MRT()))
			return E_FAIL;

		if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_DownSecond"))))
			return E_FAIL;

		if (FAILED(m_pGlowShader->Bind_Matrix("g_WorldMatrix", &m_DownWorldMatrix_Second)))
			return E_FAIL;

		if (FAILED(m_pGlowShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
			return E_FAIL;

		if (FAILED(m_pGlowShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
			return E_FAIL;

		fTexSize = { 1920.f * 0.5f, 1080.f * 0.5f };
		if (FAILED(m_pGlowShader->Bind_RawValue("g_DownTexSize", &fTexSize, sizeof(_float2))))
			return E_FAIL;

		fSamplingSize = { 6.f,6.f };
		if (FAILED(m_pGlowShader->Bind_RawValue("g_DownSamplingSize", &fSamplingSize, sizeof(_float2))))
			return E_FAIL;

		if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pGlowShader, "g_Texture", TEXT("Target_DownTarget"))))
			return E_FAIL;

		m_pGlowShader->Begin(3);
		m_pVIBuffer->Bind_Buffers();
		m_pVIBuffer->Render();

		if (FAILED(m_pRenderInstance->End_MRT()))
			return E_FAIL;
		//-------------------------------------------
		//return S_OK;
		//-------------------------------------------
		if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_Blur_X"))))
			return E_FAIL;

		if (FAILED(m_pGlowShader->Bind_Matrix("g_WorldMatrix", &m_DownWorldMatrix_Second)))
			return E_FAIL;
		if (FAILED(m_pGlowShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
			return E_FAIL;
		if (FAILED(m_pGlowShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
			return E_FAIL;

		if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pGlowShader, "g_Texture", TEXT("Target_DownTarget_Second"))))
			return E_FAIL;

		m_pGlowShader->Begin(0);
		m_pVIBuffer->Bind_Buffers();
		m_pVIBuffer->Render();

		if (FAILED(m_pRenderInstance->End_MRT()))
			return E_FAIL;
		//****************************************
		if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_Blur_Y"))))
			return E_FAIL;

		if (FAILED(m_pGlowShader->Bind_Matrix("g_WorldMatrix", &m_DownWorldMatrix_Second)))
			return E_FAIL;
		if (FAILED(m_pGlowShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
			return E_FAIL;
		if (FAILED(m_pGlowShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
			return E_FAIL;

		if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pGlowShader, "g_Texture", TEXT("Target_Blur_X"))))
			return E_FAIL;

		m_pGlowShader->Begin(1);
		m_pVIBuffer->Bind_Buffers();
		m_pVIBuffer->Render();

		if (FAILED(m_pRenderInstance->End_MRT()))
			return E_FAIL;

		//***************************************************

		if (FAILED(m_pGlowShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
			return E_FAIL;
		if (FAILED(m_pGlowShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
			return E_FAIL;
		if (FAILED(m_pGlowShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
			return E_FAIL;

		if (FAILED(m_pRenderInstance->Bind_RT_EffectShaderResource(m_pGlowShader, "g_Texture", TEXT("Target_AllGlowDiffuse_"), isPri)))
			return E_FAIL;
		//if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pGlowShader, "g_BlurTexture", TEXT("Target_UpTarget_Second"))))
		//	return E_FAIL;
		if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pGlowShader, "g_BlurTexture", TEXT("Target_Blur_Y"))))
			return E_FAIL;

		_float fCulGlowFactor = m_pRenderInstance->Get_CulGlowIndex(isPri);
		m_pGlowShader->Bind_RawValue("g_fAllGlowFactor", &fCulGlowFactor, sizeof(_float));

		/*if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_AllGlowDiffuse"), TEXT("Target_AllGlowDiffuse"))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_AllGlowDiffuse"), TEXT("Target_AllGlowAlpha"))))*/

		m_pGlowShader->Begin(12);

		m_pVIBuffer->Bind_Buffers();
		m_pVIBuffer->Render();

		m_pRenderInstance->Zero_CurGlowIndex(isPri);
	}

	return S_OK;
}

HRESULT CRenderer::Draw_Glow(CShader* pShader, GLOW_DESC* pDesc)
{
	if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_Down"))))
		return E_FAIL;

	if (FAILED(pShader->Bind_Matrix("g_WorldMatrix", &m_DownWorldMatrix)))
		return E_FAIL;
	if (FAILED(pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	_float2 fTexSize = { 1920.f, 1080.f };
	if (FAILED(pShader->Bind_RawValue("g_DownTexSize", &fTexSize, sizeof(_float2))))
		return E_FAIL;

	_float2 fSamplingSize = { 4.f,4.f };
	if (FAILED(pShader->Bind_RawValue("g_DownSamplingSize", &fSamplingSize, sizeof(_float2))))
		return E_FAIL;

	// g_DownTexSize, g_DownSamplingSize
	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(pShader, "g_Texture", TEXT("Target_GlowDiffuse"))))
		return E_FAIL;


	pShader->Begin(3);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if (FAILED(m_pRenderInstance->End_MRT()))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_DownSecond"))))
		return E_FAIL;

	if (FAILED(pShader->Bind_Matrix("g_WorldMatrix", &m_DownWorldMatrix_Second)))
		return E_FAIL;

	if (FAILED(pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	fTexSize = { 1920.f * 0.5f, 1080.f * 0.5f };
	if (FAILED(pShader->Bind_RawValue("g_DownTexSize", &fTexSize, sizeof(_float2))))
		return E_FAIL;

	fSamplingSize = { 6.f,6.f };
	if (FAILED(pShader->Bind_RawValue("g_DownSamplingSize", &fSamplingSize, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(pShader, "g_Texture", TEXT("Target_DownTarget"))))
		return E_FAIL;

	pShader->Begin(3);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if (FAILED(m_pRenderInstance->End_MRT()))
		return E_FAIL;
	//-------------------------------------------
	//return S_OK;
	//-------------------------------------------
	if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_Blur_X"))))
		return E_FAIL;

	if (FAILED(pShader->Bind_Matrix("g_WorldMatrix", &m_DownWorldMatrix_Second)))
		return E_FAIL;
	if (FAILED(pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(pShader, "g_Texture", TEXT("Target_DownTarget_Second"))))
		return E_FAIL;

	pShader->Begin(0);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if (FAILED(m_pRenderInstance->End_MRT()))
		return E_FAIL;
	//****************************************
	if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_Blur_Y"))))
		return E_FAIL;

	if (FAILED(pShader->Bind_Matrix("g_WorldMatrix", &m_DownWorldMatrix_Second)))
		return E_FAIL;
	if (FAILED(pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(pShader, "g_Texture", TEXT("Target_Blur_X"))))
		return E_FAIL;

	pShader->Begin(1);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if (FAILED(m_pRenderInstance->End_MRT()))
		return E_FAIL;

	//***************************************************

	if (FAILED(pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(pShader, "g_Texture", TEXT("Target_GlowDiffuse"))))
		return E_FAIL;
	//if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(pShader, "g_BlurTexture", TEXT("Target_UpTarget_Second"))))
	//	return E_FAIL;
	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(pShader, "g_BlurTexture", TEXT("Target_Blur_Y"))))
		return E_FAIL;

	if (nullptr != pDesc)
	{
		/*float g_GlowFactor;
		float4 g_GlowFilterColor;*/
		pShader->Bind_RawValue("g_GlowFactor", &pDesc->fGlowFactor, sizeof(_float));
		pShader->Bind_RawValue("g_GlowFilterColor", &pDesc->vGlowColor, sizeof(_float4));
		pShader->Begin(pDesc->iPassIndex);
	}
	else
		pShader->Begin(2);

	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::Draw_MapBloom()
{
	if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_Down"))))
		return E_FAIL;

	if (FAILED(m_pGlowShader->Bind_Matrix("g_WorldMatrix", &m_DownWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pGlowShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pGlowShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	_float2 fTexSize = { 1920.f, 1080.f };
	if (FAILED(m_pGlowShader->Bind_RawValue("g_DownTexSize", &fTexSize, sizeof(_float2))))
		return E_FAIL;

	_float2 fSamplingSize = { 4.f,4.f };
	if (FAILED(m_pGlowShader->Bind_RawValue("g_DownSamplingSize", &fSamplingSize, sizeof(_float2))))
		return E_FAIL;
	
	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pGlowShader, "g_Texture", TEXT("Target_MapBloomAlpha"))))
		return E_FAIL;

	m_pGlowShader->Begin(3);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if (FAILED(m_pRenderInstance->End_MRT()))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_DownSecond"))))
		return E_FAIL;

	if (FAILED(m_pGlowShader->Bind_Matrix("g_WorldMatrix", &m_DownWorldMatrix_Second)))
		return E_FAIL;

	if (FAILED(m_pGlowShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pGlowShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	fTexSize = { 1920.f * 0.5f, 1080.f * 0.5f };
	if (FAILED(m_pGlowShader->Bind_RawValue("g_DownTexSize", &fTexSize, sizeof(_float2))))
		return E_FAIL;

	fSamplingSize = { 6.f,6.f };
	if (FAILED(m_pGlowShader->Bind_RawValue("g_DownSamplingSize", &fSamplingSize, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pGlowShader, "g_Texture", TEXT("Target_DownTarget"))))
		return E_FAIL;

	m_pGlowShader->Begin(3);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if (FAILED(m_pRenderInstance->End_MRT()))
		return E_FAIL;
	//-------------------------------------------
	//return S_OK;
	//-------------------------------------------
	if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_Blur_X"))))
		return E_FAIL;

	if (FAILED(m_pGlowShader->Bind_Matrix("g_WorldMatrix", &m_DownWorldMatrix_Second)))
		return E_FAIL;
	if (FAILED(m_pGlowShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pGlowShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pGlowShader, "g_Texture", TEXT("Target_DownTarget_Second"))))
		return E_FAIL;

	m_pGlowShader->Begin(0);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if (FAILED(m_pRenderInstance->End_MRT()))
		return E_FAIL;
	//****************************************
	if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_Blur_Y"))))
		return E_FAIL;

	if (FAILED(m_pGlowShader->Bind_Matrix("g_WorldMatrix", &m_DownWorldMatrix_Second)))
		return E_FAIL;
	if (FAILED(m_pGlowShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pGlowShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pGlowShader, "g_Texture", TEXT("Target_Blur_X"))))
		return E_FAIL;

	m_pGlowShader->Begin(1);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if (FAILED(m_pRenderInstance->End_MRT()))
		return E_FAIL;

	//***************************************************

	if (FAILED(m_pGlowShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pGlowShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pGlowShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pGlowShader, "g_Texture", TEXT("Target_MapBloomDiffuse"))))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pGlowShader, "g_BlurTexture", TEXT("Target_Blur_Y"))))
		return E_FAIL;

	m_pGlowShader->Begin(13);

	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::Draw_WhiteBlack_Mode(_float fTimeDelta)
{
	if (m_pDoneCheck == nullptr)
		return S_OK;

	if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_WhiteOut"))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->End_MRT()))
		return E_FAIL;

	if (m_isRockStart == true)
	{
		m_fSpriteAccTime += fTimeDelta * m_fWhiteSpeed;
		m_fAccRockTime += fTimeDelta * m_fWhiteSpeed;
		if (m_fSpriteAccTime >= 0.05f)
		{
			m_fSpriteAccTime = 0.f;
			--m_fSpriteCurPos.x;

			if (m_fSpriteCurPos.x < 0)
			{
				m_fSpriteCurPos.x = 4.f;
				--m_fSpriteCurPos.y;
			}

			if (m_fSpriteCurPos.y < 0)
			{
				m_fSpriteCurPos.x = 0.f;
				m_fSpriteCurPos.y = 0.f;
				m_isSpriteEnd = true;
			}
		}

		if (m_fAccRockTime >= 0.5f && m_isMaintainWhite == false && m_isEndWhiteOut == false)
			m_isStartWhiteOut = true;

		if (m_isSpriteEnd == true)
		{
			m_fAccWhiteTime = 2.f;
			m_isMaintainWhite = false;
			m_isRockStart = false;
			m_isEndWhiteOut = true;
			m_fAccRockTime = 0.f;
			*m_pDoneCheck = true;
		}
	}

	if (m_isStartWhiteOut == true)
	{
		/* 화이트 스피드 테스트 */
		m_fAccWhiteTime += fTimeDelta * m_fWhiteSpeed;
		//m_fAccWhiteTime += fTimeDelta;
		
		if (m_fAccWhiteTime >= 2.5f)
		{
			m_fAccWhiteTime = 2.5f;
			m_isMaintainWhite = true;
			m_isEndWhiteOut = false;
			m_isStartWhiteOut = false;
		}
	}

	if (m_isEndWhiteOut == true)
	{
		m_fAccWhiteTime -= fTimeDelta;

		if (m_fAccWhiteTime <= 0.f)
		{ 
			m_isStartWhiteOut = false;
			m_pDoneCheck = nullptr;
		}
	}

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_isEndWhiteOut", &m_isEndWhiteOut, sizeof(_bool))))
		return E_FAIL;
	
	_bool isStartCheck = m_isMaintainWhite + m_isStartWhiteOut;
	if (FAILED(m_pShader->Bind_RawValue("g_isStartBlackOut", &isStartCheck, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_fAccBlackTime", &m_fAccWhiteTime, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_fSpriteSize", &m_fSpriteSize, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_fSpriteCurPos", &m_fSpriteCurPos, sizeof(_float2))))
		return E_FAIL;

	// g_fSpriteSizeg_fSpriteCurPos
	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pShader, "g_Texture", TEXT("Target_WhiteOut"))))
		return E_FAIL;

	if (m_isEndWhiteOut == false)
	{
		if (m_vWhiteDir.x == 1)
		{
			if(FAILED(m_pEastFinish_TextureCom->Bind_ShaderResource(m_pShader,"g_DiffuseTexture",0)))
				return E_FAIL;

		}
		else
		{
			if (FAILED(m_pEastFinish_TextureCom->Bind_ShaderResource(m_pShader, "g_DiffuseTexture", 1)))
				return E_FAIL;
		}

	}
	else
	{
		if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pShader, "g_DiffuseTexture", TEXT("Target_WhiteOut"))))
			return E_FAIL;

	}
	
	m_pShader->Begin(10);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();
}

HRESULT CRenderer::Initialize_RenderTarget()
{
	D3D11_VIEWPORT			ViewportDesc{};
	_uint					iNumViewport = { 1 };
	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

#pragma region Light_Object
	if (FAILED(m_pRenderInstance->Add_RenderTarget(TEXT("Target_Diffuse"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, XMVectorSet(1.f, 1.f, 1.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_RenderTarget(TEXT("Target_Normal"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_RenderTarget(TEXT("Target_Depth"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, XMVectorSet(0.f, 0.f, -1.f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))))
		return E_FAIL;
#pragma endregion

#pragma region Player
	if (FAILED(m_pRenderInstance->Add_RenderTarget(TEXT("Target_Player_Diffuse"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, XMVectorSet(1.f, 1.f, 1.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_RenderTarget(TEXT("Target_Player_Normal"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_RenderTarget(TEXT("Target_Player_Depth"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Add_RenderTarget(TEXT("Target_Player_AuraMask"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, XMVectorSet(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_Player"), TEXT("Target_Player_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_Player"), TEXT("Target_Player_Normal"))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_Player"), TEXT("Target_Player_Depth"))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_Player"), TEXT("Target_Player_AuraMask"))))
		return E_FAIL;
#pragma endregion

#pragma region EffectPicking
	if (FAILED(m_pRenderInstance->Add_RenderTarget(TEXT("Target_PickDepth"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, XMVectorSet(0.f, 0.f, -1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_EffectToolPick"), TEXT("Target_PickDepth"))))
		return E_FAIL;
#pragma endregion

#pragma region Light
	if (FAILED(m_pRenderInstance->Add_RenderTarget(TEXT("Target_Shade"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, XMVectorSet(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_RenderTarget(TEXT("Target_Specular"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, XMVectorSet(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
		return E_FAIL;
#pragma endregion

#pragma region Glow
	if (FAILED(m_pRenderInstance->Add_RenderTarget(TEXT("Target_GlowDiffuse"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, XMVectorSet(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_RenderTarget(TEXT("Target_GlowAlpha"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, XMVectorSet(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_GlowDiffuse"), TEXT("Target_GlowDiffuse"))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_GlowDiffuse"), TEXT("Target_GlowAlpha"))))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Add_RenderTarget(TEXT("Target_MapBloomDiffuse"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, XMVectorSet(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_RenderTarget(TEXT("Target_MapBloomAlpha"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, XMVectorSet(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_RenderTarget(TEXT("Target_MapBloomDepth"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_BloomDiffuse"), TEXT("Target_MapBloomDiffuse"))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_BloomDiffuse"), TEXT("Target_MapBloomAlpha"))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_BloomDiffuse"), TEXT("Target_MapBloomDepth"))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_RenderTarget(TEXT("Target_StageDepth"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_StageDepth"), TEXT("Target_StageDepth"))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_RenderTarget(TEXT("Target_DownTarget"), ViewportDesc.Width / 2, ViewportDesc.Height / 2, DXGI_FORMAT_B8G8R8A8_UNORM, XMVectorSet(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_RenderTarget(TEXT("Target_DownTarget_Second"), ViewportDesc.Width / 4, ViewportDesc.Height / 4, DXGI_FORMAT_B8G8R8A8_UNORM, XMVectorSet(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_Down"), TEXT("Target_DownTarget"))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_DownSecond"), TEXT("Target_DownTarget_Second"))))
		return E_FAIL;
#pragma endregion

#pragma region Glow_Blur
	if (FAILED(m_pRenderInstance->Add_RenderTarget(TEXT("Target_Blur_X"), ViewportDesc.Width / 4, ViewportDesc.Height / 4, DXGI_FORMAT_B8G8R8A8_UNORM, XMVectorSet(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_RenderTarget(TEXT("Target_Blur_Y"), ViewportDesc.Width / 4, ViewportDesc.Height / 4, DXGI_FORMAT_B8G8R8A8_UNORM, XMVectorSet(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_Blur_X"), TEXT("Target_Blur_X"))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_Blur_Y"), TEXT("Target_Blur_Y"))))
		return E_FAIL;
	/* 업샘플링 임시 */
	if (FAILED(m_pRenderInstance->Add_RenderTarget(TEXT("Target_UpTarget_Second"), ViewportDesc.Width / 2, ViewportDesc.Height / 2, DXGI_FORMAT_B8G8R8A8_UNORM, XMVectorSet(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_UpSecond"), TEXT("Target_UpTarget_Second"))))
		return E_FAIL;
#pragma endregion

#pragma region PlayerBlur
	if (FAILED(m_pRenderInstance->Add_RenderTarget(TEXT("Target_PlayerDefferd"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, XMVectorSet(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_PlayerDefferd"), TEXT("Target_PlayerDefferd"))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_RenderTarget(TEXT("Target_Player_Blur_X"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, XMVectorSet(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_RenderTarget(TEXT("Target_Player_Blur_Y"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, XMVectorSet(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_Player_Blur_X"), TEXT("Target_Player_Blur_X"))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_Player_Blur_Y"), TEXT("Target_Player_Blur_Y"))))
		return E_FAIL;


	if (FAILED(m_pRenderInstance->Add_RenderTarget(TEXT("Target_All_Blur_X"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, XMVectorSet(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_RenderTarget(TEXT("Target_All_Blur_Y"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, XMVectorSet(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_All_Blur_X"), TEXT("Target_All_Blur_X"))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_All_Blur_Y"), TEXT("Target_All_Blur_Y"))))
		return E_FAIL;
#pragma endregion

	/* 그림자 임시 */
	if (FAILED(m_pRenderInstance->Add_RenderTarget(TEXT("Target_LightDepth"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_ShadowObjects"), TEXT("Target_LightDepth"))))
		return E_FAIL;

#pragma region All_EffectGlow
	for (size_t i = 0; i < 10; i++)
	{
		if (FAILED(m_pRenderInstance->Add_RenderTarget(TEXT("Target_AllGlowDiffuse_") + to_wstring(i), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, XMVectorSet(0.f, 0.f, 0.f, 0.f))))
			return E_FAIL;
		if (FAILED(m_pRenderInstance->Add_RenderTarget(TEXT("Target_AllGlowAlpha_") + to_wstring(i), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, XMVectorSet(0.f, 0.f, 0.f, 0.f))))
			return E_FAIL;
		if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_AllGlowDiffuse_") + to_wstring(i), TEXT("Target_AllGlowDiffuse_") + to_wstring(i))))
			return E_FAIL;
		if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_AllGlowDiffuse_") + to_wstring(i), TEXT("Target_AllGlowAlpha_") + to_wstring(i))))
			return E_FAIL;
	}
#pragma endregion

#pragma region BlackOut

	if (FAILED(m_pRenderInstance->Add_RenderTarget(TEXT("Target_BlackOut"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, XMVectorSet(0.f, 0.f, 0.f, 0.5f))))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_BlackOut"), TEXT("Target_BlackOut"))))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Add_RenderTarget(TEXT("Target_AllBlackOut"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, XMVectorSet(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_AllBlackOut"), TEXT("Target_AllBlackOut"))))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Add_RenderTarget(TEXT("Target_WhiteOut"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, XMVectorSet(1.f, 1.f, 1.f, 0.5f))))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_WhiteOut"), TEXT("Target_WhiteOut"))))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Add_RenderTarget(TEXT("Target_AllWhiteOut"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_AllWhiteOut"), TEXT("Target_AllWhiteOut"))))
		return E_FAIL;

#pragma endregion	

#pragma region Distortion
	if (FAILED(m_pRenderInstance->Add_RenderTarget(TEXT("Target_Distortion"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, XMVectorSet(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_Distortion"), TEXT("Target_Distortion"))))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Add_RenderTarget(TEXT("Target_ResultDistortion_BackBuffer"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, XMVectorSet(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_ResultDistortion_BackBuffer"), TEXT("Target_ResultDistortion_BackBuffer"))))
		return E_FAIL;
#pragma endregion	

#pragma region Metalic

	if (FAILED(m_pRenderInstance->Add_RenderTarget(TEXT("Target_Metallic"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, XMVectorSet(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_Matallic"), TEXT("Target_Metallic"))))
		return E_FAIL;

#pragma endregion	

#pragma region PlayerPart

	if (FAILED(m_pRenderInstance->Add_RenderTarget(TEXT("Target_Player_Part"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, XMVectorSet(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_Player_Part"), TEXT("Target_Player_Part"))))
		return E_FAIL;

#pragma endregion	
	return S_OK;
}

HRESULT CRenderer::Draw_Test_PostProcess(_float fTImeDelta)
{
	if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_ResultDistortion_BackBuffer"))))
		return E_FAIL;

	if (FAILED(m_pDistortionShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pDistortionShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pDistortionShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	//if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pDistortionShaderCom, "g_Texture", TEXT("Target_Distortion"))))
	//	return E_FAIL;

	if (FAILED(m_pDistortionShaderCom->Bind_ShaderResourceView("g_Texture", m_pBackBufferSRV)))
		return E_FAIL;

	m_pDistortionShaderCom->Begin(2);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if (FAILED(m_pRenderInstance->End_MRT()))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_All_Blur_X"))))
		return E_FAIL;

	if (FAILED(m_pGlowShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pGlowShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pGlowShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pGlowShader, "g_Texture", TEXT("Target_ResultDistortion_BackBuffer"))))
		return E_FAIL;

	m_pGlowShader->Begin(15);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if (FAILED(m_pRenderInstance->End_MRT()))
		return E_FAIL;
	//****************************************
	if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_All_Blur_Y"))))
		return E_FAIL;

	if (FAILED(m_pGlowShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pGlowShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pGlowShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pGlowShader, "g_Texture", TEXT("Target_All_Blur_X"))))
		return E_FAIL;

	m_pGlowShader->Begin(16);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if (FAILED(m_pRenderInstance->End_MRT()))
		return E_FAIL;

	//***************************************************

	if (FAILED(m_pGlowShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pGlowShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pGlowShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pGlowShader, "g_Texture", TEXT("Target_ResultDistortion_BackBuffer"))))
		return E_FAIL;
	//if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(pShader, "g_BlurTexture", TEXT("Target_UpTarget_Second"))))
	//	return E_FAIL;
	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pGlowShader, "g_BlurTexture", TEXT("Target_All_Blur_Y"))))
		return E_FAIL;

	m_pGlowShader->Begin(14);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();
	

	/*
	if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_ResultDistortion_BackBuffer"))))
		return E_FAIL;

	if (FAILED(m_pDistortionShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pDistortionShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pDistortionShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	//if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pDistortionShaderCom, "g_Texture", TEXT("Target_Distortion"))))
	//	return E_FAIL;

	if (FAILED(m_pDistortionShaderCom->Bind_ShaderResourceView("g_BackBufferTexture", m_pBackBufferSRV)))
		return E_FAIL;

	m_pDistortionShaderCom->Begin(5);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if (FAILED(m_pRenderInstance->End_MRT()))
		return E_FAIL;


	if (FAILED(m_pDistortionShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pDistortionShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pDistortionShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pDistortionShaderCom, "g_Texture", TEXT("Target_ResultDistortion_BackBuffer"))))
		return E_FAIL;

	m_pDistortionShaderCom->Begin(2);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();
	*/
	return S_OK;
}
void CRenderer::Switch_Test_PostProcess(_bool isTrue)
{

}

void CRenderer::Switch_BlackOut(_bool isTrue)
{
	m_isStartBlackOut = isTrue;
	//m_fAccBlackTime += 0.01f;
}

void CRenderer::Switch_AllBlackOut()
{
	m_isStartAllBlackOut = true;
	m_isDownAllBlack = false;
	m_fAccAllBlackTime = { 0.f };
}

void CRenderer::Start_AllWhiteOut(_float fWhiteTime, _float fSpeed)
{
	m_isStartAllWhiteOut = true;
	m_fAccAllWhiteTime = fWhiteTime;
	m_isDownAllWhite = false;
	m_fDurationTime = fWhiteTime;
	m_fAllWhiteSpeed = fSpeed;
}

void CRenderer::Start_WhiteOut(_float2 vDir, _bool* isDone, _float fWhiteSpeed)
{
	m_vWhiteDir = vDir;
	m_isSpriteEnd = false;
	m_fWhiteSpeed = fWhiteSpeed;
	m_isStartWhiteOut = false;
	m_isMaintainWhite = false;
	m_isEndWhiteOut = false;
	m_fAccWhiteTime = 0.f;
	m_pDoneCheck = isDone;
	m_isRockStart = true;
	m_fSpriteSize = { 1.f / 5.f,1.f / 12.f };
//	m_fSpriteSize = { 1/1920.f,1/1080.f };
	//m_fSpriteSize = { 1 / 384.f,1 / 90.f };
	m_fSpriteCurPos = { 4.f,11.f };
	m_fAccRockTime = 0.f;
}

CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderer* pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pDevice, pContext)))
	{
		MSG_BOX(TEXT("Failed to Created : CRenderer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRenderer::Free()
{
	__super::Free();

	for (size_t i = 0; i < RG_END; i++)
	{
		for (auto& pRenderObject : m_RenderObjects[i])
			Safe_Release(pRenderObject);
		m_RenderObjects[i].clear();
	}
	for (auto& pRenderObject : m_DebugComponent)
		Safe_Release(pRenderObject);
	m_DebugComponent.clear();


	for (auto& pComponent : m_DebugComponent)
		Safe_Release(pComponent);
	
	Safe_Release(m_pAuraTextureCom);
	Safe_Release(m_pDistortionTransformCom);
	Safe_Release(m_pDistortionTextureCom);
	Safe_Release(m_pDistortionShaderCom);
	Safe_Release(m_pBackBufferSRV);
	Safe_Release(m_pShadowDSV);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pRenderInstance);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pGlowShader);
	Safe_Release(m_pUI_GlowShader);
	Safe_Release(m_pEastFinish_TextureCom);
}
