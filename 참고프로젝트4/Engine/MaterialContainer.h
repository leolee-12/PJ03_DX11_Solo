#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL FMaterialData final : public CBase
{
	DERIVED_CLASS(CBase, FMaterialData)

private:
	explicit FMaterialData() = default;
	explicit FMaterialData(const FMaterialData& rhs) = delete;
	virtual ~FMaterialData() = default;

public:
	static FMaterialData* Create();
	virtual void Free() override;

public:
	wstring		strTexture[TEXTURETYPE_MAX] = { TEXT("") };
};



class ENGINE_DLL CMaterialGroup final : public CBase
{
	DERIVED_CLASS(CBase, CMaterialGroup)

private:
	explicit CMaterialGroup() = default;
	explicit CMaterialGroup(const CMaterialGroup& rhs) = delete;
	virtual ~CMaterialGroup() = default;

public:
	static CMaterialGroup* Create();
	virtual void Free() override;

public:
	_uint			Get_NumMaterials() const { return m_iNumMaterials; }
	FMaterialData* Find_Material(const _uint iIndex);
	FMaterialData* Find_Material(const wstring& strName);
	HRESULT Add_Material(const wstring& strName, FMaterialData* pMatData);


private:
	_uint							m_iNumMaterials = { 0 };
	multimap<wstring, FMaterialData*>	mapMaterialDatas;
	vector<FMaterialData*>			vecMaterialDatas;

public:
	GETSET_EX1(vector<FMaterialData*>, vecMaterialDatas, MaterialDatas, GET)
};

END