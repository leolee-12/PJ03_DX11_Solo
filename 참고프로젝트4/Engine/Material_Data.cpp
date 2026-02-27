#include "Material_Data.h"

HRESULT Material_Data::Bake_Binary(ofstream& fout)
{
	for (_uint i = 0; i < (_uint)18; i++)
	{
		size_t strSize = szTextureFullPath[i].size();
		fout.write(reinterpret_cast<char*>(&strSize), sizeof(strSize));
		fout.write(reinterpret_cast<char*>(&szTextureFullPath[i][0]), (strSize));
	}
	return S_OK;
}

HRESULT Material_Data::Load_Binary(ifstream& fin)
{
	for (_uint i = 0; i < (_uint)18; i++)
	{
		size_t strSize = {};
		fin.read(reinterpret_cast<char*>(&strSize), sizeof(strSize));
		szTextureFullPath[i].resize(strSize);
		fin.read(reinterpret_cast<char*>(&szTextureFullPath[i][0]), (strSize));
	}

	return S_OK;
}
