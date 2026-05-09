#include "VP_RenderTarget.h"

CVP_RenderTarget::CVP_RenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CRenderTarget{ pDevice, pContext }
{
}

HRESULT CVP_RenderTarget::Initialize(ImVec2 vInitialSize)
{
	m_vSize = vInitialSize;

	return Create_Resources();
}

HRESULT CVP_RenderTarget::Begin_SceneRender()
{
    if (nullptr == m_pContext || nullptr == m_pRTV || nullptr == m_pDSV)
        return E_FAIL;

    if (m_vSize.x <= 0.f || m_vSize.y <= 0.f)
        return E_FAIL;

    Safe_Release(m_pPrevRTV);
    m_pPrevRTV = nullptr;
    Safe_Release(m_pPrevDSV);
    m_pPrevDSV = nullptr;

    m_iPrevViewportCount = 1;
    ZeroMemory(&m_tPrevViewport, sizeof(D3D11_VIEWPORT));

    m_pContext->OMGetRenderTargets(1, &m_pPrevRTV, &m_pPrevDSV);
    m_pContext->RSGetViewports(&m_iPrevViewportCount, &m_tPrevViewport);
    m_pContext->OMSetRenderTargets(1, &m_pRTV, m_pDSV);

    D3D11_VIEWPORT tViewport{};
    tViewport.TopLeftX = 0.f;
    tViewport.TopLeftY = 0.f;
    tViewport.Width = m_vSize.x;
    tViewport.Height = m_vSize.y;
    tViewport.MinDepth = 0.f;
    tViewport.MaxDepth = 1.f;
    m_pContext->RSSetViewports(1, &tViewport);

    const _float vClearColor[4] = { 0.10f, 0.12f, 0.15f, 1.f };
    m_pContext->ClearRenderTargetView(m_pRTV, vClearColor);
    m_pContext->ClearDepthStencilView(m_pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

    return S_OK;
}

HRESULT CVP_RenderTarget::End_SceneRender()
{
    if (nullptr == m_pContext)
        return E_FAIL;

    m_pContext->OMSetRenderTargets(1, &m_pPrevRTV, m_pPrevDSV);

    if (m_iPrevViewportCount > 0)
        m_pContext->RSSetViewports(1, &m_tPrevViewport);

    Safe_Release(m_pPrevRTV);
    m_pPrevRTV = nullptr;
    Safe_Release(m_pPrevDSV);
    m_pPrevDSV = nullptr;

	return S_OK;
}

HRESULT CVP_RenderTarget::Create_Resources()
{
    Release_Resources();

    const _uint iWidth = max(static_cast<_uint>(m_vSize.x), 1u);
    const _uint iHeight = max(static_cast<_uint>(m_vSize.y), 1u);

    if(FAILED(__super::Initialize(iWidth, iHeight, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.1f, 0.12f, 0.15f, 1.f))))
		return E_FAIL;

    D3D11_TEXTURE2D_DESC tDepthDesc{};
    tDepthDesc.Width = iWidth;
    tDepthDesc.Height = iHeight;
    tDepthDesc.MipLevels = 1;
    tDepthDesc.ArraySize = 1;
    tDepthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    tDepthDesc.SampleDesc.Count = 1;
    tDepthDesc.SampleDesc.Quality = 0;
    tDepthDesc.Usage = D3D11_USAGE_DEFAULT;
    tDepthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    if (FAILED(m_pDevice->CreateTexture2D(&tDepthDesc, nullptr, &m_pDSTexture)))
        return E_FAIL;
    if (FAILED(m_pDevice->CreateDepthStencilView(m_pDSTexture, nullptr, &m_pDSV)))
        return E_FAIL;

	return S_OK;
}

void CVP_RenderTarget::Release_Resources()
{
    Safe_Release(m_pDSV);        m_pDSV = nullptr;
    Safe_Release(m_pDSTexture);  m_pDSTexture = nullptr;
}

CVP_RenderTarget* CVP_RenderTarget::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vInitialSize)
{
    CVP_RenderTarget* pInstance = new CVP_RenderTarget(pDevice, pContext);

    if (FAILED(pInstance->Initialize(vInitialSize)))
    {
        MSG_BOX("Failed to Create : CVP_RenderTarget");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CVP_RenderTarget::Free()
{
    Safe_Release(m_pPrevRTV);  m_pPrevRTV = nullptr;
    Safe_Release(m_pPrevDSV);  m_pPrevDSV = nullptr;
    Release_Resources();

    __super::Free();
}