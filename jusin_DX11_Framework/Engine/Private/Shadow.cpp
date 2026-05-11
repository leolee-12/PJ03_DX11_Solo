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
		XMMatrixOrthographicLH(40.f, 40.f, ShadowDesc.fNear, ShadowDesc.fFar));

	// Follow 준비 : 광원 방향+거리 보관, 매 프레임 갱신 시작
	_float4 vDir;
	XMStoreFloat4(&vDir, XMVectorSubtract(XMLoadFloat4(&ShadowDesc.vEye), XMLoadFloat4(&ShadowDesc.vAt)));
	m_vEyeOffset = _float3{ vDir.x, vDir.y, vDir.z };
	m_bFollow = true;

	return S_OK;
}

HRESULT CShadow::Bind_FarZ(CShader* pShaderCom)
{
	return pShaderCom->Bind_RawValue("g_fShadowFarZ", &m_fFarZ, sizeof(m_fFarZ));
}

void CShadow::Update()
{
	if (!m_bFollow) return;

	// 카메라 월드 위치 Follow
	const _float4* pCamPos = m_pGameInstance->Get_CamPosition();
	_vector vAt = XMVectorSet(pCamPos->x, pCamPos->y, pCamPos->z, 1.f);
	_vector vEye = XMVectorAdd(vAt, XMVectorSet(m_vEyeOffset.x, m_vEyeOffset.y, m_vEyeOffset.z, 0.f));

	XMStoreFloat4x4(&m_TransformStateMatrices[ETOUI(D3DTS::VIEW)],
		XMMatrixLookAtLH(vEye, vAt, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
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
