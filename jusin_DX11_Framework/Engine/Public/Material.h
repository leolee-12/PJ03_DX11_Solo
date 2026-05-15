#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CMaterial final : public CBase
{
private:
	CMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMaterial() = default;

public:
	HRESULT Initialize(const WMODEL_MATERIAL& tMat, const _char* pBaseDir);
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName, MATERIAL_TYPE eType, _uint iIndex);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	vector<ID3D11ShaderResourceView*> m_Materials[ETOUI(MATERIAL_TYPE::END)];
	ID3D11ShaderResourceView* m_pDefaultMaterial = { nullptr };
	ID3D11ShaderResourceView* m_pDefaultNormal = { nullptr };

private:
	// 확장자별 SRV 로딩 (공통 코어)
	static HRESULT  Load_SRV_ByExtension(ID3D11Device* pDevice, const _wstring& fullPath, ID3D11ShaderResourceView** ppSRV);
	// 모드별 어댑터 - Initialize 루프에서 매크로로 한 줄 교체
	static HRESULT  Load_SRV_FromOriginal(ID3D11Device* pDevice, const _wstring& origPath, ID3D11ShaderResourceView** ppSRV);
	static HRESULT  Load_SRV_FromDDS(ID3D11Device* pDevice, const _wstring& origPath, ID3D11ShaderResourceView** ppSRV);

public:
	static CMaterial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const WMODEL_MATERIAL& tMat, const _char* pBaseDir);

private:
	virtual void Free() override;
};

NS_END