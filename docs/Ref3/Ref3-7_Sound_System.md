# 참고프로젝트3 — 사운드 시스템 심화 분석

> 분석 범위: `CSoundMgr` (Engine/System), `CHANNEL_TYPE` enum, `CTag_Finder` functor, Client 호출 패턴
> 분석 대상: 헤더 + cpp 구현부 전체

---

## 1. 시스템 핵심 책임과 경계

### 책임

| 영역 | 담당 | 책임 |
|------|------|------|
| **사운드 엔진 초기화** | `CSoundMgr` | FMOD System 생성, 채널 수 설정 |
| **사운드 파일 로드** | `CSoundMgr::LoadSoundFile` | 폴더 전체 일괄 스캔 → `FMOD_SOUND*` 캐시 |
| **재생 제어** | `CSoundMgr::Play_Sound/PlayBGM/Stop` | 채널별 재생/정지/볼륨 |
| **3D 사운드** | `CSoundMgr::Play_Sound` (오버로드) | 카메라-사운드 거리 기반 감쇠 |
| **랜덤 사운드** | `CSoundMgr::Play_RandomSound` | 파일명 넘버링으로 랜덤 변형 |
| **상태 조회** | `CSoundMgr::IsPlaying` | 채널 재생 여부 확인 |
| **프레임 갱신** | `CSoundMgr::UpdateSound` | FMOD 내부 상태 갱신 |
| **재생 트리거** | Client 각 오브젝트 | 상황별 사운드 호출 (공격, BGM, UI 등) |

### 경계

```
┌───────────────────────────────────────────────────────────┐
│  Client                                                    │
│                                                            │
│  CMainApp                                                  │
│    ├─ Ready: CSoundMgr::LoadSoundFile()  ← 일괄 로드       │
│    └─ Update: CSoundMgr::UpdateSound()   ← 매 프레임       │
│                                                            │
│  Scene_Menu / Scene_Stage / ...                            │
│    └─ Ready: CSoundMgr::PlayBGM(...)     ← 씬 BGM 시작    │
│                                                            │
│  Player_Mage / Monster_Goblin / Tower / ...                │
│    └─ Update: CSoundMgr::Play_Sound(...) ← 상황별 SFX     │
│                                                            │
│  WaveObserver                                              │
│    └─ Notify: CSoundMgr::Play_Sound(...) ← 웨이브 알림음   │
├───────────────────────────────────────────────────────────┤
│  Engine / System                                           │
│                                                            │
│  CSoundMgr (싱글톤)                                         │
│    ├─ FMOD_SYSTEM*  m_pSystem                              │
│    ├─ FMOD_CHANNEL* m_pChannel[CHANNEL_END]  (26개)        │
│    └─ map<TCHAR*, FMOD_SOUND*>  m_MapSound  (파일명 → 사운드)│
└───────────────────────────────────────────────────────────┘
```

**핵심 경계 원칙**:
- Engine의 `CSoundMgr`는 **FMOD 래퍼** 역할만 수행 — 재생/정지/볼륨/조회
- **어떤 사운드를 언제 재생할지**는 전적으로 Client가 결정
- 사운드 파일은 폴더 스캔으로 일괄 로드 — 개별 등록 불필요

---

## 2. 클래스 간 소유/참조 관계

```
CMainApp (Client)
  ├── [참조] CSoundMgr*  m_pSoundMgr  (Add_Ref)
  │         ├── [소유] FMOD_SYSTEM*  m_pSystem         ← FMOD 시스템
  │         ├── [소유] FMOD_CHANNEL* m_pChannel[26]    ← 채널 배열
  │         └── [소유] map<TCHAR*, FMOD_SOUND*>        ← 사운드 캐시
  │                    ├─ key: TCHAR* (new로 동적 할당) ← 파일명
  │                    └─ value: FMOD_SOUND*           ← FMOD 사운드 핸들
  │
  └── [직접 호출] CSoundMgr::GetInstance()
      ← Player, Monster, Tower, Scene, Observer 등 모두 직접 싱글톤 접근

CTag_Finder (Engine Functor)
  └── map 검색 시 lstrcmp로 TCHAR* 키 비교
```

### 소유권 정리

| 소유자 | 소유 대상 | 해제 방식 |
|--------|----------|----------|
| `CSoundMgr` | `FMOD_SYSTEM*` | `FMOD_System_Close` + `FMOD_System_Release` |
| `CSoundMgr` | `FMOD_SOUND*` (맵 value) | `FMOD_Sound_Release` (Free에서) |
| `CSoundMgr` | `TCHAR*` (맵 key) | `delete[]` (Free에서) |
| `CSoundMgr` | `FMOD_CHANNEL*` | FMOD가 내부 관리 (명시적 Release 없음) |
| `CMainApp` | `CSoundMgr*` 참조 | `Safe_Release` + `DestroyInstance` |

---

## 3. 주요 함수의 호출 흐름 (한 프레임 기준)

### 3.1 초기화 흐름

```
CMainApp::Ready_MainApp()
  │
  ├─ CSoundMgr::GetInstance()                    ← 싱글톤 생성
  │   └─ CSoundMgr::CSoundMgr()                 ← 생성자에서 즉시 Initialize
  │       └─ CSoundMgr::Initialize()
  │           ├─ FMOD_System_Create(&m_pSystem)  ← FMOD 시스템 생성
  │           └─ FMOD_System_Init(m_pSystem, CHANNEL_END, FMOD_INIT_NORMAL, NULL)
  │              (CHANNEL_END = 26개 채널 할당)
  │
  └─ CSoundMgr::LoadSoundFile()                  ← 사운드 파일 일괄 로드
      ├─ _findfirst("../Resources/Sound/*.*")    ← 폴더 전체 스캔
      ├─ while (_findnext):
      │   ├─ TCHAR* pSoundKey = new TCHAR[256]
      │   ├─ MultiByteToWideChar(fd.name → pSoundKey)   ← char→wchar
      │   ├─ szFullPath = "../Resources/Sound/" + fd.name
      │   ├─ FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_HARDWARE, NULL, &pSound)
      │   │   └─ FMOD이 파일을 디코딩하여 하드웨어 버퍼에 로드
      │   └─ m_MapSound.insert({pSoundKey, pSound})
      └─ FMOD_System_Update(m_pSystem)            ← 로드 완료 후 갱신
```

### 3.2 매 프레임 갱신

```
CMainApp::Update_MainApp(fTimeDelta)
  └─ CSoundMgr::UpdateSound()
      └─ FMOD_System_Update(m_pSystem)
         (FMOD 내부 스트리밍, 3D 계산, 가상 채널 관리 등 수행)
```

매 프레임 `FMOD_System_Update` 한 번만 호출 — 이것이 유일한 프레임별 사운드 처리.

### 3.3 일반 사운드 재생 (SFX)

```
[플레이어 공격 시]
CPlayer_Mage::Update_GameObject(fTimeDelta)
  └─ CSoundMgr::GetInstance()->Play_Sound(
         L"MagicStaff_Charging.ogg",    ← 사운드 키 (파일명)
         Engine::CHANNEL_MAGE_SKILL,    ← 채널
         0.7f)                          ← 볼륨

CSoundMgr::Play_Sound(pSoundKey, eChannel, fVolume)
  ├─ find_if(m_MapSound, CTag_Finder(pSoundKey))
  │   └─ CTag_Finder::operator(): lstrcmp(m_pTag, Pair.first) == 0
  │      (맵 순회하며 파일명 문자열 비교)
  │
  ├─ FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_FREE, pSound, 0, &m_pChannel[eChannel])
  │   ├─ FMOD_CHANNEL_FREE: 빈 채널 자동 선택
  │   ├─ pSound: 캐시된 사운드 핸들
  │   ├─ 0: 일시정지 없이 즉시 재생
  │   └─ &m_pChannel[eChannel]: 채널 핸들 저장
  │
  └─ FMOD_Channel_SetVolume(m_pChannel[eChannel], fVolume)
```

### 3.4 BGM 재생 (루프)

```
[씬 진입 시]
CScene_Menu::Ready_Scene()
  └─ CSoundMgr::GetInstance()->PlayBGM(L"Menu.mp3", CHANNEL_MENU, 0.5f)

CScene_Stage::Ready_Scene()
  ├─ CSoundMgr::GetInstance()->StopSound(CHANNEL_MENU)     ← 이전 BGM 정지
  └─ CSoundMgr::GetInstance()->PlayBGM(L"DungeonDefense_MainTheme.ogg",
                                       CHANNEL_STAGE, 0.3f)

CSoundMgr::PlayBGM(pSoundKey, eChannel, fVolume)
  ├─ find_if(m_MapSound, CTag_Finder(pSoundKey))
  ├─ FMOD_Sound_SetMode(pSound, FMOD_LOOP_NORMAL)           ← 루프 모드 설정
  ├─ FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_FREE, pSound, 0, &m_pChannel[eChannel])
  └─ FMOD_Channel_SetVolume(m_pChannel[eChannel], fVolume)
```

### 3.5 거리 기반 3D 사운드

```
CSoundMgr::Play_Sound(pSoundKey, eChannel, vecCamEye, vecSoundPos, fRange, iSoundNum, bPlayOnce)
  │
  ├─ if (bPlayOnce && IsPlaying(eChannel))      ← 이미 재생 중이면 무시
  │   return;
  │
  ├─ fDistance = D3DXVec3Length(&(vecCamEye - vecSoundPos))
  │   → 카메라와 사운드 원점 간 거리 계산
  │
  ├─ if (fDistance > fRange)                      ← 범위 밖이면 재생 안 함
  │   return;
  │
  └─ if (iSoundNum != 1)                         ← 다수 변형
  │   Play_RandomSound(pSoundKey, eChannel, iSoundNum)
  └─ else
      Play_Sound(pSoundKey, eChannel)             ← 단일 사운드
```

**주의**: 거리에 따른 볼륨 감쇠는 구현되지 않음 — 범위 안/밖 이진 판정만 수행.

### 3.6 랜덤 사운드 재생

```
[몬스터 공격 시]
CMonster_Goblin::Update_GameObject()
  └─ CSoundMgr::GetInstance()->Play_RandomSound(
         L"Goblin_attack.ogg",      ← 베이스 파일명
         CHANNEL_MONSTER_ATTACK,
         5,                          ← 변형 수 (1~5)
         0.5f)

CSoundMgr::Play_RandomSound(pSoundKey, eChannel, iNum, fVolume)
  ├─ lstrcpy(szTemp, pSoundKey)                  → "Goblin_attack.ogg"
  ├─ lstrcpyn(szTemp, szTemp, lstrlen(szTemp)-3) → "Goblin_attac"  (뒤 4자 제거)
  ├─ wsprintf(szTemp, "%s%d%s", szTemp, rand()%iNum+1, ".ogg")
  │   → "Goblin_attac3.ogg" (랜덤 번호 삽입)
  ├─ Play_Sound(szTemp, eChannel)
  └─ FMOD_Channel_SetVolume(m_pChannel[eChannel], fVolume)
```

**파일 명명 규칙**: `BaseName` + `숫자` + `.ogg` → `Goblin_attac1.ogg` ~ `Goblin_attac5.ogg`

### 3.7 중복 재생 방지 패턴

```
[Client에서 자주 사용되는 패턴]
if (FALSE == CSoundMgr::GetInstance()->IsPlaying(CHANNEL_MAGE_SKILL))
    CSoundMgr::GetInstance()->Play_Sound(L"MagicStaff_Charging.ogg",
                                         CHANNEL_MAGE_SKILL, 0.7f);

// 여러 채널 동시 체크
if (FALSE == CSoundMgr::GetInstance()->IsPlaying(CHANNEL_MAGE_SKILL2) &&
    FALSE == CSoundMgr::GetInstance()->IsPlaying(CHANNEL_MAGE_SKILL))
    CSoundMgr::GetInstance()->Play_Sound(...);
```

### 3.8 볼륨 동적 조절 (BGM 덕킹)

```
[웨이브 시작 시 — BGM 볼륨 줄이고 효과음 강조]
CScene_Stage::Update_Scene()
  └─ CSoundMgr::GetInstance()->Set_BGMVolume(CHANNEL_STAGE, 0.1f)  ← 30% → 10%
     if (!IsPlaying(CHANNEL_WAVE))
         CSoundMgr::GetInstance()->Play_Sound(L"DD_Wave1Beat.ogg",
                                              CHANNEL_WAVE, 0.8f)

[웨이브 종료 시]
  └─ CSoundMgr::GetInstance()->Set_BGMVolume(CHANNEL_STAGE, 0.3f)  ← 10% → 30% 복원
```

---

## 4. 사용된 디자인 패턴

### 4.1 싱글톤

```cpp
class ENGINE_DLL CSoundMgr : public CBase
{
    DECLARE_SINGLETON(CSoundMgr)
    // → static GetInstance(), DestroyInstance()
};

// Client에서 호출:
CSoundMgr::GetInstance()->Play_Sound(...)
// 모든 오브젝트가 직접 싱글톤에 접근 — 파사드/래퍼 없이 직접 호출
```

### 4.2 Functor 패턴 (CTag_Finder)

```cpp
// Engine_Functor.h
class CTag_Finder {
    const TCHAR* m_pTag;
public:
    explicit CTag_Finder(const TCHAR* pTag) : m_pTag(pTag) {}
    template <typename TC> bool operator () (TC& Pair) {
        return (0 == lstrcmp(m_pTag, Pair.first));
    }
};

// 사용:
find_if(m_MapSound.begin(), m_MapSound.end(), CTag_Finder(pSoundKey));
// → map을 선형 순회하며 문자열 비교
```

**주의**: `std::map`이지만 `find()`가 아닌 `find_if()`를 사용 — 키가 `TCHAR*` 포인터이므로 `map::find()`는 포인터 주소를 비교하게 됨. `CTag_Finder`는 `lstrcmp`로 문자열 내용을 비교.

### 4.3 채널 풀 (정적 배열)

```cpp
enum CHANNEL_TYPE {
    CHANNEL_LOGO,
    CHANNEL_MENU, CHANNEL_GAMESTART,
    CHANNEL_WAVE,
    CHANNEL_STAGE, CHANNEL_CRYSTAL_ACTIVATE, CHANNEL_GATE_SLAMLARGE,
    CHANNEL_PRESENT, CHANNEL_TRESUREBOX,
    CHANNEL_MAGE, CHANNEL_MONK, CHANNEL_GOLBIN, CHANNEL_DARKELF,
    CHANNEL_ORC, CHANNEL_KOBOLD, CHANNEL_DEMON,
    CHANNEL_MONSTER_ATTACK, CHANNEL_MONSTER_ATTACK2, CHANNEL_MONSTER_ATTACK3,
    CHANNEL_NORMAL_ATTACK, CHANNEL_MAGE_SKILL, CHANNEL_MAGE_SKILL2,
    CHANNEL_TOWER, CHANNEL_TOWER_2, CHANNEL_TOWER_3,
    CHANNEL_TOWER_ATTACK, CHANNEL_TOWER_ATTACK_2,
    CHANNEL_STAGE2,
    CHANNEL_END  // = 26
};

FMOD_CHANNEL* m_pChannel[CHANNEL_END];
// → 26개 고정 채널, enum으로 용도별 할당
```

**설계 의도**: 동시에 재생 가능한 사운드 수를 제한하고, 같은 채널에 새 사운드를 재생하면 이전 사운드가 자동으로 중단됨.

### 4.4 폴더 일괄 스캔 로딩

```
LoadSoundFile():
  ../Resources/Sound/ 폴더의 모든 파일을 _findfirst/_findnext로 스캔
  → 파일명을 TCHAR* 키로, FMOD_SOUND*를 값으로 맵에 저장
  → 개별 등록 불필요, 폴더에 넣기만 하면 자동 로드
```

---

## 5. DirectX API 호출 지점과 래핑 방식

### FMOD API 사용 (DirectX가 아닌 서드파티)

이 사운드 시스템은 **FMOD**를 사용하며, DirectX API는 호출하지 않는다. 단, 3D 사운드의 거리 계산에 D3DX 수학 함수를 사용한다.

#### FMOD C API 호출 목록

| FMOD API | 사용처 | 용도 |
|----------|--------|------|
| `FMOD_System_Create` | `Initialize` | FMOD 시스템 객체 생성 |
| `FMOD_System_Init` | `Initialize` | 채널 수 설정, 초기화 |
| `FMOD_System_CreateSound` | `LoadSoundFile` | 파일 → FMOD_SOUND 로드 |
| `FMOD_System_PlaySound` | `Play_Sound`, `PlayBGM` | 사운드 재생 |
| `FMOD_System_Update` | `UpdateSound`, `LoadSoundFile` | 내부 상태 갱신 |
| `FMOD_System_Close` | `Free` | 시스템 종료 |
| `FMOD_System_Release` | `Free` | 시스템 메모리 해제 |
| `FMOD_Sound_SetMode` | `PlayBGM` | 루프 모드 설정 |
| `FMOD_Sound_Release` | `Free` | 사운드 메모리 해제 |
| `FMOD_Channel_SetVolume` | `Play_Sound`, `Set_BGMVolume` | 채널 볼륨 |
| `FMOD_Channel_Stop` | `StopSound` | 채널 정지 |
| `FMOD_Channel_IsPlaying` | `IsPlaying` | 재생 여부 조회 |

#### D3DX 수학 함수 사용

| D3DX API | 사용처 | 용도 |
|----------|--------|------|
| `D3DXVec3Length` | `Play_Sound` (3D 오버로드) | 카메라-사운드 거리 계산 |

### 래핑 방식

```
FMOD C API를 직접 호출 (FMOD C++ API 미사용)
  → FMOD_System_*(), FMOD_Sound_*(), FMOD_Channel_*()
  → fmod.h 포함, fmod.hpp는 포함하지만 C++ 인터페이스는 사용 안 함

래핑 수준: 단순 위임
  CSoundMgr::Play_Sound() → FMOD_System_PlaySound() + FMOD_Channel_SetVolume()
  CSoundMgr::StopSound()  → FMOD_Channel_Stop()
  CSoundMgr::IsPlaying()  → FMOD_Channel_IsPlaying()
```

---

## 6. 채널 할당 설계 분석

### 채널 분류

```
[BGM 채널]
CHANNEL_LOGO          ← 로고 BGM
CHANNEL_MENU          ← 메뉴 BGM
CHANNEL_STAGE         ← 스테이지 BGM
CHANNEL_STAGE2        ← 스테이지2(라스트맨) BGM

[이벤트 채널]
CHANNEL_GAMESTART     ← 게임 시작 효과음
CHANNEL_WAVE          ← 웨이브 알림음

[환경 채널]
CHANNEL_CRYSTAL_ACTIVATE  ← 크리스탈 활성화
CHANNEL_GATE_SLAMLARGE    ← 문 닫힘
CHANNEL_PRESENT           ← 선물 상자
CHANNEL_TRESUREBOX        ← 보물 상자

[캐릭터 채널]
CHANNEL_MAGE          ← 마법사 기본
CHANNEL_MAGE_SKILL    ← 마법사 스킬 1
CHANNEL_MAGE_SKILL2   ← 마법사 스킬 2
CHANNEL_MONK          ← 수도승
CHANNEL_NORMAL_ATTACK ← 일반 공격

[몬스터 채널]
CHANNEL_GOLBIN        ← 고블린
CHANNEL_DARKELF       ← 다크엘프
CHANNEL_ORC           ← 오크
CHANNEL_KOBOLD        ← 코볼트
CHANNEL_DEMON         ← 데몬
CHANNEL_MONSTER_ATTACK  ← 몬스터 공격 1
CHANNEL_MONSTER_ATTACK2 ← 몬스터 공격 2
CHANNEL_MONSTER_ATTACK3 ← 몬스터 공격 3

[타워 채널]
CHANNEL_TOWER         ← 타워 1
CHANNEL_TOWER_2       ← 타워 2
CHANNEL_TOWER_3       ← 타워 3
CHANNEL_TOWER_ATTACK  ← 타워 공격 1
CHANNEL_TOWER_ATTACK_2 ← 타워 공격 2
```

### 설계 특징

- **엔티티 종류별 전용 채널**: 같은 몬스터가 여러 마리여도 채널 1개 → 동시에 같은 사운드 1개만
- **다중 공격 채널**: `MONSTER_ATTACK` 1~3으로 동시 공격음 3개까지 허용
- **스킬 채널 분리**: `MAGE_SKILL` + `MAGE_SKILL2`로 시전+발동 동시 재생
- **채널 재사용**: 같은 채널에 새 사운드 재생 시 이전 것은 자동 중단

---

## 7. 프레임워크 참고 설계 판단

### 7.1 채택할 만한 설계

**폴더 스캔 일괄 로드**
```cpp
_findfirst("../Resources/Sound/*.*") → 모든 파일 자동 로드
```
- 사운드 추가 시 폴더에 파일만 넣으면 끝 — 코드 변경 불필요
- 파일명 자체가 키 → 별도 ID 관리 불필요
- 단순하고 실수가 적은 방식

**채널 enum 기반 정적 할당**
- 채널을 `CHANNEL_TYPE` enum으로 정의 → 컴파일 타임에 채널 수 확정
- 같은 채널에 새 사운드 = 이전 사운드 자동 교체 → 자연스러운 중복 방지
- `IsPlaying(eChannel)`로 재생 상태 즉시 조회 가능

**BGM 덕킹 패턴**
```
웨이브 시작 → Set_BGMVolume(STAGE, 0.1f)  // BGM 줄이기
웨이브 종료 → Set_BGMVolume(STAGE, 0.3f)  // BGM 복원
```
- 간단하지만 효과적인 오디오 믹싱 — 중요 이벤트 시 BGM을 낮춤

**IsPlaying 기반 중복 방지**
```cpp
if (FALSE == IsPlaying(CHANNEL_MAGE_SKILL))
    Play_Sound(L"Charging.ogg", CHANNEL_MAGE_SKILL, 0.7f);
```
- Client가 재생 전 상태 확인 → 같은 사운드 겹치기 방지
- 채널 단위로 조회하므로 O(1)

**랜덤 변형 사운드**
```
Play_RandomSound(L"Goblin_attack.ogg", CHANNEL, 5)
  → Goblin_attac1.ogg ~ Goblin_attac5.ogg 중 랜덤
```
- 파일명 규칙만으로 변형 → 추가 데이터 구조 불필요
- 반복 재생 시 단조로움 방지

### 7.2 개선 여지가 있는 부분

**TCHAR* 포인터를 map 키로 사용 + find_if 선형 탐색**
```cpp
map<TCHAR*, FMOD_SOUND*>  m_MapSound;

// 검색 시:
find_if(m_MapSound.begin(), m_MapSound.end(), CTag_Finder(pSoundKey));
// → O(n) 선형 탐색 (map의 O(log n) 이점을 못 살림)
```
- `map::find()`는 키의 `<` 연산자를 사용 → `TCHAR*`는 포인터 주소 비교
- `find_if` + `CTag_Finder`로 문자열 비교하면 O(n)
- **개선안**: `std::wstring`을 키로 사용하면 `map::find()`가 O(log n)

**3D 사운드가 진정한 공간 음향이 아님**
```cpp
float fDistance = D3DXVec3Length(&(vecCamEye - vecSoundPos));
if (fDistance > fRange) return;
// → 범위 안이면 동일 볼륨, 범위 밖이면 무음 (거리 감쇠 없음)
```
- FMOD의 3D 사운드 기능(`FMOD_3D`, `FMOD_3D_Listener`, `FMOD_3D_Attributes`) 미사용
- **개선안**: `FMOD_Channel_Set3DAttributes` + `FMOD_System_Set3DListenerAttributes`로 진정한 공간 음향 구현

**랜덤 사운드 파일명 조작이 취약**
```cpp
lstrcpyn(szTemp, szTemp, lstrlen(szTemp) - 3); // 뒤 4자 제거 ("0.ogg")
wsprintf(szTemp, L"%s%d%s", szTemp, rand() % iNum + 1, L".ogg");
```
- 파일명 길이에 의존 — `0.ogg` 부분이 정확히 4자여야 함
- `.wav` 확장자나 2자리 숫자(`12.ogg`) 파일은 오작동
- **개선안**: 확장자 분리 + 번호 체계를 명확히 (ex: `_01`, `_02` 패딩)

**모든 사운드를 시작 시 일괄 로드**
```
LoadSoundFile() → ../Resources/Sound/ 전체 스캔
→ 모든 씬의 모든 사운드가 메모리에 상주
```
- 소규모 프로젝트에서는 문제없지만, 대규모에서는 메모리 낭비
- **개선안**: 씬별 로드/언로드 또는 스트리밍 사운드 활용

**CHANNEL_END가 고정 26개**
```
FMOD_System_Init(m_pSystem, CHANNEL_END, ...)
// → 동시 재생 최대 26개
```
- 몬스터가 수십 마리 동시에 소리 내야 할 때 채널 부족 가능
- **개선안**: FMOD 가상 채널 활용 (MAX_CHANNELS를 넉넉히 설정, 우선순위 기반 관리)

**채널 배열에 마지막 재생 핸들만 저장**
```cpp
FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_FREE, pSound, 0, &m_pChannel[eChannel]);
// → FMOD_CHANNEL_FREE로 빈 채널을 받아오지만, m_pChannel[eChannel]에 덮어씀
// → 이전 채널 핸들 유실 (이전 사운드가 계속 재생될 수 있음)
```
- `FMOD_CHANNEL_FREE`는 **FMOD가 내부에서 빈 채널을 선택**하는 것
- `m_pChannel[eChannel]`에 새 핸들을 저장하면 이전 핸들 참조 불가
- 실질적으로 채널당 1개 사운드만 제어 가능

### 7.3 내 프레임워크(DX9) 적용 시 고려사항

| 참고프로젝트3 | 내 프레임워크 | 적용 방향 |
|--------------|--------------|-----------|
| FMOD C API | (미선택) | FMOD / DirectSound / XAudio2 중 선택 |
| 폴더 일괄 스캔 | 미구현 | 폴더 스캔 도입 — 간편한 리소스 관리 |
| CHANNEL_TYPE enum (26개) | 미구현 | 채널 그룹 + 우선순위 시스템으로 확장 |
| 2D 볼륨만 (범위 체크) | 미구현 | FMOD 3D 기능 활용하여 진정한 공간 음향 |
| `TCHAR*` 키 + `find_if` | 미구현 | `wstring` 키 + `unordered_map` 도입 |
| 일괄 로드 | 미구현 | 씬별 로드/언로드 + 스트리밍 BGM |
| BGM 덕킹 (수동) | 미구현 | FMOD ChannelGroup으로 BGM/SFX/UI 그룹별 볼륨 |

### 7.4 권장 아키텍처

```
[개선된 사운드 시스템 구조안]

CSoundManager (싱글톤)
  ├─ FMOD::System*
  ├─ FMOD::ChannelGroup* m_pGroupBGM
  ├─ FMOD::ChannelGroup* m_pGroupSFX
  ├─ FMOD::ChannelGroup* m_pGroupUI
  │
  ├─ unordered_map<wstring, FMOD::Sound*>  m_mapSounds  ← wstring 키
  │
  ├─ Play_SFX(key, pos, volume)     ← 3D 공간 음향
  ├─ Play_BGM(key, volume, fadeTime) ← 크로스페이드 BGM
  ├─ Set_GroupVolume(group, volume)  ← 그룹별 볼륨
  │
  ├─ Load_SoundPack(sceneName)       ← 씬별 로드
  └─ Unload_SoundPack(sceneName)     ← 씬별 언로드
```
