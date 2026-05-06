#include "Renderer.h"
#include "GameObject.h"
#include "GameInstance.h"

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CRenderer::Initialize()
{
	if(FAILED(Resize()))
		return E_FAIL;

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_Deferred.hlsl"), VTXTEX::Elements, VTXTEX::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	return S_OK;
}

void CRenderer::Add_RenderGroup(RENDERID eGroupID, CGameObject* pGameObject)
{
	m_RenderObjects[ETOUI(eGroupID)].push_back(pGameObject);

	Safe_AddRef(pGameObject);
}

HRESULT CRenderer::Draw()
{
	if (FAILED(Render_Priority()))
		return E_FAIL;

	if (FAILED(Render_NonBlend()))
		return E_FAIL;

	if (FAILED(Render_Lights()))
		return E_FAIL;

	if (FAILED(Render_Combined()))
		return E_FAIL;

	if (FAILED(Render_NonLight()))
		return E_FAIL;

	if (FAILED(Render_Blend()))
		return E_FAIL;

	if (FAILED(Render_UI()))
		return E_FAIL;

#ifdef _DEBUG
	if (FAILED(Render_Debug()))
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CRenderer::Resize()
{
	_float2 vViewportDesc = m_pGameInstance->Get_ViewportSize();
	_uint iNewWidth = static_cast<_uint>(vViewportDesc.x);
	_uint iNewHeight = static_cast<_uint>(vViewportDesc.y);

	// RT »ý¼º
	if (FAILED(m_pGameInstance->Add_RenderTarget(TARGET_DIFFUSE, iNewWidth, iNewHeight,
		DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TARGET_NORMAL, iNewWidth, iNewHeight,
		DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TARGET_SHADE, iNewWidth, iNewHeight,
		DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TARGET_DEPTH, iNewWidth, iNewHeight,
		DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TARGET_SPECULAR, iNewWidth, iNewHeight,
		DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TARGET_PICKPOS, iNewWidth, iNewHeight,
		DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	// MRT·Î ¹­±â
	if (FAILED(m_pGameInstance->Add_MRT(MRT_GAMEOBJECTS, TARGET_DIFFUSE)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(MRT_GAMEOBJECTS, TARGET_NORMAL)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(MRT_GAMEOBJECTS, TARGET_DEPTH)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(MRT_GAMEOBJECTS, TARGET_PICKPOS)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(MRT_LIGHTACC, TARGET_SHADE)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(MRT_LIGHTACC, TARGET_SPECULAR)))
		return E_FAIL;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(vViewportDesc.x, vViewportDesc.y, 1.f));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(vViewportDesc.x, vViewportDesc.y, 0.f, 1.f));

#ifdef _DEBUG
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TARGET_DIFFUSE, 150.f, 150.f, 300.f, 300.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TARGET_NORMAL, 150.f, 450.f, 300.f, 300.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TARGET_SHADE, 450.f, 150.f, 300.f, 300.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TARGET_SPECULAR, 450.f, 450.f, 300.f, 300.f)))
		return E_FAIL;
#endif

	return S_OK;
}

#ifdef _DEBUG
void CRenderer::Add_DebugComponent(CComponent* pComponent)
{
	m_DebugComponents.push_back(pComponent);
	Safe_AddRef(pComponent);
}
#endif

HRESULT CRenderer::Render_Priority()
{
	for (auto& pRenderObject : m_RenderObjects[ETOUI(RENDERID::PRIORITY)])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[ETOUI(RENDERID::PRIORITY)].clear();

	return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
	if (FAILED(m_pGameInstance->Begin_MRT(MRT_GAMEOBJECTS)))
		return E_FAIL;

	for (auto& pRenderObject : m_RenderObjects[ETOUI(RENDERID::NONBLEND)])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[ETOUI(RENDERID::NONBLEND)].clear();

	ID3D11ShaderResourceView* nullSRV[] = {nullptr, nullptr};

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Lights()
{
	if (FAILED(m_pGameInstance->Begin_MRT(MRT_LIGHTACC)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TARGET_NORMAL, m_pShader, "g_TexNorm")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TARGET_DEPTH, m_pShader, "g_TexDepth")))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewInvMatrix", m_pGameInstance->Get_Transform_Inverse(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjInvMatrix", m_pGameInstance->Get_Transform_Inverse(D3DTS::PROJ))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_vCamPos", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_fFarZ", m_pGameInstance->Get_FarZPtr(), sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_Light(m_pShader, m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Combined()
{
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TARGET_DIFFUSE, m_pShader, "g_TexDiff")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TARGET_SHADE, m_pShader, "g_TexShade")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TARGET_SPECULAR, m_pShader, "g_TexSpec")))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(ETOUI(DEFERRED::COMBINED))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
	for (auto& pRenderObject : m_RenderObjects[ETOUI(RENDERID::NONLIGHT)])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[ETOUI(RENDERID::NONLIGHT)].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
	for (auto& pRenderObject : m_RenderObjects[ETOUI(RENDERID::BLEND)])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[ETOUI(RENDERID::BLEND)].clear();

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	auto& UIList = m_RenderObjects[ETOUI(RENDERID::UI)];

	UIList.sort([](CGameObject* pL, CGameObject* pR)
		{
			return pL->Get_RenderOrder() < pR->Get_RenderOrder();
		});

	for (auto& pRenderObject : UIList)
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	UIList.clear();
	return S_OK;
}

#ifdef _DEBUG

HRESULT CRenderer::Render_Debug()
{
	for (auto& pDebugCom : m_DebugComponents)
	{
		pDebugCom->Render();
		Safe_Release(pDebugCom);
	}

	m_DebugComponents.clear();

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Resources()))
		return E_FAIL;


	if (m_pGameInstance->Is_Debug())
	{
		m_pGameInstance->Render_RT_Debug(MRT_GAMEOBJECTS, m_pShader, m_pVIBuffer);
		m_pGameInstance->Render_RT_Debug(MRT_LIGHTACC, m_pShader, m_pVIBuffer);
	}
	return S_OK;
}

#endif

CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderer* pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CRenderer");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CRenderer::Free()
{
	__super::Free();

	for (auto& RenderObjects : m_RenderObjects)
	{
		for (auto& pRenderObject : RenderObjects)
			Safe_Release(pRenderObject);

		RenderObjects.clear();
	}

	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
