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

class CSpaceRock final : public CGameObject
{
private:
	CSpaceRock(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpaceRock(const CSpaceRock& Prototype);
	virtual ~CSpaceRock() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Camera_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;

	_bool m_isBreakRock[3] = { false,false,false };
private:
	CShader*				m_pShaderCom = { nullptr };
	CModel*					m_pModelCom[3] = { nullptr,nullptr ,nullptr };
	CModel* m_pBRModelCom[3] = { nullptr,nullptr ,nullptr };
	//_bool m_isBreakRock[3] = { true,true,true };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CSpaceRock* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END