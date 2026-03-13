# O02-A. GameObject 생성/Clone 전체 흐름 (심화)

## 1. 레퍼런스 카운팅 기초 (CBase)

모든 엔진 객체의 최상위 부모인 `CBase`가 메모리 관리의 핵심이다.

```cpp
// Base.cpp - 전체 코드 (매우 짧지만 핵심)
unsigned int CBase::AddRef() {
    return ++m_iRefCnt;    // 참조 증가
}

unsigned int CBase::Release() {
    if (0 == m_iRefCnt) {  // 이미 0이면
        Free();            // 가상 함수: 파생 클래스의 정리 코드 실행
        delete this;       // 자기 자신 삭제
        return 0;
    }
    else
        return m_iRefCnt--;  // 후위 감소: 현재값 반환 후 감소
}
```

### 주의: 후위 감소 버그
`m_iRefCnt--`는 **후위 감소**이므로 현재값을 반환한 뒤 감소한다. 즉:
- `m_iRefCnt`가 1일 때 `Release()` 호출 → 반환값 1, 이후 `m_iRefCnt`는 0
- 다음 `Release()` 호출 → `m_iRefCnt`가 0이므로 `Free()` + `delete this`

이것은 **참조 카운트가 0이 되는 순간이 아닌, 0인 상태에서 다시 Release를 호출해야 삭제**된다는 의미다. 따라서 일반적인 COM의 `Release`(`--count == 0`이면 삭제)와는 다르게, **실제 사용 시에는 `Safe_Release` 템플릿을 통해 0 반환 시 포인터를 nullptr로 만든다**:

```cpp
// Engine_Function.h
template<typename T>
unsigned int Safe_Release(T& pInstance) {
    unsigned int iRefCnt = 0;
    if (nullptr != pInstance) {
        iRefCnt = pInstance->Release();
        if (0 == iRefCnt)
            pInstance = nullptr;  // 삭제되었으므로 댕글링 방지
    }
    return iRefCnt;
}
```

---

## 2. CComponent: 프로토타입 vs 클론 (두 가지 생성자)

CComponent는 **두 개의 생성자**를 가진다. 이것이 프로토타입 패턴의 핵심이다.

### 프로토타입 생성자 (최초 1회)
```cpp
// 원본 생성 시 호출
CComponent::CComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice { pDevice }
    , m_pContext { pContext }
    , m_pGameInstance { CGameInstance::GetInstance() }
    , m_isCloned { false }     // ★ 프로토타입임을 표시
{
    Safe_AddRef(m_pGameInstance);  // 참조 카운트 증가
    Safe_AddRef(m_pDevice);        // DX11 디바이스도 COM이므로 AddRef
    Safe_AddRef(m_pContext);
}
```

### 복사 생성자 (Clone 시)
```cpp
// Clone할 때 호출
CComponent::CComponent(const CComponent& Prototype)
    : m_pDevice{ Prototype.m_pDevice }     // 같은 디바이스 공유
    , m_pContext{ Prototype.m_pContext }    // 같은 컨텍스트 공유
    , m_pGameInstance{ CGameInstance::GetInstance() }
    , m_isCloned { true }                  // ★ 클론임을 표시
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pDevice);     // 공유하므로 참조 카운트 증가 필수
    Safe_AddRef(m_pContext);
}
```

### m_isCloned의 용도
파생 클래스의 `Free()`에서 **원본 전용 리소스**(GPU 버퍼, 텍스처 등)를 클론이 중복 해제하지 않도록 분기:
```cpp
// 예: VIBuffer::Free()
void CVIBuffer::Free() {
    __super::Free();
    if (false == m_isCloned) {  // 프로토타입만 GPU 리소스 해제
        Safe_Release(m_pVB);
        Safe_Release(m_pIB);
    }
    // 클론은 m_pVB/m_pIB 포인터만 가지고 있고, 원본이 해제 담당
}
```

---

## 3. CGameObject: Initialize에서 Transform 자동 생성

```cpp
HRESULT CGameObject::Initialize(void* pArg) {
    // pArg를 GAMEOBJECT_DESC로 캐스팅
    // GAMEOBJECT_DESC는 TRANSFORM_DESC를 상속하므로 fSpeedPerSec, fRotationPerSec 포함
    GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);

    // ★ Transform은 Clone이 아닌 직접 Create
    m_pTransformCom = CTransform::Create(m_pDevice, m_pContext);
    m_pTransformCom->Initialize(pArg);  // 속도/회전속도 설정

    // 컴포넌트 맵에 등록 (태그: "Com_Transform" = g_strTransformTag)
    m_Components.emplace(g_strTransformTag, m_pTransformCom);
    Safe_AddRef(m_pTransformCom);  // 맵에 넣었으므로 참조 +1 (총 2: 맵 + 멤버변수)
    return S_OK;
}
```

**왜 Transform만 Clone이 아닌 Create인가?**
- Transform은 오브젝트마다 고유한 위치/회전 데이터를 가짐
- 프로토타입에서 Clone해봐야 의미 없음 (초기값이 동일할 필요 없음)
- `CTransform::Clone()` 자체가 `return nullptr;`로 구현됨

---

## 4. Add_Component: 프로토타입에서 Clone하여 장착

```cpp
HRESULT CGameObject::Add_Component(
    _uint iPrototypeLevelID,       // 프로토타입이 등록된 레벨 ID
    const _wstring& strPrototypeTag, // 프로토타입 태그명
    const _wstring& strComponentTag, // 이 오브젝트 내에서의 태그명
    CComponent** ppOut,              // 외부에서 직접 접근할 포인터
    void* pArg)                      // Clone 시 추가 인자
{
    // 1. 중복 태그 방지
    if (nullptr != Find_Component(strComponentTag))
        return E_FAIL;

    // 2. 프로토타입 매니저에서 Clone
    CComponent* pComponent = static_cast<CComponent*>(
        m_pGameInstance->Clone_Prototype(
            PROTOTYPE::COMPONENT,    // 컴포넌트 타입으로 Clone
            iPrototypeLevelID,
            strPrototypeTag,
            pArg
        )
    );

    // 3. 맵에 저장
    m_Components.emplace(strComponentTag, pComponent);

    // 4. 외부 포인터에 저장 (멤버 변수로 직접 접근)
    *ppOut = pComponent;

    // 5. 참조 카운트 증가 (맵 1개 + 멤버변수 1개 = 2개 보유)
    Safe_AddRef(pComponent);

    return S_OK;
}
```

### 사용 예시 (Client의 Terrain에서)
```cpp
HRESULT CTerrain::Ready_Components() {
    // Shader 컴포넌트 장착
    if (FAILED(Add_Component(
        ENUM_CLASS(LEVEL::GAMEPLAY),                           // 프로토타입 레벨
        TEXT("Prototype_Component_Shader_VtxNorTex"),           // 프로토타입 태그
        TEXT("Com_Shader"),                                     // 이 오브젝트 내 태그
        reinterpret_cast<CComponent**>(&m_pShaderCom),          // 결과 저장 포인터
        nullptr)))                                              // 추가 인자 없음
        return E_FAIL;

    // Collider 컴포넌트 장착 (추가 인자 있음)
    CBounding_AABB::AABB_DESC aabbDesc{};
    aabbDesc.vCenter = _float3(0.f, 1.f, 0.f);
    aabbDesc.vSize = _float3(1.f, 2.f, 1.f);
    if (FAILED(Add_Component(
        ENUM_CLASS(LEVEL::GAMEPLAY),
        TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider"),
        reinterpret_cast<CComponent**>(&m_pColliderCom),
        &aabbDesc)))  // ★ Clone시 초기화에 사용될 데이터
        return E_FAIL;
}
```

---

## 5. 전체 오브젝트 생성 플로우 (시퀀스 다이어그램)

```
Client (Level_GamePlay)
  │
  ├─ GameInstance->Add_GameObject(GAMEPLAY, "Proto_Terrain", GAMEPLAY, "Layer_BG")
  │     │
  │     ├─ Object_Manager->Add_GameObject(...)
  │     │     │
  │     │     ├─ GameInstance->Clone_Prototype(GAMEOBJECT, GAMEPLAY, "Proto_Terrain")
  │     │     │     │
  │     │     │     ├─ Prototype_Manager->Clone_Prototype(...)
  │     │     │     │     ├─ Find_Prototype(GAMEPLAY, "Proto_Terrain")
  │     │     │     │     │     └─ return CTerrain 원본 포인터
  │     │     │     │     │
  │     │     │     │     └─ dynamic_cast<CGameObject*>(원본)->Clone(pArg)
  │     │     │     │           │
  │     │     │     │           └─ CTerrain(const CTerrain& Proto)  ← 복사 생성자
  │     │     │     │                 │
  │     │     │     │                 ├─ CGameObject(Proto) 호출 (Device/Context 복사)
  │     │     │     │                 └─ Clone이 Initialize(pArg) 호출
  │     │     │     │                       │
  │     │     │     │                       ├─ CGameObject::Initialize(pArg)
  │     │     │     │                       │     └─ CTransform 생성 + 맵 등록
  │     │     │     │                       │
  │     │     │     │                       └─ CTerrain::Ready_Components()
  │     │     │     │                             ├─ Add_Component(Shader) → Clone
  │     │     │     │                             ├─ Add_Component(Texture) → Clone
  │     │     │     │                             └─ Add_Component(VIBuffer) → Clone
  │     │     │     │
  │     │     │     └─ return 새 CTerrain 클론
  │     │     │
  │     │     ├─ Find_Layer(GAMEPLAY, "Layer_BG")
  │     │     │     └─ 없으면 CLayer::Create() → 새 레이어 생성
  │     │     │
  │     │     └─ Layer->Add_GameObject(클론)  ← list에 push_back
  │     │
  │     └─ return S_OK
```

---

## 6. CContainerObject: 파츠 추가 흐름

```cpp
HRESULT CContainerObject::Initialize(void* pArg) {
    CONTAINER_OBJECT_DESC* pDesc = static_cast<CONTAINER_OBJECT_DESC*>(pArg);
    m_iNumPartObjects = pDesc->iNumPartObjects;  // 파츠 수
    m_PartObjects.resize(m_iNumPartObjects);      // 벡터 크기 예약
    __super::Initialize(pArg);  // Transform 생성
    return S_OK;
}

// 파츠 추가 (Client에서 호출)
HRESULT CContainerObject::Add_PartObject(
    _uint iPrototypeLevelID,
    const _wstring& strPrototypeTag,
    _uint iPartObjectIndex,    // 벡터 내 슬롯 인덱스
    void* pArg)
{
    // ★ GAMEOBJECT 타입으로 Clone (컴포넌트가 아닌 게임오브젝트)
    CPartObject* pPartObject = dynamic_cast<CPartObject*>(
        m_pGameInstance->Clone_Prototype(
            PROTOTYPE::GAMEOBJECT,
            iPrototypeLevelID,
            strPrototypeTag,
            pArg
        )
    );
    m_PartObjects[iPartObjectIndex] = pPartObject;
    return S_OK;
}
```

---

## 7. CPartObject: 부모 행렬 참조

```cpp
HRESULT CPartObject::Initialize(void* pArg) {
    PART_OBJECT_DESC* pDesc = static_cast<PART_OBJECT_DESC*>(pArg);

    // ★ 부모의 월드 행렬을 "포인터"로 받아 저장
    m_pParentMatrix = pDesc->pParentMatrix;

    // 결합 행렬 초기화
    XMStoreFloat4x4(&m_CombinedWorldMatrix, XMMatrixIdentity());

    // 부모 Initialize 호출 → Transform 생성
    __super::Initialize(pArg);
    return S_OK;
}
```

**핵심**: `pParentMatrix`는 포인터. 부모가 매 프레임 갱신하면 자동으로 최신 값이 반영된다. 이것이 파츠 시스템의 핵심 연결 메커니즘이다.

---

## 8. 참조 카운트 추적 예시

`Add_Component`를 통해 셰이더를 장착하는 과정의 참조 카운트 변화:

```
[1] CShader::Create()          → 원본 생성, RefCnt = 0
[2] Add_Prototype(원본)         → Prototype_Manager 맵에 저장 (삭제 안 하므로 RefCnt 유지 = 0)
[3] Clone_Prototype() 호출      → CShader(const CShader& Proto) 복사 생성
                                  새 클론 RefCnt = 0
[4] m_Components.emplace()      → 맵에 저장 (직접 관리)
[5] *ppOut = pComponent          → 멤버 변수에 저장
[6] Safe_AddRef(pComponent)      → RefCnt = 1
결과: 맵에서 1개 + 멤버변수에서 1개 = 참조 2개, RefCnt = 1

해제 시:
[7] GameObject::Free()
    → for(Pair) Safe_Release → RefCnt = 0 → 삭제되지 않음 (0일 때 또 Release하면 삭제)
    → Safe_Release(m_pTransformCom) 등 멤버변수도 해제
```

> **결론**: 이 프레임워크에서 `m_iRefCnt`가 0인 상태에서 `Release()`를 호출하면 `Free()` + `delete this`가 실행된다. 따라서 `Safe_AddRef`/`Safe_Release` 쌍을 정확히 맞추는 것이 필수.

---

## 9. Layer의 오브젝트 컬렉션

```cpp
// Layer.cpp
void CLayer::Priority_Update(_float fTimeDelta) {
    for (auto& pGameObject : m_GameObjects)
        pGameObject->Priority_Update(fTimeDelta);
}
// Update, Late_Update도 동일 패턴
// → 다형성(virtual)으로 각 오브젝트 타입의 구현 호출

CComponent* CLayer::Get_Component(_uint iIndex, const _wstring& strComponentTag) {
    auto iter = m_GameObjects.begin();
    for (size_t i = 0; i < iIndex; i++)
        ++iter;    // ★ list이므로 O(n) 접근
    return (*iter)->Get_Component(strComponentTag);
}
```

> **list를 사용하는 이유**: 중간 삽입/삭제가 빈번할 수 있음. 인덱스 접근은 드묾.

---

## 10. Free 호출 체인 (소멸 순서)

```
CGameObject::Free()
  ├─ __super::Free()          → CBase::Free() (비어있음)
  ├─ m_pGameInstance->DestroyInstance()  ← 싱글톤 참조 해제
  ├─ for(m_Components) Safe_Release    ← 모든 컴포넌트 해제
  ├─ m_Components.clear()
  ├─ Safe_Release(m_pTransformCom)     ← Transform 추가 해제
  ├─ Safe_Release(m_pDevice)           ← DX11 디바이스 참조 해제
  └─ Safe_Release(m_pContext)          ← DX11 컨텍스트 참조 해제

CContainerObject::Free()
  ├─ __super::Free()          → CGameObject::Free() (위 체인 전체 실행)
  ├─ for(m_PartObjects) Safe_Release   ← 모든 파츠 해제
  └─ m_PartObjects.clear()
```

**`__super::Free()` 패턴**: 파생 클래스는 반드시 부모의 Free()를 호출. 이로 인해 최하위에서 최상위까지 체인이 형성되어, 모든 리소스가 올바른 순서로 해제된다.
