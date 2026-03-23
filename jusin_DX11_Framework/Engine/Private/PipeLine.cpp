#include "PipeLine.h"

CPipeLine::CPipeLine()
{
	for (size_t i = 0; i < ETOUI(D3DTS::END); ++i)
	{
		XMStoreFloat4x4(&m_TransformStateMatrices[i], XMMatrixIdentity());
		XMStoreFloat4x4(&m_TransformStateInverseMatrices[i], XMMatrixIdentity());
	}
}

CPipeLine* CPipeLine::Create()
{
	return new CPipeLine();
}

void CPipeLine::Free()
{
	__super::Free();
}
