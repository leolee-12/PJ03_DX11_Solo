#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CShader final : public CComponent
{
private:
	CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShader(const CShader& Prototype);
	virtual ~CShader() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements);
	virtual HRESULT Initialize(void* pArg);

public:
	_uint Get_NumPasses() const { return m_iNumPasses; }

	HRESULT Begin(_uint iPassIndex);
	HRESULT Bind_Matrix(const _char* pConstName, const _float4x4* pMatrix);
	HRESULT Bind_Matrices(const _char* pConstName, const _float4x4* pMatrices, _uint iNumMatrices);
	HRESULT Bind_SRV(const _char* pConstName, ID3D11ShaderResourceView* pSRV);
	HRESULT Bind_SRVs(const _char* pConstantName, ID3D11ShaderResourceView** ppSRVArray, _uint iNumSRVs);
	HRESULT Bind_RawValue(const _char* pConstName, const void* pValue, _uint iLength);

private:
	_uint m_iNumPasses = {};
	ID3DX11Effect* m_pEffect = { nullptr };

	vector<ID3D11InputLayout*> m_InputLayouts;

public:
	static CShader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements);
	virtual CComponent* Clone(void* pArg) override;

protected:
	virtual void Free() override;
};

NS_END