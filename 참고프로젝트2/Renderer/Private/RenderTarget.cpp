#include "..\Public\RenderTarget.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"

CRenderTarget::CRenderTarget(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CRenderTarget::Initialize(_uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, _fvector vClearColor)
{
	XMStoreFloat4(&m_vClearColor, vClearColor);

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = iWidth;
	TextureDesc.Height = iHeight;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = ePixelFormat;
	
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;


	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture2D, nullptr, &m_pRTV)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture2D, nullptr, &m_pSRV)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderTarget::Bind_ShaderResource(CShader * pShader, const _char * pConstantName)
{
	return pShader->Bind_ShaderResourceView(pConstantName, m_pSRV);
}

void CRenderTarget::Clear()
{
	m_pContext->ClearRenderTargetView(m_pRTV, (_float*)&m_vClearColor);
}

HRESULT CRenderTarget::Copy_RenderTarget(ID3D11Texture2D * pTexture2D)
{
	m_pContext->CopyResource(pTexture2D, m_pTexture2D);	

	return S_OK;
}

ID3D11ShaderResourceView* CRenderTarget::Copy_ShaderResourceView()
{
	return m_pSRV;
}

#ifdef _DEBUG

HRESULT CRenderTarget::Ready_Debug(_float fCenterX, _float fCenterY, _float fSizeX, _float fSizeY)
{
	D3D11_VIEWPORT			ViewportDesc{};
	_uint					iNumViewport = { 1 };
	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	m_WorldMatrix._11 = fSizeX;
	m_WorldMatrix._22 = fSizeY;

	m_WorldMatrix._41 = fCenterX - ViewportDesc.Width * 0.5f;
	m_WorldMatrix._42 = -fCenterY + ViewportDesc.Height * 0.5f;

	return S_OK;
}

HRESULT CRenderTarget::Render_Debug(CShader * pShader, CVIBuffer_Rect * pVIBuffer)
{
	if (FAILED(pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(pShader->Bind_ShaderResourceView("g_Texture", m_pSRV)))
		return E_FAIL;

	pShader->Begin(0);

	pVIBuffer->Bind_Buffers();

	pVIBuffer->Render();

	return S_OK;
}

#endif

CRenderTarget * CRenderTarget::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, _fvector vClearColor)
{
	CRenderTarget*		pInstance = new CRenderTarget(pDevice, pContext);

	if (FAILED(pInstance->Initialize(iWidth, iHeight, ePixelFormat, vClearColor)))
	{
		MSG_BOX(TEXT("Failed to Created : CRenderTarget"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CRenderTarget::Free()
{
	__super::Free();

	// SaveDDSTextureToFile(m_pContext, m_pTexture2D, TEXT("../Bin/Tmp.dds"));

	Safe_Release(m_pSRV);
	Safe_Release(m_pRTV);
	Safe_Release(m_pTexture2D);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
