#pragma once
#include "Base.h"

BEGIN(Engine)

struct  ENGINE_DLL Material_Data
{
	string szTextureFullPath[TEXTURETYPE_MAX];

	HRESULT Bake_Binary(ofstream& fout);
	HRESULT	Load_Binary(ifstream& fin);

}typedef MATERIAL_DATA;



END