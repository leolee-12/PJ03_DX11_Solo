#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{
private:
	CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTexture(const CTexture& Prototype);
	virtual ~CTexture() = default;


public:
	virtual HRESULT Initialize_Prototype(const _tchar* pTextureFilePath, _uint iNumTextures);
	virtual HRESULT Initialize(void* pArg);

public:
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName, _uint iTextureIndex);
	HRESULT Bind_ShaderResourceArray(class CShader* pShader, const _char* pConstantName, _uint iNumTextures);
	_float2 Get_TextureSize();
	HRESULT Set_SRV(ID3D11ShaderResourceView* pSRV, _int iArray = 0);
	ID3D11ShaderResourceView* Get_SRV(_int iIndex)
	{
		if (iIndex >= m_SRVs.size())
		{
			MSG_BOX(TEXT("Your EffectModel To ShaderTab : Link Zero (Copy No)"));
			return nullptr;
		}
		else
			return m_SRVs[iIndex];
	}
private:
	_uint									m_iNumTextures = { 0 };
	vector<ID3D11ShaderResourceView*>		m_SRVs;
	_tchar			m_szFullPath[MAX_PATH] = {};
public:
	static CTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTextureFilePath, _uint iNumTextures = 1);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END