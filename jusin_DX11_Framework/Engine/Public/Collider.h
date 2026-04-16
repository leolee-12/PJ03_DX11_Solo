#pragma once

#include "Component.h"
#include "Bounding_AABB.h"

NS_BEGIN(Engine)

class CCollider final : public CComponent
{

private:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, COLLIDER eType);
	CCollider(const CCollider& Prototype);
	virtual ~CCollider() = default;
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

private:
	COLLIDER m_eType = { COLLIDER::END };
	class CBounding* m_pBounding = { nullptr };

public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, COLLIDER eType);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;


};

NS_END