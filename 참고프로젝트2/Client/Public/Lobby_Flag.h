#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
END

BEGIN(Client)

class CLobby_Flag final : public CGameObject
{
public:
	typedef struct
	{
		_uint iNumObject;
	}FLAG_DESC;

private:
	CLobby_Flag(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLobby_Flag(const CLobby_Flag& Prototype);
	virtual ~CLobby_Flag() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Camera_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;

private:
	void Default_Setting(_uint iNum);

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CTexture* m_pDiffTexture[2] = {nullptr};

private:
	_uint m_iSpriteIndex = { 0 };
	_uint m_iShaderIndex = { 0 };

private:
	_float m_fAnimFrame = { 0.f };
	_float m_fTexcoordValue = { 0.f };
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CLobby_Flag* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END