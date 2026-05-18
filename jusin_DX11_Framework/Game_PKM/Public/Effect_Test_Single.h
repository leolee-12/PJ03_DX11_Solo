#pragma once
#include "Game_PKM_Defines.h"
#include "GameObject.h"
#include "Particle.h"

NS_BEGIN(Engine)
class CTexture;
class CShader;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Game_PKM)

class CEffect_Test_Single final : public CGameObject
{
public:
	struct DESC final : public CGameObject::GAMEOBJECT_DESC
	{
		_float3 vInitVelocity = { 0.f, 1.f, 0.f };
		_float3 vAcceleration = { 0.f, 0.f, 0.f };
		_float  fLifeTime = 2.f;
		_float  fSize = 1.f;
		_float4 vColor = { 1.f, 1.f, 1.f, 1.f };
	};

private:
	CEffect_Test_Single(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Test_Single(const CEffect_Test_Single& Prototype);
	virtual ~CEffect_Test_Single() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	void Reset_Particle();
	void Update_Particle(_float fTimeDelta);
	void Sync_Transform_ToParticle();

private:
	DESC m_tDesc = {};
	CParticle m_Particle = {};

	CTexture* m_pTextureCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

public:
	static CEffect_Test_Single* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

private:
	virtual void Free() override;
};

NS_END