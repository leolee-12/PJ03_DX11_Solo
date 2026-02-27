#include "..\Public\PipeLine.h"



CPipeLine::CPipeLine()
{
}

void CPipeLine::Update()
{
	for (size_t i = 0; i < D3DTS_END; i++)	
		XMStoreFloat4x4(&m_TransformMatrices_Inverse[i], XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformMatrices[i])));	

	XMStoreFloat4(&m_vCamPosition, XMLoadFloat4x4(&m_TransformMatrices_Inverse[D3DTS_VIEW]).r[3]);

	for (size_t i = 0; i < D3DTS_END; i++)
		XMStoreFloat4x4(&m_ShadowCamTransformMatrices_Inverse[i], XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_ShadowCamTransformMatrices[i])));

	XMStoreFloat4(&m_vShadowCamPosition, XMLoadFloat4x4(&m_ShadowCamTransformMatrices_Inverse[D3DTS_VIEW]).r[3]);
}

CPipeLine * CPipeLine::Create()
{
	return new CPipeLine;
}

void CPipeLine::Free()
{
	__super::Free();

}
