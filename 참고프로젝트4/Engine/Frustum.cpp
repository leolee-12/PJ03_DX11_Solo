#include "Frustum.h"

#include "GameInstance.h"
#ifdef _DEBUG
#include "DebugDraw.h"
#endif

CFrustum::CFrustum()
	: m_pGI(GI())
{
	Safe_AddRef(m_pGI);
}

HRESULT CFrustum::Initialize()
{
#ifdef _DEBUG
#if FRUSTUM_DEBUG
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(GI()->Get_D3D11Context().Get());
	m_pEffect = new BasicEffect(GI()->Get_D3D11Device().Get());
	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCode = { nullptr };
	size_t	iShaderCodeLength = { 0 };

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderCodeLength);

	if (FAILED(GI()->Get_D3D11Device()
		->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iShaderCodeLength, m_pInputLayout.GetAddressOf())))
		return E_FAIL;
#endif
#endif

	return S_OK;
}

void CFrustum::Tick()
{
	// Frustum의 원형을 기반으로 월드위치까지 돌려버린다.
	_matrix		ProjMatrix = m_pGI->Get_TransformMatrix(CPipeLine::TS_PROJ);
	_matrix		ViewMatrixInv = m_pGI->Get_TransformMatrixInverse(CPipeLine::TS_VIEW);
	
	m_OriginFrustum.CreateFromMatrix(m_ViewFrustum, ProjMatrix);
	m_ViewFrustum.Transform(m_WorldFrustum, ViewMatrixInv);

#if FRUSTUM_DEBUG
	m_pGI->Add_DebugEvent(MakeDelegate(this, &CFrustum::Render_Debug));
#endif
}


#ifdef _DEBUG

HRESULT CFrustum::Render_Debug()
{
#if FRUSTUM_DEBUG
	if (nullptr == m_pBatch || nullptr == m_pEffect || nullptr == m_pInputLayout)
		return E_FAIL;

	m_pBatch->Begin();

	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView(GI()->Get_TransformMatrix(CPipeLine::TS_VIEW));
	m_pEffect->SetProjection(GI()->Get_TransformMatrix(CPipeLine::TS_PROJ));

	GI()->Get_D3D11Context()->IASetInputLayout(m_pInputLayout.Get());

	m_pEffect->Apply(GI()->Get_D3D11Context().Get());

	DX::Draw(m_pBatch, m_WorldFrustum, Colors::Blue);

	m_pBatch->End();
#endif

	return S_OK;
}
#endif // _DEBUG

CFrustum* CFrustum::Create()
{
	ThisClass* pInstance = new ThisClass;

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CFrustum");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFrustum::Free()
{
	Safe_Release(m_pGI);

#ifdef _DEBUG
	Safe_Delete(m_pBatch);
	Safe_Delete(m_pEffect);
#endif
}

void CFrustum::Transform_ToLocalSpace(_fmatrix WorldMatrix)
{
	/*_matrix		WorldMatrixTranspose = XMMatrixTranspose(WorldMatrix);

	for (size_t i = 0; i < 6; i++)
	{
		XMStoreFloat4(&m_LocalPlanes[i], XMPlaneTransform(XMLoadFloat4(&m_WorldPlanes[i]), WorldMatrixTranspose));
	}*/
}
