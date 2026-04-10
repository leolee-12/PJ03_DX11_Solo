#pragma once
#include "Component.h"

/* -------------------------------------------------- */
// 트랜스폼 컴포넌트
// - 객체의 월드 상태를 표현해주는 월드변환행렬을 보관
// - 월드행렬의 상태 표현을 위한 여러 인터페이스를 제공
/* -------------------------------------------------- */

NS_BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	typedef struct tagTransformDesc
	{
		_float	fSpeedPerSec = {};
		_float	fRotationPerSec = {};
	}TRANSFORM_DESC;

protected:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& Prototype);
	virtual ~CTransform() = default;

public:
	_vector Get_State(STATE eState) const
	{
		return XMLoadFloat4(reinterpret_cast
			<const _float4*>(&m_WorldMatrix.m[ETOUI(eState)]));
	}

	void XM_CALLCONV Set_State(STATE eState, _fvector vState)
	{
		XMStoreFloat4(reinterpret_cast<_float4*>
			(&m_WorldMatrix.m[ETOUI(eState)]), vState);
	}
	
	_float3 Get_Scaled() const
	{
		return _float3(	XMVectorGetX(XMVector3Length(Get_State(STATE::RIGHT))),
						XMVectorGetX(XMVector3Length(Get_State(STATE::UP))),
						XMVectorGetX(XMVector3Length(Get_State(STATE::LOOK))));
	}

	const _float4x4* Get_WorldMatrixPtr() const { return &m_WorldMatrix; }

public:
	virtual HRESULT	Initialize_Prototype();
	virtual HRESULT	Initialize(void* pArg);

public:
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstName);
	HRESULT Bind_ShaderResourceWIT(class CShader* pShader, const _char* pConstName);
	HRESULT XM_CALLCONV Bind_ShaderResourceCombinedWIT(class CShader* pShader, const _char* pConstName, _fmatrix CombinedMatrix);
	
public:
	void ScaleTo(_float fScaleX = 1.f, _float fScaleY = 1.f, _float fScaleZ = 1.f);
	void Scaling(_float fScaleX = 1.f, _float fScaleY = 1.f, _float fScaleZ = 1.f);

	void XM_CALLCONV Rotation(_fvector vAxis, _float fRadian);
	void XM_CALLCONV Turn(_fvector vAxis, _float fTimeDelta);
	void XM_CALLCONV LookAt(_fvector vFocus);

	void Go_Straight(_float fTimeDelta);
	void Go_Backward(_float fTimeDelta);
	void Go_Left(_float fTimeDelta);
	void Go_Right(_float fTimeDelta);

private:
	_float4x4 m_WorldMatrix = {};
	_float m_fSpeedPerSec = {};
	_float m_fRotationPerSec = {};


public:
	static CTransform*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent*	Clone(void* pArg) override;

protected:
	virtual void		Free() override;
};

NS_END