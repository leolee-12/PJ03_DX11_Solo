#include "Bone.h"

CBone::CBone()
{
}

HRESULT CBone::Initialize(const WMODEL_BONE& tBone)
{
	strcpy_s(m_szName, tBone.szName);
	m_iParentIndex = tBone.iParentIndex;
	m_TransformationMatrix = tBone.transformation;
	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

	return S_OK;
}

void XM_CALLCONV CBone::Update_CombinedTransformMatrices(const vector<class CBone*>& Bones, _fmatrix PreTransformMatrix)
{
	if (-1 == m_iParentIndex)
		XMStoreFloat4x4(&m_CombinedTransformationMatrix,
			PreTransformMatrix * XMLoadFloat4x4(&m_TransformationMatrix));
	else
		XMStoreFloat4x4(&m_CombinedTransformationMatrix,
			XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&Bones[m_iParentIndex]->m_CombinedTransformationMatrix));

}

CBone* CBone::Create(const WMODEL_BONE& tBone)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize(tBone)))
	{
		MSG_BOX("Failed to Created : CBone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBone* CBone::Clone()
{
	return new CBone(*this);
}

void CBone::Free()
{
	__super::Free();
}
