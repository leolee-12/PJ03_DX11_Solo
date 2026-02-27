#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Point_Instancing;
END

BEGIN(Client)

class CQTE_Continuous_Attack_Particle final : public CGameObject
{
public:
	struct QTE_Continuous_Attack_Particle_DESC
	{
		_float	fSizeX{}, fSizeY{}, fX{}, fY{};
		_float* pfGaugeRatio{};
	};

private:
	CQTE_Continuous_Attack_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CQTE_Continuous_Attack_Particle(const CQTE_Continuous_Attack_Particle& Prototype);
	virtual ~CQTE_Continuous_Attack_Particle() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Camera_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;

private:
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Point_Instancing* m_pVIBufferCom = { nullptr };

	_float					m_fSizeX{}, m_fSizeY{}, m_fX{}, m_fY{};
	_float4x4				m_ViewMatrix{}, m_ProjMatrix{};

	_float* m_pfGaugeRatio = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CQTE_Continuous_Attack_Particle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END