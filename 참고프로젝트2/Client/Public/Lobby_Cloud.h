#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
END

BEGIN(Client)

class CLobby_Cloud final : public CGameObject
{
public:
	typedef struct
	{
		_uint iNumObject = {};
	}CLOUD_DESC;

private:
	CLobby_Cloud(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLobby_Cloud(const CLobby_Cloud& Prototype);
	virtual ~CLobby_Cloud() = default;

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
	void Default_Setting(_uint iNumObj,_wstring&  strPrototypeTag);

private:
	HRESULT Ready_Components(_wstring strPrototypeTag);
	HRESULT Bind_ShaderResources();

public:
	static CLobby_Cloud* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END