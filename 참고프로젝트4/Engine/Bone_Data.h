#pragma once
#include "Base.h"

BEGIN(Engine)


struct ENGINE_DLL Bone_Data
{
	string	szName = "";
	_int	iParentIndex = { 0 };
	_float4x4	TransformationMatrix;


#ifdef _DEBUG
	HRESULT		Bake_BoneData(aiNode* pAINode, _int iParent);
#endif

	HRESULT		Bake_Binary(ofstream& fout);
	HRESULT		Load_Binary(ifstream& fin);
}typedef BONE_DATA;



END