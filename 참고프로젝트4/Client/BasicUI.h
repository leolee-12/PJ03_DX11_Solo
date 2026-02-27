#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CBasicUI final : public CGameObject
{
public:
	INFO_CLASS(CBasicUI, CGameObject)

	typedef struct tagUIDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_float			fX, fY, fSizeX, fSizeY;
		wstring			strTextureTag = {};
		_uint			iTextureIndex = {0};
	}UI_DESC;

private:
	CBasicUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBasicUI(const CBasicUI& rhs);
	virtual ~CBasicUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_cref_time fTimeDelta) override;
	virtual void Tick(_cref_time fTimeDelta) override;
	virtual void Late_Tick(_cref_time fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_float				m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4			m_ViewMatrix, m_ProjMatrix;
	wstring				m_strTextureTag = {};
	_uint				m_iTextureIndex = {0};
	_float				m_fRatioY = { 1.f };

private:
	shared_ptr<CTexture> m_pTextureCom = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	GETSET_EX2(_uint, m_iTextureIndex, TextureIndex, GET, SET)
	GETSET_EX2(_float, m_fRatioY, RatioY, GET, SET)

public:
	/* 원형객체를 생성한다. */
	static CBasicUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	/* 사본객체를 생성한다. */
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;

	virtual void Free() override;	
};

END