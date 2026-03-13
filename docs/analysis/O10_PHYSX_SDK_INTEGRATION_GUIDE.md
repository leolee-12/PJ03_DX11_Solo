# PhysX SDK 통합 가이드 (DX11 Framework)

> 작성: 2026-02-26 | 대상: `jusin_DX11_Framework/DX11_Framework_LEO/`

## 1. 개요

NVIDIA PhysX SDK를 DX11 Framework에 통합하기 위한 분석.

- **엔진 구조**: Engine(DLL) → EngineSDK(Inc/lib) → Client(EXE)
- **그래픽**: D3D11 (ID3D11Device + ID3D11DeviceContext)
- **수학**: DirectXMath (XMFLOAT3, XMFLOAT4X4, XMVECTOR, XMMATRIX)
- **플랫폼**: VS2022, x64/x86 모두 지원 (솔루션 설정 확인)
- **기존 충돌**: CCollider → CBounding (DirectXCollision BoundingBox/OBB/Sphere)

**핵심**: PhysX는 렌더링 API에 독립적 → DX11과 무관하게 동작. 기존 CBounding 충돌 시스템과 **병행 또는 대체** 가능.

---

## 2. 버전 선택

| 버전 | x64 지원 | x86 지원 | VS2022 | 비고 |
|------|----------|----------|--------|------|
| **PhysX 5.x** (최신 5.6.1) | O (공식 `vc17win64`) | 커스텀 프리셋 필요 | O | GitHub 오픈소스 (BSD-3) |
| PhysX 4.1.2 | O | O (NuGet 프리빌트) | 커뮤니티 패치 | 구버전, NuGet 간편 |

### 권장: **PhysX 5.x** (소스 빌드)

- DX11 프레임워크가 x64 빌드 지원 → PhysX 5.x `vc17win64` 프리셋 바로 사용 가능
- 최신 기능 (Articulation, Soft Body, GPU Rigid Body 등)
- x86 빌드도 필요하면 커스텀 프리셋 추가 (아래 참조)

### 대안: PhysX 4.1.2 (NuGet)

x86 전용이 필요하거나 빠른 프로토타이핑 시:
```
Install-Package NVIDIA.PhysX -Version 4.1.229882250
```

---

## 3. SDK 설치 (PhysX 5.x 소스 빌드)

```bash
git clone https://github.com/NVIDIA-Omniverse/PhysX.git
cd PhysX/physx
./generate_projects.bat   # → vc17win64 선택
```

VS2022로 생성된 `.sln` 열어 Debug/Release 빌드.

빌드 산출물 위치:
```
PhysX/physx/bin/win.x86_64.vc143.mt/debug/
  ├── PhysX_static_64.lib
  ├── PhysXCommon_static_64.lib
  ├── PhysXFoundation_static_64.lib
  ├── PhysXExtensions_static_64.lib
  └── PhysXCooking_static_64.lib
```

> CRT 설정: PhysX 기본은 `/MT`. Framework가 `/MDd` 사용 시 PhysX CMake에서 `NV_USE_STATIC_WINCRT=OFF` 설정 후 재빌드.

---

## 4. 프로젝트 설정

### 4-1. 파일 배치 (기존 패턴 준수)

```
Engine/
├── ThirdPartyLib/
│   └── PhysX/
│       ├── include/         ← PhysX 헤더 복사
│       └── lib/
│           ├── Debug/       ← Debug .lib 파일들
│           └── Release/     ← Release .lib 파일들
```

### 4-2. Engine vcxproj 설정

**C/C++ > 추가 포함 디렉터리:**
```
$(ProjectDir)..\..\ThirdPartyLib\PhysX\include
```

**링커 > 추가 라이브러리 디렉터리:**
```
$(ProjectDir)..\..\ThirdPartyLib\PhysX\lib\$(Configuration)
```

**링커 > 추가 종속성:**
```
PhysX_static_64.lib
PhysXCommon_static_64.lib
PhysXFoundation_static_64.lib
PhysXExtensions_static_64.lib
```

### 4-3. Engine_Defines.h에 PhysX 포함

```cpp
// Engine_Defines.h 하단에 추가
#include <PxPhysicsAPI.h>
using namespace physx;
```

---

## 5. 초기화 / 종료 코드

```cpp
// CPhysXMgr.h (Engine DLL 내부)
class ENGINE_DLL CPhysXMgr final : public CBase
{
    DECLARE_SINGLETON(CPhysXMgr)
private:
    CPhysXMgr();
    virtual ~CPhysXMgr() = default;

public:
    HRESULT Initialize();
    void    Simulate(_float fTimeDelta);

    PxPhysics* Get_Physics() { return m_pPhysics; }
    PxScene*   Get_Scene()   { return m_pScene; }

private:
    PxFoundation*          m_pFoundation  = { nullptr };
    PxPhysics*             m_pPhysics     = { nullptr };
    PxScene*               m_pScene       = { nullptr };
    PxDefaultAllocator     m_Allocator;
    PxDefaultErrorCallback m_ErrorCB;

protected:
    virtual void Free() override;
};
```

```cpp
// CPhysXMgr.cpp
IMPLEMENT_SINGLETON(CPhysXMgr)

HRESULT CPhysXMgr::Initialize()
{
    m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_Allocator, m_ErrorCB);
    NULL_CHECK_RETURN(m_pFoundation, E_FAIL);

    m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, PxTolerancesScale());
    NULL_CHECK_RETURN(m_pPhysics, E_FAIL);

    PxSceneDesc desc(m_pPhysics->getTolerancesScale());
    desc.gravity        = PxVec3(0.f, -9.81f, 0.f);
    desc.cpuDispatcher  = PxDefaultCpuDispatcherCreate(2);
    desc.filterShader   = PxDefaultSimulationFilterShader;
    m_pScene = m_pPhysics->createScene(desc);
    NULL_CHECK_RETURN(m_pScene, E_FAIL);

    return S_OK;
}

void CPhysXMgr::Simulate(_float fTimeDelta)
{
    m_pScene->simulate(fTimeDelta);
    m_pScene->fetchResults(true);
}

void CPhysXMgr::Free()
{
    // 역순 해제 (기존 CBase::Release 패턴과 동일)
    if (m_pScene)      m_pScene->release();
    if (m_pPhysics)    m_pPhysics->release();
    if (m_pFoundation) m_pFoundation->release();
}
```

---

## 6. RigidBody 컴포넌트

```cpp
// CRigidBody.h (CComponent 상속)
class ENGINE_DLL CRigidBody : public CComponent
{
private:
    CRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CRigidBody(const CRigidBody& Prototype);
    virtual ~CRigidBody() = default;

public:
    // PxTransform → CTransform 동기화 (Late_Update에서 호출)
    void Sync_Transform(class CTransform* pTransform);

private:
    PxRigidDynamic* m_pActor = { nullptr };

public:
    static CRigidBody* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ...);
    virtual CComponent* Clone(void* pArg) override;
    virtual void Free() override;
};
```

### PxVec3 ↔ XMFLOAT3 변환 유틸리티

```cpp
// Engine_Function.h에 추가
inline PxVec3 ToPxVec3(const _float3& v) { return PxVec3(v.x, v.y, v.z); }
inline _float3 ToFloat3(const PxVec3& v) { return _float3(v.x, v.y, v.z); }

inline PxMat44 ToPxMat44(const _float4x4& m) {
    return PxMat44(reinterpret_cast<const float*>(&m));  // row-major 주의
}
```

---

## 7. 게임루프 통합

### CGameInstance에 PhysX 연동

```cpp
// CGameInstance::Initialize_Engine() 내부
m_pPhysXMgr = CPhysXMgr::GetInstance();
m_pPhysXMgr->Initialize();

// CGameInstance::Update_Engine() 흐름
void CGameInstance::Update_Engine(_float fTimeDelta)
{
    // 1. Priority_Update (입력, AI)
    m_pObject_Manager->Priority_Update(fTimeDelta);

    // 2. Update (게임 로직)
    m_pObject_Manager->Update(fTimeDelta);

    // 3. PhysX 시뮬레이션 ← 여기에 삽입
    m_pPhysXMgr->Simulate(fTimeDelta);

    // 4. Late_Update (PhysX 결과 → Transform 동기화, 렌더러 등록)
    m_pObject_Manager->Late_Update(fTimeDelta);
}

// CGameInstance::Release_Engine()
CPhysXMgr::DestroyInstance();  // DECLARE_SINGLETON 패턴
```

---

## 8. 기존 충돌 시스템과의 관계

| 기능 | 기존 (CCollider/CBounding) | PhysX |
|------|---------------------------|-------|
| AABB/OBB/Sphere 교차 판정 | O | O |
| 물리 시뮬레이션 (중력, 반발) | X | O |
| 연속 충돌 감지 (CCD) | X | O |
| 관절/조인트 | X | O |
| 레이캐스트 | X (별도 구현) | O |
| 디버그 렌더링 | PrimitiveBatch | PVD (별도 뷰어) |

### 전략
- **단순 트리거 판정**: 기존 CCollider 유지 (가볍고 빠름)
- **물리 반응 필요 시**: PhysX RigidBody 사용
- 하이브리드 가능: CCollider로 1차 필터링 → PhysX로 정밀 물리

---

## 9. 주의사항

1. **CRT 일치**: Engine DLL과 PhysX lib의 런타임 라이브러리(`/MT` vs `/MD`) 반드시 통일
2. **DLL Export**: CPhysXMgr, CRigidBody에 `ENGINE_DLL` 매크로 필수
3. **해제 순서**: PhysX 객체는 Scene → Physics → Foundation 역순 (CBase::Free 패턴과 동일)
4. **행렬 변환**: DirectXMath는 row-major, PhysX는 column-major → 변환 시 Transpose 필요
5. **고정 타임스텝**: PhysX Simulate에 가변 fTimeDelta 대신 고정값(1/60) 권장 (accumulator 패턴)
6. **UpdateLib.bat**: PhysX 헤더/lib도 EngineSDK에 복사되도록 bat 수정 필요

---

## 참고 링크

- [NVIDIA-Omniverse/PhysX (GitHub)](https://github.com/NVIDIA-Omniverse/PhysX)
- [PhysX 5.3 빌드 문서](https://nvidia-omniverse.github.io/PhysX/physx/5.3.0/docs/BuildingWithPhysX.html)
- [NuGet: NVIDIA.PhysX 4.1.2](https://www.nuget.org/packages/NVIDIA.PhysX)
- [VS2022 빌드 이슈 (#331)](https://github.com/NVIDIA-Omniverse/PhysX/issues/331)
