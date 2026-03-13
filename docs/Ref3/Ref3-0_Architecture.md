# 참고프로젝트3 — 아키텍처 분석 (Ref_Architecture3)

> **프로젝트명**: ProjectMud (DDProject)
> **DirectX 버전**: DirectX 9 (Fixed Pipeline + Shader/Effect)
> **IDE**: Visual Studio 2015 (v14)
> **빌드**: Debug/Release × Win32/x64
> **특이사항**: 멀티플레이어(서버/클라이언트), Deferred Rendering, NavMesh, 이펙트 시스템

---

## 1. 디렉토리 구조

```
참고프로젝트3/
├── DDProject/
│   ├── ProjectMud.sln              ← 솔루션 파일 (3개 프로젝트)
│   │
│   ├── Client/                     ← 게임 클라이언트 (EXE)
│   │   ├── Default/                  WinMain 진입점, .vcxproj
│   │   ├── Codes/                    게임 오브젝트/씬/UI 구현 (~150개 .h/.cpp)
│   │   └── Headers/                  클라이언트 전용 정의 (Enum, Struct, Macro 등)
│   │
│   ├── Engine/                     ← 엔진 (DLL 2개)
│   │   ├── Headers/                  엔진 공용 헤더 (Defines, Enum, Typedef, Base 등)
│   │   ├── System/                   시스템 레이어 (System.vcxproj)
│   │   │   ├── Codes/                GraphicDev, InputDev, SoundMgr, Timer, Frame
│   │   │   └── Default/              프로젝트 파일
│   │   └── Utility/                  유틸리티 레이어 (Utility.vcxproj)
│   │       ├── Codes/                GameObject, Component, Scene, Renderer 등 (~50개)
│   │       └── Default/              프로젝트 파일
│   │
│   └── ThirdParty/                 ← 서드파티 라이브러리
│       ├── Headers/
│       │   ├── DirectX_SDK/          DX9/DX11 SDK 헤더
│       │   ├── TinyXml2/            XML 파서
│       │   ├── Vld/                 Visual Leak Detector
│       │   ├── directshow/          DirectShow (동영상 재생)
│       │   └── fmod/                FMOD (사운드)
│       └── Librarys/
│           ├── Vld/, directshow/, fmod/   각 라이브러리 .lib
│
└── Sever/
    └── Server.exe                  ← 서버 실행 파일 (바이너리만 존재)
```

### 폴더 역할 요약

| 폴더 | 역할 |
|------|------|
| `Client/Default` | WinMain 진입점, 윈도우 생성, 메시지 루프 |
| `Client/Codes` | 게임 로직 전체 — 플레이어, 몬스터, 타워, 씬, UI, 이펙트, 카메라, 네트워크 |
| `Client/Headers` | 클라이언트 전용 Enum(씬/플레이어/몬스터 상태), Struct(유닛/웨이브 정보) |
| `Engine/Headers` | 엔진 공용 — CBase, 타입 별칭, 매크로, 구조체, enum |
| `Engine/System` | 하드웨어 추상화 — D3D 디바이스, DirectInput, FMOD 사운드, 타이머/프레임 |
| `Engine/Utility` | 프레임워크 코어 — GameObject/Component/Scene 패턴, 렌더러, 메시, 셰이더 등 |

---

## 2. 빌드 구조 (솔루션 프로젝트 분리)

```
ProjectMud.sln
├── 00.Client/
│   └── Client.vcxproj              → EXE (게임 실행 파일)
│
└── 01.Engine/
    ├── 00.System/
    │   └── System.vcxproj           → DLL (GraphicDev, InputDev, SoundMgr, Timer, Frame)
    ├── 01.Utility/
    │   └── Utility.vcxproj          → DLL (GameObject, Component, Scene, Renderer 등)
    └── 99.Headers/
        └── 엔진 공용 헤더 (빌드 대상 아님, 참조용)
```

**Engine/Client 분리 기준**:
- **System**: 하드웨어 직접 접근 (D3D9 디바이스, DirectInput, FMOD) — 가장 저수준
- **Utility**: 게임 프레임워크 추상화 (씬, 오브젝트, 컴포넌트, 렌더러) — System 위에 구축
- **Client**: 구체적 게임 로직 (플레이어, 몬스터, 타워, UI) — 엔진 API 소비자

Engine은 `ENGINE_DLL` (_declspec(dllexport/import)) 매크로로 DLL 익스포트.

---

## 3. 핵심 클래스 목록

### 3-1. 엔진 기반 (Engine/Headers)

| 클래스 | 설명 |
|--------|------|
| `CBase` | **모든 클래스의 루트** — 레퍼런스 카운팅 (`Add_Ref`/`Release`), 순수 가상 `Free()` |

### 3-2. Engine/System 레이어

| 클래스 | 패턴 | 설명 |
|--------|------|------|
| `CGraphicDev` | 싱글톤 | D3D9 SDK/Device 생성, 윈도우/풀스크린 모드 |
| `CInputDev` | 싱글톤 | DirectInput8 — 키보드/마우스 입력 (OnceKeyDown, StayKeyDown) |
| `CSoundMgr` | 싱글톤 | FMOD 기반 사운드 — 채널별 재생, 3D 사운드, BGM |
| `CTimer` | - | QueryPerformanceCounter 기반 델타 타임 |
| `CTimer_Manager` | 싱글톤 | 여러 Timer 관리 (Timer_Default, Timer_60 등) |
| `CFrame` | - | FPS 제한기 |
| `CFrame_Manager` | 싱글톤 | Frame 인스턴스 관리 |

### 3-3. Engine/Utility 레이어 — 프레임워크 코어

| 클래스 | 패턴 | 설명 |
|--------|------|------|
| `CComponent` | 프로토타입 | **컴포넌트 베이스** — COM_STATIC/COM_DYNAMIC, Clone() 순수 가상 |
| `CResources` | 프로토타입 | 컴포넌트 하위 — 리소스 베이스 (그래픽 디바이스 보유) |
| `CGameObject` | - | **엔티티 베이스** — 컴포넌트 맵(STATIC/DYNAMIC), Transform 보유 |
| `CScene` | - | **씬 베이스** — Ready/Update/Render, ComponentMgr/ObjectMgr 참조 |
| `CLayer` | - | **오브젝트 컨테이너** — `list<CGameObject*>` 보유 |
| `CManagement` | 싱글톤 | **씬 매니저** — 현재 씬 관리, 게임 루프 진행 (Update/Render) |
| `CRenderer` | 컴포넌트 | **렌더 큐** — 9단계 렌더 그룹, Deferred/Glow/Bloom/Edge/Blur 후처리 |
| `CComponent_Manager` | 싱글톤 | **프로토타입 저장소** — 씬별 컴포넌트 원본 등록/Clone 제공 |
| `CObject_Manager` | 싱글톤 | **오브젝트 저장소** — 씬별 레이어 관리, Update 순회 |
| `CTransform` | 컴포넌트 | 위치/회전/스케일, 월드 행렬, 이동/회전 함수 |
| `CShader` | 컴포넌트 | D3DXEffect 래퍼 — .fx 파일 로드, Clone 지원 |
| `CTexture` | 리소스 | 일반/큐브 텍스처 로드, 셰이더 세팅 |
| `CVIBuffer` | 리소스 | 버텍스/인덱스 버퍼 베이스 |
| `CMesh` | 리소스 | 메시 베이스 (Static/Dynamic/Navigation) |
| `CDynamicMesh` | 리소스 | X파일 스키닝 메시 — HierarchyLoader, AnimationCtrl(상/하체 분리) |
| `CStaticMesh` | 리소스 | X파일 정적 메시 |
| `CCamera` | GameObject | 카메라 베이스 — View/Proj 행렬, Flow 전환 |
| `CLight_Manager` | 싱글톤 | 라이트 리스트 관리, 셰이더에 라이트 전달 |
| `CTarget_Manager` | 싱글톤 | **MRT 관리** — 렌더 타겟 생성/바인딩 (Deferred Rendering 핵심) |
| `CNavMgr` | 싱글톤 | NavMesh 관리 — 셀 기반 이동/충돌/슬라이드, 2층 지원 |
| `CEffectMgr` | 싱글톤 | 이펙트 풀 관리 — 태그별 이펙트 등록/생성/갱신 |
| `CEffect` | GameObject | 이펙트 베이스 — 빌보드, FadeIn/Out, 위치 추적 |
| `CNetwork` | - | WinSock2 네트워크 베이스 — Send/Recv NETDATA |
| `CDataManager` | 싱글톤 | 전역 데이터 허브 — View/Proj 행렬, 맵 오브젝트명, 웨이브 정보 등 |
| `CSubject` | - | 옵저버 패턴 Subject 베이스 |
| `CObserver` | - | 옵저버 패턴 Observer 베이스 |
| `CInfoSubject` | 싱글톤 | 메시지 기반 데이터 공유 (옵저버 + 데이터 리스트) |
| `CMouse_Manager` | 싱글톤 | 마우스 위치/상대 이동 계산, 커서 관리 |
| `CTextureManager` | 싱글톤 | 런타임 텍스처 캐시 (셰이더용 텍스처 등) |
| `CTextureRenderer` | - | DirectShow 비디오 → D3D 텍스처 렌더링 (동영상 재생) |
| `CCollisionSphere` | 컴포넌트 | 구체 충돌 — 충돌 반경/위치 기반 판정 |

### 3-4. Client 레이어 — 주요 클래스

| 클래스 | 부모 | 설명 |
|--------|------|------|
| `CMainApp` | CBase | **앱 최상위** — 초기화, 게임 루프 진행 |
| `CDynamicObject` | CGameObject | 동적 오브젝트 베이스 — Renderer+Shader+DynamicMesh, 애니 정보 |
| `CStaticObject` | CGameObject | 정적 오브젝트 베이스 — Renderer+Shader+StaticMesh |
| `CPlayer` | CDynamicObject | 플레이어 베이스 — 입력, NavMesh 이동, 타워 건설, 네트워크 |
| `CPlayer_Mage` | CPlayer | 마법사 캐릭터 |
| `CPlayer_Monk` | CPlayer | 수도승 캐릭터 |
| `CPlayer_Huntress` | CPlayer | 여사냥꾼 캐릭터 |
| `CMonster_Goblin/DarkElf/Orc/Kobold/Demon` | CDynamicObject | 몬스터 종류별 구현 |
| `CTower` | CDynamicObject | 타워 베이스 — 타겟 탐지, 공격, 충돌 |
| `CFireTower/MissileTower/LightningTower/StrikerTower` | CTower | 타워 종류별 구현 |
| `CScene_Logo/Menu/Lobby/Stage/Stage2` | CScene | 씬별 구현 |
| `CCamera_Manager` | CBase (싱글톤) | 카메라 전환 관리 (Static/Dynamic/Action 등 10종) |
| `CCollisionManager` | CBase (싱글톤) | 충돌 관리 — 구체 충돌 리스트, 마우스 피킹, NavMesh 피킹 |
| `CClientNetwork` | CNetwork | 클라이언트측 네트워크 구현 |

---

## 4. 클래스 상속 계층도

```
CBase (레퍼런스 카운팅 루트)
│
├── CComponent (컴포넌트 베이스 — Clone 패턴)
│   ├── CTransform (위치/회전/스케일)
│   ├── CShader (D3DXEffect 래퍼)
│   ├── CRenderer (렌더 큐 + 후처리 파이프라인)
│   ├── CCollisionSphere (구체 충돌)
│   ├── CCubeColor (컬러 큐브)
│   └── CResources (리소스 베이스)
│       ├── CTexture (일반/큐브 텍스처)
│       ├── CVIBuffer (VB/IB 베이스)
│       │   ├── CRect_Texture (사각형 텍스처 버퍼)
│       │   ├── CTerrain_Texture (지형 버퍼)
│       │   ├── CCube_Texture (큐브 버퍼)
│       │   ├── CView_Texture (뷰포트 텍스처)
│       │   ├── CTrail_Texture (트레일 버퍼)
│       │   └── CParticle_Texture (파티클 버퍼)
│       └── CMesh (메시 베이스)
│           ├── CStaticMesh (정적 X파일 메시)
│           └── CDynamicMesh (스키닝 X파일 메시)
│
├── CGameObject (엔티티 베이스)
│   ├── CCamera (카메라 베이스)
│   │   ├── CDynamicCamera (자유 카메라)
│   │   ├── CStaticCamera (고정 카메라)
│   │   ├── CActionCamera (연출용)
│   │   ├── CRecordCamera (녹화용)
│   │   ├── CTowerCamera (타워 건설용)
│   │   ├── CSummonCamera (소환 연출용)
│   │   ├── CStandCamera / CFloatCamera
│   │   ├── CGoblin_Camera / CVictory_Camera
│   │   └── ...
│   │
│   ├── CEffect (이펙트 베이스) — abstract
│   │   ├── CEffect_Single (단일 텍스처)
│   │   ├── CEffect_Frame (프레임 애니메이션)
│   │   ├── CEffect_Particle (포인트 파티클)
│   │   ├── CEffect_RectParticle (사각 파티클)
│   │   ├── CEffect_Mesh (메시 이펙트)
│   │   ├── CEffect_Trail (트레일)
│   │   ├── CEffect_Decal (데칼)
│   │   ├── CEffect_2D (2D 이펙트)
│   │   └── CEffect_Mesh_* (특수 메시 이펙트 다수)
│   │
│   ├── [Client] CDynamicObject (동적 오브젝트 베이스)
│   │   ├── CPlayer (플레이어 베이스)
│   │   │   ├── CPlayer_Mage
│   │   │   ├── CPlayer_Monk
│   │   │   ├── CPlayer_Huntress
│   │   │   ├── CPlayer_Goblin (라스트맨 모드)
│   │   │   └── CPlayer_Left / CPlayer_CrossBow / CPlayer_Spear / CPlayer_Staff
│   │   ├── CMonster_Goblin / CMonster_DarkElfArcher / CMonster_OrcBruiser
│   │   ├── CMonster_Kobold / CMonster_Demon
│   │   ├── CTower (타워 베이스)
│   │   │   ├── CFireTower / CMissileTower / CLightningTower / CStrikerTower
│   │   │   └── CBlockade / CTrapMine
│   │   ├── CWeapon / CCrossBowArrow / CMonster_Arrow
│   │   └── CLeftDoor / CRightDoor
│   │
│   ├── [Client] CStaticObject (정적 오브젝트 베이스)
│   │   ├── CTerrain / CSky
│   │   ├── CCrystal / CCrystalLight
│   │   ├── CAccessory1 / CAccessory2
│   │   ├── CTresureBox / CPresent
│   │   └── CEasterEgg / CFireTorch / ...
│   │
│   └── [Client] UI/Font 계열 (CGameObject 직접 상속)
│       ├── COrthographicUI → CStaticUI / CPanel_* / CText_* / CFont_*
│       ├── CPerspectiveUI → CSkillIcon / CUI_Skill / CUI_Check
│       ├── CMiniMap / CMapTexture
│       ├── CMousePointer / CLowHp
│       └── ...
│
├── CScene (씬 베이스)
│   ├── CScene_Logo → CScene_Menu → CScene_Lobby
│   ├── CScene_Stage (메인 스테이지)
│   ├── CScene_Stage2 (라스트맨 모드)
│   ├── CScene_Transition (전환 씬)
│   └── CTestScene_* (개발자별 테스트 씬)
│
├── CLayer (오브젝트 리스트 컨테이너)
│
├── [싱글톤 매니저들]
│   ├── CManagement (씬 관리)
│   ├── CComponent_Manager (프로토타입 저장소)
│   ├── CObject_Manager (오브젝트/레이어 관리)
│   ├── CGraphicDev / CInputDev / CSoundMgr
│   ├── CTimer_Manager / CFrame_Manager
│   ├── CLight_Manager / CTarget_Manager
│   ├── CNavMgr / CEffectMgr / CDataManager
│   ├── CMouse_Manager / CTextureManager
│   ├── CInfoSubject (옵저버 데이터 허브)
│   └── [Client] CCamera_Manager / CCollisionManager
│
├── CSubject (옵저버 패턴 Subject)
│   └── CInfoSubject
│
├── CObserver (옵저버 패턴 Observer)
│   └── [Client] CPlayerObserver / CSceneObserver / CWaveObserver / CLastManObserver
│
└── CNetwork (네트워크 베이스)
    └── [Client] CClientNetwork / CClientNetwork_Stage2
```

---

## 5. 프로그램 실행 흐름

### 5-1. 진입점 → 초기화

```
wWinMain()                              [Client/Default/Client.cpp]
  ├── _CrtSetDbgFlag()                  메모리 누수 감지 설정
  ├── LoadString / MyRegisterClass()    윈도우 클래스 등록
  ├── InitInstance()                    윈도우 생성 (WINCX × WINCY), g_hWnd 설정
  │
  ├── CMainApp::Create()               ← 핵심 초기화
  │   └── Ready_MainApp()
  │       ├── Ready_DefaultSetting()    GraphicDev 초기화 (D3D9 디바이스)
  │       │                             InputDev 초기화 (DirectInput)
  │       │                             SoundMgr 초기화 (FMOD)
  │       │                             Component_Manager 예약 (씬 개수만큼)
  │       │                             Object_Manager 예약
  │       │                             DataManager 초기화 (맵/웨이브 데이터 로드)
  │       │                             Renderer 생성 (렌더 타겟 + 셰이더)
  │       │                             Management 초기화
  │       │
  │       └── Ready_StartScene()        최초 씬 (Scene_Logo) 설정
  │           └── Management::SetUp_CurrentScene(CScene_Logo::Create())
  │
  ├── CTimer_Manager::GetInstance()     타이머 매니저 생성
  ├── CFrame_Manager::GetInstance()     프레임 매니저 생성
  ├── Timer::Create() → "Timer_Default" 등록
  └── (선택) Frame::Create() → "Frame_60" 등록 (FPS 제한 시)
```

### 5-2. 게임 루프

```
while(true)
{
  PeekMessage → 윈도우 메시지 처리
  else
  {
    Timer_Manager::Compute_TimeDelta("Timer_Default")
    fTimeDelta = Timer_Manager::Get_TimeDelta("Timer_Default")

    [FPS 계산 → SetWindowText]

    ── pMainApp->Update_MainApp(fTimeDelta) ──
    │   ├── InputDev::Set_InputState()       키보드/마우스 상태 갱신
    │   ├── Management::Update_Management()
    │   │   ├── Scene::Update_Scene()
    │   │   │   └── Object_Manager::Update_ObjMgr()
    │   │   │       └── 각 Layer::Update_Layer()
    │   │   │           └── 각 GameObject::Update_GameObject()
    │   │   │               (FirstUpdate → Update 순서)
    │   │   └── (첫 렌더 후 FirstRender_Finish 호출)
    │   └── return exitCode
    │
    ── pMainApp->Render_MainApp() ──
        └── Management::Render_Management()
            ├── Scene::Render_Scene()       (씬별 렌더 로직)
            └── Renderer::Render_GameObject()
                ├── Render_Deferred()       G-Buffer 생성
                │   ├── Begin_MRT("MRT_Deferred")
                │   ├── Render_StaticMesh_NoneAlpha()
                │   ├── Render_DynamicMesh_NoneAlpha()
                │   └── End_MRT
                ├── Render_Light()          라이트 패스
                ├── Render_Scene()          최종 합성
                ├── Render_Glow()           글로우 후처리
                ├── Render_Priority()       우선 렌더 (스카이박스 등)
                ├── Render_Default()        기본 렌더
                ├── Render_Alpha()          알파 블렌딩 (Z정렬)
                ├── Render_Effect()         이펙트
                ├── Render_UI()             UI
                └── Clear_RenderList()      렌더 리스트 초기화
  }
}
```

### 5-3. 종료

```
Safe_Release(pTimer_Manager)
Safe_Release(pFrame_Manager)
pMainApp->Release()
  └── Free()
      ├── 각 싱글톤 DestroyInstance() (역순)
      └── Safe_Release(GraphicDev)
```

---

## 6. 주요 시스템

### 6-1. 렌더링 시스템
- **Deferred Rendering**: MRT(Multiple Render Targets)로 G-Buffer 생성 → 라이트 패스 → 합성
- **렌더 큐 (9단계)**:
  `PRIORITY → DYNAMICMESH_NONEALPHA → STATICMESH_NONEALPHA → STATICMESH_CARTOON → DEFAULT → ALPHA → EFFECT → UI → SKYBOXMESH`
- **후처리**: Glow, Edge(외곽선), Bloom, Blur, DownSampling
- **셰이더**: D3DXEffect 기반, Renderer가 셰이더 맵 관리

### 6-2. 입력 시스템
- **DirectInput8** 기반
- OnceKeyDown(누른 순간), StayKeyDown(누르는 동안), OnceMouseClick, StayMouseClick
- 키 비활성화 기능 (m_bKeyDisabled) — 컷씬 등에서 입력 차단

### 6-3. 씬 관리
- **씬 흐름**: Logo → Menu → Lobby → Stage (또는 Stage2)
- Scene::Ready_Scene()에서 레이어/오브젝트 초기화, Light/NavMesh/MapObject 로드
- **CManagement**가 현재 씬 보유, SetUp_CurrentScene()으로 전환

### 6-4. 오브젝트/컴포넌트 시스템
- **프로토타입 패턴**: Component_Manager에 원본 등록 → Clone()으로 복제
- **컴포넌트 분류**: COM_STATIC(변경 불가 — Mesh, Texture), COM_DYNAMIC(변경 가능 — Transform)
- **오브젝트 계층**: Scene → Object_Manager → Layer(이름 기반) → GameObject 리스트

### 6-5. 애니메이션 시스템
- **D3DX Animation**: HierarchyLoader로 X파일 로드
- **상/하체 분리**: AnimationCtrl이 ANICTRL_HIGHER/ANICTRL_LOWER 2개
- **AnimationInfo 구조체**: 인덱스, 속도(앞/뒤), 가중치(앞/뒤), 종료 카운트 등

### 6-6. 충돌 시스템
- **구체 충돌**: CCollisionSphere — CollisionManager에 태그별 등록, 리스트 순회
- **NavMesh**: CNavMgr — 셀 기반 이동, 슬라이드 벡터, 낙하 처리, 2층 지원
- **마우스 피킹**: CollisionManager — 메시/NavMesh/지형 피킹

### 6-7. 사운드 시스템
- **FMOD** 기반, 채널별 관리 (26개 채널 — 캐릭터, 몬스터, 타워, BGM 등)
- 3D 사운드 지원 (카메라 위치 기반 거리 감쇠)
- 랜덤 사운드 재생 기능

### 6-8. 네트워크 시스템
- **WinSock2** 기반 클라이언트-서버 구조
- **NETDATA 구조체**: 메시지 타입, 월드 행렬, 애니메이션 상태, 피킹 위치 등
- 메시지 타입: 플레이어 생성/삭제/업데이트, 채팅, 타워 건설, 전투 준비 등

### 6-9. 이펙트 시스템
- **CEffectMgr**: 태그별 이펙트 프리셋 등록 → Clone으로 인스턴스 생성
- 이펙트 종류: Single, Frame, Particle, RectParticle, Mesh, Trail, Decal, 2D
- 빌보드(Y축/전축), FadeIn/Out, 위치 추적, 타이머 기반 소멸

### 6-10. 카메라 시스템
- **CCamera_Manager**: 10종 카메라 전환 관리
- 카메라 종류: Static, Dynamic, Recording, Action, Tower, Summon, Stand, Float, Goblin, Victory
- 부드러운 전환 (Flow_Camera), 셰이크 기능

### 6-11. 옵저버 패턴
- **CInfoSubject** (싱글톤): 메시지 ID → 데이터 리스트 맵
- **옵저버들**: PlayerObserver, SceneObserver, WaveObserver, LastManObserver
- 메시지: 플레이어 정보, 씬 전환, 웨이브 변경, 보스 등장 등

### 6-12. 미디어 시스템
- **DirectShow** 기반 동영상 재생
- CTextureRenderer: 비디오 프레임 → D3D 텍스처로 렌더링
- CMediaMgr/CMediaObj: 미디어 재생 관리

---

## 7. Engine/Client 경계 정리

| 기준 | Engine (System + Utility) | Client |
|------|--------------------------|--------|
| **역할** | 재사용 가능한 프레임워크 | 게임 고유 로직 |
| **빌드** | DLL (익스포트) | EXE (임포트) |
| **추상화** | CGameObject, CScene, CComponent 등 베이스 | Player, Monster, Tower 등 구체 클래스 |
| **패턴** | 싱글톤 매니저, 프로토타입, 옵저버 기반 | 상태 패턴(FSM), 팩토리 |
| **데이터** | 일반 구조체 (VTXTEX, INDEX32, NAVMESH 등) | 게임 구조체 (PLAYER_INFO, WAVE_INFO 등) |

---

## 8. 참고프로젝트2와의 주요 차이점

| 항목 | 참고프로젝트2 (DX11) | 참고프로젝트3 (DX9) |
|------|---------------------|---------------------|
| DirectX | DX11 | DX9 |
| 렌더링 | 순수 Deferred | Deferred + 다양한 후처리 (Glow/Bloom/Edge) |
| 네트워크 | 없음 | WinSock2 멀티플레이어 |
| 동영상 | 없음 | DirectShow 비디오 재생 |
| 애니메이션 | Assimp 기반 | D3DX Animation (상하체 분리) |
| 이펙트 | 단순 | 풀 이펙트 시스템 (7+종 이펙트 타입) |
| 타워 디펜스 | 없음 | 타워 건설/웨이브/보스 시스템 |
| 옵저버 패턴 | 없음 | InfoSubject 기반 메시지 시스템 |
| 사운드 | 없음 | FMOD 26채널 3D 사운드 |
