# [2-2] CoreUObject — UObject 생명주기

> 분석 대상: UE4 4.27 `Source/Runtime/CoreUObject/` (.h + .cpp)
> 선행: [2-1] 구조 파악 결과 참조
> 저장: `docs/analysis/02_CoreUObject/02_UObject_생명주기.md`

---

## 1. NewObject<T>() → 메모리 할당 → 생성자 호출 전체 흐름

### 1.1 호출 체인 요약

```
NewObject<T>(Outer, Class, Name, Flags, Template)
  └─ FStaticConstructObjectParameters 구성
       └─ StaticConstructObject_Internal(Params)
            ├─ StaticAllocateObject(Class, Outer, Name, Flags, ...)
            │    ├─ 기존 오브젝트 존재 시 → ConditionalBeginDestroy → FinishDestroy → ~UObject()
            │    ├─ 새 할당 시 → GUObjectAllocator.AllocateUObject(TotalSize, Alignment, GIsInitialLoad)
            │    ├─ Memzero 후 placement new UObjectBase(Class, Flags, Outer, Name)
            │    │    └─ UObjectBase::AddObject() → GUObjectArray 등록 + 이름 해시
            │    └─ 결과: 메모리 할당 + UObjectBase 초기화 완료 (아직 C++ 생성자 미호출)
            │
            └─ ClassConstructor(FObjectInitializer(Result, Template, ...))
                 ├─ FObjectInitializer 생성자 → ThreadContext에 push
                 ├─ UObject(const FObjectInitializer&) → C++ 생성자 실행
                 │    └─ FinalizeSubobjectClassInitialization()
                 └─ ~FObjectInitializer() (소멸자에서 후처리)
                      ├─ PendingConstruction 플래그 해제
                      ├─ ObjectArchetype 결정 (없으면 CDO 사용)
                      ├─ InitProperties() — 아키타입/CDO로부터 프로퍼티 복사
                      ├─ InitSubobjectProperties() — 서브오브젝트 프로퍼티 초기화
                      ├─ CDO인 경우 Config 로드
                      ├─ InstanceSubobjects() — 서브오브젝트 인스턴싱
                      ├─ PostInitProperties() 호출
                      └─ Class->PostInitInstance(Obj) — 클래스 레벨 후처리
```

### 1.2 StaticAllocateObject 핵심 동작

**메모리 할당** (`UObjectGlobals.cpp:2333`):

```cpp
int32 TotalSize = InClass->GetPropertiesSize();
int32 Alignment = FMath::Max(4, InClass->GetMinAlignment());
Obj = (UObject*)GUObjectAllocator.AllocateUObject(TotalSize, Alignment, GIsInitialLoad);
```

- `GUObjectAllocator`가 정렬된 메모리 블록 할당
- 초기 로드 시 Disregard-for-GC 풀 사용 가능 (영구 오브젝트용)

**UObjectBase 초기화** (placement new):

```cpp
FMemory::Memzero((void*)Obj, TotalSize);
new ((void*)Obj) UObjectBase(InClass, InFlags|RF_NeedInitialization, InternalFlags, InOuter, InName);
```

- `UObjectBase` 생성자에서 `AddObject()` 호출
- `AddObject()`: `GUObjectArray.AllocateUObjectIndex(this)` → 전역 배열 등록, `HashObject(this)` → 이름 해시 테이블 등록

### 1.3 FObjectInitializer의 역할

FObjectInitializer는 **RAII 패턴**으로 C++ 생성자 전후를 감싼다:

| 시점 | 동작 |
|------|------|
| 생성자 | ThreadContext에 push, `IsInConstructor++` |
| C++ 생성자 실행 | `UObject(const FObjectInitializer&)` — 사용자 정의 생성자 |
| 소멸자 | `PostConstructInit()` 호출 → 프로퍼티 초기화, 서브오브젝트 인스턴싱, `PostInitProperties()` |

**PostConstructInit 핵심 순서** (`UObjectGlobals.cpp:2748`):

1. 아키타입 결정 (없으면 `Class->GetDefaultObject()`)
2. `InitProperties()` — CDO/아키타입으로부터 프로퍼티 값 복사
3. `InitSubobjectProperties()` — 서브오브젝트 프로퍼티 초기화
4. CDO면 `LoadConfig()` 호출
5. `InstanceSubobjects()` — 참조 인스턴싱
6. `PostInitProperties()` — 사용자 오버라이드 가능한 초기화 콜백
7. `Class->PostInitInstance(Obj)` — 클래스 레벨 후처리

---

## 2. CDO(Class Default Object) 생성 시점과 역할

### 2.1 CDO란

- 각 UClass마다 하나 존재하는 **기본 오브젝트** (프로토타입)
- `RF_ClassDefaultObject | RF_ArchetypeObject` 플래그를 가짐
- 이름: `"Default__ClassName"`

### 2.2 생성 시점

`UClass::GetDefaultObject(bCreateIfNeeded=true)` 최초 호출 시 **지연 생성**:

```cpp
UObject* GetDefaultObject(bool bCreateIfNeeded = true) const {
    if (ClassDefaultObject == nullptr && bCreateIfNeeded)
        const_cast<UClass*>(this)->CreateDefaultObject();
    return ClassDefaultObject;
}
```

### 2.3 CreateDefaultObject 흐름 (`Class.cpp:3709`)

```
UClass::CreateDefaultObject()
  ├─ 부모 클래스 CDO 먼저 생성 (재귀)
  │    └─ ParentClass->GetDefaultObject() — 부모 CDO 강제 생성
  ├─ Blueprint 클래스면 프로퍼티 Preload + StaticLink
  ├─ StaticAllocateObject(this, GetOuter(), NAME_None, RF_Public|RF_ClassDefaultObject|RF_ArchetypeObject)
  ├─ ClassConstructor(FObjectInitializer(CDO, ParentDefaultObject, false, bShouldInitializeProperties))
  │    └─ bShouldInitializeProperties: Native=false, Blueprint=true
  └─ CDO->PostCDOContruct() — CDO 전용 후처리 콜백
```

### 2.4 CDO의 역할

| 역할 | 설명 |
|------|------|
| 프로퍼티 기본값 | 새 인스턴스 생성 시 CDO의 프로퍼티 값이 복사됨 |
| 아키타입 | `ObjectArchetype`이 지정되지 않으면 CDO가 아키타입으로 사용 |
| 서브오브젝트 템플릿 | CDO의 DefaultSubobject가 인스턴스의 서브오브젝트 템플릿 |
| 리플렉션 쿼리 | 클래스의 기본 동작/값을 런타임에 조회 가능 |
| 직렬화 델타 | CDO와 다른 값만 직렬화하여 저장 공간 절약 |

---

## 3. GC의 Mark-Sweep 과정

### 3.1 전체 흐름 (`CollectGarbageInternal`, `GarbageCollection.cpp:1940`)

```
CollectGarbage(KeepFlags, bPerformFullPurge)
  └─ CollectGarbageInternal()
       ├─ [1] 이전 Purge 완료 대기 (IncrementalPurgeGarbage(false))
       ├─ [2] PerformReachabilityAnalysis(KeepFlags)  ← Mark 단계
       │    ├─ MarkObjectsAsUnreachable()  ← 모든 오브젝트를 Unreachable로 마킹
       │    └─ PerformReachabilityAnalysisOnObjects()  ← Root에서 참조 추적
       ├─ [3] GatherUnreachableObjects()  ← Unreachable 오브젝트 수집
       ├─ [4] UnhashUnreachableObjects()  ← BeginDestroy 호출 (Full Purge 시)
       └─ [5] IncrementalPurgeGarbage()   ← 미처리분 Unhash + FinishDestroy + 메모리 해제
            └─ (Incremental 모드에서는 [4]를 여기서 점진 처리)
```

### 3.2 Mark 단계 — MarkObjectsAsUnreachable (`GarbageCollection.cpp:1135`)

**병렬 실행** (`ParallelFor`로 GUObjectArray를 스레드별 분할):

각 오브젝트에 대해:

| 조건 | 처리 |
|------|------|
| RootSet | Unreachable 마킹 안 함. `ObjectsToSerialize`에 추가 (탐색 시작점) |
| FastKeepFlags (Native, Async 등) | Unreachable 마킹 안 함 |
| KeepFlags 보유 & !PendingKill | Unreachable 마킹 안 함 |
| PendingKill & ClusterRoot | 클러스터 해체 대상 목록에 추가 |
| 그 외 | `ObjectItem->SetFlags(EInternalObjectFlags::Unreachable)` |

### 3.3 Mark 단계 — Reachability Analysis

`ObjectsToSerialize` (Root + Keep 오브젝트)에서 출발하여 **참조 체인을 추적**:

- `UClass::ReferenceTokenStream`을 사용한 토큰 기반 빠른 참조 순회
- 참조된 오브젝트의 `Unreachable` 플래그 해제 → 도달 가능 표시
- 클러스터 오브젝트는 클러스터 루트 단위로 일괄 처리
- 병렬 실행 지원 (`EFastReferenceCollectorOptions::Parallel`)

### 3.4 Sweep 단계 — GatherUnreachableObjects + Unhash + Purge

**GatherUnreachableObjects** (`GarbageCollection.cpp`):
- GUObjectArray 전체 스캔 → `Unreachable` 플래그가 남은 오브젝트 수집
- 클러스터 루트면 소속 오브젝트도 Unreachable 마킹

**UnhashUnreachableObjects**:
- 수집된 오브젝트에 `ConditionalBeginDestroy()` 호출
- 증분(incremental) 처리 지원 — 시간 제한 가능

**IncrementalPurgeGarbage** (`GarbageCollection.cpp:1446`):
1. 아직 Unhash 안 된 오브젝트 처리 (BeginDestroy)
2. `IsReadyForFinishDestroy()` 확인 후 `ConditionalFinishDestroy()` 호출
3. 준비 안 된 오브젝트는 `GGCObjectsPendingDestruction`에 추가 → 재시도
4. 모든 FinishDestroy 완료 후 `GAsyncPurge->BeginPurge()` → 실제 메모리 해제
5. 멀티스레드 삭제 지원 (`FAsyncPurge`)

---

## 4. GC Root 등록/해제 방식

### 4.1 직접 Root 관리

```cpp
// UObjectBaseUtility (인라인)
void AddToRoot()    { GUObjectArray.IndexToObject(InternalIndex)->SetRootSet(); }
void RemoveFromRoot() { GUObjectArray.IndexToObject(InternalIndex)->ClearRootSet(); }
bool IsRooted()     { return GUObjectArray.IndexToObject(InternalIndex)->IsRootSet(); }
```

- `FUObjectItem`의 `EInternalObjectFlags::RootSet` (1<<30) 비트 조작
- Root 오브젝트는 GC Mark 단계에서 **항상 도달 가능**으로 처리
- Root에 등록된 오브젝트는 `PendingKill` 마킹 불가 (`check(!IsRooted())`)

### 4.2 자동 Root 등록 — RF_MarkAsRootSet

`AddObject()` 시 `RF_MarkAsRootSet` 플래그 확인:

```cpp
if (ObjectFlags & RF_MarkAsRootSet) {
    InternalFlagsToSet |= EInternalObjectFlags::RootSet;
    ObjectFlags &= ~RF_MarkAsRootSet;
}
```

- 오브젝트 플래그에서 내부 플래그로 변환 후 원본 플래그 제거

### 4.3 FGCObject — Non-UObject의 GC 참여

비-UObject가 UObject를 참조할 때 GC에 알리는 메커니즘:

```
FGCObject 생성
  └─ UGCObjectReferencer::AddObject(this)
       └─ UGCObjectReferencer는 Root에 등록된 UObject
            → GC 순회 시 등록된 FGCObject::AddReferencedObjects() 호출
```

### 4.4 GC에서 Root 처리 흐름

`MarkObjectsAsUnreachable`에서:
1. Root 오브젝트 → Unreachable 마킹하지 않음
2. `ObjectsToSerialize`에 추가 → 참조 추적 시작점
3. Root 오브젝트가 참조하는 모든 오브젝트도 도달 가능으로 표시

---

## 5. BeginDestroy → FinishDestroy 소멸 시퀀스

### 5.1 전체 소멸 흐름

```
GC가 Unreachable 판정
  └─ ConditionalBeginDestroy()           ← UnhashUnreachableObjects에서 호출
       ├─ RF_BeginDestroyed 플래그 설정
       └─ BeginDestroy() 호출 (가상 함수)
            ├─ 링커 분리 (SetLinker(NULL, INDEX_NONE))
            ├─ 이름 제거 (LowLevelRename(NAME_None))
            └─ 외부 패키지 해제

  [시간 경과 — 비동기 리소스 해제 대기]

  IsReadyForFinishDestroy() == true
  └─ ConditionalFinishDestroy()          ← IncrementalPurgeGarbage에서 호출
       ├─ RF_FinishDestroyed 플래그 설정
       ├─ FinishDestroy() 호출 (가상 함수)
       │    └─ DestroyNonNativeProperties() — 비네이티브 프로퍼티 정리
       ├─ GUObjectArray.ResetSerialNumber() — WeakPtr 무효화
       └─ GUObjectArray.RemoveObjectFromDeleteListeners()

  [실제 메모리 해제]
  └─ GAsyncPurge — ~UObject() 소멸자 호출 + 메모리 반환
```

### 5.2 ConditionalBeginDestroy (`Obj.cpp:943`)

```cpp
bool UObject::ConditionalBeginDestroy() {
    if (!HasAnyFlags(RF_BeginDestroyed)) {
        SetFlags(RF_BeginDestroyed);
        BeginDestroy();               // 가상 함수 호출
        // Super::BeginDestroy() 호출 검증 (DebugBeginDestroyed 체크)
        return true;
    }
    return false;
}
```

- `RF_BeginDestroyed`로 중복 호출 방지
- `DebugBeginDestroyed` 배열로 `Super::BeginDestroy()` 호출 체인 검증

### 5.3 BeginDestroy (`Obj.cpp:758`)

```cpp
void UObject::BeginDestroy() {
    // RF_BeginDestroyed 미설정 시 Fatal (ConditionalBeginDestroy 통해서만 호출)
    SetLinker(NULL, INDEX_NONE);       // 링커 분리
    LowLevelRename(NAME_None);         // 이름 해시에서 제거
    SetExternalPackage(nullptr);       // 외부 패키지 해제
}
```

- 서브클래스에서 오버라이드하여 비동기 리소스 해제 시작 (GPU 펜스 등)
- 반드시 `Super::BeginDestroy()` 호출 필요 (미호출 시 Fatal)

### 5.4 IsReadyForFinishDestroy

- 기본 구현: `return true`
- 서브클래스 오버라이드: GPU 리소스 해제 펜스 완료 대기 등
- IncrementalPurgeGarbage가 반복 폴링하며 확인

### 5.5 ConditionalFinishDestroy (`Obj.cpp`)

```cpp
bool UObject::ConditionalFinishDestroy() {
    if (!HasAnyFlags(RF_FinishDestroyed)) {
        SetFlags(RF_FinishDestroyed);
        FinishDestroy();
        GUObjectArray.ResetSerialNumber(this);         // WeakPtr 무효화
        GUObjectArray.RemoveObjectFromDeleteListeners(this);
        return true;
    }
    return false;
}
```

### 5.6 FinishDestroy (`Obj.cpp:804`)

```cpp
void UObject::FinishDestroy() {
    check(!GetLinker());                // 링커 분리 확인
    DestroyNonNativeProperties();       // 비네이티브 프로퍼티 정리
}
```

### 5.7 최종 메모리 해제

- `GAsyncPurge`가 `~UObject()` C++ 소멸자 호출
- 메모리를 `GUObjectAllocator`에 반환
- 멀티스레드 삭제 지원 (`FAsyncPurge`)
- `GUObjectArray`의 슬롯은 재사용 가능 상태로 전환

---

## 분석 요약

| 항목 | 수치 |
|------|------|
| 분석한 파일 수 | 6개 (.h 3 + .cpp 3: UObjectGlobals, UObjectBase, Class, Obj, GarbageCollection, UObjectBaseUtility) |
| 핵심 함수 수 | 14개 (NewObject, StaticConstructObject_Internal, StaticAllocateObject, FObjectInitializer::PostConstructInit, CreateDefaultObject, GetDefaultObject, CollectGarbageInternal, MarkObjectsAsUnreachable, PerformReachabilityAnalysis, GatherUnreachableObjects, UnhashUnreachableObjects, IncrementalPurgeGarbage, ConditionalBeginDestroy, ConditionalFinishDestroy) |
| 불확실한 부분 | GUObjectAllocator의 내부 할당 전략 (풀/힙 구분 조건) 구현부 미확인. FAsyncPurge의 멀티스레드 삭제 구체적 구현 세부사항 미확인. PerformReachabilityAnalysisOnObjectsInternal의 토큰 기반 참조 순회 세부 로직은 FGCReferenceProcessor 템플릿 내부에 있어 전체 추적 미완료 |
| 저장 경로 | `docs/analysis/02_CoreUObject/02_UObject_생명주기.md` |
