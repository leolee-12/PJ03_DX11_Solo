#include "Bone.h"

CBone::CBone()
{
}

HRESULT CBone::Initialize(const aiNode* pAINode, _int iParentIndex)
{
    m_iParentBoneIndex = iParentIndex;

    strcpy_s(m_szName, pAINode->mName.data);

    memcpy(&m_TransformationMatrix, &pAINode->mTransformation, sizeof(_float4x4));

    XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));
    
    XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());
    
	return S_OK;
}

void CBone::Update_CombinedTransformationMatrix(const vector<CBone*>& Bones, _fmatrix PreTransformationMatrix)
{
    if (-1 == m_iParentBoneIndex)
    {
        XMStoreFloat4x4(&m_CombinedTransformationMatrix, 
            XMLoadFloat4x4(&m_TransformationMatrix) * PreTransformationMatrix);
    }
    else
    {
        XMStoreFloat4x4(&m_CombinedTransformationMatrix, 
            XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&Bones[m_iParentBoneIndex]->m_CombinedTransformationMatrix));
    }    
}

CBone* CBone::Create(const aiNode* pAINode, _int iParentIndex)
{
    CBone* pInstance = new CBone();

    if (FAILED(pInstance->Initialize(pAINode, iParentIndex)))
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
