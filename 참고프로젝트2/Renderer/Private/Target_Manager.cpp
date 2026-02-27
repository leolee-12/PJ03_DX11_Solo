#include "Target_Manager.h"
#include "RenderTarget.h"

CTarget_Manager::CTarget_Manager(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CTarget_Manager::Initialize()
{
	return S_OK;
}

HRESULT CTarget_Manager::Add_RenderTarget(const _wstring & strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, _fvector vClearColor)
{
	if (nullptr != Find_RenderTarget(strTargetTag))
		return E_FAIL;

	CRenderTarget*		pRenderTarget = CRenderTarget::Create(m_pDevice, m_pContext, iWidth, iHeight, ePixelFormat, vClearColor);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	m_RenderTargets.emplace(strTargetTag, pRenderTarget);

	return S_OK;
}

_int CTarget_Manager::Add_ClientRenderTarget(const _wstring& strMRTTag, const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, _fvector vClearColor)
{
	_int iCount = 0;

	if (nullptr != Find_RenderTarget(strTargetTag))
	{
		CRenderTarget* pRenderTarget = nullptr;
		CRenderTarget* pAlphaRenderTarget = nullptr;
		iCount++;
		for (size_t i = 0; i < 30000; i++)
		{
			if (nullptr != Find_RenderTarget(strTargetTag + to_wstring(iCount)))
			{
				iCount++;
				continue;
			}
			else
			{
				pRenderTarget = CRenderTarget::Create(m_pDevice, m_pContext, iWidth, iHeight, ePixelFormat, vClearColor);
				if (nullptr == pRenderTarget)
					return -1;

				pAlphaRenderTarget = CRenderTarget::Create(m_pDevice, m_pContext, iWidth, iHeight, ePixelFormat, vClearColor);
				if (nullptr == pAlphaRenderTarget)
					return -1;

				m_RenderTargets.emplace(strTargetTag + to_wstring(iCount), pRenderTarget);
				m_RenderTargets.emplace(strTargetTag + to_wstring(iCount) + L"_Alpha", pAlphaRenderTarget);
				break;
			}
			
		}

		list<CRenderTarget*>* pMRTList = Find_MRT(strMRTTag + to_wstring(iCount));

		if (nullptr == pMRTList)
		{
			list<CRenderTarget*>	MRTList;
			MRTList.push_back(pRenderTarget);
			MRTList.push_back(pAlphaRenderTarget);
			m_MRTs.emplace(strMRTTag + to_wstring(iCount), MRTList);
		}
		else
		{
			pMRTList->push_back(pRenderTarget);
			pMRTList->push_back(pAlphaRenderTarget);
		}

		Safe_AddRef(pRenderTarget);
		Safe_AddRef(pAlphaRenderTarget);

		return iCount;
	}

	CRenderTarget* pRenderTarget = CRenderTarget::Create(m_pDevice, m_pContext, iWidth, iHeight, ePixelFormat, vClearColor);
	if (nullptr == pRenderTarget)
		return -1;

	CRenderTarget* pAlphaRenderTarget = CRenderTarget::Create(m_pDevice, m_pContext, iWidth, iHeight, ePixelFormat, vClearColor);
	if (nullptr == pAlphaRenderTarget)
		return -1;

	m_RenderTargets.emplace(strTargetTag, pRenderTarget);
	m_RenderTargets.emplace(strTargetTag + L"_Alpha", pAlphaRenderTarget);

	list<CRenderTarget*>* pMRTList = Find_MRT(strMRTTag);

	if (nullptr == pMRTList)
	{
		list<CRenderTarget*>	MRTList;
		MRTList.push_back(pRenderTarget);
		MRTList.push_back(pAlphaRenderTarget);
		m_MRTs.emplace(strMRTTag, MRTList);
	}
	else
	{
		pMRTList->push_back(pRenderTarget);
		pMRTList->push_back(pAlphaRenderTarget);
	}

	Safe_AddRef(pRenderTarget);
	Safe_AddRef(pAlphaRenderTarget);
	return -1;
}

HRESULT CTarget_Manager::Add_ClientRenderTargetToMRT(const _wstring& strMRTTag, const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, _fvector vClearColor)
{
	if (nullptr != Find_RenderTarget(strTargetTag + L"2"))
		return S_OK;

	CRenderTarget* pRenderTarget2 = CRenderTarget::Create(m_pDevice, m_pContext, iWidth, iHeight, ePixelFormat, vClearColor);
	if (nullptr == pRenderTarget2)
		return E_FAIL;

	m_RenderTargets.emplace(strTargetTag + L"2", pRenderTarget2);

	list<CRenderTarget*>* pMRTList = Find_MRT(strMRTTag);

	pMRTList->push_back(pRenderTarget2);

	Safe_AddRef(pRenderTarget2);
	return S_OK;
}

HRESULT CTarget_Manager::Add_MRT(const _wstring & strMRTTag, const _wstring & strTargetTag)
{
	CRenderTarget*	pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	list<CRenderTarget*>*		pMRTList = Find_MRT(strMRTTag);

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

HRESULT CTarget_Manager::Begin_MRT(const _wstring & strMRTTag, ID3D11DepthStencilView* pDSV)
{
	ID3D11ShaderResourceView*			pSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {
		nullptr
	};

	m_pContext->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, pSRV);

	list<CRenderTarget*>*		pMRTList = Find_MRT(strMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;
	
	m_pContext->OMGetRenderTargets(1, &m_pOldRTV, &m_pOldDSV);
	
	_uint		iNumRTV = { 0 };

	ID3D11RenderTargetView*		RenderTargets[8] = { nullptr };

	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Clear();		

		RenderTargets[iNumRTV++] = pRenderTarget->Get_RTV();
	}

	if(nullptr != pDSV)
		m_pContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	m_pContext->OMSetRenderTargets(iNumRTV, RenderTargets, nullptr == pDSV ? m_pOldDSV : pDSV);

	return S_OK;
}

HRESULT CTarget_Manager::Begin_MRT_DoNotClear(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV)
{
	ID3D11ShaderResourceView* pSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {
		nullptr
	};

	m_pContext->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, pSRV);

	list<CRenderTarget*>* pMRTList = Find_MRT(strMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;

	m_pContext->OMGetRenderTargets(1, &m_pOldRTV, &m_pOldDSV);

	_uint		iNumRTV = { 0 };

	ID3D11RenderTargetView* RenderTargets[8] = { nullptr };

	for (auto& pRenderTarget : *pMRTList)
	{


		RenderTargets[iNumRTV++] = pRenderTarget->Get_RTV();
	}

	if (nullptr != pDSV)
		m_pContext->ClearDepthStencilView(m_pOldDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	m_pContext->OMSetRenderTargets(iNumRTV, RenderTargets, nullptr == pDSV ? m_pOldDSV : pDSV);

	return S_OK;
}

HRESULT CTarget_Manager::Begin_EffectMRT(const _wstring& strMRTTag, _uint iArrayIndex, ID3D11DepthStencilView* pDSV)
{
	ID3D11ShaderResourceView* pSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {
			nullptr
	};

	m_pContext->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, pSRV);

	list<CRenderTarget*>* pMRTList = Find_MRT(strMRTTag + to_wstring(iArrayIndex));
	if (nullptr == pMRTList)
		return E_FAIL;

	m_pContext->OMGetRenderTargets(1, &m_pOldRTV, &m_pOldDSV);

	_uint		iNumRTV = { 0 };

	ID3D11RenderTargetView* RenderTargets[8] = { nullptr };

	for (auto& pRenderTarget : *pMRTList)
	{
		if (m_iBeginEffectCountArray[iArrayIndex] == 0)
			pRenderTarget->Clear();
		
		RenderTargets[iNumRTV++] = pRenderTarget->Get_RTV();
	}

	if (nullptr != pDSV)
		m_pContext->ClearDepthStencilView(m_pOldDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	m_pContext->OMSetRenderTargets(iNumRTV, RenderTargets, nullptr == pDSV ? m_pOldDSV : pDSV);
	m_iBeginEffectCountArray[iArrayIndex]++;
	return S_OK;
}

void CTarget_Manager::Init_ArrayCount()
{
	for (size_t i = 0; i < 10; i++)
		m_iBeginEffectCountArray[i] = 0;
}

_int CTarget_Manager::Get_FrameGlowCount(_int isPri)
{
	_int iCount = { 0 };

	for (size_t i = isPri * 5; i < 5 + isPri * 5; i++)
	{
		if (m_iBeginEffectCountArray[i] > 0)		
			iCount++;
	}
	
	return iCount;
}

_float CTarget_Manager::Get_CulGlowIndex(_int isPri)
{
	_float iCurGlowIndex = { 0.f };
	for (size_t i = isPri * 5; i < 5 + isPri * 5; i++)
	{
		if (m_iBeginEffectCountArray[i] > 0)
		{

			iCurGlowIndex = (_float)i - (isPri * 5);
			break;
		}
	}
	return iCurGlowIndex + 1.f;
}

void CTarget_Manager::Zero_CurGlowIndex(_int isPri)
{
	for (size_t i = isPri * 5; i < 5 + isPri * 5; i++)
	{
		if (m_iBeginEffectCountArray[i] > 0)
		{
			m_iBeginEffectCountArray[i] = 0;
			break;
		}
	}
	
}

HRESULT CTarget_Manager::End_MRT()
{
	m_pContext->OMSetRenderTargets(1, &m_pOldRTV, m_pOldDSV);

	Safe_Release(m_pOldRTV);
	Safe_Release(m_pOldDSV);

	return S_OK;
}

HRESULT CTarget_Manager::Copy_RenderTarget(const _wstring & strTargetTag, ID3D11Texture2D * pTexture2D)
{
	CRenderTarget*		pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Copy_RenderTarget(pTexture2D);	
}

ID3D11ShaderResourceView* CTarget_Manager::Copy_RenderTarget_SRV(const _wstring& strTargetTag)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return nullptr;
	
	return pRenderTarget->Copy_ShaderResourceView();
}

HRESULT CTarget_Manager::Bind_ShaderResource(CShader * pShader, const _char * pConstantName, const _wstring & strTargetTag)
{
	CRenderTarget*		pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Bind_ShaderResource(pShader, pConstantName);	
}

HRESULT CTarget_Manager::Bind_EffectShaderResource(CShader* pShader, const _char* pConstantName, const _wstring& strTargetTag, _uint isPri)
{
	_uint iCurIndex = 0;
	for (size_t i = isPri * 5; i < 5 + isPri * 5; i++)
	{
		if (m_iBeginEffectCountArray[i] > 0)
		{
			iCurIndex = i;
			break;
		}
	}
	/* 끝나면 가장 앞에 있는 0보다 큰 카운트 제거해주기 */
	CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag + to_wstring(iCurIndex));
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Bind_ShaderResource(pShader, pConstantName);
}

#ifdef _DEBUG
HRESULT CTarget_Manager::Ready_Debug(const _wstring & strTargetTag, _float fCenterX, _float fCenterY, _float fSizeX, _float fSizeY)
{
	CRenderTarget*		pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Ready_Debug(fCenterX, fCenterY, fSizeX, fSizeY);	
}

HRESULT CTarget_Manager::Render_Debug(const _wstring & strMRTTag, CShader * pShader, CVIBuffer_Rect * pVIBuffer)
{
	list<CRenderTarget*>*	pMRTList = Find_MRT(strMRTTag);

	for (auto& pRenderTarget : *pMRTList)
		pRenderTarget->Render_Debug(pShader, pVIBuffer);

	return S_OK;
}
#endif

CRenderTarget * CTarget_Manager::Find_RenderTarget(const _wstring & strTargetTag)
{
	auto	iter = m_RenderTargets.find(strTargetTag);

	if (iter == m_RenderTargets.end())
		return nullptr;

	return iter->second;	
}

list<class CRenderTarget*>* CTarget_Manager::Find_MRT(const _wstring & strMRTTag)
{
	auto	iter = m_MRTs.find(strMRTTag);
	if (iter == m_MRTs.end())
		return nullptr;

	return &iter->second;	
}

CTarget_Manager * CTarget_Manager::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTarget_Manager*		pInstance = new CTarget_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CTarget_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CTarget_Manager::Free()
{
	__super::Free();


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

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
