# O14. Engine-Game-Client/Editor 상속 구조 설계

> O13(GameDLL 리팩토링 타당성)의 후속 분석
> Engine.dll 추상층 → Game_PKM.dll 구현층 → Client.exe/Editor.exe 실행층의 상속 설계

---

## 1. 현재 구조와 문제

### 1.1 현재 프로젝트 구조

```
Engine.dll (추상층)
├─ CBase              : RefCnt 기반 생명주기
├─ CGameObject        : abstract, Clone() 순수 가상
├─ CComponent         : abstract, Clone() 순수 가상
├─ CLevel             : abstract, Initialize/Update/Render 가상
├─ CGameInstance      : 싱글톤 퍼사드 (매니저 통합)
└─ CTransform 등      : 엔진 컴포넌트

Client.exe (구현 + 실행)
├─ CMainApp           : 엔진 초기화, 게임 루프
├─ CBackGround        : CGameObject 상속
├─ CLevel_Logo/Loading/GamePlay : CLevel 상속
└─ CLoader            : 스레드 리소스 로딩

Editor.exe (구현 + 실행)
├─ CEditorApp         : 엔진 초기화, 에디터 루프
├─ CLevel_EditLogo/EditLoading/EditPlay : CLevel 상속
├─ CEditLoader        : 에디터용 로더
├─ CImGui_Manager     : ImGui 통합
└─ CPanel_Base/MapTool : 에디터 패널
```

### 1.2 핵심 문제

| 문제 | 설명 |
|------|------|
| **게임 오브젝트 중복** | CPlayer, CMonster 등을 Client에만 구현하면 Editor에서 접근 불가 |
| **Loader 중복** | Client의 CLoader와 Editor의 CEditLoader가 동일한 프로토타입 등록을 각각 구현 |
| **레벨 로직 분리 불명확** | 게임 로직(몬스터 스폰, AI 등)이 레벨에 있는데, Editor가 이를 재사용할 수 없음 |

---

## 2. 목표 구조: 3계층 분리

```
┌─────────────────────────────────────────────────────┐
│  Client.exe          │  Editor.exe                   │
│  ├─ CMainApp         │  ├─ CEditorApp                │
│  ├─ CLevel_Logo      │  ├─ CLevel_EditLogo           │
│  ├─ CLevel_Loading   │  ├─ CLevel_EditLoading        │
│  └─ CLevel_GamePlay  │  ├─ CLevel_EditPlay           │  ← 실행층
│                      │  ├─ ImGui_Manager              │
│                      │  └─ Panel_MapTool 등           │
├──────────────────────┴───────────────────────────────┤
│  Game_PKM.dll (또는 .lib)                             │
│  ├─ CPlayer, CMonster, CForkLift                     │
│  ├─ CTerrain, CSky, CBackGround                      │  ← 구현층
│  ├─ CBody, CWeapon (PartObject)                      │
│  ├─ CGameLoader (공용 리소스 로딩)                    │
│  └─ Game_Defines.h                                   │
├──────────────────────────────────────────────────────┤
│  Engine.dll                                          │
│  ├─ CBase, CGameObject, CComponent, CLevel           │  ← 추상층
│  ├─ CGameInstance (싱글톤)                            │
│  ├─ CRenderer, CTransform, ...                       │
│  └─ 매니저들 (Prototype, Object, Level)               │
└──────────────────────────────────────────────────────┘
```

**의존 방향 (단방향만 허용)**:
```
Client.exe ──→ Game_PKM ──→ Engine.dll
Editor.exe ──→ Game_PKM ──→ Engine.dll
Editor.exe ──→ Engine.dll (직접 참조도 가능)

Engine.dll ←✕─ Game_PKM    (엔진은 게임을 모름)
Game_PKM   ←✕─ Client/Editor (게임은 실행층을 모름)
```

---

## 3. 질문 1: GameObject 상속 구조

### 3.1 상속 계층도

```
[Engine.dll]                [Game_PKM.dll]              [Client/Editor]
                                                        (레벨에서 사용)
CBase
 └─ CGameObject (abstract)
     │  - Initialize_Prototype()
     │  - Initialize(void* pArg)
     │  - Priority_Update()
     │  - Update()
     │  - Late_Update()
     │  - Render()
     │  - Clone() = 0
     │
     ├─── CPlayer                   ← Game_PKM에서 구현
     │     └─ CBody, CWeapon (Part)
     ├─── CMonster                  ← Game_PKM에서 구현
     ├─── CForkLift                 ← Game_PKM에서 구현
     ├─── CTerrain                  ← Game_PKM에서 구현
     ├─── CSky                      ← Game_PKM에서 구현
     ├─── CBackGround               ← Game_PKM에서 구현
     └─── CSnow, CSprite, CParticle ← Game_PKM에서 구현
```

### 3.2 핵심 원칙: 게임 오브젝트는 Game_PKM에만 존재

Client와 Editor 모두 **동일한 CPlayer, CMonster를 그대로 사용**한다. 게임 오브젝트 자체를 Client/Editor가 상속하지 않는다.

```cpp
// Game_PKM/Public/Player.h
class GAME_DLL CPlayer final : public CGameObject
{
public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(void* pArg) override;
    void Priority_Update(_float fTimeDelta) override;
    void Update(_float fTimeDelta) override;
    void Late_Update(_float fTimeDelta) override;
    HRESULT Render() override;
    CGameObject* Clone(void* pArg) override;

    static CPlayer* Create(ID3D11Device*, ID3D11DeviceContext*);
    void Free() override;
};
```

Client.exe와 Editor.exe 양쪽에서 동일하게:
```cpp
// 프로토타입 등록 (Loader에서)
pGameInstance->Add_Prototype(LEVEL::STATIC, L"Proto_Player", CPlayer::Create(pDevice, pContext));

// 오브젝트 생성 (Level::Initialize에서)
pGameInstance->Add_GameObject(eLevelID, L"Proto_Player", L"Layer_Player", &desc);
```

### 3.3 화면에 띄우는 흐름

**Client (게임 실행)**:
```
CMainApp::Update()
  → CGameInstance::Update_Engine(fTimeDelta)
    → CObject_Manager::Priority_Update()  ← 모든 게임 오브젝트 갱신
    → CObject_Manager::Update()
    → CObject_Manager::Late_Update()      ← CRenderer에 렌더 등록
  → CGameInstance::Draw()
    → CRenderer::Render()                 ← 등록된 오브젝트 그리기
```

**Editor (에디터 실행)**:
```
CEditorApp::Update()
  → CGameInstance::Update_Engine(fTimeDelta)
    → 동일 흐름 (게임 오브젝트 갱신/렌더 등록)
  → CGameInstance::Draw()
    → CRenderer::Render()                 ← 게임 오브젝트 그리기
  → CImGui_Manager::Render()             ← ImGui 오버레이 (에디터 전용)
```

**결론**: 게임 오브젝트의 갱신/렌더링은 Engine의 매니저가 처리하므로, Client든 Editor든 같은 오브젝트를 같은 방식으로 화면에 띄울 수 있다. 차이는 **어떤 Level이 어떤 오브젝트를 생성하느냐**에 있다.

---

## 4. 질문 2: Level 상속 구조 — Editor의 확장 방식

### 4.1 문제 정의

- **게임 로직**은 레벨 안에 있다 (몬스터 스폰, 웨이브, 승리 조건 등)
- **Client**: Game_PKM의 레벨을 그냥 돌리면 된다
- **Editor**: 게임 레벨의 오브젝트들을 화면에 보면서 + 편집 기능이 필요하다

그렇다면 Editor의 레벨은 게임 레벨을 어떻게 활용해야 하는가?

### 4.2 선택지 비교

#### 방안 A: 상속 (Editor Level → Game Level)

```
CLevel (Engine)
 └─ CLevel_GamePlay (Game_PKM)
      └─ CLevel_EditPlay (Editor)  ← 게임 레벨을 상속해서 확장
```

```cpp
// Editor/Public/Level_EditPlay.h
class CLevel_EditPlay : public CLevel_GamePlay  // Game_PKM 레벨 상속
{
    HRESULT Initialize() override {
        CLevel_GamePlay::Initialize();    // 게임 오브젝트 생성
        Ready_EditorTools();              // 에디터 전용 초기화
    }
    void Update(_float fTimeDelta) override {
        if (m_bPlayMode)
            CLevel_GamePlay::Update(fTimeDelta);  // 게임 로직 실행
        // 에디터 전용: 피킹, 기즈모, 패널
    }
};
```

| 장점 | 단점 |
|------|------|
| 간단한 구조 | Game_PKM 변경 시 Editor도 재빌드 |
| 게임 로직 자연스럽게 재사용 | 게임 레벨의 내부 구현에 강하게 결합 |
| 오버라이드로 동작 제어 가능 | 게임 레벨이 final이면 불가 |

#### 방안 B: 컴포지션 (Editor Level이 Game Level을 소유)

```
CLevel (Engine)
 ├─ CLevel_GamePlay (Game_PKM)    ← 게임 레벨 (독립)
 └─ CLevel_EditPlay (Editor)     ← 에디터 레벨 (게임 레벨을 멤버로 소유)
```

```cpp
// Editor/Public/Level_EditPlay.h
class CLevel_EditPlay : public CLevel  // Engine 레벨만 상속
{
    CLevel_GamePlay* m_pGameLevel;     // 게임 레벨을 멤버로 소유

    HRESULT Initialize() override {
        m_pGameLevel = CLevel_GamePlay::Create(m_pDevice, m_pContext);
        m_pGameLevel->Initialize();     // 게임 오브젝트 생성 위임
        Ready_EditorTools();
    }
    void Update(_float fTimeDelta) override {
        if (m_bPlayMode)
            m_pGameLevel->Update(fTimeDelta);  // 선택적으로 게임 로직 실행
        Update_EditorTools(fTimeDelta);
    }
};
```

| 장점 | 단점 |
|------|------|
| 느슨한 결합 | 약간의 간접 호출 오버헤드 |
| 에디터가 게임 로직 실행 여부를 완전히 제어 | 게임 레벨의 내부 상태 접근이 번거로울 수 있음 |
| 게임 레벨을 교체 가능 (Logo↔GamePlay) | Forwarding 코드가 필요할 수 있음 |

#### 방안 C: 공용 로더만 공유, 레벨은 독립 (현재 구조 확장)

```
CLevel (Engine)
 ├─ CLevel_GamePlay (Game_PKM)    ← Client 전용 레벨
 └─ CLevel_EditPlay (Editor)     ← Editor 전용 레벨

CGameLoader (Game_PKM)            ← 공용 리소스/프로토타입 등록
```

```cpp
// Editor/Public/Level_EditPlay.h
class CLevel_EditPlay : public CLevel  // Engine 레벨만 상속
{
    HRESULT Initialize() override {
        // Game_PKM의 Loader로 동일한 프로토타입 등록
        CGameLoader::Ready_Resources_For_GamePlay(m_pDevice, m_pContext);

        // 에디터 방식으로 오브젝트 배치 (게임과 다른 로직)
        Ready_EditorObjects();
        Ready_EditorTools();
    }
    void Update(_float fTimeDelta) override {
        // 게임 로직 없이, 에디터 자체 갱신만
        Update_EditorObjects(fTimeDelta);
        Update_EditorTools(fTimeDelta);
    }
};
```

| 장점 | 단점 |
|------|------|
| 완전한 독립 — 에디터가 게임 로직에 전혀 의존 안 함 | 게임 레벨의 오브젝트 구성을 재현하려면 별도 로직 필요 |
| 에디터 특화 최적화 가능 | Loader의 리소스 등록만 공유, 레벨 초기화는 각자 구현 |
| 현재 구조에서 자연스러운 확장 | 게임 플레이 테스트(PIE) 구현 시 게임 레벨을 별도로 로드해야 함 |

### 4.3 권장안: 방안 C (공용 로더) + 필요 시 방안 B (컴포지션) 병행

현재 프로젝트 상황에 가장 적합한 구조:

```
[Engine.dll]
CLevel (abstract)
 │  Initialize() / Update() / Render() / Free()

[Game_PKM.dll]
CLevel_GamePlay : CLevel          ← 게임 전용 레벨 (게임 로직 포함)
CGameLoader : CBase               ← 공용 프로토타입 등록기 (NEW)
 │  + Ready_Resources_For_Logo()
 │  + Ready_Resources_For_GamePlay()
 │  ※ 기존 CLoader에서 리소스 등록 부분만 분리

[Client.exe]
CLevel_Logo    : CLevel           ← CGameLoader 사용
CLevel_Loading : CLevel           ← CGameLoader 사용, 스레드 로딩
CLevel_GamePlay                   ← Game_PKM에서 직접 사용

[Editor.exe]
CLevel_EditLogo    : CLevel       ← CGameLoader 사용
CLevel_EditLoading : CLevel       ← CGameLoader 사용
CLevel_EditPlay    : CLevel       ← CGameLoader로 프로토타입 등록 후 에디터 방식으로 오브젝트 관리
```

### 4.4 권장 구조의 상세 설계

#### 4.4.1 CGameLoader — Loader에서 리소스 등록 분리

```cpp
// Game_PKM/Public/GameLoader.h
class GAME_DLL CGameLoader final : public CBase
{
public:
    // 정적 메서드: 어디서든 호출 가능
    static HRESULT Ready_Resources_For_Logo(
        ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
        CGameInstance* pGI);

    static HRESULT Ready_Resources_For_GamePlay(
        ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
        CGameInstance* pGI);

private:
    // 프로토타입 등록 헬퍼
    static HRESULT Ready_Prototype_GameObject(CGameInstance* pGI, ...);
    static HRESULT Ready_Prototype_Component(CGameInstance* pGI, ...);
};
```

- 기존 `CLoader::Ready_Resources_For_GamePlay()`에서 **프로토타입 등록 코드만** 추출
- Client의 `CLoader`와 Editor의 `CEditLoader`가 모두 이 클래스를 호출
- 스레드 관리(HANDLE, CRITICAL_SECTION)는 각 exe의 Loader가 담당

#### 4.4.2 Client의 Level — 변경 최소화

```cpp
// Client: 기존과 거의 동일
class CLevel_GamePlay : public CLevel  // Game_PKM 소속
{
    HRESULT Initialize() override {
        Ready_Lights();
        Ready_Layer_Camera(L"Layer_Camera");
        Ready_Layer_Player(L"Layer_Player");
        Ready_Layer_Monster(L"Layer_Monster");
        Ready_Layer_BackGround(L"Layer_BackGround");
        return S_OK;
    }
    void Update(_float fTimeDelta) override {
        // 게임 로직: 몬스터 스폰, 웨이브 관리 등
    }
};
```

Client의 `CMainApp`은 그냥 이 레벨을 `Change_Level()`로 설정하면 된다.

#### 4.4.3 Editor의 Level — 게임 오브젝트를 에디터 방식으로 관리

```cpp
// Editor: 게임 오브젝트를 사용하되 에디터 로직으로 관리
class CLevel_EditPlay : public CLevel  // Engine CLevel 직접 상속
{
    // 에디터 전용 상태
    EDITOR_MODE     m_eEditorMode = EDITOR_MODE::MAP;
    CGameObject*    m_pSelectedObject = nullptr;
    CPanel_MapTool* m_pMapToolPanel = nullptr;

    HRESULT Initialize() override {
        // Game_PKM의 오브젝트를 배치 (게임 레벨과 동일한 프로토타입 사용)
        m_pGameInstance->Add_GameObject(eLevelID, L"Proto_Terrain", L"Layer_Terrain", &desc);
        m_pGameInstance->Add_GameObject(eLevelID, L"Proto_Sky", L"Layer_Environment", &desc);

        // 에디터 전용 초기화
        Ready_EditorPanels();
        return S_OK;
    }

    void Update(_float fTimeDelta) override {
        // 에디터 모드에 따라 동작 분기
        switch (m_eEditorMode) {
        case EDITOR_MODE::MAP:
            Update_MapEditing(fTimeDelta);     // 터레인 편집
            break;
        case EDITOR_MODE::OBJECT:
            Update_ObjectPlacement(fTimeDelta); // 오브젝트 배치/선택/이동
            break;
        }
        // 게임 오브젝트 자체의 Update는 CObject_Manager가 자동 호출
    }

    HRESULT Render() override {
        // 기본 렌더링은 CRenderer가 처리 (게임과 동일)
        // 에디터 전용 오버레이: 그리드, 기즈모, 선택 하이라이트 등
        Render_EditorOverlay();
        return S_OK;
    }
};
```

#### 4.4.4 나중에 PIE(Play-in-Editor) 필요 시 — 방안 B 병행

게임을 에디터 안에서 테스트 실행하고 싶을 때:

```cpp
class CLevel_EditPlay : public CLevel
{
    CLevel_GamePlay* m_pGameLevel = nullptr;  // 게임 레벨 (PIE용)
    _bool m_bPIEMode = false;

    void StartPIE() {
        // 현재 에디터 상태를 직렬화 → 게임 레벨 생성 → 게임 로직 시작
        m_pGameLevel = CLevel_GamePlay::Create(m_pDevice, m_pContext);
        m_pGameLevel->Initialize();
        m_bPIEMode = true;
    }

    void StopPIE() {
        Safe_Release(m_pGameLevel);
        m_bPIEMode = false;
        // 에디터 상태 복원
    }

    void Update(_float fTimeDelta) override {
        if (m_bPIEMode)
            m_pGameLevel->Update(fTimeDelta);  // 게임 로직 실행
        else
            Update_EditorTools(fTimeDelta);     // 에디터 모드
    }
};
```

---

## 5. 전체 상속 구조 요약

```
[Engine.dll — 추상층]

CBase (RefCnt)
 ├── CGameObject (abstract)     ← Clone/Update/Render
 │    │
 │    │  [Game_PKM — 구현층]
 │    ├── CPlayer               ← 게임 오브젝트
 │    ├── CMonster
 │    ├── CForkLift
 │    ├── CTerrain
 │    ├── CSky
 │    ├── CBackGround
 │    └── CSnow, CSprite, CParticle
 │
 ├── CComponent (abstract)      ← Clone
 │    ├── CTransform             [Engine]
 │    ├── CRenderer              [Engine]
 │    ├── CBody                  [Game_PKM - PartObject]
 │    └── CWeapon                [Game_PKM - PartObject]
 │
 ├── CLevel (abstract)          ← Initialize/Update/Render
 │    │
 │    │  [Game_PKM]
 │    ├── CLevel_GamePlay        ← 게임 로직 (스폰, AI, 승리 조건)
 │    │
 │    │  [Client.exe]
 │    ├── CLevel_Logo            ← 클라이언트 전용
 │    ├── CLevel_Loading         ← 클라이언트 전용 (스레드 로딩)
 │    │   (CLevel_GamePlay는 Game_PKM 것을 직접 사용)
 │    │
 │    │  [Editor.exe]
 │    ├── CLevel_EditLogo        ← 에디터 전용
 │    ├── CLevel_EditLoading     ← 에디터 전용
 │    └── CLevel_EditPlay        ← 에디터 전용 (오브젝트 배치/편집)
 │         └─ (PIE 시 CLevel_GamePlay를 컴포지션으로 소유 가능)
 │
 ├── CGameLoader                 [Game_PKM — 공용 리소스 등록]
 │
 └── CGameInstance (singleton)   [Engine — 퍼사드]
```

---

## 6. LEVEL enum 설계

Game_PKM 분리 시 LEVEL enum도 분리 필요:

```cpp
// Engine/Public/Engine_Defines.h
// 엔진은 LEVEL을 정수(_uint)로만 취급 — enum 정의 없음

// Game_PKM/Public/Game_Defines.h
namespace Game_PKM {
    enum class LEVEL : _uint { STATIC, LOADING, LOGO, GAMEPLAY, END };
}

// Client/Public/Client_Defines.h
namespace Client {
    // Game_PKM의 LEVEL을 그대로 사용
    using LEVEL = Game_PKM::LEVEL;
}

// Editor/Public/Editor_Defines.h
namespace Editor {
    enum class LEVEL : _uint { STATIC, LOADING, EDITLOGO, EDITPLAY, END };
    // Editor는 독자적 LEVEL enum (게임과 다른 레벨 구성)
}
```

**핵심**: Engine의 매니저들(`CObject_Manager`, `CPrototype_Manager`)은 레벨 인덱스를 `_uint`로 받으므로 enum 타입에 무관하게 동작한다.

---

## 7. Export 매크로 설계

```cpp
// Game_PKM/Public/Game_Defines.h (신규)
#ifdef GAME_EXPORTS
#define GAME_DLL __declspec(dllexport)
#else
#define GAME_DLL __declspec(dllimport)
#endif
```

**적용 대상 (GAME_DLL)**: CPlayer, CMonster, CForkLift, CTerrain, CSky, CBackGround, CSnow, CSprite, CParticle, CBody, CWeapon, CCamera_Free, CLevel_GamePlay, CGameLoader

**미적용 (EXE 전용)**: CMainApp, CEditorApp, CLevel_Logo, CLevel_EditLogo, CPanel_* 등

---

## 8. 실행 흐름 비교

### Client.exe

```
WinMain
 → CMainApp::Create()
 → CMainApp::Initialize()
   → CGameInstance::Initialize_Engine()    ← 엔진 초기화
   → Start_Level(LEVEL::LOGO)             ← CLevel_Logo 시작
 → Loop:
   → CMainApp::Update(fTimeDelta)
     → CGameInstance::Update_Engine()      ← 모든 오브젝트 갱신
   → CMainApp::Render()
     → CGameInstance::Draw()               ← 모든 오브젝트 렌더
```

### Editor.exe

```
WinMain
 → CEditorApp::Create()
 → CEditorApp::Initialize()
   → CGameInstance::Initialize_Engine()    ← 동일 엔진 초기화
   → CImGui_Manager::Initialize()         ← ImGui 초기화 (에디터 전용)
   → Start_Level(LEVEL::EDITLOGO)         ← CLevel_EditLogo 시작
 → Loop:
   → CEditorApp::Update(fTimeDelta)
     → CGameInstance::Update_Engine()      ← 동일하게 오브젝트 갱신
     → CImGui_Manager::Update()           ← 에디터 패널 갱신
   → CEditorApp::Render()
     → CGameInstance::Draw()               ← 동일하게 오브젝트 렌더
     → CImGui_Manager::Render()           ← ImGui 오버레이
```

**공통점**: Engine.dll이 관리하는 게임 오브젝트의 Update/Render는 완전히 동일
**차이점**: Editor는 ImGui 레이어와 에디터 전용 레벨 로직이 추가됨

---

## 9. 구현 우선순위

| 순서 | 작업 | 난이도 | 비고 |
|------|------|--------|------|
| 1 | Game_PKM.lib 프로젝트 생성 | 하 | 솔루션에 Static Library 추가 |
| 2 | Client 게임 오브젝트 → Game_PKM 이동 | 중 | include 경로, namespace 정리 |
| 3 | CGameLoader 분리 | 중 | CLoader에서 리소스 등록 부분 추출 |
| 4 | Client.exe에서 Game_PKM.lib 링크 | 하 | 프로젝트 참조 추가 |
| 5 | Editor.exe에서 Game_PKM.lib 링크 | 하 | 프로젝트 참조 추가 |
| 6 | Editor 레벨에서 Game_PKM 오브젝트 사용 | 중 | CLevel_EditPlay 리팩토링 |
| 7 | (선택) Game_PKM.dll 전환 | 중~상 | export 매크로, 싱글톤 공유 검증 |

---

## 10. 주의사항

| 항목 | 설명 |
|------|------|
| **싱글톤 공유** | Static Library는 각 EXE에 복사 링크되므로, CGameInstance가 EXE별 독립 인스턴스를 가짐 (의도한 동작) |
| **RTTI/dynamic_cast** | DLL 전환 시 DLL 경계를 넘는 dynamic_cast에 주의 (PartObject 등) |
| **g_hWnd 위치** | 전역 변수는 각 EXE의 WinMain에서 정의 (Game_PKM에 포함하지 않음) |
| **스레드 로딩** | CGameLoader는 스레드 관리를 하지 않음 — 스레드 생성은 Client/Editor 각각의 Loader가 담당 |
| **Prototype 레벨 인덱스** | Client와 Editor의 LEVEL enum 값이 다르므로, STATIC(0)에 공유 리소스를 등록하는 패턴 유지 |
