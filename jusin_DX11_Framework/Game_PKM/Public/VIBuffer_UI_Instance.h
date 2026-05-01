#pragma once
#include "VIBuffer_Instance.h"

#include "Game_PKM_Defines.h"

NS_BEGIN(Game_PKM)

class CVIBuffer_UI_Instance final : public CVIBuffer_Instance
{
public:
	struct UI_INSTANCE_DESC final : public CVIBuffer_Instance::INSTANCE_DESC
	{
	};

private:
	CVIBuffer_UI_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const UI_INSTANCE_DESC& tDesc);
	CVIBuffer_UI_Instance(const CVIBuffer_UI_Instance& Prototype);
	virtual ~CVIBuffer_UI_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	HRESULT Update_UIInstances(const VTXPARTICLE_UI_INSTANCE* pInstances, _uint iNumInstances);

private:
	UI_INSTANCE_DESC m_tInitDesc = {};

public:
	static CVIBuffer_UI_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pInitialDesc);
	virtual CComponent* Clone(void* pArg) override;

private:
	virtual void Free() override;

};

NS_END