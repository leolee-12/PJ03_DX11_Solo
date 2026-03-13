# 참고프로젝트2 — 오브젝트/컴포넌트 시스템 심화 분석

> 프로토타입 Clone + 레퍼런스 카운팅 기반 엔티티-컴포넌트 아키텍처

---

## 1. 핵심 책임과 경계

### 이 시스템이 담당하는 것
- **오브젝트 생명주기**: 프로토타입 등록 → Clone 생성 → 레이어 배치 → 갱신 → 예약 삭제
- **컴포넌트 조립**: 프로토타입 저장소에서 Clone하여 오브젝트에 부착
- **계층 구조**: Container–Part 부모-자식 관계
- **공간 변환**: CTransform으로 월드 행렬 관리 및 셰이더 바인딩
- **레벨별 리소스 격리**: 레벨 인덱스로 프로토타입/레이어 분리

### 이 시스템이 담당하지 않는 것
- 렌더링 파이프라인 (Renderer DLL)
- 충돌 검사 로직 (CCollider_Manager)
- 씬 전환 로직 (CLevel_Manager)

---

## 2. 클래스 간 소유/참조 관계

```
CGameInstance (Singleton, 파사드)
├── [소유] CObject_Manager     ← 오브젝트 프로토타입 + 레이어
├── [소유] CComponent_Manager  ← 컴포넌트 프로토타입
└── [소유] 기타 매니저들...

CObject_Manager
├── [소유] map<wstring, CGameObject*>  m_Prototypes   ← 전역 프로토타입 (레벨 무관)
├── [소유] map<wstring, CLayer*>[]     m_pLayers      ← 레벨별 레이어 배열
└── [임시] list<CGameObject*>          m_DestoryObjects ← 삭제 예약 큐

CComponent_Manager
└── [소유] map<wstring, CComponent*>[] m_pPrototypes  ← 레벨별 컴포넌트 프로토타입

CLayer
└── [소유] list<CGameObject*>  m_GameObjects  ← 오브젝트 리스트

CGameObject (추상)
├── [참조] ID3D11Device*, ID3D11DeviceContext*  ← AddRef로 공유
├── [참조] CGameInstance*, CRenderInstance*     ← AddRef로 공유
├── [소유] CTransform*  m_pTransformCom         ← 자동 생성 (Initialize 시)
└── [소유] map<wstring, CComponent*>  m_Components ← 태그 기반 컴포넌트 맵

CContainerObject : CGameObject
└── [소유] vector<CPartObject*>  m_Parts  ← 파트 오브젝트 배열

CPartObject : CGameObject
├── [참조] const _float4x4*  m_pParentMatrix  ← 부모 월드 행렬 (포인터)
└── [계산] _float4x4  m_WorldMatrix            ← 자신 Transform × 부모 행렬

CComponent (추상)
├── [참조] ID3D11Device*, ID3D11DeviceContext*
├── [참조] CGameInstance*
└── [플래그] m_isCloned  ← 복제본 여부 (Clone 시 true)

CTransform : CComponent
└── [소유] _float4x4  m_WorldMatrix  ← 월드 변환 행렬
```

### 소유권 규칙 요약

| 관계 | 소유 방식 |
|------|-----------|
| Manager → 프로토타입 | AddRef로 소유, Free()에서 Release |
| Manager → Layer | Layer::Create()로 생성, Safe_Release로 해제 |
| Layer → GameObject | Clone 반환값을 직접 보관, Safe_Release로 해제 |
| GameObject → Component | Clone 후 emplace + AddRef, Free에서 Release |
| Container → Part | Clone_GameObject 후 직접 보관, Free에서 Release |
| Part → 부모 행렬 | **const 포인터 참조만** (소유하지 않음) |

---

## 3. 주요 함수의 호출 흐름

### 3.1 프로토타입 등록 (초기화 단계)

```
CLevel_GamePlay::Ready_Scene() 또는 CMainApp::Initialize()
│
├── [오브젝트 프로토타입 등록]
│   CGameInstance::Add_Prototype(L"Proto_Player", CPlay_Goku::Create(pDevice, pContext))
│     └── CObject_Manager::Add_Prototype()
│           ├── Find_Prototype() → 중복 확인
│           ├── 없으면: m_Prototypes.emplace(tag, pPrototype)
│           └── 있으면: Safe_Release(pPrototype)  ← 중복 프로토타입 해제
│
└── [컴포넌트 프로토타입 등록]
    CGameInstance::Add_Prototype(LEVEL_STATIC, L"Proto_Shader_Model", CShader::Create(...))
      └── CComponent_Manager::Add_Prototype(iLevelIndex, tag, pPrototype)
            ├── Find_Prototype() → 중복 시 E_FAIL + MessageBox
            └── m_pPrototypes[iLevelIndex].emplace(tag, pPrototype)
```

### 3.2 오브젝트 생성 및 레이어 배치

```
CGameInstance::Add_GameObject_ToLayer(LEVEL_GAMEPLAY, L"Proto_Player", L"Layer_Player", &desc)
│
└── CObject_Manager::Add_GameObject_ToLayer()
      ├── Find_Prototype(L"Proto_Player")  ← 프로토타입 검색
      ├── pPrototype->Clone(pArg)          ← [핵심] 복제 생성
      │     └── CPlay_Goku(const CPlay_Goku& Prototype)  ← 복사 생성자
      │           ├── CGameObject 복사 생성자: Device/Context/GameInstance/RenderInstance AddRef
      │           └── Initialize(pArg)
      │                 ├── CGameObject::Initialize(pArg)
      │                 │     ├── CTransform::Create() → m_pTransformCom  ← 자동 Transform 생성
      │                 │     ├── pArg가 FILEDATA면 → 위치/회전/스케일 적용
      │                 │     └── m_Components.emplace("Com_Transform", m_pTransformCom)
      │                 │
      │                 └── Add_Component()로 추가 컴포넌트 Clone
      │                       ├── CGameInstance::Clone_Component(LEVEL_STATIC, L"Proto_Shader_Model")
      │                       │     └── CComponent_Manager::Clone_Component()
      │                       │           ├── Find_Prototype() → CShader 프로토타입
      │                       │           └── pPrototype->Clone(pArg) → CShader 복제본
      │                       ├── m_Components.emplace(tag, pComponent)
      │                       └── Safe_AddRef(pComponent) + *ppOut = pComponent
      │
      ├── Find_Layer(iLevelIndex, L"Layer_Player")
      │     ├── 없으면: CLayer::Create() → m_pLayers[level].emplace(tag, pLayer)
      │     └── 있으면: 기존 레이어 사용
      └── pLayer->Add_GameObject(pGameObject)  ← 리스트에 추가
```

### 3.3 한 프레임 갱신 흐름

```
CGameInstance::Update_Engine(fTimeDelta)
│
├── [1] CObject_Manager::Destory_Update()
│         ← 이전 프레임에 예약된 오브젝트의 m_bDead = true 설정
│
├── [2] CObject_Manager::Player_Update(fTimeDelta)
│         └── 모든 레벨, 모든 레이어 순회
│               └── CLayer::Player_Update()
│                     ├── m_bDead인 오브젝트 → list에서 erase (메모리 해제 아님)
│                     ├── !m_bIsActive → skip
│                     └── pGameObject->Player_Update()  ← 입력 처리
│
├── [3] CObject_Manager::Update(fTimeDelta)
│         └── CLayer::Update()
│               ├── m_bDead → Safe_Release(*it) + erase  ← [여기서 실제 해제]
│               ├── !m_bIsActive → skip
│               └── pGameObject->Update()  ← 로직 갱신
│
├── [4] CCollider_Manager::Update()  ← 충돌 검사
│
├── [5] CObject_Manager::Late_Update(fTimeDelta)
│         └── CLayer::Late_Update()
│               └── pGameObject->Late_Update()
│                     └── 렌더 큐 등록: CRenderInstance::Add_RenderObject(...)
│
├── [6] CObject_Manager::Camera_Update(fTimeDelta)
│         └── pGameObject->Camera_Update()  ← 카메라 갱신
│
└── [7] CPipeLine::Update() + CFrustum::Update()
```

### 3.4 Container-Part 갱신 흐름

```
CContainerObject::Update(fTimeDelta)
├── __super::Update()           ← CGameObject::Update()
└── for each Part:
      └── pPartObject->Update()
            └── __super::Update()

CContainerObject::Late_Update(fTimeDelta)
├── __super::Late_Update()
└── for each Part:
      └── pPartObject->Late_Update()
            └── __super::Late_Update()
                  └── m_WorldMatrix = Transform.WorldMatrix × *m_pParentMatrix
                        ← [핵심] 부모 행렬과 곱하여 최종 월드 행렬 계산
```

### 3.5 오브젝트 삭제 흐름 (지연 삭제)

```
[게임 로직에서]
pGameObject->Destory()
  ├── m_pGameInstance->Destory_Reserve(this)  ← 삭제 예약 큐에 추가
  └── SetActive(false)                        ← 즉시 비활성화

[다음 프레임 시작 시]
CObject_Manager::Destory_Update()
  └── iter->m_bDead = true                    ← Dead 플래그 설정

[같은 프레임 Update 단계]
CLayer::Update()
  └── m_bDead 확인 → Safe_Release + erase    ← 실제 메모리 해제
```

---

## 4. 사용된 디자인 패턴

### 4.1 프로토타입 패턴

```cpp
// 프로토타입 등록 (한 번)
CGameInstance::Add_Prototype(L"Proto_Player", CPlay_Goku::Create(pDevice, pContext));

// Clone으로 복제 생성 (여러 번)
CGameObject* pClone = pPrototype->Clone(pArg);
```

- 모든 `CGameObject`와 `CComponent`가 `Clone()` 순수 가상 함수 보유
- 프로토타입은 `Initialize_Prototype()`으로 초기화, Clone은 `Initialize(pArg)`로 초기화
- **프로토타입 저장소가 2개**: 오브젝트(전역) vs 컴포넌트(레벨별)

### 4.2 레퍼런스 카운팅

```cpp
// CBase — 모든 클래스의 루트
_uint CBase::AddRef()  { return ++m_iRefCnt; }
_uint CBase::Release() {
    if (0 == m_iRefCnt) {
        Free();           // 가상 소멸자 대체
        m_bDead = true;
        delete this;
        return 0;
    }
    return m_iRefCnt--;   // 주의: post-decrement
}
```

- `new`로 생성 시 RefCnt = 0 → 첫 Release에서 즉시 삭제
- 공유 시 `Safe_AddRef()` 필수 → 해제 시 `Safe_Release()`
- Component를 GameObject에 추가할 때: emplace + Safe_AddRef → 이중 참조

### 4.3 싱글톤 + 파사드

```cpp
// 매크로로 정의
DECLARE_SINGLETON(CGameInstance)
IMPLEMENT_SINGLETON(CGameInstance)

// 파사드 패턴: 모든 매니저를 단일 인터페이스로 노출
CGameInstance::Add_Prototype()           → m_pObject_Manager->Add_Prototype()
CGameInstance::Add_GameObject_ToLayer()  → m_pObject_Manager->Add_GameObject_ToLayer()
CGameInstance::Clone_Component()         → m_pComponent_Manager->Clone_Component()
```

- `CGameInstance`가 13개 매니저를 소유하고 위임
- 클라이언트 코드는 `CGameInstance`만 알면 됨

### 4.4 컴포넌트 패턴

```cpp
// CGameObject 내부
map<const _wstring, CComponent*>  m_Components;

// 컴포넌트 추가
Add_Component(LEVEL_STATIC, L"Proto_Model_Goku", L"Com_Model", &m_pModelCom);
Add_Component(LEVEL_STATIC, L"Proto_Shader_Model", L"Com_Shader", &m_pShaderCom);

// 컴포넌트 검색
CComponent* pComp = pGameObject->Get_Component(L"Com_Model");
```

- `map<wstring, CComponent*>`로 태그 기반 조회
- `CTransform`은 `"Com_Transform"` 태그로 자동 등록 (Initialize 시)
- 컴포넌트 검색이 레이어 → 인덱스 → 태그 3단계로 가능

### 4.5 지연 삭제 패턴

```cpp
// 예약
pGameObject->Destory()  → Destory_Reserve(this) + SetActive(false)

// 처리 (다음 프레임 시작)
Destory_Update() → m_bDead = true

// 실제 해제 (같은 프레임 Update)
Layer::Update() → if (m_bDead) Safe_Release + erase
```

- **즉시 삭제를 피하는 이유**: 순회 중 삭제로 인한 이터레이터 무효화 방지
- Dead 플래그 설정과 실제 해제를 분리 → 안전한 2단계 삭제

### 4.6 Container-Part 복합 패턴

```cpp
// Container가 Part를 프로토타입에서 Clone
CContainerObject::Add_PartObject(iPartIndex, L"Proto_Weapon", pArg)
  └── Clone_GameObject(tag, pArg) → dynamic_cast<CPartObject*>

// Part는 부모 행렬 포인터를 받음
PARTOBJECT_DESC.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();

// Late_Update에서 최종 행렬 계산
m_WorldMatrix = Transform.WorldMatrix × *m_pParentMatrix
```

- 무기, 장비 등 부모에 종속된 파츠를 표현
- `Bind_WorldMatrix()`로 합성된 월드 행렬을 셰이더에 바인딩

---

## 5. DirectX API 호출 지점과 래핑 방식

이 시스템은 DX API를 **직접 호출하지 않는다**. 대신 DX 리소스 참조를 보유하고, 실제 호출은 컴포넌트에 위임한다.

### 5.1 DX 리소스 참조 보유 (간접 사용)

| 클래스 | 보유 리소스 | 용도 |
|--------|------------|------|
| `CBase` 계열 전체 | 없음 | 레퍼런스 카운팅만 |
| `CGameObject` | `ID3D11Device*`, `ID3D11DeviceContext*` | 하위 클래스에서 DX 리소스 생성 시 전달 |
| `CComponent` | `ID3D11Device*`, `ID3D11DeviceContext*` | Clone 시 복사, 셰이더/버퍼/텍스처 생성에 사용 |
| `CTransform` | 없음 (DX 직접 호출 없음) | CPU 행렬 연산만 |

### 5.2 CTransform의 셰이더 바인딩 (유일한 DX 연결 지점)

```cpp
HRESULT CTransform::Bind_ShaderResource(CShader* pShader, const _char* pConstantName)
{
    return pShader->Bind_Matrix(pConstantName, &m_WorldMatrix);
}
```
- CTransform 자체는 DX API를 호출하지 않음
- CShader를 통해 간접적으로 `ID3DX11EffectVariable::SetMatrix()` 호출

### 5.3 DX 리소스의 AddRef/Release 체인

```
CGameObject 생성 시:
  Safe_AddRef(m_pDevice)    ← ID3D11Device::AddRef()
  Safe_AddRef(m_pContext)   ← ID3D11DeviceContext::AddRef()

CGameObject 해제 시:
  Safe_Release(m_pDevice)   ← ID3D11Device::Release()
  Safe_Release(m_pContext)  ← ID3D11DeviceContext::Release()
```
- COM 레퍼런스 카운팅과 CBase 레퍼런스 카운팅이 공존
- DX 리소스는 COM `AddRef/Release`, 엔진 객체는 `CBase::AddRef/Release`

---

## 6. 참고할 만한 설계 판단

### 6.1 오브젝트 프로토타입(전역) vs 컴포넌트 프로토타입(레벨별)

- **오브젝트 프로토타입**: `map<wstring, CGameObject*>` 단일 맵 (레벨 무관)
- **컴포넌트 프로토타입**: `map<wstring, CComponent*>[]` 레벨별 배열
- **이유**: 오브젝트 프로토타입은 어떤 레벨에서든 재사용, 컴포넌트(셰이더/모델/텍스처)는 레벨별 리소스 해제가 필요
- **참고**: 레벨 전환 시 `Clear_Resources(iLevelIndex)`로 해당 레벨 컴포넌트만 해제 → 메모리 관리에 유리. 단, `LEVEL_STATIC`(인덱스 0)은 해제하지 않아 전역 리소스로 사용

### 6.2 Transform 자동 생성

```cpp
// CGameObject::Initialize()에서 자동으로
m_pTransformCom = CTransform::Create(m_pDevice, m_pContext);
m_Components.emplace("Com_Transform", m_pTransformCom);
```
- **모든 오브젝트가 반드시 Transform을 가짐** — 별도 Add_Component 불필요
- **참고**: 컴포넌트 시스템의 "필수 컴포넌트" 개념. 다만 CTransform이 `Clone()` → `nullptr`을 반환하여 프로토타입 패턴과 불일치 (직접 Create만 사용)

### 6.3 FILEDATA를 통한 파싱 오브젝트 지원

```cpp
if (pDesc->isParsing) {
    // 파일에서 로드한 위치/회전/스케일 적용
    m_pTransformCom->Set_State(STATE_POSITION, ...);
    m_pTransformCom->Set_State(STATE_RIGHT, ...);
}
```
- **참고**: Initialize의 `void* pArg`를 여러 구조체로 캐스팅하여 다양한 초기화 지원 (GAMEOBJECT_DESC, FILEDATA, CONTAINEROBJECT_DESC, PARTOBJECT_DESC 등)

### 6.4 지연 삭제 2단계 분리

```
프레임 N: Destory() → 예약 큐 + SetActive(false)
프레임 N+1 시작: Destory_Update() → m_bDead = true
프레임 N+1 Update: Layer가 m_bDead 확인 → Safe_Release + erase
```
- **참고**: 순회 중 안전한 삭제를 보장하면서도, 삭제 예약 즉시 비활성화하여 로직에서 제외. 3개 함수(`Player_Update`, `Camera_Update`, `Late_Update`)는 erase만 하고 Release는 하지 않음 → **`Update()`에서만 Safe_Release** (단일 해제 지점)

### 6.5 Dead 오브젝트 처리 일관성

| 함수 | m_bDead 처리 | m_bIsActive 처리 |
|------|-------------|-----------------|
| `Player_Update` | erase (Release 없음) | skip |
| `Update` | **Safe_Release + erase** | skip |
| `Late_Update` | erase (Release 없음) | skip |
| `Camera_Update` | erase (Release 없음) | skip |

- **참고**: Update()가 유일한 메모리 해제 지점이므로, 삭제 순서가 보장됨. 다른 패스에서는 리스트에서만 제거하여 이터레이터 무효화 방지

### 6.6 Component 검색의 3단계 접근

```cpp
// 방법 1: 오브젝트에서 직접
pGameObject->Get_Component(L"Com_Model");

// 방법 2: 레이어에서 인덱스로
pLayer->Get_Component(L"Com_Model", iIndex);

// 방법 3: 매니저에서 레벨+레이어+태그로
CGameInstance::Get_Component(LEVEL_GAMEPLAY, L"Layer_Player", L"Com_Model", 0);
```
- **참고**: 파사드를 통한 깊은 접근이 가능하지만, 인덱스 기반 검색은 O(n). 대규모 프로젝트에서는 해시맵 기반 직접 접근이 유리

### 6.7 Container-Part의 부모 행렬 포인터 공유

```cpp
// 부모 행렬을 포인터로 전달 (복사 아님)
PARTOBJECT_DESC.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();

// Part의 Late_Update에서 매 프레임 곱셈
m_WorldMatrix = Transform.WorldMatrix × *m_pParentMatrix;
```
- **참고**: 포인터 공유로 부모 행렬 변경이 자동 반영됨. 다만 부모가 먼저 갱신되어야 하므로 **갱신 순서 의존성**이 있음 (Container가 Part보다 먼저 Update/Late_Update)

### 6.8 m_isCloned 플래그

```cpp
CComponent::CComponent(const CComponent& Prototype)
    : m_isCloned{ true }  // 복제본임을 표시
```
- **참고**: 프로토타입과 복제본을 구분하여 리소스 해제 시 다르게 처리할 수 있음 (예: 프로토타입만 GPU 리소스 해제). 실제 활용은 제한적이지만, 디버깅과 안전한 해제에 유용

### 6.9 레퍼런스 카운팅 주의점

```cpp
// CBase::Release() — post-decrement 사용
if (0 == m_iRefCnt) {  // RefCnt가 0이면 삭제
    Free(); delete this; return 0;
}
return m_iRefCnt--;    // 아직 0이 아니면 감소
```
- **주의**: 초기 RefCnt = 0으로 시작. `new` 직후 Release하면 즉시 삭제됨
- **패턴**: 생성 후 어딘가에 저장(emplace)할 때 AddRef 없이 저장 → 마지막 소유자가 Release하면 삭제
- **참고**: COM과 유사하지만 초기값이 0인 점이 다름. AddRef를 빼먹으면 이중 삭제 위험
