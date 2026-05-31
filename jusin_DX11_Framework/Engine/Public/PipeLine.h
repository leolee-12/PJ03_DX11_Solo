#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CPipeLine final : public CBase
{
private:
	CPipeLine();
	virtual ~CPipeLine() = default;

public:
	const _float4x4* Get_Transform(D3DTS eState) const { return &m_TransformStateMatrices[ETOUI(eState)]; }
	const _float4x4* Get_Transform_Inverse(D3DTS eState) const { return &m_TransformStateInverseMatrices[ETOUI(eState)]; }
	const _float4* Get_CamPosition() const { return &m_vCamPosition; }

	void XM_CALLCONV Set_CameraWorld(_fmatrix StateMatrix)
	{	// CamWorld를 받아 TSInv[VIEW]에 저장, 역행렬 구해서 TS[VIEW]에 저장, CamPos 저장
		XMStoreFloat4x4(&m_TransformStateInverseMatrices[ETOUI(D3DTS::VIEW)], StateMatrix);
		XMStoreFloat4x4(&m_TransformStateMatrices[ETOUI(D3DTS::VIEW)], XMMatrixInverse(nullptr, StateMatrix));
		memcpy(&m_vCamPosition, &m_TransformStateInverseMatrices[ETOUI(D3DTS::VIEW)]._41, sizeof m_vCamPosition);
	}

	void XM_CALLCONV Set_Projection(_fmatrix StateMatrix)
	{	// ProjMatrix를 받아 TS[PROJ]에 저장, 역행렬 구해서 TSInv[PROJ]에 저장
		XMStoreFloat4x4(&m_TransformStateMatrices[ETOUI(D3DTS::PROJ)], StateMatrix);
		XMStoreFloat4x4(&m_TransformStateInverseMatrices[ETOUI(D3DTS::PROJ)], XMMatrixInverse(nullptr, StateMatrix));
	}

private:
	_float4x4 m_TransformStateMatrices[ETOUI(D3DTS::END)] = {};
	_float4x4 m_TransformStateInverseMatrices[ETOUI(D3DTS::END)] = {};
	_float4 m_vCamPosition = {};

public:
	static CPipeLine* Create();

protected:
	virtual void Free() override;
};

NS_END