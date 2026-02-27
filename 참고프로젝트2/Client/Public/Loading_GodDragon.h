#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CLoading_GodDragon final : public CGameObject
{
private:
	CLoading_GodDragon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLoading_GodDragon(const CLoading_GodDragon& Prototype);
	virtual ~CLoading_GodDragon() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Camera_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CTexture* m_pLimTextureCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	string m_strName;
	_int m_iDir = { 1 };

private:
	_int m_iLookDirection = { 0 };
	_float m_fTexcoordValue = { 1.f };

private:
	HRESULT Add_Light(_float4 vDirection, _float4 vDiffuse, _float4 vAmbient, _float4 vSpecular, string strName);

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CLoading_GodDragon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END