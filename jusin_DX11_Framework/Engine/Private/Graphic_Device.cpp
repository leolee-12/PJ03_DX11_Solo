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
	if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, iFlag, nullptr,
		0, D3D11_SDK_VERSION, &m_pDevice, &FeatureLV, &m_pContext)))
		return E_FAIL;

#if defined(_DEBUG)	// FX11 디버그 네이밍 노이즈 경고 제거
	ID3D11InfoQueue* pInfoQueue = nullptr;
	if (SUCCEEDED(m_pDevice->QueryInterface(__uuidof(ID3D11InfoQueue),
		reinterpret_cast<void**>(&pInfoQueue))))
	{
		D3D11_MESSAGE_ID HideIDs[] =
		{
				D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS
		};

		D3D11_INFO_QUEUE_FILTER Filter = {};
		Filter.DenyList.NumIDs = _countof(HideIDs);
		Filter.DenyList.pIDList = HideIDs;

		pInfoQueue->AddStorageFilterEntries(&Filter);
		Safe_Release(pInfoQueue);
	}
#endif

	/* 스왑체인 객체 생성 */
	if (FAILED(Ready_SwapChain(hWnd, isWindowed, iWinSizeX, iWinSizeY)))
		return E_FAIL;

	m_hWnd = hWnd;	// Game.lib 전달용으로 보관

	/* 백버퍼 렌더타겟뷰 생성*/
	if (FAILED(Ready_BackBufferRenderTargetView()))
		return E_FAIL;

	/* 뎁스스텐실뷰 생성 */
	if (FAILED(Ready_DepthStencilView(iWinSizeX, iWinSizeY)))
		return E_FAIL;

	/* 장치에 바인드할 렌더 타겟, 뎁스스텐실뷰 세팅 */
	ID3D11RenderTargetView* pRTVs[] = {
		m_pBackBufferRTV,
	};

	m_pContext->OMSetRenderTargets(1, pRTVs, m_pDepthStencilView);

	D3D11_VIEWPORT			ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = static_cast<_float>(iWinSizeX);
	ViewPortDesc.Height = static_cast<_float>(iWinSizeY);
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);

	*ppDevice = m_pDevice;
	*ppContext = m_pContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	return S_OK;
}

HRESULT CGraphic_Device::Clear_BackBuffer_View(const _float4* pClearColor)
{
	if (nullptr == m_pContext)
		return E_FAIL;

	/* 백버퍼 초기화 */
	m_pContext->ClearRenderTargetView(m_pBackBufferRTV, reinterpret_cast<const _float*>(pClearColor));

	return S_OK;
}

HRESULT CGraphic_Device::Clear_DepthStencil_View()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	/* 깊이, 스텐실버퍼 초기화 */
	m_pContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	return S_OK;
}

HRESULT CGraphic_Device::Present()
{
	if (nullptr == m_pSwapChain)
		return E_FAIL;

	/* 전후면버퍼 교체 -> 후면버퍼를 전면으로 보여줌 */
	return m_pSwapChain->Present(0, 0);
}

HRESULT CGraphic_Device::Resize_Backbuffer(_uint iNewWidth, _uint iNewHeight)
{
	// 1. 정리(바인딩, RTV, DSV)
	m_pContext->OMSetRenderTargets(0, nullptr, nullptr);
	Safe_Release(m_pBackBufferRTV);
	Safe_Release(m_pDepthStencilView);

	// 2. 스왑체인 크기 변경 & RTV, DSV 재생성 후 바인딩
	m_pSwapChain->ResizeBuffers(0, iNewWidth, iNewHeight, DXGI_FORMAT_UNKNOWN, 0);
	if (FAILED(Ready_BackBufferRenderTargetView()))
		return E_FAIL;
	if (FAILED(Ready_DepthStencilView(iNewWidth, iNewHeight)))
		return E_FAIL;
	m_pContext->OMSetRenderTargets(1, &m_pBackBufferRTV, m_pDepthStencilView);
	
	// 3. 뷰포트 크기 정보 재생성 & 바인딩
	D3D11_VIEWPORT tNewVPDesc;
	ZeroMemory(&tNewVPDesc, sizeof(D3D11_VIEWPORT));
	tNewVPDesc.TopLeftX = 0;
	tNewVPDesc.TopLeftY = 0;
	tNewVPDesc.Width = static_cast<_float>(iNewWidth);
	tNewVPDesc.Height = static_cast<_float>(iNewHeight);
	tNewVPDesc.MinDepth = 0.f;
	tNewVPDesc.MaxDepth = 1.f;
	m_pContext->RSSetViewports(1, &tNewVPDesc);

	return S_OK;
}

HRESULT CGraphic_Device::Ready_SwapChain(HWND hWnd, WINMODE isWindowed, _uint iWinCX, _uint iWinCY)
{
	IDXGIDevice* pDevice = nullptr;
	m_pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDevice);

	IDXGIAdapter* pAdapter = nullptr;
	pDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&pAdapter);

	IDXGIFactory* pFactory = nullptr;
	pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pFactory);

	/* 스왑체인을 생성한다. = 텍스쳐를 생성하는 행위 + 스왑하는 형태  */
	DXGI_SWAP_CHAIN_DESC		SwapChain;
	ZeroMemory(&SwapChain, sizeof(DXGI_SWAP_CHAIN_DESC));

	/* 백버퍼 == 텍스쳐 */
	/*텍스처(백버퍼 == ID3D11Texture2D)를 생성하는 행위*/
	SwapChain.BufferDesc.Width = iWinCX;	/* 가로 픽셀 수 */
	SwapChain.BufferDesc.Height = iWinCY;	/* 세로 픽셀 수 */

	/* float4(1.f, 1.f, 1.f, 1.f) */
	/* float4(1.f, 0.f, 0.f, 1.f) */

	SwapChain.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; /*D3DFMT_A8R8G8B8*/ /* 만든 픽셀하나의 데이터 정보 : 32BIT픽셀생성하되 부호가 없는 정규화된 수를 저장할께 */
	SwapChain.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChain.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	/* 스케치북에 사과를 그릴꺼야. */
	/* RENDER_TARGET : 그림을 당하는 대상. 스케치북 */
	SwapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChain.BufferCount = 1;

	/*스왑하는 형태 : 모니터 주사율에 따라 조절해도 됨. */
	SwapChain.BufferDesc.RefreshRate.Numerator = 60;
	SwapChain.BufferDesc.RefreshRate.Denominator = 1;

	/* 멀티샘플링 : 안티얼라이징 (계단현상방지) */
	/* 나중에 후처리 렌더링 : 멀티샘플링 지원(x) */
	SwapChain.SampleDesc.Quality = 0;
	SwapChain.SampleDesc.Count = 1;

	SwapChain.OutputWindow = hWnd;
	SwapChain.Windowed = static_cast<BOOL>(isWindowed);
	SwapChain.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	/* 백버퍼라는 텍스처(ID3D11Texture2D)를 생성했다. */
	if (FAILED(pFactory->CreateSwapChain(m_pDevice, &SwapChain, &m_pSwapChain)))
		return E_FAIL;

	Safe_Release(pFactory);
	Safe_Release(pAdapter);
	Safe_Release(pDevice);

	return S_OK;
}

HRESULT CGraphic_Device::Ready_BackBufferRenderTargetView()
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	ID3D11Texture2D* pBackBufferTexture = nullptr;

	if (FAILED(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBufferTexture)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateRenderTargetView(pBackBufferTexture, nullptr, &m_pBackBufferRTV)))
		return E_FAIL;

	Safe_Release(pBackBufferTexture);

	return S_OK;
}

HRESULT CGraphic_Device::Ready_DepthStencilView(_uint iWinCX, _uint iWinCY)
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	ID3D11Texture2D* pDepthStencilTexture = { nullptr };

	D3D11_TEXTURE2D_DESC	TextureDesc{};

	TextureDesc.Width = iWinCX;
	TextureDesc.Height = iWinCY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT /* 정적 */;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;

	/* RenderTargetView */
	/* ShaderResourceView */
	/* DepthStencilView */

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pDepthStencilView)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);

	return S_OK;
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
	if (m_pContext != nullptr)
	{
		m_pContext->OMSetRenderTargets(0, nullptr, nullptr);
		m_pContext->ClearState();
		m_pContext->Flush();
	}

	Safe_Release(m_pDepthStencilView);
	Safe_Release(m_pBackBufferRTV);
	Safe_Release(m_pSwapChain);
	Safe_Release(m_pContext);

#if defined(DEBUG) || defined(_DEBUG)
	ID3D11Debug* d3dDebug = nullptr;
	HRESULT hr = m_pDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&d3dDebug));
	if (SUCCEEDED(hr))
	{
		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
		OutputDebugStringW(L"                                                                    D3D11 Live Object ref Count Checker \r ");
		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");

		hr = d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);

		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
		OutputDebugStringW(L"                                                                    D3D11 Live Object ref Count Checker END \r ");
		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
	}
	if (d3dDebug != nullptr) d3dDebug->Release();
#endif

	Safe_Release(m_pDevice);
}
