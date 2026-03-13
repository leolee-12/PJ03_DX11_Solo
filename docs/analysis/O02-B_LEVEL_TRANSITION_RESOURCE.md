# O02-B: Level 전환 + 리소스 정리 타이밍

## 1. Level 시스템 아키텍처

### 클래스 계층 구조

```
CBase
  └── CLevel (Engine, abstract, ENGINE_DLL)
        ├── CLevel_Logo       (Client)
        ├── CLevel_Loading    (Client)
        └── CLevel_GamePlay   (Client)
```

**CLevel** (Engine/public/Level.h)은 `abstract` 키워드로 인스턴스화를 차단한다.
모든 레벨은 `ID3D11Device*`, `ID3D11DeviceContext*`, `CGameInstance*`를 보유하며,
생성자에서 3개 모두 `Safe_AddRef`, 소멸 시 `Safe_Release`한다.

```cpp
// Level.cpp - 생성자
CLevel::CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
    , m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);   // RefCount: 싱글톤 참조 +1
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}
```

### LEVEL enum (Client_Defines.h)

```cpp
enum class LEVEL { STATIC, LOADING, LOGO, GAMEPLAY, END };
```

| 레벨 ID | 용도 | 자원 수명 |
|---------|------|----------|
| `STATIC` | 모든 레벨에서 공유하는 자원 | 엔진 종료까지 유지 |
| `LOADING` | 로딩 화면 전용 | 다음 레벨 전환 시 정리 |
| `LOGO` | 로고 화면 자원 | 다음 레벨 전환 시 정리 |
| `GAMEPLAY` | 게임 플레이 자원 | 다음 레벨 전환 시 정리 |

**핵심**: LEVEL enum 값은 **배열 인덱스**로 직접 사용된다.

```cpp
// Object_Manager.cpp
m_pLayers = new map<const _wstring, CLayer*>[iNumLevels];
// Prototype_Manager.cpp
m_pPrototypes = new PROTOTYPES[iNumLevels];
```

---

## 2. 레벨별 자원 저장 구조

### 2중 배열 구조

Prototype_Manager와 Object_Manager 모두 **동적 배열 of map** 패턴을 사용한다:

```
인덱스:  [0: STATIC]  [1: LOADING]  [2: LOGO]  [3: GAMEPLAY]
         ┌──────────┐ ┌──────────┐  ┌────────┐ ┌────────────┐
Proto:   │Shader_Pos│ │(비어있음)│  │Tex_BG  │ │Model_Fiona │
         │VIBuf_Rect│ │          │  │GO_BG   │ │Tex_Terrain │
         │          │ │          │  │        │ │GO_Player...│
         └──────────┘ └──────────┘  └────────┘ └────────────┘

Object:  [0: STATIC]  [1: LOADING]  [2: LOGO]  [3: GAMEPLAY]
         ┌──────────┐ ┌──────────┐  ┌────────┐ ┌────────────┐
Layers:  │(비어있음)│ │(비어있음)│  │Layer_BG│ │Layer_Camera│
         │          │ │          │  │        │ │Layer_Player│
         │          │ │          │  │        │ │Layer_Monster│
         └──────────┘ └──────────┘  └────────┘ └────────────┘
```

### STATIC 레벨의 역할

`Loader::Loading_For_Logo()`를 보면 일부 자원은 `LEVEL::STATIC`에 등록된다:

```cpp
// VIBuffer_Rect는 STATIC에 등록 (모든 레벨에서 사용)
m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC),
    TEXT("Prototype_Component_VIBuffer_Rect"),
    CVIBuffer_Rect::Create(m_pDevice, m_pContext));

// Shader도 STATIC에 등록
m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC),
    TEXT("Prototype_Component_Shader_VtxPosTex"),
    CShader::Create(...));
```

STATIC 슬롯의 자원은 `Clear_Resources()`가 해당 인덱스(0)로 호출되지 않는 한 절대 해제되지 않는다.
레벨 전환 시에는 **현재 레벨 ID**로만 Clear가 호출되므로 STATIC은 안전하다.

---

## 3. 레벨 전환 전체 흐름

### Level_Manager::Change_Level — 핵심 함수

```cpp
HRESULT CLevel_Manager::Change_Level(_uint iLevelID, CLevel* pNewLevel)
{
    // ① 현재 레벨의 자원을 정리한다
    if (nullptr != m_pCurrentLevel)
        m_pGameInstance->Clear_Resources(m_iLevelID);  // ← 이전 레벨 ID로 정리

    // ② 현재 레벨 객체를 해제한다
    Safe_Release(m_pCurrentLevel);

    // ③ 새 레벨로 교체
    m_pCurrentLevel = pNewLevel;
    m_iLevelID = iLevelID;

    return S_OK;
}
```

**실행 순서가 중요하다:**
1. **자원 정리 먼저** → 프로토타입/레이어/오브젝트 해제
2. **레벨 객체 해제** → Level의 Free() 호출 (Device, Context, GameInstance만 Release)
3. **새 레벨 설정** → 이미 Initialize까지 완료된 레벨 객체가 전달됨

### Clear_Resources 내부

```cpp
// GameInstance.cpp
void CGameInstance::Clear_Resources(_uint iLevelID)
{
    m_pPrototype_Manager->Clear(iLevelID);  // 프로토타입 맵 정리
    m_pObject_Manager->Clear(iLevelID);     // 레이어→오브젝트 정리
}
```

```cpp
// Prototype_Manager.cpp
void CPrototype_Manager::Clear(_uint iLevelID)
{
    for (auto& Pair : m_pPrototypes[iLevelID])
        Safe_Release(Pair.second);           // 각 프로토타입 Release
    m_pPrototypes[iLevelID].clear();         // 맵 비우기
}

// Object_Manager.cpp
void CObject_Manager::Clear(_uint iLevelID)
{
    for (auto& Pair : m_pLayers[iLevelID])
        Safe_Release(Pair.second);           // 각 레이어 Release → 내부 오브젝트들도 해제
    m_pLayers[iLevelID].clear();             // 맵 비우기
}
```

---

## 4. 전체 레벨 전환 시나리오

### 시나리오: 앱 시작 → 로고 → 게임플레이

```
[1단계] MainApp::Initialize()
        ↓
        Start_Level(LEVEL::LOGO)
        ↓
        Change_Level(LOADING, Level_Loading(nextLevel=LOGO))
        ├── m_pCurrentLevel == nullptr → Clear 건너뜀
        ├── Level_Loading::Initialize(LOGO) 호출
        └── Loader 생성 → 워커 스레드에서 Loading_For_Logo() 실행

[2단계] Loading_For_Logo() 완료 + SPACE 키
        ↓
        Level_Loading::Update()에서:
        Change_Level(LOGO, Level_Logo)
        ├── Clear_Resources(LOADING) ← LOADING 슬롯 정리 (비어있으므로 실질적 동작 없음)
        ├── Safe_Release(Level_Loading) → Loader도 해제
        └── Level_Logo::Initialize() → 로고용 오브젝트 생성

[3단계] Level_Logo에서 ENTER 키
        ↓
        Level_Logo::Update()에서:
        Change_Level(LOADING, Level_Loading(nextLevel=GAMEPLAY))
        ├── Clear_Resources(LOGO) ← LOGO 슬롯의 텍스처, 오브젝트 해제
        │   ├── Prototype_Manager::Clear(2) → Tex_BG, GO_BG 프로토타입 Release
        │   └── Object_Manager::Clear(2) → Layer_BackGround와 내부 BG 오브젝트 Release
        ├── Safe_Release(Level_Logo)
        └── Level_Loading 시작 → Loader가 GamePlay 자원 로딩

[4단계] Loading_For_GamePlay() 완료 + SPACE 키
        ↓
        Change_Level(GAMEPLAY, Level_GamePlay)
        ├── Clear_Resources(LOADING) ← LOADING 슬롯 정리
        ├── Safe_Release(Level_Loading)
        └── Level_GamePlay::Initialize() → 카메라, 플레이어, 몬스터 등 생성
```

### 시퀀스 다이어그램

```
Client                  Level_Manager          GameInstance         Prototype_Mgr    Object_Mgr
  │                         │                      │                    │               │
  │ Change_Level(LOGO,new)  │                      │                    │               │
  │────────────────────────>│                      │                    │               │
  │                         │ Clear_Resources(OLD) │                    │               │
  │                         │─────────────────────>│                    │               │
  │                         │                      │ Clear(iOldLevelID) │               │
  │                         │                      │───────────────────>│               │
  │                         │                      │   Release protos   │               │
  │                         │                      │<──────────────────│               │
  │                         │                      │ Clear(iOldLevelID) │               │
  │                         │                      │──────────────────────────────────>│
  │                         │                      │   Release layers   │               │
  │                         │                      │<─────────────────────────────────│
  │                         │                      │                    │               │
  │                         │ Safe_Release(old)    │                    │               │
  │                         │ → old->Free()        │                    │               │
  │                         │ m_pCurrentLevel=new  │                    │               │
  │                         │ m_iLevelID=LOGO      │                    │               │
  │<────────────────────────│                      │                    │               │
```

---

## 5. 멀티스레드 로딩 메커니즘

### CLoader 구조

```cpp
class CLoader : public CBase {
    HANDLE              m_hThread;           // 워커 스레드 핸들
    CRITICAL_SECTION    m_CriticalSection;   // 동기화 객체
    _bool               m_isFinished;        // 로딩 완료 플래그
    LEVEL               m_eNextLevelID;      // 로딩 대상 레벨
};
```

### 워커 스레드 생성 흐름

```cpp
// Loader.cpp
HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
    m_eNextLevelID = eNextLevelID;
    InitializeCriticalSection(&m_CriticalSection);

    // 워커 스레드 생성 - ThreadMain이 진입점
    m_hThread = (HANDLE)_beginthreadex(
        nullptr, 0, ThreadMain, this, 0, nullptr);
    return S_OK;
}

// 전역 함수 - 워커 스레드 진입점
_uint APIENTRY ThreadMain(void* pArg)
{
    CLoader* pLoader = static_cast<CLoader*>(pArg);
    if (FAILED(pLoader->Loading()))
        return 1;
    return 0;
}
```

### COM 초기화가 필요한 이유

```cpp
HRESULT CLoader::Loading()
{
    CoInitializeEx(nullptr, 0);  // ← 워커 스레드에서 COM 초기화 필수!
    // ...
}
```

워커 스레드에서 **Assimp**(FBX 로딩)이나 **DirectXTK**(텍스처 로딩)를 호출하는데,
이들은 내부적으로 COM 객체를 사용한다. 스레드별로 COM이 초기화되어야 하므로
`CoInitializeEx`를 워커 스레드 진입 직후에 호출한다.

### 메인 스레드 ↔ 워커 스레드 동기화

```
메인 스레드 (Level_Loading)        워커 스레드 (Loader)
         │                                │
         │                                │ EnterCriticalSection
         │                                │ Loading_For_GamePlay()
         │ Update(): isFinished?           │   Add_Prototype(...)  ×N
         │ → false → 대기                  │   Add_Prototype(...)  ×N
         │                                │ m_isFinished = true
         │                                │ LeaveCriticalSection
         │ Update(): isFinished?           │
         │ → true + SPACE                  │
         │ Change_Level(GAMEPLAY, ...)     │
         ↓                                ↓
```

**주의점**: `CRITICAL_SECTION`은 Loading 함수 전체를 감싸지만,
`m_isFinished` 플래그 체크는 Lock 없이 수행한다.
이는 `_bool`이 단일 바이트 쓰기로 원자적이라는 가정에 기반한다.

### Loader 해제 시 스레드 안전 종료

```cpp
void CLoader::Free()
{
    WaitForSingleObject(m_hThread, INFINITE);  // 워커 스레드 완료 대기
    CloseHandle(m_hThread);                     // 핸들 닫기
    DeleteCriticalSection(&m_CriticalSection);  // CS 삭제
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
```

`WaitForSingleObject`가 **반드시** 먼저 호출되어야 워커 스레드가
아직 실행 중인 상태에서 리소스를 해제하는 문제를 방지할 수 있다.

---

## 6. Object 업데이트 범위

### 모든 레벨의 오브젝트가 업데이트된다

```cpp
// Object_Manager.cpp
void CObject_Manager::Priority_Update(_float fTimeDelta)
{
    for (size_t i = 0; i < m_iNumLevels; i++)      // ← 전체 레벨 순회!
    {
        for (auto& Pair : m_pLayers[i])
            Pair.second->Priority_Update(fTimeDelta);
    }
}
```

**Update, Late_Update도 동일한 패턴.**

이것은 `LEVEL::STATIC`에 등록된 오브젝트도 매 프레임 업데이트된다는 의미이다.
반대로, 이미 Clear된 레벨 슬롯은 맵이 비어있으므로 순회 비용이 없다.

### Level의 Update vs Object_Manager의 Update

```cpp
void CGameInstance::Update_Engine(_float fTimeDelta)
{
    m_pPicking->Update();
    m_pInput_Device->Update();

    m_pObject_Manager->Priority_Update(fTimeDelta);  // ① 전체 오브젝트

    m_pPipeLine->Update();                            // ② 카메라 행렬 갱신
    m_pFrustum->Update();                             // ③ 절두체 갱신

    m_pObject_Manager->Update(fTimeDelta);            // ④ 전체 오브젝트
    m_pObject_Manager->Late_Update(fTimeDelta);       // ⑤ 전체 오브젝트

    m_pLevel_Manager->Update(fTimeDelta);             // ⑥ 현재 레벨만
}
```

| 단계 | 호출 대상 | 범위 |
|------|-----------|------|
| ①③④⑤ | Object_Manager | **전체** 레벨의 오브젝트 |
| ⑥ | Level_Manager → CLevel::Update | **현재** 레벨만 |

Level::Update는 주로 **레벨 전환 로직**(키 입력 감지 등)에 사용되고,
실제 게임 오브젝트 로직은 Object_Manager를 통해 처리된다.

---

## 7. 엔진 종료 시 해제 순서

### MainApp::Free()

```cpp
void CMainApp::Free()
{
    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);
    m_pGameInstance->Release_Engine();    // 모든 매니저 해제
    m_pGameInstance->DestroyInstance();   // 싱글톤 삭제
}
```

### Release_Engine() — 역순 해제

```cpp
void CGameInstance::Release_Engine()
{
    // 생성 역순으로 해제 (의존성 보장)
    Safe_Release(m_pFrustum);
    Safe_Release(m_pPicking);
    Safe_Release(m_pShadow);
    Safe_Release(m_pTarget_Manager);
    Safe_Release(m_pFont_Manager);
    Safe_Release(m_pLight_Manager);
    Safe_Release(m_pPipeLine);
    Safe_Release(m_pRenderer);
    Safe_Release(m_pObject_Manager);     // 모든 레이어/오브젝트 해제
    Safe_Release(m_pPrototype_Manager);  // 모든 프로토타입 해제
    Safe_Release(m_pLevel_Manager);      // 현재 레벨 해제
    Safe_Release(m_pInput_Device);
    Safe_Release(m_pGraphic_Device);
    Safe_Release(m_pTimer_Manager);
    DestroyInstance();                   // 싱글톤 자체 삭제
}
```

**해제 순서의 의미:**
1. **Object_Manager가 Prototype_Manager보다 먼저** 해제
   - 오브젝트가 가진 컴포넌트는 Clone이므로 프로토타입과 독립적
   - 하지만 안전을 위해 오브젝트를 먼저 정리
2. **Level_Manager는 Object_Manager 이후** 해제
   - Level_Manager::Free()에서 현재 레벨의 Release 호출
   - 하지만 Clear_Resources는 호출하지 않음 (이미 매니저가 해제됨)
3. **GraphicDevice가 가장 마지막** (Timer 제외)
   - 다른 모든 리소스가 Device를 참조하므로 마지막에 해제

### Level_Manager::Free()의 특수성

```cpp
void CLevel_Manager::Free()
{
    __super::Free();
    m_pGameInstance->DestroyInstance();  // GameInstance 참조 해제
    Safe_Release(m_pCurrentLevel);       // 현재 레벨 Release
    // ⚠️ Clear_Resources는 호출하지 않는다!
}
```

종료 시에는 Object_Manager와 Prototype_Manager가 이미 자신의 Free()에서
전체 레벨의 자원을 정리하므로, Level_Manager에서 다시 Clear할 필요가 없다.

---

## 8. 설계 패턴 분석

### 레벨 전환 = "자원 슬롯 교체" 패턴

```
전환 전:  [STATIC: 유지] [LOADING: 비어있음] [LOGO: 사용중] [GAMEPLAY: 비어있음]
                                              ↑ 현재 레벨

Clear_Resources(LOGO) 후:
          [STATIC: 유지] [LOADING: 비어있음] [LOGO: 비어있음] [GAMEPLAY: 비어있음]

새 레벨 시작 후:
          [STATIC: 유지] [LOADING: 비어있음] [LOGO: 비어있음] [GAMEPLAY: 사용중]
                                                              ↑ 현재 레벨
```

### Loading 레벨의 특수 흐름

Loading 레벨은 자기 자신의 ID가 `LEVEL::LOADING`이지만,
Loader가 등록하는 자원은 **다음 레벨의 ID**로 등록된다:

```cpp
// Loader가 GAMEPLAY 자원을 로딩할 때:
m_pGameInstance->Add_Prototype(
    ENUM_CLASS(LEVEL::GAMEPLAY),  // ← 다음 레벨 슬롯에 저장!
    TEXT("Prototype_Component_Model_Fiona"), ...);
```

따라서 `Change_Level(GAMEPLAY, ...)` 시 `Clear_Resources(LOADING)`이 호출되어도
GAMEPLAY 슬롯의 자원은 영향을 받지 않는다.

### 잠재적 주의사항

1. **같은 레벨로 재전환**: 현재 레벨 ID와 같은 ID로 Change_Level을 호출하면
   새 레벨의 자원까지 Clear될 수 있다 (사전에 같은 슬롯에 등록했다면)
2. **STATIC 정리 없음**: STATIC 슬롯은 Clear되지 않으므로 누적될 수 있음
3. **Level_Loading의 자기 참조**: Change_Level은 Level_Loading::Update() 내에서
   호출되는데, 이 함수가 리턴된 후 Level_Loading 자체가 이미 해제된 상태이므로
   **Update 이후 추가 작업이 없어야** 한다 (`return;`으로 즉시 종료)

```cpp
// Level_Loading.cpp - 안전한 패턴
void CLevel_Loading::Update(_float fTimeDelta)
{
    if (true == m_pLoader->isFinished() &&
        GetKeyState(VK_SPACE) & 0x8000)
    {
        // ...
        m_pGameInstance->Change_Level(...);  // ← this가 해제됨!
        return;  // ← 반드시 즉시 리턴!
    }
}
```

---

## 9. 핵심 정리

| 항목 | 설명 |
|------|------|
| **자원 수명** | LEVEL ID로 관리, 레벨 전환 시 해당 슬롯만 정리 |
| **STATIC 슬롯** | 엔진 종료까지 유지, 공유 자원 저장 |
| **전환 순서** | Clear_Resources → Safe_Release(old) → 새 레벨 설정 |
| **로딩 스레드** | `_beginthreadex` + `CRITICAL_SECTION` + `CoInitializeEx` |
| **업데이트 범위** | Object_Manager: 전체 레벨 / Level::Update: 현재 레벨만 |
| **종료 해제** | 생성 역순: Object → Prototype → Level → Device |
| **자기 해제 주의** | Change_Level 호출 후 즉시 return 필수 |
