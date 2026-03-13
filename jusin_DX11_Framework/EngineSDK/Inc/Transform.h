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
		_float	fScale = { 1.f };
		_float	fSpeedPerSec = {};
		_float	fRotationPerSec = {};
	}TRANSFORM_DESC;

protected:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& Prototype);
	virtual ~CTransform() = default;

public:
	virtual HRESULT	Initialize_Prototype();
	virtual HRESULT	Initialize(void* pArg);

private:
	_float4x4	m_WorldMatrix = {};
	_float		m_fSpeedPerSec = {};
	_float		m_fRotationPerSec = {};


public:
	static CTransform*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent*	Clone(void* pArg) override;

protected:
	virtual void		Free() override;
};

NS_END