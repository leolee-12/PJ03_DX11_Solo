#pragma once

#include "Effect.h"
#include "VIBuffer_Instancing.h"
BEGIN(Engine)

END

BEGIN(Client)

class CParticle final : public CEffect
{
public:
	typedef struct tagParticleDesc : public CEffect::EFFECT_DESC
	{
		_uint iNumInstnace;

		// -----  MaskArray --------
		
		_bool	bMaskArray = { false };
		_uint	iMaskArrayNum = { 0 };
		wstring strMaskArray[18];
		
		CVIBuffer_Instancing::INSTANCING_DESC Instancing_Desc = {};
		
		// ----- Soft -------
		_bool		bParticleSoft = { true };

	}PARTICLE_DESC;
private:
	CParticle(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CParticle(const CParticle& rhs);
	virtual ~CParticle() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Prototype(string strFilePath);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Begin_Play(_cref_time fTimeDelta) override;
	virtual void Priority_Tick(_cref_time fTimeDelta) override;
	virtual void Tick(_cref_time fTimeDelta) override;
	virtual void Late_Tick(_cref_time fTimeDelta) override;
	virtual void Before_Render(_cref_time fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual	void	Reset_Effect(_bool bActivate = false) override;
	virtual	void	Reset_Prototype_Data() override;

private:
	CVIBuffer_Instancing::INSTANCING_DESC* m_pInstancingDesc = { nullptr };
	//CTransform* m_pOwnerTransformCom = { nullptr };
	_bool		  m_bPlay = { true };

	PARTICLE_DESC	m_tParticle_Desc;

private:
	//vector<wstring> m_vecMask;	// 마스크 배열을 사용하기 위함
	shared_ptr<class CMaterialComponent> m_pMaskArrayCom = { nullptr };
	//wstring	m_strMaskTagArray[18];

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	virtual void	Judge_Dead(EFFECT_DESC EffectDesc) override;

public:
		void Reset_AllParticles();

		GETSET_EX2(_bool, m_bPlay, Play, GET, SET)
		//GETSET_EX2(CTransform*, m_pOwnerTransformCom, OwnerTransformCom, GET, SET)
		GETSET_EX2(PARTICLE_DESC, m_tParticle_Desc, ParticleDesc, GET, SET)

public:
	virtual void Write_Json(json& Out_Json);
	virtual void Load_Json(const json& In_Json);

public:
	/* 원형객체를 생성한다. */
	static shared_ptr<CParticle> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	static shared_ptr<CParticle> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, string strFilePath);

	/* 사본객체를 생성한다. */
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;

	virtual void Free() override;
};

END