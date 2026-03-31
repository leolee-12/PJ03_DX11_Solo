#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CBone final : public CBase
{
private:
	CBone();
	virtual ~CBone() = default;

public:
	HRESULT Initialize(const aiNode* pAINode, _int iParentIndex);

private:
	_char m_szName[MAX_PATH] = {};
	_int m_iParentIndex = { -1 };
	_float4x4 m_TransformationMatrix = {};
	_float4x4 m_CombinedTransformationMatrix = {};

public:
	static CBone* Create(const aiNode* pAINode, _int iParentIndex);

private:
	virtual void Free() override;
};

NS_END