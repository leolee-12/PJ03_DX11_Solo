#include "PipeLine.h"

CPipeLine::CPipeLine()
{
}

const _float4x4* CPipeLine::Get_Transform_Float4x4_Ptr(D3DTS eState)
{
	return &m_TransformationMatrix[ENUM_CLASS(eState)];	
}

_matrix CPipeLine::Get_Transform_Matrix(D3DTS eState)
{
	return XMLoadFloat4x4(&m_TransformationMatrix[ENUM_CLASS(eState)]);
}

const _float4x4* CPipeLine::Get_Transform_Float4x4_Inverse_Ptr(D3DTS eState)
{
	return &m_TransformationMatrix_Inverse[ENUM_CLASS(eState)];
}

_matrix CPipeLine::Get_Transform_Matrix_Inverse(D3DTS eState)
{
	return XMLoadFloat4x4(&m_TransformationMatrix_Inverse[ENUM_CLASS(eState)]);
}

const _float4* CPipeLine::Get_CamPosition()
{
	return &m_vCamPosition;
}

void CPipeLine::Set_Transform(D3DTS eState, _fmatrix TransformMatrix)
{
	XMStoreFloat4x4(&m_TransformationMatrix[ENUM_CLASS(eState)], TransformMatrix);
}

HRESULT CPipeLine::Initialize()
{
	for (size_t i = 0; i < ENUM_CLASS(D3DTS::END); i++)
	{
		XMStoreFloat4x4(&m_TransformationMatrix[ENUM_CLASS(i)], XMMatrixIdentity());
		XMStoreFloat4x4(&m_TransformationMatrix_Inverse[ENUM_CLASS(i)], XMMatrixIdentity());
	}

	return S_OK;
}

void CPipeLine::Update()
{
	for (size_t i = 0; i < ENUM_CLASS(D3DTS::END); i++)
	{		
		XMStoreFloat4x4(&m_TransformationMatrix_Inverse[ENUM_CLASS(i)], XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformationMatrix[ENUM_CLASS(i)])));
	}

	memcpy(&m_vCamPosition, &m_TransformationMatrix_Inverse[ENUM_CLASS(D3DTS::VIEW)].m[3], sizeof(_float4));

}

CPipeLine* CPipeLine::Create()
{
	CPipeLine* pInstance = new CPipeLine();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CPipeLine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPipeLine::Free()
{
	__super::Free();
}
