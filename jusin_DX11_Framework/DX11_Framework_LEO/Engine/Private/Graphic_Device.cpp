#include "Graphic_Device.h"

CGraphic_Device::CGraphic_Device()
	: m_pDevice{ nullptr }
	, m_pContext{ nullptr }
{
}

HRESULT CGraphic_Device::Initialize(HWND hWnd, WINMODE isWindowed, _uint iWinSizeX, _uint iWinSizeY, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext)
{
	_uint iFlag = 0;

#ifdef _DEBUG
	iFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif	// _DEBUG

	D3D_FEATURE_LEVEL FeatureLV;
	// 지금 생성을 수행한 그래픽카드는 DX 몇 번까지 지원하는가?
	// -> DX11을 지원하지 않는 그래픽카드는 거의 없으므로 활용되지 않는 변수

	/* 그래픽 장치 초기화 */
	if (FAILED(D3D11CreateDevice(	nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, iFlag, nullptr, 0,
									D3D11_SDK_VERSION, &m_pDevice, &FeatureLV, &m_pContext)))
		return E_FAIL;
















	return E_NOTIMPL;
}

HRESULT CGraphic_Device::Clear_BackBuffer_View(const _float4* pClearColor)
{
	return E_NOTIMPL;
}

HRESULT CGraphic_Device::Clear_DepthStencil_View()
{
	return E_NOTIMPL;
}

HRESULT CGraphic_Device::Present()
{
	return E_NOTIMPL;
}

CGraphic_Device* CGraphic_Device::Create(HWND hWnd, WINMODE isWindowed, _uint iWinSizeX, _uint iWinSizeY, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext)
{
	CGraphic_Device* pInstance = new CGraphic_Device();

	if (FAILED(pInstance->Initialize(hWnd, isWindowed, iWinSizeX, iWinSizeY, ppDevice, ppContext)))
	{
		MSG_BOX("Failed to Create : CGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGraphic_Device::Free()
{
	__super::Free();
}
