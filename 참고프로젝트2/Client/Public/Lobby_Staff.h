#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CLobby_Staff final : public CGameObject
{
public:
	typedef struct
	{
		_uint iNumObject = { 0 };
	}STAFF_DESC;

public:
	enum NPC_STATE {IDLE , RUN , STATE_END};

private:
	CLobby_Staff(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLobby_Staff(const CLobby_Staff& Prototype);
	virtual ~CLobby_Staff() = default;

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

private:
	NPC_STATE m_eCurrState = { STATE_END };
	NPC_STATE m_ePrevState = { STATE_END };

private:
	void State();
	void AnimState();

	_bool Idle(_float fTimeDelta);
	_bool Run(_vector vTargPos, _float fTimeDelta);

private:
	void Default_Setting(_uint iNumObj);

private:
	_bool m_bAnimChange = { TRUE };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CLobby_Staff* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END