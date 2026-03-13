# O13. Game DLL 분리 리팩토링 타당성 분석

> 분석 대상: 참고프로젝트1 (Framework)
> 목표: 클라이언트 구현층을 Game DLL로 분리하여 Client EXE + Tool EXE 양쪽에서 공유

---

## 1. 현재 구조 (As-Is)

```
Engine.dll (DLL)
  ├─ CGameInstance (퍼사드 싱글톤)
  ├─ CGameObject / CComponent / CLevel (추상 베이스, ENGINE_DLL export)
  ├─ CContainerObject / CPartObject (ENGINE_DLL export)
  ├─ CPrototype_Manager (프로토타입 레지스트리)
  ├─ CObject_Manager (레이어/오브젝트 관리)
  ├─ CRenderer, CPipeLine, CLight_Manager, ...
  └─ CTransform, CShader, CTexture, CModel, CCollider, ... (엔진 컴포넌트)

Client.exe (EXE)
  ├─ CMainApp (진입점, 엔진 초기화, 루프)
  ├─ CLoader (스레드 로딩, 프로토타입 등록)
  ├─ CLevel_Logo, CLevel_Loading, CLevel_GamePlay (CLevel 상속)
  ├─ CPlayer, CMonster, CForkLift, ... (CGameObject/CContainerObject 상속)
  ├─ CBody, CWeapon (CPartObject 상속)
  ├─ CCamera_Free (CCamera 상속)
  ├─ CTerrain, CSky, CSnow, ... (CGameObject 상속)
  └─ Client_Defines.h (LEVEL enum, 윈도우 크기 등)

Tool.exe (EXE) — 별도 프로젝트
  ├─ 자체 진입점
  └─ Engine.dll 참조 → 추상 베이스만 사용 가능
      ⚠️ CPlayer, CMonster 등 구현층 클래스에 접근 불가
```

### 핵심 문제
- Tool.exe가 Engine.dll만 참조하므로, 클라이언트에서 정의한 **구현층 클래스**(Player, Monster, Level_GamePlay 등)의 타입 정보와 Create/Clone 함수에 접근할 수 없다.
- Loader에서 `CPlayer::Create()`를 호출해야 프로토타입이 등록되는데, Tool.exe에는 CPlayer 심볼 자체가 없다.

---

## 2. 목표 구조 (To-Be)

### Stage 1: Static Library (.lib) — 우선 구현

```
Engine.dll (DLL) — 변경 없음
  └─ (기존과 동일)

Game.lib (Static Library) — 신규
  ├─ CPlayer, CMonster, CForkLift, ...    ← Client에서 이동
  ├─ CBody, CWeapon, CCamera_Free         ← Client에서 이동
  ├─ CTerrain, CSky, CSnow, CSprite, ...  ← Client에서 이동
  ├─ CLevel_Logo, CLevel_GamePlay, ...    ← Client에서 이동
  ├─ CLoader                              ← Client에서 이동
  └─ Game_Defines.h (LEVEL enum, 상수 등) ← Client_Defines에서 이동

Client.exe (EXE) — 경량화
  ├─ WinMain + 윈도우 생성 + g_hWnd/g_hInstance 정의
  ├─ CMainApp (엔진 초기화, Start_Level)
  └─ Game.lib 링크 → 구현층 클래스 사용

Tool.exe (EXE) — Game.lib 참조 추가
  ├─ WinMain + 툴 윈도우 생성 + g_hWnd/g_hInstance 정의
  ├─ CToolApp (엔진 초기화, 선택적 프로토타입 등록)
  └─ Game.lib 링크 → 구현층 클래스 사용 가능!
```

**핵심 이점**: DLL export 매크로 불필요, 싱글톤 공유 자동 보장, RTTI 문제 없음

### Stage 2: DLL 전환 — 안정성 확보 후

```
Engine.dll (DLL) — 변경 없음
Game.dll (DLL)   — Game.lib를 DLL로 전환
  └─ GAME_DLL export 매크로 추가, .lib 자동 생성

Client.exe / Tool.exe — Game.dll import로 전환
```

**전환 시점**: Tool 연동이 안정화되고, 핫 리로드나 바이너리 공유가 필요해질 때

---

## 3. 결론: 현실적으로 가능한가?

**가능하다.** 구조적으로 불가능한 장벽은 없다. 엔진이 이미 DLL로 분리되어 있고, 클라이언트 구현층은 엔진 추상 베이스를 상속하는 패턴이므로, 이를 별도 DLL로 뽑아내는 것은 기존 패턴의 자연스러운 확장이다.

다만 **난이도가 '쉽지는 않다'**. 아래 6개 영역에서 구체적으로 신경 써야 할 지점들이 있다.

---

## 4. 신경 써야 할 지점들

### 4-1. DLL Export 매크로 설계

**문제**: 현재 클라이언트 클래스(CPlayer 등)에는 export 매크로가 없다. Game.dll로 옮기면 `__declspec(dllexport/dllimport)` 처리가 필요하다.

**해결**:
```cpp
// Game_Defines.h
#ifdef GAME_EXPORTS
#define GAME_DLL __declspec(dllexport)
#else
#define GAME_DLL __declspec(dllimport)
#endif
```

**주의점**:
- Game.dll 프로젝트에서 `GAME_EXPORTS` 전처리기 정의 필요
- Game.dll이 Engine.dll을 **import** 해야 하므로, Game.dll 빌드 시 `ENGINE_EXPORTS`를 정의하면 **안 된다**
- 빌드 순서: `Engine.dll → Game.dll → Client.exe / Tool.exe`
- **모든 구현층 클래스**에 `GAME_DLL` 매크로를 붙여야 함 (누락 시 링크 에러)

### 4-2. 싱글톤 인스턴스 공유 (가장 핵심적인 문제)

**문제**: `CGameInstance`는 Engine.dll 내부의 static 멤버(`m_pInstance`)에 인스턴스가 저장된다. Engine.dll, Game.dll, Client.exe 모두 같은 Engine.dll의 심볼을 사용하므로 **이론적으로는** 하나의 인스턴스를 공유한다.

**왜 이것이 핵심인가**: 만약 싱글톤 인스턴스가 모듈별로 분리되면, Game.dll에서 `CGameInstance::GetInstance()`를 호출했을 때 Client.exe가 초기화한 것과 다른 인스턴스를 받게 되어 전체 시스템이 무너진다.

**확인 사항**:
- `DECLARE_SINGLETON` 매크로가 Engine.dll에 정의되고 `ENGINE_DLL`로 export되므로, 다른 모듈에서 `GetInstance()`를 호출하면 **Engine.dll의 static 변수**를 참조한다 → **정상 동작**
- 단, `CGameInstance`의 `GetInstance()`와 `DestroyInstance()`가 `ENGINE_DLL`로 export되어야 한다 → 현재 `CGameInstance` 클래스 자체가 `ENGINE_DLL`이므로 **문제 없음**
- ⚠️ **Game.dll 내부에 별도 싱글톤을 만드는 경우** (예: GameManager): `GAME_DLL`로 export해야 Client.exe와 공유됨

### 4-3. LEVEL enum과 Level ID 체계

**문제**: 현재 `LEVEL` enum이 `Client_Defines.h`에 클라이언트 전용으로 정의되어 있다.
```cpp
enum class LEVEL { STATIC, LOADING, LOGO, GAMEPLAY, END };
```
이 enum을 Game.dll로 옮기면 Client.exe와 Tool.exe 양쪽에서 참조 가능하지만, **Tool은 LOGO → LOADING → GAMEPLAY 흐름이 아닌 독자적 레벨 구조**를 가질 수 있다.

**해결 방안**:
- **방안 A**: Game.dll에 공통 LEVEL enum 정의 + Tool용 LEVEL 별도 정의
- **방안 B (권장)**: LEVEL을 정수 ID 기반으로 변경하고, 레벨 매핑을 런타임에 결정
  ```cpp
  // Game_Defines.h
  constexpr _uint LEVEL_STATIC   = 0;
  constexpr _uint LEVEL_LOADING  = 1;
  constexpr _uint LEVEL_LOGO     = 2;
  constexpr _uint LEVEL_GAMEPLAY = 3;
  // Tool은 LEVEL_TOOL_EDIT = 2 등 자체 상수 사용 가능
  ```
- **주의**: `iNumLevels` 초기화 시 Client와 Tool이 서로 다른 값을 전달할 수 있으므로, Game.dll의 Loader는 이를 유연하게 처리해야 함

### 4-4. Loader와 프로토타입 등록 — 의존성 역전 지점

**문제**: 현재 `CLoader::Loading_For_GamePlay()`에서 모든 구현층 클래스의 `Create()`를 직접 호출하여 프로토타입을 등록한다.
```cpp
#include "Player.h"      // 구현층 직접 #include
CPlayer::Create(...)     // 구현층 직접 호출
```
Game.dll로 옮기면 이 의존 관계는 해결되지만, **Tool이 같은 Loader를 쓸 수 있는가?**가 문제다.

**고려할 점**:
- Tool에서는 게임 전체 레벨 로딩이 필요 없을 수 있다 (특정 오브젝트만 배치/편집)
- Tool 전용 로딩 함수가 필요할 수 있음 (예: `Loading_For_Tool()`)

**해결 방안**:
```cpp
// Game.dll export 함수
GAME_DLL HRESULT Register_GamePlay_Prototypes(ID3D11Device*, ID3D11DeviceContext*);
GAME_DLL HRESULT Register_Common_Prototypes(ID3D11Device*, ID3D11DeviceContext*);

// Client → Register_Common + Register_GamePlay
// Tool   → Register_Common만 (또는 선택적 등록)
```

### 4-5. PartObject 간 크로스 캐스팅

**문제**: Player.cpp에서 Body의 소켓 본 행렬을 가져올 때 `dynamic_cast`를 사용한다:
```cpp
WeaponDesc.pSocketBoneMatrix =
    dynamic_cast<CBody*>(m_PartObjects[ENUM_CLASS(PARTOBJ::BODY)])
        ->Get_SocketBoneMatrix_Ptr("SWORD");
```
DLL 경계를 넘는 `dynamic_cast`는 **RTTI(Run-Time Type Information)**에 의존한다.

**주의점**:
- Engine.dll에서 CPartObject가 정의되고, Game.dll에서 CBody가 정의된다
- CBody의 RTTI 정보는 Game.dll에 존재하므로, Game.dll 내부에서의 `dynamic_cast`는 **정상 동작**
- ⚠️ **Tool.exe에서 Game.dll의 클래스로 dynamic_cast 시**: Tool.exe가 Game.dll의 타입 정보를 import하고 있으면 동작함 → `GAME_DLL` export가 정확히 되어 있어야 함
- **MSVC 기준**: 같은 컴파일러, 같은 RTTI 설정(`/GR`)이면 DLL 경계 dynamic_cast 정상 동작

### 4-6. 전역 변수 (g_hWnd, g_hInstance 등) 소속 문제

**문제**: 현재 `g_hWnd`, `g_hInstance`는 **Client.exe의 Client.cpp**에서 정의되고, `Client_Defines.h`에서 `extern`으로 선언된다. Level_Logo, Level_GamePlay, Loader 등에서 이 전역 변수를 직접 사용한다:
```cpp
// Level_GamePlay.cpp
SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));
ShadowDesc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;

// Loader.cpp
SetWindowText(g_hWnd, m_szLoading);
```
Game.dll로 Level/Loader를 옮기면, 이 전역 변수들의 **정의(definition)**가 Client.exe에 남아 있으므로 Game.dll에서 참조할 수 없다 (링크 에러).

**해결 방안**:
- **방안 A (권장)**: 전역 변수를 Game.dll로 이동하고 `GAME_DLL`로 export. Client.exe와 Tool.exe는 import하여 WinMain에서 값 대입
  ```cpp
  // Game_Defines.h
  GAME_DLL extern HWND g_hWnd;
  GAME_DLL extern HINSTANCE g_hInstance;

  // Game.cpp (Game.dll)
  HWND g_hWnd = nullptr;
  HINSTANCE g_hInstance = nullptr;
  ```
- **방안 B**: 전역 변수 대신 Engine의 `ENGINE_DESC`에 이미 `hWnd`, `hInstance`가 있으므로, GameInstance를 통해 접근하는 래퍼 함수를 추가하여 전역 변수 의존을 제거
- **방안 C**: Game.dll 초기화 함수에서 매개변수로 전달
  ```cpp
  GAME_DLL HRESULT Game_Initialize(HWND hWnd, HINSTANCE hInst, _uint iWinSizeX, _uint iWinSizeY);
  ```

**주의**: `g_iWinSizeX`, `g_iWinSizeY`는 `Client_Defines.h`에 `static const`로 정의되어 있어 헤더를 include하는 각 모듈에 복사됨 → 이것은 Game_Defines.h로 옮기면 해결. 하지만 `g_hWnd`, `g_hInstance`는 **런타임 대입** 값이므로 반드시 export 또는 매개변수 전달이 필요.

### 4-7. 스레드 로딩과 DLL 초기화

**문제**: Loader가 별도 스레드에서 `_beginthreadex` → `Loading()` 을 실행한다. Game.dll이 별도 DLL이 되면 스레드에서 DLL 함수를 호출하는 것에 주의가 필요하다.

**주의점**:
- `CoInitializeEx(nullptr, 0)` 호출이 Loader 스레드에서 이루어지고 있음 — Game.dll에서도 동일하게 유지 필요
- DLL의 `DllMain`에서 스레드 생성/대기 금지 (Loader Deadlock 위험) → Loader 스레드는 Client.exe 또는 Tool.exe에서 생성하므로 문제 없음
- **CriticalSection** 사용은 DLL 경계와 무관하게 정상 동작

---

## 5. Stage 1 — Static Library 리팩토링 순서

### Phase 1: Game.lib 프로젝트 생성
1. Visual Studio 솔루션에 Game 프로젝트 추가 (**Static Library, x64, Debug**)
2. `Game_Defines.h` 생성:
   ```cpp
   #pragma once
   #include "Engine_Defines.h"  // Engine public 헤더

   namespace Game
   {
       static const unsigned int g_iWinSizeX = 1280;
       static const unsigned int g_iWinSizeY = 720;
       enum class LEVEL { STATIC, LOADING, LOGO, GAMEPLAY, END };
   }
   extern HINSTANCE g_hInstance;  // EXE에서 정의, Game.lib 코드에서 참조
   extern HWND      g_hWnd;      // EXE에서 정의, Game.lib 코드에서 참조
   using namespace Game;
   ```
3. Engine.dll의 `.lib` + public 헤더 경로를 추가 인클루드/라이브러리 디렉터리로 설정
4. PCH 설정: `pch.h` / `pch.cpp` 생성 (또는 PCH 미사용)
5. 출력 경로: `$(SolutionDir)Bin/` (Client/Tool과 동일)
6. **빌드 의존성**: Engine.dll → Game.lib → Client.exe / Tool.exe

### Phase 2: 클래스 이동

**이동 대상** (총 16개 클래스):

| 구분 | 클래스 | 헤더 + 소스 |
|------|--------|-------------|
| GameObject | CPlayer, CMonster, CForkLift, CTerrain, CSky, CSnow, CSprite, CParticle_Explosion, CBackGround | 9쌍 |
| PartObject | CBody, CWeapon | 2쌍 |
| Camera | CCamera_Free | 1쌍 |
| Level | CLevel_Logo, CLevel_Loading, CLevel_GamePlay | 3쌍 |
| Utility | CLoader | 1쌍 |

**작업 순서**:
1. Game 프로젝트에 `Public/` (헤더), `Private/` (소스) 폴더 생성
2. Client에서 위 파일들을 Game 프로젝트로 **이동** (복사가 아닌 이동)
3. `Client_Defines.h` 의존부를 `Game_Defines.h`로 전환 — 각 파일의 `#include "Client_Defines.h"` → `#include "Game_Defines.h"`
4. namespace `Client` → `Game` 변경 (또는 공통 namespace 사용)
5. ⚠️ **export 매크로 불필요** — Static Library이므로 `GAME_DLL` 같은 매크로 추가 없음

### Phase 3: Client.exe 경량화

**Client에 남는 파일**:
- `Client.cpp` — WinMain, 윈도우 생성, `g_hWnd`/`g_hInstance` 정의
- `CMainApp` — 엔진 초기화 + `Start_Level()` 호출
- `framework.h` — PCH (기존 유지)

**변경 사항**:
1. Client 프로젝트에서 이동한 16개 클래스 파일을 **제거**
2. Client 프로젝트 속성에 Game.lib 링크 추가:
   - 추가 라이브러리 디렉터리: `$(SolutionDir)Bin/`
   - 추가 종속성: `Game.lib`
3. Client 프로젝트 추가 인클루드 디렉터리에 `Game/Public/` 추가
4. `Client_Defines.h` 제거 (또는 `#include "Game_Defines.h"` wrapper로 축소)
5. `CMainApp`의 `#include` 경로 Game 프로젝트 기준으로 수정
6. **빌드 확인**: Client.exe가 기존과 동일하게 동작하는지 검증

### Phase 4: Tool.exe 연동
1. Tool 프로젝트에 Engine.dll `.lib` + Game.lib 참조 추가
2. Tool 프로젝트 추가 인클루드 디렉터리에 `Engine/public/`, `Game/Public/` 추가
3. `Tool_Defines.h` 생성:
   ```cpp
   #include "Game_Defines.h"
   // Tool 전용 LEVEL 상수 (필요 시)
   constexpr _uint LEVEL_TOOL_EDIT = 2;
   ```
4. `CToolApp` 작성 — MainApp 패턴 복제, 엔진 초기화 후 Tool 전용 Level로 시작
5. Loader의 `Loading_For_GamePlay()` 함수를 Tool에서도 호출하여 프로토타입 등록
6. Tool 전용 Level에서 Game.lib의 CPlayer, CTerrain 등을 직접 사용

### Phase 5: 검증 체크리스트
- [ ] Client.exe 단독 빌드 성공
- [ ] Client.exe 실행: Logo → Loading → GamePlay 전환 정상
- [ ] Tool.exe 빌드 성공 (Engine.dll + Game.lib 링크)
- [ ] Tool.exe에서 `CPlayer::Create()` / `CTerrain::Create()` 호출 가능
- [ ] Tool.exe에서 `CGameInstance::GetInstance()` 정상 동작 (Engine.dll 싱글톤)
- [ ] Loader 스레드 로딩 정상 동작 (동일 프로세스 내이므로 문제 없어야 함)
- [ ] `dynamic_cast<CBody*>(...)` 정상 동작 (static lib이므로 RTTI 문제 없음)

---

## 6. 난이도 평가 (Stage 1: Static Library 기준)

| 항목 | 난이도 | 설명 |
|------|--------|------|
| 프로젝트 생성/설정 | **하** | VS에서 Static Library 프로젝트 추가, 인클루드/라이브러리 경로 설정 |
| 클래스 이동 | **하** | 16개 클래스 파일 이동, 기계적 작업 |
| namespace/Defines 전환 | **하** | `Client` → `Game`, `Client_Defines.h` → `Game_Defines.h` |
| 전역 변수 처리 | **하** | LIB에서는 extern 선언만, 정의는 각 EXE — 현재 패턴 유지 |
| LEVEL enum 분리 | **중** | Tool 전용 레벨 구조 고려한 설계 필요 |
| Loader Tool 호환 | **중** | Tool 전용 로딩 경로 분리 또는 선택적 등록 설계 |
| 빌드 체인 | **하** | Engine.dll → Game.lib → EXE, 단순한 의존 순서 |
| 전체 작업량 | **하~중** | DLL 대비 export/싱글톤/RTTI 이슈가 없어 대폭 경감 |

### Static Library 방식에서 제거되는 리스크
- ~~싱글톤 인스턴스 분리~~ → 같은 바이너리 내 링크이므로 **자동 공유**
- ~~DLL export 누락~~ → export 매크로 자체가 **불필요**
- ~~DLL 경계 RTTI/dynamic_cast~~ → 단일 바이너리이므로 **문제 없음**
- ~~DLL 초기화 순서~~ → DllMain 없으므로 **해당 없음**

### 남아 있는 주의점
1. **Engine.dll 싱글톤은 여전히 DLL 경계**: Game.lib 코드가 EXE에 링크되더라도 Engine.dll의 `GetInstance()`는 DLL import를 통해 호출됨 → 기존과 동일한 구조이므로 **문제 없음**
2. **순환 의존 불가능**: Game.lib → Engine.dll 단방향 의존만 존재 (Engine은 Game 타입을 모름)

---

## 7. Stage 2 — DLL 마이그레이션 경로

Static Library로 안정성을 확보한 후, 필요 시 DLL로 전환하는 절차.

### 전환 조건 (하나 이상 해당 시 고려)
- Tool에서 **게임 실행 중 코드 교체**(핫 리로드)가 필요할 때
- Client.exe + Tool.exe **바이너리 크기**가 문제될 때 (Game 코드 중복 링크 해소)
- **플러그인 구조**로 확장하여 여러 Game 모듈을 교체 로드해야 할 때

### 전환 절차

#### Step 1: DLL 프로젝트 전환
1. VS에서 Game 프로젝트 속성 → 구성 형식을 **Static Library → DLL** 변경
2. `GAME_EXPORTS` 전처리기 매크로 추가
3. `Game_Defines.h`에 export 매크로 추가:
   ```cpp
   #ifdef GAME_EXPORTS
   #define GAME_DLL __declspec(dllexport)
   #else
   #define GAME_DLL __declspec(dllimport)
   #endif
   ```

#### Step 2: 클래스에 export 매크로 적용
- **모든 구현층 클래스**에 `GAME_DLL` 붙이기 (누락 시 링크 에러)
  ```cpp
  class GAME_DLL CPlayer : public CContainerObject { ... };
  class GAME_DLL CBody : public CPartObject { ... };
  ```
- 4절의 DLL 관련 주의사항(싱글톤 검증, RTTI, 전역 변수 export) 적용

#### Step 3: 전역 변수 export 전환
- Static Library에서는 EXE에서 정의하던 `g_hWnd`, `g_hInstance`를 **Game.dll로 이동**
- `GAME_DLL extern HWND g_hWnd;` (헤더) + `HWND g_hWnd = nullptr;` (소스)
- 또는 초기화 함수 패턴 적용 (4-6절 방안 C)

#### Step 4: Client.exe / Tool.exe 링크 변경
- 추가 종속성: `Game.lib` (DLL이 자동 생성하는 import library)
- 출력 경로에 `Game.dll` 복사 (Post-Build Event)
  ```
  xcopy /Y "$(SolutionDir)Bin\Game.dll" "$(OutDir)"
  ```

#### Step 5: 검증 (4절 전체 항목)
- 싱글톤 공유: `CGameInstance::GetInstance()` 동일 인스턴스 확인
- RTTI: `dynamic_cast<CBody*>(...)` Tool.exe에서 정상 동작 확인
- 스레드 로딩: Loader 스레드에서 Game.dll 함수 호출 정상 확인

### DLL 전환 시 추가 난이도

| 항목 | 난이도 | 비고 |
|------|--------|------|
| export 매크로 추가 | **하** | 기계적, Engine.dll 패턴 복제 |
| 전역 변수 export | **중** | 소속 이전 + EXE 초기화 로직 변경 |
| 싱글톤 공유 검증 | **중** | DLL import 경로 확인 필수 |
| 빌드 체인 변경 | **하** | 프로젝트 속성 변경 + Post-Build 추가 |

### 롤백 경로
DLL 전환 후 문제 발생 시, 프로젝트 구성 형식을 다시 Static Library로 되돌리고 export 매크로를 빈 매크로(`#define GAME_DLL`)로 변경하면 **즉시 LIB로 롤백** 가능. 코드 구조 변경이 없으므로 안전.
