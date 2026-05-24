#pragma once
#include "VIBuffer.h"
#include "Game_PKM_Defines.h"

NS_BEGIN(Game_PKM)

class CVIBuffer_Trail final : public CVIBuffer
{
public:
	static constexpr _uint iMaxVertices = 256;   // 128 segment ¡¿ 2

private:
	CVIBuffer_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Trail(const CVIBuffer_Trail& Prototype);
	virtual ~CVIBuffer_Trail() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render() override;

	HRESULT Update_Vertices(const VTXTRAIL* pVertices, _uint iNumVertices);

private:
	D3D11_BUFFER_DESC m_VBDesc = {};

public:
	static CVIBuffer_Trail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
private:
	virtual void Free() override;
};
NS_END