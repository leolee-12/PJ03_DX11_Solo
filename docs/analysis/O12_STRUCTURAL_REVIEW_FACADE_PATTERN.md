# O12: 구조 검수 및 퍼사드 패턴 분석

> 작성: 2026-02-27 | 대상: `origin/main` 5bf98d3 (02/27 수업 반영)

## 1. 업데이트 요약 (f97fc4a → 5bf98d3)

| 커밋 | 주요 변경 |
|------|----------|
| 02/24 | CObject_Manager: 프로토타입/오브젝트 매니저로 원본, 사본 객체(BackGround) 생성 |
| 02/26 | CRenderer: BackGround 렌더콜 등록, CGameInstance::Clear_Resources 구현 |
| 02/27 | CComponent, CTransform: 컴포넌트 시스템 기초 |

### 신규 클래스

- **CComponent** — 모든 컴포넌트의 추상 베이스. Device/Context 보유, Clone() 순수 가상
- **CTransform** — 월드 매트릭스 + 속도/회전 속도. TRANSFORM_DESC로 초기화
- **CRenderer** — 렌더 큐 4단계 (PRIORITY → NONBLEND → BLEND → UI)

---

## 2. O09 인사이트 해결 현황

| # | 항목 | 상태 | 비고 |
|---|------|------|------|
| 1 | Clear_Resources 미구현 | **해결** | ObjectMgr + PrototypeMgr 둘 다 Clear 호출 |
| 2 | m_isFinished 동기화 | **해결** | `std::atomic<_bool>` 적용 완료 |
| 3 | Release() 후위 감소 | 유지 | 설계 이해 (수정 불필요) |
| 4 | #define new DBG_NEW | **해결** | 주석 처리됨 |
| 5 | using namespace std | 유지 | 수업 관행, 수정 불필요 |
| 6 | 프레임 타이밍 드리프트 | 미확인 | Client.cpp 변경 없음 |
| 7 | PROTOTYPE enum Clone 분기 | **유지** | 아래 3절에서 재논의 |

---

## 3. 새로운 구조적 인사이트

### [상] 3-1. CObject_Manager → CGameInstance 역참조 (순환 의존)

```
CGameInstance ──소유──→ CObject_Manager
CObject_Manager ──참조──→ CGameInstance
```

```cpp
// Object_Manager.cpp:8
CObject_Manager::CObject_Manager()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

// Object_Manager.cpp:29
CGameObject* pGameObject = dynamic_cast<CGameObject*>(
    m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ...));
```

**문제**: 소유자(GameInstance)가 피소유자(ObjectMgr)를 소유하고, 피소유자가 다시 소유자를 참조. AddRef까지 걸려 있어 해제 순서에 민감. 현재는 `Release_Engine()`에서 명시적 순서로 해제하므로 동작하지만, 구조적으로 불안정.

**동일 패턴이 Level_Manager에도 존재** — Level_Manager도 CGameInstance를 역참조하여 Clear_Resources를 호출.

→ 4절 퍼사드 분석에서 해결 방안 제시

---

### [상] 3-2. CGameObject::Initialize에서 CTransform 직접 생성

```cpp
// GameObject.cpp:33-35
m_pTransformCom = CTransform::Create(m_pDevice, m_pContext);
if (nullptr == m_pTransformCom)
    return E_FAIL;
```

CTransform을 프로토타입 시스템을 거치지 않고 `Create()`로 직접 생성. 다른 컴포넌트는 프로토타입 매니저를 통해 Clone하는 패턴인데, Transform만 예외.

**의도 추정**: Transform은 모든 GameObject에 필수이고, 프로토타입에서 복제할 데이터가 없으므로(매번 새 월드 매트릭스 필요) 직접 생성이 합리적. 그러나 이후 컴포넌트 시스템이 확장될 때 "컴포넌트 추가 = 프로토타입에서 Clone"이라는 일관된 규칙에서 벗어남.

**인사이트**: 이 불일치는 수업 진행 과정에서 자연스러운 것이며, 향후 컴포넌트가 많아질 때 `Add_Component()` 패턴이 도입되면 Transform도 프로토타입으로 통합될 가능성 높음. 현재 단계에서는 문제 아님.

---

### [중] 3-3. CTransform의 Free()에서 m_pTransformCom 미해제

```cpp
// GameObject.cpp Free()
void CGameObject::Free()
{
    __super::Free();
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);
    // m_pTransformCom 해제 누락
}
```

CGameObject::Initialize()에서 `CTransform::Create()`로 생성된 m_pTransformCom이 Free()에서 해제되지 않음. **메모리 누수**.

**수정**: `Safe_Release(m_pTransformCom);`을 Free()에 추가 필요.

---

### [중] 3-4. Renderer::Add_RenderGroup 범위 검증 부재

```cpp
// Renderer.cpp:23
void CRenderer::Add_RenderGroup(RENDERID eGroupID, CGameObject* pGameObject)
{
    m_RenderObjects[ETOUI(eGroupID)].push_back(pGameObject);
    Safe_AddRef(pGameObject);
}
```

`eGroupID`가 `RENDERID::END` 이상이면 배열 범위 초과. enum class이므로 실수 가능성은 낮지만, 방어적 검증 없음.

---

### [중] 3-5. Renderer의 Render_* 함수 중복

```cpp
HRESULT CRenderer::Render_Priority() { /* 루프 + Release + clear */ }
HRESULT CRenderer::Render_NonBlend() { /* 동일 패턴 */ }
HRESULT CRenderer::Render_Blend()    { /* 동일 패턴 */ }
HRESULT CRenderer::Render_UI()       { /* 동일 패턴 */ }
```

4개 함수 로직이 완전히 동일(인덱스만 다름). 향후 셰이더 스테이트 전환(알파 블렌딩 on/off 등)이 함수별로 추가되므로 현재 분리가 올바름. 현 시점에서는 인지만 필요.

---

### [하] 3-6. PROTOTYPE enum 분기 — O09 #7 재확인

CComponent에 `Clone()` 순수 가상이 추가되었고, CGameObject에도 `Clone()` 순수 가상이 있음. 그러나 두 클래스의 공통 부모인 CBase에는 Clone이 없으므로, Prototype_Manager는 여전히 enum 분기 + dynamic_cast를 사용.

```cpp
// Prototype_Manager.cpp:39-42
if (PROTOTYPE::GAMEOBJECT == eType)
    pInstance = dynamic_cast<CGameObject*>(pPrototype)->Clone(pArg);
else
    pInstance = dynamic_cast<CComponent*>(pPrototype)->Clone(pArg);
```

CBase에 `virtual CBase* Clone(void*)` 순수 가상을 두면 이 분기와 enum이 완전히 불필요해지지만, CBase가 abstract가 아닌 점(CLayer 등이 CBase를 직접 상속하면서 Clone 불필요)을 고려하면 현재 구조도 합리적. 다만 Clone이 필요한 클래스(GameObject, Component)만의 공통 인터페이스(ICloneable 등)를 두는 방법도 있음.

---

## 4. 퍼사드 패턴 심층 분석

### 4-1. 참고 프로젝트(3D최종버전)의 완성된 CGameInstance

참고 프로젝트에서 CGameInstance는 **14개 매니저**를 소유하며, 약 **60개 이상의 위임 메서드**를 제공한다.

```
┌──────────────────────────────────────────────────────┐
│              CGameInstance (Facade) — 참고 프로젝트     │
│──────────────────────────────────────────────────────│
│ 소유 매니저 (14개):                                     │
│  CGraphic_Device, CInput_Device, CTimer_Manager,     │
│  CLevel_Manager, CPrototype_Manager, CObject_Manager,│
│  CRenderer, CPipeLine, CLight_Manager, CFont_Manager,│
│  CTarget_Manager, CShadow, CPicking, CFrustum        │
│──────────────────────────────────────────────────────│
│ 역할 ①: 오케스트레이션                                  │
│  Initialize_Engine / Update_Engine / Draw / Release   │
│ 역할 ②: 단순 위임 (~50개 메서드)                        │
│  Add_Timer, Change_Level, Add_Prototype, ...          │
│ 역할 ③: 매니저 간 서비스 로케이터 (아래 분석)              │
└──────────────────────────────────────────────────────┘
```

### 4-2. GameInstance를 역참조하는 매니저들 (참고 프로젝트)

참고 프로젝트의 14개 매니저 중, **5개가 GameInstance를 역참조**한다:

| 매니저 | 역참조 대상 (GameInstance 경유) | 용도 |
|--------|------------------------------|------|
| **CRenderer** | Target_Manager, PipeLine, Light_Manager, Shadow | 디퍼드 렌더링 파이프라인 전체 |
| **CObject_Manager** | Prototype_Manager | Clone_Prototype으로 사본 생성 |
| **CLevel_Manager** | Object_Manager + Prototype_Manager | Clear_Resources (레벨 전환 시 정리) |
| **CPicking** | Target_Manager | Copy_RT_Resource (Target_World 픽셀 읽기) |
| **CFrustum** | PipeLine | Get_Transform_Matrix_Inverse (뷰/프로젝션 역행렬) |

나머지 9개 매니저(Graphic_Device, Input_Device, Timer_Manager, Prototype_Manager, PipeLine, Light_Manager, Font_Manager, Target_Manager, Shadow)는 **GameInstance를 참조하지 않는다.**

### 4-3. 역참조 유형 분류 — 두 가지 본질적 차이

분석 결과, 매니저의 GameInstance 역참조는 **두 가지 근본적으로 다른 유형**으로 나뉜다:

#### 유형 1: 오케스트레이션 (단순 조합)

```
Object_Manager::Add_GameObject()
  → GameInstance->Clone_Prototype()    // 1회 호출
  → 자체 레이어에 추가                    // 끝

Level_Manager::Change_Level()
  → GameInstance->Clear_Resources()    // 1회 호출
  → 자체 레벨 교체                       // 끝
```

**특징**: 다른 매니저의 기능을 **한 번** 호출하여 결과를 받고, 자신의 작업을 수행. 단순 순서 제어이며 퍼사드로 이동 가능.

#### 유형 2: 인프라 의존 (지속적 서비스 이용)

```
Renderer::Initialize()
  → GameInstance->Add_RenderTarget(9개)      // Target_Manager
  → GameInstance->Add_MRT(9개)               // Target_Manager

Renderer::Render_Priority()
  → GameInstance->Begin_MRT("MRT_Final")     // Target_Manager
  → [오브젝트 렌더]
  → GameInstance->End_MRT()                   // Target_Manager

Renderer::Render_Lights()
  → GameInstance->Begin_MRT("MRT_LightAcc")  // Target_Manager
  → GameInstance->Bind_RT_ShaderResource()   // Target_Manager (x2)
  → GameInstance->Get_CamPosition()          // PipeLine
  → GameInstance->Get_Transform_Float4x4_Inverse_Ptr() // PipeLine (x2)
  → GameInstance->Render_Lights()            // Light_Manager
  → GameInstance->End_MRT()                  // Target_Manager

Renderer::Render_Combined()
  → GameInstance->Begin_MRT("MRT_Final")     // Target_Manager
  → GameInstance->Bind_RT_ShaderResource()   // Target_Manager (x5)
  → GameInstance->Get_Transform_*()          // PipeLine (x2)
  → GameInstance->Get_Shadow_Transform_*()   // Shadow (x2)
  → [셰이더 렌더링]
  → GameInstance->End_MRT()                  // Target_Manager

Picking::Update()
  → GameInstance->Copy_RT_Resource("Target_World") // Target_Manager

Frustum::Update()
  → GameInstance->Get_Transform_Matrix_Inverse(VIEW)       // PipeLine
  → GameInstance->Get_Transform_Matrix_Inverse(PROJECTION)  // PipeLine
```

**특징**: 자신의 핵심 로직 수행 중에 다른 서브시스템의 서비스를 **반복적/지속적으로** 이용. Renderer 하나만 해도 렌더 패스마다 Target_Manager/PipeLine/Light_Manager/Shadow를 교차 호출.

### 4-4. 왜 유형 2를 퍼사드로 옮길 수 없는가

Renderer의 디퍼드 렌더링 파이프라인을 예로 들면:

```
Begin_MRT → Bind_RT(Normal) → Bind_RT(Depth) → Get_CamPos
→ Bind_Matrix(ViewInv) → Bind_Matrix(ProjInv) → Render_Lights
→ End_MRT → Begin_MRT → Bind_RT(Diffuse) → Bind_RT(Shade) → ...
```

이 **~30줄의 렌더링 시퀀스**를 퍼사드(GameInstance)로 옮기면:
- GameInstance가 디퍼드 렌더링 로직을 직접 구현하게 됨
- 이건 "오케스트레이션"이 아니라 **렌더링 엔진 코드**
- 퍼사드가 비대해지는 정도가 아니라, **퍼사드가 렌더러 자체**가 됨

Picking, Frustum도 마찬가지. Target_World 텍스처를 GPU에서 읽고 마우스 좌표로 변환하는 로직은 Picking의 고유 책임이며, PipeLine에서 뷰/프로젝션 역행렬을 가져와 프러스텀 평면을 만드는 것은 Frustum의 고유 책임.

### 4-5. 참고 프로젝트의 선택: GameInstance를 서비스 로케이터로 활용

참고 프로젝트는 유형 2에 대해 **GameInstance를 서비스 로케이터로 사용**하는 방식을 택했다:

```cpp
// Renderer.cpp 생성자
CRenderer::CRenderer(...)
    : m_pGameInstance{ CGameInstance::GetInstance() }  // 서비스 로케이터 획득
{
    Safe_AddRef(m_pGameInstance);
}

// 이후 필요한 서비스를 GameInstance 경유로 사용
m_pGameInstance->Begin_MRT(...);       // → Target_Manager
m_pGameInstance->Get_CamPosition();    // → PipeLine
m_pGameInstance->Render_Lights(...);   // → Light_Manager
```

**이 패턴의 장단점**:

| 장점 | 단점 |
|------|------|
| 매니저가 하나의 참조(GameInstance)만 보유 | 순환 의존 (GameInstance ↔ 매니저) |
| 새 의존성 추가 시 생성자 변경 불필요 | GameInstance.h에 모든 매니저 API 노출 필수 |
| DLL 경계에서 통일된 접근점 | 실제 의존 관계가 코드에서 안 보임 |
| 이미 GameInstance가 싱글톤이라 획득 용이 | 해제 순서에 민감 (Release_Engine 필수) |

### 4-6. 대안: 직접 의존성 주입 (DI)

서비스 로케이터 대신, 매니저가 필요한 의존성을 **생성 시 직접 받는** 방식:

```cpp
// 방안 C: 직접 의존성 주입
CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
    CTarget_Manager* pTargetMgr, CPipeLine* pPipeLine,
    CLight_Manager* pLightMgr, CShadow* pShadow)
{
    // pTargetMgr, pPipeLine 등을 멤버로 보유
}

// 사용 시
m_pTargetMgr->Begin_MRT(...);     // 직접 호출
m_pPipeLine->Get_CamPosition();   // 직접 호출
```

| 장점 | 단점 |
|------|------|
| 순환 의존 완전 제거 | 생성자 파라미터 증가 |
| 의존 관계가 명시적 (코드에서 보임) | 초기화 순서 중요 (의존 대상 먼저 생성) |
| 각 매니저 독립 테스트 가능 | GameInstance의 Initialize_Engine이 복잡해짐 |
| GameInstance.h 비대화 완화 | DLL 경계에서 매니저 헤더 노출 필요 |

### 4-7. 유형별 최종 결론

| 유형 | 해당 매니저 | 권장 방안 |
|------|-----------|---------|
| **유형 1: 오케스트레이션** | Object_Manager, Level_Manager | **방안 B** (퍼사드 오케스트레이션) |
| **유형 2: 인프라 의존** | Renderer, Picking, Frustum | **방안 A 유지** (현재 패턴) 또는 **방안 C** (DI) |

#### 유형 1에 대한 결론: 방안 B

Object_Manager와 Level_Manager의 역참조는 단순 조합이므로 퍼사드가 담당하는 것이 맞다.

```cpp
// GameInstance에서 조합
HRESULT CGameInstance::Add_GameObject(...) {
    CGameObject* pObj = dynamic_cast<CGameObject*>(
        m_pPrototypeMgr->Clone_Prototype(...));
    if (!pObj) return E_FAIL;
    return m_pObjectMgr->Add_To_Layer(pObj, iLayerLevel, strLayerTag);
}

HRESULT CGameInstance::Change_Level(_uint iNewID, CLevel* pNew) {
    Clear_Resources(m_iCurrentLevelID);  // 자체 오케스트레이션
    return m_pLevelMgr->Change_Level(iNewID, pNew);
}
```

이렇게 하면 Object_Manager와 Level_Manager에서 `m_pGameInstance` 제거 가능.

#### 유형 2에 대한 결론: 방안 A 유지 (현실적 선택)

Renderer, Picking, Frustum은 **내부 로직에서 다른 서브시스템을 지속적으로 이용**하므로 퍼사드 오케스트레이션이 불가능. 현재 참고 프로젝트의 서비스 로케이터 패턴(GameInstance 역참조)이 가장 현실적.

이유:
1. **DI(방안 C)가 이상적이지만**, Renderer만 해도 4개 매니저 의존이 필요하고, 향후 더 늘어남. 학원 프레임워크의 Create 패턴과 잘 맞지 않음
2. **GameInstance가 이미 싱글톤**이므로, 서비스 로케이터로 사용하는 비용이 매우 낮음
3. **순환 참조 문제는 Release_Engine()의 명시적 해제 순서로 관리** — 참고 프로젝트도 동일하게 동작하고 있으며, 실무에서도 이 패턴으로 문제가 발생하지 않음
4. 특히 참고 프로젝트의 Object_Manager::Free()에서 `m_pGameInstance->DestroyInstance()`를 호출하는 패턴은, Release_Engine()에서 Safe_Release(m_pObjectManager) → ObjectMgr::Free() → DestroyInstance()로 이어지는 **의도된 체인**

### 4-8. 방안 B 적용 시 변경 사항 (유형 1만 해당)

```
현재:
  ObjectMgr: m_pGameInstance 멤버 → 제거
  ObjectMgr::Add_GameObject(protoLevel, protoTag, layerLevel, layerTag)
    → Clone + 레이어 추가 모두 수행
  LevelMgr: m_pGameInstance 멤버 → 제거

변경 후:
  ObjectMgr: Add_To_Layer(CGameObject* pObj, layerLevel, layerTag)
    → 받은 객체를 레이어에 추가만
  LevelMgr: Change_Level(iIndex, CLevel*)
    → 이전 레벨 해제만 (Clear_Resources 호출 안 함)
  GameInstance::Add_GameObject(...)
    → Clone + ObjectMgr::Add_To_Layer 조합
  GameInstance::Change_Level(...)
    → Clear_Resources + LevelMgr::Change_Level 조합
```

### 4-9. 향후 확장 시 가이드라인

새로운 매니저를 추가하고 매니저 간 상호작용이 필요할 때:

```
질문: "이 상호작용은 단순 조합인가, 지속적 서비스 이용인가?"

단순 조합 (1-2회 호출로 끝남)
  → 퍼사드(GameInstance)에서 오케스트레이션
  → 예: Clone + AddToLayer, Clear + ChangeLevel

지속적 서비스 이용 (내부 로직에서 반복 호출)
  → 매니저가 GameInstance를 서비스 로케이터로 참조
  → 예: Renderer가 매 프레임 Begin_MRT/End_MRT 호출
```

이 기준으로 판단하면 퍼사드는 "얇은 조합"만 담당하고, 복잡한 인프라 의존은 서비스 로케이터 패턴으로 처리하여 퍼사드 비대화를 방지할 수 있다.

---

## 5. 클라이언트 퍼사드 도입 여부

### 상황

엔진의 CGameInstance를 모방하여 클라이언트에도 퍼사드를 두고 게임 고유 매니저들(몬스터 매니저, 퀘스트 매니저, 인벤토리 매니저 등)을 관리하는 안.

### 비교

| 관점 | 엔진 퍼사드 (CGameInstance) | 클라이언트 퍼사드 |
|------|---------------------------|----------------|
| DLL 경계 | 있음 → 캡슐화 필수 | 없음 (동일 EXE) |
| 사용자 | 클라이언트 개발자 (타인 가능) | 자기 자신 |
| 숨길 것 | 엔진 내부 구현 | 딱히 없음 |
| 매니저 수 | 6개 (확장 시 ~10개) | 게임 규모에 따라 3~8개 |
| 매니저 간 상호작용 | 빈번 (Clone, Clear 등) | 게임 로직에 따라 다름 |

### 결론: 도입하지 않는 것을 권장

1. **퍼사드의 존재 이유가 없음**. 엔진 퍼사드는 DLL 경계 뒤의 복잡성을 숨기기 위해 존재. 클라이언트는 같은 EXE 안에서 자기 코드를 자기가 쓰는 것이므로, 숨길 대상이 없음.

2. **싱글톤 하나로 통합하면 오히려 불편**. 몬스터 매니저에 접근하려고 `CClientInstance::GetInstance()->Get_MonsterMgr()->FindMonster(...)` 같은 체인이 생기는데, `CMonsterMgr::GetInstance()->FindMonster(...)` 가 더 직관적.

3. **개인 프로젝트에서 퍼사드의 이점은 협업 편의성**. 이번 프로젝트는 개인이므로 메리트 없음.

4. **매니저 간 상호작용이 필요하면**: 필요한 매니저를 직접 참조하면 됨. 클라이언트 매니저 간에는 DLL 경계가 없으므로, 직접 호출이 가장 단순.

### 대안: 매니저별 독립 싱글톤

```cpp
// 각 매니저가 독립 싱글톤
CMonsterMgr::GetInstance()->SpawnMonster(...);
CQuestMgr::GetInstance()->CheckCondition(...);
CInventoryMgr::GetInstance()->AddItem(...);

// 매니저 간 상호작용이 필요하면 호출하는 쪽에서 직접
void CQuestMgr::OnMonsterKilled(MONSTER_ID id) {
    CInventoryMgr::GetInstance()->AddReward(GetRewardFor(id));
}
```

이 구조가 개인 프로젝트에서 가장 단순하고 실용적. 다만 매니저가 8개를 넘어가면 초기화/해제 순서 관리가 번거로워지는데, 그때 초기화/해제 순서만 관리하는 경량 부트스트래퍼를 두면 충분.

```cpp
// 부트스트래퍼 (퍼사드가 아닌 생명주기 관리자)
void CGameBoot::Initialize() {
    CMonsterMgr::GetInstance()->Initialize();
    CQuestMgr::GetInstance()->Initialize();
    // ...
}
void CGameBoot::Release() {
    // 역순 해제
    CQuestMgr::DestroyInstance();
    CMonsterMgr::DestroyInstance();
}
```

---

## 6. 우선순위 요약

| 순위 | 항목 | 유형 | 영향도 |
|------|------|------|--------|
| 상 | ObjectMgr/LevelMgr 역참조 → 퍼사드 오케스트레이션 (방안 B) | 구조 개선 | 순환 의존 제거 |
| 상 | CGameObject::Free()에서 m_pTransformCom 미해제 | 버그 | 메모리 누수 |
| 중 | Renderer::Add_RenderGroup 범위 검증 | 방어 코딩 | 잠재적 크래시 |
| 하 | PROTOTYPE enum 분기 | 구조 | 확장성 (현재 무방) |
| 하 | Renderer 함수 중복 | 코드 | 향후 차별화 예정 |
| - | Renderer/Picking/Frustum의 GameInstance 역참조 | 설계 | **유지** (서비스 로케이터) |
| - | 클라이언트 퍼사드 | 설계 결정 | **불필요** (독립 싱글톤 권장) |
