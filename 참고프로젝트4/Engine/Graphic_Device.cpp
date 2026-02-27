#include "..\public\Graphic_Device.h"

CGraphic_Device::CGraphic_Device()
	: m_pDevice(nullptr)
	, m_pDeviceContext(nullptr)
{	

}

HRESULT CGraphic_Device::Initialize(const GRAPHIC_DESC& GraphicDesc, ComPtr<ID3D11Device>* ppDeviceOut, ComPtr<ID3D11DeviceContext>* ppDeviceContextOut)
{
	// LPDIRECT3DTEXTURE9
	// ID3D11Texture2D : 텍스쳐긴한데. 이새끼로는 아무것도 못해. (딴거하나는해 )
	// ID3D11Texture2D로 내가 실제 사용하기위한 텍스쳐 타입을 생성한다.
	/* ID3D11Device, ID3D11DeviceContext 바로 생성한다. */
	/* 스왑체인이라는 객체를 생성하기 시작.(백버퍼텍스쳘르 생성한다. ) */
	/* 백버퍼 뷰를 생성한다. */
	/* 뎁스스텐실 뷰를 생성한다. */

	_uint		iFlag = 0;

#ifdef _DEBUG
	iFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL			FeatureLV;

	/* 그래픽 장치를 초기화한다. */
	/* ID3D11Device, ID3D11DeviceContext 바로 생성한다. */
	if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, iFlag, nullptr, 0, D3D11_SDK_VERSION, m_pDevice.GetAddressOf(), &FeatureLV, m_pDeviceContext.GetAddressOf())))
		RETURN_EFAIL;

	/* SwapChain 전면과 후면버퍼를 번갈아가며 화면에 보여준다.(Present) */

	/* 백버퍼를 생성하기위한 texture2D만든거야. */
	if (FAILED(Ready_SwapChain(GraphicDesc.hWnd, GraphicDesc.eWinMode, GraphicDesc.iBackBufferSizeX, GraphicDesc.iBackBufferSizeY)))
		RETURN_EFAIL;

	/* 스왑체인이 들고 있는 텍스쳐 2ㅇ를 각져와서 이를 바탕으로 백버퍼 렌더타겟뷰 를 만든다.*/
	if (FAILED(Ready_BackBufferRenderTargetView()))
		RETURN_EFAIL;

	if (FAILED(Ready_DepthStencilRenderTargetView(GraphicDesc.iBackBufferSizeX, GraphicDesc.iBackBufferSizeY)))
		RETURN_EFAIL;

	if (FAILED(Ready_ComputeShader()))
		return E_FAIL;

	/* 장치에 바인드해놓을 렌더타겟들과 뎁스스텐실뷰를 셋팅한다. */
	/* 장치는 최대 8개의 렌더타겟을 동시에 들고 있을 수 있다. */
	ID3D11RenderTargetView*		pRTVs[1] = {
		m_pBackBufferRTV.Get()		
	};

	m_pDeviceContext->OMSetRenderTargets(1, pRTVs, m_pDepthStencilView.Get());		
	
	D3D11_VIEWPORT			ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)GraphicDesc.iBackBufferSizeX;
	ViewPortDesc.Height = (_float)GraphicDesc.iBackBufferSizeY;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pDeviceContext->RSSetViewports(1, &ViewPortDesc);

	*ppDeviceOut = m_pDevice;
	*ppDeviceContextOut = m_pDeviceContext;

	return S_OK;
}

HRESULT CGraphic_Device::Clear_BackBuffer_View(_float4 vClearColor)
{
	if (nullptr == m_pDeviceContext)
		RETURN_EFAIL;

	//m_pGraphic_Device->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 
	//	vClearColor, 1.f, 0)
	//

	/* 백버퍼를 초기화한다.  */
	m_pDeviceContext->ClearRenderTargetView(m_pBackBufferRTV.Get(), (_float*)&vClearColor);

 	return S_OK;
}

HRESULT CGraphic_Device::Clear_DepthStencil_View()
{
	if (nullptr == m_pDeviceContext)
		RETURN_EFAIL;

	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	return S_OK;
}

HRESULT CGraphic_Device::Present()
{
	if (nullptr == m_pSwapChain)
		RETURN_EFAIL;

	/* 전면 버퍼와 후면버퍼를 교체하여 후면버퍼를 전면으로 보여주는 역할을 한다. */
	/* 후면버퍼를 직접화면에 보여줄께.*/
	return m_pSwapChain->Present(0, 0);
}


HRESULT CGraphic_Device::Ready_SwapChain(HWND hWnd, GRAPHIC_DESC::WINMODE eWinMode, _uint iWinCX, _uint iWinCY)
{
	ComPtr<IDXGIDevice>			pDevice = { nullptr };
	m_pDevice->QueryInterface(__uuidof(IDXGIDevice), ReCast<void**>(pDevice.GetAddressOf()));

	ComPtr<IDXGIAdapter>		pAdapter = { nullptr };
	pDevice->GetParent(__uuidof(IDXGIAdapter), ReCast<void**>(pAdapter.GetAddressOf()));

	ComPtr<IDXGIFactory>		pFactory = { nullptr };
	pAdapter->GetParent(__uuidof(IDXGIFactory), ReCast<void**>(pFactory.GetAddressOf()));

	/* 스왑체인을 생성한다. = 텍스쳐를 생성하는 행위 + 스왑하는 형태  */
	DXGI_SWAP_CHAIN_DESC		SwapChainDesc;
	ZeroMemory(&SwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
			
	/*텍스쳐(백버퍼)를 생성하는 행위*/
	SwapChainDesc.BufferDesc.Width = iWinCX;
	SwapChainDesc.BufferDesc.Height = iWinCY;

	
	SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.BufferCount = 1;

	/*스왑하는 형태*/
	SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	SwapChainDesc.SampleDesc.Quality = 0;
	SwapChainDesc.SampleDesc.Count = 1;	

	SwapChainDesc.OutputWindow = hWnd;	
	SwapChainDesc.Windowed = eWinMode;
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	/* 백버퍼라는 텍스쳐를 생성했다. */
	if (FAILED(pFactory->CreateSwapChain(m_pDevice.Get(), &SwapChainDesc, m_pSwapChain.GetAddressOf())))
		RETURN_EFAIL;

	return S_OK;
}


HRESULT CGraphic_Device::Ready_BackBufferRenderTargetView()
{
	if (nullptr == m_pDevice)
		RETURN_EFAIL;

	/* 내가 앞으로 사용하기위한 용도의 텍스쳐를 생성하기위한 베이스 데이터를 가지고 있는 객체이다. */
	/* 내가 앞으로 사용하기위한 용도의 텍스쳐 : ID3D11RenderTargetView, ID3D11ShaderResoureView, ID3D11DepthStencilView */
	ID3D11Texture2D*		pBackBufferTexture = nullptr;

	/* 스왑체인이 들고있던 텍스처를 가져와봐. */
	if (FAILED(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBufferTexture)))
		RETURN_EFAIL;

	if (FAILED(m_pDevice->CreateRenderTargetView(pBackBufferTexture, nullptr, m_pBackBufferRTV.GetAddressOf())))
		RETURN_EFAIL;	



	Safe_Release(pBackBufferTexture);

	return S_OK;
}

HRESULT CGraphic_Device::Ready_DepthStencilRenderTargetView(_uint iWinCX, _uint iWinCY)
{
	if (nullptr == m_pDevice)
		RETURN_EFAIL;

	ComPtr<ID3D11Texture2D>		pDepthStencilTexture = { nullptr };
	
	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = iWinCX;
	TextureDesc.Height = iWinCY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	//TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, pDepthStencilTexture.GetAddressOf())))
		RETURN_EFAIL;

	/* RenderTarget */
	/* ShaderResource */
	/* DepthStencil */	

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture.Get(), &depthStencilViewDesc, m_pDepthStencilView.GetAddressOf())))
		RETURN_EFAIL;


	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	if (FAILED(m_pDevice->CreateShaderResourceView(pDepthStencilTexture.Get(), &shaderResourceViewDesc, m_pDepthStencilSRV.GetAddressOf())))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CGraphic_Device::Ready_ComputeShader()
{
	HRESULT hr = 0;
	
	// 하드웨어 디바이스 테스트
	// WARP 디바이스 테스트
	// 레퍼런스 테스트, 귀찮

	/*D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS hwopts;
	m_pDevice->CheckFeatureSupport(D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, &hwopts, sizeof(hwopts));

	if (hwopts.ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x)
	{
		cerr << "D3D_DRIVER_TYPE_HARDWARE" << endl;
		return hr;
	}
	
	D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS hwopts;
	m_pDevice->CheckFeatureSupport(D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, &hwopts, sizeof(hwopts));
	if (hwopts.ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x)
	{

	}*/

	return S_OK;
}

CGraphic_Device* CGraphic_Device::Create(const GRAPHIC_DESC& GraphicDesc, _Inout_ ComPtr<ID3D11Device>* ppDevice,
	_Inout_ ComPtr<ID3D11DeviceContext>* ppContext)
{
	CGraphic_Device* pInstance = new CGraphic_Device();

	if (FAILED(pInstance->Initialize(GraphicDesc, ppDevice, ppContext)))
	{
		MSG_BOX("Failed to Created : CGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGraphic_Device::Free()
{
	m_pDepthStencilView.Reset();
	m_pDepthStencilSRV.Reset();
	m_pBackBufferRTV.Reset();
	m_pDeviceContext.Reset();
	m_pSwapChain.Reset();

#if defined(DEBUG) || defined(_DEBUG)
	ID3D11Debug*		pDebug = { nullptr };
	HRESULT hr = m_pDevice->QueryInterface(__uuidof(ID3D11Debug), ReCast<void**>(&pDebug));
	if (SUCCEEDED(hr))
	{
		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
		OutputDebugStringW(L"                                                                    D3D11 Live Object ref Count Checker \r ");
		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");

		hr = pDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);

		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
		OutputDebugStringW(L"                                                                    D3D11 Live Object ref Count Checker END \r ");
		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
	}
	Safe_Release(pDebug);
#endif

	m_pDevice.Reset();
	
}
