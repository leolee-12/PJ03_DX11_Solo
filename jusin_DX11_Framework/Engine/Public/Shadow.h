#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CShadow final : public CBase
{
private:
	CShadow();
	virtual ~CShadow() = default;

public:
	const _float4x4* Get_Transform(D3DTS eState) const { return &m_TransformStateMatrices[ETOUI(eState)]; }

	HRESULT Set_ShadowLight(const SHADOW_LIGHT_DESC& ShadowDesc);
	HRESULT Bind_FarZ(class CShader* pShaderCom);
	void Update();

private:
	class CGameInstance*	m_pGameInstance = { nullptr };
	_float4x4				m_TransformStateMatrices[ETOUI(D3DTS::END)] = {};
	_float					m_fFarZ = {};

	_float3					m_vEyeOffset = {};		// 광원 방향 + 거리 (정규화X)
	_bool					m_bFollow = { false };	// Set 이후 Follow 시작

public:
	static CShadow* Create();
	virtual void Free() override;
};

NS_END