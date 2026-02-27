#pragma once	

#include "Client_Defines.h"
#include "Effect.h"


BEGIN(Client)

class CEffect_NoneLight : public CEffect
{
private:
	CEffect_NoneLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_NoneLight(const CEffect_NoneLight& Prototype);
	virtual ~CEffect_NoneLight() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Camera_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Priority_Render(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;

private:
	virtual HRESULT Ready_Components(_wstring* pModelName, _wstring* pMaskTextureName, _wstring* pDiffuseTexturueName) override;
	virtual HRESULT Bind_ShaderResources() override;

public:
	static CEffect_NoneLight* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END