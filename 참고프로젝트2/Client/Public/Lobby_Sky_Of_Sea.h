#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
END

BEGIN(Client)

class CLobby_Sky_Of_Sea final : public CGameObject
{
private:
	CLobby_Sky_Of_Sea(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLobby_Sky_Of_Sea(const CLobby_Sky_Of_Sea& Prototype);
	virtual ~CLobby_Sky_Of_Sea() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Camera_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;

	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

	_float m_fTime = { 0.f };
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CLobby_Sky_Of_Sea* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END