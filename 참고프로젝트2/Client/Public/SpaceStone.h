#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Map_Manager.h"
BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
END

BEGIN(Client)

class CSpaceStone final : public CGameObject
{
private:
	CSpaceStone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpaceStone(const CSpaceStone& Prototype);
	virtual ~CSpaceStone() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Camera_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;

private:
	CShader*				m_pShaderCom = { nullptr };
	CModel*					m_pModelCom = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CSpaceStone* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END