#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Point_Instancing;
END

BEGIN(Client)

class CQTE_Hit_UI_Particle final : public CGameObject
{
public:
	struct Hit_PARTICLE_DESC
	{
		_float	fSizeX{}, fSizeY{}, fX{}, fY{};
	};

private:
	CQTE_Hit_UI_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CQTE_Hit_UI_Particle(const CQTE_Hit_UI_Particle& Prototype);
	virtual ~CQTE_Hit_UI_Particle() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Camera_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;

public:
	_float					m_fSizeX{}, m_fSizeY{}, m_fX{}, m_fY{};
	_float4x4				m_ViewMatrix{}, m_ProjMatrix{};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Point_Instancing* m_pVIBufferCom = { nullptr };

public:
	static CQTE_Hit_UI_Particle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END