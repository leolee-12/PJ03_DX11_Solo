# 참고프로젝트2 아키텍처 분석

> Dragon Ball FighterZ 모작 — DirectX 11 기반 3D 격투 게임

---

## 1. 디렉토리 구조

```
참고프로젝트2/
├── Framework.sln                  # VS2022 솔루션 (4개 프로젝트)
├── Engine/                        # 엔진 DLL (ENGINE_DLL)
│   ├── Default/Engine.vcxproj
│   ├── Public/                    # 헤더 (외부 공개 인터페이스)
│   └── Private/                   # 구현부
├── Renderer/                      # 렌더러 DLL (RENDERER_DLL)
│   ├── Default/Renderer.vcxproj
│   ├── Public/
│   └── Private/
├── Client/                        # 실행파일 (게임 로직)
│   ├── Default/Client.vcxproj     # WinMain 진입점
│   ├── Public/                    # 헤더
│   └── Private/                   # 구현부
├── AssimpSaveAndLoad/             # 모델 변환 도구 (별도 프로젝트)
│   └── AssimpSaveAndLoad.vcxproj
├── EngineSDK/Inc/                 # 외부 라이브러리 헤더
│   ├── DirectXTK/                 # DirectX Tool Kit
│   ├── Effects11/                 # Effects11 프레임워크
│   ├── Fmod/                      # FMOD 사운드
│   └── assimp/                    # Assimp 모델 로더
└── UpdateLib.bat                  # 라이브러리 업데이트 배치
```

### 프로젝트별 역할

| 프로젝트 | 출력 | 역할 |
|----------|------|------|
| **Engine** | DLL | 코어 프레임워크: 오브젝트/컴포넌트/레벨 관리, 입력, 타이머, 충돌, 애니메이션 |
| **Renderer** | DLL | 디퍼드 렌더링, 라이트, 렌더 타겟, 글로우/디스토션 후처리 |
| **Client** | EXE | 게임 콘텐츠: 캐릭터, UI, 레벨, 이펙트, QTE, 전투 시스템 |
| **AssimpSaveAndLoad** | EXE | FBX→바이너리 모델 변환 오프라인 도구 |

---

## 2. 핵심 클래스 목록

### Engine (코어)

| 클래스 | 설명 |
|--------|------|
| `CBase` | 모든 클래스의 루트. 레퍼런스 카운팅(AddRef/Release) |
| `CGameInstance` | **엔진 파사드 싱글톤** — 모든 매니저 접근 창구 |
| `CGameObject` | 게임 오브젝트 추상 베이스. 컴포넌트 맵 보유. Clone 패턴 |
| `CContainerObject` | 파트 오브젝트를 포함하는 컨테이너 (무기 등 결합체) |
| `CPartObject` | 부모 행렬에 종속된 하위 오브젝트 (장비 파츠) |
| `CComponent` | 컴포넌트 추상 베이스. Clone 패턴 |
| `CTransform` | 위치/회전/스케일 월드 행렬 관리 |
| `CModel` | 3D 모델 (메시+본+애니메이션). 바이너리 로드 지원 |
| `CMesh` | 단일 메시(VB/IB). 본 가중치 포함 |
| `CBone` | 본 계층 구조 노드 |
| `CAnimation` | 애니메이션 클립 (채널 기반, 사운드 이벤트 지원) |
| `CChannel` | 단일 본의 키프레임 보간 |
| `CShader` | Effects11 셰이더 래퍼 (패스 기반 렌더) |
| `CTexture` | 텍스처 SRV 컨테이너 (멀티 텍스처) |
| `CVIBuffer` | 버텍스/인덱스 버퍼 추상 베이스 |
| `CVIBuffer_Rect` | 사각형 버퍼 (UI, 후처리 풀스크린 쿼드) |
| `CVIBuffer_Instancing` | GPU 인스턴싱 버퍼 |
| `CVIBuffer_Point_Instancing` | 포인트 파티클 인스턴싱 |
| `CVIBuffer_Trail_Rect` | 트레일 이펙트 버퍼 |
| `CCamera` | 카메라 추상 베이스 (FOV/Near/Far) |
| `CCollider` | 충돌체 컴포넌트 (AABB/OBB/Sphere) |
| `CBounding` / `CBounding_AABB` / `CBounding_OBB` / `CBounding_Sphere` | 바운딩 볼륨 |
| `CLevel` | 레벨(씬) 추상 베이스 |
| `CLayer` | 오브젝트 리스트 컨테이너 |
| `CLevel_Manager` | 레벨 전환 관리 |
| `CObject_Manager` | 프로토타입 저장소 + 레이어 기반 오브젝트 관리 |
| `CComponent_Manager` | 컴포넌트 프로토타입 저장소 |
| `CCollider_Manager` | 충돌 그룹 관리 및 충돌 검사 |
| `CGraphic_Device` | D3D11 디바이스/스왑체인/백버퍼 초기화 |
| `CInput_Device` | DirectInput 키보드/마우스 |
| `CKey_Manager` | 키 Down/Up/Pressing 상태 추적 |
| `CPipeLine` | View/Proj 행렬 + 역행렬 + 섀도우 카메라 행렬 저장 |
| `CTimer` / `CTimer_Manager` | 델타 타임 계산, 슬로우 모션 |
| `CThreadPool` | 멀티스레드 태스크 큐 |
| `CFile_Manager` | 오브젝트/카메라 포인트/이펙트 직렬화 |
| `CFont_Manager` / `CCustomFont` | SpriteFont 텍스트 렌더링 |
| `CFrustum` | 뷰 프러스텀 컬링 |
| `CSound_Manager` | FMOD 사운드 (그룹/카테고리 볼륨 관리) |

### Renderer (렌더링)

| 클래스 | 설명 |
|--------|------|
| `CRenderInstance` | **렌더러 파사드 싱글톤** — Renderer+Light+Target+Picking 접근 |
| `CRenderer` | 렌더 파이프라인 (26개 렌더 그룹, 디퍼드+포워드 하이브리드) |
| `CLobby_Renderer` | 로비 전용 렌더러 (별도 렌더 그룹) |
| `CRenderTarget` | 단일 렌더 타겟 텍스처 (RT 생성/바인드/클리어) |
| `CTarget_Manager` | MRT(Multi Render Target) 관리 |
| `CLight` / `CLight_Manager` | 라이트 객체 및 카테고리별 라이트 관리 |
| `CPicking` | 월드 공간 피킹 |

### Client (게임 콘텐츠)

| 클래스 | 설명 |
|--------|------|
| `CMainApp` | 게임 초기화/갱신/렌더 총괄 |
| `CCharacter` | **격투 캐릭터 베이스** — 커맨드 입력, 중력, 스턴, 콤보, QTE 등 |
| `CPlay_Goku` / `CPlay_Frieza` / `CPlay_Hit` / `CPlay_21` | 개별 캐릭터 구현 |
| `CAttackObject` | 공격 판정 오브젝트 (히트박스, 데미지, 넉백) |
| `CAttackObject_Chase/Energy/Grab/Ranged/Reflect/CommandGrab` | 공격 유형별 특화 |
| `CEffect` / `CEffect_*` | 이펙트 시스템 (Blend/NoneLight/ZNone/Overlap/Animation) |
| `CEffect_Layer` / `CEffect_Manager` | 이펙트 레이어 및 매니저 |
| `CParticle` / `CParticle_Manager` / `CParticle_*` | 파티클 시스템 |
| `CUIObject` | UI 추상 베이스 (직교 투영, 애니메이션) |
| `CUI_*` (HpGauge, Combo, Timer, Skill 등) | 배틀 UI 요소들 |
| `CBattleInterface` | 전투 인터페이스 총괄 |
| `CLevel_Logo/Lobby/Loading/CharaSelect/VS/GamePlay` | 게임 레벨들 |
| `CLoader` | 비동기 리소스 로딩 (스레드풀 활용) |
| `CMain_Camera` / `CMain_Camera_Lobby` / `CVirtual_Camera` | 카메라 구현체들 |
| `CImgui_Manager` / `CIMGUI_*_Tab` | 에디터 도구 (ImGui 기반) |
| `CQTE_Manager` / `CQTE_*` | QTE(Quick Time Event) 시스템 |
| `CMap_Manager` / `CSpaceStage` / `CVolcano_Stage` | 맵/스테이지 관리 |
| `CFrameEvent_Manager` | 애니메이션 키프레임 이벤트 |
| `CSubTitle_Manager` | 자막 시스템 |

---

## 3. 클래스 상속 계층도

```
CBase (레퍼런스 카운팅)
├── CGameInstance [Singleton]
├── CGraphic_Device
├── CInput_Device / CKey_Manager
├── CPipeLine
├── CTimer / CTimer_Manager
├── CLevel_Manager
├── CObject_Manager
├── CComponent_Manager
├── CCollider_Manager
├── CThreadPool
├── CFile_Manager
├── CFont_Manager / CCustomFont
├── CFrustum
├── CSound_Manager
├── CAnimation
├── CBone
├── CChannel
│
├── CLevel (추상)
│   ├── CLevel_Logo
│   ├── CLevel_Lobby
│   ├── CLevel_Loading
│   ├── CLevel_Chara_Select
│   ├── CLevel_VS
│   └── CLevel_GamePlay
│
├── CLayer
│
├── CComponent (추상)
│   ├── CTransform
│   ├── CShader
│   ├── CTexture
│   ├── CModel
│   ├── CMesh
│   ├── CCollider
│   ├── CVIBuffer (추상)
│   │   ├── CVIBuffer_Rect
│   │   ├── CVIBuffer_Instancing
│   │   ├── CVIBuffer_Point_Instancing
│   │   ├── CVIBuffer_Line_Rect
│   │   └── CVIBuffer_Trail_Rect
│   └── CBounding (추상)
│       ├── CBounding_AABB
│       ├── CBounding_OBB
│       └── CBounding_Sphere
│
├── CGameObject (추상)
│   ├── CCamera (추상)
│   │   ├── CMain_Camera
│   │   ├── CMain_Camera_Lobby
│   │   ├── CCharaSelect_Camera
│   │   ├── CLoading_Camera
│   │   ├── CShadow_Camera
│   │   └── CVirtual_Camera
│   │
│   ├── CContainerObject (추상)  ← 파트 오브젝트 컨테이너
│   │
│   ├── CPartObject (추상)  ← 부모 행렬 종속
│   │
│   ├── CCharacter  ← 격투 캐릭터 베이스
│   │   ├── CPlay_Goku
│   │   ├── CPlay_Frieza
│   │   ├── CPlay_Hit
│   │   └── CPlay_21
│   │
│   ├── CAttackObject  ← 공격 판정
│   │   ├── CAttackObject_Chase
│   │   ├── CAttackObject_Energy
│   │   ├── CAttackObject_Grab
│   │   ├── CAttackObject_CommandGrab
│   │   ├── CAttackObject_Ranged
│   │   └── CAttackObject_Reflect
│   │
│   ├── CUIObject (추상)  ← UI 베이스
│   │   ├── CUI_HpGauge / CUI_HpPanel / CUI_SubHpGauge
│   │   ├── CUI_Combo / CUI_ComboNumber / CUI_ComboFont
│   │   ├── CUI_Timer / CUI_Skill / CUI_SkillGauge
│   │   └── CUI_* (수십 개 UI 요소)
│   │
│   ├── CEffect  ← 이펙트 베이스
│   │   ├── CEffect_NoneLight
│   │   ├── CEffect_Blend
│   │   ├── CEffect_ZNone
│   │   ├── CEffect_Overlap
│   │   └── CEffect_Animation
│   │
│   ├── CParticle / CParticle_*
│   ├── CBoneEffectObject
│   ├── CLine_Draw
│   ├── CModel_Preview
│   ├── CFallingStar
│   │
│   ├── Lobby 오브젝트: CLobby_Goku, CLobby_Frieza, CLobby_Krillin, ...
│   ├── Space맵: CSpaceStage, CSpaceGround, CSpaceRock, CSpaceSky, ...
│   ├── Volcano맵: CVolcano_Stage, CVolcano_Ground, CVolcano_Lava_*, ...
│   │
│   ├── QTE: CQTE_Continuous_Attack, CQTE_Hit, CQTE_Same_Grab, ...
│   └── Loading: CLoading_GodDragon, COpening_Model, CCharaSelect_Model
│
├── [Renderer 네임스페이스]
│   ├── CRenderInstance [Singleton]
│   ├── CRenderer
│   ├── CLobby_Renderer
│   ├── CRenderTarget
│   ├── CTarget_Manager
│   ├── CLight
│   ├── CLight_Manager
│   └── CPicking
│
└── [Client 매니저]
    ├── CMainApp
    ├── CLoader
    ├── CImgui_Manager / CIMGUI_*_Tab
    ├── CUI_Manager
    ├── CEffect_Manager / CEffect_Layer
    ├── CParticle_Manager
    ├── CQTE_Manager
    ├── CMap_Manager
    ├── CFrameEvent_Manager
    └── CSubTitle_Manager
```

---

## 4. 프로그램 실행 흐름

### 4.1 진입점 → 초기화

```
wWinMain()  [Client.cpp]
  │
  ├── MyRegisterClass() / InitInstance()    ← Win32 윈도우 생성
  │
  ├── CMainApp::Create()
  │   ├── CGameInstance::Get_Instance()     ← 엔진 싱글톤 획득
  │   ├── CRenderInstance::Get_Instance()   ← 렌더 싱글톤 획득
  │   │
  │   └── CMainApp::Initialize()
  │       ├── CGameInstance::Initialize_Engine()
  │       │   ├── CGraphic_Device::Create()           ← D3D11 디바이스/스왑체인
  │       │   ├── CInput_Device 초기화                 ← DirectInput
  │       │   ├── CLevel_Manager::Create()
  │       │   ├── CTimer_Manager::Create()
  │       │   ├── CObject_Manager::Create(iNumLevels)
  │       │   ├── CComponent_Manager::Create(iNumLevels)
  │       │   ├── CCollider_Manager::Create()
  │       │   ├── CPipeLine::Create()
  │       │   ├── CFile_Manager::Create()
  │       │   ├── CFrustum::Create()
  │       │   ├── CFont_Manager::Create()
  │       │   └── CSound_Manager::Create()
  │       │
  │       ├── CRenderInstance::Initialize_Engine()
  │       │   ├── CRenderer::Create()       ← 렌더 파이프라인 + 렌더 타겟 초기화
  │       │   ├── CTarget_Manager::Create() ← MRT 설정
  │       │   ├── CLight_Manager::Create()
  │       │   └── CPicking::Create()
  │       │
  │       ├── Ready_Prototype_Component_ForStatic()   ← 정적 컴포넌트 프로토타입 등록
  │       ├── Ready_Fonts()                            ← 폰트 로드
  │       ├── Create_IMGUI_Manager()                   ← ImGui 에디터 초기화
  │       └── Open_Level(LEVEL_LOGO)                   ← 첫 레벨로 이동
  │
  ├── Add_Timer("Timer_Default") / Add_Timer("Timer_60")
  │
  └── 메시지 루프 진입
```

### 4.2 게임 루프

```
메시지 루프 (PeekMessage)
  │
  ├── Compute_TimeDelta("Timer_Default")  ← 매 프레임
  ├── Compute_TimeDelta("Timer_60")       ← 60fps 프레임 리미터
  │
  ├── CMainApp::Update(fTimeDelta)
  │   └── CGameInstance::Update_Engine(fTimeDelta)
  │       ├── CInput_Device 갱신
  │       ├── CKey_Manager 갱신
  │       ├── CObject_Manager::Player_Update()   ← 플레이어 입력 처리
  │       ├── CObject_Manager::Camera_Update()   ← 카메라 갱신
  │       ├── CObject_Manager::Update()          ← 전체 오브젝트 갱신
  │       ├── CObject_Manager::Late_Update()     ← 후처리 갱신
  │       ├── CLevel_Manager::Update()           ← 레벨 갱신
  │       ├── CCollider_Manager 충돌 검사
  │       └── CObject_Manager::Destory_Update()  ← 예약 삭제 처리
  │
  ├── CMainApp::Fixed_Update(fTimeDelta)  ← 고정 간격 업데이트
  │
  └── CMainApp::Render(fTimeDelta)
      ├── Clear_BackBuffer_View() / Clear_DepthStencil_View()
      ├── CRenderInstance::Render_Engine(fTimeDelta)
      │   └── CRenderer::Draw(fTimeDelta)
      │       ├── Render_Priority()          ← 배경 등 우선 렌더
      │       ├── Render_ShadowObj()         ← 섀도우 맵 생성
      │       ├── Render_StageDeferred()     ← 스테이지 G-버퍼
      │       ├── Render_Map()               ← 맵 렌더
      │       ├── Render_NonBlend()          ← 불투명 오브젝트
      │       ├── Render_Player()            ← 플레이어 G-버퍼
      │       ├── Render_PlayerDeferred()    ← 플레이어 디퍼드 라이팅
      │       ├── Render_Lights()            ← 라이트 패스
      │       ├── Render_Deferred()          ← 디퍼드 합성
      │       ├── Render_NonLight()          ← 라이트 미적용 오브젝트
      │       ├── Render_Glow()              ← 글로우 이펙트
      │       ├── Render_Blend()             ← 알파 블렌딩
      │       ├── Render_UI()               ← UI 렌더
      │       ├── Render_Distortion()        ← 디스토션 후처리
      │       └── Render_Debug()             ← 디버그 렌더 타겟 표시
      │
      ├── CLevel_Manager::Render()
      ├── CImgui_Manager 렌더 (에디터)
      └── Present()                           ← 프레젠트

```

### 4.3 레벨 전환 흐름

```
LEVEL_LOGO → LEVEL_LOADING → LEVEL_LOBBY → LEVEL_LOADING → LEVEL_CHARACTER
  → LEVEL_LOADING → LEVEL_VS → LEVEL_LOADING → LEVEL_GAMEPLAY
```

`CLevel_Loading`은 `CLoader`를 통해 다음 레벨 리소스를 스레드풀로 비동기 로드한다.

### 4.4 종료

```
CMainApp::Free()
  ├── CImgui_Manager::Free()
  ├── CRenderInstance::Release_Engine()   ← 렌더러 해제
  ├── CGameInstance::Release_Engine()     ← 엔진 해제 (역순 파괴)
  └── CRenderInstance::Destroy_Instance() / CGameInstance::Destroy_Instance()
```

---

## 5. 주요 시스템

### 5.1 렌더링 시스템
- **디퍼드 + 포워드 하이브리드** 렌더링
- 26개 렌더 그룹 (RG_PRIORITY ~ RG_NODE)
- MRT 기반 G-Buffer → 라이트 패스 → 합성
- 글로우, 디스토션, 블랙아웃/화이트아웃 후처리
- 섀도우 맵 (별도 DSV)
- 로비 전용 렌더러 분리 (CLobby_Renderer)

### 5.2 오브젝트/컴포넌트 시스템
- **프로토타입 패턴**: `Clone()`으로 오브젝트/컴포넌트 복제 생성
- **레이어 기반 관리**: `CObject_Manager` → `CLayer` → `CGameObject` 리스트
- **컴포넌트 맵**: `map<wstring, CComponent*>` (태그 기반 조회)
- **갱신 순서**: `Player_Update → Camera_Update → Update → Late_Update → Render`

### 5.3 전투 시스템
- **커맨드 입력**: 격투 게임 스타일 (236+공격, 214+공격 등) 입력 버퍼 파싱
- **공격 판정**: `CAttackObject` 계열 — 히트 모션, 공격 등급, 넉백, 스턴
- **중력/물리**: 캐릭터별 중력, 점프, 임펄스, 체이스 시스템
- **QTE**: 연타/동시입력/방향 입력 QTE 서브시스템
- **가드/반사**: 가드 타입 비교, 리플렉트 공격
- **HP/콤보**: HP 관리, 콤보 카운트, 공격 스텝, 기 게이지
- **스파킹/태그**: 스파킹 버스트, 태그 인/아웃

### 5.4 애니메이션 시스템
- Assimp → 바이너리 변환 (오프라인 도구)
- `CModel` → `CAnimation` → `CChannel` 계층
- 키프레임 보간, 블렌딩 (크로스 페이드)
- `CFrameEvent_Manager`: 특정 프레임에 이펙트/사운드/공격 판정 트리거
- 사운드 이벤트 통합 (`CAnimation::SoundEvent`)

### 5.5 이펙트/파티클 시스템
- `CEffect_Layer` → `CEffect` 계층 (레이어별 키프레임)
- 5가지 이펙트 타입: NoneLight, Blend, ZNone, Overlap, Animation
- 파티클 매니저로 집중 타격/포커스/확산 파티클
- 디스토션 이펙트 (화면 왜곡)
- 글로우 이펙트 (5종: PRI, STAR, MAIN, UI, UI_MULTY)

### 5.6 입력 시스템
- `CInput_Device`: DirectInput 8 (키보드/마우스 raw 상태)
- `CKey_Manager`: Down/Up/Pressing 논리 상태 추적
- `CCharacter::inputBuffer`: 격투 게임 입력 버퍼 (30프레임)

### 5.7 사운드 시스템
- FMOD 기반
- 키 기반/그룹 기반 사운드 재생
- 카테고리별 볼륨 제어
- 애니메이션 연동 사운드 이벤트

### 5.8 UI 시스템
- `CUIObject` 추상 베이스 (직교 투영 렌더)
- `CUI_Manager`: 플레이어 슬롯/상태 정보 브릿지
- 이동 애니메이션 지원 (`MoveAnimUI`, `Animation`)
- 해상도 오프셋 처리

### 5.9 씬/레벨 관리
- `CLevel` → `CLevel_Manager`로 전환
- `CLevel_Loading` + `CLoader`로 비동기 로딩
- 레벨별 리소스 격리 (`Clear_LevelResources`)

---

## 6. Engine / Renderer / Client 경계

```
┌─────────────────────────────────────────────────┐
│  Client (EXE)                                    │
│  게임 로직, 캐릭터, UI, 레벨, 이펙트            │
│  CMainApp, CCharacter, CUIObject, CLevel_*, ... │
├─────────────────────────────────────────────────┤
│           ↕ CGameInstance (파사드)                │
│           ↕ CRenderInstance (파사드)              │
├──────────────────────┬──────────────────────────┤
│  Engine (DLL)        │  Renderer (DLL)           │
│  ENGINE_DLL export   │  RENDERER_DLL export      │
│                      │                           │
│  오브젝트/컴포넌트   │  렌더 파이프라인          │
│  레벨/레이어 관리    │  라이트/렌더 타겟         │
│  입력/타이머         │  후처리 (글로우/디스토션)  │
│  충돌/물리           │  피킹                     │
│  애니메이션/모델     │                           │
│  사운드/파일/폰트    │                           │
└──────────────────────┴──────────────────────────┘
```

- **Engine**: `ENGINE_DLL` 매크로로 export. 게임 로직과 무관한 코어 프레임워크
- **Renderer**: `RENDERER_DLL` 매크로로 export. 렌더링 전용 (Engine에 의존)
- **Client**: 두 DLL을 import하여 게임 콘텐츠 구현
- **파사드 패턴**: `CGameInstance`와 `CRenderInstance`가 각각 Engine/Renderer 내부 매니저들의 접근 창구
- Engine의 `CGameObject`는 `Renderer::CRenderInstance`를 직접 참조 (렌더 그룹 등록)
- `Renderer_Defines.h`의 구조체(`RENDER_OBJECT`, `GLOW_DESC`, `DISTORTION_DESC`)가 Engine/Client 양쪽에서 사용

---

## 7. 주요 설계 패턴 정리

| 패턴 | 적용 위치 |
|------|-----------|
| **싱글톤** | `CGameInstance`, `CRenderInstance` (DECLARE/IMPLEMENT_SINGLETON 매크로) |
| **프로토타입** | `CGameObject::Clone()`, `CComponent::Clone()` — 객체 복제 생성 |
| **파사드** | `CGameInstance`(Engine), `CRenderInstance`(Renderer) — 내부 매니저 통합 인터페이스 |
| **레퍼런스 카운팅** | `CBase::AddRef()/Release()` — 수동 메모리 관리 |
| **컴포넌트** | `CGameObject`가 `map<wstring, CComponent*>` 보유 |
| **옵저버** | `OnCollisionEnter/Stay/Exit` — 충돌 콜백 |
| **커맨드 패턴** | `CCharacter::CommandPattern` — 격투 게임 커맨드 입력 |
| **상태 기반** | 레벨 전환, 캐릭터 애니메이션 상태 (스턴/체이스/가드 등) |
