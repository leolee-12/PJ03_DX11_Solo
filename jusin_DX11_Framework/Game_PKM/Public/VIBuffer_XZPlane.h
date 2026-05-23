#pragma once
#include "VIBuffer.h"
#include "Game_PKM_Defines.h"

NS_BEGIN(Game_PKM)

class CVIBuffer_XZPlane final : public CVIBuffer
{
public:
	struct XZ_PLANE_DESC
	{
		_float fWidth = 60.f;
		_float fDepth = 60.f;
		_float fTileU = 1.f;
		_float fTileV = 1.f;
	};

private:
	CVIBuffer_XZPlane(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_XZPlane(const CVIBuffer_XZPlane& Prototype);
	virtual ~CVIBuffer_XZPlane() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	static CVIBuffer_XZPlane* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;

private:
	virtual void Free() override;
};

NS_END