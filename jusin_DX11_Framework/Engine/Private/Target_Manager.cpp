#include "Target_Manager.h"
#include "RenderTarget.h"

CTarget_Manager::CTarget_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CTarget_Manager::Add_RenderTarget(WNameID strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
	if (nullptr != Find_RenderTarget(strTargetTag))
		return E_FAIL;

	CRenderTarget* pRenderTarget = CRenderTarget::Create(m_pDevice, m_pContext, iWidth, iHeight, ePixelFormat, vClearColor);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	m_RenderTargets.emplace(strTargetTag, pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Add_MRT(WNameID strMRTTag, WNameID strTargetTag)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	MRT* pVecMRT = Find_MRT(strMRTTag);

	if (nullptr == pVecMRT)
	{
		MRT vecMRT;
		vecMRT.reserve(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT);
		vecMRT.push_back(pRenderTarget);
		m_MRTs.emplace(strMRTTag, move(vecMRT));
	}
	else
	{
		if(pVecMRT->size() >= D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT)
			return E_FAIL;

		pVecMRT->push_back(pRenderTarget);
	}

	Safe_AddRef(pRenderTarget);
	return S_OK;
}

HRESULT CTarget_Manager::Begin_MRT(WNameID strMRTTag)
{
	auto pMRTList = Find_MRT(strMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;

	m_pContext->OMGetRenderTargets(1, &m_pBackBufferRTV, &m_pOriginalDSV);

	ID3D11RenderTargetView* pRenderTargets[8] = { nullptr };

	_uint iNumRenderTargets = {};

	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Clear();
		pRenderTargets[iNumRenderTargets++] = pRenderTarget->Get_RTV();
	}

	ID3D11ShaderResourceView* nullSRV[8] = {};
	m_pContext->PSSetShaderResources(0, 8, nullSRV);

	m_pContext->OMSetRenderTargets(iNumRenderTargets, pRenderTargets, m_pOriginalDSV);

	return S_OK;
}

HRESULT CTarget_Manager::End_MRT()
{
	m_pContext->OMSetRenderTargets(1, &m_pBackBufferRTV, m_pOriginalDSV);

	Safe_Release(m_pBackBufferRTV);
	Safe_Release(m_pOriginalDSV);

	return S_OK;
}

HRESULT CTarget_Manager::Bind_ShaderResource(WNameID strTargetTag, CShader* pShader, const _char* pConstName)
{
	auto pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Bind_ShaderResource(pShader, pConstName);
}

HRESULT CTarget_Manager::Copy_Resource(WNameID strTargetTag, ID3D11Texture2D* pOut)
{
	auto		pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Copy_Resource(pOut);
}

HRESULT CTarget_Manager::Copy_SubResource(WNameID strTargetTag, ID3D11Texture2D* pOut, const D3D11_BOX* pSrcBox)
{
	auto pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;
	return pRenderTarget->Copy_SubResource(pOut, pSrcBox);
}

void CTarget_Manager::Reset()
{
	m_MRTs.for_each([](auto& Pair)
		{
			for (auto& pRenderTarget : Pair.second)
				Safe_Release(pRenderTarget);
			Pair.second.clear();
		});
	m_MRTs.clear();

	m_RenderTargets.for_each([](auto& Pair)
		{
			Safe_Release(Pair.second);
		});
	m_RenderTargets.clear();
}

#ifdef _DEBUG

HRESULT CTarget_Manager::Ready_Debug(WNameID strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	auto pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Ready_Debug(fX, fY, fSizeX, fSizeY);
}

HRESULT CTarget_Manager::Render_Debug(WNameID strMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	auto pMRTList = Find_MRT(strMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;

	for (auto& pRenderTarget : *pMRTList)
		pRenderTarget->Render_Debug(pShader, pVIBuffer);

	return S_OK;
}

#endif

CRenderTarget* CTarget_Manager::Find_RenderTarget(WNameID strTargetTag)
{
	auto pp = m_RenderTargets.find(strTargetTag);
	return pp ? *pp : nullptr;
}

CTarget_Manager::MRT* CTarget_Manager::Find_MRT(WNameID strMRTTag)
{
	return m_MRTs.find(strMRTTag);
}

CTarget_Manager* CTarget_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return new CTarget_Manager(pDevice, pContext);
}

void CTarget_Manager::Free()
{
	__super::Free();

	Reset();

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
