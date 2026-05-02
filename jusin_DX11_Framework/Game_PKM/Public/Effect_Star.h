#pragma once
#include "UIObject.h"
#include "Game_PKM_Defines.h"

NS_BEGIN(Engine)
class CTexture;
class CShader;
class CUIObject;
NS_END

NS_BEGIN(Game_PKM)
class CVIBuffer_UI_Instance;

class CEffect_Star final : public CUIObject
{
public:
	struct EFFECT_STAR_DESC final : public CUIObject::UIOBJECT_DESC
	{
		_uint iNumParticles = {};
		_float2 vSpawnRange = {};
		_float2 vSizeRange = {};
		_float2 vSpeedRange = {};
		_float2 vEmitDir = {};
		_float fEmitSpreadAngle = {};
		_float2 vLifeRange = {};
		_float2 vRotationSpeedRange = {};
		_float fMaskStrength{ 1.f };
		_float2 vMaskRotationSpeedRange = {};
		TEXTURE_SAMPLE_MODE eMaskSampleMode{ TEXTURE_SAMPLE_MODE::SINGLE };
		TEXTURE_SAMPLE_MODE eSubSampleMode{ TEXTURE_SAMPLE_MODE::SINGLE };

		_float4 vColor = {};
		_bool bStartActive{ false };
		_bool bLoop{ false };

		_uint iStarTextureIndex = {};
		_uint iMaskTextureIndex = {};
		_uint iDiamondTextureIndex = {};
	};

protected:
	CEffect_Star(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Star(const CEffect_Star& Prototype);
	virtual ~CEffect_Star() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Play();
	void Stop();

private:
	CTexture* m_pTextureCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_UI_Instance* m_pVIBufferCom = { nullptr };

	EFFECT_STAR_DESC m_tDesc = {};
	vector<PARTICLE_UI_STATE> m_Particles;
	vector<VTXUI_INSTANCE> m_RenderInstances;

	_bool m_isActive = { false };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	void Initialize_Particle(_uint iParticleIndex);
	void Reset_AllParticles();
	void Update_Particles(_float fTimeDelta);
	void Build_RenderInstances();
	HRESULT Upload_RenderInstances();

public:
	static CEffect_Star* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

private:
	virtual void Free() override;
};

NS_END