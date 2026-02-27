#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Line_Rect final : public CVIBuffer
{
private:
	CVIBuffer_Line_Rect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Line_Rect(const CVIBuffer_Line_Rect& Prototype);
	virtual ~CVIBuffer_Line_Rect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Prototype_Half();
	virtual HRESULT Initialize(void* pArg) override;

	void CalculateQuad(const _float3& start, const _float3& end, _float thickness, const _float3& cameraPos);

public:
	static CVIBuffer_Line_Rect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	static CVIBuffer_Line_Rect* Create_Half(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END