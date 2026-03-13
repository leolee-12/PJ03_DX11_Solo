# 참고프로젝트3 — 입력 시스템 심화 분석

> **분석 대상**: CInputDev (Engine/System), CMouse_Manager (Engine/Utility)
> **모드**: [분석]

---

## 1. 핵심 책임과 시스템 경계

### CInputDev — 하드웨어 입력 추상화
- DirectInput8을 통한 **키보드/마우스 원시 입력** 획득
- 원시 상태를 가공하여 **OnceKeyDown / StayKeyDown / OnceMouseClick / StayMouseClick** 제공
- 컷씬 등에서 **키 비활성화**(m_bKeyDisabled) 기능

### CMouse_Manager — 마우스 좌표 추상화
- Win32 API(`GetCursorPos` + `ScreenToClient`)로 **클라이언트 좌표계 마우스 위치** 제공
- 화면 중앙 기준 **상대 이동량** 계산
- 마우스 포인터 GameObject 참조 관리

### 시스템 경계 — 입력이 하지 않는 것
- **마우스 피킹**: CCollisionManager가 담당 (Ray 생성 → 메시/NavMesh 충돌 판정)
- **카메라 조작**: CCamera 계열이 InputDev를 직접 호출하여 처리
- **플레이어 이동**: CPlayer가 InputDev를 직접 호출하여 처리
- **입력 바인딩/리매핑**: 없음 — 소비자가 DIK 상수를 직접 사용

---

## 2. 클래스 간 소유/참조 관계

```
CInputDev (싱글톤, Engine/System 레이어)
├── 소유: m_pInput        → LPDIRECTINPUT8 (DirectInput 메인 객체)
├── 소유: m_pKeyBoard     → LPDIRECTINPUTDEVICE8 (키보드 디바이스)
├── 소유: m_pMouse        → LPDIRECTINPUTDEVICE8 (마우스 디바이스)
├── 값:   m_byKeyState[256]  → 키보드 원시 상태 배열
├── 값:   m_MouseState       → DIMOUSESTATE (버튼 3개 + 이동량 XYZ)
├── 값:   m_bKeyDown[256]    → OnceKeyDown용 이전 프레임 상태
├── 값:   m_bMouseDown[3]    → OnceMouseClick용 이전 프레임 상태
└── 값:   m_bKeyDisabled     → 키 비활성화 플래그

CMouse_Manager (싱글톤, Engine/Utility 레이어)
├── 값:   m_hWnd             → 윈도우 핸들 (Initialize에서 세팅)
├── 값:   m_vecInitialPos    → 화면 중앙 좌표 (상대 이동 계산 기준)
└── 참조: m_pMousePointer    → CGameObject* (마우스 커서 오브젝트, AddRef)
```

### 소유 관계 핵심
- **CInputDev**는 DI 객체를 직접 소유하고 `Free()`에서 `Safe_Release`
- **CMouse_Manager**는 CInputDev를 참조하지 않음 — 두 시스템이 **완전 독립**
- CInputDev는 하드웨어 입력 상태만 제공, CMouse_Manager는 Win32 좌표만 제공
- **소비자(Player, Camera 등)**가 두 시스템을 각각 직접 호출

---

## 3. 한 프레임 입력 처리 흐름

```
CMainApp::Update_MainApp(fTimeDelta)
│
├─ 1. CInputDev::Set_InputState()           ◀ 프레임 시작 시 1회 호출
│   ├─ m_pKeyBoard->GetDeviceState(256, m_byKeyState)
│   │   └─ 실패 시(DIERR_INPUTLOST/NOTACQUIRED) → m_pKeyBoard->Acquire()
│   └─ m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_MouseState)
│       └─ 실패 시 → m_pMouse->Acquire()
│
├─ 2. CManagement::Update_Management()
│   └─ Scene::Update_Scene()
│       └─ 각 GameObject::Update_GameObject()
│           │
│           ├─ [CPlayer] 키보드 입력 소비
│           │   ├─ CInputDev::GetInstance()->StayKeyDown(DIK_W)  → 이동
│           │   ├─ CInputDev::GetInstance()->OnceKeyDown(DIK_1)  → 스킬
│           │   └─ CInputDev::GetInstance()->OnceMouseClick(DIM_LBUTTON) → 공격
│           │
│           ├─ [CCamera] 마우스 이동 소비
│           │   ├─ CInputDev::GetInstance()->GetDIMouseMove(DIM_X/DIM_Y) → 회전
│           │   └─ CMouse_Manager::GetInstance()->GetMousePos()  → 좌표
│           │
│           └─ [UI 등] 마우스 클릭 소비
│               └─ CInputDev::GetInstance()->OnceMouseClick(DIM_LBUTTON)
│
└─ 3. CRenderer::Render_GameObject()
    └─ CInputDev::GetInstance()->OnceKeyDown(DIK_R)  → 디버그 RT 토글
```

### 호출 타이밍 정리
| 시점 | 호출 | 역할 |
|------|------|------|
| 프레임 초 | `Set_InputState()` | 하드웨어 폴링 (1회) |
| Update 중 | `GetDIKeyState` / `OnceKeyDown` / `StayKeyDown` 등 | 소비자가 필요 시 읽기 |
| Render 중 | `OnceKeyDown(DIK_R)` | 디버그 기능 (예외적 사용) |

---

## 4. 주요 함수 상세 분석

### 4-1. OnceKeyDown — 한 번만 반응
```cpp
bool OnceKeyDown(BYTE KeyFlag, bool KeyEnable = false)
{
    if (m_bKeyDisabled && !KeyEnable)  // 키 비활성 상태면 무시
        return false;

    if (m_byKeyState[KeyFlag])         // 현재 눌려 있으면
    {
        if (m_bKeyDown[KeyFlag] == false)  // 이전 프레임에 안 눌려 있었으면
        {
            m_bKeyDown[KeyFlag] = true;    // 눌림 기록
            return true;                    // → "방금 눌렸다"
        }
    }
    else
        m_bKeyDown[KeyFlag] = false;   // 떼면 리셋

    return false;
}
```
- **엣지 감지 패턴**: 현재 상태와 이전 상태 비교로 "누른 순간"만 감지
- `KeyEnable=true`이면 비활성 상태에서도 동작 (ESC 등 시스템 키용)

### 4-2. GetDIMouseMove — 마우스 이동량
```cpp
long GetDIMouseMove(MOUSEMOVE KeyFlag)
{
    return *(((long*)&m_MouseState) + KeyFlag);
}
```
- `DIMOUSESTATE`의 메모리 레이아웃을 이용한 포인터 산술
- `DIMOUSESTATE = { lX, lY, lZ, rgbButtons[4] }` → KeyFlag(0/1/2)로 X/Y/Z 접근
- **위험하지만 효율적**: 구조체 레이아웃에 의존

### 4-3. CMouse_Manager::GetMouseRelativeGap
```cpp
_vec3 GetMouseRelativeGap()
{
    return GetMousePos() - m_vecInitialPos;  // 현재 위치 - 화면 중앙
}
```
- 화면 중앙 기준 상대 좌표 → 카메라 회전 등에 활용

---

## 5. 사용된 디자인 패턴

### 5-1. 싱글톤 (CInputDev, CMouse_Manager)
- `DECLARE_SINGLETON` / `IMPLEMENT_SINGLETON` 매크로
- 전역 접근: `CInputDev::GetInstance()->OnceKeyDown(DIK_W)`

### 5-2. 폴링 패턴 (Pull 방식)
- **이벤트 기반이 아님** — 프레임마다 `Set_InputState()`로 전체 상태 스냅샷
- 소비자가 `GetDIKeyState()` / `OnceKeyDown()`으로 필요한 키만 조회
- **장점**: 구현이 단순, 동기적, 프레임 내 일관된 상태 보장
- **단점**: 프레임 간 입력 유실 가능 (짧은 키 입력이 한 프레임 내에 시작·종료되면 놓침)

### 5-3. 엣지 감지 패턴 (OnceKeyDown / OnceMouseClick)
- 이전 프레임 상태(`m_bKeyDown[]`)를 별도 배열로 유지
- `!이전 && 현재` → true (Rising Edge)
- Falling Edge(놓는 순간)는 구현하지 않음

### 5-4. 디바이스 손실 복구 (Acquire 패턴)
```cpp
hr = m_pKeyBoard->GetDeviceState(256, m_byKeyState);
if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
    m_pKeyBoard->Acquire();
```
- 포커스 상실 시 `DIERR_INPUTLOST` → 즉시 `Acquire()` 재시도
- 별도 에러 처리 없이 다음 프레임에 자연스럽게 복구

---

## 6. DirectX/Win32 API 호출 지점과 래핑

### CInputDev — DirectInput8 래핑

| DirectInput API | CInputDev 메서드 | 호출 시점 |
|-----------------|-----------------|----------|
| `DirectInput8Create()` | `Ready_InputDevice()` | 앱 초기화 |
| `CreateDevice(GUID_SysKeyboard)` | `Ready_KeyBoard()` | 앱 초기화 |
| `CreateDevice(GUID_SysMouse)` | `Ready_Mouse()` | 앱 초기화 |
| `SetCooperativeLevel(DISCL_FOREGROUND\|DISCL_NONEXCLUSIVE\|DISCL_NOWINKEY)` | `Ready_KeyBoard/Mouse()` | 앱 초기화 |
| `SetDataFormat(&c_dfDIKeyboard/Mouse)` | `Ready_KeyBoard/Mouse()` | 앱 초기화 |
| `Acquire()` | `Ready_*()` + `Set_InputState()` | 초기화 + 매 프레임 복구 |
| `GetDeviceState()` | `Set_InputState()` | 매 프레임 |
| `Safe_Release(device)` | `Free()` | 종료 |

**래핑 수준**: DirectInput의 초기화/폴링/해제를 캡슐화. 소비자는 DI API를 전혀 모름.
단, `DIK_*` 상수(DIK_W, DIK_SPACE 등)는 소비자에 노출.

### CMouse_Manager — Win32 API 래핑

| Win32 API | CMouse_Manager 메서드 |
|-----------|---------------------|
| `GetCursorPos()` | `GetMousePos()` / `Get_MousePoint()` |
| `ScreenToClient()` | `GetMousePos()` / `Get_MousePoint()` |

**래핑 수준**: 스크린→클라이언트 좌표 변환을 캡슐화.

---

## 7. 설계 판단 분석 — 참고할 점과 한계

### 참고할 점

**1) Set_InputState의 프레임 단위 스냅샷**
- 프레임 초에 1회 폴링 → 프레임 내 모든 쿼리가 동일 상태 참조
- 경쟁 조건(race condition) 없음, 입력 상태의 일관성 보장
- 구현: 단 6줄 (`GetDeviceState` × 2 + 에러 복구 × 2)

**2) OnceKeyDown의 엣지 감지 — 간결하고 효과적**
- 이전 상태 배열 하나로 "누른 순간" 감지
- `KeyEnable` 매개변수로 비활성 모드에서도 특정 키 허용 (ESC 등)
- 게임에서 가장 자주 필요한 패턴을 최소 코드로 해결

**3) 키 비활성화 기능 (m_bKeyDisabled)**
- 컷씬, 씬 전환 등에서 `Set_KeyDisEnabled()` → 전체 키 차단
- `KeyEnable=true`인 호출만 통과 → 시스템 키(ESC, R 등) 예외 처리
- 단순 bool 플래그지만 게임플레이에 필수적인 기능

**4) 디바이스 손실 자동 복구**
- Alt+Tab 등으로 포커스 상실 → 다음 `Set_InputState()` 호출에서 자동 `Acquire()`
- 별도 에러 코드 전파 없이 조용히 복구 → 소비자 코드에 영향 없음

**5) CInputDev와 CMouse_Manager의 역할 분리**
- CInputDev: **DirectInput 전용** (상대적 입력 — 키 상태, 마우스 이동량)
- CMouse_Manager: **Win32 API 전용** (절대적 입력 — 마우스 스크린 좌표)
- 각각 독립된 API 래핑 → 서로 의존성 없음

### 한계/개선 가능 포인트

**1) 소비자가 DIK 상수에 직접 의존**
- `OnceKeyDown(DIK_W)` 형태로 호출 → 키 리바인딩 불가
- 개선: 액션 매핑 레이어 추가 (`ACTION_MOVE_FORWARD` → `DIK_W` 매핑)

**2) Falling Edge 미구현**
- "키를 놓는 순간" 감지가 없음 → 차지 공격 등에서 불편
- 개선: `OnceKeyUp()` 추가 (`이전 && !현재` → true)

**3) GetDIMouseMove의 포인터 산술**
```cpp
return *(((long*)&m_MouseState) + KeyFlag);
```
- DIMOUSESTATE 메모리 레이아웃에 의존하는 위험한 캐스팅
- 구조체 패딩이 바뀌면 버그 → `switch`문이나 배열 인덱싱이 더 안전

**4) m_pInput(DirectInput 메인 객체) 미해제**
- `Free()`에서 `m_pKeyBoard`와 `m_pMouse`만 Release
- `m_pInput` (LPDIRECTINPUT8)의 Release가 누락 → **메모리 누수**

**5) CMouse_Manager의 static POINT**
```cpp
_vec3 GetMousePos(void) {
    static POINT ptMouse;    // ← 정적 변수
    GetCursorPos(&ptMouse);
    ...
}
```
- 함수 내 static 변수 → 멀티스레드 안전하지 않음
- 이 프로젝트에서는 단일 스레드이므로 문제없지만, 습관적으로 피해야 할 패턴
