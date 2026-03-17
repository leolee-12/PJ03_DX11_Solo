# O15. Game_PKM 정적 라이브러리 — 구체적 실현 방안

> O13(GameDLL 리팩토링 타당성), O14(상속 구조 설계) 후속
> 세 가지 핵심 질문에 대한 코드 기반 검증 결과

### 현재 상태 (코드 검증 기준)

Game_PKM 프로젝트는 **이미 부분적으로 존재**:
- `Game_PKM_Defines.h` — `namespace Game_PKM`, LEVEL enum, `extern g_hWnd/g_hInstance` 정의 완료
- `CBackGround` — Game_PKM 네임스페이스로 구현 완료 (현재 Client의 CLoader에서 프로토타입 등록)
- CLevel_GamePlay, CLevel_EditPlay — **빈 스켈레톤** 상태 (Ready_Layer_*, Add_GameObject 미구현)
- CLoader::Ready_Resources_For_GamePlay() — **빈 구현** (프로토타입 등록 코드 미작성)

> 아래 분석의 코드 예시 중 CPlayer, CMonster 등 게임 오브젝트와 Ready_Layer_* 패턴은
> **참고프로젝트1(Framework) 기반 설계**이며, 현재 jusin_DX11_Framework에는 미구현 상태.
> 실제 구현 시 이 패턴을 따라 작성하면 된다.

---

## 질문 1: Prototype_Manager에서 Client/Game_PKM 프로토타입 공존 가능한가?

### 결론: **문제 없음**

### 1.1 현재 구조 확인 (코드 원문)

`CPrototype_Manager`는 레벨 인덱스별 `map<_wstring, CBase*>` 배열로 프로토타입을 관리한다:

```cpp
// Prototype_Manager.h (실제 코드)
typedef map<_wstring, class CBase*>	PROTOTYPES;
PROTOTYPES*	m_pPrototypes = { nullptr };
_uint		m_iNumLevels = {};
```

**등록 시 하는 일**: 레벨 인덱스 + 태그 문자열만으로 저장. 등록 주체가 누구인지(Client인지, Game_PKM인지)는 전혀 관여하지 않음.

```cpp
// Prototype_Manager.cpp — Add_Prototype (실제 코드)
HRESULT CPrototype_Manager::Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, CBase* pPrototype)
{
	if (nullptr == m_pPrototypes ||
		iLevelIndex >= m_iNumLevels ||
		nullptr != Find_Prototype(iLevelIndex, strPrototypeTag))
		return E_FAIL;

	m_pPrototypes[iLevelIndex].emplace(strPrototypeTag, pPrototype);
	return S_OK;
}
```

**Clone 시 하는 일**: `PROTOTYPE` enum으로 타입 분기 후 `dynamic_cast` → `Clone()` 호출.

```cpp
// Prototype_Manager.cpp — Clone_Prototype (실제 코드)
CBase* CPrototype_Manager::Clone_Prototype(PROTOTYPE eType, _uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg)
{
	CBase* pPrototype = Find_Prototype(iLevelIndex, strPrototypeTag);
	if (nullptr == pPrototype)
		return nullptr;

	CBase* pInstance = { nullptr };

	if (PROTOTYPE::GAMEOBJECT == eType)
		pInstance = dynamic_cast<CGameObject*>(pPrototype)->Clone(pArg);
	else
		pInstance = dynamic_cast<CComponent*>(pPrototype)->Clone(pArg);

	if (nullptr == pInstance)
		return nullptr;

	return pInstance;
}
```

**Object_Manager에서의 호출** (Clone 후 레이어에 추가):
```cpp
// Object_Manager.cpp — Add_GameObject (실제 코드)
CGameObject* pGameObject = dynamic_cast<CGameObject*>(
	m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iPrototypeLevelIndex, strPrototypeTag, pArg)
);
if (nullptr == pGameObject)
	return E_FAIL;

CLayer* pLayer = Find_Layer(iLayerLevelIndex, strLayerTag);
if (nullptr == pLayer)
{
	pLayer = CLayer::Create();
	pLayer->Add_GameObject(pGameObject);
	m_pLayers[iLayerLevelIndex].emplace(strLayerTag, pLayer);
}
else
	pLayer->Add_GameObject(pGameObject);
```

### 1.2 공존이 가능한 이유

| 체크 항목 | 결과 |
|----------|------|
| Prototype_Manager가 등록 소스를 구분하는가? | **아니오** — `(iLevelIndex, strTag)` 쌍만 관리 |
| CGameInstance가 싱글톤인가? | **예** — `DECLARE_SINGLETON(CGameInstance)`, Engine.dll에 static 멤버 |
| Static Library가 같은 싱글톤 인스턴스를 공유하는가? | **예** — .lib는 EXE에 링크되므로 동일 프로세스 내 동일 Engine.dll import |
| 태그명 충돌 가능성? | **사용자 주의 필요** — 동일 레벨/동일 태그로 중복 등록 시 `E_FAIL` 반환 |

### 1.3 구체적 실행 흐름 (설계 기준)

```
[CMainApp::Initialize()]
  → Ready_Prototype_For_Static()
    → Add_Prototype(STATIC, "Prototype_Component_VIBuffer_Rect", CVIBuffer_Rect::Create(...))
    → Add_Prototype(STATIC, "Prototype_Component_Shader_VtxTex", CShader::Create(...))
    ← 엔진 컴포넌트 프로토타입이 STATIC 슬롯에 등록됨
    ※ 현재 실제 코드에서 등록하는 프로토타입은 이 2개뿐

[CLoader::Loading() — 워커 스레드]
  → Ready_Resources_For_GamePlay()
    → Add_Prototype(GAMEPLAY, "Proto_GO_Player",  CPlayer::Create(...))   ← Game_PKM 클래스 (향후 구현)
    → Add_Prototype(GAMEPLAY, "Proto_GO_Monster", CMonster::Create(...))  ← Game_PKM 클래스 (향후 구현)
    ※ 현재는 빈 구현 — 여기에 Game_PKM 오브젝트 프로토타입을 등록할 예정

[CLevel_GamePlay::Initialize()]  (향후 구현)
  → Ready_Layer_Player()
    → Add_GameObject(GAMEPLAY, "Proto_GO_Player", GAMEPLAY, "Layer_Player")
      → Clone_Prototype(GAMEOBJECT, GAMEPLAY, "Proto_GO_Player")
        → CPlayer::Clone() → new CPlayer(*this)
          → CPlayer::Initialize()
            → Ready_Components()
              → Clone_Prototype(COMPONENT, STATIC, "Prototype_Component_Shader_VtxTex")
              ← CMainApp이 등록한 엔진 컴포넌트 프로토타입을 Game_PKM 객체가 Clone
```

**핵심**: Game_PKM의 게임 오브젝트가 CMainApp에서 등록한 `LEVEL::STATIC` 엔진 컴포넌트 프로토타입을 `Clone_Prototype()`으로 복사해서 사용 — **정상 동작**.

### 1.4 주의사항

1. **태그명 충돌 방지**: Client와 Game_PKM에서 동일 태그를 동일 레벨에 등록하지 않도록 네이밍 컨벤션 유지
2. **등록 순서**: `LEVEL::STATIC` 프로토타입(엔진 컴포넌트)은 CMainApp에서 먼저 등록 → 이후 Loader가 `LEVEL::GAMEPLAY` 등에 게임 오브젝트 등록 → 게임 오브젝트의 `Ready_Components()`에서 STATIC 컴포넌트 Clone. **현재 순서 그대로 문제 없음**
3. **해제 타이밍**: `Prototype_Manager::Clear(_uint iLevelIndex)`로 레벨별 일괄 해제 — Client/Game_PKM 구분 없이 해당 레벨 인덱스의 프로토타입 전체 해제. **기존 동작 그대로 유지됨**

---

## 질문 2: CLevel 하위 클래스를 Game_PKM으로 옮겨도 문제 없는가?

### 결론: **CLevel_GamePlay만 옮기고, Logo/Loading은 각 EXE에 남기는 것을 권장**

### 2.1 현재 CLevel 하위 클래스 구조

```
Client.exe:
  CLevel_Logo        — 로고 화면, ENTER → Loading 전환
  CLevel_Loading     — CLoader 소유, 로딩 완료 → GamePlay 전환
  CLevel_GamePlay    — 게임 로직 (오브젝트 배치, 업데이트)

Editor.exe:
  CLevel_EditLogo    — 에디터 로고
  CLevel_EditLoading — CEditLoader 소유
  CLevel_EditPlay    — 에디터 편집 모드 (ImGui 연동)
```

### 2.2 각 레벨의 의존성 분석

| 레벨 | 의존 대상 | Game_PKM 이동 적합성 |
|------|----------|---------------------|
| **CLevel_GamePlay** | 게임 오브젝트(Player, Monster 등), CGameInstance | **적합** — 게임 오브젝트와 함께 이동해야 Client/Editor 양쪽에서 공유 가능 |
| **CLevel_Logo** | `g_hWnd`, LEVEL enum, 다음 레벨 생성(`CLevel_Loading::Create`) | **부적합** — UI 흐름이 Client 전용이고, Editor는 별도 EditLogo 사용 |
| **CLevel_Loading** | CLoader(스레드), `g_hWnd`, LEVEL enum, 다음 레벨 생성 | **부적합** — 스레드 관리 포함, Client/Editor별로 다른 레벨 전환 흐름 |

### 2.3 권장 구조

```
Game_PKM.lib:
  CLevel_GamePlay    ← Client/Editor 양쪽에서 사용하는 게임 레벨
  CPlayer, CMonster, CTerrain, ...  ← 게임 오브젝트

Client.exe:
  CLevel_Logo        ← Client 전용 흐름
  CLevel_Loading     ← Client 전용 흐름 + CLoader 소유
  CMainApp

Editor.exe:
  CLevel_EditLogo    ← Editor 전용 흐름
  CLevel_EditLoading ← Editor 전용 흐름 + CEditLoader 소유
  CLevel_EditPlay    ← Editor 전용 (Game_PKM의 오브젝트 사용)
  CEditorApp
```

### 2.4 CLevel_GamePlay 이동 시 필요한 작업

**현재 CLevel_GamePlay의 실제 의존성** (코드 검증 결과):

```cpp
// Level_GamePlay.cpp 실제 #include
#include "Level_GamePlay.h"    // 자체 헤더 (Client_Defines.h 포함)
// ※ 현재 Level_Loading.h는 include하지 않음 (빈 스켈레톤이므로)
// ※ g_hWnd 사용: Render()의 _DEBUG 블록에서 SetWindowText(g_hWnd, ...) 호출만

static constexpr _uint CURRENT_LEVEL = ETOUI(LEVEL::GAMEPLAY);
```

현재는 빈 스켈레톤이므로 이동이 매우 간단하나, 향후 게임 로직 추가 시 다음 원칙을 유지해야 함:

**필요 변경**:

1. **네임스페이스 전환**: `namespace Client` → `namespace Game_PKM` (이미 `Game_PKM_Defines.h`에 정의됨)
2. **`#include "Client_Defines.h"` → `#include "Game_PKM_Defines.h"`**: LEVEL enum은 이미 `Game_PKM_Defines.h`에 동일하게 정의되어 있음
3. **`g_hWnd` 처리**: `Game_PKM_Defines.h`에 이미 `extern HWND g_hWnd;` 선언되어 있으므로 **변경 불필요**
4. **레벨 전환 의존 제거 원칙**: Game_PKM의 CLevel_GamePlay는 다른 레벨 클래스를 직접 참조하지 않도록 설계
   - 현재 CLevel_GamePlay에는 레벨 전환 코드가 없으므로 문제 없음
   - 향후 게임 종료/재시작 시에도 플래그/콜백으로 Client/Editor에 위임

**참고 — CLevel_Loading의 하드 의존성** (이동하지 않는 이유를 보여주는 근거):
```cpp
// Level_Loading.cpp 실제 #include (Client에 남겨야 함)
#include "Level_Loading.h"
#include "GameInstance.h"
#include "Loader.h"
#include "Level_Logo.h"       // ← 직접 참조: CLevel_Logo::Create() 호출
#include "Level_GamePlay.h"   // ← 직접 참조: CLevel_GamePlay::Create() 호출
```
CLevel_Loading은 switch문에서 다음 레벨을 **직접 Create**하므로 Client 전용으로 남겨야 함. 단, CLevel_GamePlay가 Game_PKM으로 이동하면 `#include` 경로만 Game_PKM의 Public 디렉터리로 변경하면 됨.

### 2.5 Editor에서 CLevel_GamePlay 재사용 방식

O14에서 권장한 **방안 C (공용 로더) + 필요 시 방안 B (컴포지션)**:

```cpp
// Editor: CLevel_EditPlay는 CLevel을 직접 상속
class CLevel_EditPlay : public CLevel {
    // 에디터 모드: Game_PKM의 프로토타입으로 오브젝트 배치/편집
    // PIE 모드: CLevel_GamePlay를 컴포지션으로 소유하여 게임 로직 실행 가능
    CLevel_GamePlay* m_pGameLevel = nullptr;  // PIE용 (선택적)
};
```

---

## 질문 3: CLevel_Loading + CLoader를 Game_PKM으로 옮길 수 있는가?

### 결론: **CLoader를 분리하여 "리소스 등록 부분"만 Game_PKM으로, 스레드 관리는 각 EXE에 남기는 것을 권장**

### 3.1 현재 CLoader 구조 (실제 코드)

```cpp
// Loader.h (Client) — 실제 코드
class CLoader final : public CBase {
	ID3D11Device*        m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	CGameInstance*       m_pGameInstance = { nullptr };
	LEVEL                m_eNextLevelID = { LEVEL::END };

	HANDLE               m_hThread = {};
	CRITICAL_SECTION     m_CriticalSection = {};

	_tchar               m_szLoadingText[MAX_PATH] = {};
	std::atomic<_bool>   m_isFinished = {};

	HRESULT Initialize(LEVEL eNextLevelID);   // _beginthreadex로 스레드 생성
	HRESULT Loading();                         // 스레드에서 호출
	HRESULT Ready_Resources_For_Logo();        // 프로토타입 등록 + m_isFinished = true
	HRESULT Ready_Resources_For_GamePlay();    // 프로토타입 등록 + m_isFinished = true
#ifdef _DEBUG
	void Show();                               // SetWindowText(g_hWnd, m_szLoadingText)
#endif
};
```

**주의**: `m_isFinished = true`는 `Loading()`이 아니라 **각 `Ready_Resources_For_*()` 내부 끝에서** 설정됨.
`SetWindowText`는 `Show()` 메서드에서만 호출되며, `CLevel_Loading::Render()`에서 `_DEBUG` 빌드 시 호출.

**핵심**: CLoader는 **두 가지 역할**을 하나의 클래스에 담고 있음:
1. **스레드 관리** (생성, 동기화, 완료 통지) — EXE 종속
2. **리소스/프로토타입 등록** (`Ready_Resources_For_*`) — 게임 콘텐츠 종속

### 3.2 왜 CLoader를 통째로 옮기면 문제인가?

| 문제 | 설명 |
|------|------|
| **레벨 전환 하드코딩** | `Loading()` 내부에서 `switch(m_eNextLevelID)`로 Client 전용 LEVEL enum 사용 |
| **`SetWindowText(g_hWnd, m_szLoadingText)`** | `_DEBUG`의 `Show()` 메서드에서 호출, Client 전역 변수에 의존 |
| **Editor와의 차이** | Editor의 `CEditLoader`는 `Ready_Resources_For_EditLogo/EditPlay` 등 별도 함수 — 동일 구조지만 다른 리소스 세트 |
| **스레드 생성 컨텍스트** | `_beginthreadex`는 어떤 모듈에서 호출하든 동작하지만, Static Library에서는 EXE의 CRT에 바인딩됨 — 문제는 없으나 의미상 EXE 책임 |

### 3.3 권장 분리 설계: CGameLoader (Game_PKM) + CLoader (각 EXE)

```
[Game_PKM.lib]
CGameLoader (신규)
  ├─ Ready_Resources_For_Logo()       ← 프로토타입 등록만 담당
  ├─ Ready_Resources_For_GamePlay()   ← 프로토타입 등록만 담당
  └─ Ready_Resources_Common()         ← 공용 리소스 (선택적)

[Client.exe]
CLoader (기존 구조 유지, 리소스 등록은 CGameLoader에 위임)
  ├─ m_hThread, m_CriticalSection, m_isFinished  ← 스레드 관리
  ├─ Loading() → CGameLoader::Ready_Resources_For_*() 호출
  └─ SetWindowText(g_hWnd, ...) ← UI 표시

[Editor.exe]
CEditLoader (기존 구조 유지, 리소스 등록은 CGameLoader에 위임)
  ├─ m_hThread, m_CriticalSection, m_isFinished  ← 스레드 관리
  ├─ Loading() → CGameLoader::Ready_Resources_For_*() 호출
  └─ SetWindowText(g_hWnd, ...) ← UI 표시
```

### 3.4 구체적 구현 예시

#### CGameLoader (Game_PKM 소속)

```cpp
// Game_PKM/Public/GameLoader.h
class CGameLoader final : public CBase {
public:
    // 프로토타입 등록 전용 — 스레드 무관
    static HRESULT Ready_Resources_For_Logo(
        ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
        CGameInstance* pGI, _uint iLevelIndex);

    static HRESULT Ready_Resources_For_GamePlay(
        ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
        CGameInstance* pGI, _uint iLevelIndex);
};
```

```cpp
// Game_PKM/Private/GameLoader.cpp
HRESULT CGameLoader::Ready_Resources_For_GamePlay(
    ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
    CGameInstance* pGI, _uint iLevelIndex)
{
    // 텍스처
    if (FAILED(pGI->Add_Prototype(iLevelIndex, TEXT("Proto_Comp_Texture_Terrain"),
        CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile%d.dds"), 2))))
        return E_FAIL;

    // 모델
    if (FAILED(pGI->Add_Prototype(iLevelIndex, TEXT("Proto_Comp_Model_Fiona"),
        CModel::Create(pDevice, pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Fiona/Fiona.fbx"))))
        return E_FAIL;

    // 게임 오브젝트
    if (FAILED(pGI->Add_Prototype(iLevelIndex, TEXT("Proto_GO_Player"),
        CPlayer::Create(pDevice, pContext))))
        return E_FAIL;

    if (FAILED(pGI->Add_Prototype(iLevelIndex, TEXT("Proto_GO_Monster"),
        CMonster::Create(pDevice, pContext))))
        return E_FAIL;

    // ... 나머지 프로토타입
    return S_OK;
}
```

#### Client의 CLoader (변경 후)

```cpp
// Client/Private/Loader.cpp — 변경 후 설계
HRESULT CLoader::Loading()
{
    EnterCriticalSection(&m_CriticalSection);

    HRESULT hr = { S_OK };

    switch (m_eNextLevelID) {
    case LEVEL::LOGO:
        lstrcpy(m_szLoadingText, TEXT("게임오브젝트 로드 중"));
        hr = CGameLoader::Ready_Resources_For_Logo(
            m_pDevice, m_pContext, m_pGameInstance, ETOUI(LEVEL::LOGO));
        break;
    case LEVEL::GAMEPLAY:
        lstrcpy(m_szLoadingText, TEXT("게임플레이 리소스 로드 중"));
        hr = CGameLoader::Ready_Resources_For_GamePlay(
            m_pDevice, m_pContext, m_pGameInstance, ETOUI(LEVEL::GAMEPLAY));
        break;
    }

    LeaveCriticalSection(&m_CriticalSection);

    if (FAILED(hr))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("로드가 완료되었습니다."));
    m_isFinished = true;
    return S_OK;
}
```

**기존 코드와의 차이**: 기존에는 `m_isFinished = true`가 `Ready_Resources_For_*()` 내부에 있었지만,
분리 후에는 `Loading()` 끝에서 설정. 로딩 진행 메시지(`m_szLoadingText`)도 Loader가 관리.

### 3.5 스레드 안전성

| 항목 | 상태 |
|------|------|
| `CGameLoader::Ready_Resources_For_*()` 내에서 CGameInstance 호출 | **안전** — 현재도 CLoader의 워커 스레드에서 동일하게 호출 중 |
| `CoInitializeEx` | **현재 CLoader에서는 호출하지 않음** — 향후 COM 기반 리소스 로딩(WIC 등) 추가 시 Loader 스레드에서 호출 필요 |
| `Add_Prototype()` 호출 | **안전** — 현재 구조에서 메인 스레드는 로딩 중 Prototype_Manager에 접근하지 않음 (CLevel_Loading::Update()에서 isFinished만 확인) |
| `CRITICAL_SECTION` | **각 EXE의 Loader가 관리** — CGameLoader는 스레드 동기화를 모름 (호출자 책임) |

---

## 종합 정리

### 각 질문 답변 요약

| 질문 | 답변 | 핵심 근거 |
|------|------|----------|
| Client 프로토타입 + Game_PKM 프로토타입 공존? | **문제 없음** | Prototype_Manager는 `(iLevelIndex, strTag)` 쌍만으로 관리, 등록 주체 무관 |
| Game_PKM 객체가 Client 프로토타입을 Clone? | **문제 없음** | `Clone_Prototype()`은 `CBase*` → `dynamic_cast` → `Clone()` — 타입만 맞으면 동작 |
| CLevel 하위 클래스를 Game_PKM으로 이동? | **CLevel_GamePlay만 이동 권장** | Logo/Loading은 EXE별 UI 흐름에 종속 |
| CLevel_Loading + CLoader를 Game_PKM으로 이동? | **리소스 등록만 분리 (CGameLoader)** | 스레드 관리는 EXE 책임, 프로토타입 등록만 공유 |

### 최종 파일 배치

```
Game_PKM.lib (기존 프로젝트 확장):
  Public/
    Game_PKM_Defines.h    ← 이미 존재 (LEVEL enum, extern g_hWnd/g_hInstance)
    GameLoader.h          ← 신규: 프로토타입 등록 전용 (스레드 무관)
    BackGround.h          ← 이미 존재
    Player.h, Monster.h, ...  ← 향후 추가할 게임 오브젝트
    Body.h, Weapon.h      ← 향후 추가할 PartObject
    Level_GamePlay.h      ← Client에서 이동
  Private/
    GameLoader.cpp
    BackGround.cpp        ← 이미 존재
    Player.cpp, Monster.cpp, ...
    Level_GamePlay.cpp

Client.exe (경량화):
  Public/
    Client_Defines.h      ← Game_PKM_Defines.h include + Client 전용 설정
    MainApp.h             ← 엔진 초기화, Ready_Prototype_For_Static()
    Loader.h              ← 스레드 관리, CGameLoader 호출
    Level_Logo.h          ← Client 전용
    Level_Loading.h       ← Client 전용, CLoader 소유
  Private/
    Client.cpp            ← WinMain, g_hWnd/g_hInstance 정의

Editor.exe (기존 유지):
  Public/
    Editor_Defines.h      ← Game_PKM_Defines.h include + Editor 전용 설정
    EditorApp.h
    EditLoader.h          ← 스레드 관리, CGameLoader 호출
    Level_EditLogo.h
    Level_EditLoading.h
    Level_EditPlay.h      ← Game_PKM 오브젝트 사용 + ImGui
  Private/
    Editor.cpp            ← WinMain, g_hWnd/g_hInstance 정의
```

### 네임스페이스 전환 주의

현재 각 프로젝트의 네임스페이스가 분리되어 있음:
- `namespace Client` — Client_Defines.h에서 `using namespace Client;`
- `namespace Editor` — Editor_Defines.h에서 `using namespace Editor;`
- `namespace Game_PKM` — Game_PKM_Defines.h에서 `using namespace Game_PKM;`

CLevel_GamePlay를 Game_PKM으로 이동 시 `NS_BEGIN(Client)` → `NS_BEGIN(Game_PKM)` 변경 필요.
Client의 CLevel_Loading에서 `CLevel_GamePlay::Create()`를 호출할 때 `Game_PKM::CLevel_GamePlay` 또는 `using namespace Game_PKM;` 필요.

### Static Library 방식의 이점 재확인

O13에서 분석한 대로 Static Library이므로:
- ~~DLL export 매크로~~ → **불필요**
- ~~싱글톤 인스턴스 분리~~ → **자동 공유** (같은 바이너리에 링크)
- ~~DLL 경계 RTTI/dynamic_cast~~ → **문제 없음** (단일 바이너리)
- ~~DLL 초기화 순서~~ → **해당 없음** (DllMain 없음)

### CLoader의 Free() 패턴 보존 주의

CLoader/CEditLoader 분리 후에도 스레드 정리 패턴을 그대로 유지해야 함:
```cpp
// Loader.cpp — Free() (실제 코드)
void CLoader::Free()
{
    __super::Free();
    WaitForSingleObject(m_hThread, INFINITE);   // 스레드 완료 대기
    DeleteCriticalSection(&m_CriticalSection);  // 임계 섹션 삭제
    CloseHandle(m_hThread);                     // 핸들 닫기
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
```
CGameLoader는 스레드/동기화를 모르므로, 이 정리 로직은 각 EXE의 Loader에 그대로 남김.
