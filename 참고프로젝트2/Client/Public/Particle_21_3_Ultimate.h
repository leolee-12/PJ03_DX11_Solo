#pragma once

#include "Client_Defines.h"
#include "Particle.h"
BEGIN(Engine)
class CShader;
class CVIBuffer_Point_Instancing;
END

BEGIN(Client)

class CParticle_21_3_Ultimate final : public CParticle
{
protected:
	CParticle_21_3_Ultimate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle_21_3_Ultimate(const CParticle_21_3_Ultimate& Prototype);
	virtual ~CParticle_21_3_Ultimate() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Camera_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;

	virtual void Set_Particle_Active(_bool isActive) override;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Point_Instancing* m_pVIBufferCom = { nullptr };

public:
	static CParticle_21_3_Ultimate* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END