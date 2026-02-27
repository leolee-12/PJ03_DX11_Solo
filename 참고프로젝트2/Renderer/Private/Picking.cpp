#include "Picking.h"
#include "RenderInstance.h"
#include "GameInstance.h"

CPicking::CPicking(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pRenderInstance{ CRenderInstance::Get_Instance() }
	, m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CPicking::Initialize(HWND hWnd)
{
	m_hWnd = hWnd;

	D3D11_VIEWPORT			ViewportDesc{};
	_uint					iNumViewport = { 1 };
	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = ViewportDesc.Width;
	TextureDesc.Height = ViewportDesc.Height;
	m_iWidth = TextureDesc.Width;
	m_iHeight = TextureDesc.Height;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_STAGING;
	TextureDesc.BindFlags = 0;
	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D)))
		return E_FAIL;

	return S_OK;
}

_float4 CPicking::Picked_Position(_bool* pPicked)
{
	_float4		vWorldPosition{};

	POINT		ptMouse{};

	GetCursorPos(&ptMouse);

	ScreenToClient(m_hWnd, &ptMouse);

	/* Target_PickDepth렌더타겟을 미리 만들어놓은 텍스쳐에 복사한다. */
	m_pRenderInstance->Copy_RenderTarget(TEXT("Target_PickDepth"), m_pTexture2D);

	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pTexture2D, 0, D3D11_MAP_READ, 0, &SubResource);

	_float4*	pPickDepth = static_cast<_float4*>(SubResource.pData);

	_uint		iIndex = (float)ptMouse.y * m_iWidth + (float)ptMouse.x;

	_float4		vProjPos;
	
	/* 투영공간상의 픽셀의 위치를 구한다. */
	vProjPos.x = ptMouse.x / (m_iWidth * 0.5f) - 1.f;
	vProjPos.y = ptMouse.y / (m_iHeight * -0.5f) + 1.f;
	vProjPos.z = pPickDepth[iIndex].y;
	vProjPos.w = 1.f;

	/* 뷰공간상의 픽셀의 위치를 구한다. */
	_vector		vViewPos = XMVector3TransformCoord(XMLoadFloat4(&vProjPos), m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_PROJ));

	/* 월드공간상의 픽셀의 위치를 구한다. */
	_vector		vWorldPos = XMVector3TransformCoord(vViewPos, m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_VIEW));

	XMStoreFloat4(&vWorldPosition, vWorldPos);

	m_pContext->Unmap(m_pTexture2D, 0);

	if (1.f == pPickDepth[iIndex].w)
		*pPicked = false;
	else
		*pPicked = true;

	return vWorldPosition;
}

_int CPicking::Picked_Effect_Index()
{
	_float4		vWorldPosition{};

	POINT		ptMouse{};

	GetCursorPos(&ptMouse);

	ScreenToClient(m_hWnd, &ptMouse);

	/* Target_PickDepth렌더타겟을 미리 만들어놓은 텍스쳐에 복사한다. */
	m_pRenderInstance->Copy_RenderTarget(TEXT("Target_PickDepth"), m_pTexture2D);

	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pTexture2D, 0, D3D11_MAP_READ, 0, &SubResource);

	_float4* pPickDepth = static_cast<_float4*>(SubResource.pData);

	_uint		iIndex = (float)ptMouse.y * m_iWidth + (float)ptMouse.x;

	m_pContext->Unmap(m_pTexture2D, 0);
	if (iIndex > 1920 * 1080)
		return -1;
	
	return (_int)pPickDepth[iIndex].z;
}

//
//_float4 CPicking::Picked_Position(const _float4 & _vWorldPos, _bool * pPicked)
//{
//	/*_vWorldPos.xyz * 수직내려다보는 카메라 뷰행렬을 곱하면. 투영행렬. x, y는 곧 x, z위치를 의미. */
//
//	_float4		vWorldPosition{};
//
//	_vector		vPickPos = XMVector3TransformCoord(XMLoadFloat4(&_vWorldPos), m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
//	vPickPos = XMVector3TransformCoord(vPickPos, m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
//
//	_float		fHalfWidth = m_iWidth * 0.5f;
//	_float		fHalfHeight = m_iHeight * 0.5f;
//
//	_float2		vViewPos = _float2(XMVectorGetX(vPickPos) * fHalfWidth + fHalfWidth,
//		XMVectorGetY(vPickPos) * -fHalfHeight+ fHalfHeight);
//
//	/* Target_PickDepth렌더타겟을 미리 만들어놓은 텍스쳐에 복사한다. */
//	m_pGameInstance->Copy_RenderTarget(TEXT("Target_PickDepth"), m_pTexture2D);
//
//	D3D11_MAPPED_SUBRESOURCE		SubResource{};
//
//	m_pContext->Map(m_pTexture2D, 0, D3D11_MAP_READ, 0, &SubResource);
//
//	_float4*	pPickDepth = static_cast<_float4*>(SubResource.pData);
//
//	_uint		iIndex = (float)vViewPos.y * m_iWidth + (float)vViewPos.x;
//
//	_float4		vProjPos;
//
//	/* 투영공간상의 픽셀의 위치를 구한다. */
//	vProjPos.x = XMVectorGetX(vPickPos);
//	vProjPos.y = XMVectorGetY(vPickPos);
//	vProjPos.z = pPickDepth[iIndex].y;
//	vProjPos.w = 1.f;
//
//	/* 뷰공간상의 픽셀의 위치를 구한다. */
//	_vector		vViewPosition = XMVector3TransformCoord(XMLoadFloat4(&vProjPos), m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_PROJ));
//
//	/* 월드공간상의 픽셀의 위치를 구한다. */
//	_vector		vWorldPos = XMVector3TransformCoord(vViewPosition, m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_VIEW));
//
//	XMStoreFloat4(&vWorldPosition, vWorldPos);
//
//	m_pContext->Unmap(m_pTexture2D, 0);
//
//	if (1.f == pPickDepth[iIndex].w)
//		*pPicked = false;
//	else
//		*pPicked = true;
//
//	return vWorldPosition;
//}


CPicking * CPicking::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, HWND hWnd)
{
	CPicking*		pInstance = new CPicking(pDevice, pContext);

	if (FAILED(pInstance->Initialize(hWnd)))
	{
		MSG_BOX(TEXT("Failed to Created : CPicking"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CPicking::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pTexture2D);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
	
}
