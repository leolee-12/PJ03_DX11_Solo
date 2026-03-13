# 참고프로젝트2 — 사운드 시스템 심화 분석

> 분석 범위: `CSound_Manager`, `CAnimation::SoundEvent`, `CGameInstance`(사운드 파사드), `CFrameEvent_Manager`(사운드 트리거)
> 분석 대상: 헤더 + cpp 구현부

---

## 1. 시스템 핵심 책임과 경계

### 책임 분리

| 계층 | 클래스 | 위치 | 책임 |
|------|--------|------|------|
| **사운드 엔진** | `CSound_Manager` | Engine | FMOD 초기화, 사운드 등록/재생/정지, 볼륨 관리, 채널 관리 |
| **파사드 중계** | `CGameInstance` | Engine | 사운드 API를 Client에 노출 (1:1 위임) |
| **사운드 트리거** | `CFrameEvent_Manager` | Client | 애니메이션 키프레임에서 사운드 재생 호출 |
| **사운드 이벤트 데이터** | `CAnimation::SoundEvent` | Engine | 애니메이션에 사운드 트리거 시점 저장 (구조체) |

### 경계

```
┌─────────────────────────────────────────────────────┐
│  Client                                              │
│  CFrameEvent_Manager → Character::Play_Sound()       │
│  Level_GamePlay::Ready → Register_Sound()            │
│    ↓ 파사드 경유                                      │
├─────────────────────────────────────────────────────┤
│  Engine (CGameInstance 파사드)                        │
│    ↓ 위임                                            │
│  CSound_Manager                                      │
│    ↓ FMOD C API                                      │
│  FMOD (외부 라이브러리)                               │
└─────────────────────────────────────────────────────┘
```

- Engine은 **"이 사운드를 등록/재생/정지하라"** 까지 담당
- **"언제, 어떤 사운드를 재생할지"** 결정은 Client의 책임
- `CSound_Manager`는 FMOD를 완전히 래핑하여 Client가 FMOD API를 직접 호출하지 않음
- 특이점: `CSound_Manager`는 `CBase`가 아니라 **`CGameObject`를 상속** (Update 루프에 참여하기 위해)

---

## 2. 클래스 간 소유/참조 관계

```
CGameInstance (싱글톤)
  │ [소유] CSound_Manager* m_pSoundManager
  │          ├── FMOD_SYSTEM*      m_pSoundSystem    (FMOD 시스템)
  │          ├── FMOD_CHANNELGROUP* m_pChannelGroup   (채널 그룹)
  │          │
  │          ├── unordered_map<SOUND_KEY_NAME, FMOD_SOUND*>     m_soundMap     (개별 사운드)
  │          ├── unordered_map<SOUND_GROUP_KEY_NAME, FMOD_SOUND*> m_groupSoundMap (그룹 사운드)
  │          │
  │          ├── unordered_map<SOUND_KEY_NAME, ChannelInfo>    m_channelMap     (재생 채널)
  │          ├── unordered_map<SOUND_GROUP_KEY_NAME, ChannelInfo> m_groupChannelMap
  │          │
  │          ├── unordered_map<SOUND_KEY_NAME, SOUND_CATEGORY>  m_soundCategoryMap
  │          ├── unordered_map<SOUND_CATEGORY, float>           m_categoryVolumes
  │          │
  │          ├── unordered_map<SOUND_GROUP_KEY, vector<SOUND_GROUP_KEY_NAME>> m_soundGroupMap
  │          └── unordered_map<SOUND_GROUP_KEY, SOUND_GROUP_KEY_NAME>        m_lastPlayedSound
  │
  ↓ (Client에서 참조)
Level_GamePlay / Level_CharaSelect
  └── Register_Sound() / Register_Sound_Group() 호출

CFrameEvent_Manager
  └── Character::Play_Sound() / Play_Group_Sound() 호출

CAnimation
  └── vector<SoundEvent> m_SoundEvents (데이터만 보유, 직접 재생하지 않음)
```

### 생성 체인

```
CGameInstance::Initialize_Engine()
  → CSound_Manager::Create(pDevice, pContext)
    → CSound_Manager 생성자 (CGameObject 상속)
    → Initialize_Prototype()
      → FMOD_System_Create()
      → FMOD_System_Init(MAX_CHANNELS=64, FMOD_INIT_NORMAL | FMOD_INIT_STREAM_FROM_UPDATE)
      → FMOD_System_CreateChannelGroup()
      → 카테고리 볼륨 초기화 (BGM/VOICE/SFX = 1.0f)
```

### 해제 체인

```
CGameInstance::Release_Engine()
  → Safe_Release(m_pSoundManager)
    → CSound_Manager::Free()
      → FMOD_Sound_Release() × N (모든 개별 사운드)
      → FMOD_Sound_Release() × N (모든 그룹 사운드)
      → FMOD_System_Close()
      → FMOD_System_Release()
      → __super::Free() (CGameObject::Free)
```

---

## 3. 주요 함수의 호출 흐름 (한 프레임 기준)

### 3.1 사운드 등록 흐름 (레벨 초기화 시 1회)

```
Level_GamePlay::Ready_Scene()
  │
  ├─ Register_Sound(파일경로, SOUND_KEY_NAME, CATEGORY, loop, nonOverlapping)
  │   → CGameInstance::Register_Sound()
  │     → CSound_Manager::Register_Sound()
  │       ├─ 중복 체크 (m_soundMap.find)
  │       ├─ wstring → string 변환 (codecvt)
  │       ├─ FMOD_MODE 결정 (loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF)
  │       ├─ FMOD_System_CreateSound() → FMOD_SOUND* 생성
  │       ├─ m_soundMap[alias] = sound
  │       ├─ m_soundCategoryMap[alias] = category
  │       └─ nonOverlapping이면 m_nonOverlappingSounds.insert(alias)
  │
  └─ Register_Sound_Group(GROUP_KEY, 파일경로, GROUP_KEY_NAME, CATEGORY, loop)
      → CSound_Manager::Register_Sound_Group()
        ├─ 그룹 없으면 m_soundGroupMap[groupKey] = {} 생성
        ├─ FMOD_System_CreateSound() → FMOD_SOUND* 생성
        ├─ m_groupSoundMap[alias] = sound
        └─ m_soundGroupMap[groupKey].push_back(alias)
```

### 3.2 매 프레임 업데이트

```
CGameInstance::Update_Engine(fTimeDelta)
  └─ m_pSoundManager->Update(fTimeDelta)
      ├─ FMOD_System_Update()                    ← FMOD 내부 상태 갱신
      └─ 완료된 채널 정리
          ├─ FMOD_ChannelGroup_GetNumChannels()
          └─ 각 채널: IsPlaying==false → FMOD_Channel_Stop()
```

### 3.3 사운드 재생 흐름

```
[직접 호출 경로]
CPlay_Goku::Attack_Light()
  → m_pGameInstance->Play_Sound(Goku_Heavy_Attack, false, 1.0f)
  → m_pGameInstance->Play_Group_Sound(LIGHT_ATTACK_Goku, false, 1.0f)

[프레임 이벤트 경로]
CCharacter::ProcessEventsBetweenFrames2()
  → CFrameEvent_Manager::ProcessEvent()
    → pCharacter->Play_Sound(SoundName, loop, volume)
      → m_pGameInstance->Play_Sound(...)
```

**Play_Sound 내부 흐름**:
```
CSound_Manager::Play_Sound(alias, loop, volume)
  ├─ m_isImguiPlay 체크 (에디터 음소거)
  ├─ m_soundMap에서 FMOD_SOUND* 조회
  ├─ nonOverlapping 체크 → 이미 재생 중이면 return
  ├─ 채널 수 확인 → MAX_CHANNELS(64) 초과 시 가장 오래된 채널 Stop
  ├─ 카테고리 볼륨 조회 → actualVolume = volume × categoryVolume
  ├─ FMOD_System_PlaySound() → FMOD_CHANNEL* 획득
  ├─ FMOD_Channel_SetVolume(actualVolume)
  └─ m_channelMap[alias] = { channel, volume }  (baseVolume 보존)
```

**Play_Group_Sound 내부 흐름**:
```
CSound_Manager::Play_Group_Sound(groupKey, loop, volume)
  ├─ m_soundGroupMap에서 그룹의 사운드 목록 조회
  ├─ 마지막 재생된 사운드 제외한 후보 목록 구성
  ├─ rand() % 후보 수 → 랜덤 선택 (연속 중복 방지)
  ├─ FMOD_System_PlaySound() → 재생
  ├─ FMOD_Channel_SetVolume(volume × categoryVolume)
  └─ m_lastPlayedSound[groupKey] = selectedSoundKey
```

---

## 4. 사용된 디자인 패턴

### 4.1 파사드 패턴 (Facade)

`CGameInstance`가 `CSound_Manager`의 모든 공개 메서드를 1:1 위임한다.

```cpp
void CGameInstance::Play_Sound(CSound_Manager::SOUND_KEY_NAME alias, _bool loop, _float volume) {
    m_pSoundManager->Play_Sound(alias, loop, volume);
}
```

Client는 `CSound_Manager`를 직접 참조하지 않고 `CGameInstance`를 통해서만 접근한다.

### 4.2 키-값 저장소 패턴

사운드를 enum 키로 등록하고 조회하는 Dictionary 패턴:

```cpp
unordered_map<SOUND_KEY_NAME, FMOD_SOUND*>   m_soundMap;      // 등록
unordered_map<SOUND_KEY_NAME, ChannelInfo>    m_channelMap;    // 재생 상태
unordered_map<SOUND_KEY_NAME, SOUND_CATEGORY> m_soundCategoryMap; // 카테고리 분류
```

### 4.3 그룹 랜덤 재생 패턴

동일 유형의 사운드를 그룹으로 묶고, 재생 시 **연속 중복을 방지**하며 랜덤 선택:

```cpp
// 마지막에 재생된 사운드를 제외한 후보 구성
vector<SOUND_GROUP_KEY_NAME> availableSounds;
for (const auto& soundAlias : soundList) {
    if (m_lastPlayedSound[groupKey] != soundAlias)
        availableSounds.push_back(soundAlias);
}
// 랜덤 선택
int randomIndex = rand() % availableSounds.size();
```

용도: 약공격 보이스가 2~3가지인 경우, 같은 사운드가 연속으로 나오지 않게 한다.

### 4.4 카테고리 볼륨 믹싱 패턴

```
실제 볼륨 = baseVolume × categoryVolume
```

카테고리별(BGM/VOICE/SFX) 마스터 볼륨을 분리하여, 옵션 UI에서 카테고리별 볼륨 조절이 가능하다. 카테고리 볼륨 변경 시 해당 카테고리의 **모든 활성 채널**을 순회하며 갱신:

```cpp
void Set_Category_Volume(SOUND_CATEGORY category, float volume) {
    m_categoryVolumes[category] = volume;
    // 개별 사운드 채널 갱신
    for (const auto& pair : m_soundCategoryMap) {
        if (pair.second == category) {
            float newVolume = channelIt->second.baseVolume * volume;
            FMOD_Channel_SetVolume(channelIt->second.channel, newVolume);
        }
    }
    // 그룹 사운드 채널도 동일하게 갱신
}
```

### 4.5 프레임 이벤트 기반 트리거

`CFrameEvent_Manager`가 애니메이션 진행도를 감시하다가 특정 프레임에 도달하면 사운드를 재생한다:

```
ProcessEventsBetweenFrames2(charIndex, animIndex, prevPos, curPos)
  → 이벤트 타입이 SOUND → Character::Play_Sound() 호출
  → 이벤트 타입이 GROUP_SOUND → Character::Play_Group_Sound() 호출
```

---

## 5. FMOD API 호출 지점과 래핑 방식

이 프로젝트는 **FMOD C API**를 사용한다 (C++ API가 아닌 순수 C 인터페이스).

### 5.1 초기화 (Initialize_Prototype)

| FMOD API | 용도 |
|----------|------|
| `FMOD_System_Create(&m_pSoundSystem, FMOD_VERSION)` | 시스템 객체 생성 |
| `FMOD_System_Init(m_pSoundSystem, 64, FMOD_INIT_NORMAL \| FMOD_INIT_STREAM_FROM_UPDATE, nullptr)` | 64채널, 스트리밍 모드 |
| `FMOD_System_CreateChannelGroup(m_pSoundSystem, nullptr, &m_pChannelGroup)` | 마스터 채널 그룹 |

### 5.2 사운드 로딩 (Register_Sound)

| FMOD API | 용도 |
|----------|------|
| `FMOD_System_CreateSound(m_pSoundSystem, path, FMOD_DEFAULT \| mode, nullptr, &sound)` | 사운드 리소스 생성 |

`FMOD_MODE` 조합:
- 루프 사운드: `FMOD_LOOP_NORMAL | FMOD_CREATESAMPLE`
- 단발 사운드: `FMOD_LOOP_OFF | FMOD_CREATESAMPLE`

`FMOD_CREATESAMPLE` — 메모리에 완전히 디코딩하여 로드 (저지연, 메모리 사용 높음).

### 5.3 재생/정지 (Play_Sound / Stop_Sound)

| FMOD API | 용도 |
|----------|------|
| `FMOD_System_PlaySound(m_pSoundSystem, sound, m_pChannelGroup, false, &channel)` | 사운드 재생 → 채널 반환 |
| `FMOD_Channel_SetVolume(channel, actualVolume)` | 채널 볼륨 설정 |
| `FMOD_Channel_Stop(channel)` | 채널 정지 |
| `FMOD_Channel_IsPlaying(channel, &isPlaying)` | 재생 상태 확인 |

### 5.4 매 프레임 갱신 (Update)

| FMOD API | 용도 |
|----------|------|
| `FMOD_System_Update(m_pSoundSystem)` | FMOD 내부 상태 갱신 (필수) |
| `FMOD_ChannelGroup_GetNumChannels(m_pChannelGroup, &count)` | 활성 채널 수 조회 |
| `FMOD_ChannelGroup_GetChannel(m_pChannelGroup, i, &channel)` | 인덱스로 채널 접근 |

### 5.5 해제 (Free)

| FMOD API | 용도 |
|----------|------|
| `FMOD_Sound_Release(sound)` | 사운드 리소스 해제 |
| `FMOD_System_Close(m_pSoundSystem)` | 시스템 닫기 |
| `FMOD_System_Release(m_pSoundSystem)` | 시스템 해제 |

---

## 6. 사운드 식별 체계

### 6.1 세 종류의 enum

```cpp
// 1. 개별 사운드 키 (약 150개) — 고유한 사운드 1:1 매핑
enum class SOUND_KEY_NAME : _int {
    SPACE_BGM = 0,
    Goku_Heavy_Attack,
    ...
};

// 2. 그룹 사운드 개별 키 (약 50개) — 그룹 내 각 변형에 매핑
enum class SOUND_GROUP_KEY_NAME : _int {
    Light_Attack_Goku_1 = 100,   // 시작값 100으로 구분
    Light_Attack_Goku_2,
    ...
};

// 3. 그룹 키 (약 11개) — 여러 변형을 하나로 묶음
enum class SOUND_GROUP_KEY : _int {
    LIGHT_ATTACK_Goku = 200,     // 시작값 200으로 구분
    Hit_Goku,
    ...
};
```

### 6.2 그룹 관계도

```
SOUND_GROUP_KEY::LIGHT_ATTACK_Goku (그룹)
  ├── SOUND_GROUP_KEY_NAME::Light_Attack_Goku_1 → "Light_Attack_1.ogg"
  └── SOUND_GROUP_KEY_NAME::Light_Attack_Goku_2 → "Light_Attack_2.ogg"

재생 시: Play_Group_Sound(LIGHT_ATTACK_Goku) → 둘 중 하나 랜덤 선택
```

### 6.3 카테고리 분류

```cpp
enum class SOUND_CATEGORY { BGM, VOICE, SFX };
```

각 사운드 등록 시 카테고리 지정 → 옵션 UI에서 카테고리별 볼륨 조절 가능.

---

## 7. 특이 설계: CGameObject 상속

`CSound_Manager`가 **`CBase`가 아닌 `CGameObject`를 상속**한다:

```cpp
class CSound_Manager final : public CGameObject { ... };
```

이유:
- `Update(_float fTimeDelta)` 가상 함수를 활용하여 매 프레임 `FMOD_System_Update()` 호출
- `CGameInstance::Update_Engine()`에서 직접 `m_pSoundManager->Update(dt)` 호출

단점:
- `CGameObject`의 컴포넌트 맵, Transform, Device/Context 등 불필요한 멤버를 모두 상속
- `Clone()`이 `return this;`로 구현 — 프로토타입 패턴의 의미가 없음
- `Camera_Update`, `Late_Update`, `Render`가 빈 함수

---

## 8. 비중첩 재생 (NonOverlapping)

특정 사운드는 **동시에 여러 번 재생되면 안 되는** 경우가 있다 (예: BGM, 장시간 보이스):

```cpp
// 등록 시 비중첩 플래그 설정
Register_Sound(path, alias, category, loop, isNonOverlapping=true);

// 재생 시 체크
if (isNonOverlapping) {
    auto channelIt = m_channelMap.find(alias);
    if (channelIt != end) {
        FMOD_BOOL isPlaying = false;
        FMOD_Channel_IsPlaying(channelIt->second.channel, &isPlaying);
        if (isPlaying) return;  // 이미 재생 중이면 무시
    }
}
```

---

## 9. 프레임워크 참고 설계 판단

### 9.1 채택할 만한 설계

**카테고리별 볼륨 믹싱**
- `actualVolume = baseVolume × categoryVolume` 공식으로 개별 볼륨과 마스터 볼륨을 분리
- 옵션 UI에서 BGM/SFX/VOICE 별도 조절이 자연스럽게 가능

**그룹 랜덤 재생 + 연속 중복 방지**
- 동일 행동의 사운드 변형을 그룹으로 묶어 단조로움 방지
- `m_lastPlayedSound`로 직전 사운드를 제외하는 심플한 방식

**비중첩 플래그**
- BGM이나 환경음 같은 사운드가 중복 재생되는 것을 명시적으로 방지
- 등록 시점에 속성으로 지정하여 재생 로직을 깔끔하게 유지

**채널 수 제한 + 오래된 채널 교체**
- MAX_CHANNELS(64) 초과 시 가장 오래된 채널을 중단하고 새 사운드 재생
- 사운드 폭주 시에도 시스템이 안정적으로 동작

**FMOD C API 사용**
- C++ API보다 가볍고, DLL 경계를 넘을 때 ABI 호환성 문제가 없음
- 모든 FMOD 호출이 `CSound_Manager` 내부에 격리되어 교체 용이

### 9.2 개선 여지가 있는 부분

**CGameObject 상속은 과잉**
- 단순히 `Update()`를 호출하기 위해 `CGameObject`를 상속하는 것은 불필요한 의존성
- `CBase`를 상속하고 `CGameInstance::Update_Engine()`에서 직접 Update 호출이면 충분

**enum 하드코딩**
- `SOUND_KEY_NAME`에 사운드가 약 150개 하드코딩 → 새 사운드 추가마다 enum 수정 필요
- 문자열 키 또는 해시 기반 등록으로 유연성 확보 가능

**enum 시작값 분리 (0, 100, 200)**
- `SOUND_KEY_NAME`은 0부터, `SOUND_GROUP_KEY_NAME`은 100부터, `SOUND_GROUP_KEY`는 200부터 시작
- 의도는 ID 충돌 방지이지만, 타입이 다른 enum class이므로 실제로는 불필요

**`FMOD_CREATESAMPLE`로 모든 사운드를 메모리에 로드**
- BGM처럼 긴 파일도 전체 디코딩하여 메모리에 올림
- BGM은 `FMOD_CREATESTREAM`으로 스트리밍하는 것이 메모리 효율적

**Update에서의 채널 정리 방식**
- 매 프레임 전체 채널을 순회하며 `IsPlaying` 체크 → O(n) 비용
- `IsPlaying==false`인 채널에 `Stop`을 호출하는 것은 불필요 (이미 정지 상태)
- FMOD가 내부적으로 채널을 관리하므로 이 로직 자체가 불필요할 수 있음

**wstring → string 변환**
- `std::wstring_convert<std::codecvt_utf8>` 사용 — C++17에서 deprecated
- 파일 경로를 처음부터 `std::string`으로 관리하거나 `WideCharToMultiByte` 사용이 안전

### 9.3 내 프레임워크(DX9) 적용 시 고려사항

| 참고프로젝트2 | 내 프레임워크 | 차이/적용 |
|--------------|--------------|-----------|
| FMOD C API | (사운드 미구현) | FMOD 도입 시 C API 권장 — 가볍고 안정적 |
| CGameObject 상속 | CBase 상속 권장 | Update 호출만 필요하므로 CBase로 충분 |
| enum 하드코딩 | wstring 키 권장 | 기존 프레임워크의 프로토타입 관리와 일관성 유지 |
| CREATESAMPLE 전용 | SFX=CREATESAMPLE, BGM=CREATESTREAM | 메모리 효율 고려 |
| 파사드 위임 | CDInputMgr처럼 싱글톤 직접 접근 | 파사드 도입 시 일관성 확보 |
| 카테고리 볼륨 | 채택 권장 | BGM/SFX/VOICE 분리는 거의 모든 게임에서 필요 |
| 그룹 랜덤 재생 | 채택 권장 | 타격음 등의 다양성에 효과적 |

---

## 부록: 사운드 등록~재생 전체 시퀀스

```
[레벨 초기화 — 1회]
Level_GamePlay::Ready_Scene()
  ├─ Register_Sound("013_bat_space.ogg", SPACE_BGM, BGM, loop=true)
  │   → FMOD_System_CreateSound() → m_soundMap에 저장
  ├─ Register_Sound_Group(LIGHT_ATTACK_Goku, "Light_Attack_1.ogg", ...)
  │   → FMOD_System_CreateSound() → m_groupSoundMap에 저장
  │   → m_soundGroupMap[LIGHT_ATTACK_Goku].push_back(alias)
  └─ Play_Sound(SPACE_BGM, true, 0.2f)  ← BGM 즉시 재생

[매 프레임]
Update_Engine(dt)
  └─ CSound_Manager::Update(dt)
      └─ FMOD_System_Update()

[전투 중 — 이벤트 기반]
CCharacter::Player_Update(dt)
  → ProcessEventsBetweenFrames2(prevPos, curPos)
    → CFrameEvent_Manager: 프레임 이벤트 트리거
      → Character::Play_Sound(Goku_Heavy_Attack, false, 1.0)
        → CGameInstance::Play_Sound()
          → CSound_Manager::Play_Sound()
            ├─ nonOverlapping 체크
            ├─ categoryVolume 조회 (VOICE → 1.0)
            ├─ actualVolume = 1.0 × 1.0 = 1.0
            ├─ FMOD_System_PlaySound()
            └─ m_channelMap[alias] = { channel, baseVolume=1.0 }

[옵션 UI — 볼륨 변경]
UI_Opt_Sound_Volume_Gauge → Set_Category_Volume(SFX, 0.5)
  → 모든 SFX 카테고리 채널 순회
    → newVolume = baseVolume × 0.5
    → FMOD_Channel_SetVolume(newVolume)
```
