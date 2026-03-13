# 참고프로젝트3 — 네트워크 시스템 심화 분석

## 분석 대상 파일

| 레이어 | 클래스 | 파일 |
|--------|--------|------|
| Engine | `CNetwork` | `Engine/Utility/Codes/Network.h/.cpp` |
| Client | `CClientNetwork` | `Client/Codes/ClientNetwork.h/.cpp` |
| Client | `CClientNetwork_Stage2` | `Client/Codes/ClientNetwork_Stage2.h/.cpp` |
| Engine (공유) | `NETDATA`, `eNETMSG` | `Engine/Headers/Engine_Struct.h`, `Engine_Enum.h` |
| Engine (옵저버) | `CSubject` / `CInfoSubject` | `Engine/Utility/Codes/Subject.h`, `InfoSubject.h` |

---

## 1. 핵심 책임과 경계

### Engine 레이어 — `CNetwork`
- **Winsock2 초기화·소켓 생성·서버 연결**을 캡슐화
- `SendMsg()` / `RecvMsg()` 를 통한 **NETDATA 단위 송수신 인터페이스** 제공
- `RecvData()` 는 순수 가상 함수 → 메시지 해석 책임은 Client에 위임
- **경계**: 소켓 레벨의 바이트 송수신까지만 책임. 메시지 라우팅·게임 로직 반영은 하위 클래스 몫

### Client 레이어 — `CClientNetwork` / `CClientNetwork_Stage2`
- `RecvData()` 를 구현하여 서버 메시지를 **메시지 타입별 핸들러**로 분기
- 원격 플레이어 **생성·삭제·상태 동기화**를 직접 수행
- `CInfoSubject` (옵저버)를 통해 게임 시스템(Scene, UI 등)에 **변경 알림 전파**
- **경계**: 네트워크 메시지 → 게임 오브젝트 반영까지. 렌더링·물리 등에는 관여하지 않음

```
┌─────────────────────────────────────────────────┐
│  Server (외부)                                   │
└──────────────┬──────────────────────────────────┘
               │ TCP (Winsock2, Port 6872)
┌──────────────▼──────────────────────────────────┐
│  CNetwork (Engine)                               │
│  - WSAStartup, socket(), connect()               │
│  - send() / recv()  ← NETDATA 고정 크기 송수신   │
│  - RecvData() = 0  (순수 가상)                   │
└──────────────┬──────────────────────────────────┘
               │ 상속
┌──────────────▼──────────────────────────────────┐
│  CClientNetwork (Client) — 싱글톤               │
│  - RecvData() 구현: while 루프 (별도 스레드)     │
│  - 메시지별 핸들러 (Create/Delete/Update/Chat…)  │
│  - 원격 플레이어 배열 관리                       │
│  - CInfoSubject 를 통한 옵저버 알림              │
└──────────────┬──────────────────────────────────┘
               │ 직접 참조 / dynamic_cast
┌──────────────▼──────────────────────────────────┐
│  CPlayer, CPlayer_Goblin, CChat 등              │
│  (게임 오브젝트 — 수신 데이터 반영)              │
└─────────────────────────────────────────────────┘
```

---

## 2. 클래스 간 소유·참조 관계

### 생성 흐름
```
CMainApp::Initialize()
  └─ CClientNetwork::GetInstance()         ← 싱글톤 최초 접근 시 생성
       └─ InitNetwork(pGraphicDev)         ← Winsock 초기화 + 서버 연결
            └─ m_pGraphicDev->AddRef()     ← COM 참조 카운트 증가
```

### 소유 관계

| 소유자 | 소유 대상 | 방식 |
|--------|-----------|------|
| `CNetwork` | `SOCKET socket_client` | 직접 소유 (closesocket으로 해제) |
| `CNetwork` | `LPDIRECT3DDEVICE9 m_pGraphicDev` | AddRef/Release COM 패턴 |
| `CClientNetwork` | `m_pPlayerArray[MAX_CLIENT_NUM]` | 포인터 배열, Add_Ref/Safe_Release |
| `CClientNetwork` | `m_pPlayerArray2[MAX_CLIENT_NUM2]` | Stage2용 플레이어 배열 (동일 방식) |
| `CClientNetwork` | `m_vecAI` | vector, AI 오브젝트 소유 (Add_Ref/Safe_Release) |
| `CClientNetwork` | `m_pChat` | CChat 객체 포인터, Add_Ref/Safe_Release |
| `CClientNetwork_Stage2` | `m_pPlayerArray[MAX_CLIENT_NUM2]` | CClientNetwork와 동일 패턴 |

### 참조 관계 (비소유)
- `CClientNetwork` → `CInfoSubject::GetInstance()` : 싱글톤 참조, 옵저버 알림 전파
- `CClientNetwork` → `CObject_Manager::GetInstance()` : 생성한 플레이어를 Scene 레이어에 등록
- `CClientNetwork` → `CDataManager::GetInstance()` : ID 설정, AI 패턴 데이터 조회
- `CPlayer` → `CClientNetwork::GetInstance()` : 역참조로 `SendMsg()` 직접 호출

---

## 3. 주요 함수의 호출 흐름 (한 프레임 기준)

### 3-1. 초기화 (앱 시작 시 1회)

```
CMainApp::Initialize()
  ├─ CClientNetwork::GetInstance()->InitNetwork(pGraphicDev)
  │     ├─ WSAStartup(WINSOCK_VERSION, &wsaData)
  │     ├─ socket(AF_INET, SOCK_STREAM, 0)
  │     ├─ 서버 IP 파일 읽기 (CreateFile → ReadFile → CloseHandle)
  │     ├─ inet_pton() 으로 주소 설정
  │     ├─ connect(socket_client, &servAddr, ...)
  │     └─ m_pGraphicDev->AddRef()
  │
  └─ std::thread(CClientNetwork::RecvData).detach()    ← 수신 전용 스레드 시작
```

### 3-2. 수신 스레드 (무한 루프, 메인 스레드와 별도)

```
CClientNetwork::RecvData()       ← while(m_pInstance != nullptr) 무한 루프
  ├─ RecvMsg()
  │     ├─ recv(socket_client, buffer, sizeof(NETDATA), 0)  ← 블로킹 수신
  │     └─ return *(NETDATA*)buffer
  │
  ├─ ErrorCheck()                ← WSAGetLastError() 로 소켓 에러 진단
  │
  ├─ chKey 검증 (72 확인)       ← 유효 메시지 필터링
  │
  └─ switch(RxData.eMessage)     ← 메시지 타입별 분기
        ├─ NETMSG_SERVER_SET_INDEX       → g_iClientIndex 설정 (접속 시 1회)
        ├─ NETMSG_SERVER_CREATE_PLAYER   → m_bClientSetting[idx] = true
        ├─ NETMSG_SERVER_DELETE_PLAYER   → m_bClientSetting[idx] = false
        ├─ NETMSG_SERVER_PLAYER_UPDATE   → Player->Set_LocalData(tData)
        ├─ NETMSG_SERVER_UPDATE_TOWER    → Player->Set_TowerData(tData)
        ├─ NETMSG_SERVER_START_COMBAT    → m_ePhaseType 변경 + 옵저버 Notify
        ├─ NETMSG_SERVER_CHANGE_PLAYER   → Player 교체
        ├─ NETMSG_SERVER_SEND_CHATTING   → Chat->Get_ServerChat(tData)
        ├─ NETMSG_SERVER_PLAYER_UPDATE2  → Goblin->Set_LocalData(tData)
        ├─ NETMSG_SERVER_SEND_ID         → DataManager에 ID 저장
        ├─ NETMSG_SERVER_DECAL           → Goblin->Create_Decal()
        ├─ NETMSG_SERVER_ROUND2_STATE    → 라운드 진행 상태 처리
        └─ NETMSG_SERVER_KILL            → 킬 처리 (플레이어/AI 분기)
```

### 3-3. 메인 스레드 Update (매 프레임)

```
CScene_Stage::Update(fTimeDelta)
  ├─ m_pClinetNetwork->Check_Player_Create()
  │     └─ for each index: m_bClientSetting[i] == true && m_pPlayerArray[i] == nullptr
  │           └─ Create_Player(i)
  │                 ├─ CPlayer_Monk::Create(m_pGraphicDev)  (index별 캐릭터 분기)
  │                 ├─ Player->SetServerIndex(i)
  │                 ├─ m_pPlayerArray[i] = pGameObject; pGameObject->Add_Ref()
  │                 └─ CObject_Manager::Add_GameObject(scene, L"Layer_GameObject", ...)
  │
  └─ m_pClinetNetwork->Check_Player_Delete()
        └─ for each index: m_bClientSetting[i] == false && m_pPlayerArray[i] != nullptr
              └─ Delete_Player(i)
                    ├─ Player->Set_ServerExit()
                    ├─ Safe_Release(m_pPlayerArray[i])
                    └─ CInfoSubject::Notify_Message(MESSAGE_PLAYER_DELETE)
```

### 3-4. 송신 흐름 (게임 오브젝트 → 서버)

```
CPlayer::Update(fTimeDelta)       ← 매 프레임 로컬 플레이어가 호출
  └─ SendLocalData()
        ├─ NETDATA 구성: chKey=72, eMessage=NETMSG_CLIENT_PLAYER_UPDATE
        │   iIndex, matWorld, iHighAnimation, iLowAnimation, eMainState, vecPick
        └─ CClientNetwork::GetInstance()->SendMsg(tData)
              └─ send(socket_client, (char*)&tData, sizeof(tData), 0)
```

---

## 4. 사용된 디자인 패턴

### 4-1. 싱글톤 (Singleton)
- **적용 클래스**: `CClientNetwork`, `CClientNetwork_Stage2`, `CInfoSubject`
- **구현**: `DECLARE_SINGLETON` / `IMPLEMENT_SINGLETON` 매크로
- **용도**: 네트워크 연결은 앱 전체에서 하나만 유지. 어디서든 `GetInstance()`로 접근

### 4-2. 옵저버 (Observer)
- **구조**: `CSubject` (추상) → `CInfoSubject` (구체) + `CObserver` (구독자)
- **네트워크 시스템에서의 사용**:
  ```
  // 데이터 등록 (생성 시)
  CInfoSubject::Add_Data(MESSAGE_PHASECHANGE, &m_ePhaseType)
  CInfoSubject::Add_Data(MESSAGE_PLAYER_DELETE, &m_iIndex)

  // 알림 발행 (이벤트 발생 시)
  CInfoSubject::Notify_Message(MESSAGE_PHASECHANGE)    // 전투 시작
  CInfoSubject::Notify_Message(MESSAGE_PLAYER_DELETE)   // 플레이어 퇴장
  CInfoSubject::Notify_Message(MESSAGE_LASTMAN_DELETE)  // Stage2 퇴장
  ```
- **특징**: 데이터(`void*`)를 메시지 키에 매핑하여, 옵저버가 `GetDatalist(message)`로 관련 데이터를 꺼내 가는 **Pull 방식** 혼합

### 4-3. 템플릿 메서드 (Template Method)
- `CNetwork::RecvData()` = 순수 가상 → 하위 클래스가 메시지 분기 로직을 구현
- `CNetwork` 가 소켓 초기화·송수신 뼈대를 제공하고, 해석 정책은 하위에 위임

### 4-4. 메시지 디스패치 (Message Dispatch)
- `RecvData()` 내부에서 `switch(RxData.eMessage)` 로 메시지 타입별 핸들러 호출
- 명시적인 Command 객체는 없지만, `eNETMSG` enum + `NETDATA` 구조체가 사실상 **커맨드 패킷** 역할

### 4-5. 참조 카운팅 (Reference Counting)
- `CBase` 기반의 `Add_Ref()` / `Safe_Release()` — COM 스타일 수명 관리
- 네트워크가 소유하는 원격 플레이어 객체와 `CObject_Manager`에 등록된 동일 객체가 공동 참조

---

## 5. DirectX API 호출 지점과 래핑 방식

### 이 시스템에서의 DirectX 사용은 극히 제한적

| 호출 지점 | DirectX API | 용도 |
|-----------|-------------|------|
| `CNetwork::InitNetwork()` | `LPDIRECT3DDEVICE9` 파라미터 수신, `AddRef()` | 디바이스 참조를 보관하여 하위 클래스에서 **플레이어 생성 시** 전달 |
| `CNetwork::Free()` | `Safe_Release(m_pGraphicDev)` | 디바이스 참조 해제 |
| `NETDATA::matWorld` | `D3DXMATRIX` 타입 필드 | 플레이어 월드 행렬을 네트워크로 직접 전송 |
| `NETDATA::vecPick` | `_vec3` (`D3DXVECTOR3`) 타입 필드 | 픽킹 좌표 전송 |

**래핑 방식**: 없음. DirectX 타입(`D3DXMATRIX`, `D3DXVECTOR3`)을 `NETDATA` 구조체에 **직접 포함**시켜 네트워크 패킷으로 전송. 별도의 직렬화/역직렬화 계층 없이 **구조체를 통째로 `send()`/`recv()`** 한다.

```cpp
// 송신: 구조체 → 바이트 배열 캐스팅
send(socket_client, (char*)&tData, sizeof(tData), 0);

// 수신: 바이트 배열 → 구조체 캐스팅
RxData = (NETDATA*)buffer;
```

---

## 6. NETDATA 프로토콜 구조

```cpp
typedef struct tagNetData
{
    char        szChatMessage[16];  // 채팅 메시지 텍스트
    char        chKey;              // 매직 넘버 '72' — 유효 패킷 검증
    eNETMSG     eMessage;           // 주 메시지 타입 (enum)
    WORD        wSubMessage;        // 보조 메시지 (용도에 따라 다름)
    int         iIndex;             // 클라이언트/플레이어 인덱스
    D3DXMATRIX  matWorld;           // 월드 변환 행렬 (4x4 = 64바이트)
    int         iHighAnimation;     // 상체 애니메이션 상태
    int         iLowAnimation;      // 하체 애니메이션 상태
    int         eMainState;         // 플레이어 상태
    _vec3       vecPick;            // 픽킹 좌표
} NETDATA;
```

### 메시지 방향 규칙
- `NETMSG_CLIENT_*` : 클라이언트 → 서버 전송용
- `NETMSG_SERVER_*` : 서버 → 클라이언트 수신용
- 동일 이벤트에 대해 CLIENT/SERVER 쌍이 존재 (예: `CLIENT_PLAYER_UPDATE` ↔ `SERVER_PLAYER_UPDATE`)

### 매직 넘버 검증
- `chKey == 72` 로 유효 패킷 필터링. 손상·잔여 바이트 방어용

---

## 7. 설계 특성 및 참고할 판단들

### 참고할 만한 설계 판단

| 판단 | 설명 | 평가 |
|------|------|------|
| **Engine/Client 분리** | 소켓 관리(`CNetwork`)는 Engine에, 메시지 해석(`CClientNetwork`)은 Client에 배치. 엔진 재사용성 확보 | 좋은 판단 |
| **순수 가상 RecvData** | 메시지 해석 정책을 하위 클래스에 완전 위임. 서버용으로 확장할 때도 `CNetwork` 재사용 가능 | 좋은 판단 |
| **수신 전용 스레드** | `std::thread` + `detach()`로 블로킹 `recv()`를 메인 루프에서 분리. UI 프리징 방지 | 좋은 판단 |
| **플래그 기반 지연 생성** | 수신 스레드에서 `m_bClientSetting[i] = true`만 세팅 → 메인 스레드 Update에서 실제 오브젝트 생성. **스레드 안전성** 확보 (DirectX 디바이스 접근은 메인 스레드에서만) | 핵심 설계 판단 |
| **옵저버를 통한 간접 알림** | 플레이어 삭제·페이즈 전환 등을 `CInfoSubject::Notify_Message()`로 전파. 네트워크 코드가 UI·씬 코드를 직접 참조하지 않음 | 좋은 판단 |
| **서버 IP 외부 파일 관리** | `Server.txt`에서 IP를 읽어 런타임 설정 가능 (단, 하드코딩 loopback으로 덮어쓰는 코드가 남아 있음) | 방향은 좋으나 미완성 |

### 개선이 필요한 부분 (내 프레임워크에서 보완할 점)

| 문제점 | 상세 | 개선 방향 |
|--------|------|-----------|
| **고정 크기 패킷** | `sizeof(NETDATA)` 통째로 송수신 → `D3DXMATRIX`(64B) 포함하여 패킷이 항상 큼. 채팅만 보내도 전체 크기 전송 | 메시지 타입별 가변 크기 직렬화 도입 |
| **직렬화 없음** | 구조체 메모리 레이아웃을 네트워크에 그대로 노출. 엔디안·패딩·컴파일러 차이에 취약 | 직렬화 계층 (protobuf, flatbuffers, 또는 수동 직렬화) |
| **스레드 동기화 미비** | `m_bClientSetting[]` 플래그를 수신 스레드에서 쓰고 메인 스레드에서 읽지만, mutex/atomic 없음. 현재 `bool` 단일 값이라 실질적 문제는 적지만 구조적으로 위험 | `std::atomic<bool>` 또는 메시지 큐 사용 |
| **에러 복구 부재** | 연결 끊김 시 `closesocket()` 후 루프 종료. 재연결 로직이 주석 처리된 채 방치 | 재연결 정책·백오프 메커니즘 구현 |
| **Stage별 중복 코드** | `CClientNetwork`과 `CClientNetwork_Stage2`가 거의 동일한 구조 반복. `ErrorCheck()` 함수도 복붙 | 공통 로직을 베이스 클래스로 통합하거나, Stage를 파라미터화 |
| **플레이어 생성의 하드코딩** | `Create_Player()`에서 index에 따라 캐릭터 클래스가 고정 분기 (`case 1: Monk, case 3: Mage`) | 팩토리 패턴 또는 서브메시지 기반 캐릭터 타입 전달 |
| **dynamic_cast 남용** | 수신 핸들러에서 `CGameObject*` → `CPlayer*`/`CPlayer_Goblin*` 다운캐스팅이 빈번 | 타입 안전한 플레이어 배열 또는 인터페이스 설계 |

### 내 프레임워크에 적용할 핵심 아이디어

1. **플래그 기반 지연 생성 패턴**: 네트워크 수신 스레드에서는 플래그만 세팅하고, 실제 리소스 생성(GPU 자원 포함)은 메인 스레드의 Update 루프에서 수행. 이 패턴은 DirectX 디바이스의 단일 스레드 제약과 게임 루프 동기화를 동시에 해결함
2. **Engine 레이어에서 소켓 래핑, Client에서 프로토콜 해석**: 분리 경계를 명확히 두면 엔진을 다른 프로젝트에 재사용할 때 네트워크 계층을 가져갈 수 있음
3. **옵저버로 네트워크 이벤트 전파**: 네트워크 시스템이 UI/Scene 등 상위 시스템을 직접 참조하지 않도록 `Subject-Observer` 패턴으로 디커플링
4. **메시지 큐 도입 검토**: 현재 구조의 플래그 방식을 발전시켜, 수신 스레드에서 `ConcurrentQueue<NETDATA>`에 적재 → 메인 스레드에서 큐를 drain하는 방식으로 개선하면 확장성과 안전성 모두 확보
