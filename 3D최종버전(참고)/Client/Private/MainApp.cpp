#include "MainApp.h"
#include "GameInstance.h"

#include "Level_Loading.h"

CMainApp::CMainApp()
	: m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);



		

}

HRESULT CMainApp::Initialize()
{	
	/*m_pContext->OMSetBlendState()*/
	/*D3D11_BLEND_DESC*/
	//D3D11_DEPTH_STENCIL_DESC
	// m_pContext->OMSetDepthStencilState()
	//ID3D11RasterizerState* pRSState;
	//D3D11_RASTERIZER_DESC	RSDesc{};

	//m_pDevice->CreateRasterizerState(&RSDesc, &pRSState);

	//m_pContext->RSSetState(pRSState);

	//m_pContext->OMSetDepthStencilState();	

	//m_pContext->OMSetBlendState();

	/* 초기화ㅏ */
	ENGINE_DESC			EngineDesc{};
	EngineDesc.hInstance = g_hInstance;
	EngineDesc.hWnd = g_hWnd;
	EngineDesc.eWinMode = WINMODE::WIN;
	EngineDesc.iWinSizeX = g_iWinSizeX;
	EngineDesc.iWinSizeY = g_iWinSizeY;
	EngineDesc.iNumLevels = ENUM_CLASS(LEVEL::END);

	/* 엔진의 기능을 이용하기위해 필요한 준비과정을 수행한다. */
	/* 그래픽 디바이스 초기화, 타이머매니져 준비. + etc */
	if (FAILED(m_pGameInstance->Initialize_Engine(EngineDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;
	/*MakeSpriteFont "넥슨Lv1고딕 Bold" /FontSize:20 /FastPack /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 155ex.spritefont */
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Default"), TEXT("../Bin/Resources/Fonts/155ex.spritefont"))))
		return E_FAIL;

	if (FAILED(Start_Level(LEVEL::LOGO)))
		return E_FAIL;

	if (FAILED(Ready_Gara()))
		return E_FAIL;

	return S_OK;
}

void CMainApp::Update(_float fTimeDelta)
{
#ifdef _DEBUG
	m_fTimeAcc += fTimeDelta;
#endif

	m_pGameInstance->Update_Engine(fTimeDelta);
}

HRESULT CMainApp::Render()
{
	_float4		vClearColor = _float4(0.f, 0.f, 1.f, 1.f);

	/* 백, 깊이버퍼를 초기화한다. */
	m_pGameInstance->Begin_Draw(&vClearColor);

	/* 객체들을 그린다. */ 
	m_pGameInstance->Draw();	

#ifdef _DEBUG
	++m_iNumDraw;

	if (1.f <= m_fTimeAcc)
	{
		wsprintf(m_szFPS, TEXT("FPS:%d"), m_iNumDraw);
		m_fTimeAcc = 0.f;
		m_iNumDraw = 0;
	}

	m_pGameInstance->Draw_Font(TEXT("Font_Default"), m_szFPS, _float2(0.f, 0.f));
#endif

	/* 후면버퍼를 전면으로 보여준다. */
	m_pGameInstance->End_Draw();

	return S_OK;
}


HRESULT CMainApp::Ready_Gara()
{	
	_ulong			dwByte = { 0 };
	HANDLE			hFile = CreateFile(TEXT("../Bin/DataFiles/Navigation.dat"), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	_float3			vPoints[3] = {};

	vPoints[0] = _float3(0.f, 0.f, 10.f);
	vPoints[1] = _float3(10.f, 3.f, 0.f);
	vPoints[2] = _float3(0.f, 5.f, 0.f);
	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	vPoints[0] = _float3(0.f, 0.f, 10.f);
	vPoints[1] = _float3(10.f, 0.f, 10.f);
	vPoints[2] = _float3(10.f, 3.f, 0.f);
	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	vPoints[0] = _float3(0.f, 0.f, 20.f);
	vPoints[1] = _float3(10.f, 0.f, 10.f);
	vPoints[2] = _float3(0.f, 0.f, 10.f);
	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	vPoints[0] = _float3(10.f, 0.f, 10.f);
	vPoints[1] = _float3(20.f, 0.f, 0.f);
	vPoints[2] = _float3(10.f, 3.f, 0.f);
	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	CloseHandle(hFile);


	ID3D11Texture2D* pTexture2D = nullptr;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	/* 깊이 버퍼의 픽셀은 백버퍼의 픽셀과 갯수가 동일해야만 깊이 텍스트가 가능해진다. */
	/* 픽셀의 수가 다르면 아에 렌더링을 못함. */
	TextureDesc.Width = 256;
	TextureDesc.Height = 256;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;
	
	TextureDesc.Usage = D3D11_USAGE_STAGING;
	
	TextureDesc.BindFlags = 0;
	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	TextureDesc.MiscFlags = 0;

	_uint* pPixels = new _uint[256 * 256];
	ZeroMemory(pPixels, sizeof(_uint) * 256 * 256);

	/* a b g r */
	pPixels[0] = 0xffff00ff;

	D3D11_SUBRESOURCE_DATA		SubResource{};
	SubResource.pSysMem = pPixels;
	SubResource.SysMemPitch = sizeof(_uint) * 256;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, &SubResource, &pTexture2D)))
		return E_FAIL;

	D3D11_MAPPED_SUBRESOURCE		MappedSubResource{};	

	m_pContext->Map(pTexture2D, 0, D3D11_MAP_READ_WRITE, 0, &MappedSubResource);

	_uint* pTexturePixels = static_cast<_uint*>(MappedSubResource.pData);

	for (size_t i = 0; i < 256; i++)
	{
		for (size_t j = 0; j < 256; j++)
		{
			_uint		iIndex = i * 256 + j;

			if(j < 128)
				pTexturePixels[iIndex] = 0xffffffff;
			else
				pTexturePixels[iIndex] = 0xff000000;
		}
	}

	m_pContext->Unmap(pTexture2D, 0);

	DirectX::SaveDDSTextureToFile(m_pContext, pTexture2D, TEXT("../Bin/Resources/TExtures/Terrain/Mask.dds"));	

	Safe_Release(pTexture2D);

	Safe_Delete_Array(pPixels);

	return S_OK;
}

HRESULT CMainApp::Start_Level(LEVEL eStartLevelID)
{
	return m_pGameInstance->Change_Level(ENUM_CLASS(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, eStartLevelID));	
}

CMainApp* CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{	
	__super::Free();

	/* 내 멤버를 정리하낟. */	
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	m_pGameInstance->Release_Engine();

	m_pGameInstance->DestroyInstance();
	
	
	
}
