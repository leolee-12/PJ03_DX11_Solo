#include "Target_Manager.h"
#include "RenderTarget.h"
#include "GameInstance.h"

CTarget_Manager::CTarget_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
	 
	 
}

HRESULT CTarget_Manager::Initialize()
{
	return S_OK;
}

HRESULT CTarget_Manager::Add_RenderTarget(const wstring& strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor, _uint iArraySize)
{
	if (nullptr != Find_RenderTarget(strTargetTag))
		RETURN_EFAIL;

	CRenderTarget* pRenderTarget = CRenderTarget::Create(m_pDevice, m_pContext, iSizeX, iSizeY, ePixelFormat, vClearColor, iArraySize);
	if (nullptr == pRenderTarget)
		RETURN_EFAIL;

	m_RenderTargets.emplace(strTargetTag, pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Add_MRT(const wstring& strMRTTag, const wstring& strTargetTag)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		RETURN_EFAIL;

	list<CRenderTarget*>* pMRTList = Find_MRT(strMRTTag);
	if (nullptr == pMRTList)
	{
		list<CRenderTarget*>	MRTList;
		MRTList.push_back(pRenderTarget);

		m_MRTs.emplace(strMRTTag, MRTList);
	}
	else
		pMRTList->push_back(pRenderTarget);

	Safe_AddRef(pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Begin_MRT(const wstring& strMRTTag, _bool bClear, ComPtr<ID3D11DepthStencilView> pDSV)
{
	ID3D11ShaderResourceView* pSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {
	nullptr
	};
	m_pContext->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, pSRV);

	/* strMRTTag에 모여있는 렌더타겟들을 장치에 순차적으로 바인딩한다. */
	list<CRenderTarget*>* pMRTList = Find_MRT(strMRTTag);
	if (nullptr == pMRTList)
		RETURN_EFAIL;
	ID3D11RenderTargetView* pRTVs[8];

	_uint		iNumRTVs = { 0 };

	for (auto& pRenderTarget : *pMRTList)
	{
		if(bClear)
			pRenderTarget->Clear();
		pRTVs[iNumRTVs++] = pRenderTarget->Get_RTV().Get();
	}

	if (nullptr != pDSV)
		m_pContext->ClearDepthStencilView(pDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	m_pContext->OMSetRenderTargets(iNumRTVs, pRTVs, pDSV == nullptr ? m_pGameInstance->Get_DSV().Get() : pDSV.Get()); //strMRTTag에 있는 렌더타겟과 기존 깊이 버퍼 세팅

	return S_OK;
}

HRESULT CTarget_Manager::End_MRT()
{
	ID3D11RenderTargetView* pRTVs[] = {
		m_pGameInstance->Get_BackBufferRTV().Get()
	};

	/* 최초의 상태(백버퍼)로 되돌린다. */
	m_pContext->OMSetRenderTargets(1, pRTVs, m_pGameInstance->Get_DSV().Get());

	return S_OK;
}

HRESULT CTarget_Manager::Bind_ShaderResource(const wstring& strTargetTag, CShader* pShader, const _char* pConstantName)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		RETURN_EFAIL;

	return pRenderTarget->Bind_ShaderResource(pShader, pConstantName); //해당 렌더타겟(->ShaderResourceView) 셰이더에 바인딩
}

#ifdef _DEBUG

HRESULT CTarget_Manager::Ready_Debug(const wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		RETURN_EFAIL;

	return pRenderTarget->Ready_Debug(fX, fY, fSizeX, fSizeY); //디버깅용 렌더타겟 출력하기위한 월드 세팅 
}

HRESULT CTarget_Manager::Render_Debug(const wstring& strMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	list<CRenderTarget*>* pMRTList = Find_MRT(strMRTTag);
	if (nullptr == pMRTList)
		RETURN_EFAIL;

	for (auto& pRenderTarget : *pMRTList)
	{
		//해당 렌더타겟(->ShaderResourceView) 셰이더에 바인딩 , 월드 바인딩, VIBuffer에 출력
		pRenderTarget->Render_Debug(pShader, pVIBuffer); 
	}
	return S_OK;
}

#endif

CRenderTarget* CTarget_Manager::Find_RenderTarget(const wstring& strTargetTag)
{
	auto	iter = m_RenderTargets.find(strTargetTag);

	if (iter == m_RenderTargets.end())
		return nullptr;

	return iter->second;
}

list<class CRenderTarget*>* CTarget_Manager::Find_MRT(const wstring& strMRTTag)
{
	auto	iter = m_MRTs.find(strMRTTag);

	if (iter == m_MRTs.end())
		return nullptr;

	return &iter->second;
}


HRESULT CTarget_Manager::Bake_AntiAliasingDepthStencilView(_uint iWidth, _uint iHeight)
{
	ComPtr<ID3D11Texture2D>		pDepthStencilTexture;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = iWidth;
	TextureDesc.Height = iHeight;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, pDepthStencilTexture.GetAddressOf())))
		RETURN_EFAIL;

	/* RenderTarget */
	/* ShaderResource */
	/* DepthStencil */

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture.Get(), nullptr, m_pAntiAliasingDepthStencilView.GetAddressOf())))
		RETURN_EFAIL;

	ZeroMemory(&m_tAntiAliasingViewPortDesc, sizeof(D3D11_VIEWPORT));
	m_tAntiAliasingViewPortDesc.TopLeftX = 0;
	m_tAntiAliasingViewPortDesc.TopLeftY = 0;
	m_tAntiAliasingViewPortDesc.Width = (FLOAT)iWidth;
	m_tAntiAliasingViewPortDesc.Height = (FLOAT)iHeight;
	m_tAntiAliasingViewPortDesc.MinDepth = 0.f;
	m_tAntiAliasingViewPortDesc.MaxDepth = 1.f;


	return S_OK;
}

CTarget_Manager* CTarget_Manager::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	CTarget_Manager* pInstance = new CTarget_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CTarget_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTarget_Manager::Free()
{
	Safe_Release(m_pGameInstance);

	for (auto& Pair : m_MRTs)
	{
		for (auto& pRenderTarget : Pair.second)
			Safe_Release(pRenderTarget);
		Pair.second.clear();
	}
	m_MRTs.clear();

	for (auto& Pair : m_RenderTargets)
		Safe_Release(Pair.second);

	m_RenderTargets.clear();

	 
	 
}
