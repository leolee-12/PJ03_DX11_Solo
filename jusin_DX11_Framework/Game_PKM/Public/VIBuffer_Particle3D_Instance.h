#pragma once
#include "VIBuffer_Instance.h"
#include "Game_PKM_Defines.h"

NS_BEGIN(Game_PKM)

class CVIBuffer_Particle3D_Instance final : public CVIBuffer_Instance
{
public:
	struct PARTICLE3D_INSTANCE_DESC final : public CVIBuffer_Instance::INSTANCE_DESC
	{
	};

private:
	CVIBuffer_Particle3D_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const
		PARTICLE3D_INSTANCE_DESC& tDesc);
	CVIBuffer_Particle3D_Instance(const CVIBuffer_Particle3D_Instance& Prototype);
	virtual ~CVIBuffer_Particle3D_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	HRESULT Update_Particle3D_Instances(const VTXPARTICLE3D_INSTANCE* pInstances, _uint
		iNumInstances);

private:
	PARTICLE3D_INSTANCE_DESC m_tInitDesc = {};

public:
	static CVIBuffer_Particle3D_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext*
		pContext, void* pInitialDesc);
	virtual CComponent* Clone(void* pArg) override;

private:
	virtual void Free() override;
};

NS_END