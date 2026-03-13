# 참고프로젝트3 — 오브젝트/컴포넌트 시스템 심화 분석

> **분석 대상**: CBase, CComponent, CResources, CGameObject, CTransform, CLayer, CScene, CManagement, CComponent_Manager, CObject_Manager + 유틸(Engine_Function, Engine_Functor, Engine_Macro)
> **모드**: [분석]

---

## 1. 핵심 책임과 시스템 경계

### 이 시스템이 담당하는 것
- **레퍼런스 카운팅 수명 관리**: CBase의 `Add_Ref()`/`Release()` → 모든 엔진 객체의 수명 제어
- **컴포넌트 프로토타입 관리**: Component_Manager에 원본 등록 → `Clone()`으로 인스턴스 생성
- **오브젝트 계층 구조**: Scene → Object_Manager → Layer(이름 기반) → GameObject 리스트
- **프레임 루프 진행**: Management가 Scene::Update → Object_Manager::Update → Layer::Update → 각 GameObject::Update 순회
- **월드 행렬 계산**: Transform의 SRT 조합 → 부모 행렬 곱 → 셰이더/디바이스에 전달

### 시스템 경계 — 이것은 하지 않는다
- **렌더링**: Renderer가 별도 처리. 오브젝트는 `Add_RenderList()`로 등록만
- **충돌**: CollisionManager/NavMgr가 별도 처리
- **입력**: InputDev를 각 오브젝트가 직접 호출
- **리소스 로드**: Mesh/Texture가 자체적으로 DX 리소스 생성

---

## 2. 클래스 간 소유/참조 관계

```
CBase (레퍼런스 카운팅 루트)
│   m_dwRefCnt, Add_Ref(), Release() → Free() → delete this
│
├── CComponent (컴포넌트 베이스)
│   │   순수 가상: Clone(), Free()
│   │   enum: COM_STATIC(변경 불가), COM_DYNAMIC(변경 가능)
│   │
│   ├── CTransform
│   │   ├── 소유: m_pGraphicDev (AddRef)
│   │   ├── 값: m_vInformation[3] (Scale, Angle, Position)
│   │   ├── 값: m_matWorld (월드 행렬)
│   │   └── 참조: m_pParentMatrix (부모 행렬 포인터, 소유 안 함)
│   │
│   └── CResources (리소스 베이스)
│       └── 소유: m_pGraphicDev (AddRef)
│           └── [Texture, VIBuffer, Mesh 등이 상속]
│
├── CGameObject (엔티티 베이스)
│   ├── 소유: m_pGraphicDev (AddRef)
│   ├── 소유: m_pTransformCom → CTransform* (별도 참조 유지)
│   ├── 참조: m_pComponentMgr → CComponent_Manager* (AddRef)
│   ├── 소유: m_mapComponent[COM_END] → map<tchar*, CComponent*>
│   │         STATIC 맵 + DYNAMIC 맵, 각 컴포넌트를 소유
│   └── 값: m_fViewZ (카메라 거리), m_bDead (제거 플래그)
│
├── CLayer (오브젝트 리스트 컨테이너)
│   ├── 소유: m_ObjectList → list<CGameObject*> (각 오브젝트 소유)
│   └── 값: m_bFirstUpdate (최초 업데이트 플래그)
│
├── CScene (씬 베이스)
│   ├── 소유: m_pGraphicDev (AddRef)
│   ├── 참조: m_pComponentMgr → CComponent_Manager* (AddRef)
│   └── 참조: m_pObjectMgr → CObject_Manager* (AddRef)
│
├── CManagement (싱글톤 — 게임 루프 오케스트레이터)
│   ├── 소유: m_pCurrentScene → CScene*
│   ├── 참조: m_pRenderer → CRenderer* (Clone으로 획득)
│   └── 값: m_bAfterFirstRender
│
├── CComponent_Manager (싱글톤 — 프로토타입 저장소)
│   ├── 소유: m_pMapComponent → MAPCOMPONENT[씬 개수] (동적 배열)
│   │         각 맵이 <태그, CComponent*> 프로토타입 보유
│   └── 값: m_iContainerSize (씬 개수)
│
└── CObject_Manager (싱글톤 — 오브젝트/레이어 관리)
    ├── 소유: m_pMapLayer → MAPLAYER[씬 개수] (동적 배열)
    │         각 맵이 <태그, CLayer*> 보유
    └── 값: m_iMaxContainerSize (씬 개수)
```

### 소유 원칙 요약
| 관계 | 방식 |
|------|------|
| DX 디바이스 참조 | 생성자에서 `AddRef()`, `Free()`에서 `Safe_Release()` |
| 싱글톤 매니저 참조 | `GetInstance()` + `Add_Ref()`, `Free()`에서 `Safe_Release()` |
| 컴포넌트 소유 | GameObject의 `m_mapComponent`에 저장, `Free()`에서 전체 Release |
| 오브젝트 소유 | Layer의 `m_ObjectList`에 저장, `Free()`에서 `CRelease_Single` |
| 씬 전환 시 | Management가 이전 씬 `Safe_Release()` → 새 씬 포인터 저장 |

---

## 3. 한 프레임 호출 흐름

```
CMainApp::Update_MainApp(fTimeDelta)
│
├─ CManagement::Update_Management(fTimeDelta)
│   └─ CScene::Update_Scene(fTimeDelta)
│       └─ CObject_Manager::Update_ObjMgr(fTimeDelta)
│           └─ 씬 인덱스별 순회
│               └─ 레이어별 순회
│                   └─ CLayer::Update_Layer(fTimeDelta)
│                       │
│                       ├─ [최초 프레임만] FirstUpdate_GameObject(fTimeDelta)
│                       │   └─ 각 오브젝트의 초기화 로직 (1회)
│                       │
│                       └─ [매 프레임] Update_GameObject(fTimeDelta)
│                           │
│                           ├─ CGameObject::Update_GameObject()  ← 베이스
│                           │   └─ COM_DYNAMIC 컴포넌트 전체 Update_Component()
│                           │       └─ CTransform::Update_Component()
│                           │           SRT 행렬 조합 → 부모 행렬 곱 → m_matWorld 갱신
│                           │
│                           ├─ [하위 클래스] 게임 로직 (입력, AI, 물리 등)
│                           │   └─ Renderer::Add_RenderList(RENDER_TYPE, this)
│                           │
│                           └─ exitCode == 1이면 → Safe_Release + erase (오브젝트 제거)
│
├─ CManagement::Render_Management()
│   ├─ CRenderer::Render_GameObject()  ← Deferred 파이프라인
│   ├─ [최초만] CScene::FirstRender_Finish()
│   └─ CScene::Render_Scene()
```

### 오브젝트 제거 메커니즘
```cpp
// Layer::Update_Layer()
iExitCode = (*iter_begin)->Update_GameObject(fTimeDelta);
if (1 == iExitCode)  // "죽었다" 시그널
{
    Safe_Release(*iter_begin);           // 레퍼런스 카운트 감소 → 0이면 삭제
    iter_begin = m_ObjectList.erase(iter_begin);  // 리스트에서 제거
}
```
- 오브젝트가 `Update`에서 1을 반환하면 Layer가 즉시 제거
- `Safe_Release`로 해제 → 다른 곳에서 참조 중이면 아직 살아있음

---

## 4. 프로토타입 패턴 상세

### 등록 (씬 초기화 시)
```
Scene::Ready_Scene()
  └─ Component_Manager::Add_Component(씬ID, "Proto_Transform", CTransform::Create(dev))
  └─ Component_Manager::Add_Component(씬ID, "Proto_DynMesh_Player", CDynamicMesh::Create(...))
```

### 복제 (오브젝트 생성 시)
```
GameObject::Add_Component()  [하위 클래스에서 구현]
  └─ m_pComponentMgr->Clone_Component(씬ID, "Proto_Transform")
      └─ Find_Component() → pProto->Clone()
          └─ new CTransform(*this)  [복사 생성자]
```

### Clone의 두 가지 유형

| 유형 | 방식 | 예시 |
|------|------|------|
| **진짜 복제** | `new T(rhs)` (복사 생성자) | CTransform, CShader, CTexture |
| **공유 (유사 싱글톤)** | `AddRef(); return this;` | CRenderer |

CTransform 복사 생성자:
```cpp
CTransform(const CTransform& rhs)
    : m_pGraphicDev(rhs.m_pGraphicDev)    // 디바이스 공유 (AddRef)
    , m_matWorld(rhs.m_matWorld)           // 값 복사
    , m_pParentMatrix(rhs.m_pParentMatrix) // 포인터 복사
{
    m_pGraphicDev->AddRef();
    memcpy(m_vInformation, rhs.m_vInformation, sizeof(_vec3) * INFO_END);
}
```

### 씬별 프로토타입 분리
- `m_pMapComponent`가 **배열**(씬 개수만큼) → 씬마다 독립 프로토타입 풀
- `Release_Component(씬ID)` → 해당 씬 프로토타입만 해제
- 씬 전환 시 이전 씬 프로토타입 정리 + 새 씬 프로토타입 등록

---

## 5. 사용된 디자인 패턴

### 5-1. 레퍼런스 카운팅 (CBase)
```cpp
unsigned long Release(void) {
    if (0 == m_dwRefCnt) {
        Free();          // 하위 클래스 정리
        delete this;     // 자기 삭제
        return 0;
    }
    else return m_dwRefCnt--;  // ⚠️ 후위 감소
}
```
- COM 스타일 수명 관리. `delete` 직접 호출 금지
- **주의**: `m_dwRefCnt--`는 후위 감소 → 반환값은 감소 **전** 값

### 5-2. 프로토타입 (Component_Manager + Clone)
- 원본(프로토타입)을 매니저에 등록 → `Clone()`으로 인스턴스 생산
- `new` 직접 사용 대신 팩토리 + 프로토타입 조합

### 5-3. 싱글톤 (매크로 기반)
```cpp
// DECLARE_SINGLETON: static 포인터 + GetInstance + DestroyInstance 선언
// IMPLEMENT_SINGLETON: 구현 — nullptr이면 new, DestroyInstance는 Release 호출
```
- `DestroyInstance()` → `m_pInstance->Release()` → refCnt 0이면 삭제
- 다른 곳에서 `Add_Ref()` 중이면 `DestroyInstance()` 호출해도 살아있음

### 5-4. 컴포지트 (Scene → Object_Manager → Layer → GameObject)
- 4단계 계층 구조. Update가 최상위에서 최하위로 전파
- 각 레벨이 자식을 소유하고 순회

### 5-5. 팩토리 메서드 (`static Create()`)
- 모든 엔진 클래스가 `Create()` → `new` + `Ready` → 실패 시 `Safe_Release`
- 생성자 private → Create만 허용하여 초기화 보장

### 5-6. 템플릿 펑터 (Engine_Functor.h)
- `CRelease_Single`: 단일 포인터 Release (list용 `for_each`)
- `CRelease_Pair`: pair의 second Release (map용 `for_each`)
- `CTag_Finder`: `lstrcmp` 기반 태그 검색 (`find_if`용)

---

## 6. DirectX API 호출 지점과 래핑 방식

### CTransform — 유일한 DX 직접 호출 지점

| DX9 API | CTransform 메서드 | 용도 |
|---------|-----------------|------|
| `D3DXMatrixScaling/RotationX/Y/Z/Translation` | `Update_Component()` | SRT 행렬 생성 |
| `D3DXMatrixIdentity` | `Ready_Component()` | 월드 행렬 초기화 |
| `SetTransform(D3DTS_WORLD)` | `SetUp_OnGraphicDev()` | 고정 파이프라인에 월드 행렬 세팅 |
| `pEffect->SetMatrix()` | `SetUp_OnShader()` | 셰이더에 행렬 전달 |
| `D3DXMatrixMultiply` | `SetUp_OnShader()` | WVP 행렬 미리 계산 |
| `D3DXVec3Normalize` | `Go_Straight/Right` | 이동 방향 정규화 |

### CGameObject — 간접 DX 호출
```cpp
void Compute_ViewZ(const D3DXVECTOR3* pPos) {
    m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);  // View 행렬 읽기
    D3DXMatrixInverse(&matView, NULL, &matView);         // 역변환 → 카메라 위치
    m_fViewZ = D3DXVec3Length(&(vCamPos - *pPos));       // 카메라-오브젝트 거리
}
```
- 알파 정렬용 ViewZ 계산. 매 프레임 호출.

### CResources — DX 디바이스 보관만
- `m_pGraphicDev`를 `AddRef`하여 보유, 하위 클래스(Texture, Mesh 등)가 사용

### 나머지 클래스 — DX 호출 없음
- CBase, CComponent, CLayer, CScene, CManagement, Component_Manager, Object_Manager
- 순수 C++ 로직만 사용

---

## 7. 설계 판단 분석 — 참고할 점과 한계

### 참고할 점

**1) COM_STATIC / COM_DYNAMIC 분류**
- 컴포넌트를 변경 가능/불가로 이분
- `Update_GameObject()`에서 **COM_DYNAMIC만 순회** → 불필요한 Update 호출 제거
- Texture, Mesh = STATIC (한 번 세팅 후 변경 없음), Transform = DYNAMIC (매 프레임 갱신)

**2) 씬별 프로토타입/오브젝트 풀 분리**
- Component_Manager, Object_Manager 모두 **씬 인덱스 배열** 사용
- `Release_Component(씬ID)` / `Release_GameObject(씬ID)` → 씬 전환 시 해당 씬 리소스만 정리
- 다른 씬의 리소스는 유지 가능 (공유 리소스 씬 0번 등)

**3) Layer의 FirstUpdate 패턴**
```cpp
if (FALSE == m_bFirstUpdate) {
    m_bFirstUpdate = TRUE;
    for (...) (*iter)->FirstUpdate_GameObject(fTimeDelta);
}
```
- 오브젝트 추가 후 첫 프레임에만 `FirstUpdate` 호출 → 1회성 초기화
- 이후 프레임은 `Update_GameObject`만 호출
- 초기화 타이밍 문제(Ready에서 다른 오브젝트 참조 불가)를 해결

**4) exitCode 기반 오브젝트 제거**
- `Update_GameObject`가 1 반환 → Layer가 즉시 erase + Release
- 별도 "죽은 오브젝트 큐" 없이 순회 중 즉시 제거
- `0x80000000` 비트 체크 → 심각한 에러 시 상위로 전파

**5) Transform의 SetUp_OnShader 오버로드**
```cpp
// 기본: 월드 행렬만 전달
void SetUp_OnShader(pEffect, "g_matWorld");

// WVP 미리 곱: CPU에서 한 번 계산 → GPU 부담 감소
void SetUp_OnShader(pEffect, "g_matWVP", matView, matProj);
```
- 상황에 따라 GPU/CPU 부하 분배를 선택할 수 있는 유연한 설계

**6) Safe_Release 템플릿 — nullptr 자동 처리**
```cpp
template <typename TC> unsigned long Safe_Release(TC& pointer) {
    if (nullptr != pointer) {
        dwRefCnt = pointer->Release();
        if (0 == dwRefCnt) pointer = nullptr;  // 해제 후 자동 null
    }
}
```
- 댕글링 포인터 방지. Release 후 자동으로 nullptr 세팅.

### 한계/개선 가능 포인트

**1) CBase::Release()의 후위 감소 버그**
```cpp
if (0 == m_dwRefCnt) { Free(); delete this; return 0; }
else return m_dwRefCnt--;  // 후위 감소: 1이면 1 반환하고 0으로 감소
```
- `m_dwRefCnt`가 1일 때: 반환값은 1이지만 실제 카운트는 0 → **다음 Release에서 삭제**
- 결과: refCnt 초기값 0에서 시작 + 한 번도 AddRef 안 하면 첫 Release에서 바로 삭제 (정상)
- 하지만 반환값이 실제 남은 카운트와 **1 차이** → 호출자가 혼란할 수 있음

**2) Component_Manager의 문자열 키 검색**
- `find_if` + `CTag_Finder`(lstrcmp) → O(N) 선형 탐색
- `unordered_map`이지만 `const _tchar*` 포인터를 키로 사용 → 해시 아닌 주소 비교
- 결과: 같은 문자열이라도 주소가 다르면 못 찾음 → `find`가 아닌 `find_if`로 우회

**3) CGameObject가 Component_Manager를 직접 참조**
- 모든 오브젝트가 `m_pComponentMgr` 보유 (AddRef)
- `Add_Component()` 호출 시 편리하지만, 오브젝트 ↔ 매니저 간 결합도 증가
- 개선: 씬이나 팩토리에서 컴포넌트를 주입하는 방식

**4) Transform이 NavMgr에 직접 의존**
```cpp
DWORD Go_Straight(const _float& fSpeedTimeDelta, ...) {
    return CNavMgr::GetInstance()->MoveOnNavMesh(&m_vInformation[INFO_POSITION], ...);
}
```
- 컴포넌트가 네비게이션 시스템 싱글톤에 직접 의존 → 재사용성 저하
- 개선: 이동 결과를 NavMgr가 아닌 별도 MovementComponent에서 처리

**5) m_pParentMatrix — 원시 포인터로 부모 참조**
```cpp
void Set_ParentMatrix(_matrix* pParentMatrix) {
    m_pParentMatrix = pParentMatrix;  // AddRef 없이 단순 저장
}
```
- 부모가 먼저 삭제되면 댕글링 포인터 → 크래시 위험
- 이 프로젝트에서는 부모가 항상 자식보다 오래 살기 때문에 문제없지만, 일반화 시 주의
