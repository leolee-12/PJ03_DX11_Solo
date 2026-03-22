#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CPhysX_Manager final : public CBase
{
private:
	CPhysX_Manager();
	virtual ~CPhysX_Manager() = default;

public:
	HRESULT Initialize();
	void Simulate(_float fTimeDelta);

	physx::PxPhysics* Get_Physics() { return m_pPhysics; }
	physx::PxScene* Get_Scene() { return m_pScene; }

private:
	physx::PxFoundation* m_pFoundation = { nullptr };
	physx::PxPhysics* m_pPhysics = { nullptr };
	physx::PxScene* m_pScene = { nullptr };
	physx::PxDefaultAllocator m_Allocator;
	physx::PxDefaultErrorCallback m_ErrorCB;

public:
	static CPhysX_Manager* Create();

protected:
	virtual void Free() override;
};

NS_END