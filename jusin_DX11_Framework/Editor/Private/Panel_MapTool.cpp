#include "Panel_MapTool.h"

CPanel_MapTool::CPanel_MapTool()
	: CPanel_Base()
{
}

HRESULT CPanel_MapTool::Initialize()
{
	m_strTitle = "Map";

	if (FAILED(__super::Initialize()))
		return E_FAIL;

	return S_OK;
}

void CPanel_MapTool::Update(_float fTimeDelta)
{
}

HRESULT CPanel_MapTool::Render()
{
	return S_OK;
}

HRESULT CPanel_MapTool::Ready_EditableTexture(const _tchar* pFileDir)
{
	//ID3D11Texture2D* pTexture2D = { nullptr };
	//D3D11_TEXTURE2D_DESC TextureDesc{};

	//TextureDesc.Width = 256;
	//TextureDesc.Height = 256;
	//TextureDesc.MipLevels = 1;
	//TextureDesc.ArraySize = 1;
	//TextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//TextureDesc.SampleDesc.Quality = 0;
	//TextureDesc.SampleDesc.Count = 1;
	//TextureDesc.Usage = D3D11_USAGE_STAGING;
	//TextureDesc.BindFlags = 0;
	//TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	//TextureDesc.MiscFlags = 0;

	//_uint* pPixels = new _uint[256 * 256];

	//for (size_t i = 0; i < 256; i++)
	//{
	//	for (size_t j = 0; j < 256; j++)
	//	{
	//		_uint   iIndex = i * 256 + j;

	//		pPixels[iIndex] = 0xffffffff;
	//	}
	//}

	//D3D11_SUBRESOURCE_DATA InitialData{};
	//InitialData.pSysMem = pPixels;
	//InitialData.SysMemPitch = 256 * 4;

	//if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, &InitialData, &pTexture2D)))
	//	return E_FAIL;

	//D3D11_MAPPED_SUBRESOURCE SubResource{};

	//if (FAILED(m_pContext->Map(pTexture2D, 0, D3D11_MAP_READ_WRITE, 0, &SubResource)))
	//	return E_FAIL;

	//_uint* pTexturePixels = static_cast<_uint*>(SubResource.pData);

	//for (size_t i = 0; i < 256; i++)
	//{
	//	for (size_t j = 0; j < 256; j++)
	//	{
	//		_uint   iIndex = i * 256 + j;

	//		if (j < 128)
	//			pTexturePixels[iIndex] = D3DCOLOR_ARGB(255, 255, 255, 255); /* a, b, g, r */
	//		else
	//			pTexturePixels[iIndex] = D3DCOLOR_ARGB(255, 0, 0, 0);
	//	}
	//}

	//m_pContext->Unmap(pTexture2D, 0);

	//SaveDDSTextureToFile(m_pContext, pTexture2D, pFileDir);

	return S_OK;
}

CPanel_MapTool* CPanel_MapTool::Create()
{
	CPanel_MapTool* pInstance = new CPanel_MapTool();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create : CPanel_MapTool");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPanel_MapTool::Free()
{
	__super::Free();
}
