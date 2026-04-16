#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CBounding abstract : public CBase
{
public:
	struct BOUNDING_DESC
	{
		_float3 vCenter;
	};


protected:
	CBounding(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding() = default;

public:
	virtual HRESULT Initialize(const CBounding::BOUNDING_DESC* pBoundingDesc);

protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };


protected:
	virtual void Free() override;
};

NS_END