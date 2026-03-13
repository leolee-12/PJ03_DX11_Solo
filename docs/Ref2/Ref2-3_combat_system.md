# 전투 시스템 심화 분석

> 참고프로젝트2 — Dragon Ball FighterZ 모작
> 분석 대상: Character, AttackObject, Collider_Manager, Input, FrameEvent_Manager, Play_Goku

---

## 1. 핵심 책임과 경계

| 클래스 | 책임 | 위치 |
|--------|------|------|
| `CCharacter` | 격투 캐릭터 베이스. 입력 버퍼, 가드, 스턴, 중력, 체이스, 그랩, 태그 등 전투 상태 전반 | Client |
| `CInput` | 방향(9방향) + 버튼(8종) 쌍 — 입력 버퍼의 단위 요소 | Client |
| `CAttackObject` | 판정 박스(=히트박스) 오브젝트. ATTACK_DESC로 데미지/스턴/임펄스/모션 등 40여 개 속성 보유 | Client |
| `CCollider` | AABB 충돌 컴포넌트. Enter/Stay/Exit 콜백 | Engine |
| `CCollider_Manager` | 11개 충돌 그룹 조합별 충돌 검사 + 결과 분배 (멀티스레드) | Engine |
| `CFrameEvent_Manager` | 애니메이션 프레임 이벤트 해석/실행 (CSV 기반) | Client |
| `Play_Goku` 등 | 캐릭터별 구현체 — 커맨드 등록, AttackEvent, Gravity 오버라이드 | Client |

### 소유 관계

```
CCharacter (CGameObject 상속)
 ├─ CCollider*         m_pColliderCom     (Body 충돌 컴포넌트)
 ├─ CModel*            m_pModelCom        (애니메이션 모델)
 ├─ CTransform*        m_pTransformCom    (위치/이동)
 ├─ CCharacter*        m_pEnemy           (상대 캐릭터 참조, 비소유)
 ├─ vector<CInput>     inputBuffer        (입력 버퍼)
 ├─ vector<CommandPattern> MoveCommandPatternsFunction       (느슨 매칭 커맨드)
 └─ vector<CommandPattern> MoveCommandPatternsFunction_Exactly (정확 매칭 커맨드)

CAttackObject (CGameObject 상속)
 ├─ CCollider*         m_pColliderCom     (판정 충돌 컴포넌트)
 ├─ CCharacter*        m_pOwner           (공격 주체, 비소유)
 └─ CTransform*        m_pOwnerTransform  (주체 위치 추적)
```

---

## 2. 프레임당 전투 업데이트 흐름

`CGameInstance::Update_Engine` 순서 기반:

```
1. Destory_Update    — dead 플래그 객체 제거
2. Player_Update     — ★ 캐릭터 전투 로직 (아래 상세)
3. Sound_Update
4. Collider_Update   — ★ 충돌 검사 + 콜백 발동
5. Update            — 일반 오브젝트 Update
6. Collider_Update   — (2차) 추가 충돌 검사
7. Late_Update       — 렌더 등록
8. Camera_Update
```

### Player_Update 내부 (Play_Goku 기준)

```
Player_Update(dt)
│
├─ InputedCommandUpdate(dt)        ← 0.35초 초과 입력 제거
├─ if (!chase && !stun)
│   ├─ InputCommand()              ← 키보드 → CInput 변환, 버퍼에 push
│   └─ CheckAllCommands()          ← 커맨드 패턴 매칭 → action() 실행
│
├─ Chase2(dt)                      ← 체이스 이동/공격
├─ Chase_Grab(dt)                  ← 그랩 체이스
├─ Character_Play_Animation(dt)    ← 애니메이션 재생 + 프레임 이벤트 발동
│   └─ ProcessEventsBetweenFrames2() → FrameEvent_Manager::UseEvent()
│
├─ if (stun) Update_StunImpus(dt)  ← 피격 넉백 처리
├─ Gravity(dt)                     ← 중력 + 낙하
├─ AttckCancleJump()               ← 점프 캔슬
├─ Move(dt) → MoveKey1Team(dt)     ← 이동 입력 처리
├─ Guard_Update()                  ← 가드 상태 갱신
├─ Collider Update                 ← 위치 반영
└─ Tag_KeyCheck()                  ← 태그 교체 체크
```

---

## 3. 입력 시스템

### CInput 구조

```cpp
enum DirectionInput { NONE, UP, DOWN, LEFT, RIGHT, UP_LEFT, UP_RIGHT, DOWN_LEFT, DOWN_RIGHT };
enum ButtonInput    { NONE_BUTTON, LIGHT, MEDIUM, HEAVY, SPECIAL, ASSIST1, ASSIST2, KI, GRAB };
```

### 입력 버퍼 메커니즘

| 항목 | 값 |
|------|-----|
| 버퍼 크기 | 30 프레임 |
| 타임아웃 | 0.35초 (InputedCommandUpdate에서 제거) |
| 중복 제거 | 직전 입력과 동일하면 push 안 함 |
| 팀별 키맵 | 1P: WASD+UIJKO / 2P: 방향키+넘패드 |

### InputCommand() 흐름

```
키보드 폴링 → 방향 결정 (LookDirection 반영: 앞=RIGHT, 뒤=LEFT)
              → 버튼 결정 (동시입력: L+M→GRAB 등)
              → CInput 생성 → 중복 체크 → inputBuffer.push_back()
```

### 커맨드 패턴 매칭

```cpp
struct CommandPattern {
    vector<CInput> pattern;    // 입력 시퀀스
    function<void()> action;   // 매칭 시 실행할 함수
};
```

**두 가지 매칭 모드:**
- `CheckCommandSkippingExtras` — 느슨 매칭. 중간에 다른 입력이 있어도 순서만 맞으면 OK
- `CheckCommand_Exactly` — 정확 매칭. 연속으로 정확히 일치해야 함

**CheckAllCommands 우선순위:**
1. `MoveCommandPatternsFunction` (느슨 매칭) — 먼저 검사
2. `MoveCommandPatternsFunction_Exactly` (정확 매칭) — 나중에 검사
3. 첫 매칭 시 즉시 return + inputBuffer.clear()

**표준 커맨드 패턴 (static):**

| 패턴명 | 입력 시퀀스 |
|--------|------------|
| `Pattern_236` | DOWN → DOWN_RIGHT → RIGHT + 버튼 |
| `Pattern_214` | DOWN → DOWN_LEFT → LEFT + 버튼 |
| `Pattern_236236` | 236 × 2 (궁극기) |
| `Pattern_214214` | 214 × 2 (궁극기) |

---

## 4. 충돌 시스템

### 충돌 그룹 (11개)

```
CG_1P_BODY, CG_2P_BODY           — 캐릭터 바디
CG_1P_Energy_Attack, CG_2P_Energy_Attack  — 에너지 공격 (기공파 등)
CG_1P_Ranged_Attack, CG_2P_Ranged_Attack  — 원거리 공격 (기탄 등)
CG_1P_Melee_Attack,  CG_2P_Melee_Attack   — 근접 공격
CG_1P_REFLECT,       CG_2P_REFLECT        — 반사
CG_END
```

### 충돌 검사 파이프라인

```
Check_Collision(dt)
│
├─ 1) 모든 그룹 조합 → collisionPairs 벡터 생성
│     (Body×Body, Body×Energy, Energy×Energy, ... 총 22개 조합)
│
├─ 2) 스레드풀 분배 (N스레드)
│     각 스레드: AABB 교차 검사 → localCollisions → mutex lock → m_CollisionResults에 병합
│
└─ 3) ProcessCollisionResults(dt)
      ├─ 충돌 결과를 그룹 조합별로 분류
      ├─ 즉시 처리: Body×Body, Ranged×Body, Melee×Body 등 (개별 pair)
      ├─ 묶음 처리: Energy×Energy, Energy×Body 등 (vector 모아서 front()만 처리)
      ├─ Enter/Stay/Exit 판정 (m_CollisionHistory 비교)
      └─ m_CollisionHistory = currentCollisions (히스토리 갱신)
```

### 충돌 결과 처리 패턴

**즉시 처리 (Ranged/Melee vs Body):**
- Enter/Stay 콜백 호출 후 `Destroy_Reserve(공격콜라이더)` → 판정 1회성

**묶음 처리 (Energy 계열):**
- 같은 프레임에 여러 Energy 충돌 시 `front()`만 처리 → 다중 히트 방지

**반사(Reflect) 처리:**
- Reflect 콜라이더와 공격 충돌 시 → 공격 쪽 Destroy + 반사 효과

---

## 5. 공격 판정 (AttackObject)

### ATTACK_DESC 주요 필드

| 필드 | 타입 | 역할 |
|------|------|------|
| `ihitCharacter_Motion` | HitMotion | 피격 모션 종류 (14종) |
| `eAttackGrade` | AttackGrade | NORMAL / ULTIMATE |
| `eAttackType` | AttackType | LOW / MIDDLE / HIGH / GRAB_GROUND / GRAB_AIR / COMMANDGRAB |
| `fhitCharacter_Impus` | float2 | 넉백 벡터 (x,y) |
| `fhitCharacter_StunTime` | float | 스턴 지속 시간 |
| `iDamage` | int | 데미지 (DamageScale 적용 후) |
| `fLifeTime` | float | 판정 지속 시간 |
| `fAnimationLockTime` | float | 히트스톱 시간 |
| `bGroundSmash` | bool | 바운드 발동 여부 |
| `bReflect` | bool | 반사 가능 여부 |
| `iGainAttackStep` | ushort | 보정 단계 증가량 |
| `iGainKiAmount` | int | 기 게이지 획득량 |
| `pOwner` | CCharacter* | 공격 주체 |

### AttackObject 생명주기

```
1. 캐릭터 AttackEvent() 호출 시 ATTACK_DESC 설정
2. Add_GameObject_ToLayer → CAttackObject::Initialize(ATTACK_DESC)
3. Update: m_fAccLifeTime 누적 → LifeTime 초과 시 Set_RemoteDestory()
4. 충돌 발생 시 OnCollisionEnter() → 피격 처리 → Destroy
```

### OnCollisionEnter 처리 (AttackObject → Body)

```
1. 피격자가 Reflect 상태 → return
2. Set_Hit4() 호출 → AttackColliderResult 반환
3. RESULT_HIT:
   ├─ GroundSmash 설정
   ├─ 공격자 AnimationStop (히트스톱)
   ├─ Ki 게이지 획득
   ├─ AttackStep/HitCount 증가
   ├─ 카메라 줌/쉐이크
   ├─ 히트 이펙트 생성 (BurstU-1, BurstU-2 등)
   └─ 판정 소멸
4. RESULT_GUARD:
   ├─ 양측 AnimationStop(0.08초)
   └─ 판정 소멸
5. RESULT_MISS:
   ├─ GRAB 타입 → 소멸
   └─ 그 외 → 유지 (return)
6. RESULT_DRAW:
   └─ 양측 AnimationStop(0.3초)
```

---

## 6. 피격/가드 시스템

### Set_Hit4() — 메인 피격 판정

```
Set_Hit4(hitMotion, grade, type, stunTime, damage, lockTime, attackerDir, impulse)
│
├─ 1. BreakFall 면역 체크 → m_bBreakFall_Air 시 return RESULT_NONE
├─ 2. GroundSmash 면역 체크 → m_bHitGroundSmashed 시 return RESULT_NONE
├─ 3. Guard_Check3() 호출
│     ├─ 가드 애니메이션 중 → CompareGuardType3()
│     │   ├─ HIGH → 무조건 가드 성공
│     │   ├─ MIDDLE → 앉기 가드 시 실패
│     │   └─ LOW → 서기 가드 시 실패
│     ├─ GRAB 타입 → CompareGrabType3()
│     │   ├─ GRAB_GROUND → 공중이면 MISS
│     │   ├─ GRAB_AIR → 지상이면 MISS
│     │   └─ 그랩 버튼 입력 중 → MISS (그랩 풀기)
│     └─ 가드 미입력 → RESULT_HIT
│
├─ 4. RESULT_GUARD 시:
│     ├─ 가드 애니메이션 설정 (서기/앉기/공중)
│     └─ 가드 넉백 적용
│
├─ 5. RESULT_HIT 시:
│     ├─ m_bStun = true, 스턴 시간 설정
│     ├─ HP 감소 (Red HP 시스템 포함)
│     ├─ Set_HitAnimation() — 히트모션별 분기
│     └─ 공격자 방향 기반 넉백 방향 결정
│
└─ return eResult
```

### HitMotion 종류 (14종)

| HitMotion | 설명 |
|-----------|------|
| `HIT_LIGHT` | 약 피격 (제자리) |
| `HIT_MEDIUM` | 중 피격 |
| `HIT_HEAVY` / `HIT_HEAVY_DOWN` | 강 피격 / 내려찍기 |
| `HIT_CHASE` | 체이스 피격 |
| `HIT_CROUCH_MEDIUM` | 앉기 중 피격 → 공중 뜸 |
| `HIT_KNOCK_AWAY_LEFT` | 횡방향 날아감 (벽바운스 가능) |
| `HIT_KNOCK_AWAY_UP` | 수직 날아감 |
| `HIT_KNOCK_AWAY_LEFTDOWN` | 대각 아래 날아감 |
| `HIT_KNOCK_AWAY_UP_GRAVITY` | 수직 + 즉시 중력 |
| `HIT_KNOCK_AWAY_LEFT_NONEBOUNDE` | 횡방향 (벽바운스 불가) |
| `HIT_SPIN_AWAY_LEFTUP` | 회전하며 대각 위로 |
| `HIT_SPIN_AWAY_UP` | 회전하며 수직 위로 |
| `HIT_WALLBOUNCE` | 벽 바운스 피격 |
| `HIT_NONE` | 모션 없음 |

### Guard_Update — 가드 입력 판정

```
조건: 지상 이동/점프/낙하 애니메이션 중
방식: "상대 반대 방향키를 누르고 있으면" 가드 활성
  - 1P: LookDir=1(→)이면 A키(←), LookDir=-1이면 D키(→)
  - 2P: 방향키 동일 로직
```

---

## 7. 중력/물리 시스템

### Gravity 공식

```cpp
// 포물선 공식 (Ver4)
fGravity = (-0.7f * (2*m_fGravityTime - m_fJumpPower)² + 4) * 0.1f
```

| 변수 | 역할 |
|------|------|
| `m_fGravityTime` | 누적 중력 시간 (deltaTime 합산) |
| `m_fJumpPower` | 점프력 (포물선 정점 결정) |
| `m_fImpuse` | 넉백 벡터 (x, y) |
| `m_bNoGravity` | 중력 무시 플래그 (안전시간 존재) |
| `m_bAttackGravity` | 공격 중 중력 적용 여부 |

### 중력 적용 대상 애니메이션

점프/낙하, 공중 공격(Air1~3), 공중 피격, 날아감(Away), 벽바운스, BreakFall 등

### 특수 중력 설정

| 함수 | 효과 |
|------|------|
| `Set_ForcedGravityDown()` | GravityTime=0, JumpPower=0 → 즉시 낙하 |
| `Set_ForcedGravityTime_LittleUp()` | GravityTime=0, JumpPower=0.37 → 살짝 뜸 |
| `Set_ForcveGravityTime(f)` | GravityTime=f (중간값 지정) |

### 벽 바운스 (Wall Bounce)

```
Update_StunImpus 중:
  HIT_KNOCK_AWAY_LEFT 상태 + 벽 근접(|X|>12 또는 적과 거리>8)
  → m_bWallBounce=true 시: HIT_WallBounce 애니메이션 + 반대 임펄스 + 카메라 쉐이크
  → m_bWallBounce=false 시: 그냥 정지
```

---

## 8. 체이스/그랩 시스템

### Chase2() — 체이스 공격

```
1. 준비자세 (Fall 애니메이션):
   ├─ 상향 이동 (-1*dt, 4.8*dt)
   └─ 0.2초 후 → Chase 애니메이션 전환
       ├─ 이펙트 생성 (BurstR-02 + 본 매트릭스 추적)
       └─ Attack_Chase 오브젝트 생성 (Melee, 데미지 300)

2. 추적 (Chase 애니메이션):
   ├─ 적 위치로 방향 벡터 계산
   ├─ 거리 < 0.5 → 체이스 종료 + Fall 전환
   ├─ 가속 이동: dir * accTime² * 0.7 (가속도 기반)
   └─ 이펙트 회전 = atan2(dirY, dirX)

3. 20초 타임아웃 → 강제 종료
```

### 그랩 시스템

```
AttackType:
  ATTACKTYPE_GRAB_GROUND  — 지상 그랩 (공중이면 MISS)
  ATTACKTYPE_GRAB_AIR     — 공중 그랩 (지상이면 MISS)
  ATTACKTYPE_COMMANDGRAB  — 커맨드 그랩

그랩 풀기: 그랩 버튼 입력 중이면 MISS
그랩 루프: m_iGrabLoof (반복 횟수 제어)
```

---

## 9. 데미지 보정 (Damage Scaling)

```cpp
Get_DamageScale() {
  // AttackStep 기반 보정 (콤보가 길수록 데미지 감소)
  Step  0~7:  1.0 → 0.3 (10%씩 감소)
  Step  8~10: 0.3
  Step 11~13: 0.25
  Step 14~16: 0.2
  Step 17+:   0.15

  // 궁극기 최소 보정: 35%
  // 스파킹 보너스: +10%
  // 최종 보정: × 0.7
}
```

---

## 10. 애니메이션 이벤트 시스템

### 데이터 구조

```cpp
// 3중 map: 캐릭터 → 애니메이션 → 프레임 → 이벤트 리스트
typedef map<_uint, map<_int, map<_int, vector<string>>>> FrameEventMap;
```

### ProcessEventsBetweenFrames2 흐름

```
매 프레임:
  prevFrame = 이전 프레임 위치
  currentFrame = 현재 프레임 위치
  → 그 사이의 모든 이벤트를 FrameEvent_Manager::UseEvent()로 실행
```

### UseEvent 커맨드 목록

| 커맨드 | 매개변수 | 역할 |
|--------|----------|------|
| `ObjectMove` | x,y,z,w | 캐릭터 위치 이동 (방향 반영) |
| `TickPerSecondChange` | speed | 애니메이션 속도 변경 |
| `AnimSpeedChange` | maxTime, tickPerSec | 재생 속도 상세 제어 |
| `PositionChange` | frame | 애니메이션 프레임 점프 |
| `NextAnimationCheck` | — | AttackNextMoveCheck 호출 |
| `AttackEvent` | index, addEvent | 공격 판정 생성 |
| `SetAnimation` | index | 즉시 애니메이션 전환 |
| `SetNextAnimation` | index, position | 다음 애니메이션 예약 |
| `FlipPlayerDirection` | — | 방향 전환 |
| `EnemyChase` | offsetX, offsetY | 적 위치로 텔레포트 |
| `AttackGravity` | bool | 공격 중 중력 토글 |
| `DynamicMove` | bool | 동적 이동 토글 |
| `PlaySound` / `PlayGroupSound` | index, loop, vol | 사운드 재생 |
| `Camera_Play_*` | camera, anim, shake | 카메라 연출 |
| `SubTitle_Play/Stop` | id, duration | 자막 |

---

## 11. 캐릭터 구현체 패턴 (Play_Goku)

### 구조

```
Play_Goku : CCharacter
 ├─ enum AnimationIndex { ANIME_IDLE, ANIME_ATTACK_LIGHT1, ..., ~80개 }
 ├─ Player_Update()  — 전체 전투 루프 오버라이드
 ├─ Gravity()        — 캐릭터별 공격 중 중력 커스텀
 ├─ AttackEvent()    — 애니메이션별 ATTACK_DESC 설정 (switch-case)
 └─ Ready_Components() — Body 콜라이더 생성
```

### AttackEvent 패턴

```cpp
void CPlay_Goku::AttackEvent(_int iAttackEvent, _int AddEvent) {
    switch (m_pModelCom->m_iCurrentAnimationIndex) {
    case ANIME_ATTACK_LIGHT1: {
        CAttackObject::ATTACK_DESC Desc{};
        Desc.ColliderDesc.colliderGroup = (team==1) ? CG_1P_Melee : CG_2P_Melee;
        Desc.ColliderDesc.vCenter = { 0.9f * lookDir, 0.8f, 0.f };
        Desc.ColliderDesc.vExtents = { 0.3f, 0.5f, 0.2f };
        Desc.fhitCharacter_Impus = { 0.3f * lookDir, 0 };
        Desc.fhitCharacter_StunTime = 0.4f;
        Desc.iDamage = 400 * Get_DamageScale();
        Desc.ihitCharacter_Motion = HIT_LIGHT;
        // ... Add_GameObject_ToLayer()
    } break;
    // LIGHT2: 700dmg, HIT_LIGHT, 이펙트 추가
    // LIGHT3: 1000dmg, HIT_SPIN_AWAY_LEFTUP, 0.8s stun (체이스 연결용)
    }
}
```

### Player_Update 핵심 흐름 (Play_Goku)

```
1. 방향 전환 (적 위치 기반, 이동/점프 중만)
2. 입력 갱신 → 커맨드 체크 (chase/stun 아닐 때만)
3. 스파킹 활성화 체크
4. 체이스 시작 키(R / PageDown)
5. 체이스 실행 → 체이스 종료 시 Fall 전환
6. 그랩 실행
7. 애니메이션 재생 + 프레임 이벤트
8. 모션 종료 시 → 피격 → AirFall / 비피격 → AnimeEndNextMoveCheck
9. 스턴 중 → Stun_Shake + StunImpus 업데이트
10. 비스턴 → BreakFall_Air 체크
11. Gravity → AttckCancleJump → Move
12. 벽 체크 → 태그 키 체크
```

---

## 12. 설계 판단과 채택 가치

### 1) 입력 버퍼 + 패턴 매칭 분리

**구조:** InputCommand → 버퍼 축적 → CheckAllCommands (느슨/정확 2단계)
**장점:** 격투게임 커맨드 입력의 관대함(leniency) 구현 가능. 새 커맨드 추가 시 pattern + action 쌍만 등록
**채택 가치:** ★★★ — 커맨드 패턴을 데이터로 관리하여 확장성 확보

### 2) AttackObject 분리 패턴

**구조:** 공격 판정 = 독립 GameObject (ATTACK_DESC로 생성, LifeTime 후 소멸)
**장점:** 캐릭터와 판정 박스의 생명주기 분리. 다양한 판정 (근접/원거리/에너지) 통일된 인터페이스
**채택 가치:** ★★★ — 판정 로직을 캐릭터에서 분리하여 복잡도 관리

### 3) 프레임 이벤트 CSV 기반 데이터 드리븐

**구조:** 애니메이션 프레임마다 문자열 이벤트 지정 → UseEvent에서 파싱/실행
**장점:** 프로그래머 개입 없이 애니메이션 타이밍에 공격 판정, 이동, 사운드 등 배치 가능
**단점:** 문자열 비교 기반으로 타입 안정성 부재
**채택 가치:** ★★☆ — 개념은 좋으나, enum 기반이나 직렬화 구조로 개선 권장

### 4) 충돌 매니저 멀티스레드 + 그룹 분류

**구조:** 스레드풀로 AABB 검사 병렬화 → 결과를 그룹별 Process_* 함수로 분배
**장점:** 충돌 쌍이 많을 때 성능 확보. 그룹별 처리 로직 분리
**채택 가치:** ★★☆ — 1v1 격투에선 과잉이지만, 확장 시 유용

### 5) 히트스톱(AnimationStop) 양측 적용

**구조:** HIT 시 공격자/피격자 모두 애니메이션 일시정지
**장점:** 격투게임 특유의 타격감 구현
**채택 가치:** ★★★ — 필수 격투게임 피드백 패턴

### 6) DamageScale 보정 테이블

**구조:** AttackStep(콤보 단계) 기반 데미지 감소율 + 궁극기 하한/스파킹 보너스
**장점:** 무한 콤보 방지, 밸런스 조절 용이
**채택 가치:** ★★★ — 격투게임 밸런스의 핵심 메커니즘

### 7) 가드 3종 판정 (HIGH/MIDDLE/LOW)

**구조:** 공격 타입(AttackType) vs 가드 상태(서기/앉기/공중) 비교
**장점:** 격투게임 상성 시스템 구현
**주의:** 현재 컨트롤이 "뒤로 누르기=가드"이므로 별도 가드 버튼 불필요
**채택 가치:** ★★☆ — 단순하지만 효과적

### 8) 캐릭터-베이스/구현체 분리 패턴

**구조:** CCharacter(전투 공통) → Play_Goku(캐릭터별 커맨드/공격/중력)
**장점:** 공통 전투 시스템은 베이스에, 캐릭터 개성은 구현체에 집중
**단점:** CCharacter가 ~3400줄로 비대 — 상태 머신 패턴으로 분리 권장
**채택 가치:** ★★☆ — 기본 방향은 올바르나 리팩터링 여지 있음

---

## 부록: DX API 연동 지점

전투 시스템은 렌더링과 직접 연동하지 않으며, 충돌은 순수 AABB 수학 연산.
카메라 연출(줌/쉐이크)과 이펙트 생성만 렌더링 파이프라인과 간접 연동:

| 전투 이벤트 | 렌더링 연동 |
|-------------|------------|
| 히트 발생 | CEffect_Manager::Copy_Layer (이펙트 생성) |
| 강 피격 | CMain_Camera::StartCameraShake |
| 궁극기 | CMain_Camera::Play (가상 카메라) |
| 스파킹 | CRenderInstance::Switch_AllBlackOut |
