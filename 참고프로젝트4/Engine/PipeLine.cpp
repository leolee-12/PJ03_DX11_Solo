#include "PipeLine.h"

#include "GameInstance.h"

CPipeLine::CPipeLine()
{
}

void CPipeLine::Set_Transform(TRANSFORMSTATE eState, _fmatrix TransformMatrix)
{
	XMStoreFloat4x4(&m_Transform[eState], TransformMatrix);
}

void CPipeLine::Set_Transform(TRANSFORMSTATE eState, _float4x4 TransformMatrix)
{
	m_Transform[eState] = TransformMatrix;
}

_matrix CPipeLine::Get_TransformMatrix(TRANSFORMSTATE eState)
{
	return XMLoadFloat4x4(&m_Transform[eState]);
}

_float4x4 CPipeLine::Get_TransformFloat4x4(TRANSFORMSTATE eState)
{
	return m_Transform[eState];
}

_matrix CPipeLine::Get_TransformMatrixInverse(TRANSFORMSTATE eState)
{
	return XMLoadFloat4x4(&m_Transform_Inverse[eState]);
}

_float4x4 CPipeLine::Get_TransformFloat4x4Inverse(TRANSFORMSTATE eState)
{
	return m_Transform_Inverse[eState];
}

_float4 CPipeLine::Get_CamPosition()
{
	return m_vCamPosition;
}

HRESULT CPipeLine::Initialize()
{
	for (_uint i = 0; i < (_uint)TS_END; i++)
	{
		XMStoreFloat4x4(&m_Transform[i], XMMatrixIdentity());
		XMStoreFloat4x4(&m_Transform_Inverse[i], XMMatrixIdentity());
	}

	return S_OK;
}

void CPipeLine::Tick()
{
	for (_uint i = 0; i < (_uint)TS_END; i++)
	{
		XMStoreFloat4x4(&m_Transform_Inverse[i], XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_Transform[i])));
	}
	
	// 사운드 매니저에 청자 위치를 바꿔주는 함수
	GI()->Set_ListenerAttributeByMatrix(m_Transform_Inverse[TS_VIEW]);

	/*뷰 역행렬의 3행 == 카메라 위치*/
	//memcpy(&m_vCamPosition, &m_Transform_Inverse[TS_VIEW].m[3], sizeof(_float4));
}


CPipeLine* CPipeLine::Create()
{
	CPipeLine* pInstance = new CPipeLine;

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CPipeLine");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPipeLine::Free()
{
}