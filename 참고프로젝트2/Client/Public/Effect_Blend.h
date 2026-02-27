#pragma once	

#include "Client_Defines.h"
#include "Effect.h"


BEGIN(Client)

class CEffect_Blend : public CEffect
{
private:
	CEffect_Blend(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Blend(const CEffect_Blend& Prototype);
	virtual ~CEffect_Blend() = default;

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
	static CEffect_Blend* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END