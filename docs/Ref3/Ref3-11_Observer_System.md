# 참고프로젝트3 — 옵저버 패턴 시스템 심화 분석

> **분석 대상**: CSubject, CObserver, CInfoSubject(엔진) + CPlayerObserver, CSceneObserver, CWaveObserver, CLastManObserver(클라이언트) + OBSERVER_MESSAGE enum, ANI_INFO/PLAYER_INFO/WAVE_INFO/LASTMAN_INFO 구조체
> **모드**: [분석]

---

## 1. 핵심 책임과 시스템 경계

### 이 시스템이 담당하는 것
- **이벤트 기반 데이터 공유**: 게임 오브젝트가 데이터를 등록하면 관심 있는 옵저버에게 자동 통보
- **느슨한 결합**: 발행자(플레이어, 몬스터, 씬)가 구독자(UI, 카메라, 게임 로직)를 직접 참조하지 않음
- **메시지 ID 기반 라우팅**: `int message`로 이벤트 종류 구분 → 옵저버가 switch로 관심 메시지만 처리
- **`void*` 데이터 버스**: 타입 무관하게 임의의 데이터 포인터를 메시지별 리스트로 관리

### 시스템 경계 — 이것은 하지 않는다
- **데이터 소유**: `void*` 포인터만 보관, 실제 데이터의 수명은 발행자가 관리
- **메시지 큐잉/비동기**: 동기식 즉시 통보 — `Add_Data` 호출 시점에 모든 옵저버 즉시 실행
- **타입 안전성**: `void*` 캐스팅 — 잘못된 타입 캐스트 시 런타임 에러
- **프레임 루프 통합**: Update/Render 파이프라인에 직접 끼어들지 않음 (이벤트성)

---

## 2. 클래스 간 소유/참조 관계

```
CBase
├── CSubject (추상 — 옵저버 리스트 관리)
│   │   Subscribe_Observer(CObserver*)    ← 구독 등록
│   │   UnSubscribe_Observer(CObserver*)  ← 구독 해제
│   │   Notify_Message(int message)       ← 전체 옵저버에게 통보
│   │   m_Observerlist : list<CObserver*> ← 구독자 리스트 (소유 안 함, 참조만)
│   │
│   └── CInfoSubject (싱글톤 — 메시지+데이터 허브) ★
│       │   DECLARE_SINGLETON
│       │   m_mapDatalist : map<int, list<void*>>  ← 메시지ID → 데이터 포인터 리스트
│       │
│       │   Add_Data(msg, void*)       → push_back + Notify_Message
│       │   Add_DataFront(msg, void*)  → push_front + Notify_Message
│       │   Remove_Data(msg, void*)    → 리스트에서 제거 (통보 없음)
│       │   GetDatalist(msg)           → list<void*>* 반환
│       │
│       └── [유일한 인스턴스 — 전역 데이터 버스 역할]
│
└── CObserver (추상 — 이벤트 수신자)
    │   Ready_Observer() PURE       ← 초기화
    │   Update_Observer(int) PURE   ← 메시지 수신 콜백
    │
    ├── CPlayerObserver (클라이언트)
    │   └── m_arrPlayerInfo[MAX_CLIENT_NUM] : const PLAYER_INFO*
    │
    ├── CSceneObserver (클라이언트)
    │   └── m_eCurrentScene : SCENEID
    │
    ├── CWaveObserver (클라이언트)
    │   ├── m_tWaveInfo : WAVE_INFO (값 보유)
    │   └── m_pGraphicDev : LPDIRECT3DDEVICE9 (AddRef — UI 생성용)
    │
    └── CLastManObserver (클라이언트)
        ├── m_arrLastManInfo[MAX_CLIENT_NUM2] : const LASTMAN_INFO*
        └── m_iAICount, m_iPlayerCount 등 카운터
```

### 소유 원칙

| 관계 | 방식 |
|------|------|
| Subject → Observer | **참조만** (list에 포인터 저장, Release/Delete 안 함) |
| InfoSubject → 데이터 | **참조만** (`void*` 포인터 저장, 해제 안 함) |
| Observer → 데이터 | **참조만** (`const PLAYER_INFO*` 등 포인터 보관) |
| Observer → InfoSubject | `GetInstance()` 직접 호출 (싱글톤 참조) |
| WaveObserver → DX 디바이스 | `AddRef()` 소유 (UI 오브젝트 생성에 필요) |

**핵심**: 이 시스템은 **아무것도 소유하지 않는다** — 순수하게 포인터 참조와 이벤트 통보만 담당

---

## 3. 주요 호출 흐름

### 3-1. 초기화 (씬 시작 시)

```
Scene::Ready_Scene()
  │
  ├─ CInfoSubject::GetInstance()  ← 싱글톤 최초 생성
  │
  ├─ CPlayerObserver::Create()
  │   └─ new + Ready_Observer() → ZeroMemory(m_arrPlayerInfo)
  │
  ├─ CInfoSubject::GetInstance()->Subscribe_Observer(pPlayerObs)
  │   └─ m_Observerlist.push_back(pPlayerObs)
  │
  ├─ CSceneObserver::Create()
  ├─ Subscribe_Observer(pSceneObs)
  │
  ├─ CWaveObserver::Create(pGraphicDev)
  └─ Subscribe_Observer(pWaveObs)
```

### 3-2. 데이터 발행 (게임 오브젝트에서)

```
CPlayer::Ready_GameObject()
  │
  └─ CInfoSubject::GetInstance()->Add_Data(MESSAGE_PLAYER_INFO, &m_tPlayerInfo)
      │
      ├─ m_mapDatalist[MESSAGE_PLAYER_INFO].push_back(&m_tPlayerInfo)
      │   → 이 포인터는 Player가 살아있는 한 유효
      │
      └─ Notify_Message(MESSAGE_PLAYER_INFO)  ← 즉시 통보
          └─ 모든 옵저버의 Update_Observer(MESSAGE_PLAYER_INFO) 호출
              │
              ├─ CPlayerObserver::Update_Observer()
              │   └─ 데이터 리스트 순회 → m_arrPlayerInfo[idx] = pInfo
              │
              ├─ CSceneObserver::Update_Observer()
              │   └─ MESSAGE_PLAYER_INFO에 관심 없음 → 무시 (switch default)
              │
              └─ CWaveObserver::Update_Observer()
                  └─ MESSAGE_PLAYER_INFO에 관심 없음 → 무시
```

### 3-3. 상태 변경 통보 (런타임)

```
CMonster_Goblin::Update_GameObject()
  │
  ├─ [몬스터 사망 시]
  │   CInfoSubject::GetInstance()->Add_Data(MESSAGE_MINUSCOUNT, nullptr???)
  │   → 실제로는 void*에 카운트/상태 전달
  │
  └─ Notify_Message(MESSAGE_MINUSCOUNT)
      └─ CWaveObserver::Update_Observer(MESSAGE_MINUSCOUNT)
          └─ --m_tWaveInfo.iCurUnits
              └─ if (0 == iCurUnits)
                  ├─ ePhaseType = WAVE_COMPLETE
                  ├─ ++iCurrentWave
                  ├─ Text_Phase() → UI 텍스트 오브젝트 생성
                  └─ 사운드 재생
```

### 3-4. 데이터 제거 (오브젝트 소멸 시)

```
CPlayer::Free()
  │
  └─ CInfoSubject::GetInstance()->Remove_Data(MESSAGE_PLAYER_INFO, &m_tPlayerInfo)
      └─ m_mapDatalist[MESSAGE_PLAYER_INFO]에서 해당 포인터 erase
         (Notify 호출 없음 — 제거 시엔 통보 안 함)
```

### 3-5. 전체 흐름 요약 (시퀀스)

```
[발행자]                    [InfoSubject]               [옵저버들]
   │                            │                          │
   ├─ Add_Data(MSG, &data) ───→│                          │
   │                            ├─ mapDatalist에 저장      │
   │                            ├─ Notify_Message(MSG) ──→│
   │                            │                          ├─ Update_Observer(MSG)
   │                            │                          │   GetDatalist(MSG) ──→│
   │                            │                          │   ←── list<void*>*    │
   │                            │                          │   switch(MSG) 처리    │
   │                            │                          │                      │
   ├─ Remove_Data(MSG, &data)─→│                          │
   │                            ├─ mapDatalist에서 제거     │
   │                            │  (통보 없음)              │
```

---

## 4. 사용된 디자인 패턴

### 4-1. 옵저버 패턴 (GoF) — 핵심

```cpp
// Subject: 구독자 관리
void Subscribe_Observer(CObserver* pObserver) {
    m_Observerlist.push_back(pObserver);
}
void Notify_Message(int message) {
    for (auto iter : m_Observerlist)
        iter->Update_Observer(message);  // 다형성 호출
}

// Observer: 순수 가상 콜백
virtual void Update_Observer(int message) PURE;
```
- GoF 옵저버의 정석 구현
- **Push 모델**: Subject가 메시지 ID를 밀어줌 → Observer가 Pull로 데이터 가져감

### 4-2. 싱글톤 (CInfoSubject)
- `DECLARE_SINGLETON` / `IMPLEMENT_SINGLETON` 매크로
- 전역 유일 인스턴스 → 어디서든 `GetInstance()`로 접근
- 옵저버 패턴의 Subject가 곧 전역 이벤트 버스

### 4-3. 중재자 패턴 (Mediator) 변형
- InfoSubject가 **중재자 역할** — 발행자와 구독자가 서로를 모름
- 발행자: `Add_Data(msg, data)` → InfoSubject에만 의존
- 구독자: `GetDatalist(msg)` → InfoSubject에만 의존
- 발행자↔구독자 직접 참조 없음

### 4-4. 데이터 주도 메시지 (Message + Data Bus)
```cpp
// 데이터 등록과 동시에 통보
void Add_Data(int message, void* pData) {
    m_mapDatalist[message].push_back(pData);
    Notify_Message(message);  // ← 등록 즉시 통보
}
```
- **메시지 = 이벤트 종류** (int enum)
- **데이터 = 이벤트 페이로드** (`void*` 리스트)
- 통보와 데이터 전달을 하나의 호출로 결합

### 4-5. 팩토리 메서드 (Observer::Create)
- 각 옵저버가 `static Create()` → `new` + `Ready_Observer()` + 실패 시 `Safe_Release`

---

## 5. DirectX API 호출 지점

이 시스템 자체는 **DX API를 직접 호출하지 않는다**. 순수 C++ 이벤트 시스템.

**유일한 예외 — CWaveObserver**:
```cpp
CWaveObserver(LPDIRECT3DDEVICE9 pGraphicDev) {
    m_pGraphicDev->AddRef();  // 디바이스 참조 보유
}

void Text_Phase(PHASE_TYPE eType) {
    CGameObject* pGameObject = CText_Phase::Create(m_pGraphicDev, eType);
    CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_UI", pGameObject);
}
```
- 옵저버가 직접 UI 오브젝트를 **생성**하고 **씬에 추가**
- 이를 위해 DX 디바이스와 Object_Manager에 의존 → 옵저버 패턴의 순수성이 약간 깨짐

---

## 6. OBSERVER_MESSAGE 체계

### 메시지 카테고리

| 카테고리 | 메시지 | 처리 옵저버 | 용도 |
|---------|--------|------------|------|
| **Player** | `MESSAGE_PLAYER_INFO` | PlayerObserver | 플레이어 정보 등록/갱신 |
| | `MESSAGE_PLAYER_DELETE` | PlayerObserver | 플레이어 제거 |
| | `MESSAGE_PLAYER_MP` | PlayerObserver | MP 변경 |
| **Scene** | `MESSAGE_SCENECHANGE` | SceneObserver | 씬 전환 통보 |
| **Wave** | `MESSAGE_PHASECHANGE` | WaveObserver | 전투/빌드 페이즈 전환 |
| | `MESSAGE_PLUSCOUNT` | WaveObserver | 몬스터 생성 카운트 |
| | `MESSAGE_MINUSCOUNT` | WaveObserver | 몬스터 사망 카운트 |
| | `MESSAGE_BOSS_APPEAR/HP` | WaveObserver | 보스 출현/체력 |
| **LastMan** | `MESSAGE_LASTMAN_INFO` | LastManObserver | PvP 플레이어 정보 |
| | `MESSAGE_LASTMAN_PLAYER_DEAD` | LastManObserver | PvP 사망 → 승자 판정 |

### 옵저버별 관심 메시지

```
PlayerObserver : MESSAGE_PLAYER_INFO, MESSAGE_PLAYER_DELETE, MESSAGE_PLAYER_MP
SceneObserver  : MESSAGE_SCENECHANGE
WaveObserver   : MESSAGE_PHASECHANGE, MESSAGE_BUILD_PHASECHANGE, MESSACE_CUTSCENE,
                 MESSAGE_PLUSCOUNT, MESSAGE_MINUSCOUNT, MESSAGE_MAX_UNITCOUNT,
                 MESSAGE_BOSS_APPEAR, MESSAGE_BOSS_HP, MESSAGE_STAGE_CLEAR
LastManObserver: MESSAGE_LASTMAN_INFO, MESSAGE_LASTMAN_DELETE,
                 MESSAGE_LASTMAN_COMPUTER_ADD/DEAD, MESSAGE_LASTMAN_PLAYER_ADD/DEAD
```

---

## 7. 설계 판단 분석 — 참고할 점과 한계

### 참고할 점

**1) Push-Pull 하이브리드**
```cpp
// Push: Subject가 메시지 ID를 밀어줌
Notify_Message(MESSAGE_PLAYER_INFO);

// Pull: Observer가 필요한 데이터를 당겨감
list<void*>* pDatalist = CInfoSubject::GetInstance()->GetDatalist(message);
PLAYER_INFO* pInfo = (PLAYER_INFO*)(*iter);
```
- 메시지 ID로 "무슨 일이 일어났는지" Push → 옵저버가 "어떤 데이터인지" Pull
- 모든 데이터를 Push하면 불필요한 복사 발생 → 포인터 기반 Pull이 효율적

**2) 데이터 리스트 방식 — 1:N 관계 지원**
```cpp
map<int, list<void*>> m_mapDatalist;
// MESSAGE_PLAYER_INFO → [Player1_INFO*, Player2_INFO*, Player3_INFO*, Player4_INFO*]
```
- 같은 메시지에 여러 데이터 등록 가능 → 멀티플레이어 정보를 하나의 메시지로 관리
- 옵저버가 리스트를 순회하며 필요한 것만 선택

**3) Add_Data와 Notify의 결합**
```cpp
void Add_Data(int message, void* pData) {
    m_mapDatalist[message].push_back(pData);
    Notify_Message(message);  // 등록 = 즉시 통보
}
```
- 데이터 등록과 통보를 원자적으로 처리 → 등록 후 통보 누락 방지
- 단, `Remove_Data`는 통보 없음 → 비대칭적 설계 (의도적)

**4) 전역 이벤트 버스로서의 단순함**
- 클래스 3개(Subject, Observer, InfoSubject)만으로 전체 시스템 구현
- 어디서든 `InfoSubject::GetInstance()->Add_Data(MSG, ptr)` 한 줄로 이벤트 발행
- 구독도 `Subscribe_Observer` 한 줄

**5) 옵저버의 부가 로직 — "반응형 시스템"**
```cpp
// WaveObserver: 몬스터 전멸 → 페이즈 전환 + UI 생성 + 사운드 재생
case MESSAGE_MINUSCOUNT:
    --m_tWaveInfo.iCurUnits;
    if (0 == iCurUnits) {
        ePhaseType = WAVE_COMPLETE;
        ++iCurrentWave;
        Text_Phase(WAVE_COMPLETE);      // UI 오브젝트 생성
        CSoundMgr::Play_Sound(...);     // 사운드 재생
    }
```
- 옵저버가 단순 데이터 캐싱을 넘어 게임 로직(승리 판정, UI 생성, 사운드)까지 수행
- WaveObserver = "웨이브 게임 로직 컨트롤러" 역할

### 한계/개선 가능 포인트

**1) `void*` 타입 안전성 없음**
```cpp
PLAYER_INFO* pInfo = (PLAYER_INFO*)(*iter_begin);  // 위험한 C 캐스트
SCENEID scene = *(SCENEID*)pDatalist->back();       // 타입 보장 없음
```
- 잘못된 메시지에 잘못된 타입을 등록하면 **런타임 크래시**
- 개선: 템플릿 기반 타입 세이프 이벤트 시스템, 또는 `std::any`/`std::variant`

**2) 모든 옵저버에게 모든 메시지 통보**
```cpp
void Notify_Message(int message) {
    for (auto iter : m_Observerlist)
        iter->Update_Observer(message);  // 전원에게 브로드캐스트
}
```
- SceneObserver는 `MESSAGE_PLAYER_INFO`에 관심 없지만 `Update_Observer` 호출됨
- 옵저버 4개 × 메시지 20종 = 80회 호출 중 대부분 switch default로 빠짐
- 개선: 메시지별 구독 리스트 → `map<int, list<CObserver*>>`

**3) 데이터 수명 관리 위임**
```cpp
// Player가 Add_Data로 &m_tPlayerInfo 등록
// Player가 소멸될 때 Remove_Data로 제거해야 함
// 만약 Remove 없이 소멸 → 댕글링 포인터 → 크래시
```
- InfoSubject는 데이터 수명을 **전혀 관리하지 않음**
- 발행자가 반드시 `Remove_Data`를 호출해야 안전 → 누락 시 버그
- 개선: weak_ptr, 또는 등록 시 토큰 반환 → RAII로 자동 해제

**4) 동기식 Notify — 재진입 위험**
```cpp
// Add_Data → Notify_Message → Observer::Update_Observer
//   → 옵저버 안에서 Add_Data 호출하면? → 재귀 Notify → 리스트 변경 중 순회
```
- 이 프로젝트에서는 옵저버가 `Add_Data`를 호출하지 않아 문제없음
- 하지만 일반적으로 위험 → 개선: 지연 통보 큐, 또는 Copy-on-Write 리스트

**5) Remove_Data 시 통보 없음 — 비대칭**
```cpp
void Remove_Data(int message, void* pData) {
    // 리스트에서 제거만 — Notify 호출 없음
}
```
- 플레이어 퇴장 시 PlayerObserver가 자동으로 알 수 없음
- 별도 `MESSAGE_PLAYER_DELETE` 메시지를 수동으로 발행해야 함
- 개선: `Remove_Data`에서도 Notify, 또는 `OnRemove` 콜백

**6) WaveObserver의 과도한 책임**
- 데이터 캐싱 + 게임 로직 + UI 생성 + 사운드 재생을 모두 수행
- 옵저버의 원래 목적(알림 수신)을 넘어 **게임 컨트롤러** 역할
- 개선: 옵저버는 상태만 갱신, 게임 로직은 별도 매니저에서 상태를 읽어 처리
