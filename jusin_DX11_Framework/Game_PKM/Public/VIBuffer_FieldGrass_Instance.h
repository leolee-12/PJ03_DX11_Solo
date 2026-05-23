#pragma once
#include "VIBuffer_Instance.h"
#include "Game_PKM_Defines.h"

NS_BEGIN(Game_PKM)

class CVIBuffer_FieldGrass_Instance final : public CVIBuffer_Instance
{
public:
	struct FIELDGRASS_INSTANCE_DESC final : public CVIBuffer_Instance::INSTANCE_DESC
	{
		const _char* pModelFilePath = "../../Resources/Models/grass/grass.wmodel";
	};

private:
	CVIBuffer_FieldGrass_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
		const FIELDGRASS_INSTANCE_DESC& tDesc);
	CVIBuffer_FieldGrass_Instance(const CVIBuffer_FieldGrass_Instance& Prototype);
	virtual ~CVIBuffer_FieldGrass_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	HRESULT Update_FieldGrass_Instances(const VTXFIELDGRASS_INSTANCE* pInstances, _uint iNumInstances);

private:
	HRESULT Ready_Mesh_FromWModel(const _char* pModelFilePath);
	HRESULT Ready_InstanceBufferDesc();

private:
	FIELDGRASS_INSTANCE_DESC m_tInitDesc = {};

public:
	static CVIBuffer_FieldGrass_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
		void* pInitialDesc);
	virtual CComponent* Clone(void* pArg) override;

private:
	virtual void Free() override;
};

NS_END