#include "..\Public\Renderer.h"
#include "GameObject.h"

#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "Shader.h"
#include "BlendObject.h"
#include "UIObject.h"
#include "RenderTarget.h"
#include "PhysX_Manager.h"
#include "Renderer_Utility.h"

_uint      g_iSizeX = 8192;
_uint      g_iSizeY = 4608;


CRenderer::CRenderer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : m_pDevice(pDevice)
    , m_pContext(pContext)
    , m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CRenderer::Initialize()
{
    D3D11_VIEWPORT      Viewport;
    _uint            iNumViewport = { 1 };

    m_pContext->RSGetViewports(&iNumViewport, &Viewport);

    _matrix         WorldMatrix = XMMatrixIdentity();
    WorldMatrix.r[0] = XMVectorSet(Viewport.Width, 0.f, 0.f, 0.f);
    WorldMatrix.r[1] = XMVectorSet(0.f, Viewport.Height, 0.f, 0.f);

    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixTranspose(WorldMatrix));

    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());

    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(Viewport.Width, Viewport.Height, 0.f, 1.f)));

    /*WorldMatrix.r[0] = XMVectorSet(Viewport.Width * 2.f, 0.f, 0.f, 0.f);
    WorldMatrix.r[1] = XMVectorSet(0.f, Viewport.Height * 2.f, 0.f, 0.f);
    XMStoreFloat4x4(&m_AntiAliasingWorldMatrix, XMMatrixTranspose(WorldMatrix));
    XMStoreFloat4x4(&m_AntiAliasingProjMatrixTranspose, XMMatrixTranspose(XMMatrixOrthographicLH(Viewport.Width * 2.f, Viewport.Height * 2.f, 0.f, 1.f)));*/

    if (FAILED(m_pGameInstance->Bake_AntiAliasingDepthStencilView((_uint)Viewport.Width * 2.f, (_uint)Viewport.Height * 2.f)))
        RETURN_EFAIL;

    /* Target_Diffuse */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"), Viewport.Width, Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(1.f, 1.f, 1.f, 0.f))))
        RETURN_EFAIL;

    /* Target_Normal */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"), Viewport.Width, Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
        RETURN_EFAIL;

    /* Target_Depth */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Depth"), Viewport.Width, Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
        RETURN_EFAIL;

    /* Target_Depth */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Depth_Character"), Viewport.Width, Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
        RETURN_EFAIL;

    /* Target_Shade */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"), Viewport.Width, Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
        RETURN_EFAIL;

    /* Target_Specular */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Specular"), Viewport.Width, Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        RETURN_EFAIL;

    /* Target_Ambient */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Ambient"), Viewport.Width, Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        RETURN_EFAIL;

    /* Target_Shadow */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shadow"), Viewport.Width, Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        RETURN_EFAIL;

    /* Target_LightDepth */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_LightDepth"), SHADOW_CASCADE_SIZE, SHADOW_CASCADE_SIZE, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f), 4)))
        RETURN_EFAIL;

    /* Target_Result */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Result"), Viewport.Width, Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        RETURN_EFAIL;

    /* Target_HBAO+ */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_HBAO+"),
        Viewport.Width, Viewport.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
        RETURN_EFAIL;

    /* Target_PBR */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_PBR"),
        Viewport.Width, Viewport.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        RETURN_EFAIL;

    /* Target_Emissive */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Emissive"),
        Viewport.Width, Viewport.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        RETURN_EFAIL;

    /* Target_Effect */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Effect"), Viewport.Width, Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        RETURN_EFAIL;

	/* Target_ModelEffect */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_ModelEffect"), Viewport.Width, Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		RETURN_EFAIL;

    /* Target_EffectResult */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_EffectResult"), Viewport.Width, Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        RETURN_EFAIL;

    /* Target_Blur_X */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur_X"), Viewport.Width, Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        RETURN_EFAIL;

    /* Target_Blur_Y */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur_Y"), Viewport.Width, Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        RETURN_EFAIL;

    /* Target_Luminance */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Luminance"), Viewport.Width, Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        RETURN_EFAIL;

    /* Target_Luminance_DownSample */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Luminance_DownSample"), Viewport.Width, Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        RETURN_EFAIL;

    /* Target_Luminance_Blur_X */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Luminance_Blur_X"), Viewport.Width, Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        RETURN_EFAIL;

    /* Target_Luminance_Blur_Y */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Luminance_Blur_Y"), Viewport.Width, Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        RETURN_EFAIL;

    /* Target_Distortion */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Distortion"), Viewport.Width, Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        RETURN_EFAIL; // 추가

    /* Target_RadialBlur */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_RadialBlur"), Viewport.Width, Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
       RETURN_EFAIL; // 추가

	/* Target_SolidEffect */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_SolidEffect"), Viewport.Width, Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		RETURN_EFAIL; // 추가

	/* Target_PostProcessing */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_PostProcessing"), Viewport.Width, Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		RETURN_EFAIL;

    /* Target_CopyForMotionBlur */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_CopyForMotionBlur"), Viewport.Width, Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        RETURN_EFAIL;

	/* Target_CopyForRadialBlur */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_CopyForRadialBlur"), Viewport.Width, Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		RETURN_EFAIL; // 추가

	/*Target_AntiAliasing*/
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_AntiAliasing"),
		(_uint)Viewport.Width * 2.f, (_uint)Viewport.Height * 2.f, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 128.f / 255.f, 1.f, 1.f))))
		RETURN_EFAIL;

    /* Target_Volumetric */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Volumetric"), Viewport.Width, Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        RETURN_EFAIL;

    /*Target_ShaderTag*/
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_ShaderTag"), Viewport.Width, Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        RETURN_EFAIL;

    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
        RETURN_EFAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
        RETURN_EFAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))))
        RETURN_EFAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_PBR"))))
        RETURN_EFAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Emissive"))))
        RETURN_EFAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_ShaderTag"))))
        RETURN_EFAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_ModelEffect"))))
        RETURN_EFAIL;

    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
        RETURN_EFAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
        RETURN_EFAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Ambient"))))
        RETURN_EFAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Volumetric"))))
        RETURN_EFAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shadow"))))
        RETURN_EFAIL;

    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Shadow"), TEXT("Target_LightDepth"))))
        RETURN_EFAIL;

    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Result"), TEXT("Target_Result"))))
        RETURN_EFAIL;

    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_PostProcessing"), TEXT("Target_PostProcessing"))))
        RETURN_EFAIL;

    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_PostProcessing"), TEXT("Target_CopyForMotionBlur"))))
        RETURN_EFAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_PostProcessing"), TEXT("Target_CopyForRadialBlur"))))
		RETURN_EFAIL; // 추가

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_MotionBlur"), TEXT("Target_PostProcessing"))))
		RETURN_EFAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_RadialBlur"), TEXT("Target_RadialBlur"))))
		RETURN_EFAIL; // 추가

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_AntiAliasing"), TEXT("Target_AntiAliasing"))))
		RETURN_EFAIL;

#pragma region MRT_Bloom
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Effect"), TEXT("Target_Effect"))))
        RETURN_EFAIL;

    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Effect"), TEXT("Target_Distortion"))))
        RETURN_EFAIL; // 추가

    //if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Effect"), TEXT("Target_RadialBlur"))))
    //   RETURN_EFAIL; // 추가

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Effect"), TEXT("Target_SolidEffect"))))
		RETURN_EFAIL; // 추가

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Blur_X"), TEXT("Target_Blur_X"))))
		RETURN_EFAIL;

    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Blur_Y"), TEXT("Target_Blur_Y"))))
        RETURN_EFAIL;

    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Luminance"), TEXT("Target_Luminance"))))
        RETURN_EFAIL;

    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Luminance_DownSample"), TEXT("Target_Luminance_DownSample"))))
        RETURN_EFAIL;

    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Luminance_Blur_X"), TEXT("Target_Luminance_Blur_X"))))
        RETURN_EFAIL;

    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Luminance_Blur_Y"), TEXT("Target_Luminance_Blur_Y"))))
        RETURN_EFAIL;

    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_EffectResult"), TEXT("Target_EffectResult"))))
        RETURN_EFAIL;
#pragma endregion


    m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
    if (nullptr == m_pVIBuffer)
        RETURN_EFAIL;

    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("Shader_Deferred.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
    if (nullptr == m_pShader)
        RETURN_EFAIL;

    m_pBlurShader = CShader::Create(m_pDevice, m_pContext, TEXT("Shader_Blur.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
    if (nullptr == m_pBlurShader)
        RETURN_EFAIL;

    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(Viewport.Width, Viewport.Height, 1.f));
    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(Viewport.Width, Viewport.Height, 0.f, 1.f));

#ifdef _DEBUG
    //Diffuse,Normal,Shade 디버깅용 렌더타겟 출력 준비 (+각 출력용 렌더타겟 월드행렬 세팅)
    if (FAILED(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Diffuse"), 100.f, 100.f, 200.f, 200.f)))
        RETURN_EFAIL;
    if (FAILED(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Normal"), 100.f, 300.f, 200.f, 200.f)))
        RETURN_EFAIL;
    if (FAILED(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Depth"), 100.f, 500.f, 200.f, 200.f)))
        RETURN_EFAIL;
    if (FAILED(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Shade"), 300.f, 100.f, 200.f, 200.f)))
        RETURN_EFAIL;
    if (FAILED(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Specular"), 300.f, 300.f, 200.f, 200.f)))
        RETURN_EFAIL;

    if (FAILED(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_LightDepth"), 1130.f, 150.f, 300.f, 300.f)))
        RETURN_EFAIL;

    if (FAILED(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_ShaderTag"), 300.f, 500.f, 200.f, 200.f)))
        RETURN_EFAIL;

    if (FAILED(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_PBR"), 500.f, 500.f, 200.f, 200.f)))
        RETURN_EFAIL;

    if (FAILED(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Emissive"), 700.f, 500.f, 200.f, 200.f)))
        RETURN_EFAIL;
    if (FAILED(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Blur_Y"), 900.f, 500.f, 100.f, 100.f)))
        RETURN_EFAIL;

    if (FAILED(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Luminance_Blur_Y"), 1000.f, 500.f, 100.f, 100.f)))
        RETURN_EFAIL;

	//if (FAILED(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Distortion"), 1100.f, 500.f, 100.f, 100.f)))
	//	RETURN_EFAIL; // 추가
	//if (FAILED(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_RadialBlur"), 1100.f, 500.f, 100.f, 100.f)))
	//	RETURN_EFAIL; // 추가
	if (FAILED(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_SolidEffect"), 1100.f, 500.f, 100.f, 100.f)))
		RETURN_EFAIL; // 추가
#endif

    /* LightDepth용 DepthStencil 생성*/

    if (nullptr == m_pDevice)
        RETURN_EFAIL;

    ID3D11Texture2D* pDepthStencilTexture = nullptr;

    D3D11_TEXTURE2D_DESC   TextureDesc;
    ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

    TextureDesc.Width = g_iSizeX;
    TextureDesc.Height = g_iSizeY;
    TextureDesc.MipLevels = 1;
    TextureDesc.ArraySize = 1;
    TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

    TextureDesc.SampleDesc.Quality = 0;
    TextureDesc.SampleDesc.Count = 1;

    TextureDesc.Usage = D3D11_USAGE_DEFAULT;
    TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    /*| D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE*/;
    TextureDesc.CPUAccessFlags = 0;
    TextureDesc.MiscFlags = 0;

    if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr,
        &pDepthStencilTexture)))
        RETURN_EFAIL;

    /* RenderTarget */
    /* ShaderResource */
    /* DepthStencil */

    if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr,
        m_pLightDepthDSV.GetAddressOf())))
        RETURN_EFAIL;

    Safe_Release(pDepthStencilTexture);

    /*Cascade용 DSV,SRV 생성*/
    _uint Width = SHADOW_CASCADE_SIZE;
    _uint Height = SHADOW_CASCADE_SIZE;
    _uint ArrayCount = CASCADE_COUNT;
    D3D11_TEXTURE2D_DESC texDesc = { Width, Height, 1, ArrayCount, DXGI_FORMAT_R32_TYPELESS, 1, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE, 0, 0 };
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = { DXGI_FORMAT_D32_FLOAT, D3D11_DSV_DIMENSION_TEXTURE2DARRAY, 0 };
    dsvDesc.Texture2DArray.FirstArraySlice = 0;
    dsvDesc.Texture2DArray.ArraySize = ArrayCount;
    dsvDesc.Texture2DArray.MipSlice = 0;
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = { DXGI_FORMAT_R32_FLOAT, D3D11_SRV_DIMENSION_TEXTURE2DARRAY, 0, 0 };
    srvDesc.Texture2DArray.FirstArraySlice = 0;
    srvDesc.Texture2DArray.ArraySize = ArrayCount;
    srvDesc.Texture2DArray.MipLevels = 1;
    srvDesc.Texture2DArray.MostDetailedMip = 0;

    ID3D11Texture2D* pTex = nullptr;

    if (FAILED(m_pDevice->CreateTexture2D(&texDesc, NULL, &pTex)))
        RETURN_EFAIL;
    if (FAILED(m_pDevice->CreateDepthStencilView(pTex, &dsvDesc, m_pShadowMapDSV.GetAddressOf())))
        RETURN_EFAIL;
    if (FAILED(m_pDevice->CreateShaderResourceView(pTex, &srvDesc, m_pShadowMapSRV.GetAddressOf())))
        RETURN_EFAIL;

    Safe_Release(pTex);

    /*텍스처 없을시 대체용 Default SRV*/

    ID3D11Texture2D* pDefaultTextureZERO = nullptr;
    ID3D11Texture2D* pDefaultTextureONE = nullptr;
    ID3D11Texture2D* pDefaultTextureNORMAL = nullptr;

    TextureDesc.Width = 1.f;
    TextureDesc.Height = 1.f;
    TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    TextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr,
        &pDefaultTextureZERO)))
        RETURN_EFAIL;
    if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr,
        &pDefaultTextureONE)))
        RETURN_EFAIL;
    if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr,
        &pDefaultTextureNORMAL)))
        RETURN_EFAIL;

    /*Black*/
    _ubyte clearColorZERO[4] = { 0, 0 , 0, 0 }; // RGBA values

    m_pContext->UpdateSubresource(pDefaultTextureZERO, 0, NULL, clearColorZERO, TextureDesc.Width * sizeof(clearColorZERO), 0);

    if (FAILED(m_pDevice->CreateShaderResourceView(pDefaultTextureZERO, nullptr,
        &m_pDefaultSRV_ZERO)))
        RETURN_EFAIL;

    /*White*/
    _ubyte clearColorONE[4] = { 255, 255, 255, 255 }; // RGBA values

    m_pContext->UpdateSubresource(pDefaultTextureONE, 0, NULL, clearColorONE, TextureDesc.Width * sizeof(clearColorONE), 0);

    if (FAILED(m_pDevice->CreateShaderResourceView(pDefaultTextureONE, nullptr,
        &m_pDefaultSRV_ONE)))
        RETURN_EFAIL;

    /*Blue*/
    _ubyte clearColorNORMAL[4] = { 0,0,255,255 };// RGBA values

    m_pContext->UpdateSubresource(pDefaultTextureNORMAL, 0, NULL, clearColorNORMAL, TextureDesc.Width * sizeof(clearColorNORMAL), 0);
    if (FAILED(m_pDevice->CreateShaderResourceView(pDefaultTextureNORMAL, nullptr,
        &m_pDefaultSRV_NORMAL)))
        RETURN_EFAIL;

    Safe_Release(pDefaultTextureZERO);
    Safe_Release(pDefaultTextureONE);
    Safe_Release(pDefaultTextureNORMAL);

    /*For PBR Material Component*/
    //if (FAILED(m_pGameInstance->Include_All_Textures(TEXT("PBR/"), true)))
    //   RETURN_EFAIL;

    //m_pMaterialCom_BRDF = CMaterialComponent::Create(m_pDevice, m_pContext);
    //m_pMaterialCom_PreFiltered = CMaterialComponent::Create(m_pDevice, m_pContext);
    //m_pMaterialCom_Irradiance = CMaterialComponent::Create(m_pDevice, m_pContext);
    //
    //m_pMaterialCom_BRDF->Ready_CustomSingleMaterial(1, L"BRDF", 1);
    //m_pMaterialCom_PreFiltered->Ready_CustomSingleMaterial(1, L"IrradianceMap", 1);
    //m_pMaterialCom_Irradiance->Ready_CustomSingleMaterial(1, L"PreFilter", 1);

    /*ViewPort Desc 설정 (기본/LightDepth용)*/

    ZeroMemory(&m_ViewPortDesc[0], sizeof(D3D11_VIEWPORT));
    m_ViewPortDesc[0].TopLeftX = 0;
    m_ViewPortDesc[0].TopLeftY = 0;
    m_ViewPortDesc[0].Width = 1280;
    m_ViewPortDesc[0].Height = 720;
    m_ViewPortDesc[0].MinDepth = 0.f;
    m_ViewPortDesc[0].MaxDepth = 1.f;

    //Anim Dynamic Shadow용
    ZeroMemory(&m_ViewPortDesc[1], sizeof(D3D11_VIEWPORT));
    m_ViewPortDesc[1].TopLeftX = 0;
    m_ViewPortDesc[1].TopLeftY = 0;
    m_ViewPortDesc[1].Width = SHADOW_CASCADE_SIZE;
    m_ViewPortDesc[1].Height = SHADOW_CASCADE_SIZE;
    m_ViewPortDesc[1].MinDepth = 0.f;
    m_ViewPortDesc[1].MaxDepth = 1.f;

    //NonAnim Static Shadow용
    ZeroMemory(&m_ViewPortDesc[2], sizeof(D3D11_VIEWPORT));
    m_ViewPortDesc[2].TopLeftX = 0;
    m_ViewPortDesc[2].TopLeftY = 0;
    m_ViewPortDesc[2].Width = g_iSizeX;
    m_ViewPortDesc[2].Height = g_iSizeY;
    m_ViewPortDesc[2].MinDepth = 0.f;
    m_ViewPortDesc[2].MaxDepth = 1.f;


    Initialize_HBAO();

    return S_OK;
}

HRESULT CRenderer::Initialize_HBAO()
{
    mAOParameters = {};

    mAOParameters.Radius = 2.f;
    mAOParameters.Bias = 0.2f;
    mAOParameters.PowerExponent = 2.f;
    mAOParameters.Blur.Enable = true;
    mAOParameters.Blur.Sharpness = 16.f;
    mAOParameters.Blur.Radius = GFSDK_SSAO_BLUR_RADIUS_4;
    mAOParameters.DepthStorage = GFSDK_SSAO_FP32_VIEW_DEPTHS;
    mAOParameters.EnableDualLayerAO = false;

    const UINT NodeMask = 1;

#ifdef _DEBUG
#ifdef new
#undef new
#endif
    GFSDK_SSAO_CustomHeap CustomHeap;
    CustomHeap.new_ = ::operator new;
    CustomHeap.delete_ = ::operator delete;
#ifndef new
#define new DBG_NEW
#endif
#else
    GFSDK_SSAO_CustomHeap CustomHeap;
    CustomHeap.new_ = ::operator new;
    CustomHeap.delete_ = ::operator delete;
#endif

    GFSDK_SSAO_Status status = GFSDK_SSAO_CreateContext_D3D11(m_pDevice.Get(), &mSSAOContext, &CustomHeap);
#ifdef _DEBUG
    assert(status == GFSDK_SSAO_OK);
#endif // DEBUG

    return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eGroupID, shared_ptr<CGameObject> pGameObject)
{
    if (nullptr == pGameObject ||
        eGroupID >= RENDER_END)
        RETURN_EFAIL;

    /* 원래 객체들은 오브젝트 매니져에 담겨있다. */
    /* 렌더러에 객체를 공유했다. */
    m_RenderObjects[eGroupID].push_back(pGameObject);

    return S_OK;
}

HRESULT CRenderer::Add_DebugRender(shared_ptr<CComponent> pDebugCom)
{
    m_DebugComponent.push_back(pDebugCom);

    return S_OK;
}

HRESULT CRenderer::Draw_RenderGroup()
{
    if (m_pGameInstance->Key_PressingEx(DIK_LEFT, DIK_MD_LCTRL))
        if (m_fShaderHalf_X > 0.f)
            m_fShaderHalf_X -= 0.02f;
    if (m_pGameInstance->Key_PressingEx(DIK_RIGHT, DIK_MD_LCTRL))
        if (m_fShaderHalf_X < 1.f)
            m_fShaderHalf_X += 0.02f;
    if (m_pGameInstance->Key_DownEx(DIK_DOWN, DIK_MD_LCTRL))
            m_bShaderHalf = !m_bShaderHalf;
    if (m_pGameInstance->Key_DownEx(DIK_UP, DIK_MD_LCTRL))
        m_fShaderHalf_X = 0.5f;

    if (FAILED(Render_Priority()))
        RETURN_EFAIL;
    if (FAILED(Render_Shadow()))
        RETURN_EFAIL;
    if (FAILED(Render_NonLight()))
        RETURN_EFAIL;
    if (FAILED(Render_NonBlend()))
        RETURN_EFAIL;
    if (FAILED(Render_HBAO()))
        RETURN_EFAIL;
    if (FAILED(Render_LightAcc()))
        RETURN_EFAIL;
    if (FAILED(Render_Blend_Particle()))
        RETURN_EFAIL;
    if (FAILED(Render_Blend_Effect()))
        RETURN_EFAIL;
    if (FAILED(Render_Extract_Luminance()))
        RETURN_EFAIL;
    if (FAILED(Render_Bloom()))
        RETURN_EFAIL;
    if (FAILED(Render_BlendBloom()))
        RETURN_EFAIL;

	if (FAILED(Render_Deferred()))
		RETURN_EFAIL;
	if (FAILED(Render_Result()))
		RETURN_EFAIL;
	if (FAILED(Render_MotionBlur()))
		RETURN_EFAIL;
	//if (FAILED(Render_RadialBlur()))
	//	RETURN_EFAIL; // 추가
	//if (FAILED(Render_SSR()))
		//RETURN_EFAIL;
	//if (FAILED(Render_Blend()))
		//RETURN_EFAIL;
	if (FAILED(Render_AntiAliasing()))
		RETURN_EFAIL;
	if (FAILED(Render_UI()))
		RETURN_EFAIL;
#ifdef _DEBUG
    if (FAILED(Render_PhysX_Debug()))
        RETURN_EFAIL;

	// DirectX 버전의 충돌체 디버깅을 위한 함수입니다.
	//if (FAILED(Render_Debug()))
		//RETURN_EFAIL;
	if (FAILED(Render_Debug()))
		RETURN_EFAIL;

    if (m_pGameInstance->Key_Down(DIK_F1))
    {
        if (mAOParameters.Radius == 2.0f)
            mAOParameters.Radius = 0.f;
        else
            mAOParameters.Radius = 2.0f;
    }

#endif // DEBUG


    return S_OK;
}

_bool CRenderer::Update_ShadowInfo()
{
    weak_ptr<CLight> pLight = m_pGameInstance->Get_DirectionalLight();
    weak_ptr<CCamera> pCamera = m_pGameInstance->Get_CurCamera();

    if (pLight.lock() && pCamera.lock())
    {
        std::array<float, CASCADE_COUNT> split_distances;
        std::array<_matrix, CASCADE_COUNT> proj_matrices = RecalculateProjectionMatrices(pCamera, 0.25f /*split_lambda*/, split_distances);

        BoundingBox light_bounding_box;
        _float4x4 ProjInvMatrix = m_pGameInstance->Get_TransformFloat4x4Inverse(CPipeLine::TRANSFORMSTATE::TS_VIEW);
        for (_uint_32 i = 0; i < CASCADE_COUNT; ++i)
        {
           auto const& [V, P] = LightViewProjection_Cascades(pLight, pCamera, proj_matrices[i], light_bounding_box);
           m_Shadow_LightViewProjMatrices[i] = V * P;

           //shadow_cbuf_data.lightview = V;
        }
        //LightViewProjection_Cascades2(ProjInvMatrix, pCamera, pLight, m_Shadow_LightViewProjMatrices);

        m_fShadow_MapSize = SHADOW_CASCADE_SIZE;
        m_Shadow_Matrices[0] =/*ProjInvMatrix */ m_Shadow_LightViewProjMatrices[0];
        m_Shadow_Matrices[1] =/*ProjInvMatrix */ m_Shadow_LightViewProjMatrices[1];
        m_Shadow_Matrices[2] =/*ProjInvMatrix */ m_Shadow_LightViewProjMatrices[2];
        m_Shadow_Matrices[3] =/*ProjInvMatrix */ m_Shadow_LightViewProjMatrices[3];
        m_fShadow_SplitDistances[0] = split_distances[0];
        m_fShadow_SplitDistances[1] = split_distances[1];
        m_fShadow_SplitDistances[2] = split_distances[2];
        m_fShadow_SplitDistances[3] = split_distances[3];

        _float4 Test = XMVector3TransformCoord(_float4(0.f, 0.f, 5.f, 1.f), m_Shadow_LightViewProjMatrices[0]);

        return m_bCascadeShadow = true;
    }
    return m_bCascadeShadow = false;
}

HRESULT CRenderer::Render_Priority()
{
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObjects"))))
        RETURN_EFAIL;

    for (auto& pGameObject : m_RenderObjects[RENDER_PRIORITY])
    {
        if (nullptr != pGameObject)
            pGameObject->Render();
    }

    m_RenderObjects[RENDER_PRIORITY].clear();

    /* 백버퍼를 원래 위치로 다시 장치에 바인딩한다. */
    if (FAILED(m_pGameInstance->End_MRT()))
        RETURN_EFAIL;

    return S_OK;
}

HRESULT CRenderer::Render_Shadow()
{
    //if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Shadow"), true, m_pLightDepthDSV)))
       //RETURN_EFAIL;
    if (Update_ShadowInfo())
    {
        if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Shadow"), true, m_pShadowMapDSV)))
            RETURN_EFAIL;

        m_pContext->RSSetViewports(1, &m_ViewPortDesc[1]);

        for (auto& pGameObject : m_RenderObjects[RENDER_SHADOW])
        {
            if (nullptr != pGameObject)
                pGameObject->Render_Shadow();
        }

        m_RenderObjects[RENDER_SHADOW].clear();

        if (FAILED(m_pGameInstance->End_MRT()))
            RETURN_EFAIL;

        m_pContext->RSSetViewports(1, &m_ViewPortDesc[0]);
    }
    return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Result"))))
        RETURN_EFAIL;

    for (auto& pGameObject : m_RenderObjects[RENDER_NONLIGHT])
    {
        if (nullptr != pGameObject)
            pGameObject->Render();
    }

    m_RenderObjects[RENDER_NONLIGHT].clear();

    if (FAILED(m_pGameInstance->End_MRT()))
        RETURN_EFAIL;

    return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
    /* Diffuse + Normal */
    /* 기존에 셋팅되어있던 백버퍼를 빼내고 Diffuse와 Normal을 장치에 바인딩한다. */

    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObjects"), false)))
        RETURN_EFAIL;

    m_pContext->GSSetShader(nullptr, nullptr, 0);

    for (auto& pGameObject : m_RenderObjects[RENDER_NONBLEND_CHARACTER])
    {
        if (nullptr != pGameObject)
            pGameObject->Render();
    }

    m_RenderObjects[RENDER_NONBLEND_CHARACTER].clear();


    for (auto& pGameObject : m_RenderObjects[RENDER_NONBLEND])
    {
        if (nullptr != pGameObject)
            pGameObject->Render();
    }

    m_RenderObjects[RENDER_NONBLEND].clear();

    /* 백버퍼를 원래 위치로 다시 장치에 바인딩한다. */
    if (FAILED(m_pGameInstance->End_MRT()))
        RETURN_EFAIL;

    return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
    m_RenderObjects[RENDER_BLEND].sort([](shared_ptr<CGameObject> pSour, shared_ptr<CGameObject> pDest)->_bool
        {
            return ((CBlendObject*)pSour.get())->Get_CamDistance() > ((CBlendObject*)pDest.get())->Get_CamDistance();
        });

    for (auto& pGameObject : m_RenderObjects[RENDER_BLEND])
    {
        if (nullptr != pGameObject)
            pGameObject->Render();
    }

    m_RenderObjects[RENDER_BLEND].clear();

    return S_OK;
}

HRESULT CRenderer::Render_Blend_Particle()
{
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Effect"))))
        RETURN_EFAIL;

    m_RenderObjects[RENDER_BLEND_PARTICLE].sort([](shared_ptr<CGameObject> pSour, shared_ptr<CGameObject> pDest)->_bool
        {
            return ((CBlendObject*)pSour.get())->Get_CamDistance() > ((CBlendObject*)pDest.get())->Get_CamDistance();
        });

    for (auto& pGameObject : m_RenderObjects[RENDER_BLEND_PARTICLE])
    {
        if (nullptr != pGameObject)
            pGameObject->Render();


    }

    m_RenderObjects[RENDER_BLEND_PARTICLE].clear();

    return S_OK;
}

HRESULT CRenderer::Render_Blend_Effect()
{
    m_RenderObjects[RENDER_BLEND_EFFECT].sort([](shared_ptr<CGameObject> pSour, shared_ptr<CGameObject> pDest)->_bool
        {
            return ((CBlendObject*)pSour.get())->Get_CamDistance() > ((CBlendObject*)pDest.get())->Get_CamDistance();
        });

    for (auto& pGameObject : m_RenderObjects[RENDER_BLEND_EFFECT])
    {
        if (nullptr != pGameObject)
            pGameObject->Render();


    }

    m_RenderObjects[RENDER_BLEND_EFFECT].clear();

    if (FAILED(m_pGameInstance->End_MRT()))
        RETURN_EFAIL;

    return S_OK;
}

HRESULT CRenderer::Render_Extract_Luminance()
{
    if (m_bHDRBloom)
    {
        if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Luminance"))))
            RETURN_EFAIL;

        if (FAILED(m_pBlurShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
            RETURN_EFAIL;
        if (FAILED(m_pBlurShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
            RETURN_EFAIL;
        if (FAILED(m_pBlurShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
            RETURN_EFAIL;
        if (FAILED(m_pBlurShader->Bind_RawValue("g_Brightness", &m_fHDRBloom_Brightness, sizeof(_float))))
            RETURN_EFAIL;

        //맵 말고 이펙트만 휘도 추출하게 임시 수정
     /*   if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Result"), m_pBlurShader.get(), "g_OriginalRenderTexture")))
           RETURN_EFAIL;*/

           //이펙트 추출
        if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Effect"), m_pBlurShader.get(), "g_OriginalRenderTexture")))
            RETURN_EFAIL;
        if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_ModelEffect"), m_pBlurShader.get(), "g_ModelEffectTexture")))
            RETURN_EFAIL;
        if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_ShaderTag"), m_pBlurShader.get(), "g_ShaderTagTexture")))
            RETURN_EFAIL;

        m_pBlurShader->Begin(2);
        m_pVIBuffer->Bind_VIBuffers();
        m_pVIBuffer->Render();

        //Emissive 추출
        if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Emissive"), m_pBlurShader.get(), "g_OriginalRenderTexture")))
            RETURN_EFAIL;
        m_pVIBuffer->Render();

        //ModelEffect 추출
        if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_ModelEffect"), m_pBlurShader.get(), "g_OriginalRenderTexture")))
            RETURN_EFAIL;
        m_pVIBuffer->Render();

        if (FAILED(m_pGameInstance->End_MRT()))
            RETURN_EFAIL;

        if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Luminance_DownSample"))))
            RETURN_EFAIL;

        if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Luminance"), m_pBlurShader.get(), "g_OriginalRenderTexture")))
            RETURN_EFAIL;
        _float fPixelWidth = 1 / m_ViewPortDesc[0].Width;
        _float fPixelHeight = 1 / m_ViewPortDesc[0].Height;
        if (FAILED(m_pBlurShader->Bind_RawValue("g_PixelWidth", &fPixelWidth, sizeof(_float))))
            RETURN_EFAIL;
        if (FAILED(m_pBlurShader->Bind_RawValue("g_PixelHeight", &fPixelHeight, sizeof(_float))))
            RETURN_EFAIL;
        m_pBlurShader->Begin(7);
        m_pVIBuffer->Bind_VIBuffers();
        m_pVIBuffer->Render();

        if (FAILED(m_pGameInstance->End_MRT()))
            RETURN_EFAIL;
    }

    return S_OK;
}

HRESULT CRenderer::Render_Bloom()
{
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur_X"))))
        RETURN_EFAIL;

    //Effect블러
    if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Effect"), m_pBlurShader.get(), "g_ExtractBloomTexture")))
        RETURN_EFAIL;

    if (FAILED(m_pBlurShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        RETURN_EFAIL;
    if (FAILED(m_pBlurShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        RETURN_EFAIL;
    if (FAILED(m_pBlurShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        RETURN_EFAIL;

    _float fPixelWidth = 1 / m_ViewPortDesc[0].Width * m_fBlur_Range;
    _float fPixelHeight = 1 / m_ViewPortDesc[0].Height * m_fBlur_Range;
    if (FAILED(m_pBlurShader->Bind_RawValue("g_PixelWidth", &fPixelWidth, sizeof(_float))))
        RETURN_EFAIL;
    if (FAILED(m_pBlurShader->Bind_RawValue("g_PixelHeight", &fPixelHeight, sizeof(_float))))
        RETURN_EFAIL;
    if (FAILED(m_pBlurShader->Bind_RawValue("g_BlurStrength", &m_fBlur_Stregth, sizeof(_float))))
        RETURN_EFAIL;

    m_pVIBuffer->Bind_VIBuffers();
    m_pBlurShader->Begin(5);
    m_pVIBuffer->Render();

	//Emissive 블러
	if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Emissive"), m_pBlurShader.get(), "g_ExtractBloomTexture")))
		RETURN_EFAIL;
	fPixelWidth = 1 / m_ViewPortDesc[0].Width * 4.f;
	fPixelHeight = 1 / m_ViewPortDesc[0].Height * 4.f;
	_float fEmmisive_Blur_Strength = 1.f;
	if (FAILED(m_pBlurShader->Bind_RawValue("g_PixelWidth", &fPixelWidth, sizeof(_float))))
		RETURN_EFAIL;
	if (FAILED(m_pBlurShader->Bind_RawValue("g_PixelHeight", &fPixelHeight, sizeof(_float))))
		RETURN_EFAIL;
	if (FAILED(m_pBlurShader->Bind_RawValue("g_BlurStrength", &fEmmisive_Blur_Strength, sizeof(_float))))
		RETURN_EFAIL;
	m_pVIBuffer->Bind_VIBuffers();
	m_pBlurShader->Begin(5);
	m_pVIBuffer->Render();

    //ModelEffect 블러
    if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_ModelEffect"), m_pBlurShader.get(), "g_ExtractBloomTexture")))
        RETURN_EFAIL;
    m_pVIBuffer->Bind_VIBuffers();
    m_pBlurShader->Begin(5);
    m_pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        RETURN_EFAIL;

    /************************************************************************/
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur_Y"))))
        RETURN_EFAIL;

    if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Blur_X"), m_pBlurShader.get(), "g_ExtractBloomTexture")))
        RETURN_EFAIL;

    m_pVIBuffer->Bind_VIBuffers();

    m_pBlurShader->Begin(6);

    m_pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        RETURN_EFAIL;

    /************************************************************************/

    if (m_bHDRBloom)
    {
        if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Luminance_Blur_X"))))
            RETURN_EFAIL;

        if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Luminance_DownSample"), m_pBlurShader.get(), "g_ExtractBloomTexture")))
            RETURN_EFAIL;

        fPixelWidth = 1 / m_ViewPortDesc[0].Width * m_fHDRBloom_Range;
        fPixelHeight = 1 / m_ViewPortDesc[0].Height * m_fHDRBloom_Range;

        if (FAILED(m_pBlurShader->Bind_RawValue("g_PixelWidth", &fPixelWidth, sizeof(_float))))
            RETURN_EFAIL;
        if (FAILED(m_pBlurShader->Bind_RawValue("g_PixelHeight", &fPixelHeight, sizeof(_float))))
            RETURN_EFAIL;
        if (FAILED(m_pBlurShader->Bind_RawValue("g_BlurStrength", &m_fHDRBloom_Stregth, sizeof(_float))))
            RETURN_EFAIL;

        m_pVIBuffer->Bind_VIBuffers();

        m_pBlurShader->Begin(5);

        m_pVIBuffer->Render();

        if (FAILED(m_pGameInstance->End_MRT()))
            RETURN_EFAIL;

        /************************************************************************/
        if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Luminance_Blur_Y"))))
            RETURN_EFAIL;

        if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Luminance_Blur_X"), m_pBlurShader.get(), "g_ExtractBloomTexture")))
            RETURN_EFAIL;

        m_pVIBuffer->Bind_VIBuffers();

        m_pBlurShader->Begin(6);

        m_pVIBuffer->Render();

        if (FAILED(m_pGameInstance->End_MRT()))
            RETURN_EFAIL;
    }


    return S_OK;
}

HRESULT CRenderer::Render_BlendBloom()
{
    //if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_PostProcessing"), false)))
    //   RETURN_EFAIL;

    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_EffectResult"), true)))
        RETURN_EFAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        RETURN_EFAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        RETURN_EFAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        RETURN_EFAIL;
    if (FAILED(m_pShader->Bind_RawValue("g_bHDRBloom", &m_bHDRBloom, sizeof(_bool))))
        RETURN_EFAIL;

    if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Blur_Y"), m_pShader.get(), "g_BloomTexture")))
        RETURN_EFAIL;

    if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Luminance_Blur_Y"), m_pShader.get(), "g_LuminanceBloomTexture")))
        RETURN_EFAIL;

    if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Effect"), m_pShader.get(), "g_OriginalTexture")))
        RETURN_EFAIL;

	if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_SolidEffect"), m_pShader.get(), "g_SolidEffectTexture")))
		RETURN_EFAIL; // 추가
	
	if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_ModelEffect"), m_pShader.get(), "g_ModelEffectTexture")))
		RETURN_EFAIL;

    m_pShader->Begin(10);
    m_pVIBuffer->Bind_VIBuffers();
    m_pVIBuffer->Render();

    /*if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Emissive"), m_pShader.get(), "g_OriginalTexture")))
       RETURN_EFAIL;

    m_pShader->Begin(10);
    m_pVIBuffer->Bind_VIBuffers();
    m_pVIBuffer->Render();*/

    if (FAILED(m_pGameInstance->End_MRT()))
        RETURN_EFAIL;

    return S_OK;
}

HRESULT CRenderer::Render_Result()
{
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_PostProcessing"))))
        RETURN_EFAIL;

    if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Result"), m_pShader.get(), "g_DiffuseTexture")))
        RETURN_EFAIL;

    if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_EffectResult"), m_pShader.get(), "g_EffectTexture")))
        RETURN_EFAIL;

    if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Emissive"), m_pShader.get(), "g_EmissiveTexture")))
        RETURN_EFAIL;

    if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_ShaderTag"), m_pShader.get(), "g_ShaderTagTexture")))
        RETURN_EFAIL;

    if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Distortion"), m_pShader.get(), "g_DistortionTexture")))
        RETURN_EFAIL; // 디스토션 추가

	if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_RadialBlur"), m_pShader.get(), "g_RadialBlurTexture")))
		RETURN_EFAIL; // 레디얼 블러 추가

	//if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_SolidEffect"), m_pShader.get(), "g_SolidEffectTexture")))
	//	RETURN_EFAIL; // 추가
	
	if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Volumetric"), m_pShader.get(), "g_VolumetricTexture")))
		RETURN_EFAIL;


    if (FAILED(m_pShader->Bind_RawValue("g_vHSV", &m_vHSV, sizeof(_float3))))
        RETURN_EFAIL;

    if (FAILED(m_pShader->Bind_RawValue("g_bToneMapping", &m_bToneMapping, sizeof(_bool))))
        RETURN_EFAIL;

    if (FAILED(m_pShader->Bind_RawValue("g_bShaderHalf", &m_bShaderHalf, sizeof(_bool))))
        RETURN_EFAIL;
    if (FAILED(m_pShader->Bind_RawValue("g_fShaderHalf_X", &m_fShaderHalf_X, sizeof(_float))))
        RETURN_EFAIL;


    m_pShader->Begin(6);

    m_pVIBuffer->Bind_VIBuffers();

    m_pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        RETURN_EFAIL;

    return S_OK;
}

HRESULT CRenderer::Render_MotionBlur()
{
    if (m_bMotionBlur)
    {
        if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_MotionBlur"))))
            RETURN_EFAIL;

        if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_CopyForMotionBlur"), m_pShader.get(), "g_DiffuseTexture")))
            RETURN_EFAIL;

        if (FAILED(m_pShader->Bind_RawValue("g_fMotionBlurAmount", &m_fMotionBlurAmount, sizeof(_float))))
            RETURN_EFAIL;

        if (FAILED(m_pShader->Bind_RawValue("g_fMotionBlurWidth", &m_fMotionBlurScale, sizeof(_float))))
            RETURN_EFAIL;

        if (FAILED(m_pShader->Bind_RawValue("g_vMotionBlurCenter", &m_vMotionBlurCenter, sizeof(_float2))))
            RETURN_EFAIL;

        m_pShader->Begin(7);

        m_pVIBuffer->Bind_VIBuffers();

        m_pVIBuffer->Render();

        if (FAILED(m_pGameInstance->End_MRT()))
            RETURN_EFAIL;
    }



    return S_OK;
}

HRESULT CRenderer::Render_RadialBlur()
{
	if (m_bRadialBlur)
	{
		if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_RadialBlur"))))
			RETURN_EFAIL;

		/*if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_CopyForRadialBlur"), m_pShader.get(), "g_DiffuseTexture")))
			RETURN_EFAIL;*/

		if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
			RETURN_EFAIL;
		if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
			RETURN_EFAIL;

		if (FAILED(m_pShader->Bind_RawValue("g_vRadialBlurWorldPosition", &m_vRadialBlurWorldPos, sizeof(_float3))))
			RETURN_EFAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_fRadialBlurIntensity", &m_fRadialBlurIntensity, sizeof(_float))))
			RETURN_EFAIL;

		m_pShader->Begin(15);

		m_pVIBuffer->Bind_VIBuffers();

		m_pVIBuffer->Render();

		if (FAILED(m_pGameInstance->End_MRT()))
			RETURN_EFAIL;
	}

	return S_OK; // 추가
}

HRESULT CRenderer::Render_AntiAliasing()
{
    /*if (m_bAntiAliasing)
    {
       if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_AntiAliasing"), true, m_pGameInstance->Get_AntiAliasingDepthStencilView())))
          RETURN_EFAIL;

       m_pContext->RSSetViewports(1, &m_pGameInstance->Get_AntiAliasingViewPortDesc());

       if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_PostProcessing"), m_pShader.get(), "g_OriginalRenderTexture")))
          RETURN_EFAIL;

       m_pShader->Bind_RawValue("g_WorldMatrix", &m_AntiAliasingWorldMatrix, sizeof(_float4x4));
       m_pShader->Bind_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
       m_pShader->Bind_RawValue("g_ProjMatrix", &m_AntiAliasingProjMatrixTranspose, sizeof(_float4x4));

       m_pShader->Begin(11);
       m_pVIBuffer->Render();

       if (FAILED(m_pGameInstance->End_MRT()))
          RETURN_EFAIL;

       m_pContext->RSSetViewports(1, &m_ViewPortDesc[0]);
    }
 */


 /*if (m_bAntiAliasing)
 {
    if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_AntiAliasing"), m_pShader.get(), "g_OriginalRenderTexture")))
       RETURN_EFAIL;
 }
 else*/
    {
        if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_PostProcessing"), m_pShader.get(), "g_OriginalRenderTexture")))
            RETURN_EFAIL;
    }

    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        RETURN_EFAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        RETURN_EFAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        RETURN_EFAIL;


    if (m_bAntiAliasing)
    {
        m_pShader->Begin(14);
        m_pVIBuffer->Render();
    }
    else
    {
        m_pShader->Begin(11);
        m_pVIBuffer->Render();
    }


    return S_OK;
}

HRESULT CRenderer::Render_UI()
{
    m_RenderObjects[RENDER_UI].sort([](const shared_ptr<CGameObject>& pSour, const shared_ptr<CGameObject>& pDest)->bool
        {
            // 먼저 ProjType 기준으로 비교
            if (((CUIObject*)pSour.get())->Get_ProjType() != ((CUIObject*)pDest.get())->Get_ProjType())
                return ((CUIObject*)pSour.get())->Get_ProjType() > ((CUIObject*)pDest.get())->Get_ProjType();

            // 다음 RENDERTYPE 기준으로 비교
            if (((CUIObject*)pSour.get())->Get_RenderType() != ((CUIObject*)pDest.get())->Get_RenderType())
                return ((CUIObject*)pSour.get())->Get_RenderType() < ((CUIObject*)pDest.get())->Get_RenderType();

            // 같은 RENDERTYPE일 경우 ZDepth로 비교
            return ((CUIObject*)pSour.get())->Get_ZDepth() > ((CUIObject*)pDest.get())->Get_ZDepth();
        });

    for (auto& pGameObject : m_RenderObjects[RENDER_UI])
    {
        if (nullptr != pGameObject)
            pGameObject->Render();
    }

    m_RenderObjects[RENDER_UI].clear();

    return S_OK;
}

HRESULT CRenderer::Render_LightAcc()
{
    /* Shade */
    /* 여러개 빛의 연산 결과를 저장해 준다. */
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_LightAcc"))))
        RETURN_EFAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        RETURN_EFAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        RETURN_EFAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        RETURN_EFAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", &m_pGameInstance->Get_TransformFloat4x4Inverse(CPipeLine::TRANSFORMSTATE::TS_VIEW))))
        RETURN_EFAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", &m_pGameInstance->Get_TransformFloat4x4Inverse(CPipeLine::TRANSFORMSTATE::TS_PROJ))))
        RETURN_EFAIL;
    if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        RETURN_EFAIL;

    if (FAILED(m_pShader->Bind_RawValue("g_bShaderHalf", &m_bShaderHalf, sizeof(_bool))))
        RETURN_EFAIL;
    if (FAILED(m_pShader->Bind_RawValue("g_fShaderHalf_X", &m_fShaderHalf_X, sizeof(_float))))
        RETURN_EFAIL;


    if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Normal"), m_pShader.get(), "g_NormalTexture")))
        RETURN_EFAIL;

    if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Depth"), m_pShader.get(), "g_DepthTexture")))
        RETURN_EFAIL;

    if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_PBR"), m_pShader.get(), "g_PBRTexture")))
        RETURN_EFAIL;

    if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Diffuse"), m_pShader.get(), "g_DiffuseTexture")))
        RETURN_EFAIL;

    if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_ShaderTag"), m_pBlurShader.get(), "g_ShaderTagTexture")))
        RETURN_EFAIL;

    if (FAILED(m_pShader->Bind_RawValue("g_bPBR", &m_bPBR, sizeof(_bool))))
        RETURN_EFAIL;  
        
    if (FAILED(m_pShader->Bind_RawValue("g_bCascadeShadow", &m_bCascadeShadow, sizeof(_bool))))
        RETURN_EFAIL;

    m_pGameInstance->Render_Lights(m_pShader, m_pVIBuffer); //라이트 여러개 돌면서 m_pVIBuffer에 모두 Shade 기록

    /* 0번째에 백버퍼렌더타겟이 올라갔다. */
    if (FAILED(m_pGameInstance->End_MRT()))
        RETURN_EFAIL;

    return S_OK;
}

/* 디퍼드에 의한 최종적인 장며을 만들어주낟. */
HRESULT CRenderer::Render_Deferred()
{
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Result"), false)))
        RETURN_EFAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        RETURN_EFAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        RETURN_EFAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        RETURN_EFAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_LightViewMatrix", &m_pGameInstance->Get_ShadowLight_ViewMatrix())))
        RETURN_EFAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_LightProjMatrix", &m_pGameInstance->Get_ShadowLight_ProjMatrix())))
        RETURN_EFAIL;

    if (FAILED(m_pShader->Bind_RawValue("g_bCascadeShadow", &m_bCascadeShadow, sizeof(_bool))))
        RETURN_EFAIL;

    if (FAILED(m_pShader->Bind_RawValue("g_bShaderHalf", &m_bShaderHalf, sizeof(_bool))))
        RETURN_EFAIL;
    if (FAILED(m_pShader->Bind_RawValue("g_fShaderHalf_X", &m_fShaderHalf_X, sizeof(_float))))
        RETURN_EFAIL;


    if (m_bCascadeShadow)
    {
        if (FAILED(m_pShader->Bind_Matrices("g_ShadowMatrices", m_Shadow_Matrices, 4)))
            RETURN_EFAIL;
        if (FAILED(m_pShader->Bind_RawValue("g_fSplitDistances", m_fShadow_SplitDistances, sizeof(_float) * 4)))
            RETURN_EFAIL;
        if (FAILED(m_pShader->Bind_SRV("g_CascadeDepthTexture", m_pShadowMapSRV.Get())))
            RETURN_EFAIL;
    }

    if (FAILED(m_pShader->Bind_RawValue("g_fFogStart", &m_fFogStart, sizeof(_float))))
        RETURN_EFAIL;
    if (FAILED(m_pShader->Bind_RawValue("g_fFogDensity", &m_fFogDensity, sizeof(_float))))
        RETURN_EFAIL;
    if (FAILED(m_pShader->Bind_RawValue("g_fFogSkyStrength", &m_fFogSkyStrength, sizeof(_float))))
        RETURN_EFAIL;
    if (FAILED(m_pShader->Bind_RawValue("g_vFogColor", &m_vFogColor, sizeof(_float4))))
        RETURN_EFAIL;
    if (FAILED(m_pShader->Bind_RawValue("g_bFog", &m_bFog, sizeof(_bool))))
        RETURN_EFAIL;
    if (FAILED(m_pShader->Bind_RawValue("g_bFogHeight", &m_bFogHeight, sizeof(_bool))))
        RETURN_EFAIL;
    if (FAILED(m_pShader->Bind_RawValue("g_fFogFalloff", &m_fFogFalloff, sizeof(_float))))
        RETURN_EFAIL;

    if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Depth"), m_pShader.get(), "g_DepthTexture")))
        RETURN_EFAIL;
    if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Diffuse"), m_pShader.get(), "g_DiffuseTexture")))
        RETURN_EFAIL;
    if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Shade"), m_pShader.get(), "g_ShadeTexture")))
        RETURN_EFAIL;
    if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Specular"), m_pShader.get(), "g_SpecularTexture")))
        RETURN_EFAIL;
    if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Ambient"), m_pShader.get(), "g_AmbientTexture")))
        RETURN_EFAIL;

    if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Shadow"), m_pShader.get(), "g_ShadowTexture")))
       RETURN_EFAIL;
 
    if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_ShaderTag"), m_pShader.get(), "g_ShaderTagTexture")))
        RETURN_EFAIL;

    if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_HBAO+"), m_pShader.get(), "g_HBAOTexture")))
        RETURN_EFAIL;


    //if(FAILED(m_pShader->Bind_RawValue("g_bPBR", &m_bPBR, sizeof(_bool))))
    //   RETURN_EFAIL;
    //if (FAILED(m_pShader->Bind_RawValue("g_fExposure", &m_fExposure, sizeof(_float))))
    //   RETURN_EFAIL;

    //if (FAILED(m_pMaterialCom_BRDF->Bind_SRVToShader(m_pShader.get(), "g_BRDFTexture", 1, 0, true)))
    //   RETURN_EFAIL;
    //if (FAILED(m_pMaterialCom_PreFiltered->Bind_SRVToShader(m_pShader.get(), "g_PreFilteredTexture", 1, 0,true)))
    //   RETURN_EFAIL;
    //if (FAILED(m_pMaterialCom_Irradiance->Bind_SRVToShader(m_pShader.get(), "g_IrradianceTexture", 1, 0, true)))
    //   RETURN_EFAIL;

    m_pShader->Begin(3);

    m_pVIBuffer->Bind_VIBuffers();

    m_pVIBuffer->Render();

    /* 0번째에 백버퍼렌더타겟이 올라갔다. */
    if (FAILED(m_pGameInstance->End_MRT()))
        RETURN_EFAIL;

    return S_OK;
}

HRESULT CRenderer::Render_RimLight()
{
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Result"), false)))
        RETURN_EFAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", &m_pGameInstance->Get_TransformFloat4x4Inverse(CPipeLine::TRANSFORMSTATE::TS_VIEW))))
        RETURN_EFAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", &m_pGameInstance->Get_TransformFloat4x4Inverse(CPipeLine::TRANSFORMSTATE::TS_PROJ))))
        RETURN_EFAIL;
    if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        RETURN_EFAIL;
    if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_ShaderTag"), m_pShader.get(), "g_DepthTexture")))
        RETURN_EFAIL;
    if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Normal"), m_pShader.get(), "g_NormalTexture")))
        RETURN_EFAIL;

    m_pShader->Begin(8);

    m_pVIBuffer->Bind_VIBuffers();

    m_pVIBuffer->Render();

    /* 0번째에 백버퍼렌더타겟이 올라갔다. */
    if (FAILED(m_pGameInstance->End_MRT()))
        RETURN_EFAIL;

    return S_OK;
}

HRESULT CRenderer::Render_HBAO()
{
    GFSDK_SSAO_InputData_D3D11 InputData = {};
    InputData.DepthData.DepthTextureType = GFSDK_SSAO_HARDWARE_DEPTHS;

    // FullResDepthTextureSRV
    /*if (mAOParameters.EnableDualLayerAO)
    {
       InputData.DepthData.pFullResDepthTextureSRV = m_pGameInstance->Find_RenderTarget(TEXT("Target_Depth"))->Get_SRV().Get();
       InputData.DepthData.pFullResDepthTexture2ndLayerSRV = mDepthSRV[1].Get();
    }
    else*/
    {
        InputData.DepthData.pFullResDepthTextureSRV = m_pGameInstance->Get_DS_SRV().Get();
        //InputData.DepthData.pFullResDepthTexture2ndLayerSRV = nullptr;
    }

    // DepthData

    InputData.DepthData.ProjectionMatrix.Data = GFSDK_SSAO_Float4x4((const GFSDK_SSAO_FLOAT*)&XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), (_float)1280 / 720, 0.1f, 1000.f));
    InputData.DepthData.ProjectionMatrix.Layout = GFSDK_SSAO_ROW_MAJOR_ORDER;
    InputData.DepthData.MetersToViewSpaceUnits = 1.0f;
    InputData.NormalData.Enable = false;

    GFSDK_SSAO_RenderMask RenderMask = GFSDK_SSAO_RENDER_AO;


    GFSDK_SSAO_Output_D3D11 Output;

    Output.pRenderTargetView = m_pGameInstance->Find_RenderTarget(TEXT("Target_HBAO+"))->Get_RTV().Get();
    Output.Blend.Mode = GFSDK_SSAO_OVERWRITE_RGB;

    {
        GFSDK_SSAO_Status status = mSSAOContext->RenderAO(m_pContext.Get(), InputData, mAOParameters, Output, RenderMask);
        assert(status == GFSDK_SSAO_OK);
    }

    return S_OK;
}

HRESULT CRenderer::Render_SSR()
{
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_PostProcessing"), false)))
        RETURN_EFAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        RETURN_EFAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        RETURN_EFAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", &m_pGameInstance->Get_TransformFloat4x4Inverse(CPipeLine::TRANSFORMSTATE::TS_VIEW))))
        RETURN_EFAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", &m_pGameInstance->Get_TransformFloat4x4Inverse(CPipeLine::TRANSFORMSTATE::TS_PROJ))))
        RETURN_EFAIL;
    if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        RETURN_EFAIL;
    if (FAILED(m_pShader->Bind_RawValue("g_fSSRStep", &m_fSSRStep, sizeof(_float))))
        RETURN_EFAIL;
    if (FAILED(m_pShader->Bind_RawValue("g_iSSRStepDistance", &m_iSSRStepDistance, sizeof(_int))))
        RETURN_EFAIL;


    if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Depth"), m_pShader.get(), "g_DepthTexture")))
        RETURN_EFAIL;
    if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Normal"), m_pShader.get(), "g_NormalTexture")))
        RETURN_EFAIL;
    if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Result"), m_pShader.get(), "g_ResultTexture")))
        RETURN_EFAIL;

    m_pShader->Begin(9);

    m_pVIBuffer->Bind_VIBuffers();

    m_pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        RETURN_EFAIL;

    return S_OK;

}


#ifdef _DEBUG
HRESULT CRenderer::Render_Debug()
{
    //각 렌더타겟 출력용 (월드는 각 렌더타겟이 들고있다.)
    //m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
    //m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);

    /*for (auto& pDebugCom : m_DebugComponent)
    {
        pDebugCom->Render();
    }*/
    //m_DebugComponent.clear();

    m_DebugEvent.Broadcast();
    m_DebugEvent.Clear();

   // m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_GameObjects"), m_pShader.get(), m_pVIBuffer.get()); //Diffuse, Normal
    //m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_LightAcc"), m_pShader.get(), m_pVIBuffer.get()); //Shade
   // m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_Shadow"), m_pShader.get(), m_pVIBuffer.get()); //Shadow
   // m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_Effect"), m_pShader.get(), m_pVIBuffer.get());

  //  m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_Luminance_Blur_Y"), m_pShader.get(), m_pVIBuffer.get());
    //m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_PBR"), m_pShader.get(), m_pVIBuffer.get());
  //  m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_Blur_Y"), m_pShader.get(), m_pVIBuffer.get());
    //m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_Emissive"), m_pShader.get(), m_pVIBuffer.get());
    //m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_Shadow_Static"), m_pShader.get(), m_pVIBuffer.get());
    return S_OK;
}

HRESULT CRenderer::Render_PhysX_Debug()
{
    if (m_pGameInstance->Key_Down(DIK_P))
        m_bPhysXDebug = !m_bPhysXDebug;

    if (m_bPhysXDebug)
        GET_SINGLE(CPhysX_Manager)->Render_PhysXDebugger();

    return S_OK;
}
#endif

CRenderer* CRenderer::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    CRenderer* pInstance = new CRenderer(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CRenderer");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CRenderer::Free()
{
    for (auto& ObjectList : m_RenderObjects)
    {
        ObjectList.clear();
    }


#ifdef _DEBUG

    m_DebugComponent.clear();
#endif
    m_pLightDepthDSV.Reset();

    m_pDefaultSRV_ONE->Release();
    m_pDefaultSRV_ZERO->Release();
    m_pDefaultSRV_NORMAL->Release();

    mSSAOContext->Release();
    Safe_Release(m_pGameInstance);
}

