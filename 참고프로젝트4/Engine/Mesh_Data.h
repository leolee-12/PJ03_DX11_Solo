#pragma once
#include "Base.h"
#include "Bone_Data.h"

BEGIN(Engine)

struct ENGINE_DLL Mesh_Data
{
	string				szName = "";
	_uint				iMaterialIndex;
	_uint				iNumVertices;
	_uint				iNumFaces;
	_uint				iNumBones;
	_uint				iNumOffsetMatrices;
	MODELTYPE			eModelType = { MODELTYPE::NONANIM };

	vector<VTXMESH>			pVertices;
	vector<VTXMESHANIM>		pAnimVertices;
	vector<_uint>			pIndices;

	vector<_uint>		BoneIndices;
	vector<_float4x4>	OffsetMatrices;

#ifdef _DEBUG
	HRESULT				Bake_MeshData(const aiMesh* pAIMesh, MODELTYPE eType, _fmatrix PivotMatrix, const vector<BONE_DATA> Bones);
	HRESULT				Ready_Vertices_NonAnim(const aiMesh* pAIMesh, _fmatrix PivotMatrix);
	HRESULT				Ready_Vertices_Anim(const aiMesh* pAIMesh, const vector<BONE_DATA> Bones);
#endif

	HRESULT				Bake_Binary(ofstream& fout);
	HRESULT				Load_Binary(ifstream& fin);

}typedef MESH_DATA;

END