#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CMaterial final : public CBase
{
private:
	CMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMaterial() = default;

public:
	HRESULT Initialize(aiMaterial* pAIMaterial, const _char* pModelFilePath);
	HRESULT Initialize_Ex(aiMaterial* pAIMaterial, const _char* pModelFilePath);
	HRESULT Initialize(const WMODEL_MATERIAL& tMat, const _char* pBaseDir);
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName, TEXTURE_TYPE eType, _uint iIndex);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	vector<ID3D11ShaderResourceView*> m_Materials[ETOUI(TEXTURE_TYPE::END)];
	ID3D11ShaderResourceView* m_DefaultMaterial = { nullptr };

public:
	static CMaterial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, aiMaterial* pAIMaterial, const _char* pModelFilePath);
	static CMaterial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const WMODEL_MATERIAL& tMat, const _char* pBaseDir);
	virtual void Free() override;
};

NS_END