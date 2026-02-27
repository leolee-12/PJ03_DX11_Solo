#pragma once

/* ViewMatrix, ProjMatrix의 정보를 보관한다. 리턴해준다. */
/* Update함수도 구현해서. 매 프레임마다 한번씩 호출해준다. */
/* Update : 들고 있는 View, Proj행렬의 역행렬을 구하여 저장한다. 카메라위치도 저장해둔다. */

#include "Base.h"

BEGIN(Engine)

class CPipeLine final : public CBase
{
public:
	enum D3DTRANSFORMSTATE { D3DTS_VIEW, D3DTS_PROJ, D3DTS_END };
private:
	CPipeLine();
	virtual ~CPipeLine() = default;

public:
	_matrix Get_Transform_Matrix(D3DTRANSFORMSTATE eState) const {
		return XMLoadFloat4x4(&m_TransformMatrices[eState]);
	}
	_matrix Get_ShadowTransform_Matrix(D3DTRANSFORMSTATE eState) const {
		return XMLoadFloat4x4(&m_ShadowCamTransformMatrices[eState]);
	}

	_float4x4 Get_Transform_Float4x4(D3DTRANSFORMSTATE eState) const {
		return m_TransformMatrices[eState];
	}

	_float4x4 Get_ShadowTransform_Float4x4(D3DTRANSFORMSTATE eState) const {
		return m_ShadowCamTransformMatrices[eState];
	}

	_matrix Get_Transform_Inverse_Matrix(D3DTRANSFORMSTATE eState) const {
		return XMLoadFloat4x4(&m_TransformMatrices_Inverse[eState]);
	}
	_matrix Get_ShadowTransform_Inverse_Matrix(D3DTRANSFORMSTATE eState) const {
		return XMLoadFloat4x4(&m_ShadowCamTransformMatrices_Inverse[eState]);
	}
	_float4x4 Get_Transform_Inverse_Float4x4(D3DTRANSFORMSTATE eState) const {
		return m_TransformMatrices_Inverse[eState];
	}
	_float4x4 Get_ShadowTransform_Inverse_Float4x4(D3DTRANSFORMSTATE eState) const {
		return m_ShadowCamTransformMatrices_Inverse[eState];
	}
	_vector Get_CamPosition_Vector() const {
		return XMLoadFloat4(&m_vCamPosition);
	}

	_float4 Get_CamPosition_Float4() const {
		return m_vCamPosition;
	}

public:
	void Set_Transform(D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix) {
		XMStoreFloat4x4(&m_TransformMatrices[eState], TransformMatrix);
	}

	void Set_ShadowTransform(D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix) {
		XMStoreFloat4x4(&m_ShadowCamTransformMatrices[eState], TransformMatrix);
	}

public:
	void Update();

private:
	_float4x4			m_TransformMatrices[D3DTS_END] = {};
	_float4x4			m_TransformMatrices_Inverse[D3DTS_END] = {};
	_float4				m_vCamPosition = {};

	_float4x4			m_ShadowCamTransformMatrices[D3DTS_END] = {};
	_float4x4			m_ShadowCamTransformMatrices_Inverse[D3DTS_END] = {};
	_float4				m_vShadowCamPosition = {};
public:
	static CPipeLine* Create();
	virtual void Free() override;
};

END