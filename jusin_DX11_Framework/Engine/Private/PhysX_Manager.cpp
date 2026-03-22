#include "PhysX_Manager.h"
using namespace physx;

HRESULT CPhysX_Manager::Initialize()
{
    m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_Allocator, m_ErrorCB);
    NULL_CHECK_RETURN(m_pFoundation, E_FAIL);

    m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, PxTolerancesScale());
    NULL_CHECK_RETURN(m_pPhysics, E_FAIL);

    PxSceneDesc desc(m_pPhysics->getTolerancesScale());
    desc.gravity = PxVec3(0.f, -9.81f, 0.f);
    desc.cpuDispatcher = PxDefaultCpuDispatcherCreate(2);
    desc.filterShader = PxDefaultSimulationFilterShader;
    m_pScene = m_pPhysics->createScene(desc);
    NULL_CHECK_RETURN(m_pScene, E_FAIL);

    return S_OK;
}

void CPhysX_Manager::Simulate(_float fTimeDelta)
{
    m_pScene->simulate(fTimeDelta);
    m_pScene->fetchResults(true);
}

void CPhysX_Manager::Free()
{
    // 역순 해제
    if (m_pScene) m_pScene->release();
    if (m_pPhysics) m_pPhysics->release();
    if (m_pFoundation) m_pFoundation->release();
}