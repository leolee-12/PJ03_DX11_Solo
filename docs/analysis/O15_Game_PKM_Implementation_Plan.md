# O15. Game_PKM 정적 라이브러리 — 구체적 실현 방안

> O13(GameDLL 리팩토링 타당성), O14(상속 구조 설계) 후속
> 핵심 질문: Client의 Logo/Loading(Loader)/GamePlay를 lib(또는 dll)로 옮겨 Editor와 공유할 수 있는가?

### 배경

현재 Client에 구현된 Logo, Loading(Loader), GamePlay를 Editor에서도 사용하고 싶다.
이를 위해 Game_PKM 프로젝트(정적 라이브러리)로 옮기는 것을 고려 중.
**Editor에 현재 존재하는 EditLogo/EditLoading/EditPlay 등의 클래스와는 무관한 논의**이며,
순수하게 "LIB-EXE 경계를 넘나드는 것이 기술적으로 가능한가?"가 핵심 질문이다.

### 현재 상태 (코드 검증 기준)

Game_PKM 프로젝트는 **이미 부분적으로 존재**:
- `Game_PKM_Defines.h` — `namespace Game_PKM`, LEVEL enum, `extern g_hWnd/g_hInstance` 정의 완료
- `CBackGround` — Game_PKM 네임스페이스로 구현 완료 (현재 Client의 CLoader에서 프로토타입 등록)
- `CPlayer` — Game_PKM 네임스페이스로 스켈레톤 존재
- CLevel_GamePlay — **빈 스켈레톤** 상태
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
| **스레드 생성** | DLL에서 `_beginthreadex` 시 CRT 초기화 주의 | .lib는 EXE의 CRT에 바인딩 → **문제 없음** |
| **Win32 API 호출** | 어디서든 가능 | 어디서든 가능 → **문제 없음** |

**결론**: Static Library는 링크 시 EXE 바이너리에 코드가 직접 포함되므로, "경계"라는 개념 자체가 성립하지 않는다. `_beginthreadex`, `SetWindowText`, `g_hWnd` 접근 등 모든 것이 EXE에서 직접 실행하는 것과 완전히 동일하게 동작한다.

### 1.2 현재 프로젝트에서의 검증

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
| dynamic_cast가 LIB 코드에서 동작하는가? | **예** — 단일 바이너리이므로 경계 자체가 없음 |
| 태그명 충돌 가능성? | **사용자 주의** — 동일 레벨/동일 태그 중복 시 `E_FAIL` |

### 1.4 실행 흐름 예시

```
[CMainApp::Initialize()]
  → Ready_Prototype_For_Static()
    → Add_Prototype(STATIC, "Proto_Comp_VIBuffer_Rect", CVIBuffer_Rect::Create(...))
    ← 엔진 컴포넌트가 STATIC 슬롯에 등록

[CLoader::Loading() — 워커 스레드, Game_PKM.lib 코드]
  → Ready_Resources_For_GamePlay()
    → Add_Prototype(GAMEPLAY, "Proto_GO_Player", CPlayer::Create(...))
    ← Game_PKM 클래스의 프로토타입이 같은 Prototype_Manager에 등록

[CLevel_GamePlay::Initialize() — Game_PKM.lib 코드]
  → Ready_Layer_Player()
    → Add_GameObject → Clone_Prototype → CPlayer::Clone()
      → CPlayer::Initialize() → Clone_Prototype(COMPONENT, STATIC, "Proto_Comp_VIBuffer_Rect")
      ← CMainApp이 등록한 엔진 컴포넌트를 Game_PKM 객체가 Clone — 정상 동작
```

---

## 질문 2: Client의 Level 클래스들을 통째로 lib에 넣을 수 있는가?

### 결론: **기술적으로 전부 가능. LEVEL enum도 당연히 함께 이동**

질문 1에서 확인한 대로, Static Library에서는 경계 문제가 없으므로 Logo, Loading, GamePlay **모두** lib로 이동이 기술적으로 가능하다. 아래는 각 레벨별 이동 시 검토사항.

### 2.1 CLevel_GamePlay — 이동에 아무 문제 없음

```cpp
// 현재: Client_Defines.h 포함, NS_BEGIN(Client)
// 의존성: g_hWnd(디버그용), CGameInstance, 게임 오브젝트들
// → 모두 Game_PKM_Defines.h 또는 Engine에서 해결 가능
```

게임 오브젝트(Player, Monster 등)와 같은 lib에 들어가므로 의존성 문제 없음.

### 2.2 CLevel_Logo — 이동 가능

현재 의존성:
```cpp
// Level_Logo.cpp 실제 코드
#include "Level_Logo.h"       // Client_Defines.h → Game_PKM_Defines.h로 변경
#include "Level_Loading.h"    // CLevel_Loading이 같은 lib에 있으면 문제 없음
#include "GameInstance.h"     // Engine — 문제 없음
```

- `g_hWnd`: Game_PKM_Defines.h에 이미 `extern HWND g_hWnd;` 선언 → 문제 없음
- `CLevel_Loading::Create()` 호출: Loading도 함께 lib로 가면 → 문제 없음
- `Ready_Layer_BackGround()`: CBackGround가 이미 Game_PKM에 있으므로 → 문제 없음

**단, Logo가 Client/Editor에서 완전히 다른 경우** (예: 클라이언트와 에디터가 서로 구별되는 로고를 사용, 전환 흐름이 다른 등) — 이런 경우에 한해서만 각 EXE에 별도 구현을 두는 것이 합당. 이것은 기술적 제약이 아니라 **설계 선택**의 문제.

### 2.3 CLevel_Loading + CLoader — 이동 가능

CLoader의 모든 기능이 lib에서 정상 동작:
| 기능 | lib에서 동작? | 이유 |
|------|-------------|------|
| `_beginthreadex` | **정상** | .lib는 EXE의 CRT에 바인딩, 스레드 생성에 제약 없음 |
| `CRITICAL_SECTION` | **정상** | Win32 커널 객체, 호출 위치 무관 |
| `SetWindowText(g_hWnd, ...)` | **정상** | Win32 API + extern 전역변수, lib에서도 동일 |
| `m_isFinished` (atomic) | **정상** | 표준 C++ atomic, 위치 무관 |
| `LEVEL enum switch` | **정상** | enum이 같은 lib에 있으면 문제 없음 |
| `CLevel_Logo::Create()`, `CLevel_GamePlay::Create()` 호출 | **정상** | 같은 lib 내 클래스 참조 |

**기존 분석 오류 정정**: 이전에 "Loading은 Client 전용 LEVEL enum에 의존하므로 이동 부적합"이라고 했으나, 이것은 서순이 잘못됨. 코드가 lib로 가면 LEVEL enum도 당연히 함께 가므로 의존성 문제가 되지 않는다.

### 2.4 LEVEL enum class — 서순 정정

올바른 논리:
1. 공유 로직(Logo, Loading, GamePlay 등)을 Game_PKM으로 옮기기로 결정
2. → 그 로직이 사용하는 LEVEL enum도 당연히 Game_PKM에 정의
3. → 각 EXE의 Defines.h가 Game_PKM_Defines.h를 include하여 enum 사용

현재 상태 — Game_PKM_Defines.h에 이미 LEVEL enum이 정의되어 있으므로 **구조적으로 준비 완료**:
```cpp
// Game_PKM_Defines.h (현재)
enum class LEVEL { STATIC, LOADING, LOGO, GAMEPLAY, END };
// Client_Defines.h에도 동일 enum이 있지만, lib 이동 후에는 Game_PKM 것을 사용
```

Client_Defines.h가 Game_PKM_Defines.h를 include하면, Client 자체의 LEVEL enum 정의는 제거하고 Game_PKM의 것을 사용하면 된다.

### 2.5 이동 시 필요한 공통 변경

각 클래스 이동 시 공통적으로:
1. **`#include "Client_Defines.h"` → `#include "Game_PKM_Defines.h"`**
2. **`NS_BEGIN(Client)` → `NS_BEGIN(Game_PKM)`**
3. **`g_hWnd`**: Game_PKM_Defines.h에 이미 `extern HWND g_hWnd;` 선언 → 변경 불필요

CLevel_Loading의 경우 추가로:
- `#include "Level_Logo.h"`, `#include "Level_GamePlay.h"` — 같은 lib에 있으므로 경로만 조정

---

## 질문 3: 설계 선택 — 전부 lib에 넣을 것인가, 일부만 넣을 것인가?

### 전제: 기술적으로는 전부 가능 (질문 1~2에서 확인)

여기서부터는 기술적 제약이 아닌 **설계 선택**의 문제. 두 가지 방향이 있다.

### 3.1 방안 A: Logo/Loading/Loader/GamePlay 전부 lib로

```
Game_PKM.lib:
  CLevel_Logo, CLevel_Loading, CLevel_GamePlay  ← 전부 이동
  CLoader                                        ← 스레드 관리 포함 전부 이동
  CPlayer, CBackGround, ...                      ← 게임 오브젝트

Client.exe:
  CMainApp                                       ← 엔진 초기화 + 루프만
  Client_Defines.h                               ← Game_PKM_Defines.h include
  Client.cpp                                     ← WinMain, g_hWnd 정의
```

**장점**: Client가 극도로 경량화. Editor에서 동일한 Logo/Loading/GamePlay를 그대로 재사용 가능.
**고려사항**: 향후 Editor가 고유한 로고나 로딩 흐름이 필요해지면, 그때 lib의 클래스를 상속하거나 별도 클래스를 EXE에 작성.

### 3.2 방안 B: 리소스 등록만 lib로 분리, 스레드 관리는 EXE

Logo/Loading의 **전환 흐름**은 각 EXE가 담당하고, **프로토타입 등록**만 Game_PKM에 위임:

```
Game_PKM.lib:
  CGameLoader (신규)      ← 프로토타입 등록 전용
  CLevel_GamePlay          ← 게임 레벨
  CPlayer, CBackGround     ← 게임 오브젝트

Client.exe:
  CLevel_Logo, CLevel_Loading  ← 전환 흐름 유지
  CLoader                       ← 스레드 관리 + CGameLoader 호출
```

**장점**: 각 EXE가 고유한 로딩 흐름을 자유롭게 커스터마이즈 가능.
**단점**: CLoader/CEditLoader에 동일한 스레드 관리 코드가 중복.

### 3.3 방안 B 상세 — CGameLoader 설계

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

Client CLoader 변경 후:
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

### 3.4 스레드 안전성 (양쪽 방안 공통)

| 항목 | 상태 |
|------|------|
| lib 코드에서 CGameInstance 호출 | **안전** — 현재도 워커 스레드에서 동일하게 호출 |
| Add_Prototype() 동시 접근 | **안전** — 메인 스레드는 로딩 중 Prototype_Manager 미접근 |
| lib에서 `_beginthreadex` | **정상** — EXE의 CRT에 바인딩 |

---

## 종합 정리

### 핵심 답변

| 질문 | 답변 |
|------|------|
| LIB-EXE 경계 기술적 문제? | **없음** — .lib는 EXE에 링크되어 단일 바이너리. 싱글톤, RTTI, 스레드, Win32 API 모두 정상 |
| LEVEL enum을 lib로? | **당연히 이동** — 코드가 lib로 가면 enum도 따라감 (이미 Game_PKM_Defines.h에 존재) |
| Logo/Loading 포함 전부 lib로 가능? | **가능** — 기술적 장벽 없음. "전부 옮기기(A)" vs "리소스 등록만 분리(B)"는 설계 선택 |

### LEVEL enum 서순 정정 (기존 분석 오류)

- **기존**: "Client의 LEVEL enum을 쓰니까 Logo/Loading 이동은 부적합"
- **정정**: 코드를 옮기면 enum도 함께 가는 것이 당연. enum 때문에 이동이 막히는 것이 아니라, **이동하기로 했으면 enum도 같이 가져가면 됨**

### 독립적 레벨의 의미 (예시)

기술적 문제가 아닌 **설계 선택**으로 EXE에 남길 수 있는 경우:
- 클라이언트와 에디터가 서로 구별되는 로고를 사용하는 경우
- 에디터만의 독자적인 레벨이 필요한 경우 (예: 맵 에디터 전용 레벨)
- 로딩 흐름이 근본적으로 달라야 하는 경우

이런 경우에도 lib의 공유 클래스를 **상속하여 확장**하거나, **컴포지션**으로 활용할 수 있다.
