#include "Bone_Data.h"

#ifdef _DEBUG
HRESULT Bone_Data::Bake_BoneData(aiNode* pAINode, _int iParent)
{
	iParentIndex = iParent;

	szName = pAINode->mName.data;

	memcpy(&TransformationMatrix, &pAINode->mTransformation, sizeof(_float4x4));

	XMStoreFloat4x4(&TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&TransformationMatrix)));

	return S_OK;
}
#endif

HRESULT Bone_Data::Bake_Binary(ofstream& fout)
{
	size_t strSize = szName.size();
	fout.write(reinterpret_cast<char*>(&strSize), sizeof(strSize));
	fout.write(reinterpret_cast<char*>(&szName[0]), (strSize));

	fout.write(reinterpret_cast<char*>(&iParentIndex), sizeof(iParentIndex));
	fout.write(reinterpret_cast<char*>(&TransformationMatrix), sizeof(TransformationMatrix));

	return S_OK;
}

HRESULT Bone_Data::Load_Binary(ifstream& fin)
{
	size_t strSize = {};
	fin.read(reinterpret_cast<char*>(&strSize), sizeof(strSize));
	szName.resize(strSize);
	fin.read(reinterpret_cast<char*>(&szName[0]), (strSize));

	fin.read(reinterpret_cast<char*>(&iParentIndex), sizeof(iParentIndex));
	fin.read(reinterpret_cast<char*>(&TransformationMatrix), sizeof(TransformationMatrix));

	return S_OK;
}
