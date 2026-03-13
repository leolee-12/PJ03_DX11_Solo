# 참고프로젝트2 — 입력 시스템 심화 분석

> 분석 범위: `CInput_Device`, `CKey_Manager`, `CInput`, `CCharacter`(입력 관련부)
> 분석 대상: 헤더 + cpp 구현부

---

## 1. 시스템 핵심 책임과 경계

### 책임 분리 (3계층)

| 계층 | 클래스 | 위치 | 책임 |
|------|--------|------|------|
| **하드웨어 폴링** | `CInput_Device` | Engine | DirectInput8 키보드/마우스 raw 상태 획득 + Down/Up/Pressing 엣지 판정 |
| **파사드 중계** | `CGameInstance` | Engine | 입력 API를 클라이언트에 노출 (위임 패턴) |
| **게임 입력 해석** | `CInput` + `CCharacter` | Client | 방향+버튼 추상화, 입력 버퍼, 커맨드 패턴 매칭 |

### 경계

```
┌──────────────────────────────────────────────────┐
│  Client                                           │
│  CCharacter::InputCommand()                       │
│    ↓ 키 조회                                      │
│  CGameInstance::Key_Down/Pressing(DIK_*)          │
│    ↓ 위임                                         │
├──────────────────────────────────────────────────┤
│  Engine                                           │
│  CInput_Device::Key_Down/Pressing()               │
│    ↓ 하드웨어 폴링                                │
│  DirectInput8 (LPDIRECTINPUTDEVICE8)              │
└──────────────────────────────────────────────────┘
```

- Engine은 **"이 키가 눌렸는가?"** 까지만 답변
- **"이 입력 시퀀스가 파동권인가?"** 는 Client의 책임
- `CKey_Manager`는 프로젝트에 포함되어 있지만 **실제 사용되지 않는 데드 코드** (`GetAsyncKeyState` 기반, CGameInstance에서 참조 없음)

---

## 2. 클래스 간 소유/참조 관계

```
CGameInstance (싱글톤)
  │ [소유] CInput_Device* m_pInput_Device
  │          ├── LPDIRECTINPUT8     m_pInputSDK   (DirectInput SDK)
  │          ├── LPDIRECTINPUTDEVICE8 m_pKeyBoard (키보드 디바이스)
  │          └── LPDIRECTINPUTDEVICE8 m_pMouse    (마우스 디바이스)
  │
  │ [호출 위임] Key_Down/Up/Pressing, Mouse_Down/Up/Pressing
  │             Get_DIKeyState, Get_DIMouseState, Get_DIMouseMove
  │
  ↓ (Client에서 참조)
CCharacter (게임 오브젝트)
  │ [참조] m_pGameInstance → CGameInstance
  │ [소유] vector<CInput> inputBuffer           (입력 버퍼)
  │ [소유] vector<CommandPatternFunction>        (커맨드 패턴 목록)
  │ [static] Command_236Attack, Command_214Attack... (패턴 정의)
  │
  └─→ CInput (방향 + 버튼 조합 구조체)
```

### 생성 체인

```
CMainApp::Initialize()
  → CGameInstance::Initialize_Engine(hInst, hWnd, ...)
    → CInput_Device::Create(hInst, hWnd)
      → Ready_InputDev()
        → DirectInput8Create()
        → CreateDevice(GUID_SysKeyboard) → SetDataFormat → SetCooperativeLevel → Acquire
        → CreateDevice(GUID_SysMouse)    → SetDataFormat → SetCooperativeLevel → Acquire
```

### 해제 체인

```
CGameInstance::Release_Engine()
  → Safe_Release(m_pInput_Device)
    → CInput_Device::Free()
      → Safe_Release(m_pKeyBoard)    // COM Release
      → Safe_Release(m_pMouse)       // COM Release
      → Safe_Release(m_pInputSDK)    // COM Release
```

---

## 3. 한 프레임 기준 호출 흐름

```
CMainApp::Update(fTimeDelta)
  └→ CGameInstance::Update_Engine(fTimeDelta)
      │
      ├─① CInput_Device::Update()              ← 하드웨어 폴링
      │     ├─ memcpy(PrevKey ← CurKey)         ← 이전 프레임 상태 백업
      │     ├─ m_pKeyBoard->GetDeviceState()    ← 키보드 256바이트 읽기
      │     ├─ PrevMouse ← CurMouse             ← 마우스 상태 백업
      │     └─ m_pMouse->GetDeviceState()       ← 마우스 상태 읽기
      │
      ├─② CObject_Manager::Destory_Update()     ← 예약 삭제 처리
      │
      ├─③ CObject_Manager::Player_Update(dt)    ← 플레이어 전용 업데이트
      │     └→ CCharacter::Player_Update(dt)
      │         ├─ InputCommand()                ← ③-a. 키 상태 → CInput 변환
      │         │   ├─ Key_Pressing(DIK_W/A/S/D) → DirectionInput 계산
      │         │   ├─ Key_Down(DIK_U/I/J/K/...) → ButtonInput 계산
      │         │   ├─ 중복 입력 필터링 (이전 입력과 동일하면 무시)
      │         │   └─ UpdateInputBuffer(CInput) → inputBuffer에 추가
      │         │
      │         ├─ InputedCommandUpdate(dt)       ← ③-b. 입력 버퍼 에이징
      │         │   └─ 각 CInput.frameTime += dt
      │         │      0.35초 초과 항목 제거 (remove_if + erase)
      │         │
      │         └─ CheckAllCommands()             ← ③-c. 커맨드 패턴 매칭
      │             ├─ MoveCommandPatternsFunction 순회
      │             │   └─ CheckCommandSkippingExtras() (관대한 매칭)
      │             └─ MoveCommandPatternsFunction_Exactly 순회
      │                 └─ CheckCommand_Exactly() (엄밀한 매칭)
      │
      ├─④ CObject_Manager::Camera_Update(dt)
      ├─⑤ CObject_Manager::Update(dt)
      └─⑥ CObject_Manager::Late_Update(dt)
```

### 핵심 설계 결정: Player_Update 분리

일반 `Update()`보다 **먼저** `Player_Update()`가 호출된다. 이유:
- 플레이어 입력 → 행동 결정이 다른 오브젝트 갱신보다 선행되어야 함
- 카메라가 플레이어 위치를 추적하므로 `Camera_Update()`도 분리

---

## 4. 설계 패턴

### 4.1 파사드 패턴 (Facade)

`CGameInstance`가 `CInput_Device`의 모든 공개 메서드를 1:1 위임한다.

```cpp
// CGameInstance.cpp — 순수 위임
_bool CGameInstance::Key_Down(_uint _iKey) {
    return m_pInput_Device->Key_Down(_iKey);
}
```

**효과**: Client는 `CInput_Device`의 존재를 모르고 `CGameInstance`만 알면 된다.

### 4.2 커맨드 패턴 (Command Pattern)

격투 게임 입력을 **패턴(시퀀스) + 액션(콜백)** 으로 분리한다.

```cpp
struct CommandPatternFunction {
    vector<CInput> pattern;        // 입력 시퀀스 조건
    std::function<void()> action;  // 매칭 시 실행할 콜백
};
```

등록 예시 (Goku):
```cpp
MoveCommandPatternsFunction.push_back({
    Command_236Attack,
    bind(&CGoku_MeleeAttack::Attack_236, &m_tAttackMap)
});
```

**효과**: 새 기술 추가 = 패턴 정의 + 콜백 함수만 추가. 입력 처리 로직 수정 불필요.

### 4.3 입력 버퍼 패턴 (Ring Buffer 변형)

```cpp
void UpdateInputBuffer(CInput newInput) {
    if (inputBuffer.size() >= BUFFER_SIZE)  // 30 프레임 제한
        inputBuffer.erase(inputBuffer.begin());
    inputBuffer.push_back(newInput);
}
```

- 고정 크기(30) vector를 링 버퍼처럼 사용
- 시간 기반 만료: 0.35초 경과 시 제거
- 중복 필터: 이전 프레임과 동일한 입력은 추가하지 않음

### 4.4 싱글톤 (Singleton)

`CGameInstance`가 `DECLARE_SINGLETON/IMPLEMENT_SINGLETON` 매크로로 구현. 입력 디바이스를 전역 접근 가능하게 만든다.

---

## 5. DirectX API 호출 지점과 래핑 방식

### 5.1 초기화 (Ready_InputDev)

| DirectInput API | 호출 위치 | 래핑 방식 |
|-----------------|-----------|-----------|
| `DirectInput8Create()` | Ready_InputDev | SDK 객체 생성 → `m_pInputSDK`에 저장 |
| `CreateDevice(GUID_SysKeyboard)` | Ready_InputDev | 키보드 디바이스 → `m_pKeyBoard` |
| `CreateDevice(GUID_SysMouse)` | Ready_InputDev | 마우스 디바이스 → `m_pMouse` |
| `SetDataFormat(&c_dfDIKeyboard)` | Ready_InputDev | 키보드 데이터 포맷 설정 |
| `SetDataFormat(&c_dfDIMouse)` | Ready_InputDev | 마우스 데이터 포맷 설정 |
| `SetCooperativeLevel(DISCL_BACKGROUND \| DISCL_NONEXCLUSIVE)` | Ready_InputDev | 비독점 + 백그라운드 모드 |
| `Acquire()` | Ready_InputDev | 디바이스 접근 권한 획득 |

### 5.2 매 프레임 폴링 (Update)

| DirectInput API | 래핑 방식 |
|-----------------|-----------|
| `m_pKeyBoard->GetDeviceState(256, m_byKeyState)` | 256바이트 키보드 상태 배열 통째로 읽기 |
| `m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_tMouseState)` | 마우스 버튼 + 이동량 구조체 통째로 읽기 |

### 5.3 상태 판정 래핑

```
Raw 바이트 (0x80 비트 마스크) → Pressing/Down/Up 논리 상태
```

```cpp
// Pressing: 현재 프레임에 눌려 있는가
_bool Key_Pressing(_uint _iKey) {
    return (m_byKeyState[_iKey] & 0x80) != 0;
}

// Down: 이전=미눌림, 현재=눌림 (상승 엣지)
_bool Key_Down(_uint _iKey) {
    return (!(m_byPrevKeyState[_iKey] & 0x80) && (m_byKeyState[_iKey] & 0x80));
}

// Up: 이전=눌림, 현재=미눌림 (하강 엣지)
_bool Key_Up(_uint _iKey) {
    return ((m_byPrevKeyState[_iKey] & 0x80) && !(m_byKeyState[_iKey] & 0x80));
}
```

마우스 버튼도 동일 패턴이지만 `_bool` 배열로 추적:
```cpp
// Update에서 변환
m_bMouseState[i] = (m_tMouseState.rgbButtons[i] & 0x80) != 0;
```

마우스 이동량은 `DIMOUSESTATE` 구조체 메모리 레이아웃을 직접 포인터 연산으로 접근:
```cpp
_long Get_DIMouseMove(MOUSEMOVESTATE eMouseState) {
    return *(((_long*)&m_tMouseState) + eMouseState);
}
// eMouseState: DIMM_X=0, DIMM_Y=1, DIMM_WHEEL=2
// DIMOUSESTATE 레이아웃: { lX, lY, lZ, rgbButtons[4] }
```

---

## 6. 격투 게임 커맨드 시스템 상세

### 6.1 입력 추상화 (CInput)

```cpp
enum DirectionInput {    // 격투 게임 넘패드 표기법과 대응
    MOVEKEY_NEUTRAL,     // 5 (중립)
    MOVEKEY_UP,          // 8
    MOVEKEY_DOWN,        // 2
    MOVEKEY_LEFT,        // 4 (뒤)
    MOVEKEY_RIGHT,       // 6 (앞)
    MOVEKEY_UP_LEFT,     // 7
    MOVEKEY_UP_RIGHT,    // 9
    MOVEKEY_DOWN_LEFT,   // 1
    MOVEKEY_DOWN_RIGHT   // 3
};

enum ButtonInput {
    ATTACK_NONE, ATTACK_LIGHT, ATTACK_MEDIUM,
    ATTACK_HEAVY, ATTACK_SPECIAL, ATTACK_GRAB,
    ATTACK_BENISHING, ATTACK_TRANSFORM
};
```

**방향 상대화**: `m_iLookDirection`(+1/-1)을 곱해 좌우 방향을 캐릭터 시선 기준으로 변환한다.
```cpp
DirectionX -= m_iLookDirection;  // A키: 뒤로
DirectionX += m_iLookDirection;  // D키: 앞으로
```

### 6.2 커맨드 패턴 정의 (static)

```cpp
// 236+공격 = ↓ → ↘ + 공격 (하도켄 모션)
vector<CInput> Command_236Attack = {
    {MOVEKEY_DOWN, ATTACK_NONE},
    {MOVEKEY_DOWN_RIGHT, ATTACK_NONE},
    {MOVEKEY_RIGHT, ATTACK_LIGHT}   // 마지막에 버튼
};

// 단순 공격 = 버튼 1개
vector<CInput> Command_LightAttack = { {MOVEKEY_NEUTRAL, ATTACK_LIGHT} };

// 백대시 = 뒤 → 중립 → 뒤
vector<CInput> Command_BackDash = {
    {MOVEKEY_LEFT, ATTACK_NONE},
    {MOVEKEY_NEUTRAL, ATTACK_NONE},
    {MOVEKEY_LEFT, ATTACK_NONE}
};
```

### 6.3 두 가지 매칭 알고리즘

**CheckCommandSkippingExtras** (관대한 매칭):
- 버퍼를 순회하며 패턴의 각 요소를 **순서대로** 찾되, 중간에 다른 입력이 끼어도 허용
- 용도: 복합 커맨드 (236, 214 등) — 실제 플레이에서 깨끗한 입력은 거의 불가능

**CheckCommand_Exactly** (엄밀한 매칭):
- 버퍼 내에서 패턴이 **연속으로 정확히** 일치해야 함
- 용도: 백대시, 리플렉트 등 — 의도치 않은 발동 방지

**우선순위**: 복합 커맨드(MoveCommandPatternsFunction)를 **먼저** 검사 → 단순 커맨드(MoveCommandPatternsFunction_Exactly)를 **나중에** 검사. 236+L이 단순 L공격보다 우선.

### 6.4 2인 플레이 키 매핑

| 기능 | Player 1 (Team 1) | Player 2 (Team 2) |
|------|-------------------|-------------------|
| 방향 | W/A/S/D | Arrow Keys |
| 약공격 | U | Numpad 7 |
| 중공격 | I | Numpad 8 |
| 특수 | J | Numpad 4 |
| 강공격 | K | Numpad 5 |
| 잡기 | O | Numpad 9 |
| 베니싱 | L | Numpad 6 |
| 변신 | M | Numpad 1 |

---

## 7. CKey_Manager 분석 (미사용 코드)

`GetAsyncKeyState()` (Windows API) 기반으로 구현되어 있지만, `CGameInstance`에서 참조하지 않는다.

```cpp
// CKey_Manager::Update() — 매 프레임 전체 VK 스캔
for (int i = 0; i < VK_MAX; i++) {
    m_bKeyState[i] = GetAsyncKeyState(i) ? true : false;
}
```

**DirectInput과의 차이점**:

| 항목 | `CInput_Device` (DirectInput) | `CKey_Manager` (Win32 API) |
|------|------------------------------|---------------------------|
| 키코드 | `DIK_*` (스캔코드) | `VK_*` (가상 키코드) |
| 마우스 | 통합 처리 | 미지원 |
| 상태 추적 | prev/cur 배열 분리 | 단일 배열 + GetAsyncKeyState 재호출 |
| 포커스 | `DISCL_BACKGROUND`로 항상 수신 | 포커스 윈도우만 응답 |
| 지연 | 직접 디바이스 폴링 (저지연) | OS 메시지 큐 경유 |

**추정**: 프로젝트 초기에 `CKey_Manager`로 시작했다가 DirectInput으로 전환, 제거하지 않은 것으로 보임.

---

## 8. 프레임워크 참고 설계 판단

### 8.1 채택할 만한 설계

**엣지 검출 패턴 (prev/cur 비교)**
- `Down = !prev && cur`, `Up = prev && !cur`는 보편적이고 안정적
- DirectInput의 `0x80` 비트 마스크 처리 방식도 표준적

**커맨드 패턴의 데이터 주도 설계**
- 입력 시퀀스를 `vector<CInput>`로 데이터화하여 매칭 로직과 분리
- 새 기술 추가가 패턴 등록만으로 가능 (개방-폐쇄 원칙)

**입력 버퍼 + 시간 만료**
- 0.35초 시간 창으로 오래된 입력을 자동 제거
- 격투 게임의 입력 관용(leniency) 구현에 적합

**Player_Update 분리**
- 입력 처리를 일반 Update보다 선행하여 프레임 내 일관성 보장
- 의존 체인: 입력 → 플레이어 → 카메라 → 일반 오브젝트 → 후처리

**파사드 위임으로 DI 의존성 격리**
- Client는 `CGameInstance::Key_Down()`만 호출하므로, 입력 백엔드를 교체해도 Client 코드 변경 없음

### 8.2 개선 여지가 있는 부분

**중복 시스템 방치**
- `CKey_Manager`가 컴파일은 되지만 사용되지 않음 → 불필요한 빌드 비용 + 혼란

**vector를 링 버퍼처럼 사용**
- `erase(begin())`은 O(n) 비용 → `std::deque`나 실제 circular buffer가 더 효율적
- 30개 규모에서는 실질적 문제 없지만, 원칙적으로는 개선 가능

**하드코딩된 키 매핑**
- Player 1/2 키 배치가 `InputCommand()` 안에 `if/else` 분기로 하드코딩
- 키 리매핑(config)이 불가능 → 데이터 테이블로 분리하면 유연성 증가

**마우스 이동량의 포인터 연산**
- `*(((_long*)&m_tMouseState) + eMouseState)` — 메모리 레이아웃에 의존하는 unsafe 캐스트
- `switch(eMouseState)` 분기가 더 안전

**Acquire 재시도 없음**
- `Ready_InputDev`에서 `Acquire()` 1회만 호출. 포커스 변경 시 디바이스가 Lost될 수 있지만 재획득 로직 없음
- DX9 프레임워크의 `CDInputMgr`도 동일한 문제가 있음

### 8.3 내 프레임워크(DX9) 적용 시 고려사항

| 참고프로젝트2 (DX11) | 내 프레임워크 (DX9) | 차이/적용 |
|----------------------|---------------------|-----------|
| `CInput_Device` | `CDInputMgr` | 동일한 DirectInput8 사용. 구조 유사 |
| 파사드 위임 (`CGameInstance`) | 직접 싱글톤 접근 (`CDInputMgr::GetInstance()`) | 파사드 도입 고려 |
| 커맨드 패턴 시스템 | 없음 | 격투/액션 게임 시 도입 가치 있음 |
| `Player_Update` 분리 | 없음 (단일 Update 파이프라인) | 입력 의존 객체의 갱신 순서 보장에 유용 |
| `CKey_Manager` (Win32) | 없음 | DirectInput이면 불필요 |

---

## 부록: 전체 입력 흐름 시퀀스 다이어그램

```
Frame N
  │
  ├─ CInput_Device::Update()
  │   └─ GetDeviceState() × 2 (keyboard + mouse)
  │
  ├─ CCharacter::Player_Update()
  │   │
  │   ├─ InputCommand()
  │   │   ├─ Key_Pressing(DIK_W) → DirectionY = 1
  │   │   ├─ Key_Pressing(DIK_D) → DirectionX = +1 × lookDir
  │   │   ├─ → iMoveKey = MOVEKEY_RIGHT
  │   │   ├─ Key_Down(DIK_U) → iAttackkey = ATTACK_LIGHT
  │   │   ├─ CInput(RIGHT, LIGHT) != inputBuffer.back()
  │   │   └─ UpdateInputBuffer({RIGHT, LIGHT})
  │   │
  │   ├─ InputedCommandUpdate(0.016f)
  │   │   └─ 모든 CInput.frameTime += 0.016
  │   │      > 0.35초 항목 제거
  │   │
  │   └─ CheckAllCommands()
  │       ├─ 236+L 패턴? → inputBuffer에 [DOWN, DOWN_RIGHT, {RIGHT,LIGHT}] 있는지
  │       │   └─ CheckCommandSkippingExtras() → 매칭!
  │       │       └─ bind(&Attack_236)() 실행
  │       │       └─ inputBuffer.clear()
  │       └─ (매칭 성공 시 이후 패턴 검사 중단)
  │
  ├─ Camera_Update() → 플레이어 위치 기반 카메라 갱신
  ├─ Update() → 나머지 오브젝트 갱신
  └─ Late_Update() → 후처리
```
