#include "..\Public\Shader.h"

CShader::CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CShader::CShader(const CShader& Prototype)
	: CComponent{ Prototype }
	, m_pEffect{ Prototype.m_pEffect }
	, m_InputLayouts{ Prototype.m_InputLayouts }
	, m_iNumPasses{ Prototype.m_iNumPasses }
{
	Safe_AddRef(m_pEffect);

	for (auto& pInputLayout : m_InputLayouts)
		Safe_AddRef(pInputLayout);
}

HRESULT CShader::Initialize_Prototype(const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
	_uint		iHlslFlag = { 0 };

#ifdef _DEBUG
	iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif

	/* 쉐이더를 빌드하는 기능을 수행하고. */
	/* 쉐이더를 대표하는 컴객체를 생성한다. */
	if (FAILED(D3DX11CompileEffectFromFile(pShaderFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, iHlslFlag, 0, m_pDevice, &m_pEffect, nullptr)))
		return E_FAIL;

	ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
		return E_FAIL;

	D3DX11_TECHNIQUE_DESC		TechniqueDesc{};

	pTechnique->GetDesc(&TechniqueDesc);

	m_iNumPasses = TechniqueDesc.Passes;

	//D3D11_INPUT_ELEMENT_DESC		Elements[] = {
	//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//};

	for (size_t i = 0; i < m_iNumPasses; i++)
	{
		ID3DX11EffectPass* pPass = pTechnique->GetPassByIndex(i);
		if (nullptr == pPass)
			return E_FAIL;

		D3DX11_PASS_DESC		PassDesc{};

		pPass->GetDesc(&PassDesc);



		ID3D11InputLayout* pInputLayout = { nullptr };
		if (FAILED(m_pDevice->CreateInputLayout(
			pElements, /* 내 정점의 멤버변수에 대한 설명. */
			iNumElements, /* 이 정점은 몇개의 멤버변수르 ㄹ가지고 있냐? */
			PassDesc.pIAInputSignature, /* 셰이더에서 어떤 형태의 정점을 받고 있는지를 내놔?! */
			PassDesc.IAInputSignatureSize, /* 위에서 전달해준 정점 메모리의 크기는 몇바이트인지?  */
			&pInputLayout)))
			return E_FAIL;

		m_InputLayouts.emplace_back(pInputLayout);
	}

	return S_OK;
}

HRESULT CShader::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CShader::Begin(_uint iPassIndex)
{
	if (iPassIndex >= m_iNumPasses)
		return E_FAIL;

	m_pContext->IASetInputLayout(m_InputLayouts[iPassIndex]);

	ID3DX11EffectPass* pPass = m_pEffect->GetTechniqueByIndex(0)->GetPassByIndex(iPassIndex);
	if (nullptr == pPass)
		return E_FAIL;

	/* 이 쉐이더 안에 정의된 패스로 정점과 인덱스를 그리겠다. */
	/* Apply함수를 호출하기 전에 쉐이더에 전달해야할 모든 전역데이터들을 다 전달해야한다. */
	return pPass->Apply(0, m_pContext);
}

HRESULT CShader::Bind_RawValue(const _char* pConstantName, const void* pData, _uint iSize)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	return pVariable->SetRawValue(pData, 0, iSize);
}

HRESULT CShader::Bind_Matrix(const _char* pConstantName, const _float4x4* pMatrix)
{
	/* 전달해준 문자열과 같은 이름을 가진 쉐이더내에 존재하는 전역변수의 컴객체(핸들)를 얻어온다.*/
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
	if (nullptr == pMatrixVariable)
		return E_FAIL;

	return pMatrixVariable->SetMatrix(reinterpret_cast<const _float*>(pMatrix));
}

HRESULT CShader::Bind_Matrices(const _char* pConstantName, const _float4x4* pMatrix, _uint iNumMatrices)
{
	/* 전달해준 문자열과 같은 이름을 가진 쉐이더내에 존재하는 전역변수의 컴객체(핸들)를 얻어온다.*/
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
	if (nullptr == pMatrixVariable)
		return E_FAIL;

	return pMatrixVariable->SetMatrixArray(reinterpret_cast<const _float*>(pMatrix), 0, iNumMatrices);
}

HRESULT CShader::Bind_ShaderResourceView(const _char* pConstantName, ID3D11ShaderResourceView* pSRV)
{
	/* 전달해준 문자열과 같은 이름을 가진 쉐이더내에 존재하는 전역변수의 컴객체(핸들)를 얻어온다.*/
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable* pSRVariable = pVariable->AsShaderResource();
	if (nullptr == pSRVariable)
		return E_FAIL;

	return pSRVariable->SetResource(pSRV);
}

HRESULT CShader::Bind_ShaderResourceViewWithMip(const _char* pConstantName, ID3D11ShaderResourceView* pSRV, UINT mipLevel)
{
	if (mipLevel < 0)
		return E_FAIL;

	/* 전달해준 문자열과 같은 이름을 가진 쉐이더내에 존재하는 전역변수의 컴객체(핸들)를 얻어온다.*/
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable* pSRVariable = pVariable->AsShaderResource();
	if (nullptr == pSRVariable)
		return E_FAIL;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	pSRV->GetDesc(&srvDesc);
	if (srvDesc.ViewDimension != D3D11_SRV_DIMENSION_TEXTURE2D)
		return E_FAIL;
	
	srvDesc.Texture2D.MostDetailedMip = mipLevel;
	srvDesc.Texture2D.MipLevels = 1;  // 지정한 MIP 레벨만 사용

	ID3D11Resource* pResource = { nullptr };
	pSRV->GetResource(&pResource);
	
	ID3D11ShaderResourceView* pNewSRV = { nullptr };
	if (FAILED(m_pDevice->CreateShaderResourceView(pResource, &srvDesc, &pNewSRV)))
		return E_FAIL;

	return pSRVariable->SetResource(pNewSRV);
}


HRESULT CShader::Bind_ShaderResourceViews(const _char* pConstantName, ID3D11ShaderResourceView** ppSRV, _uint iNumTextures)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable* pSRVariable = pVariable->AsShaderResource();
	if (nullptr == pSRVariable)
		return E_FAIL;

	return pSRVariable->SetResourceArray(ppSRV, 0, iNumTextures);
}

CShader* CShader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
	CShader* pInstance = new CShader(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pShaderFilePath, pElements, iNumElements)))
	{
		MSG_BOX(TEXT("Failed to Created : CShader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CShader::Clone(void* pArg)
{
	CShader* pInstance = new CShader(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CShader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CShader::Free()
{
	__super::Free();

	Safe_Release(m_pEffect);

	for (auto& pInputLayout : m_InputLayouts)
		Safe_Release(pInputLayout);
	m_InputLayouts.clear();

}
