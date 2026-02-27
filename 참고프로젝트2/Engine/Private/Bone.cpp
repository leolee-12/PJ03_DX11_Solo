#include "..\Public\Bone.h"
#include <fstream>
#include <iostream>

CBone::CBone()
{

}

HRESULT CBone::Initialize(const string& name, _int parentIndex, const XMFLOAT4X4& transformationMatrix)
{
	// m_szName 배열에 name을 복사
	if (name.length() >= MAX_PATH) {
		OutputDebugStringA("ERROR:: Bone name length exceeds buffer size\n");
		return E_FAIL;
	}
	strcpy_s(m_szName, name.c_str());

	m_iParentIndex = parentIndex;

	// Transformation matrix 복사 및 전치
	m_TransformationMatrix = transformationMatrix;
	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));
	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

	return S_OK;
}

void CBone::Update_CombinedTransformationMatrix(const vector<CBone*>& Bones, _fmatrix PreTransformMatrix)
{
	/* 1. 특정 애니메이션을 내가 재생했다면. == 이 애니메이션이 구동하는 뼈의 TransformationMatrix가 갱신됐다. */
	/* 2. 모든 뼈들을 순회하면서 갱신된 특정 뼈의 TransformationMatrix를 포함한 CombinedTransformation을 갱신해준다. */
	// m_CombinedTransformationMatrix = m_TransformationMatrix * 부모`s CombinedTransformationMatrix;

	if (-1 == m_iParentIndex)
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, PreTransformMatrix * XMLoadFloat4x4(&m_TransformationMatrix));

	else
		XMStoreFloat4x4(&m_CombinedTransformationMatrix,
			XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&Bones[m_iParentIndex]->m_CombinedTransformationMatrix));	
}



CBone* CBone::Create(const string& name, _int parentIndex, const XMFLOAT4X4& transformationMatrix)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize(name, parentIndex, transformationMatrix)))
	{
		MSG_BOX(TEXT("Failed to Created : CBone"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBone * CBone::Clone()
{
	return new CBone(*this);
}

void CBone::Free()
{
	__super::Free();

}

