#include "..\Public\Lobby_Renderer.h"

#include "GameObject.h"
#include "GameInstance.h"
#include "RenderInstance.h"

#include "Shader.h"
#include "VIBuffer_Rect.h"

_uint		g_iLobbySizeX = 1920;
_uint		g_iLobbySizeY = 1080;

CLobby_Renderer::CLobby_Renderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pRenderInstance{ CRenderInstance::Get_Instance() }
	, m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pRenderInstance);
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CLobby_Renderer::Initialize()
{
	D3D11_VIEWPORT			ViewportDesc{};
	_uint					iNumViewport = { 1 };
	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	/* 렌더타겟들을 추가한다. */
	if (FAILED(m_pRenderInstance->Add_RenderTarget(TEXT("Target_Lobby_Diffuse"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, XMVectorSet(1.f, 1.f, 1.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_RenderTarget(TEXT("Target_Lobby_Normal"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_RenderTarget(TEXT("Target_Lobby_Depth"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_RenderTarget(TEXT("Target_Lobby_PickDepth"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Add_RenderTarget(TEXT("Target_Lobby_Shade"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, XMVectorSet(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_RenderTarget(TEXT("Target_Lobby_Specular"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, XMVectorSet(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Add_RenderTarget(TEXT("Target_Lobby_LightDepth"), g_iLobbySizeX, g_iLobbySizeY, DXGI_FORMAT_R32G32B32A32_FLOAT, XMVectorSet(1.f, 1.f,1.f, 1.f))))
		return E_FAIL;


	/* 멀티렌더타겟에 타겟들을 모아놓는다. */
	if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_Lobby_GameObjects"), TEXT("Target_Lobby_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_Lobby_GameObjects"), TEXT("Target_Lobby_Normal"))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_Lobby_GameObjects"), TEXT("Target_Lobby_Depth"))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_Lobby_GameObjects"), TEXT("Target_Lobby_PickDepth"))))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_Lobby_LightAcc"), TEXT("Target_Lobby_Shade"))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_Lobby_LightAcc"), TEXT("Target_Lobby_Specular"))))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Add_MRT(TEXT("MRT_Lobby_ShadowObjects"), TEXT("Target_Lobby_LightDepth"))))
		return E_FAIL;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 1.f));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_DeferredLobby.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	ID3D11Texture2D* pDepthStencilTexture = nullptr;



	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = g_iLobbySizeX;
	TextureDesc.Height = g_iLobbySizeY;
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

	/* RenderTarget */
	/* ShaderResource */
	/* DepthStencil */

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pShadowDSV)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);

#ifdef _DEBUG
	if (FAILED(m_pRenderInstance->Ready_RT_Debug(TEXT("Target_Lobby_Diffuse"), 100.f, 100.f, 200.0f, 200.0f)))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Ready_RT_Debug(TEXT("Target_Lobby_Normal"), 100.f, 300.f, 200.0f, 200.0f)))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Ready_RT_Debug(TEXT("Target_Lobby_Depth"), 100.f, 500.f, 200.0f, 200.0f)))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Ready_RT_Debug(TEXT("Target_Lobby_PickDepth"), 600.f, 100.f, 200.0f, 200.0f)))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Ready_RT_Debug(TEXT("Target_Lobby_Shade"), 350.f, 150.f, 300.f, 300.f)))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Ready_RT_Debug(TEXT("Target_Lobby_Specular"), 350.f, 450.f, 300.f, 300.f)))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Ready_RT_Debug(TEXT("Target_Lobby_LightDepth"), ViewportDesc.Width - 150.0f, 150.f, 300.f, 300.f)))
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CLobby_Renderer::Add_LobbyRenderObject(RENDERGROUP eRenderGroup, CGameObject* pRenderObject)
{
	if (eRenderGroup >= RG_END)
		return E_FAIL;

	m_RenderObjects[eRenderGroup].emplace_back(pRenderObject);

	Safe_AddRef(pRenderObject);

	return S_OK;
}

HRESULT CLobby_Renderer::Add_LobbyDebugComponent(CComponent* pDebugComponent)
{
	m_DebugComponent.emplace_back(pDebugComponent);

	Safe_AddRef(pDebugComponent);

	return S_OK;
}

HRESULT CLobby_Renderer::Draw(_float fTimeDelta)
{

	if (FAILED(Render_Priority(fTimeDelta)))
		return E_FAIL;

	if (FAILED(Render_ShadowObj(fTimeDelta)))
		return E_FAIL;

	if (FAILED(Render_NonBlend(fTimeDelta)))
		return E_FAIL;

	if (FAILED(Render_Lights(fTimeDelta)))
		return E_FAIL;

	if (FAILED(Render_Deferred(fTimeDelta)))
		return E_FAIL;

	if (FAILED(Render_NonLight(fTimeDelta)))
		return E_FAIL;

	if (FAILED(Render_Blend(fTimeDelta)))
		return E_FAIL;

	if (FAILED(Render_UI(fTimeDelta)))
		return E_FAIL;

#ifdef _DEBUG
	if (FAILED(Render_Debug(fTimeDelta)))
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CLobby_Renderer::Render_Priority(_float fTimeDelta)
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

HRESULT CLobby_Renderer::Render_ShadowObj(_float fTimeDelta)
{
	/* Target_Lobby_LightDepth */
	if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_Lobby_ShadowObjects"), m_pShadowDSV)))
		return E_FAIL;

	D3D11_VIEWPORT			ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)g_iLobbySizeX;
	ViewPortDesc.Height = (_float)g_iLobbySizeY;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);


	for (auto& pRenderObject : m_RenderObjects[RG_SHADOWOBJ])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render(fTimeDelta);

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[RG_SHADOWOBJ].clear();

	if (FAILED(m_pRenderInstance->End_MRT()))
		return E_FAIL;

	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)1920.0f;
	ViewPortDesc.Height = (_float)1080.0f;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);

	return S_OK;
}

HRESULT CLobby_Renderer::Render_NonBlend(_float fTimeDelta)
{
	/* Diffuse + Normal */
	if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_Lobby_GameObjects"))))
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
HRESULT CLobby_Renderer::Render_Lights(_float fTimeDelta)
{
	/* 빛 연산한 결과를 렌더타겟에 기록해준다. (명암 + ) */
	/* Bind : Shade */
	if (FAILED(m_pRenderInstance->Begin_MRT(TEXT("MRT_Lobby_LightAcc"))))
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

	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pShader, "g_NormalTexture", TEXT("Target_Lobby_Normal"))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pShader, "g_DepthTexture", TEXT("Target_Lobby_Depth"))))
		return E_FAIL;

	_float4 camPosition = m_pGameInstance->Get_CamPosition_Float4();
	if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", &camPosition, sizeof(_float4))))
		return E_FAIL;

	m_pVIBuffer->Bind_Buffers();

	m_pRenderInstance->Render_Lights(CLight_Manager::LIGHT_BACKGROUND, m_pShader, m_pVIBuffer, "", fTimeDelta);

	if (FAILED(m_pRenderInstance->End_MRT()))
		return E_FAIL;


	return S_OK;
}
HRESULT CLobby_Renderer::Render_Deferred(_float fTimeDelta)
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

	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pShader, "g_DiffuseTexture", TEXT("Target_Lobby_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pShader, "g_ShadeTexture", TEXT("Target_Lobby_Shade"))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pShader, "g_SpecularTexture", TEXT("Target_Lobby_Specular"))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pShader, "g_LightDepthTexture", TEXT("Target_Lobby_LightDepth"))))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Bind_RT_ShaderResource(m_pShader, "g_DepthTexture", TEXT("Target_Lobby_Depth"))))
		return E_FAIL;

	_float4x4			LightViewMatrix, LightProjMatrix;

	XMStoreFloat4x4(&LightViewMatrix, XMMatrixLookAtLH(XMVectorSet(0.f, 10.f, 0.f, 1.f), XMVectorSet(1.f, -1.f, 1.f, 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	XMStoreFloat4x4(&LightProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), (_float)1920.0f / 1080.0f, 0.1f, 1000.f));

	if (FAILED(m_pShader->Bind_Matrix("g_LightViewMatrix", &LightViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_LightProjMatrix", &LightProjMatrix)))
		return E_FAIL;

	m_pShader->Begin(3);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();


	return S_OK;
}
HRESULT CLobby_Renderer::Render_NonLight(_float fTimeDelta)
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
//
//_bool Compare(CGameObject* pSour, CGameObject* pDest)
//{
//	return ((CBlendObject*)pSour)->Get_ViewZ() > ((CBlendObject*)pDest)->Get_ViewZ();
//}

HRESULT CLobby_Renderer::Render_Blend(_float fTimeDelta)
{
	//m_RenderObjects[RG_BLEND].sort([](CGameObject* pSour, CGameObject* pDest)->_bool
	//{
	//	return ((CBlendObject*)pSour)->Get_ViewZ() > ((CBlendObject*)pDest)->Get_ViewZ();
	//});

	for (auto& pRenderObject : m_RenderObjects[RG_BLEND])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render(fTimeDelta);

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[RG_BLEND].clear();

	return S_OK;
}

HRESULT CLobby_Renderer::Render_UI(_float fTimeDelta)
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

#ifdef _DEBUG

HRESULT CLobby_Renderer::Render_Debug(_float fTimeDelta)
{
	for (auto& pComponent : m_DebugComponent)
	{
		pComponent->Render(fTimeDelta);

		Safe_Release(pComponent);
	}

	m_DebugComponent.clear();

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->Render_RT_Debug(TEXT("MRT_Lobby_GameObjects"), m_pShader, m_pVIBuffer)))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Render_RT_Debug(TEXT("MRT_Lobby_LightAcc"), m_pShader, m_pVIBuffer)))
		return E_FAIL;
	if (FAILED(m_pRenderInstance->Render_RT_Debug(TEXT("MRT_Lobby_ShadowObjects"), m_pShader, m_pVIBuffer)))
		return E_FAIL;

	return S_OK;
}

#endif

CLobby_Renderer* CLobby_Renderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLobby_Renderer* pInstance = new CLobby_Renderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLobby_Renderer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLobby_Renderer::Free()
{
	__super::Free();

	for (size_t i = 0; i < RG_END; i++)
	{
		for (auto& pRenderObject : m_RenderObjects[i])
			Safe_Release(pRenderObject);
		m_RenderObjects[i].clear();
	}

	Safe_Release(m_pShadowDSV);

	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pRenderInstance);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
