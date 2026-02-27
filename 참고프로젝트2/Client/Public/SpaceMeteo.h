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

class CSpaceMeteo final : public CGameObject
{
private:
	CSpaceMeteo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpaceMeteo(const CSpaceMeteo& Prototype);
	virtual ~CSpaceMeteo() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Camera_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;

private:
	CShader*				m_pShaderCom = { nullptr };
	CModel*					m_pModelCom[3] = { nullptr,nullptr ,nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CSpaceMeteo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END