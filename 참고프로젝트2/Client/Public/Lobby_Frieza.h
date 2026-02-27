#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CLobby_Frieza final : public CGameObject
{
private:
	CLobby_Frieza(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLobby_Frieza(const CLobby_Frieza& Prototype);
	virtual ~CLobby_Frieza() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Camera_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;

	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	HRESULT Ready_Text();
		
private:
	_uint m_iPassIndex = { 3 };
	_bool m_isShadow = { true };

public:
	static CLobby_Frieza* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END