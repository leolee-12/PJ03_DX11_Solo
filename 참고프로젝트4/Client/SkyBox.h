//Sky.h
#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Cube;
END

BEGIN(Client)

class CSkyBox final : public CGameObject
{
	INFO_CLASS(CSkyBox, CGameObject)
private:
	CSkyBox(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CSkyBox(const CSkyBox& rhs);
	virtual ~CSkyBox() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring& strTextureTag) ;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_cref_time fTimeDelta) override;
	virtual void Tick(_cref_time fTimeDelta) override;
	virtual void Late_Tick(_cref_time fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	shared_ptr<CTexture> m_pTextureCom = { nullptr };

	wstring		m_TextureProtoTag = {};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


public:
	/* 원형객체를 생성한다. */
	static shared_ptr<CSkyBox> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strTextureTag);

	/* 사본객체를 생성한다. */
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;

	virtual void Free() override;
};

END