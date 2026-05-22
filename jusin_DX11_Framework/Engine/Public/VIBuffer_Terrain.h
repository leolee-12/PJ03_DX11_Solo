#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)
class CQuadTree;

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
private:
	CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pHeightMapFilePath);
	CVIBuffer_Terrain(const CVIBuffer_Terrain& Prototype);
	virtual ~CVIBuffer_Terrain() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	void XM_CALLCONV Culling(_fmatrix WorldMatrix);

private:
	_uint m_iNumVerticesX{}, m_iNumVerticesZ{};
	const _wstring m_strHeightMapFilePath = {};
	_float3* m_pVtxPos = { nullptr };
	CQuadTree* m_pQuadTree{ nullptr };

public:
	static CVIBuffer_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pHeightMapFilePath);
	virtual CComponent* Clone(void* pArg) override;

protected:
	virtual void Free() override;

};

NS_END