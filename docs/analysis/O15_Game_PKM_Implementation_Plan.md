# O15. Game_PKM 정적 라이브러리 — 구체적 실현 방안

> O13(GameDLL 리팩토링 타당성), O14(상속 구조 설계) 후속
> 핵심 질문: LIB-EXE 경계에서 코드가 넘나들 때 기술적 문제가 있는가? + 구체적 이동 범위

### 현재 상태 (코드 검증 기준)

Game_PKM 프로젝트는 **이미 부분적으로 존재**:
- `Game_PKM_Defines.h` — `namespace Game_PKM`, LEVEL enum, `extern g_hWnd/g_hInstance` 정의 완료
- `CBackGround` — Game_PKM 네임스페이스로 구현 완료 (현재 Client의 CLoader에서 프로토타입 등록)
- `CPlayer` — Game_PKM 네임스페이스로 스켈레톤 존재
- CLevel_GamePlay, CLevel_EditPlay — **빈 스켈레톤** 상태
- CLoader::Ready_Resources_For_GamePlay() — **빈 구현**

> 아래 분석의 코드 예시 중 Ready_Layer_* 등의 패턴은
> **참고프로젝트1(Framework) 기반 설계**이며, 현재 프로젝트에는 미구현 상태.

---

## 질문 1: LIB-EXE 경계에서 코드가 넘나드는 것이 기술적으로 문제 없는가?

### 결론: **Static Library(.lib)이므로 경계 문제 자체가 없음**

### 1.1 Static Library vs DLL — 경계 문제 비교

| 잠재적 문제 | DLL 경우 | Static Library(.lib) 경우 |
|------------|---------|--------------------------|
| **싱글톤 인스턴스 분리** | DLL마다 별도 힙 → 인스턴스 분리 가능 | .lib 코드가 EXE에 직접 링크 → **동일 인스턴스** |
| **RTTI / dynamic_cast** | DLL 경계에서 타입 정보 불일치 가능 | **단일 바이너리** → 문제 없음 |
| **메모리 할당/해제** | DLL에서 new, EXE에서 delete → 크래시 가능 | **동일 CRT** → 문제 없음 |
| **Export 매크로** | `__declspec(dllexport/dllimport)` 필수 | **불필요** |
| **초기화 순서** | DllMain 타이밍 이슈 | DllMain 없음 → **해당 없음** |

### 1.2 현재 프로젝트에서의 검증

Game_PKM.lib의 코드가 EXE(Client/Editor)에서 호출될 때의 흐름:

```
[Client.exe 링크 시]
  Engine.dll  ←── import lib으로 링크 (DLL)
  Game_PKM.lib ←── 코드가 Client.exe 바이너리에 직접 포함 (.lib)

→ Game_PKM의 코드는 Client.exe의 일부로 동작
→ Engine.dll의 CGameInstance::GetInstance()를 호출하면
   Client.exe가 직접 호출하는 것과 완전히 동일한 인스턴스 반환
```

**CGameInstance 싱글톤 공유 확인**: `DECLARE_SINGLETON(CGameInstance)`로 Engine.dll에 static 멤버가 존재. .lib 코드는 EXE에 링크되어 같은 import 경로로 접근하므로 **동일 인스턴스 보장**.

### 1.3 Prototype_Manager 공존 (코드 근거)

`CPrototype_Manager`는 `(iLevelIndex, strTag)` 쌍만으로 관리하며, 등록 주체를 구분하지 않음:

```cpp
// Prototype_Manager.cpp — Add_Prototype (실제 코드)
m_pPrototypes[iLevelIndex].emplace(strPrototypeTag, pPrototype);
```

`Clone_Prototype()`도 `CBase*` → `dynamic_cast` → `Clone()` 패턴 — Static Library이므로 RTTI 문제 없음.

| 체크 항목 | 결과 |
|----------|------|
| Prototype_Manager가 등록 소스를 구분하는가? | **아니오** — 태그 쌍만 관리 |
| Static Library가 같은 싱글톤을 공유하는가? | **예** — 동일 프로세스, 동일 Engine.dll import |
| dynamic_cast가 LIB-EXE 경계에서 동작하는가? | **예** — 단일 바이너리이므로 경계 자체가 없음 |
| 태그명 충돌 가능성? | **사용자 주의** — 동일 레벨/동일 태그 중복 시 `E_FAIL` |

### 1.4 실행 흐름 예시

```
[CMainApp::Initialize()]
  → Ready_Prototype_For_Static()
    → Add_Prototype(STATIC, "Proto_Comp_VIBuffer_Rect", CVIBuffer_Rect::Create(...))
    ← 엔진 컴포넌트가 STATIC 슬롯에 등록

[CLoader::Loading() — 워커 스레드]
  → Game_PKM의 리소스 등록 함수 호출
    → Add_Prototype(GAMEPLAY, "Proto_GO_Player", CPlayer::Create(...))
    ← Game_PKM 클래스의 프로토타입이 같은 Prototype_Manager에 등록

[CLevel_GamePlay::Initialize()] — Game_PKM 소속
  → Ready_Layer_Player()
    → Add_GameObject → Clone_Prototype → CPlayer::Clone()
      → CPlayer::Initialize() → Clone_Prototype(COMPONENT, STATIC, "Proto_Comp_VIBuffer_Rect")
      ← CMainApp이 등록한 엔진 컴포넌트를 Game_PKM 객체가 Clone — 정상 동작
```

---

## 질문 2: CLevel 하위 클래스 — 어디까지 Game_PKM으로 옮기는가?

### 결론: **공유 로직이 있는 레벨은 lib로, LEVEL enum도 함께 이동이 원칙**

### 2.1 판단 기준: "독립적인 레벨"인가, "로직이 겹치는 레벨"인가?

| 판단 | lib로 이동 | 각 EXE에 남김 |
|------|-----------|-------------|
| 기준 | Client/Editor 양쪽에서 **동일한 로직**이 필요 | EXE별로 **완전히 다른 흐름** (고유 UI, 고유 전환) |
| 예시 | GamePlay (게임 오브젝트 배치/업데이트) | Client 전용 Logo, Editor 전용 EditLogo |

### 2.2 현재 코드 기반 분석

**CLevel_GamePlay** — 게임 로직의 핵심. Client/Editor 양쪽에서 공유해야 함:
```cpp
// 현재: Client_Defines.h 포함, NS_BEGIN(Client)
// Game_PKM으로 이동 시: Game_PKM_Defines.h 포함, NS_BEGIN(Game_PKM)
// 의존성: g_hWnd(디버그용), CGameInstance — 모두 Game_PKM_Defines.h에서 해결 가능
```

**CLevel_Logo vs CLevel_EditLogo** — 각 EXE의 독자적인 진입 흐름:
- Client: `CLevel_Logo` → BackGround 배치, ENTER 키 → `CLevel_Loading::Create(LEVEL::GAMEPLAY)`
- Editor: `CLevel_EditLogo` → (별도 UI), → `CLevel_EditLoading::Create(LEVEL::EDITPLAY)`
- **서로 다른 전환 대상, 다른 UI** → 각 EXE에 남기는 것이 적합

**CLevel_Loading / CLevel_EditLoading** — 스레드 관리 + 레벨 전환 switch가 EXE별로 다름:
```cpp
// Level_Loading.cpp — Client 전용 switch
case LEVEL::LOGO:    pNextLevel = CLevel_Logo::Create(...);     break;
case LEVEL::GAMEPLAY: pNextLevel = CLevel_GamePlay::Create(...); break;
```
- 전환 대상 레벨 클래스를 **직접 Create** → EXE별 구체 레벨에 하드 의존
- Loading 자체의 **전환 흐름**은 EXE에 남기되, **리소스 등록 로직**은 lib로 분리 (질문 3에서 상세)

### 2.3 LEVEL enum class — 서순 문제 정정

기존 분석에서는 "Client의 LEVEL enum을 쓰니까 이동이 부적합하다"고 했으나, **이것은 서순이 잘못됨**.

올바른 논리:
1. 공유 로직(GamePlay 등)을 Game_PKM으로 옮기기로 결정
2. → 그 로직이 사용하는 LEVEL enum도 당연히 Game_PKM에 정의
3. → 각 EXE의 Defines.h가 Game_PKM_Defines.h를 include하여 enum 사용

현재 LEVEL enum 현황:
```cpp
// Client:   enum class LEVEL { STATIC, LOADING, LOGO,     GAMEPLAY, END };
// Editor:   enum class LEVEL { STATIC, LOADING, EDITLOGO, EDITPLAY, END };
// Game_PKM: enum class LEVEL { STATIC, LOADING, LOGO,     GAMEPLAY, END };
```

**설계 방향**: Game_PKM에 공통 레벨(STATIC, LOADING, GAMEPLAY)을 정의하고, 각 EXE는 자신만의 레벨을 추가.

```cpp
// Game_PKM_Defines.h — 공통 LEVEL 정의
enum class LEVEL { STATIC, LOADING, LOGO, GAMEPLAY, END };
// ※ LOGO는 Client/Editor 모두 "로고 → 로딩" 흐름이 있으므로 공통으로 둘 수 있음
// ※ Editor가 EDITLOGO 대신 LOGO를 쓰되, 구체적인 CLevel 구현만 달리하는 방식도 가능
```

또는 Editor가 정말 다른 레벨 슬롯이 필요하면:
```cpp
// Game_PKM_Defines.h
enum class LEVEL { STATIC, LOADING, LOGO, GAMEPLAY, EDITPLAY, END };
// 확장된 통합 enum — Client는 EDITPLAY를 안 쓰고, Editor는 GAMEPLAY를 그대로 쓸 수 있음
```

어떤 방식이든, **enum이 lib에 있고 EXE가 이를 참조**하는 것이 올바른 방향. 현재 `Game_PKM_Defines.h`에 이미 LEVEL enum이 정의되어 있으므로 구조적으로 준비 완료.

### 2.4 이동 시 필요한 변경 (CLevel_GamePlay 기준)

현재 CLevel_GamePlay는 빈 스켈레톤이므로 변경이 매우 간단:

1. **`#include "Client_Defines.h"` → `#include "Game_PKM_Defines.h"`**
2. **`NS_BEGIN(Client)` → `NS_BEGIN(Game_PKM)`**
3. **`g_hWnd`**: Game_PKM_Defines.h에 이미 `extern HWND g_hWnd;` 선언 → 변경 불필요
4. **레벨 전환 의존 제거**: Game_PKM의 레벨 클래스는 다른 레벨을 직접 Create하지 않음 (현재도 안 함)

Client의 CLevel_Loading에서 Game_PKM의 CLevel_GamePlay를 사용할 때:
```cpp
#include "Level_GamePlay.h"  // Game_PKM의 Public 디렉터리에서 include
// Game_PKM_Defines.h의 using namespace Game_PKM;에 의해 CLevel_GamePlay 접근 가능
```

### 2.5 권장 구조

```
Game_PKM.lib:
  CLevel_GamePlay    ← Client/Editor 양쪽에서 사용하는 게임 레벨
  CPlayer, CBackGround, ...  ← 게임 오브젝트
  LEVEL enum class   ← Game_PKM_Defines.h에 정의 (이미 존재)

Client.exe:
  CLevel_Logo        ← Client 고유 진입 흐름
  CLevel_Loading     ← Client 고유 전환 흐름 (리소스 등록은 Game_PKM에 위임)

Editor.exe:
  CLevel_EditLogo    ← Editor 고유 진입 흐름
  CLevel_EditLoading ← Editor 고유 전환 흐름
  CLevel_EditPlay    ← Editor 전용 편집 모드 (Game_PKM 오브젝트 사용, ImGui 연동)
```

---

## 질문 3: CLoader의 리소스 등록 로직을 Game_PKM으로 분리하는 방안

### 결론: **프로토타입 등록 로직을 Game_PKM으로, 스레드 관리는 각 EXE에 남김**

### 3.1 CLoader의 두 가지 역할

현재 CLoader는 한 클래스에 두 역할을 담고 있음:

| 역할 | 내용 | 위치 |
|------|------|------|
| **스레드 관리** | `_beginthreadex`, `CRITICAL_SECTION`, `m_isFinished` | EXE 종속 (로딩 UI, 전환 흐름) |
| **리소스/프로토타입 등록** | `Ready_Resources_For_Logo()`, `Ready_Resources_For_GamePlay()` | 게임 콘텐츠 종속 → **lib로 이동 대상** |

Client CLoader와 Editor CEditLoader의 구조가 거의 동일 (스레드 생성, CS, Loading() switch문) — **리소스 등록 부분만 다름**.

### 3.2 분리 설계

```
[Game_PKM.lib]
CGameLoader (신규)
  ├─ Ready_Resources_For_Logo(_uint iLevelIndex)
  ├─ Ready_Resources_For_GamePlay(_uint iLevelIndex)
  └─ (공통 프로토타입 등록 전담)

[Client.exe]
CLoader (스레드 관리 유지)
  ├─ m_hThread, m_CriticalSection, m_isFinished
  ├─ Loading() → CGameLoader::Ready_Resources_For_*() 호출
  └─ Show() → SetWindowText(g_hWnd, ...)

[Editor.exe]
CEditLoader (스레드 관리 유지)
  ├─ m_hThread, m_CriticalSection, m_isFinished
  ├─ Loading() → CGameLoader::Ready_Resources_For_*() 호출
  └─ Show() → SetWindowText(g_hWnd, ...)
```

### 3.3 CGameLoader 설계 예시

```cpp
// Game_PKM/Public/GameLoader.h
class CGameLoader final : public CBase {
public:
    static HRESULT Ready_Resources_For_Logo(
        ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
        CGameInstance* pGI, _uint iLevelIndex);
    static HRESULT Ready_Resources_For_GamePlay(
        ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
        CGameInstance* pGI, _uint iLevelIndex);
};
```

iLevelIndex를 인자로 받으므로 **enum 값을 호출자가 결정** — Client/Editor 어느 쪽에서든 자신의 레벨 인덱스로 호출 가능.

### 3.4 Client CLoader 변경 후

```cpp
HRESULT CLoader::Loading()
{
    EnterCriticalSection(&m_CriticalSection);
    HRESULT hr = { S_OK };
    switch (m_eNextLevelID) {
    case LEVEL::LOGO:
        hr = CGameLoader::Ready_Resources_For_Logo(
            m_pDevice, m_pContext, m_pGameInstance, ETOUI(LEVEL::LOGO));
        break;
    case LEVEL::GAMEPLAY:
        hr = CGameLoader::Ready_Resources_For_GamePlay(
            m_pDevice, m_pContext, m_pGameInstance, ETOUI(LEVEL::GAMEPLAY));
        break;
    }
    LeaveCriticalSection(&m_CriticalSection);
    if (FAILED(hr)) return E_FAIL;
    m_isFinished = true;
    return S_OK;
}
```

**차이점**: `m_isFinished = true`가 각 `Ready_Resources_For_*()` 내부가 아니라 `Loading()` 끝에서 설정. 로딩 메시지도 CLoader가 관리.

### 3.5 스레드 안전성

| 항목 | 상태 |
|------|------|
| CGameLoader에서 CGameInstance 호출 | **안전** — 현재도 워커 스레드에서 동일하게 호출 |
| Add_Prototype() 동시 접근 | **안전** — 메인 스레드는 로딩 중 Prototype_Manager 미접근 |
| CRITICAL_SECTION | **각 EXE Loader가 관리** — CGameLoader는 스레드를 모름 |

### 3.6 Free() 패턴 보존

스레드 정리 로직은 각 EXE의 Loader에 그대로 유지:
```cpp
void CLoader::Free() {
    WaitForSingleObject(m_hThread, INFINITE);
    DeleteCriticalSection(&m_CriticalSection);
    CloseHandle(m_hThread);
    // ... Safe_Release
}
```

---

## 종합 정리

### 질문별 답변

| 질문 | 답변 | 핵심 근거 |
|------|------|----------|
| LIB-EXE 경계 기술적 문제? | **없음** | .lib는 EXE에 링크 → 단일 바이너리, 싱글톤 공유, RTTI 정상 |
| LEVEL enum을 lib로? | **당연히 이동** | 공유 로직이 lib로 가면 enum도 따라감 (서순: 코드 → enum) |
| Level 클래스 이동 범위? | **GamePlay → lib, Logo/Loading → EXE** | 공유 로직은 lib, EXE 고유 흐름은 EXE |
| Loader 분리? | **리소스 등록만 lib로** | 스레드 관리는 EXE, 프로토타입 등록은 Game_PKM |

### 최종 파일 배치

```
Game_PKM.lib:
  Public/
    Game_PKM_Defines.h    ← LEVEL enum 정의 (이미 존재, 권위적 소스)
    GameLoader.h          ← 신규: 프로토타입 등록 전용
    Level_GamePlay.h      ← Client에서 이동
    BackGround.h          ← 이미 존재
    Player.h              ← 이미 존재
  Private/
    GameLoader.cpp
    Level_GamePlay.cpp
    BackGround.cpp, Player.cpp  ← 이미 존재

Client.exe:
  Public/
    Client_Defines.h      ← Game_PKM_Defines.h include + Client 전용 설정
    Loader.h              ← 스레드 관리, CGameLoader 호출
    Level_Logo.h          ← Client 고유
    Level_Loading.h       ← Client 고유
  Private/
    Client.cpp            ← WinMain, g_hWnd/g_hInstance 정의

Editor.exe:
  Public/
    Editor_Defines.h      ← Game_PKM_Defines.h include + Editor 전용 설정
    EditLoader.h          ← 스레드 관리, CGameLoader 호출
    Level_EditLogo.h      ← Editor 고유
    Level_EditLoading.h   ← Editor 고유
    Level_EditPlay.h      ← Editor 전용 (Game_PKM 오브젝트 + ImGui)
  Private/
    Editor.cpp            ← WinMain, g_hWnd/g_hInstance 정의
```

### 네임스페이스 전환

- Game_PKM_Defines.h: `using namespace Game_PKM;`
- Client_Defines.h가 Game_PKM_Defines.h를 include하면 Game_PKM 네임스페이스 자동 사용 가능
- Client 전용 설정(윈도우 크기 등)만 `namespace Client`에 추가 정의
