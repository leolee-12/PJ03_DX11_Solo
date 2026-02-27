//#include "Bone.h"
//
//CBone::CBone()
//{
//}
//
//#ifdef _DEBUG
//HRESULT CBone::Initialize(aiNode* pAINode, _int iParentIndex)
//{
//	m_iParentIndex = iParentIndex;
//
//	strcpy_s(m_szName, pAINode->mName.data);
//
//	memcpy(&m_TransformationMatrix, &pAINode->mTransformation, sizeof(_float4x4));
//
//	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));
//
//	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());
//
//	return S_OK;
//}
//#endif
//
//HRESULT CBone::Initialize(BONE_DATA& BoneData)
//{
//	m_iParentIndex = BoneData.iParentIndex;
//	strcpy_s(m_szName, BoneData.szName.c_str());
//	memcpy(&m_TransformationMatrix, &BoneData.TransformationMatrix, sizeof(_float4x4));
//
//	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());
//	return S_OK;
//}
//
//void CBone::Invalidate_CombinedTransformationMatrix(CModel::BONES& Bones , _fmatrix PivotMatrix)
//{
//	if (-1 == m_iParentIndex)
//		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * PivotMatrix);
//	else
//	{
//		XMStoreFloat4x4(&m_CombinedTransformationMatrix,
//			XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&Bones[m_iParentIndex]->m_CombinedTransformationMatrix));
//	}
//
//}
//
//#ifdef _DEBUG
//CBone* CBone::Create(aiNode* pAINode, _int iParentIndex)
//{
//	CBone* pInstance = new CBone();
//
//	if (FAILED(pInstance->Initialize(pAINode, iParentIndex)))
//	{
//		MSG_BOX("Failed to Created : CBone");
//		Safe_Release(pInstance);
//	}
//	return pInstance;
//}
//#endif
//
//CBone* CBone::Create(BONE_DATA& BoneData)
//{
//	CBone* pInstance = new CBone();
//
//	if (FAILED(pInstance->Initialize(BoneData)))
//	{
//		MSG_BOX("Failed to Created : CBone");
//		Safe_Release(pInstance);
//	}
//	return pInstance;
//}
//
//CBone* CBone::Clone()
//{
//	return new CBone(*this);
//}
//
//void CBone::Free()
//{
//}
