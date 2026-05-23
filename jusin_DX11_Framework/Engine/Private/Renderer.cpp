#include "Renderer.h"
#include "GameObject.h"
#include "GameInstance.h"

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CRenderer::Initialize()
{
	if(FAILED(Resize()))
		return E_FAIL;

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_Deferred.hlsl"), VTXTEX::Elements, VTXTEX::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	m_pShader_PostProcess = CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_PostProcess.hlsl"), VTXTEX::Elements, VTXTEX::iNumElements);
	if (nullptr == m_pShader_PostProcess)
		return E_FAIL;

	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	return S_OK;
}

void CRenderer::Add_RenderGroup(RENDERID eGroupID, CGameObject* pGameObject)
{
	m_RenderObjects[ETOUI(eGroupID)].push_back(pGameObject);

	Safe_AddRef(pGameObject);
}

HRESULT CRenderer::Draw()
{
	if (FAILED(Render_Priority()))
		return E_FAIL;

	if (FAILED(Render_Shadow()))
		return E_FAIL;

	if (FAILED(Render_NonBlend()))
		return E_FAIL;

	if (FAILED(Render_OutlineMask()))
		return E_FAIL;

	if (FAILED(Render_Lights()))
		return E_FAIL;

	if (FAILED(Render_Combined(m_bUseShadow)))
		return E_FAIL;

	if (FAILED(Render_PostProcess()))
		return E_FAIL;

	if (FAILED(Render_NonLight()))
		return E_FAIL;

	if (FAILED(Render_Blend()))
		return E_FAIL;

	if (FAILED(Render_UI()))
		return E_FAIL;

#ifdef _DEBUG
	if (FAILED(Render_Debug()))
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CRenderer::Resize()
{
	_float2 vViewportDesc = m_pGameInstance->Get_ViewportSize();
	_uint iNewWidth = static_cast<_uint>(vViewportDesc.x);
	_uint iNewHeight = static_cast<_uint>(vViewportDesc.y);

	// RT »ý¼º
	if (FAILED(m_pGameInstance->Add_RenderTarget(TARGET_DIFFUSE, iNewWidth, iNewHeight,
		DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TARGET_NORMAL, iNewWidth, iNewHeight,
		DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TARGET_SHADE, iNewWidth, iNewHeight,
		DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TARGET_DEPTH, iNewWidth, iNewHeight,
		DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TARGET_AMBIENT, iNewWidth, iNewHeight,
		DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.505f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TARGET_SPECULAR, iNewWidth, iNewHeight,
		DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TARGET_PICKPOS, iNewWidth, iNewHeight,
		DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TARGET_LIGHTDEPTH, g_iMaxWidth, g_iMaxHeight,
		DXGI_FORMAT_R32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TARGET_COMBINED, iNewWidth, iNewHeight,
		DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TARGET_OUTLINEMASK, iNewWidth, iNewHeight,
		DXGI_FORMAT_R8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(Ready_DepthStencil_Buffer()))
		return E_FAIL;
	// MRT·Î ¹­±â
	if (FAILED(m_pGameInstance->Add_MRT(MRT_GAMEOBJECTS, TARGET_DIFFUSE)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(MRT_GAMEOBJECTS, TARGET_NORMAL)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(MRT_GAMEOBJECTS, TARGET_DEPTH)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(MRT_GAMEOBJECTS, TARGET_AMBIENT)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(MRT_GAMEOBJECTS, TARGET_PICKPOS)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(MRT_LIGHTACC, TARGET_SHADE)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(MRT_LIGHTACC, TARGET_SPECULAR)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(MRT_SHADOWOBJECTS, TARGET_LIGHTDEPTH)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(MRT_POSTPROCESS_IN, TARGET_COMBINED)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(MRT_OUTLINEMASK, TARGET_OUTLINEMASK)))
		return E_FAIL;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(vViewportDesc.x, vViewportDesc.y, 1.f));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(vViewportDesc.x, vViewportDesc.y, 0.f, 1.f));

#ifdef _DEBUG
	_float fSizeX = 256.f;
	_float fSizeY = 144.f;

	if (FAILED(m_pGameInstance->Ready_RT_Debug(TARGET_DIFFUSE, fSizeX * 0.5f, fSizeY * 0.5f, fSizeX, fSizeY)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TARGET_NORMAL, fSizeX * 0.5f, fSizeY * 1.5f, fSizeX, fSizeY)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TARGET_AMBIENT, fSizeX * 0.5f, fSizeY * 2.5f, fSizeX, fSizeY)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TARGET_SHADE, fSizeX * 1.5f, fSizeY * 0.5f, fSizeX, fSizeY)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TARGET_SPECULAR, fSizeX * 1.5f, fSizeY * 1.5f, fSizeX, fSizeY)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TARGET_LIGHTDEPTH, fSizeX * 1.5f, fSizeY * 2.5f, fSizeX, fSizeY)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TARGET_COMBINED, fSizeX * 0.5f, fSizeY * 3.5f, fSizeX, fSizeY)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TARGET_OUTLINEMASK, fSizeX * 1.5f, fSizeY * 3.5f, fSizeX, fSizeY)))
		return E_FAIL;
#endif

	return S_OK;
}

void CRenderer::Set_DecalTexture(CTexture* pTexture, _uint iTextureIndex)
{
	if (m_pDecalTexture == pTexture && m_iDecalTextureIndex == iTextureIndex)
		return;

	Safe_AddRef(pTexture);
	Safe_Release(m_pDecalTexture);

	m_pDecalTexture = pTexture;
	m_iDecalTextureIndex = iTextureIndex;
}

#ifdef _DEBUG
void CRenderer::Add_DebugComponent(CComponent* pComponent)
{
	m_DebugComponents.push_back(pComponent);
	Safe_AddRef(pComponent);
}
#endif

HRESULT CRenderer::Render_Priority()
{
	for (auto& pRenderObject : m_RenderObjects[ETOUI(RENDERID::PRIORITY)])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[ETOUI(RENDERID::PRIORITY)].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Shadow()
{
	if (FAILED(m_pGameInstance->Begin_MRT(MRT_SHADOWOBJECTS, m_pMaxDSV)))
		return E_FAIL;

	Change_ViewportDesc(g_iMaxWidth, g_iMaxHeight);

	for (auto& pRenderObject : m_RenderObjects[ETOUI(RENDERID::SHADOW)])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render_Shadow();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[ETOUI(RENDERID::SHADOW)].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;


	_float2 vViewportSize = m_pGameInstance->Get_ViewportSize();
	Change_ViewportDesc(static_cast<_uint>(vViewportSize.x), static_cast<_uint>(vViewportSize.y));

	return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
	if (FAILED(m_pGameInstance->Begin_MRT(MRT_GAMEOBJECTS)))
		return E_FAIL;

	for (auto& pRenderObject : m_RenderObjects[ETOUI(RENDERID::NONBLEND)])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[ETOUI(RENDERID::NONBLEND)].clear();

	ID3D11ShaderResourceView* nullSRV[] = {nullptr, nullptr};

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_OutlineMask()
{
	if (FAILED(m_pGameInstance->Begin_MRT(MRT_OUTLINEMASK)))
		return E_FAIL;

	for (auto& pRenderObject : m_RenderObjects[ETOUI(RENDERID::OUTLINEMASK)])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render_OutlineMask();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[ETOUI(RENDERID::OUTLINEMASK)].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Lights()
{
	if (FAILED(m_pGameInstance->Begin_MRT(MRT_LIGHTACC)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TARGET_NORMAL, m_pShader, "g_TexNorm")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TARGET_DEPTH, m_pShader, "g_TexDepth")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TARGET_AMBIENT, m_pShader, "g_TexAmbt")))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewInvMatrix", m_pGameInstance->Get_Transform_Inverse(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjInvMatrix", m_pGameInstance->Get_Transform_Inverse(D3DTS::PROJ))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_vCamPos", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_fFarZ", m_pGameInstance->Get_FarZPtr(), sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_Light(m_pShader, m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Combined(_bool m_bUseShadow)
{
	if (FAILED(m_pGameInstance->Begin_MRT(MRT_POSTPROCESS_IN)))
		return E_FAIL;

	_uint iShaderPass = ETOUI(DEFERRED::COMBINED);

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TARGET_DIFFUSE, m_pShader, "g_TexDiff")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TARGET_SHADE, m_pShader, "g_TexShade")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TARGET_SPECULAR, m_pShader, "g_TexSpec")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TARGET_DEPTH, m_pShader, "g_TexDepth")))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	const _bool bUseDecal =
		true == m_DecalParam.bEnable && nullptr != m_pDecalTexture && 0.f < m_DecalParam.fStrength;

	if (m_bUseShadow || bUseDecal)
	{
		if (FAILED(m_pShader->Bind_Matrix("g_ViewInvMatrix", m_pGameInstance->Get_Transform_Inverse(D3DTS::VIEW))))
			return E_FAIL;

		if (FAILED(m_pShader->Bind_Matrix("g_ProjInvMatrix", m_pGameInstance->Get_Transform_Inverse(D3DTS::PROJ))))
			return E_FAIL;

		if (FAILED(m_pShader->Bind_RawValue("g_fFarZ", m_pGameInstance->Get_FarZPtr(), sizeof(_float))))
			return E_FAIL;
	}

	const _int iUseDecal = bUseDecal ? 1 : 0;

	if (m_iBoundUseDecal != iUseDecal)
	{
		if (FAILED(m_pShader->Bind_RawValue("g_iUseDecal", &iUseDecal, sizeof(_int))))
			return E_FAIL;

		m_iBoundUseDecal = iUseDecal;
	}

	if (bUseDecal)
	{
		if (FAILED(m_pShader->Bind_RawValue("g_fDecalStrength", &m_DecalParam.fStrength, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_fDecalTiling", &m_DecalParam.fTiling, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_fDecalTime", &m_DecalParam.fTime, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_vDecalScrollDir", &m_DecalParam.vScrollDir, sizeof(_float2))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_fDecalSpeed", &m_DecalParam.fSpeed, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_fDecalCoverageLow", &m_DecalParam.fCoverageLow, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_fDecalCoverageHigh", &m_DecalParam.fCoverageHigh, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_fDecalDarkness", &m_DecalParam.fDarkness, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pDecalTexture->Bind_ShaderResource(m_pShader, "g_TexDecal", m_iDecalTextureIndex)))
			return E_FAIL;
	}

	if (m_bUseShadow)
	{
		iShaderPass++;

		if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TARGET_LIGHTDEPTH, m_pShader, "g_TexLightDepth")))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_Matrix("g_SLViewMatrix", m_pGameInstance->Get_Shadow_Transform(D3DTS::VIEW))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_Matrix("g_SLProjMatrix", m_pGameInstance->Get_Shadow_Transform(D3DTS::PROJ))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Bind_Shadow_FarZ(m_pShader)))
			return E_FAIL;
	}

	if (FAILED(m_pVIBuffer->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(iShaderPass)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_PostProcess()
{
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TARGET_COMBINED, m_pShader_PostProcess, "g_TexCombined")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TARGET_NORMAL, m_pShader_PostProcess, "g_TexNorm")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TARGET_DEPTH, m_pShader_PostProcess, "g_TexDepth")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TARGET_OUTLINEMASK, m_pShader_PostProcess, "g_TexOutlineMask")))
		return E_FAIL;

	if (FAILED(m_pShader_PostProcess->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader_PostProcess->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader_PostProcess->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	_float2 vViewportSize = m_pGameInstance->Get_ViewportSize();
	_float2 vTexelSize = _float2(1.f / vViewportSize.x, 1.f / vViewportSize.y);

	if (FAILED(m_pShader_PostProcess->Bind_RawValue("g_vTexelSize", &vTexelSize, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShader_PostProcess->Bind_RawValue("g_fFarZ", m_pGameInstance->Get_FarZPtr(), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader_PostProcess->Bind_RawValue("g_iOutlineMode", &m_OutlineParam.iMode, sizeof(_int))))
		return E_FAIL;
	if (FAILED(m_pShader_PostProcess->Bind_RawValue("g_fOutlineStrength", &m_OutlineParam.fStrength, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader_PostProcess->Bind_RawValue("g_fOutlineDepthStrength",
		&m_OutlineParam.fDepthStrength, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader_PostProcess->Bind_RawValue("g_fOutlineNormalStrength",
		&m_OutlineParam.fNormalStrength, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader_PostProcess->Bind_RawValue("g_fOutlineThresholdLow",
		&m_OutlineParam.fThresholdLow, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader_PostProcess->Bind_RawValue("g_fOutlineThresholdHigh",
		&m_OutlineParam.fThresholdHigh, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader_PostProcess->Bind_RawValue("g_fOutlineThicknessPx",
		&m_OutlineParam.fThicknessPx, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader_PostProcess->Bind_RawValue("g_fOutlineDarkenFactor",
		&m_OutlineParam.fDarkenFactor, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader_PostProcess->Bind_RawValue("g_fOutlineMaskBias",
		&m_OutlineParam.fMaskBias,sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Resources()))
		return E_FAIL;

	_uint iShaderPass = 0;
	if (m_OutlineParam.bEnable && 1 <= m_OutlineParam.iMode && m_OutlineParam.iMode <= 3)
		iShaderPass = 1;
	else if (m_OutlineParam.bEnable && 4 <= m_OutlineParam.iMode && m_OutlineParam.iMode <= 5)
		iShaderPass = 2;

	if (FAILED(m_pShader_PostProcess->Begin(iShaderPass)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
	for (auto& pRenderObject : m_RenderObjects[ETOUI(RENDERID::NONLIGHT)])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[ETOUI(RENDERID::NONLIGHT)].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
	for (auto& pRenderObject : m_RenderObjects[ETOUI(RENDERID::BLEND)])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[ETOUI(RENDERID::BLEND)].clear();

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	auto& UIList = m_RenderObjects[ETOUI(RENDERID::UI)];

	UIList.sort([](CGameObject* pL, CGameObject* pR)
		{
			return pL->Get_RenderOrder() < pR->Get_RenderOrder();
		});

	for (auto& pRenderObject : UIList)
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	UIList.clear();
	return S_OK;
}

HRESULT CRenderer::Ready_DepthStencil_Buffer()
{
	ID3D11Texture2D* pDepthStencilTexture = { nullptr };

	D3D11_TEXTURE2D_DESC TextureDesc{};
	TextureDesc.Width = g_iMaxWidth;
	TextureDesc.Height = g_iMaxHeight;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pMaxDSV)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);

	return S_OK;
}

HRESULT CRenderer::Change_ViewportDesc(_uint iWidth, _uint iHeight)
{
	D3D11_VIEWPORT ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)iWidth;
	ViewPortDesc.Height = (_float)iHeight;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);

	return S_OK;
}

#ifdef _DEBUG

HRESULT CRenderer::Render_Debug()
{
	for (auto& pDebugCom : m_DebugComponents)
	{
		pDebugCom->Render();
		Safe_Release(pDebugCom);
	}

	m_DebugComponents.clear();

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Resources()))
		return E_FAIL;


	if (m_pGameInstance->Is_Debug())
	{
		m_pGameInstance->Render_RT_Debug(MRT_GAMEOBJECTS, m_pShader, m_pVIBuffer);
		m_pGameInstance->Render_RT_Debug(MRT_LIGHTACC, m_pShader, m_pVIBuffer);
		m_pGameInstance->Render_RT_Debug(MRT_SHADOWOBJECTS, m_pShader, m_pVIBuffer);
		m_pGameInstance->Render_RT_Debug(MRT_POSTPROCESS_IN, m_pShader, m_pVIBuffer);
		m_pGameInstance->Render_RT_Debug(MRT_OUTLINEMASK, m_pShader, m_pVIBuffer);
	}
	return S_OK;
}

#endif

CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
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
	for (auto& RenderObjects : m_RenderObjects)
	{
		for (auto& pRenderObject : RenderObjects)
			Safe_Release(pRenderObject);

		RenderObjects.clear();
	}

	Safe_Release(m_pDecalTexture);
	Safe_Release(m_pMaxDSV);
	Safe_Release(m_pShader);
	Safe_Release(m_pShader_PostProcess);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	__super::Free();
}
