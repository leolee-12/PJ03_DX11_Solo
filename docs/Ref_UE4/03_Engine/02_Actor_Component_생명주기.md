# [3-2] Engine — Actor-Component 생명주기

- 선행: [3-1] 구조파악 결과 확인 완료
- 대상: `Ref_UE4/Source/Runtime/Engine/` (.h + .cpp)
- 소스 버전: UE4 4.27

---

## 1. SpawnActor 호출 시 일어나는 일

### 1.1 전체 시퀀스

```
UWorld::SpawnActor()                     [LevelActor.cpp:308]
  │
  ├─ 1. 유효성 검증 (Deprecated, Abstract, 클래스 타입 등)
  ├─ 2. Template 결정 (CDO 또는 전달된 Template)
  ├─ 3. SpawnLevel 결정 (Owner의 Level 또는 CurrentLevel)
  ├─ 4. 충돌 처리 방식 결정 (CollisionHandlingMethod)
  ├─ 5. NewObject<AActor>() — 메모리 할당 + CDO 프로퍼티 복사
  ├─ 6. Level->Actors.Add(Actor) — 레벨에 등록
  ├─ 7. OnActorPreSpawnInitialization 브로드캐스트
  │
  └─ 8. Actor->PostSpawnInitialize()     [Actor.cpp:3079]
        ├─ 네트워크 Role 설정 (ExchangeNetRoles)
        ├─ Owner / Instigator 설정
        ├─ RootComponent Transform 설정
        ├─ DispatchOnComponentsCreated() — 네이티브 컴포넌트에 OnComponentCreated 호출
        ├─ RegisterAllComponents() — 컴포넌트를 월드에 등록 (렌더링/물리 프록시 생성)
        ├─ PostActorCreated() — CDO 로드가 아닌 새 생성임을 알림
        │
        └─ (!bDeferConstruction인 경우) FinishSpawning()  [Actor.cpp:3199]
              ├─ bHasFinishedSpawning = true
              ├─ ExecuteConstruction() — 블루프린트 Construction Script 실행
              │
              └─ PostActorConstruction()   [Actor.cpp:3253]
                    ├─ PreInitializeComponents()   — 입력 자동 수신 설정
                    ├─ InitializeComponents()      — 각 컴포넌트의 InitializeComponent() 호출
                    ├─ 충돌 핸들링 (위치 조정 or Destroy)
                    ├─ PostInitializeComponents()  — bActorInitialized = true, ReplicatedComponents 갱신
                    │
                    └─ (World->HasBegunPlay()이면) DispatchBeginPlay()
```

### 1.2 핵심 함수 시그니처

```cpp
// LevelActor.cpp:308
AActor* UWorld::SpawnActor(UClass* Class, FTransform const* UserTransformPtr,
                           const FActorSpawnParameters& SpawnParameters);

// Actor.cpp:3079
void AActor::PostSpawnInitialize(FTransform const& UserSpawnTransform,
    AActor* InOwner, APawn* InInstigator, bool bRemoteOwned,
    bool bNoFail, bool bDeferConstruction);

// Actor.cpp:3199
void AActor::FinishSpawning(const FTransform& UserTransform,
    bool bIsDefaultTransform,
    const FComponentInstanceDataCache* InstanceDataCache = nullptr);
```

---

## 2. BeginPlay 호출 순서

### 2.1 AActor::DispatchBeginPlay → BeginPlay 흐름

```
AActor::DispatchBeginPlay(bFromLevelStreaming)    [Actor.cpp:3506]
  │
  ├─ ActorHasBegunPlay = EActorBeginPlayState::BeginningPlay
  │
  └─ AActor::BeginPlay()                          [Actor.cpp:3539]
        │
        ├─ 1. SetLifeSpan(InitialLifeSpan)  — 수명 타이머 설정
        ├─ 2. RegisterAllActorTickFunctions(true, false) — Actor Tick 등록
        │
        ├─ 3. 모든 컴포넌트 순회:
        │     for (UActorComponent* Component : Components)
        │       ├─ Component->RegisterAllComponentTickFunctions(true)
        │       └─ Component->BeginPlay()   ← 컴포넌트별 BeginPlay
        │
        ├─ 4. AutoDestroySubsystem 등록 (bAutoDestroyWhenFinished)
        ├─ 5. ReceiveBeginPlay()  — BP Event: BeginPlay
        │
        └─ ActorHasBegunPlay = EActorBeginPlayState::HasBegunPlay
```

### 2.2 호출 순서 요약

| 순서 | 대상 | 함수 |
|------|------|------|
| 1 | Actor | `SetLifeSpan()` |
| 2 | Actor | `RegisterAllActorTickFunctions()` |
| 3 | 각 Component | `RegisterAllComponentTickFunctions()` → `BeginPlay()` |
| 4 | Actor | `ReceiveBeginPlay()` (BP 이벤트) |

**핵심**: 컴포넌트의 `BeginPlay()`가 먼저 호출되고, Actor의 `ReceiveBeginPlay()`(BP)는 마지막에 호출됨.

---

## 3. Tick 함수의 호출 흐름 (TickGroup별 순서)

### 3.1 TickGroup 열거형

```cpp
// EngineBaseTypes.h:79
enum ETickingGroup
{
    TG_PrePhysics,      // 물리 시뮬레이션 전 (기본값, Actor/Component 대부분)
    TG_StartPhysics,    // 물리 시작 (Hidden, 내부용)
    TG_DuringPhysics,   // 물리 진행 중 (비동기)
    TG_EndPhysics,      // 물리 종료 (Hidden, 내부용)
    TG_PostPhysics,     // 물리 후 (결과 반영, 애니메이션 등)
    TG_PostUpdateWork,  // 업데이트 후 (카메라, 최종 위치 조정)
    TG_LastDemotable,   // 최후 그룹
};
```

### 3.2 UWorld::Tick() 내 TickGroup 실행 순서

```
UWorld::Tick(DeltaSeconds)                           [LevelTick.cpp:1273]
  │
  ├─ FTickTaskManagerInterface::Get().StartFrame()    ← 프레임 시작
  │
  ├─ RunTickGroup(TG_PrePhysics)                      ← ① Actor/Component 기본 Tick
  ├─ EnsureCollisionTreeIsBuilt()                     ← 충돌 트리 갱신
  ├─ RunTickGroup(TG_StartPhysics)                    ← ② 물리 시뮬레이션 시작
  ├─ RunTickGroup(TG_DuringPhysics, false/*비동기*/)  ← ③ 물리 진행 중 (대기 안 함)
  ├─ RunTickGroup(TG_EndPhysics)                      ← ④ 물리 시뮬레이션 종료
  ├─ RunTickGroup(TG_PostPhysics)                     ← ⑤ 물리 결과 반영
  │
  ├─ LatentActionManager.ProcessLatentActions()       ← Latent 액션 처리
  ├─ TimerManager.Tick()                              ← 타이머 처리
  ├─ FTickableGameObject::TickObjects()               ← Non-Actor Tickable 처리
  ├─ PlayerController->UpdateCameraManager()          ← 카메라 갱신
  │
  ├─ RunTickGroup(TG_PostUpdateWork)                  ← ⑥ 최종 업데이트
  ├─ RunTickGroup(TG_LastDemotable)                   ← ⑦ 최후 그룹
  │
  └─ FTickTaskManagerInterface::Get().EndFrame()      ← 프레임 종료
```

### 3.3 Actor Tick 호출 체인

```cpp
// FActorTickFunction에 의해 호출됨
AActor::TickActor(DeltaSeconds, TickType, ThisTickFunction)  [Actor.cpp:1082]
  └─ AActor::Tick(DeltaSeconds)                               [Actor.cpp:1094]
       └─ ReceiveTick(DeltaSeconds)  // BP Tick 이벤트
```

- 기본 TickGroup은 `TG_PrePhysics` (Actor.cpp:97)
- `SetTickGroup()`으로 변경 가능
- `bAllowTickBeforeBeginPlay` — BeginPlay 전 Tick 허용 여부 (기본 false)

---

## 4. AActor::Destroy() 호출 시 컴포넌트 해제 순서

### 4.1 Destroy → 실제 파괴 흐름

```
AActor::Destroy(bNetForce, bShouldModifyLevel)       [Actor.cpp:4050]
  │
  └─ UWorld::DestroyActor(ThisActor, ...)             [LevelActor.cpp:633]
        │
        ├─ 1. 유효성/권한 검증 (PendingKill, WorldSettings, 네트워크 Role)
        ├─ 2. BeginPlay 중이면 → bActorWantsDestroyDuringBeginPlay 설정 후 지연
        ├─ 3. MarkActorIsBeingDestroyed — 재귀 방지
        ├─ 4. IStreamingManager 알림
        │
        ├─ 5. ThisActor->Destroyed()                   [Actor.cpp:2332]
        │     ├─ RouteEndPlay(EEndPlayReason::Destroyed)
        │     │   ├─ AActor::EndPlay()
        │     │   │   ├─ ActorHasBegunPlay = HasNotBegunPlay
        │     │   │   ├─ ReceiveEndPlay() — BP 이벤트
        │     │   │   ├─ OnEndPlay 브로드캐스트
        │     │   │   └─ 각 Component->EndPlay()  ← 컴포넌트 EndPlay
        │     │   └─ UninitializeComponents()
        │     ├─ ReceiveDestroyed() — BP 이벤트
        │     └─ OnDestroyed 브로드캐스트
        │
        ├─ 6. 자식 Actor Detach (AttachedActors)
        ├─ 7. 부모로부터 Detach (DetachFromActor)
        ├─ 8. ClearComponentOverlaps()
        ├─ 9. SetOwner(NULL)
        │
        ├─ 10. 네트워크 드라이버 알림 (NotifyActorDestroyed)
        ├─ 11. RemoveActor() — Level->Actors에서 제거
        │
        ├─ 12. ThisActor->UnregisterAllComponents()  ← 컴포넌트 월드 등록 해제
        ├─ 13. ThisActor->MarkPendingKill()           ← GC 대상 마킹
        ├─ 14. ThisActor->MarkPackageDirty()          ← 패키지 더티 마킹
        ├─ 15. ThisActor->MarkComponentsAsPendingKill() ← 컴포넌트도 PendingKill
        └─ 16. RegisterAllActorTickFunctions(false)   ← Tick 해제
```

### 4.2 컴포넌트 해제 순서 요약

| 순서 | 동작 | 설명 |
|------|------|------|
| 1 | `Component->EndPlay()` | 각 컴포넌트의 EndPlay (Destroyed→RouteEndPlay 내부) |
| 2 | `UninitializeComponents()` | 컴포넌트 초기화 해제 |
| 3 | `UnregisterAllComponents()` | 렌더링/물리 프록시 제거, 월드에서 등록 해제 |
| 4 | `MarkComponentsAsPendingKill()` | GC 대상 마킹 |

---

## 5. Deferred Spawn과 즉시 Spawn의 차이

### 5.1 분기 지점

`FActorSpawnParameters::bDeferConstruction` 값에 따라 `PostSpawnInitialize()` 내부에서 분기:

```cpp
// Actor.cpp:3185
if (!bDeferConstruction)
{
    FinishSpawning(UserSpawnTransform, true);   // ← 즉시: Construction + Init + BeginPlay
}
else if (SceneRootComponent != nullptr)
{
    // 지연: 네이티브 RootComponent가 있을 때만 Transform을 캐시
    // 나중에 수동 FinishSpawning 호출 대기
    GSpawnActorDeferredTransformCache.Emplace(this, UserSpawnTransform);
}
```

### 5.2 비교 표

| 항목 | 즉시 Spawn (기본) | Deferred Spawn |
|------|-------------------|----------------|
| `bDeferConstruction` | `false` | `true` |
| 사용 API | `UWorld::SpawnActor()` | `UWorld::SpawnActorDeferred()` + 수동 `FinishSpawning()` |
| `PostSpawnInitialize`까지 | 동일 | 동일 |
| `FinishSpawning()` 시점 | `PostSpawnInitialize` 내부에서 즉시 | 호출자가 프로퍼티 설정 후 수동 호출 |
| `ExecuteConstruction()` | 자동 | 수동 `FinishSpawning()` 시 실행 |
| `PreInitializeComponents()` | 자동 | 수동 `FinishSpawning()` 시 실행 |
| `BeginPlay()` | 자동 (월드가 BegunPlay 상태면) | 수동 `FinishSpawning()` 시 실행 |

### 5.3 Deferred Spawn 사용 시점

Deferred Spawn은 SpawnActor와 FinishSpawning 사이에 **Construction Script 실행 전** 프로퍼티를 설정해야 할 때 사용:

```
1. SpawnActorDeferred<T>()
2. Actor->SomeProperty = Value;    ← Construction Script에서 이 값을 참조 가능
3. Actor->FinishSpawning(Transform);
```

### 5.4 공통 실행 구간

두 경우 모두 `PostSpawnInitialize()`까지는 동일하게 실행됨:
- 네트워크 Role 설정
- Owner/Instigator 설정
- 네이티브 컴포넌트 생성 및 등록 (RootComponent 포함)
- `PostActorCreated()` 호출

---

## 분석 요약

| 항목 | 수치 |
|------|------|
| 분석한 소스 파일 | `Actor.cpp`, `LevelActor.cpp`, `LevelTick.cpp`, `ActorConstruction.cpp`, `Actor.h`, `World.h`, `EngineBaseTypes.h` |
| 분석한 핵심 함수 수 | 15개 (`SpawnActor`, `PostSpawnInitialize`, `FinishSpawning`, `PostActorConstruction`, `DispatchBeginPlay`, `BeginPlay`, `TickActor`, `Tick`, `Destroy`, `DestroyActor`, `Destroyed`, `RouteEndPlay`, `EndPlay`, `PreInitializeComponents`, `PostInitializeComponents`) |
| 불확실 사항 | `InitializeComponents()`의 내부 구현 — 각 컴포넌트에 `InitializeComponent()`를 호출하는 것은 확인했으나, 호출 순서(등록 순서 vs 계층 순서)는 구현부 추가 확인 필요 |
