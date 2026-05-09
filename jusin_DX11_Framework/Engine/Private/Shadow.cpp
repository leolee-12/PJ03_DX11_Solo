#include "Shadow.h"
#include "GameInstance.h"

CShadow::CShadow()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CShadow::Set_ShadowLight(const SHADOW_LIGHT_DESC& ShadowDesc)
{
	m_fFarZ = ShadowDesc.fFar;

	XMStoreFloat4x4(&m_TransformStateMatrices[ETOUI(D3DTS::VIEW)],
		XMMatrixLookAtLH(XMLoadFloat4(&ShadowDesc.vEye), XMLoadFloat4(&ShadowDesc.vAt), XMVectorSet(0.f, 1.f, 0.f, 0.f)));

	XMStoreFloat4x4(&m_TransformStateMatrices[ETOUI(D3DTS::PROJ)],
		XMMatrixOrthographicLH(200.f, 200.f, ShadowDesc.fNear, ShadowDesc.fFar));

	return S_OK;
}

HRESULT CShadow::Bind_FarZ(CShader* pShaderCom)
{
	return pShaderCom->Bind_RawValue("g_fShadowFarZ", &m_fFarZ, sizeof(m_fFarZ));
}

CShadow* CShadow::Create()
{
	return new CShadow();
}

void CShadow::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
