#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CModel_Preview final : public CGameObject
{
public:
	typedef struct: CGameObject::GAMEOBJECT_DESC
	{
		_wstring strModelName;
	}MODEL_PREVIEW_DESC;
private:
	CModel_Preview(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel_Preview(const CModel_Preview& Prototype);
	virtual ~CModel_Preview() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Camera_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;

	void FlipDirection(_int iDirection = 0);

	_int Get_iDirection() { return m_iLookDirection; };

private:
	CShader*				m_pShaderCom = { nullptr };	
	CModel*					m_pModelCom = { nullptr };

	_float					m_fRandom = {};
	_wstring				m_strModelName{};


	_int					m_iLookDirection = { 1 };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CModel_Preview* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END