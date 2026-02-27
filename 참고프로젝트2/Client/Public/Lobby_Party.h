#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
END

BEGIN(Client)

class CLobby_Party final : public CGameObject
{
private:
	CLobby_Party(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLobby_Party(const CLobby_Party& Prototype);
	virtual ~CLobby_Party() = default;

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
	CTexture* m_pDiffTexture[8] = {nullptr};
	CTexture* m_pWaterSur = { nullptr };
	CTexture* m_pWaterLight = { nullptr };

private:
	_uint m_iShaderIndex = { 0 };
	_float m_fTexcoordValue = { 0.f };


private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CLobby_Party* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END