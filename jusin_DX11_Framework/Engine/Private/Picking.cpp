#include "Picking.h"
#include "GameInstance.h"

CPicking::CPicking(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CPicking::Initialize(HWND hWnd)
{
	m_hWnd = hWnd;

	D3D11_TEXTURE2D_DESC TextureDesc{};

	TextureDesc.Width = 1;
	TextureDesc.Height = 1;
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

_bool CPicking::Picking(_float4& Out)
{
	_float2 vViewportDesc = m_pGameInstance->Get_ViewportSize();

	// 1. 마우스 위치 얻기
	::POINT ptMouse{};
	GetCursorPos(&ptMouse);
	ScreenToClient(m_hWnd, &ptMouse);

	// 2. 경계 검사
	if (ptMouse.x < 0 || ptMouse.y < 0 ||
		ptMouse.x >= static_cast<LONG>(vViewportDesc.x) ||
		ptMouse.y >= static_cast<LONG>(vViewportDesc.y))
		return false;

	// 3. 마우스 위치 1픽셀만 RT로부터 Staging에 복사
	D3D11_BOX srcBox{};
	srcBox.left = static_cast<_uint>(ptMouse.x);
	srcBox.top = static_cast<_uint>(ptMouse.y);
	srcBox.right = srcBox.left + 1;
	srcBox.bottom = srcBox.top + 1;
	srcBox.front = 0;
	srcBox.back = 1;

	if (FAILED(m_pGameInstance->Copy_RT_SubResource(TARGET_PICKPOS, m_pTexture2D, &srcBox)))
		return false;

	// 4. 1픽셀 Map + Read-Only
	D3D11_MAPPED_SUBRESOURCE SubResource{};
	if (FAILED(m_pContext->Map(m_pTexture2D, 0, D3D11_MAP_READ, 0, &SubResource)))
		return false;

	_float4 vWorldPos = *static_cast<_float4*>(SubResource.pData);
	m_pContext->Unmap(m_pTexture2D, 0);

	if (0.f == vWorldPos.w)
		return false;

	Out = vWorldPos;
	return true;
}

CPicking* CPicking::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd)
{
	CPicking* pInstance = new CPicking(pDevice, pContext);

	if (FAILED(pInstance->Initialize(hWnd)))
	{
		MSG_BOX("Failed to Created : CPicking");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CPicking::Free()
{
	__super::Free();

	Safe_Delete_Array(m_pWorldPos);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pTexture2D);
	Safe_Release(m_pGameInstance);
}
