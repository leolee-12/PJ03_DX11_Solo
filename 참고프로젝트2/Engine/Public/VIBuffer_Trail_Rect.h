#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Trail_Rect final : public CVIBuffer
{
private:
	CVIBuffer_Trail_Rect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Trail_Rect(const CVIBuffer_Trail_Rect& Prototype);
	virtual ~CVIBuffer_Trail_Rect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Prototype_Trail(_uint iNumRect ,_float fRectWidth);
	virtual HRESULT Initialize(void* pArg) override;

	void Line(_float3 vOffPos);

private:
	_uint m_iNumRect = { 0 };

	vector<VTXPOSTEX>trailVertices = {};

public:
	static CVIBuffer_Trail_Rect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	static CVIBuffer_Trail_Rect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNummRect, _float fRectWidth);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END