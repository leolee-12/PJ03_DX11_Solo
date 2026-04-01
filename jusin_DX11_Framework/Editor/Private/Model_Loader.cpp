#include "Model_Loader.h"
using namespace Assimp;

HRESULT XM_CALLCONV CModel_Loader::Export_Binary(const _char* pFbxPath, const _char* pOutputPath, MODEL eType, _fmatrix PreTransform)
{
	return S_OK;
}

HRESULT XM_CALLCONV CModel_Loader::Export_JSON(const _char* pFbxPath, const _char* pOutputPath, MODEL eType, _fmatrix PreTransform, _uint iVertexSampleCount)
{
	return S_OK;
}

HRESULT XM_CALLCONV CModel_Loader::Export_All(const _char* pFbxPath, const _char* pOutputDir, MODEL eType, _fmatrix PreTransform)
{
	return S_OK;
}
