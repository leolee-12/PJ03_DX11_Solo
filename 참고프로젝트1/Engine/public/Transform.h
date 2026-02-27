#pragma once

#include "Component.h"

/* 객체의 월드 변환을 위한 정보를 가진다.(월드행렬) */
/* 월드 상태 변환을 위한 기능을 제공한다.(Go, Back, Turn, Chase) */

NS_BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	typedef struct tagTransformDesc 
	{
		_float		fSpeedPerSec{}, fRotationPerSec{};
	}TRANSFORM_DESC;
private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);	
	virtual ~CTransform() = default;

public:	
	_vector Get_State(STATE eState) const {
		return XMLoadFloat4x4(&m_WorldMatrix).r[ENUM_CLASS(eState)];
	} 

	const _float4x4* Get_WorldMatrix_Ptr() const {
		return &m_WorldMatrix;
	}

	_float3 Get_Scaled() const {
		return _float3(
			XMVectorGetX(XMVector3Length(Get_State(STATE::RIGHT))), 
			XMVectorGetX(XMVector3Length(Get_State(STATE::UP))),
			XMVectorGetX(XMVector3Length(Get_State(STATE::LOOK)))
		);
	}

	_matrix Get_WorldMatrix_Inverse() {
		return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix));
	}
	void Set_State(STATE eState, _fvector vState) {
		XMStoreFloat4(reinterpret_cast<_float4*>(&m_WorldMatrix.m[ENUM_CLASS(eState)]),
			vState);
	}

	void Set_Scale(_float fSizeX = 1.f, _float fSizeY = 1.f, _float fSizeZ = 1.f);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName);

public:
	void Go_Straight(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void Go_Left(_float fTimeDelta);
	void Go_Right(_float fTimeDelta);
	void Go_Backward(_float fTimeDelta);

	/* 항등상태 기준으로 정해준 Radian만큼 회전한다.  */
	void Rotation(_fvector vAxis, _float fRadian);
	void Rotation(_float fRotX, _float fRotY, _float fRotZ);

	/* 현재상태 기준으로 가지고 있떤 속도만큼 회전한다. */
	void Turn(_fvector vAxis, _float fTimeDelta);

	void LookAt(_fvector vFocus);
	void Chase(_fvector vDest, _float fTimeDelta, _float fLimitDistance = 0.f);

private:
	_float4x4					m_WorldMatrix = { };
	_float						m_fSpeedPerSec = {};
	_float						m_fRotationPerSec = {};

public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) { return nullptr; }
	virtual void Free() override;
};

NS_END