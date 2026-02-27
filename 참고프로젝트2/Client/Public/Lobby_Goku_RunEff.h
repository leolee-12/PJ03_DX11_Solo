#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CShader;
class CTexture;
class CVIBuffer_Point_Instancing;

END

BEGIN(Client)

class CLobby_Goku_RunEff final :public CGameObject
{
private:
	CLobby_Goku_RunEff(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLobby_Goku_RunEff(const CLobby_Goku_RunEff& Prototype);
	virtual ~CLobby_Goku_RunEff() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Camera_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render(_float fTimeDelta);

private:
	virtual HRESULT Ready_Components();
	virtual HRESULT Bind_ShaderResources();

private:
	void Chase(_float fOffsetZ);

private:
	_uint m_iTextureIndex = { 0 };
	_float m_fDestroyTimer = { 0.f };

	_vector m_vDir = {};

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Point_Instancing* m_pVIBufferCom = { nullptr };

public:
	static CLobby_Goku_RunEff* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END