#pragma once

/* 그림자용 광원의 설정을 위한 뷰, 투영행렬을 들고있는다. */
#include "Base.h"

NS_BEGIN(Engine)

class CShadow final : public CBase
{


private:
	CShadow();
	virtual ~CShadow() = default;

public:
	HRESULT Add_Shadow_Light(const SHADOW_DESC& ShadowDesc);

	const _float4x4* Get_Shadow_Transform_Float4x4_Ptr(D3DTS eState) {
		return &m_TransformationMatrices[ENUM_CLASS(eState)];
	}

private:
	_float4x4				m_TransformationMatrices[ENUM_CLASS(D3DTS::END)] = {};

public:
	static CShadow* Create();
	virtual void Free() override;

};

NS_END