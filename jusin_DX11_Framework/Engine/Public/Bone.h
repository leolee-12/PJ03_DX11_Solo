#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CBone final : public CBase
{
private:
	CBone();
	virtual ~CBone() = default;

public:
	_bool Compare_Name(const _char* pBoneName) { return !strcmp(pBoneName, m_szName); }
	const _float4x4* Get_CombinedTransformationMatrixPtr() const { return &m_CombinedTransformationMatrix; }
	void XM_CALLCONV Set_TransformationMatrix(_fmatrix TransformationMatrix) { XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix); }

	HRESULT Initialize(const aiNode* pAINode, _int iParentIndex);
	void XM_CALLCONV Update_CombinedTransformMatrices(const vector<class CBone*>& Bones, _fmatrix PreTransformMatrix);

private:
	_char m_szName[MAX_PATH] = {};
	_int m_iParentIndex = { -1 };
	_float4x4 m_TransformationMatrix = {};
	_float4x4 m_CombinedTransformationMatrix = {};

public:
	static CBone* Create(const aiNode* pAINode, _int iParentIndex);
	CBone* Clone();
private:
	virtual void Free() override;
};

NS_END