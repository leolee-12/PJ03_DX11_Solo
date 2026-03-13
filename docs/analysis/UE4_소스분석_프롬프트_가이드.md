# UE4 소스코드 분석 프롬프트 가이드 (Claude Code 반자동)

---

## 사전 준비

분석 결과를 저장할 폴더 구조:

```
프로젝트루트/
├── UE4-4.27/Engine/       ← 정리된 UE4 소스
├── analysis/              ← 분석 결과 저장
│   ├── 01_Core/
│   ├── 02_CoreUObject/
│   ├── 03_Engine/
│   ├── 04_RHI/
│   ├── 05_D3D11RHI/
│   ├── 06_RenderCore/
│   ├── 07_Renderer/
│   ├── 08_PhysicsCore/
│   ├── 09_AnimationCore/
│   ├── 10_AIModule/
│   ├── 11_Navigation/
│   ├── 12_Slate/
│   ├── 13_UMG/
│   ├── 14_Editor/
│   ├── 15_Network/
│   └── Shaders/
└── CLAUDE.md
```

---

## 작업 목록

### 1순위: 엔진 기반

---

#### [1-1] Core — 구조 파악

- 대상: `Engine/Source/Runtime/Core/Public/` 의 .h 파일
- 저장: `analysis/01_Core/01_구조파악.md`

분석 항목:
1. 클래스/구조체 계층 구조를 트리 다이어그램으로
2. 각 클래스의 핵심 역할을 한 줄로
3. 핵심 매크로와 타입 별칭(typedef/using) 목록
4. 주요 하위 디렉토리별 역할 구분

영역별로 구분하여 정리:
- 메모리 관리 (할당자, 스마트 포인터)
- 컨테이너 (TArray, TMap, TSet 등)
- 델리게이트 시스템
- 문자열 처리 (FName, FString, FText)
- 수학 라이브러리 (FVector, FMatrix 등)
- 멀티스레딩 (FRunnable, TaskGraph 등)

---

#### [1-2] Core — 메모리 관리 심화

- 선행: [1-1] 결과 파일을 먼저 읽을 것
- 대상: `Engine/Source/Runtime/Core/` 메모리 관련 코드 (.h + .cpp)
- 저장: `analysis/01_Core/02_메모리관리.md`

분석 항목:
1. FMalloc 계열 할당자의 상속 구조와 각 할당자의 특성
2. TSharedPtr, TWeakPtr, TUniquePtr의 구현 방식
3. FMemory::Malloc() 호출부터 실제 할당까지의 호출 체인 (함수 시그니처 포함)
4. 커스텀 할당자가 어떻게 등록되고 선택되는지

---

#### [1-3] Core — 델리게이트 심화

- 선행: [1-1] 결과 파일을 먼저 읽을 것
- 대상: `Engine/Source/Runtime/Core/` 델리게이트 관련 코드
- 저장: `analysis/01_Core/03_델리게이트.md`

분석 항목:
1. DECLARE_DELEGATE 매크로가 실제로 어떤 코드를 생성하는지 전개 과정
2. 싱글캐스트 vs 멀티캐스트 vs 다이나믹 델리게이트의 구현 차이
3. 바인딩(BindRaw, BindUObject, BindSP 등)별 내부 저장 방식
4. 델리게이트 호출 시 실행 흐름

---

#### [1-4] Core — 컨테이너 심화

- 선행: [1-1] 결과 파일을 먼저 읽을 것
- 대상: `Engine/Source/Runtime/Core/` 컨테이너 관련 코드
- 저장: `analysis/01_Core/04_컨테이너.md`

분석 항목:
1. TArray의 메모리 할당 전략 (초기 크기, 증가 정책, Slack)
2. TMap의 해시 테이블 구현 (해시 함수, 충돌 처리, 리해시)
3. TSet의 내부 구조
4. 각 컨테이너에서 이터레이터가 어떻게 구현되는지
5. STL 컨테이너와의 설계 차이점

---

#### [1-5] Core — 수학 라이브러리 심화

- 선행: [1-1] 결과 파일을 먼저 읽을 것
- 대상: `Engine/Source/Runtime/Core/` 수학 관련 코드
- 저장: `analysis/01_Core/05_수학라이브러리.md`

분석 항목:
1. UE4의 좌표계 컨벤션 (왼손/오른손, 축 방향)과 DirectX 좌표계와의 관계
2. FMatrix의 행 우선/열 우선 저장 방식과 곱셈 순서 (operator* 구현부 확인)
3. FTransform의 벡터화 구현(TransformVectorized.h)과 비벡터화 구현의 차이 — 어떤 조건에서 분기되는지
4. SIMD 구현 구조 (VectorRegister 타입, UnrealMathSSE.h의 주요 함수, 플랫폼 분기 방식)
5. FPerspectiveMatrix, FOrthoMatrix의 생성자 파라미터와 DX11 투영 행렬과의 대응 관계
6. FVector::ForwardVector 등 정적 상수의 축 매핑 (X=Forward, Y=Right, Z=Up 확인)

DX11에서 직접 투영 행렬과 뷰 행렬을 만들 때 주의해야 할 점을 정리할 것.

---

#### [1-6] Core — 문자열 시스템 심화

- 선행: [1-1] 결과 파일을 먼저 읽을 것
- 대상: `Engine/Source/Runtime/Core/` 문자열 관련 코드
- 저장: `analysis/01_Core/06_문자열시스템.md`

분석 항목:
1. FName의 전역 해시 테이블 구조 — 엔트리 저장 방식, 해시 함수, 슬롯 탐색, 동시성 처리
2. FName 생성 시 문자열 등록 흐름 (FName 생성자 → 해시 계산 → 테이블 탐색/삽입)
3. FName 비교가 O(1)인 이유 — FNameEntryId의 내부 구조와 비교 연산자 구현
4. FString ↔ FName ↔ FText 간 변환 비용과 각 변환 함수
5. FName의 Number 필드 역할 (동일 이름 인스턴스 구분, 예: "Weapon_0", "Weapon_1")
6. FName의 메모리 특성 — sizeof(FName), 전역 테이블의 메모리 회수 가능 여부

string 기반 식별자를 사용하는 시스템(AI 상태 이름 등)에서 FName 방식의 해시 기반 식별자로 전환할 때의 설계 고려사항도 정리할 것.

---

#### [1-7] Core — 프레임워크 적용

- 선행: [1-1] ~ [1-6] 결과 파일을 모두 읽을 것
- 저장: `analysis/01_Core/07_프레임워크_적용.md`

분석 항목:
1. UE4 Core의 메모리 관리 패턴 중 DX11 학습용 소규모 엔진에 적용할 만한 것
2. UE4 델리게이트와 직접 구현한 델리게이트의 구조적 차이 가능성
3. TArray의 메모리 증가 정책과 std::vector 비교 시 trade-off
4. 프레임워크의 참조 카운팅 구조에 대한 UE4 관점의 개선 제안

실제 적용 가능한 수준의 구체적인 제안을 할 것.

---

#### [2-1] CoreUObject — 구조 파악

- 대상: `Engine/Source/Runtime/CoreUObject/Public/` 의 .h 파일
- 저장: `analysis/02_CoreUObject/01_구조파악.md`

분석 항목:
1. UObject 클래스의 상속 계층 (UObjectBase → UObjectBaseUtility → UObject)
2. UClass, UProperty, UFunction 등 리플렉션 타입의 관계
3. 가비지 컬렉션 관련 클래스/구조체
4. 직렬화(Serialization) 관련 인터페이스
5. 패키지 시스템 관련 클래스

---

#### [2-2] CoreUObject — UObject 생명주기

- 선행: [2-1] 결과 파일을 먼저 읽을 것
- 대상: `Engine/Source/Runtime/CoreUObject/` (.h + .cpp)
- 저장: `analysis/02_CoreUObject/02_UObject_생명주기.md`

분석 항목:
1. NewObject<T>() 호출부터 메모리 할당, 생성자 호출까지의 전체 흐름
2. CDO(Class Default Object)가 생성되는 시점과 역할
3. GC가 UObject를 수집하는 조건과 Mark-Sweep 과정
4. GC Root 등록/해제 방식
5. BeginDestroy → FinishDestroy 소멸 시퀀스

---

#### [2-3] CoreUObject — 리플렉션 시스템

- 선행: [2-1] 결과 파일을 먼저 읽을 것
- 대상: `Engine/Source/Runtime/CoreUObject/` 리플렉션 관련 코드
- 저장: `analysis/02_CoreUObject/03_리플렉션.md`

분석 항목:
1. UCLASS(), UPROPERTY(), UFUNCTION() 매크로가 생성하는 코드
2. UnrealHeaderTool이 만드는 .generated.h 파일의 구조
3. 런타임에 UClass로부터 프로퍼티/함수 정보를 조회하는 흐름
4. 리플렉션 정보가 에디터의 디테일 패널과 어떻게 연결되는지

---

#### [3-1] Engine — 구조 파악

- 대상: `Engine/Source/Runtime/Engine/Classes/` 의 .h 파일
- 저장: `analysis/03_Engine/01_구조파악.md`

분석 항목:
1. AActor의 상속 계층과 주요 파생 클래스
2. UActorComponent의 상속 계층과 주요 컴포넌트 종류
3. UWorld → ULevel → AActor → UActorComponent 소유 관계
4. AGameModeBase, APlayerController, APawn의 역할과 관계
5. UGameInstance의 역할

---

#### [3-2] Engine — Actor-Component 생명주기

- 선행: [3-1] 결과 파일을 먼저 읽을 것
- 대상: `Engine/Source/Runtime/Engine/` (.h + .cpp)
- 저장: `analysis/03_Engine/02_Actor_Component_생명주기.md`

분석 항목:
1. SpawnActor 호출 시 일어나는 일 (메모리 할당 → 컴포넌트 등록 → BeginPlay)
2. AActor::BeginPlay() → UActorComponent::BeginPlay() 호출 순서
3. Tick 함수의 호출 흐름 (TickGroup별 순서)
4. AActor::Destroy() 호출 시 컴포넌트 해제 순서
5. Deferred Spawn과 즉시 Spawn의 차이

---

#### [3-3] Engine — 게임 루프

- 선행: [3-1] 결과 파일을 먼저 읽을 것
- 대상: `Engine/Source/Runtime/Engine/` + `Engine/Source/Runtime/Launch/`
- 저장: `analysis/03_Engine/03_게임루프.md`

분석 항목:
1. WinMain → FEngineLoop::PreInit → Init → Tick 전체 시퀀스
2. FEngineLoop::Tick() 한 프레임에서 일어나는 일의 순서 (입력 처리 → 월드 틱 → 렌더링 → 프레임 동기화)
3. UWorld::Tick()이 Actor들의 Tick을 어떻게 호출하는지
4. 렌더링 스레드와 게임 스레드의 분리 지점

---

### 2순위: 렌더링 파이프라인

---

#### [4-1] RHI — 구조 파악

- 대상: `Engine/Source/Runtime/RHI/Public/` 의 .h 파일
- 저장: `analysis/04_RHI/01_구조파악.md`

분석 항목:
1. FDynamicRHI 인터페이스의 주요 가상 함수 목록을 카테고리별로 (리소스 생성, 렌더 상태, Draw Call, 컴퓨트 등)
2. RHI 리소스 타입 계층 (FRHITexture, FRHIBuffer, FRHIShader 등)
3. FRHICommandList의 역할과 구조
4. ERHIFeatureLevel 정의와 각 레벨의 의미

---

#### [4-2] RHI — 추상화 설계 분석

- 선행: [4-1] 결과 파일을 먼저 읽을 것
- 대상: `Engine/Source/Runtime/RHI/` + `Engine/Source/Runtime/Windows/D3D11RHI/`
- 저장: `analysis/04_RHI/02_추상화_설계.md`

분석 항목:
1. FDynamicRHI 인터페이스를 D3D11RHI가 어떻게 구현하는지 (RHI 추상 함수 → D3D11 구현 매핑 예시 3개 이상)
2. RHI 리소스(FRHITexture 등)와 실제 D3D11 리소스(ID3D11Texture2D 등)의 래핑 구조
3. RHICreateTexture2D() 호출이 D3D11의 CreateTexture2D()에 도달하기까지의 전체 호출 체인
4. 이 추상화 구조의 장단점

---

#### [5-1] D3D11RHI — 구조 파악

- 대상: `Engine/Source/Runtime/Windows/D3D11RHI/` 의 모든 .h 파일
- 저장: `analysis/05_D3D11RHI/01_구조파악.md`

분석 항목:
1. FD3D11DynamicRHI 클래스의 멤버 변수와 주요 함수 목록
2. D3D11 디바이스/컨텍스트 초기화 구조
3. 리소스 관련 클래스들 (FD3D11Texture2D, FD3D11VertexBuffer 등)
4. 파일별 역할 구분 (어떤 파일이 어떤 기능을 담당하는지)

---

#### [5-2] D3D11RHI — 디바이스 초기화 흐름

- 선행: [5-1] 결과 파일을 먼저 읽을 것
- 대상: `Engine/Source/Runtime/Windows/D3D11RHI/` (.h + .cpp)
- 저장: `analysis/05_D3D11RHI/02_디바이스_초기화.md`

분석 항목:
IDXGIFactory → D3D11CreateDevice → SwapChain 생성까지의 전체 흐름.
함수 시그니처, DX11 API 호출, 에러 처리를 포함.
내가 DX11로 직접 구현할 때 참고할 수 있도록 핵심 API 호출 순서 정리.

---

#### [5-3] D3D11RHI — 버텍스버퍼 생성 흐름

- 선행: [5-1] 결과 파일을 먼저 읽을 것
- 대상: `Engine/Source/Runtime/Windows/D3D11RHI/` (.h + .cpp)
- 저장: `analysis/05_D3D11RHI/03_버텍스버퍼_생성.md`

분석 항목:
CreateBuffer → IASetVertexBuffers까지의 전체 흐름.
함수 시그니처, DX11 API 호출, 에러 처리를 포함.

---

#### [5-4] D3D11RHI — 텍스처 생성 흐름

- 선행: [5-1] 결과 파일을 먼저 읽을 것
- 대상: `Engine/Source/Runtime/Windows/D3D11RHI/` (.h + .cpp)
- 저장: `analysis/05_D3D11RHI/04_텍스처_생성.md`

분석 항목:
CreateTexture2D → SRV 생성까지의 전체 흐름.
함수 시그니처, DX11 API 호출, 에러 처리를 포함.

---

#### [5-5] D3D11RHI — 셰이더 바인딩 흐름

- 선행: [5-1] 결과 파일을 먼저 읽을 것
- 대상: `Engine/Source/Runtime/Windows/D3D11RHI/` (.h + .cpp)
- 저장: `analysis/05_D3D11RHI/05_셰이더_바인딩.md`

분석 항목:
VS/PS 컴파일 → 바인딩 → 상수버퍼 설정까지의 전체 흐름.
함수 시그니처, DX11 API 호출, 에러 처리를 포함.

---

#### [5-6] D3D11RHI — 렌더타겟 및 DrawCall 흐름

- 선행: [5-1] 결과 파일을 먼저 읽을 것
- 대상: `Engine/Source/Runtime/Windows/D3D11RHI/` (.h + .cpp)
- 저장: `analysis/05_D3D11RHI/06_렌더타겟_DrawCall.md`

분석 항목:
1. RTV 생성 → OMSetRenderTargets까지의 렌더타겟 설정 흐름
2. DrawIndexed 호출 전후의 전체 상태 설정 흐름
함수 시그니처, DX11 API 호출, 에러 처리를 포함.

---

#### [5-7] D3D11RHI vs D3D12RHI 비교

- 선행: [5-2] ~ [5-6] 결과 파일을 모두 읽을 것
- 대상: `Engine/Source/Runtime/Windows/D3D11RHI/` + `Engine/Source/Runtime/D3D12RHI/`
- 저장: `analysis/05_D3D11RHI/07_D3D11_vs_D3D12.md`

비교 대상:
1. 디바이스 초기화
2. 버퍼 생성
3. Draw Call 실행

각 항목에서:
- D3D11의 코드 흐름 요약
- D3D12의 코드 흐름 요약
- 핵심 차이점과 그 이유

---

#### [6-1] RenderCore — 구조 파악

- 대상: `Engine/Source/Runtime/RenderCore/Public/` 의 .h 파일
- 저장: `analysis/06_RenderCore/01_구조파악.md`

분석 항목:
1. 렌더 커맨드(FRenderCommand) 시스템 구조
2. 셰이더 파라미터 바인딩 관련 클래스
3. FGlobalShader와 FMaterialShader의 관계
4. 렌더링 스레드로 작업을 넘기는 메커니즘 (ENQUEUE_RENDER_COMMAND 매크로 등)

---

#### [7-1] Renderer — 구조 파악

- 대상: `Engine/Source/Runtime/Renderer/Private/` 의 .h 파일 (Renderer는 대부분 Private)
- 저장: `analysis/07_Renderer/01_구조파악.md`

분석 항목:
1. FSceneRenderer 상속 계층 (FDeferredShadingSceneRenderer 등)
2. 렌더 패스 종류와 실행 순서
3. FScene, FPrimitiveSceneProxy, FSceneView의 역할
4. 라이트 관련 클래스 구조

---

#### [7-2] Renderer — 디퍼드 렌더링 파이프라인

- 선행: [7-1] 결과 파일을 먼저 읽을 것
- 대상: `Engine/Source/Runtime/Renderer/Private/` (.h + .cpp)
- 저장: `analysis/07_Renderer/02_디퍼드렌더링.md`

분석 항목:
FDeferredShadingSceneRenderer::Render() 함수의 실행 흐름을 추적.
한 프레임이 렌더링되는 전체 과정을 순서대로:
1. 가시성 판정 (Visibility)
2. Base Pass (GBuffer 생성)
3. 라이팅 패스
4. 반투명 패스
5. 포스트프로세스

각 패스에서 어떤 RHI 함수가 호출되는지, 어떤 렌더타겟에 그리는지를 포함.

---

### 3순위: 게임 시스템

---

#### [8-1] PhysicsCore — 구조 파악

- 대상: `Engine/Source/Runtime/PhysicsCore/` 의 .h 파일
- 저장: `analysis/08_PhysicsCore/01_구조파악.md`

분석 항목:
1. 물리 인터페이스 추상화 구조 (PhysX 래핑 방식)
2. FBodyInstance의 역할과 구조
3. 콜리전 채널/프로필 시스템
4. 물리 시뮬레이션이 게임 루프에서 어떤 시점에 실행되는지

---

#### [9-1] AnimationCore — 구조 파악

- 대상: `Engine/Source/Runtime/AnimationCore/` 의 .h 파일
- 저장: `analysis/09_AnimationCore/01_구조파악.md`

분석 항목:
1. 애니메이션 관련 핵심 타입 (FBoneReference, FTransform 등)
2. 스켈레톤 구조 (본 계층, 소켓)
3. 애니메이션 블렌딩 관련 구조체
4. AnimGraph 노드의 기본 인터페이스

---

#### [10-1] AIModule — 구조 파악

- 대상: `Engine/Source/Runtime/AIModule/Public/` 의 .h 파일
- 저장: `analysis/10_AIModule/01_구조파악.md`

분석 항목:
1. AAIController의 역할과 APawn과의 관계
2. 비헤이비어 트리 (UBehaviorTree, UBTNode 계층 구조)
3. 블랙보드 (UBlackboardComponent, UBlackboardData)
4. EQS (Environment Query System) 기본 구조
5. AI Perception 시스템

---

#### [11-1] NavigationSystem — 구조 파악

- 대상: `Engine/Source/Runtime/NavigationSystem/` 의 .h 파일
- 저장: `analysis/11_Navigation/01_구조파악.md`

분석 항목:
1. UNavigationSystemV1의 역할
2. ARecastNavMesh와 네비메시 생성 구조
3. 경로 탐색 (FindPathSync/Async) 호출 흐름
4. NavMesh 업데이트가 동적 오브젝트와 어떻게 연동되는지

---

### 4순위: UI/에디터

---

#### [12-1] Slate — 구조 파악

- 대상: `Engine/Source/Runtime/SlateCore/Public/` + `Engine/Source/Runtime/Slate/Public/` 의 .h 파일
- 저장: `analysis/12_Slate/01_구조파악.md`

분석 항목:
1. SWidget 상속 계층과 주요 위젯 타입 (SButton, STextBlock 등)
2. 위젯 생성 문법 (SNew, SAssignNew 매크로)
3. Slate의 레이아웃 시스템 (슬롯, 정렬, 패딩)
4. 이벤트/입력 처리 흐름 (OnClicked, OnTextChanged 등)
5. Slate 렌더링이 RHI와 어떻게 연결되는지 (SlateRHIRenderer)
6. ImGui(Immediate Mode)와 비교 시 Slate(Retained Mode)의 구조적 차이

---

#### [13-1] UMG — 구조 파악

- 대상: `Engine/Source/Runtime/UMG/Public/` 의 .h 파일
- 저장: `analysis/13_UMG/01_구조파악.md`

분석 항목:
1. UWidget → UUserWidget 계층 구조
2. UMG 위젯과 Slate 위젯의 관계 (UMG가 Slate를 래핑하는 구조)
3. UWidgetComponent로 3D 공간에 UI를 배치하는 구조
4. 애니메이션/바인딩 시스템

---

#### [14-1] Editor — 구조 파악

- 대상: `Engine/Source/Editor/` 에서 UnrealEd, MainFrame, LevelEditor, ContentBrowser, PropertyEditor, DetailCustomizations 모듈의 .h 파일
- 저장: `analysis/14_Editor/01_구조파악.md`

분석 항목:
1. 에디터 메인 프레임의 탭/패널 구조
2. 콘텐츠 브라우저의 에셋 표시/필터링 구조
3. 프로퍼티 에디터가 UObject의 리플렉션 정보를 읽어서 디테일 패널을 생성하는 흐름
4. 커스텀 디테일 패널을 만드는 패턴 (IDetailCustomization)
5. ImGui로 비슷한 기능을 구현한다면 어떤 구조를 참고해야 하는지

---

#### [14-2] Editor — 에디터 기능 요구사항 정리

- 선행: [14-1] 결과 파일을 먼저 읽을 것
- 대상: Engine/Source/Editor/ 전체 모듈 목록 + Engine/Source/Developer/ 전체 모듈 목록
- 저장: analysis/14_Editor/02_에디터_기능_요구사항.md

분석 항목:
1. Source/Editor/와 Source/Developer/의 전체 모듈을 기능 카테고리별로 분류 (뷰포트, 에셋 편집, 노드 그래프, 타임라인, 브러시 도구, 프로파일링, 디버깅 등)
2. 각 카테고리에서 UE4가 제공하는 핵심 기능 목록
3. 게임 에디터로서 필수/핵심 편의/고급으로 우선순위 분류
4. 각 기능을 ImGui로 구현할 때의 접근 방식
5. 프레임워크에서 어떤 데이터를 에디터에 노출해야 하는지

---

#### [14-3] Editor — 노드 그래프 에디터 구조

- 선행: [14-1] 결과 파일을 먼저 읽을 것
- 대상: Engine/Source/Editor/ 에서 GraphEditor, MaterialEditor, BlueprintGraph, Kismet 모듈의 .h 파일
- 저장: analysis/14_Editor/03_노드그래프.md

분석 항목:
1. SGraphEditor의 핵심 구조 (노드, 핀, 연결)
2. UEdGraphNode → UEdGraphPin 관계
3. 노드 생성/삭제/연결의 흐름
4. 머티리얼 에디터가 GraphEditor를 어떻게 활용하는지
5. ImGui로 노드 에디터를 만들 때 참고할 구조

---

#### [14-4] Editor — 타임라인/시퀀서 구조

- 선행: [14-1] 결과 파일을 먼저 읽을 것
- 대상: Engine/Source/Editor/ 에서 Sequencer, SequencerWidgets, CurveEditor 모듈의 .h 파일
- 저장: analysis/14_Editor/04_타임라인_시퀀서.md

분석 항목:
1. 시퀀서의 트랙/섹션/키프레임 데이터 모델
2. 타임라인 UI 위젯 구조
3. 커브 에디터의 키프레임 편집 구조
4. ImGui로 타임라인 UI를 만들 때 참고할 구조

---

#### [14-5] Editor — 에셋 에디터 패턴

- 선행: [14-1] 결과 파일을 먼저 읽을 것
- 대상: Engine/Source/Editor/ 에서 StaticMeshEditor, SkeletalMeshEditor, TextureEditor 모듈 + Engine/Source/Developer/AssetTools/ 의 .h 파일
- 저장: analysis/14_Editor/05_에셋에디터.md

분석 항목:
1. FAssetEditorToolkit을 상속하는 에셋 에디터의 공통 패턴
2. 프리뷰 뷰포트 + 디테일 패널 조합 구조
3. AssetTools의 에셋 생성/임포트/익스포트 프레임워크
4. 새로운 에셋 타입의 에디터를 추가하는 흐름

---

### 5순위: 네트워크

---

#### [15-1] Network — 구조 파악

- 대상: `Engine/Source/Runtime/Net/` + `Engine/Source/Runtime/Sockets/` 의 .h 파일
- 저장: `analysis/15_Network/01_구조파악.md`

분석 항목:
1. FSocket 추상화와 플랫폼별 구현 구조
2. UNetDriver → UNetConnection → UChannel 계층
3. Actor Replication 흐름 (서버에서 프로퍼티 변경 → 클라이언트 동기화)
4. RPC (Remote Procedure Call) 구현 구조
5. 패킷 직렬화/역직렬화 과정

---

### 셰이더 분석 (렌더링 파이프라인 분석 완료 후)

---

#### [S-1] 셰이더 — 공통 기반

- 대상: `Engine/Shaders/Private/` 의 Common.ush, Definitions.usf, BRDF.ush
- 저장: `analysis/Shaders/01_공통셰이더.md`

분석 항목:
1. 전역으로 정의되는 구조체 (FVertexFactoryInput, FMaterialAttributes 등)
2. 공용 유틸리티 함수 목록과 각각의 역할
3. #define으로 정의되는 주요 상수와 조건부 컴파일 분기
4. 다른 셰이더 파일에서 이 파일들을 어떻게 include해서 쓰는지

---

#### [S-2] 셰이더 — Base Pass

- 선행: [S-1] 결과 파일을 먼저 읽을 것
- 대상: `Engine/Shaders/Private/` 의 BasePassPixelShader.usf, BasePassVertexShader.usf, BasePassVertexCommon.ush, BasePassCommon.ush
- 저장: `analysis/Shaders/02_BasePass.md`

분석 항목:
1. 버텍스 셰이더 입력 → 출력 구조
2. 픽셀 셰이더가 GBuffer에 쓰는 데이터와 그 레이아웃
3. 머티리얼 파라미터가 어떻게 셰이더에 전달되는지
4. 라이팅 모델 분기 (Default Lit, Unlit 등)

---

#### [S-3] 셰이더 — Deferred Lighting

- 선행: [S-1] 결과 파일을 먼저 읽을 것
- 대상: `Engine/Shaders/Private/` 의 DeferredLightingCommon.ush, DeferredShadingCommon.ush
- 저장: `analysis/Shaders/03_DeferredLighting.md`

분석 항목:
1. GBuffer 읽기/디코딩 함수들
2. 라이트 유형별 (Directional, Point, Spot) 처리 분기
3. 그림자 계산 함수의 입력/출력
4. PBR 라이팅 계산 흐름 (BRDF 함수 호출 포함)

---

### 종합

---

#### [F-1] 종합 아키텍처 정리

- 선행: 모든 모듈의 1단계 결과 파일을 읽을 것
- 저장: `analysis/00_종합_아키텍처.md`

분석 항목:
1. 모듈 간 의존 관계 (어떤 모듈이 어떤 모듈에 의존하는지)
2. 한 프레임에서 데이터가 흐르는 경로 (입력 → 게임 로직 → 물리 → 애니메이션 → 렌더링 → 출력)
3. 스레드 경계 (게임 스레드 / 렌더 스레드 / RHI 스레드)
4. 프레임워크(Management → Scene → Layer → GameObject → Component)와 UE4 구조의 대응 관계
