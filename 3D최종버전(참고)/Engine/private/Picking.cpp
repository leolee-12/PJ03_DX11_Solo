#include "Picking.h"

#include "GameInstance.h"

CPicking::CPicking(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice { pDevice }
    , m_pContext { pContext }
	, m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CPicking::Initialize(HWND hWnd, _uint iWidth, _uint iHeight)
{	
	m_hWnd = hWnd;
	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	/* 깊이 버퍼의 픽셀은 백버퍼의 픽셀과 갯수가 동일해야만 깊이 텍스트가 가능해진다. */
	/* 픽셀의 수가 다르면 아에 렌더링을 못함. */
	TextureDesc.Width = iWidth;
	TextureDesc.Height = iHeight;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_STAGING;

	TextureDesc.BindFlags = 0;
	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D)))
		return E_FAIL;

	m_iNumPixels = iWidth * iHeight;
	m_iNumPixelX = iWidth;

	m_pPixelPositions = new _float4[m_iNumPixels];
	ZeroMemory(m_pPixelPositions, sizeof(_float4) * m_iNumPixels);

    return S_OK;
}

void CPicking::Update()
{
	if (FAILED(m_pGameInstance->Copy_RT_Resource(TEXT("Target_World"), m_pTexture2D)))
		return;

	D3D11_MAPPED_SUBRESOURCE		SubResource{};
	m_pContext->Map(m_pTexture2D, 0, D3D11_MAP_READ_WRITE, 0, &SubResource);

	memcpy(m_pPixelPositions, SubResource.pData, sizeof(_float4) * m_iNumPixels);

	m_pContext->Unmap(m_pTexture2D, 0);
	
}

_bool CPicking::Picking(_float3* pOut)
{
	::POINT		ptMouse = {};

	GetCursorPos(&ptMouse);
	ScreenToClient(m_hWnd, &ptMouse);

	/* 타일과 마우스 충돌 */
	_uint		iIndex = ptMouse.y * m_iNumPixelX + ptMouse.x;

	if (0.f == m_pPixelPositions[iIndex].w)
		return false;

	memcpy(pOut, &m_pPixelPositions[iIndex], sizeof(_float3));

    return true;
}

CPicking* CPicking::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd, _uint iWidth, _uint iHeight)
{
    CPicking* pInstance = new CPicking(pDevice, pContext);

    if (FAILED(pInstance->Initialize(hWnd, iWidth, iHeight)))
    {
        MSG_BOX("Failed to Created : CPicking");
        Safe_Release(pInstance);
    }

    return pInstance;
}


void CPicking::Free()
{
    __super::Free();

	Safe_Release(m_pTexture2D);
	Safe_Delete_Array(m_pPixelPositions);
	Safe_Release(m_pGameInstance);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
