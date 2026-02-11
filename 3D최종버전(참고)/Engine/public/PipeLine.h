#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CPipeLine final : public CBase
{
private:
	CPipeLine();
	virtual ~CPipeLine() = default;

public: /* Getter */
	const _float4x4* Get_Transform_Float4x4_Ptr(D3DTS eState);
	_matrix Get_Transform_Matrix(D3DTS eState);
	const _float4x4* Get_Transform_Float4x4_Inverse_Ptr(D3DTS eState);
	_matrix Get_Transform_Matrix_Inverse(D3DTS eState);

	const _float4* Get_CamPosition();

public: /* Setter */
	void Set_Transform(D3DTS eState, _fmatrix TransformMatrix);

public:
	HRESULT Initialize();
	void Update();

private:
	_float4x4				 m_TransformationMatrix[ENUM_CLASS(D3DTS::END)] = {};
	_float4x4				 m_TransformationMatrix_Inverse[ENUM_CLASS(D3DTS::END)] = {};
	_float4					 m_vCamPosition = {0.0f, 0.f, 0.f, 1.f};

public:
	static CPipeLine* Create();
	virtual void Free();

};

NS_END